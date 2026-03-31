#include "bsaextractor.h"
#include "archivehandlerdispatch.h"

#include <uibase/iplugingame.h>
#include <uibase/imodlist.h>
#include <uibase/versioninfo.h>
#include <uibase/imodinterface.h>
#include <uibase/questionboxmemory.h>
#include <uibase/report.h>

#include <bsatk/bsaarchive.h>

#include <QDir>
#include <QFileInfoList>
#include <QCoreApplication>
#include <QMessageBox>
#include <QTimer>
#include <QtPlugin>
#include <QtDebug>

#include <algorithm>
#include <functional>

using namespace MOBase;

BsaExtractor::BsaExtractor()
  : m_Organizer(nullptr)
{
}

bool BsaExtractor::init(MOBase::IOrganizer *moInfo)
{
  m_Organizer = moInfo;
  if (qEnvironmentVariableIntValue("BSA_EXTRACTOR_DISABLE_INSTALL_HOOK") == 1) {
    qWarning().noquote() << "[BSA Extractor] install hook disabled by"
                            " BSA_EXTRACTOR_DISABLE_INSTALL_HOOK=1";
    return true;
  }

  // Delay registration a bit to avoid early-startup initialization hazards.
  QTimer::singleShot(2000, this, [this]() { tryRegisterInstallHook(); });
  return true;
}

void BsaExtractor::tryRegisterInstallHook()
{
  if (m_InstallHookRegistered || m_Organizer == nullptr) {
    return;
  }

  auto* modList = m_Organizer->modList();
  if (modList == nullptr) {
    qWarning().noquote() << "[BSA Extractor] modList unavailable;"
                            " extraction hook not registered";
    return;
  }

  m_InstallHookRegistered = modList->onModInstalled([this](IModInterface* mod) {
    if (m_Organizer == nullptr || mod == nullptr) {
      return;
    }

    // Bounce to the Qt event loop and resolve by name to avoid stale pointers.
    const QString modName = mod->name();
    QMetaObject::invokeMethod(
        this,
        [this, modName]() {
          if (m_Organizer == nullptr) {
            return;
          }

          auto* currentModList = m_Organizer->modList();
          if (currentModList == nullptr) {
            return;
          }

          auto* currentMod = currentModList->getMod(modName);
          if (currentMod != nullptr) {
            modInstalledHandler(currentMod);
          }
        },
        Qt::QueuedConnection);
  });
  if (!m_InstallHookRegistered) {
    qWarning().noquote() << "[BSA Extractor] failed to register onModInstalled hook";
  }
}

QString BsaExtractor::name() const
{
  return "BSA Extractor";
}

QString BsaExtractor::localizedName() const
{
  return tr("BSA Extractor");
}


QString BsaExtractor::author() const
{
  return "Tannin & MO2 Team";
}

QString BsaExtractor::description() const
{
  return tr("Offers a dialog during installation of a mod to unpack all its BSAs");
}

VersionInfo BsaExtractor::version() const
{
  return VersionInfo(1, 5, 0, VersionInfo::RELEASE_FINAL);
}

QList<PluginSetting> BsaExtractor::settings() const
{
  return {
    PluginSetting("only_alternate_source", "only trigger bsa extraction for alternate game sources", true)
  };
}


bool BsaExtractor::extractProgress(QProgressDialog &progress, int percentage, std::string fileName)
{
  progress.setLabelText(fileName.c_str());
  progress.setValue(percentage);
  qApp->processEvents();
  return !progress.wasCanceled();
}

bool BsaExtractor::extractWithBsaTk(IModInterface* mod, const QFileInfo& archiveInfo)
{
  BSA::Archive archive;
  BSA::EErrorCode result = archive.read(archiveInfo.absoluteFilePath().toLocal8Bit().constData(),
                                        true);
  if ((result != BSA::ERROR_NONE) && (result != BSA::ERROR_INVALIDHASHES)) {
    reportError(tr("failed to read %1: %2").arg(archiveInfo.fileName()).arg(result));
    return false;
  }

  QProgressDialog progress(nullptr);
  progress.setMaximum(100);
  progress.setValue(0);
  progress.show();

  archive.extractAll(mod->absolutePath().toLocal8Bit().constData(),
                     [this, &progress](int value, std::string filename) {
                       return extractProgress(progress, value, filename);
                     },
                     false);

  if (result == BSA::ERROR_INVALIDHASHES) {
    reportError(tr("This archive contains invalid hashes. Some files may be broken."));
  }

  archive.close();
  return true;
}

bool BsaExtractor::extractWithGameHandler(
    IModInterface* mod, const QFileInfo& archiveInfo,
    const std::shared_ptr<const GameArchiveHandler>& archiveHandler)
{
  QProgressDialog progress(nullptr);
  progress.setLabelText(archiveInfo.fileName());
  progress.setMaximum(100);
  progress.setValue(0);
  progress.show();

  QString errorMessage;
  const bool extracted = archiveHandler->extractArchive(
      archiveInfo.absoluteFilePath(), mod->absolutePath(),
      [this, &progress, archiveInfo](qint64 current, qint64 total) {
        const int percent =
            (total > 0)
                ? static_cast<int>(std::clamp((current * 100) / total, qint64(0), qint64(100)))
                : 0;
        extractProgress(progress, percent, archiveInfo.fileName().toStdString());
      },
      &errorMessage);

  if (!extracted) {
    reportError(tr("failed to extract %1: %2").arg(archiveInfo.fileName(), errorMessage));
  }

  return extracted;
}


void BsaExtractor::modInstalledHandler(IModInterface *mod)
{
  if (m_Organizer == nullptr || mod == nullptr) {
    return;
  }

  auto* modList = m_Organizer->modList();
  auto* managedGame = m_Organizer->managedGame();
  if (modList == nullptr || managedGame == nullptr) {
    return;
  }

  if (m_Organizer->pluginSetting(name(), "only_alternate_source").toBool() &&
      !(modList->state(mod->name()) & IModList::STATE_ALTERNATE)) {
    return;
  }

  if (QFileInfo(mod->absolutePath()) == QFileInfo(managedGame->dataDirectory().absolutePath())) {
    QMessageBox::information(nullptr, tr("invalid mod name"),
                             tr("BSA extraction doesn't work on mods that have the same name as a non-MO mod."
                                "Please remove the mod then reinstall with a different name."));
    return;
  }
  QDir dir(mod->absolutePath());

  const auto archiveHandler = findGameArchiveHandler(m_Organizer);
  QFileInfoList archives    = findExtractableArchives(dir, archiveHandler);
  if (archives.length() != 0 &&
      (QuestionBoxMemory::query(nullptr, "unpackBSA", tr("Extract BSA"),
                             tr("This mod contains at least one BSA. Do you want to unpack it?\n"
                                "(If you don't know about BSAs, just select no)"),
                             QDialogButtonBox::Yes | QDialogButtonBox::No, QDialogButtonBox::No) == QDialogButtonBox::Yes)) {

    bool removeBSAs = (QuestionBoxMemory::query(nullptr, "removeUnpackedBSA", tr("Remove extracted archives"),
                       tr("Do you wish to remove BSAs after extraction completed?\n"),
                       QDialogButtonBox::Yes | QDialogButtonBox::No, QDialogButtonBox::No) == QDialogButtonBox::Yes);
    foreach (QFileInfo archiveInfo, archives) {
      const bool extracted = shouldDelegateArchive(archiveInfo.absoluteFilePath(), archiveHandler)
                                 ? extractWithGameHandler(mod, archiveInfo, archiveHandler)
                                 : extractWithBsaTk(mod, archiveInfo);
      if (!extracted) {
        return;
      }

      if (removeBSAs) {
        if (!QFile::remove(archiveInfo.absoluteFilePath())) {
          qCritical("failed to remove archive %s", archiveInfo.absoluteFilePath().toUtf8().constData());
        }
      }
    }
  }
}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
Q_EXPORT_PLUGIN2(bsaextractor, BsaExtractor)
#endif
