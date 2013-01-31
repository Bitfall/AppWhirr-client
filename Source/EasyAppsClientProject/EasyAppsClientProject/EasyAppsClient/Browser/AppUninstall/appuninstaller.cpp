#include "appuninstaller.h"

#include <QThreadPool>
#include <QDir>
#include <QMetaType>

// utility
#include "Application/appspecificpathhelper.h"
#include "Common/Helpers/pathhelper.h"

#include "Common/Runnables/runnable_deletedirectoryrecursively.h"
//#include "Common/Runnables/runnable_copydirectory.h"
#include "Common/Runnables/runnable_checkforrunningexecutable.h"

#define BACKUP_RENAME_PREFIX QString("bak.")

AppUninstaller::AppUninstaller(QObject *parent) :
    QObject(parent),
    _isInUninstallProcess(false)
{
}

void AppUninstaller::uninstallThisApp(ApplicationDescriptionModel appDescriptionModel)
{
    this->_uninstallQueue.enqueue(appDescriptionModel);
    this->_tryToStartNextUninstallInQueue();
}

void AppUninstaller::retrySuspendedUninstall()
{
    this->_checkWhetherAppRunning();
}

void AppUninstaller::abortSuspendedUninstall()
{
//    this->_startRevertBackup();
    this->_currentUninstallFailedWithError(tr("Running app detected. Cannot remove it."));
}

void AppUninstaller::_tryToStartNextUninstallInQueue()
{
    if(this->_isInUninstallProcess) {
        DLog("Still uninstalling.");
        return;
    }

    if(this->_uninstallQueue.isEmpty()) {
        DLog("Empty queue");
        return;
    }

    this->_isInUninstallProcess = true;

    this->_checkWhetherAppRunning();
}

void AppUninstaller::_currentUninstallFailedWithError(QString errorMessage)
{
    // remove the item
    ApplicationDescriptionModel appDescr = this->_uninstallQueue.dequeue();
    WLog(" -- Uninstall failed for item: ") << appDescr.getDynamicDescription_ReadOnly()->getAppId() << appDescr.getDynamicDescription_ReadOnly()->getAppName();
    // emit the error
    Q_EMIT failedWithError(appDescr, errorMessage);

    // and try to start the next uninstall
    _isInUninstallProcess = false;
    this->_tryToStartNextUninstallInQueue();
}

void AppUninstaller::_currentUninstallFinishedWithSuccess()
{
    ApplicationDescriptionModel appDescr = this->_uninstallQueue.dequeue();
    DLog(" -- Uninstall finished for item: ") << appDescr.getDynamicDescription_ReadOnly()->getAppId() << appDescr.getDynamicDescription_ReadOnly()->getAppName();
    Q_EMIT finishedWithSuccess(appDescr);

    _isInUninstallProcess = false;
    this->_tryToStartNextUninstallInQueue();
}

void AppUninstaller::_checkWhetherAppRunning()
{
    QString appId = this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getAppId();
    Q_EMIT progressStateMessage(appId, tr("Checking for running app"));

    qRegisterMetaType< QList<PlatformspecificRunningExecutableDataInterface*> >("QList<PlatformspecificRunningExecutableDataInterface*>");
    Runnable_CheckForRunningExecutable *runningExeCheckWorker = new Runnable_CheckForRunningExecutable(this->getCurrentAppInstalledRootDirFileInfo().absoluteFilePath());
    connect(runningExeCheckWorker, SIGNAL(finishedWithSuccess(QList<PlatformspecificRunningExecutableDataInterface*>)), this, SLOT(_checkWhetherAppRunningCheckResult(QList<PlatformspecificRunningExecutableDataInterface*>)));
    QThreadPool::globalInstance()->start(runningExeCheckWorker);
}

void AppUninstaller::_checkWhetherAppRunningCheckResult(QList<PlatformspecificRunningExecutableDataInterface *> resultPlatformSpecificRunningAppInfos)
{
    if(resultPlatformSpecificRunningAppInfos.isEmpty()) {
        // no running app detected, start app uninstall with backup
        this->_startBackup();
    }
    else {
        // running app detected!
        Q_EMIT uninstallSuspendedInternally(this->_uninstallQueue.head());
    }
}

// --------------
// --- backup ---

void AppUninstaller::_startBackup()
{
    DLog("--- clear backup");
    QString appId = this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getAppId();
    Q_EMIT progressStateMessage(appId, tr("Preparing app backup"));

#if 0
    // step 1: clear the backup folder
    Runnable_DeleteDirectoryRecursively *backupCleanerWorker = new Runnable_DeleteDirectoryRecursively(AppSpecificPathHelper::getTmp_UninstallBackupDirectoryPath());
    connect(backupCleanerWorker, SIGNAL(failedWithError(QString)), this, SLOT(_backupFolderClearFailedWithError(QString)));
    connect(backupCleanerWorker, SIGNAL(finishedWithSuccess()), this, SLOT(_backupFolderCleareFinished()));
    QThreadPool::globalInstance()->start(backupCleanerWorker);
#endif
    // step 1: clear the backup folder
    QString backupTargetFolderPath = PathHelper::combineAndCleanPathes(
                AppSpecificPathHelper::getCurrentUserLocalAppFolderPath(),
                this->getCurrentAppTargetBackupDirName());
    DLog("Backup target folder path: ") << backupTargetFolderPath;

    Runnable_DeleteDirectoryRecursively *backupCleanerWorker = new Runnable_DeleteDirectoryRecursively(backupTargetFolderPath);
    connect(backupCleanerWorker, SIGNAL(failedWithError(QString)), this, SLOT(_backupFolderClearFailedWithError(QString)));
    connect(backupCleanerWorker, SIGNAL(finishedWithSuccess()), this, SLOT(_backupFolderClearFinished()));
    QThreadPool::globalInstance()->start(backupCleanerWorker);
}

void AppUninstaller::_backupFolderClearFinished()
{
    DLog("--- do backup");

    // step 2: do backup
    if(!QDir(this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getInstalledRootPath()).exists()) {
        WLog("Application is not installed - it's installed root path does not exists here: ") << this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getInstalledRootPath();

        // will emit success to allow the GUI to refresh as not-installed
        this->_currentUninstallFinishedWithSuccess();
        return;
    }
    QString appId = this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getAppId();
    Q_EMIT progressStateMessage(appId, tr("Performing app backup"));

#if 0
    QString appIntalledDirPath = this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getInstalledRootPath();
    Runnable_CopyDirectory *backupWorker = new Runnable_CopyDirectory(appIntalledDirPath, AppSpecificPathHelper::getTmp_UninstallBackupDirectoryPath(), true, PathHelper::SHM_UseAsFile, true);
    connect(backupWorker, SIGNAL(failedWithError(QString)), this, SLOT(_backupFailedWithError(QString)));
    connect(backupWorker, SIGNAL(finishedWithSuccess()), this, SLOT(_backupFinished()));
    QThreadPool::globalInstance()->start(backupWorker);
#endif

    if( !PathHelper::renameFileOrFolder(this->getCurrentAppInstalledRootDirFileInfo(),
                                        this->getCurrentAppTargetBackupDirName()) )
    {
        this->_backupFailedWithError(tr("Cannot backup the app"));
    }
    else {
        this->_backupFinished();
    }
}

void AppUninstaller::_backupFolderClearFailedWithError(QString errorMessage)
{
    this->_currentUninstallFailedWithError(tr("Cannot clear the backup folder: %1").arg(errorMessage));
    PathHelper::logLastOSSpecificFileSystemHandlingError();
}

void AppUninstaller::_backupFinished()
{
    // step 3: try to remove the application
    this->_startRemoveApp();
}

void AppUninstaller::_backupFailedWithError(QString errorMessage)
{
    this->_currentUninstallFailedWithError(tr("Cannot backup the application: %1").arg(errorMessage));
    PathHelper::logLastOSSpecificFileSystemHandlingError();
}

// --------------
// --- remove ---

void AppUninstaller::_startRemoveApp()
{
    DLog("--- start remove");
    QString appId = this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getAppId();
    Q_EMIT progressStateMessage(appId, tr("Removing the app"));
#if 0
    // step 3: remove the application
    Runnable_DeleteDirectoryRecursively *removeWorker = new Runnable_DeleteDirectoryRecursively(this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getInstalledRootPath());
    connect(removeWorker, SIGNAL(failedWithError(QString)), this, SLOT(_appRemoveFailedWithError(QString)));
    connect(removeWorker, SIGNAL(finishedWithSuccess()), this, SLOT(_appRemoveFinished()));
    QThreadPool::globalInstance()->start(removeWorker);
#endif
    // the app is already 'removed' -> it's folder is renamed as backup, so all we need to do is remove the renamed backup
    this->_appRemoveFinished();
}

void AppUninstaller::_appRemoveFinished()
{
    //
    // last step: clean up
    this->_startCleanUp();
}

void AppUninstaller::_appRemoveFailedWithError(QString errorMessage)
{
    WLog("App remove failed with error: ") << errorMessage;
    PathHelper::logLastOSSpecificFileSystemHandlingError();
//    Q_EMIT uninstallSuspendedInternally(this->_uninstallQueue.head());
    this->_currentUninstallFailedWithError("App remove failed.");
}

// ---------------
// --- cleanup ---
void AppUninstaller::_startCleanUp()
{
    QString appId = this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getAppId();
    Q_EMIT progressStateMessage(appId, tr("Performing final cleanups"));

    Runnable_DeleteDirectoryRecursively *removeWorker = new Runnable_DeleteDirectoryRecursively(this->getCurrentAppTargetBackupDirFileInfo().absoluteFilePath());
    connect(removeWorker, SIGNAL(failedWithError(QString)), this, SLOT(_cleanUpFailedWithError(QString)));
    connect(removeWorker, SIGNAL(finishedWithSuccess()), this, SLOT(_cleanUpFinished()));
    QThreadPool::globalInstance()->start(removeWorker);
}

void AppUninstaller::_cleanUpFinished()
{
    this->_currentUninstallFinishedWithSuccess();
}

void AppUninstaller::_cleanUpFailedWithError(QString errorMessage)
{
    WLog("Clean-up failed with error: ") << errorMessage;
    PathHelper::logLastOSSpecificFileSystemHandlingError();
    // backup cleanup is not a critical action in uninstall - everything else succeeded so the uninstall did succeed
    this->_currentUninstallFinishedWithSuccess();
}


// ---------------------
// --- revert backup ---
void AppUninstaller::_startRevertBackup()
{
#if 0
    QString appIntalledDirPath = this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getInstalledRootPath();
    Runnable_CopyDirectory *backupWorker = new Runnable_CopyDirectory(AppSpecificPathHelper::getTmp_UninstallBackupDirectoryPath(), appIntalledDirPath, true, PathHelper::SHM_UseAsFile, false);
    connect(backupWorker, SIGNAL(failedWithError(QString)), this, SLOT(_revertBackupFailedWithError(QString)));
    connect(backupWorker, SIGNAL(finishedWithSuccess()), this, SLOT(_revertBackupFinished()));
    QThreadPool::globalInstance()->start(backupWorker);
#endif

    if( !PathHelper::renameFileOrFolder(this->getCurrentAppTargetBackupDirFileInfo(),
                                        this->getCurrentAppInstalledRootDirName()) )
    {
        this->_revertBackupFailedWithError(tr("Cannot revert the backup"));
    }
    else {
        this->_revertBackupFinished();
    }
}

void AppUninstaller::_revertBackupFinished()
{
    this->_currentUninstallFailedWithError(tr("Cannot uninstall the application."));
}

void AppUninstaller::_revertBackupFailedWithError(QString errorMessage)
{
    WLog("Revert backup failed with error: ") << errorMessage;
    PathHelper::logLastOSSpecificFileSystemHandlingError();

    // don't store anything, revert backup is not that crucial
    this->_currentUninstallFailedWithError(tr("Cannot remove the application."));
}


// ---------------------
// --- helpers

QString AppUninstaller::getBackupDirNameForDir(QString originalDirName) const
{
    return QString("%1%2").arg(BACKUP_RENAME_PREFIX).arg(originalDirName);
}

QFileInfo AppUninstaller::getCurrentAppInstalledRootDirFileInfo() const
{
    return QFileInfo( this->_uninstallQueue.head().getDynamicDescription_ReadOnly()->getInstalledRootPath() );
}

QString AppUninstaller::getCurrentAppInstalledRootDirName() const
{
    QDir appInstalledRootDir( this->getCurrentAppInstalledRootDirFileInfo().absoluteFilePath() );
    return appInstalledRootDir.dirName();
}

QFileInfo AppUninstaller::getCurrentAppTargetBackupDirFileInfo() const
{
    QDir installedDir( this->getCurrentAppInstalledRootDirFileInfo().absoluteFilePath() );
    if( !installedDir.cdUp() ) {
        return QFileInfo();
    }
    return QFileInfo(PathHelper::combineAndCleanPathes(installedDir.absolutePath(),
                                                       this->getBackupDirNameForDir(this->getCurrentAppInstalledRootDirName())));
}

QString AppUninstaller::getCurrentAppTargetBackupDirName() const
{
    return this->getBackupDirNameForDir(this->getCurrentAppInstalledRootDirName());
}
