#include "background_applicationuncompressor.h"

#include <QDir>

// utility
#include "../EasyAppsClient/Common/Helpers/pathhelper.h"
#include "../EasyAppsClient/Application/appspecificpathhelper.h"
#include "../EasyAppsClient/Common/Uncompressor/uncompressor.h"

Background_ApplicationUncompressor::Background_ApplicationUncompressor(QString appId, QString zipFileName, QObject *parent) :
    QObject(parent),
    _appId(appId),
    _zipFileName(zipFileName)
{
}

void Background_ApplicationUncompressor::run() {
    // Uncompress into
    QString tmpDirPathToUncompressInto = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmp_Install_Uncompressed_DirectoryPath(), _appId);
    PathHelper::ensureDirectoryCreated(tmpDirPathToUncompressInto);

    // Uncompress this
    QString tempZipFileContainingDir = AppSpecificPathHelper::getLocalAppWhirrTmpDirectory();
    QString tempZipFilePath = PathHelper::combineAndCleanPathes(tempZipFileContainingDir, _zipFileName);

    // Check for the directory to compress into
    if (QDir(tmpDirPathToUncompressInto).exists()) {
        // Clean directory
        if (!PathHelper::deleteThisDirectoryAndEverythingBelow(tmpDirPathToUncompressInto)) {
            Q_EMIT failedWithError("Failed to clean temp datas");
            return;
        }
    }

    // Let's uncompress it
    if (!Uncompressor::uncompress(tempZipFilePath, tmpDirPathToUncompressInto, true)) {
        Q_EMIT failedWithError("Failed to uncompress zip");
        return;
    }

    // Remove created __MACOSX directory
#ifdef Q_WS_MAC
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

    Q_EMIT finishedSuccessfully();
}
