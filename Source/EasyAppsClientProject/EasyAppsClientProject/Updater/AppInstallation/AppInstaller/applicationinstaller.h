#ifndef BASEAPPLICATIONINSTALLER_H
#define BASEAPPLICATIONINSTALLER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

#include "../installfallbackstatesenum.h"
#include "../EasyAppsClient/Application/Managers/AppGrab/appinstallstates.h"

class PlatformspecificRunningExecutableDataInterface;

class ApplicationInstaller : public QObject
{
    Q_OBJECT
    Q_ENUMS(AppInstallStates::AppInstallStatesEnum)

public:
    explicit ApplicationInstaller(QObject *parent = 0);
//    virtual ~ApplicationInstaller();

    void startInstall();

    void setApplicationZipFileNameAndAppId(const QString& applicationZipFileName, QString appId);

Q_SIGNALS:
    void applicationInstallationFailed(QString errorMessage, ApplicationInstaller* sender);
    void applicationInstallationSucceeded(ApplicationInstaller* sender);
    void applicationHasARunningCopy();
    void installProgressState(AppInstallStates::AppInstallStatesEnum appInstallProgressState);

    void _internalInstallFailed(QString errorMessage, ApplicationInstaller* sender);


    // -------------------------------
    // --- app install
private Q_SLOTS:
    void startAppInstall();

    // -------------------------------
    // --- before install cleanup
//private Q_SLOTS:
    /*! Start a 'before app install' cleanup on the target directory of the app
    */
//    void cleanupAppInstallTargetFolder();
//    void _cleanupAppInstallTargetFolderFinishedWithSuccess();
//    void _cleanupAppInstallTargetFolderFailedWithError(QString errorMessage);

    // -------------------------------
    // --- running copy check
private Q_SLOTS:
//    void installUncompressedApplication();
    void checkForRunningCopy();
private Q_SLOTS:
    void __resultOfRunningExecutableCheck(QList<PlatformspecificRunningExecutableDataInterface*> result);

private Q_SLOTS:
//    void _uncompressorFinishedSuccessfully();
//    void _uncompressorFailedWithError(QString errorMessage);

private Q_SLOTS:
    void _installerFailedWithError(QString errorMessage);
    void _installerCompletedBackupStep();
    void _installerFinishedSuccessfully();

private:
    void errorHappenedSoCleanUpThenFailWithError(QString errorMessage);
private Q_SLOTS:
    void afterErrorCleanupFinished(QString theGivenErrorMessage);


private:
    InstallFallbackStates::InstallFallbackStatesEnum _fallbackState;
    QString                         _applicationZipFileName;
    QString                         _appId;
};

#endif // BASEAPPLICATIONINSTALLER_H
