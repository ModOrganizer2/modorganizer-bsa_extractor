#include <gtest/gtest.h>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include "../src/archivehandlerdispatch.h"

namespace
{

class FakeArchiveHandler : public MOBase::GameArchiveHandler
{
public:
  explicit FakeArchiveHandler(QStringList supportedFiles)
      : m_SupportedFiles(std::move(supportedFiles))
  {}

  bool supportsArchive(const QString& archivePath) const override
  {
    const QString fileName = QFileInfo(archivePath).fileName();
    return m_SupportedFiles.contains(fileName, Qt::CaseInsensitive);
  }

  bool extractArchive(const QString&, const QString&, const ProgressCallback&,
                      QString*) const override
  {
    return true;
  }

private:
  QStringList m_SupportedFiles;
};

void touchFile(const QString& path)
{
  QFile file(path);
  ASSERT_TRUE(file.open(QIODevice::WriteOnly));
  file.close();
}

}  // namespace

TEST(ArchiveHandlerDispatch, FallsBackToBuiltInArchivePatternsWithoutFeature)
{
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  touchFile(QDir(tempDir.path()).filePath("foo.bsa"));
  touchFile(QDir(tempDir.path()).filePath("foo.ba2"));
  touchFile(QDir(tempDir.path()).filePath("foo.rsc"));

  const auto archives = findExtractableArchives(QDir(tempDir.path()), true, {});

  ASSERT_EQ(archives.size(), 2);
  EXPECT_EQ(archives.at(0).fileName(), "foo.ba2");
  EXPECT_EQ(archives.at(1).fileName(), "foo.bsa");
}

TEST(ArchiveHandlerDispatch, AddsDelegatedArchivesWhenFeatureSupportsThem)
{
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  touchFile(QDir(tempDir.path()).filePath("arena.bsa"));
  touchFile(QDir(tempDir.path()).filePath("textures.rsc"));
  touchFile(QDir(tempDir.path()).filePath("notes.txt"));

  auto handler =
      std::make_shared<FakeArchiveHandler>(QStringList({"arena.bsa", "textures.rsc"}));
  const auto archives = findExtractableArchives(QDir(tempDir.path()), true, handler);

  ASSERT_EQ(archives.size(), 2);
  EXPECT_EQ(archives.at(0).fileName(), "arena.bsa");
  EXPECT_EQ(archives.at(1).fileName(), "textures.rsc");
}

TEST(ArchiveHandlerDispatch, DelegatesOnlyWhenFeatureClaimsArchiveSupport)
{
  auto handler = std::make_shared<FakeArchiveHandler>(QStringList({"daggerfall.bsa"}));

  EXPECT_TRUE(shouldDelegateArchive("D:/mods/daggerfall.bsa", handler));
  EXPECT_FALSE(shouldDelegateArchive("D:/mods/skyrim.ba2", handler));
  EXPECT_FALSE(shouldDelegateArchive("D:/mods/skyrim.ba2", {}));
}

