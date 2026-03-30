#ifndef ARCHIVEHANDLERDISPATCH_H
#define ARCHIVEHANDLERDISPATCH_H

#include <memory>

#include <QDir>
#include <QFileInfoList>

#include <uibase/game_features/gamearchivehandler.h>

namespace MOBase
{
class IOrganizer;
}

std::shared_ptr<const MOBase::GameArchiveHandler>
findGameArchiveHandler(const MOBase::IOrganizer* organizer);

QFileInfoList findExtractableArchives(
    const QDir& directory,
    const std::shared_ptr<const MOBase::GameArchiveHandler>& archiveHandler);

bool shouldDelegateArchive(
    const QString& archivePath,
    const std::shared_ptr<const MOBase::GameArchiveHandler>& archiveHandler);

#endif  // ARCHIVEHANDLERDISPATCH_H
