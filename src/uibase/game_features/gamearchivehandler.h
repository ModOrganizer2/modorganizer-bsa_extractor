#ifndef UIBASE_GAMEFEATURES_GAMEARCHIVEHANDLER_H
#define UIBASE_GAMEFEATURES_GAMEARCHIVEHANDLER_H

// Temporary extractor-local stopgap until MO2 uibase provides the canonical
// GameArchiveHandler feature type. While this macro exists, extractor-side delegated
// feature lookup is intentionally disabled.
#define BSA_EXTRACTOR_LOCAL_GAMEARCHIVEHANDLER_SHIM 1

#include <functional>

#include <QString>
#include <QStringList>
#include <QtGlobal>

#include <uibase/game_features/game_feature.h>

namespace MOBase
{

class GameArchiveHandler : public details::GameFeatureCRTP<GameArchiveHandler>
{
public:
  using ProgressCallback = std::function<bool(int, const QString&)>;

public:
  virtual ~GameArchiveHandler() = default;

  // Optional glob filters that help tools narrow archive scans.
  virtual QStringList supportedArchiveNameFilters() const { return {}; }

  // Returns true when this handler owns the archive path and can process it.
  virtual bool supportsArchive(const QString& archivePath) const = 0;

  virtual bool extractArchive(const QString& archivePath, const QString& outputDirectory,
                              const ProgressCallback& progress = {},
                              QString* errorMessage            = nullptr) const = 0;

  virtual bool canCreateArchive(const QString& archivePath) const
  {
    Q_UNUSED(archivePath);
    return false;
  }

  virtual bool createArchive(const QString& sourceDirectory, const QString& archivePath,
                             QString* errorMessage = nullptr) const
  {
    Q_UNUSED(sourceDirectory);
    Q_UNUSED(archivePath);
    Q_UNUSED(errorMessage);
    return false;
  }
};

}  // namespace MOBase

#endif  // UIBASE_GAMEFEATURES_GAMEARCHIVEHANDLER_H
