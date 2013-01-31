#include "applicationinstaller.h"

#include "../EasyAppsClient/Common/Helpers/pathhelper.h"
#include "../EasyAppsClient/Application/appspecificpathhelper.h"
#include "../EasyAppsClient/Common/Runnables/runnable_checkforrunningexecutable.h"
#include "../EasyAppsClient/Common/Runnables/runnable_deletedirectoryrecursively.h"

#include <QFile>
#include <QDir>
#include <QStringList>
#include <QThreadPool>
#include <QMetaType>

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

#ifdef Q_WS_WIN
#include "../EasyAppsClient/Common/Application/win_applicationprocessbyexedata.h"
#endif

// utility
#include "../EasyAppsClient/Common/Database/simplelogdatabasemanager.h"
#include "./background_applicationuncompressor.h"
#include "./background_appinstaller.h"
#include "../EasyAppsClient/Common/Application/platformspecificrunningexecutabledatainterface.h"
#include "../Runnables/runnable_appinstallcleanupaftererror.h"

// config
#include "updaterconfigs.h"

ApplicationInstaller::ApplicationInstaller(QObject *parent) :
    QObject(parent),
    _fallbackState(InstallFallbackStates::NoAction)
{

}

//ApplicationInstaller::~ApplicationInstaller() {
//    this->disconnect();
//}

void ApplicationInstaller::errorHappenedSoCleanUpThenFailWithError(QString errorMessage)
{
    WLog("Error happened: ") << errorMessage;
    DLog("Clean up the Error-mess.");

    Runnable_AppInstallCleanupAfterError *worker = new Runnable_AppInstallCleanupAfterError(this->_fallbackState, this->_appId, errorMessage);
    connect(worker, SIGNAL(finished(QString)), this, SLOT(afterErrorCleanupFinished(QString)));
    QThreadPool::globalInstance()->start(worker);


//    Q_EMIT applicationInstallationFailed(errorMessage, sender);
}

void ApplicationInstaller::afterErrorCleanupFinished(QString theGivenErrorMessage)
{
    Q_EMIT applicationInstallationFailed(theGivenErrorMessage, this);
}

void ApplicationInstaller::setApplicationZipFileNameAndAppId(const QString& applicationZipFileName, QString appId) {
    this->_applicationZipFileName = applicationZipFileName;
    this->_appId = appId;
}

void ApplicationInstaller::startInstall()
{
    this->checkForRunningCopy();
}


// -------------------------------
// --- app install

void ApplicationInstaller::startAppInstall()
{
    Background_AppInstaller *installer = new Background_AppInstaller(this->_appId, this->_applicationZipFileName);
    connect(installer, SIGNAL(failedWithError(QString)), this, SLOT(_installerFailedWithError(QString)));
    connect(installer, SIGNAL(installProgressState(AppInstallStates::AppInstallStatesEnum)), this, SIGNAL(installProgressState(AppInstallStates::AppInstallStatesEnum)));
    connect(installer, SIGNAL(completedBackupStep()), this, SLOT(_installerCompletedBackupStep()));
    connect(installer, SIGNAL(finishedSuccessfully()), this, SLOT(_installerFinishedSuccessfully()));
    QThreadPool::globalInstance()->start(installer);
}

// -------------------------------
// --- before install cleanup

void ApplicationInstaller::_installerFailedWithError(QString errorMessage)
{
    this->errorHappenedSoCleanUpThenFailWithError(errorMessage);
}

void ApplicationInstaller::_installerCompletedBackupStep()
{
    this->_fallbackState = InstallFallbackStates::CopiedInstalledApplicationToTempDirectory;
}

void ApplicationInstaller::_installerFinishedSuccessfully()
{
    this->_fallbackState = InstallFallbackStates::SuccessfullyUncompressedZippedApplication;
    Q_EMIT applicationInstallationSucceeded(this);
}

// --------------------------------------
// --- running exe detection + handling

void ApplicationInstaller::checkForRunningCopy()
{
    QString userLocalAppRelatedInstalledRootPath = AppSpecificPathHelper::getCurrentUserLocalInstalledAppPathForAppId(this->_appId);

    if(userLocalAppRelatedInstalledRootPath.isEmpty())
    {
        WLog("User related app-folder cannot be retrieved. Probably no user selected.");

        // and fail
        Q_EMIT applicationInstallationFailed("Probably no user selected.", this);
        return;
    }

    qRegisterMetaType< QList<PlatformspecificRunningExecutableDataInterface*> >("QList<PlatformspecificRunningExecutableDataInterface*>");

    DLog("-- checking for running app at path: ") << userLocalAppRelatedInstalledRootPath;
    Runnable_CheckForRunningExecutable *worker = new Runnable_CheckForRunningExecutable(userLocalAppRelatedInstalledRootPath);
    connect(worker, SIGNAL(finishedWithSuccess(QList<PlatformspecificRunningExecutableDataInterface*>)), this, SLOT(__resultOfRunningExecutableCheck(QList<PlatformspecificRunningExecutableDataInterface*>)));
    QThreadPool::globalInstance()->start(worker);
}

void ApplicationInstaller::__resultOfRunningExecutableCheck(QList<PlatformspecificRunningExecutableDataInterface*> result)
{

//    if (!PathHelper::checkDirectoryCanBeSaflyCopiedToDirectory(tmpDirPathToUncompressInto, userLocalAppDirPath, runningExecutableDataList)) {
    if (!result.isEmpty()) {
        DLog("-- running app found");
        // setup mac specific datas
#ifdef Q_WS_MAC
#warning MAC - atnez
//        CFNumberRef psnNumber = (CFNumberRef)CFDictionaryGetValue((CFDictionaryRef)runningCopyData, CFSTR("PSN"));

//        SInt64 psnLongLongValue;
//        CFNumberGetValue(psnNumber, kCFNumberSInt64Type, &psnLongLongValue);
//        ProcessSerialNumber psn = { (int)(psnLongLongValue>>32), (int)psnLongLongValue };

//        int pid;
//        GetProcessPID(&psn, &pid);
#endif

#ifdef Q_WS_WIN
//        QList<DWORD> relatedProcessIds = PlatformSpecificProgramIdentifierHelper::getAllProcessIdsByExePath();
        if(!result.isEmpty()) {
            for(int i = 0; i < result.size(); i++) {
                Win_RunningExecutableData *win_runningExeData = dynamic_cast<Win_RunningExecutableData *>(result[i]);
                if(win_runningExeData == NULL) {
                    WLog("Something very very bad happened!! Cannot cast.");
                }
                else {
                    // delete it
                    delete win_runningExeData;
                }
            }
        }
        else {
            WLog("Something bad happened!! The result list is empty (should not in this case)");
//            return;
        }
#endif

        Q_EMIT applicationHasARunningCopy();
    } else {
        DLog(" -- no running app found, app install can start");
        this->startAppInstall();
    }
}
