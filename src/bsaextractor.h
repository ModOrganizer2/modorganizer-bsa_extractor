#ifndef BSAEXTRACTOR_H
#define BSAEXTRACTOR_H

#include <iplugin.h>
#include <QProgressDialog>

class BsaExtractor : public QObject, public MOBase::IPlugin
{
  Q_OBJECT
  Q_INTERFACES(MOBase::IPlugin)
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
  Q_PLUGIN_METADATA(IID "org.tannin.BSAExtractor" FILE "bsaextractor.json")
#endif

public:
  BsaExtractor();

  virtual bool init(MOBase::IOrganizer *moInfo);
  virtual QString name() const;
  virtual QString author() const;
  virtual QString description() const;
  virtual MOBase::VersionInfo version() const;
  virtual bool isActive() const;
  virtual QList<MOBase::PluginSetting> settings() const;

private:
  void modInstalledHandler(MOBase::IModInterface *mod);
  bool extractProgress(QProgressDialog &progress, int percentage, std::string fileName);
private:
  MOBase::IOrganizer *m_Organizer;
};

#endif // BSAEXTRACTOR_H
