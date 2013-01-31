#include "backgroundclientinstaller.h"

// utility
#include "../EasyAppsClient/Common/Helpers/pathhelper.h"
#include "../EasyAppsClient/Application/appspecificpathhelper.h"

#include <QDir>
#include <QStringList>

#ifdef Q_OS_MAC
#include "Helpers/mac_applescriptexecutor.h"
#endif

Background_ClientInstaller::Background_ClientInstaller(QString clientRootPath, InstallMajorSteps continueFromThisStep, QObject *parent) :
    QObject(parent),
    _clientRootPath(clientRootPath),
    _lastStepDone(continueFromThisStep)
{

}

void Background_ClientInstaller::run()
{
    Q_EMIT installPercentage(0);
    // first: get admin rights and set required permissions (it's an issue with the AppWhirr v0.9.15's installer)
#ifdef Q_OS_MAC
    DLog("Client root path: ") << _clientRootPath;
    QString asToExecute = QString("do shell script \"chmod -R 777 %1\" with administrator privileges").arg(_clientRootPath);
    Mac_AppleScriptExecutor::executeAppleScript(asToExecute);
#endif

    Q_EMIT installPercentage(10);
    if(this->_lastStepDone == DoFromTheBeginning) {
        if(!this->tryToBackup()) {
            Q_EMIT failedWithErrorMessage(tr("Cannot backup the old version"), DoFromTheBeginning);
            return;
        }
        else {
            this->_lastStepDone = BackupFinished;
        }
    }

    Q_EMIT installPercentage(25);
    if(this->_lastStepDone == BackupFinished) {
        if(!this->tryToRemoveOldVersion()) {
            Q_EMIT failedWithErrorMessage(tr("Cannot remove the old version. Probably it's still running"), BackupFinished);
            return;
        }
        else {
            this->_lastStepDone = OldVersionRemoved;
        }
    }

    Q_EMIT installPercentage(50);
    if(this->_lastStepDone == OldVersionRemoved) {
        if(!this->copyNewVersion()) {
            // try to restore from backup - but if the copy of the new version failed this one will as well...
            PathHelper::copyWholeDirectory(AppSpecificPathHelper::getTmp_ClientSelfUpdate_OldVersionBackupFolder(), this->_clientRootPath, true, PathHelper::SHM_UseAsFile);
            Q_EMIT failedWithErrorMessage(tr("Cannot install the new version"), OldVersionRemoved);
            return;
        }
        else {
            this->_lastStepDone = NewVersionCopiedToItsFinalPlace;
        }
    }

    Q_EMIT installPercentage(75);
    if(this->_lastStepDone == NewVersionCopiedToItsFinalPlace) {
        if(!this->setRequiredPermissionsForTheNewVersion()) {
            Q_EMIT failedWithErrorMessage(tr("Cannot set required permissions for the new version"), NewVersionCopiedToItsFinalPlace);
            return;
        }
        else {
            Q_EMIT installPercentage(100);
            Q_EMIT succeeded();
        }
    }
}

bool Background_ClientInstaller::tryToBackup()
{
    // remove the previous backup
    if(!PathHelper::deleteThisDirectoryAndEverythingBelow(AppSpecificPathHelper::getTmp_ClientSelfUpdate_OldVersionBackupFolder())) {
        WLog("Failed to clear the backup folder.");
        return false;
    }
    else {
        DLog("Backup folder cleared.");
    }

#ifdef Q_OS_WIN
    // do the new backup
    if(!PathHelper::copyWholeDirectory(this->_clientRootPath, AppSpecificPathHelper::getTmp_ClientSelfUpdate_OldVersionBackupFolder(), true, PathHelper::SHM_UseAsFile)) {
        WLog("Failed to do the backup.");
        return false;
    }
    else {
        DLog("Backup finished.");
    }
#endif

#ifdef Q_OS_MAC
   // Get apps with *.appwhirr.bitfall bundle identifier and delete them
    QDir root(this->_clientRootPath);

    QStringList dirs = root.entryList(QDir::NoDotAndDotDot | QDir::Dirs);

    QString dir;
    Q_FOREACH(dir, dirs) {
        if (dir.endsWith(".app", Qt::CaseInsensitive)) {
            QString path = PathHelper::combineAndCleanPathes(this->_clientRootPath, dir);
            if (PathHelper::getBundleIdentifier(path).endsWith(".appwhirr.bitfall")) {
                // do the new backup
                if(!PathHelper::copyWholeDirectory(path, AppSpecificPathHelper::getTmp_ClientSelfUpdate_OldVersionBackupFolder(), false, PathHelper::SHM_UseAsFile)) {
                    WLog("Failed to do the backup.");
                    return false;
                }
                else {
                    DLog("Backup finished.");
                }
            }
        }
    }
#endif

    return true;
}

bool Background_ClientInstaller::tryToRemoveOldVersion() {
#ifdef Q_OS_WIN
    // remove the current version
    if(!PathHelper::deleteThisDirectoryAndEverythingBelow(this->_clientRootPath)) {
        WLog("Failed to remove the old version.");
        return false;
    }
    else {
        DLog("Old version removed.");
    }
#endif

#ifdef Q_OS_MAC
   // Get apps with *.appwhirr.bitfall bundle identifier and delete them
    QDir root(this->_clientRootPath);

    QStringList dirs = root.entryList(QDir::NoDotAndDotDot | QDir::Dirs);

    QString dir;
    Q_FOREACH(dir, dirs) {
        if (dir.endsWith(".app", Qt::CaseInsensitive)) {
            QString path = PathHelper::combineAndCleanPathes(this->_clientRootPath, dir);
            if (PathHelper::getBundleIdentifier(path).endsWith(".appwhirr.bitfall")) {
                // It's an AW app, remove it.
                if(!PathHelper::deleteThisDirectoryAndEverythingBelow(path)) {
                    WLog("Failed to remove the old version.");
                    return false;
                }
                else {
                    DLog("Old version removed.");
                }
            }
        }
    }

#endif

    return true;
}

bool Background_ClientInstaller::copyNewVersion()
{
    // recreate the directory
    if(!PathHelper::ensureDirectoryCreated(this->_clientRootPath)) {
        WLog("Failed to recreate target client directory.");
        return false;
    }
    else {
        DLog("Target client directory recreated.");
    }

    if(!PathHelper::copyWholeDirectory(AppSpecificPathHelper::getTmp_ClientSelfUpdate_UncompressedNewVersionPath(), this->_clientRootPath, true, PathHelper::SHM_UseAsFile)) {
        WLog("Failed to copy the new version to it's final place.");
        return false;
    }
    else {
        DLog("New version copied to it's final place.");
    }

    return true;
}

bool Background_ClientInstaller::setRequiredPermissionsForTheNewVersion()
{
#ifdef Q_OS_WIN
    // set execution permission first
    if (!PathHelper::setPermissionForFilesOfDirectory(this->_clientRootPath, QFile::ExeGroup | QFile::ExeOther | QFile::ExeOwner | QFile::ExeUser, true, true)) {
        WLog("Failed to set required permissions.");
        return false;
    }
    else {
        DLog("Permissions set.");
        return true;
    }
#endif

#ifdef Q_OS_MAC
   // Get apps with *.appwhirr.bitfall bundle identifier and delete them
    QDir root(this->_clientRootPath);

    QStringList dirs = root.entryList(QDir::NoDotAndDotDot | QDir::Dirs);

    QString dir;
    Q_FOREACH(dir, dirs) {
        if (dir.endsWith(".app", Qt::CaseInsensitive)) {
            QString path = PathHelper::combineAndCleanPathes(this->_clientRootPath, dir);
            if (PathHelper::getBundleIdentifier(path).endsWith(".appwhirr.bitfall")) {
                // set execution permission first
                if (!PathHelper::setPermissionForFilesOfDirectory(path, QFile::ExeGroup | QFile::ExeOther | QFile::ExeOwner | QFile::ExeUser, true, true)) {
                    WLog("Failed to set required permissions.");
                    return false;
                }
                else {
                    DLog("Permissions set.");
                }
            }
        }
    }

    return true;
#endif
}
