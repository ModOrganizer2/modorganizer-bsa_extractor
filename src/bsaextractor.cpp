#include "bsaextractor.h"

#include "iplugingame.h"
#include <versioninfo.h>
#include <imodinterface.h>
#include <questionboxmemory.h>
#include <bsaarchive.h>
#include <report.h>

#include <QDir>
#include <QFileInfoList>
#include <QCoreApplication>
#include <QMessageBox>
#include <QtPlugin>

#include <functional>

#include <boost/bind.hpp>

using namespace MOBase;
namespace bindph = std::placeholders;


BsaExtractor::BsaExtractor()
  : m_Organizer(nullptr)
{
}

bool BsaExtractor::init(MOBase::IOrganizer *moInfo)
{
  m_Organizer = moInfo;
  moInfo->onModInstalled(std::bind(&BsaExtractor::modInstalledHandler, this, bindph::_1));
  return true;
}

QString BsaExtractor::name() const
{
  return "BSA Extractor";
}

QString BsaExtractor::author() const
{
  return "Tannin";
}

QString BsaExtractor::description() const
{
  return tr("Offers a dialog during installation of a mod to unpack all its BSAs");
}

VersionInfo BsaExtractor::version() const
{
  return VersionInfo(1, 0, 0, VersionInfo::RELEASE_FINAL);
}

bool BsaExtractor::isActive() const
{
  return m_Organizer->pluginSetting(this->name(), "enabled").toBool();
}

QList<PluginSetting> BsaExtractor::settings() const
{
  return QList<PluginSetting>() << PluginSetting("enabled", "enable bsa extraction (you can still choose to not extract during installation)", false);
      ;// << PluginSetting("always", "always unpack all bsas without asking", false);
}


bool BsaExtractor::extractProgress(QProgressDialog &progress, int percentage, std::string fileName)
{
  progress.setLabelText(fileName.c_str());
  progress.setValue(percentage);
  qApp->processEvents();
  return !progress.wasCanceled();
}


void BsaExtractor::modInstalledHandler(const QString &modName/*, EFileCategory installerCategory*/)
{
  if (!isActive()) {
    return;
  }

  IModInterface *mod = m_Organizer->getMod(modName);
  if (QFileInfo(mod->absolutePath()) == QFileInfo(m_Organizer->managedGame()->dataDirectory().absolutePath())) {
    QMessageBox::information(nullptr, tr("invalid mod name"),
                             tr("BSA extraction doesn't work on mods that have the same name as a non-MO mod."
                                "Please remove the mod then reinstall with a different name."));
    return;
  }
  QDir dir(mod->absolutePath());

  QFileInfoList archives = dir.entryInfoList(QStringList("*.bsa"));
  if (archives.length() != 0 &&
      (QuestionBoxMemory::query(nullptr, "unpackBSA", tr("Extract BSA"),
                             tr("This mod contains at least one BSA. Do you want to unpack it?\n"
                                "(This removes the BSA after completion. If you don't know about BSAs, just select no)"),
                             QDialogButtonBox::Yes | QDialogButtonBox::No, QDialogButtonBox::No) == QDialogButtonBox::Yes)) {

    foreach (QFileInfo archiveInfo, archives) {
      BSA::Archive archive;
      BSA::EErrorCode result = archive.read(archiveInfo.absoluteFilePath().toLocal8Bit().constData(), true);
      if ((result != BSA::ERROR_NONE) && (result != BSA::ERROR_INVALIDHASHES)) {
        reportError(tr("failed to read %1: %2").arg(archiveInfo.fileName()).arg(result));
        return;
      }

      QProgressDialog progress(nullptr);
      progress.setMaximum(100);
      progress.setValue(0);
      progress.show();

      archive.extractAll(mod->absolutePath().toLocal8Bit().constData(),
                         boost::bind(&BsaExtractor::extractProgress, this, boost::ref(progress), _1, _2),
                         false);

      if (result == BSA::ERROR_INVALIDHASHES) {
        reportError(tr("This archive contains invalid hashes. Some files may be broken."));
      }

      archive.close();

      if (!QFile::remove(archiveInfo.absoluteFilePath())) {
        qCritical("failed to remove archive %s", archiveInfo.absoluteFilePath().toUtf8().constData());
      }
    }
  }
}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
Q_EXPORT_PLUGIN2(bsaextractor, BsaExtractor)
#endif
