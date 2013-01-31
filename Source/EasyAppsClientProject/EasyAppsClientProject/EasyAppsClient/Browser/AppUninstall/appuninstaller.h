#ifndef APPUNINSTALLER_H
#define APPUNINSTALLER_H

#include <QObject>
#include <QQueue>
#include <QFileInfo>

#include "../../Application/ApplicationDescription/applicationdescriptionmodel.h"

class PlatformspecificRunningExecutableDataInterface;

class AppUninstaller : public QObject
{
    Q_OBJECT
public:
    explicit AppUninstaller(QObject *parent = 0);

    //
    // NOT THREAD-SAFE
    void uninstallThisApp(ApplicationDescriptionModel appDescriptionModel);

Q_SIGNALS: // main communication signals
    /*! General printable progress-state message (like: 'backuping', 'cleanup', etc)
    */
    void progressStateMessage(QString appId, QString uninstallProgressState);
    void finishedWithSuccess(ApplicationDescriptionModel inputAppDescriptionModel);
    void failedWithError(ApplicationDescriptionModel inputAppDescriptionModel, QString errorMessage);

    // an uninstall can become suspended internally, if it fails in some parts
    // e.g.: if it cannot remove files then it suspends the uninstall and a delegate can try to resolve the problem
    //  currently it will try to close the given app, because probably that causes the trouble
    //
    // [!] so when AppUninstaller suspends an uninstall it emits uninstallSuspendedInternally()
    //  and it won't continue until a delegate calls it's retrySuspendedUninstall()
    //  -> in this case it will retry the uninstall and will emit the same signals if it fails
    //  [!] if the delegate cannot resolve the problems then it HAS TO call the abortSuspendedUninstall() slot -> after this the AppUninstaller will free-up the temporary backups and will try to start the next uninstall if additional items added
    void uninstallSuspendedInternally(ApplicationDescriptionModel inputAppDescriptionModel);

public Q_SLOTS:
    void retrySuspendedUninstall();
    void abortSuspendedUninstall();

private Q_SLOTS:
    void _tryToStartNextUninstallInQueue();
    void _currentUninstallFailedWithError(QString errorMessage);
    void _currentUninstallFinishedWithSuccess();
//    void _storeError(QString errorMessage);

    // -----------------------------
    // --- app running check
    void _checkWhetherAppRunning();
    void _checkWhetherAppRunningCheckResult(QList<PlatformspecificRunningExecutableDataInterface *> resultPlatformSpecificRunningAppInfos);

    // --------------
    // --- backup ---
    void _startBackup();
    void _backupFolderClearFinished();
    void _backupFolderClearFailedWithError(QString errorMessage);
    void _backupFinished();
    void _backupFailedWithError(QString errorMessage);

    // --------------
    // --- remove ---
    void _startRemoveApp();
    void _appRemoveFinished();
    void _appRemoveFailedWithError(QString errorMessage);

    // ---------------
    // --- cleanup ---
    void _startCleanUp();
    void _cleanUpFinished();
    void _cleanUpFailedWithError(QString errorMessage);

    // ---------------------
    // --- revert backup ---
    void _startRevertBackup();
    void _revertBackupFinished();
    void _revertBackupFailedWithError(QString errorMessage);

    // ---------------------
    // --- helpers
private:
    QString getBackupDirNameForDir(QString originalDirName) const;
    QFileInfo getCurrentAppInstalledRootDirFileInfo() const;
    QString getCurrentAppInstalledRootDirName() const;
    QFileInfo getCurrentAppTargetBackupDirFileInfo() const;
    QString getCurrentAppTargetBackupDirName() const;

private:
    QQueue<ApplicationDescriptionModel> _uninstallQueue;
    bool _isInUninstallProcess;
};

#endif // APPUNINSTALLER_H
