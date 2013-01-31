#include "runnable_appinstallcleanupaftererror.h"

#include <QDir>

#include "../EasyAppsClient/Common/Helpers/pathhelper.h"
#include "../EasyAppsClient/Application/appspecificpathhelper.h"

Runnable_AppInstallCleanupAfterError::Runnable_AppInstallCleanupAfterError(InstallFallbackStates::InstallFallbackStatesEnum fallbackState, QString appId, QString givenErrorMessage, QObject *parent) :
    QObject(parent),
    _fallbackState(fallbackState),
    _appId(appId),
    _givenErrorMessage(givenErrorMessage)
{
}

void Runnable_AppInstallCleanupAfterError::run()
{
    // ! rewritten: proper handling of backup (and cleanup) is now the background_installer's responsibility
#if 0
    // this part needs some review
    switch(_fallbackState) {
    case InstallFallbackStates::RemovedTempFiles:
        // Everything was deleted
        // So there is no way to copy back the previous version
        break;
//    case savedApplicationDatasToDatabase:
        // Nothing to do
//    case copiedDownloadedApplicationToApplicationDirectory:
        // Nothing to do
    case InstallFallbackStates::CopiedInstalledApplicationToTempDirectory:
        //
        // revert by backup
        if (QDir(AppSpecificPathHelper::getTmp_Install_InstalledBackup_DirectoryPath()).exists()) {
            // Remove installed app
            QString userLocalAppDirPath = AppSpecificPathHelper::getCurrentUserLocalAppFolderPath();

            if(userLocalAppDirPath.isEmpty()) {
                WLog("User related app-folder cannot be retrieved. Probably no user selected.");
            }
            else {
                QString installedApplicationDirectoryPath = AppSpecificPathHelper::getCurrentUserLocalInstalledAppPathForAppId(this->_appId);
                PathHelper::deleteThisDirectoryAndEverythingBelow(installedApplicationDirectoryPath);
                PathHelper::copyWholeDirectory(AppSpecificPathHelper::getTmp_Install_InstalledBackup_DirectoryPath(), installedApplicationDirectoryPath, true, PathHelper::SHM_UseAsFile, false);
                // Copy back previous app
                PathHelper::deleteThisDirectoryAndEverythingBelow(AppSpecificPathHelper::getTmp_Install_InstalledBackup_DirectoryPath());
            }
        }
//    case InstallFallbackStates::SuccessfullyUncompressedZippedApplication:
        //
        // remove the uncompressed files
//        QDir().remove(AppSpecificPathHelper::getTmp_Install_Uncompressed_DirectoryPath());
    }
#endif

    Q_EMIT finished(this->_givenErrorMessage);
}
