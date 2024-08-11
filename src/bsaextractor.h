#ifndef BSAEXTRACTOR_H
#define BSAEXTRACTOR_H

#include <QProgressDialog>

#include <uibase/iplugin.h>

class BsaExtractor : public QObject, public MOBase::IPlugin
{
  Q_OBJECT
  Q_INTERFACES(MOBase::IPlugin)
  Q_PLUGIN_METADATA(IID "org.tannin.BSAExtractor")

public:
  BsaExtractor();

  virtual bool init(MOBase::IOrganizer *moInfo) override;
  virtual QString name() const override;
  virtual QString localizedName() const override;
  virtual QList<MOBase::Setting> settings() const override;

private:
  void modInstalledHandler(MOBase::IModInterface *mod);
  bool extractProgress(QProgressDialog &progress, int percentage, std::string fileName);
private:
  MOBase::IOrganizer *m_Organizer;
};

#endif // BSAEXTRACTOR_H
