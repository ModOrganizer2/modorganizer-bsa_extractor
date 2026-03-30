#include "archivehandlerdispatch.h"

#include <QSet>

#include <uibase/game_features/igamefeatures.h>
#include <uibase/imoinfo.h>

namespace
{

QStringList builtInArchiveNameFilters()
{
  return {"*.bsa", "*.ba2"};
}

}  // namespace

std::shared_ptr<const MOBase::GameArchiveHandler>
findGameArchiveHandler(const MOBase::IOrganizer* organizer)
{
  if (organizer == nullptr || organizer->gameFeatures() == nullptr) {
    return {};
  }

#ifdef BSA_EXTRACTOR_LOCAL_GAMEARCHIVEHANDLER_SHIM
  // The extractor carries a local stopgap declaration of GameArchiveHandler so this
  // branch can be reviewed in isolation, but clean feature lookup requires the
  // canonical uibase declaration and IGameFeatures support. Until that lands, behave as
  // if no handler is registered instead of reaching into protected internals.
  return {};
#else
  return organizer->gameFeatures()->gameFeature<MOBase::GameArchiveHandler>();
#endif
}

QFileInfoList findExtractableArchives(
    const QDir& directory,
    const std::shared_ptr<const MOBase::GameArchiveHandler>& archiveHandler)
{
  QFileInfoList archives = directory.entryInfoList(
      builtInArchiveNameFilters(), QDir::Files | QDir::NoDotAndDotDot,
      QDir::Name | QDir::IgnoreCase);

  if (!archiveHandler) {
    return archives;
  }

  QSet<QString> seenPaths;
  for (const auto& archiveInfo : archives) {
    seenPaths.insert(QDir::cleanPath(archiveInfo.absoluteFilePath()).toLower());
  }

  QStringList delegatedFilters = archiveHandler->supportedArchiveNameFilters();
  QFileInfoList allFiles =
      delegatedFilters.isEmpty()
          ? directory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot,
                                    QDir::Name | QDir::IgnoreCase)
          : directory.entryInfoList(delegatedFilters, QDir::Files | QDir::NoDotAndDotDot,
                                    QDir::Name | QDir::IgnoreCase);

  for (const auto& fileInfo : allFiles) {
    const QString cleanPath = QDir::cleanPath(fileInfo.absoluteFilePath()).toLower();
    if (seenPaths.contains(cleanPath)) {
      continue;
    }

    if (archiveHandler->supportsArchive(fileInfo.absoluteFilePath())) {
      archives.push_back(fileInfo);
      seenPaths.insert(cleanPath);
    }
  }

  return archives;
}

bool shouldDelegateArchive(
    const QString& archivePath,
    const std::shared_ptr<const MOBase::GameArchiveHandler>& archiveHandler)
{
  return archiveHandler && archiveHandler->supportsArchive(archivePath);
}
