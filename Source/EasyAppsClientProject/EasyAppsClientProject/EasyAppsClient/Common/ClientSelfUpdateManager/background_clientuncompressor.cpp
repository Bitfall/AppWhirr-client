#include "background_clientuncompressor.h"

#include <QDir>

// utility
#include "../../Application/appspecificpathhelper.h"
#include "../Helpers/pathhelper.h"
#include "../Uncompressor/uncompressor.h"

Background_ClientUncompressor::Background_ClientUncompressor(QObject *parent) :
    QObject(parent)
{
}

void Background_ClientUncompressor::run()
{
    // Uncompress into
    QString tmpDirPathToUncompressInto = AppSpecificPathHelper::getTmp_ClientSelfUpdate_UncompressedTmpCopyWillPerformSelfUpdatePath();

    // Uncompress this
    QString tempZipFilePath = AppSpecificPathHelper::getTmp_ClientSelfUpdate_ZipPath();

    // Check for the directory to compress into
    if (QDir(tmpDirPathToUncompressInto).exists()) {
        // Clean target directory
        if (!PathHelper::deleteThisDirectoryAndEverythingBelow(tmpDirPathToUncompressInto)) {
            Q_EMIT failedWithError("Failed to clean temp datas");
            return;
        }
    }

    PathHelper::ensureDirectoryCreated(tmpDirPathToUncompressInto);

    // Let's uncompress it
    if (!Uncompressor::uncompress(tempZipFilePath, tmpDirPathToUncompressInto, true)) {
        WLog("Uncompress failed.");
        Q_EMIT failedWithError("Failed to uncompress");
        return;
    }
    else {
        DLog("Finished uncompression");
    }

#ifdef Q_WS_MAC
    // Remove created __MACOSX directory
    if (QDir(tmpDirPathToUncompressInto + "/__MACOSX").exists()) {
        if (!PathHelper::deleteThisDirectoryAndEverythingBelow(tmpDirPathToUncompressInto + "/__MACOSX")) {
            Q_EMIT failedWithError("Failed to remove __MACOSX directory");
            return;
        }
    }
#endif

    // Add execution permissions
    if (!PathHelper::setPermissionForFilesOfDirectory(tmpDirPathToUncompressInto, QFile::ExeGroup | QFile::ExeOther | QFile::ExeOwner | QFile::ExeUser, true, true)) {
        Q_EMIT failedWithError("Failed to add permission to the file");
        return;
    }

//    connect(this, SIGNAL(destroyed()), this->_clientSelfUpdateManager, SLOT(_uncompressFinished()));
    Q_EMIT finishedSuccessfully();
}
