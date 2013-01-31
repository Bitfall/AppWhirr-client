#include "background_appinstaller.h"

#include <QDir>

// utility
#include "../EasyAppsClient/Common/Helpers/pathhelper.h"
#include "../EasyAppsClient/Common/Uncompressor/uncompressor.h"
#include "../EasyAppsClient/Application/appspecificpathhelper.h"
#include "../EasyAppsClient/Common/awfilefolderbackup.h"

Background_AppInstaller::Background_AppInstaller(QString appId, QString zipFileName, QObject *parent) :
    QObject(parent),
    _appId(appId),
    _zipFileName(zipFileName)
{
}

void Background_AppInstaller::run() {

    DLog("-- Main app-install process [begin]");

    // user check
    QString userLocalAppDirPath = AppSpecificPathHelper::getCurrentUserLocalAppFolderPath();

    if(userLocalAppDirPath.isEmpty()) {
        WLog("User related app-folder cannot be retrieved. Probably no user selected.");
        Q_EMIT failedWithError("Probably no user selected.");
        return;
    }

    //
    // --- backup

    // Copy the installed application to a temporary directory if exists
//    QString installedApplicationDirectoryPath = PathManager::combineAndCleanPathes(userLocalAppDirPath, applicationInstaller->_applicationZipFileName.split(".").at(0));
    QString installedApplicationDirectoryPath = AppSpecificPathHelper::getCurrentUserLocalInstalledAppPathForAppId(this->_appId);

    AWFileFolderBackup fileFolderBackup(QFileInfo(installedApplicationDirectoryPath), QString("upb."));

    Q_EMIT installProgressState(AppInstallStates::PerformingBackup);
    if( !fileFolderBackup.doBackup() ) {
        Q_EMIT failedWithError("Backup failed!");
        return;
    }
    Q_EMIT completedBackupStep();

#if 0
    if (QDir(installedApplicationDirectoryPath).exists())
    {
        DLog("Prepare backup");
        // already installed - create a backup about the current version before copy the new one

        // clean the backup folder
        Q_EMIT installProgressState(AppInstallStates::PreparingBackup);
        if (!PathHelper::deleteThisDirectoryAndEverythingBelow(AppSpecificPathHelper::getTmp_Install_InstalledBackup_DirectoryPath())) {
            Q_EMIT failedWithError("Failed to clean temp datas for installation");
            return;
        }

        // ensure it exists
        if (!PathHelper::ensureDirectoryCreated(AppSpecificPathHelper::getTmp_Install_InstalledBackup_DirectoryPath())) {
            Q_EMIT failedWithError("Failed to create temp directory for installed file");
            return;
        }

        // do backup
        DLog("Do backup");
        Q_EMIT installProgressState(AppInstallStates::PerformingBackup);
        if (!PathHelper::copyWholeDirectory(installedApplicationDirectoryPath, AppSpecificPathHelper::getTmp_Install_InstalledBackup_DirectoryPath(), true, PathHelper::SHM_UseAsFile)) {
            Q_EMIT failedWithError("Failed to copy installed files to temp folder");
            return;
        }

        Q_EMIT completedBackupStep();

        // remove the installed application
//        DLog("Remove the installed app files.");
//        if (!PathHelper::deleteThisDirectoryAndEverythingBelow(installedApplicationDirectoryPath)) {
//            Q_EMIT failedWithError("Failed to clean already installed application");
//            return;
//        }
    }
    else {
        DLog("Backup not needed.");
    }
#endif

    // ---


    //
    // --- cleanup
    // clean the app's installed folder
#if 0
    Q_EMIT installProgressState(AppInstallStates::PreparingAppDirectory);
    if (!PathHelper::deleteThisDirectoryAndEverythingBelow(installedApplicationDirectoryPath)) {
        Q_EMIT failedWithError("Failed to remove app's installed files.");
        return;
    }
#endif
    // ---


    //
    // --- uncompress
    // Uncompress this
    Q_EMIT installProgressState(AppInstallStates::PerformingInstall);
    QString tempZipFileContainingDir = AppSpecificPathHelper::getLocalAppWhirrTmpDirectory();
    QString tempZipFilePath = PathHelper::combineAndCleanPathes(tempZipFileContainingDir, _zipFileName);
    QStringList skipTheseRootDirsDuringUncompress;
    skipTheseRootDirsDuringUncompress << "__MACOSX";
    if (!Uncompressor::uncompress(tempZipFilePath, installedApplicationDirectoryPath, true, skipTheseRootDirsDuringUncompress, true)) {
        WLog("Uncompress failed.");
        QString errorStr = "Failed to uncompress";
        if( fileFolderBackup.rollbackBackup() ) {
            WLog("!FATAL! Cannot rollback the backup!!");
            errorStr += " | Failed to rollback the backup!";
        }
        Q_EMIT failedWithError(errorStr);
        return;
    }
    else {
        DLog("Finished app uncompression");
    }
    // ---

//    applicationInstaller->fallbackID = copiedDownloadedApplicationToApplicationDirectory;

//    // Save installed application datas to our database
//    // TODO: version number is not set yet!
//    applicationInstaller->fallbackID = savedApplicationDatasToDatabase;

    // Remove temporary files
    DLog("Remove temporary files");
    Q_EMIT installProgressState(AppInstallStates::AfterInstallCleanup);
    if( !fileFolderBackup.deleteBackup() ) {
        WLog("[CLEANUP] Cannot delete the backup (after successfull app install)");
    }
    if( !QFile().remove(PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalAppWhirrTmpDirectory(), _zipFileName)) ) {
        WLog("[CLEANUP] Cannot delete the app-pack-ZIP (after successfull app install)");
    }

#if 0
    if (/*!PathManager::deleteThisDirectoryAndEverythingBelow(tmpDirPathToUncompressInto) ||*/ // unnecessary - will delete the whole getTmp_Install_Uncompressed_DirectoryPath() directory
        !PathHelper::deleteThisDirectoryAndEverythingBelow(AppSpecificPathHelper::getTmp_Install_InstalledBackup_DirectoryPath()) ||
//        !PathHelper::deleteThisDirectoryAndEverythingBelow(AppSpecificPathHelper::getTmp_Install_Uncompressed_DirectoryPath()) ||
        !QFile().remove(PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmpDirectory(), _zipFileName))) {
//        Q_EMIT failedWithError("Failed to clean temp datas");
//        return;
        WLog("Cannot clean-up tmp dirs and/or zip. 'After successfull install' step.");
    }
#endif

//    applicationInstaller->fallbackID = removedTempFiles;

//    Q_EMIT applicationInstaller->applicationInstallationSucceeded(applicationInstaller);

    DLog("-- Main app-install process [end: SUCCESS!]");

    Q_EMIT finishedSuccessfully();
}
