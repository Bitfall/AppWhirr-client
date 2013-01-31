#include "awuninstallperformertask.h"

#include <QDir>
#include <QFileInfo>

#include "../EasyAppsClient/Common/awfilefolderbackup.h"
#include "uninstallconfigs.h"
#include "../EasyAppsClient/Application/appspecificpathhelper.h"

AWUninstallPerformerTask::AWUninstallPerformerTask(UninstallConfigs *uninstallConfigs, QObject *parent) :
    AWTask(parent),
    _uninstallConfigs(uninstallConfigs)
{
}

void AWUninstallPerformerTask::run()
{
    this->startTheUninstall();
}

void AWUninstallPerformerTask::startTheUninstall()
{
    if( _uninstallConfigs->getIsRemoveInstalledAppsAsWell() ) {
        this->removeInstalledApps();
    }
    else if( _uninstallConfigs->getIsRemoveAppWhirrAndLocalData() ){
        this->startOSSpecificCleanup_PreClientRemove();
    }
}

void AWUninstallPerformerTask::removeInstalledApps()
{
    Q_EMIT uninstallStateMessage(tr("Removing installed apps..."));

    QString userDirPathString = AppSpecificPathHelper::getLocalUsersDirectoryPath();
    QFileInfo userDirFileInfo(userDirPathString);
    if( !this->deleteFileOrFolderWithBackup(userDirFileInfo) ) {
        _uninstallConfigs->addFailedUninstallStep(UninstallDeleteSteps::RemoveUserApps);
        Q_EMIT uninstallStateMessage(tr("Failed to remove installed apps..."));
    }

    this->finishedWithRemoveInstalledApps();
}

void AWUninstallPerformerTask::finishedWithRemoveInstalledApps()
{
    if( _uninstallConfigs->getIsRemoveAppWhirrAndLocalData() ) {
        this->startOSSpecificCleanup_PreClientRemove();
    }
    else {
        this->finishedWithUninstall();
    }
}

void AWUninstallPerformerTask::removeAppWhirrClient()
{
    Q_EMIT uninstallStateMessage(tr("Removing AppWhirr app..."));

    QFileInfo clientAppFolderFileInfo(_uninstallConfigs->getAppWhirrClientAppFolderPath());

#ifdef Q_OS_MAC
    QString clientAppFolderDirName( QDir(clientAppFolderFileInfo.absoluteFilePath()).dirName() );
    if( clientAppFolderDirName != "AppWhirrData" ) {
        // fail for security reasons (don't delete any other folder, especially not the Applications folder)
        WLog("Because of security reasons the AppWhirr client have to be in the 'AppWhirrData' folder - and it's not. Won't delete the client. => Detected dir-name: ") << clientAppFolderDirName;
        _uninstallConfigs->addFailedUninstallStep(UninstallDeleteSteps::RemoveAppWhirrClientFiles);
        Q_EMIT uninstallStateMessage(tr("Failed to remove the AppWhirr app..."));
        this->finishedWithRemoveAppWhirrClient();
        return;
    }
    else {
        DLog("AppWhirr is in the 'AppWhirrData' folder - uninstall can be performed");
    }
#endif

    if(clientAppFolderFileInfo.exists()) {
        if( !this->deleteFileOrFolderWithBackup(QFileInfo(_uninstallConfigs->getAppWhirrClientAppFolderPath())) ) {
            _uninstallConfigs->addFailedUninstallStep(UninstallDeleteSteps::RemoveAppWhirrClientFiles);
            Q_EMIT uninstallStateMessage(tr("Failed to remove the AppWhirr app..."));
        }
    }

    this->finishedWithRemoveAppWhirrClient();
}

void AWUninstallPerformerTask::finishedWithRemoveAppWhirrClient()
{
    this->removeAppWhirrLocalFiles();
}

void AWUninstallPerformerTask::removeAppWhirrLocalFiles()
{
    Q_EMIT uninstallStateMessage(tr("Removing AppWhirr app's local files..."));

    bool isSomeErrorHappened = false;

    // remove every AppWhirr local folder except the user folder
    QDir awLocalRootDir( AppSpecificPathHelper::getLocalRootDirectory() );
    if( !awLocalRootDir.exists() )
    {
        isSomeErrorHappened = true;
        Q_EMIT uninstallStateMessage(tr("Failed to remove AppWhirr's local files: local folder not found!"));
    }
    else
    {
        QFileInfoList awLocalRootFolderDirs = awLocalRootDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden);
        QFileInfoList awLocalRootFolderFiles = awLocalRootDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Hidden | QDir::System | QDir::Drives/* | QDir::NoSymLinks*/);

        //
        // delete files
        Q_FOREACH( QFileInfo currFi, awLocalRootFolderFiles )
        {
            QString currFileInPath = currFi.fileName();
            QString currFileFullPath = currFi.absoluteFilePath();
    #ifdef ENABLE_FILE_SYSTEM_LOGGING
            DLogS << QString("--- entry in dir [%1]: %2").arg(currFileInPath).arg(currFileFullPath);
    #endif
//            if( !currFi.isSymLink() && !currFi.isFile() )
//            {
//                WLog("Not a symlink and not a file! Cannot remove it!") << currFileFullPath;
//                isSomeErrorHappened = true;
//            }
//            else
//            {
                if( !QFile::remove(currFileFullPath) )
                {
                    WLogS << QString("The specified file cannot be removed: %1 | full path: %2").arg(currFileInPath).arg(currFileFullPath);
                    isSomeErrorHappened = true;
                    Q_EMIT uninstallStateMessage(tr("Failed to remove the file: %1").arg(currFileFullPath));
                }
//            }
        }

        //
        // delete folders except the users folder
        Q_FOREACH( QFileInfo currFi, awLocalRootFolderDirs )
        {
            if(currFi.isSymLink()) {
                if( !QFile::remove(currFi.absoluteFilePath()) ) {
                    WLogS << QString("The specified symlink-to-file cannot be removed: %1").arg(currFi.absoluteFilePath());
                    isSomeErrorHappened = true;
                    Q_EMIT uninstallStateMessage(tr("Failed to remove the symlink-file: %1").arg(currFi.absoluteFilePath()));
                }
            }
            else {
                if( QDir(currFi.absoluteFilePath()).dirName() == AppSpecificPathHelper::getLocalUsersDirectoryDirName() ) {
                    // skip
                    DLog("User dir found - will be skipped: ") << currFi.absoluteFilePath();
                }
                else {
                    // delete + every content of it
                    DLog("Delete dir and it's content: ") << currFi.absoluteFilePath();
                    if( !this->deleteFileOrFolderWithBackup(currFi) ) {
                        isSomeErrorHappened = true;
                        Q_EMIT uninstallStateMessage(tr("Failed to remove the directory: %1").arg(currFi.absoluteFilePath()));
                    }
                }
            }
        }
    }

    if( isSomeErrorHappened ) {
        _uninstallConfigs->addFailedUninstallStep(UninstallDeleteSteps::RemoveAppWhirrLocalFiles);
    }
    this->finishedWithRemoveAppWhirrLocalFiles();
}

void AWUninstallPerformerTask::finishedWithRemoveAppWhirrLocalFiles()
{
    this->startOSSpecificCleanup_PostClientRemove();
}


void AWUninstallPerformerTask::startOSSpecificCleanup_PreClientRemove()
{
#ifdef Q_OS_WIN
    bool isSuccess = this->win_OSSpecificCleanup_PreClientRemove();
#endif
#ifdef Q_OS_MAC
    bool isSuccess = this->mac_OSSpecificCleanup_PreClientRemove();
#endif

    this->finishedWithOSSpecificCleanup_PreClientRemove();
}

void AWUninstallPerformerTask::startOSSpecificCleanup_PostClientRemove()
{
#ifdef Q_OS_WIN
    bool isSuccess = this->win_OSSpecificCleanup_PostClientRemove();
#endif
#ifdef Q_OS_MAC
    bool isSuccess = this->mac_OSSpecificCleanup_PostClientRemove();
#endif

    this->finishedWithOSSpecificCleanup_PostClientRemove();
}

#ifdef Q_OS_WIN
bool AWUninstallPerformerTask::win_OSSpecificCleanup_PreClientRemove()
{
    bool isSuccess = true;

    // auto-startup file
    QString appwhirrWindowStartupFilePath = AppSpecificPathHelper::getWinAndWinUserSpecAppWhirrClientAutoStartupFilePath();
    if( this->removeFileByPath(appwhirrWindowStartupFilePath) ) {
            DLog("Auto-startup file removed");
    }
    else {
        WLog("Auto-startup file found but cannot be removed");
        isSuccess = false;
    }

    // start-menu shortcut file
    QString appwhirrStartMenuShortcutFilePath = AppSpecificPathHelper::getWinAndWinUserSpecAppWhirrClientStartMenuShortcutFilePath();
    DLog("SH ")<< appwhirrStartMenuShortcutFilePath;
    if( this->removeFileByPath(appwhirrStartMenuShortcutFilePath) ) {
            DLog("Start-menu shortcut file removed");
    }
    else {
        WLog("Start-menu shortcut found but cannot be removed");
        isSuccess = false;
    }

    // desktop shortcut file
    QString appwhirrDesktopShortcutFilePath = AppSpecificPathHelper::getWinAndWinUserSpecAppWhirrClientDesktopShortcutFilePath();
    if( this->removeFileByPath(appwhirrDesktopShortcutFilePath) ) {
            DLog("Desktop shortcut file removed");
    }
    else {
        WLog("Desktop shortcut found but cannot be removed");
        isSuccess = false;
    }

    return isSuccess;
}

bool AWUninstallPerformerTask::win_OSSpecificCleanup_PostClientRemove()
{
    // nothing to do post-client-remove
    return true;
}
#endif

#ifdef Q_OS_MAC
bool AWUninstallPerformerTask::mac_OSSpecificCleanup_PreClientRemove()
{
    bool isSuccess = true;
    // remove the alias from Applications
    QFileInfo awClientAliasFileInfo( AppSpecificPathHelper::getMacSpecAppWhirrClientAliasFilePath() );
    if( awClientAliasFileInfo.exists() && awClientAliasFileInfo.isSymLink() ) {
        DLog("Client symlink found");
        if( !QFile(awClientAliasFileInfo.absoluteFilePath()).remove() ) {
            WLog("Cannot remove the alias!");
            isSuccess = false;
        }
    }

    return isSuccess;
}

bool AWUninstallPerformerTask::mac_OSSpecificCleanup_PostClientRemove()
{
    // nothing to do post-client-remove
    return true;
}
#endif

void AWUninstallPerformerTask::finishedWithOSSpecificCleanup_PreClientRemove()
{
    this->removeAppWhirrClient();
}

void AWUninstallPerformerTask::finishedWithOSSpecificCleanup_PostClientRemove()
{
    this->finishedWithUninstall();
}

void AWUninstallPerformerTask::finishedWithUninstall()
{
    Q_EMIT finished();
}

bool AWUninstallPerformerTask::deleteFileOrFolderWithBackup(QFileInfo fileOrFolderFileInfoToBeDeleted)
{
    AWFileFolderBackup backupper(fileOrFolderFileInfoToBeDeleted);
    if( !backupper.doBackup() ) {
        return false;
    }
    else
    {
        // backup success
        //  -> delete it
        if( !backupper.deleteBackup() ) {
            if( !backupper.rollbackBackup() ) {
                WLog("Failed to rollback!");
            }
            return false;
        }
    }

    return true;
}

bool AWUninstallPerformerTask::removeFileByPath(QString filePathString)
{
    QFile fileToRemove( filePathString );
    if( fileToRemove.exists() ) {
        DLog("File exists: ") << filePathString;
        if( !fileToRemove.remove() ) {
            WLog("CANNOT remove the file: ") << filePathString;
            return false;
        }
    }
    else {
        WLog("File does not exists! : ") << filePathString;
    }

    return true;
}
