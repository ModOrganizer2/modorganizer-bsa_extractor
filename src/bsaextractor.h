#ifndef BSAEXTRACTOR_H
#define BSAEXTRACTOR_H

#include <QProgressDialog>

#include <memory>
#include <string>

#include <QFileInfo>

#include <uibase/iplugin.h>
#include <uibase/game_features/gamearchivehandler.h>

class BsaExtractor : public QObject, public MOBase::IPlugin
{
  Q_OBJECT
  Q_INTERFACES(MOBase::IPlugin)
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
  Q_PLUGIN_METADATA(IID "org.tannin.BSAExtractor")
#endif

public:
  BsaExtractor();

  virtual bool init(MOBase::IOrganizer *moInfo) override;
  virtual QString name() const override;
  virtual QString localizedName() const override;
  virtual QString author() const override;
  virtual QString description() const override;
  virtual MOBase::VersionInfo version() const override;
  virtual QList<MOBase::PluginSetting> settings() const override;

private:
  void tryRegisterInstallHook();
  void modInstalledHandler(MOBase::IModInterface *mod);
  bool extractProgress(QProgressDialog &progress, int percentage, std::string fileName);
  bool extractWithBsaTk(MOBase::IModInterface* mod, const QFileInfo& archiveInfo);
  bool extractWithGameHandler(
      MOBase::IModInterface* mod, const QFileInfo& archiveInfo,
      const std::shared_ptr<const MOBase::GameArchiveHandler>& archiveHandler);
private:
  MOBase::IOrganizer *m_Organizer;
  bool m_InstallHookRegistered = false;
};

#endif // BSAEXTRACTOR_H
