#include "appwhirrupdaterapplication.h"

#include <QThreadPool>
#include <QTimer>
#include <QProcess>
#include <QMetaType>

#include "appwhirrupdatermainwindow.h"
#include "./AppInstallation/appinstallmanager.h"
#include "BackgroundClientInstaller/backgroundclientinstaller.h"

// utility
#include "../EasyAppsClient/Common/Helpers/pathhelper.h"

#include <QFileInfo>

// configurations
#define RETRY_INTERVALL_MILLISECONDS 5000

#ifdef Q_OS_MAC
#   define MAX_RETRY_COUNT 0
#elif defined(Q_OS_WIN)
#   define MAX_RETRY_COUNT 3
#endif

AppWhirrUpdaterApplication::AppWhirrUpdaterApplication(int &argc, char **argv, int qtVersion) :
    QApplication(argc, argv, qtVersion),
    _appInstallManager(NULL),
    _retryClientInstallCountLeft(MAX_RETRY_COUNT)
{
    _updaterMainWindow = new AppWhirrUpdaterMainWindow;
    _updaterMainWindow->setProgress(0);
}

void AppWhirrUpdaterApplication::startAppInstallOrUpdate(QString targetZip, QString appId)
{
    _updaterMainWindow->hide();
    _updaterMainWindow->setStatusText(tr("Installing: %1").arg(QFileInfo(targetZip).baseName()));

    _appInstallManager = new AppInstallManager(this);
    QObject::connect(_appInstallManager, SIGNAL(finished()), _updaterMainWindow, SLOT(close()));

    _appInstallManager->startInstallZip(targetZip, appId);
}

void AppWhirrUpdaterApplication::startClientSelfUpdate(QString clientRootPath)
{
    this->_clientRootPath = clientRootPath;

    this->_updaterMainWindow->show();
    this->_updaterMainWindow->activateWindow();
    this->_updaterMainWindow->setFocus();

    this->_updaterMainWindow->setStatusText(tr("Installing AppWhirr..."));
    DLog("Given client-root-path: ") << clientRootPath;


    _retryClientInstallTimer = new QTimer(this);
    _retryClientInstallTimer->setSingleShot(true);
    _retryClientInstallTimer->setInterval(RETRY_INTERVALL_MILLISECONDS);
    connect(_retryClientInstallTimer, SIGNAL(timeout()), this, SLOT(tryToInstall()));

    _retryClientInstallCountLeft = MAX_RETRY_COUNT;

    this->_fallbackInstallStep = DoFromTheBeginning;
    // do the first try
    this->tryToInstall();
}

void AppWhirrUpdaterApplication::startAppWhirrRestart(QString appWhirrClientExecutablePath, QString appWhirrExecutableWorkingDirPath)
{
    DLog("AppWhirr - client restart. Executable path: ") << appWhirrClientExecutablePath << appWhirrExecutableWorkingDirPath;

    _awRestart_awClientExecutablePath = appWhirrClientExecutablePath;
    _awRestart_awClientExecutableWorkingDirPath = appWhirrExecutableWorkingDirPath;
    QTimer::singleShot(0, this, SLOT(_doAppWhirrRestart()));
}

// SLOT
void AppWhirrUpdaterApplication::tryToInstall()
{
    Background_ClientInstaller* backgroundClientInstaller = new Background_ClientInstaller(this->_clientRootPath, this->_fallbackInstallStep, this);
    qRegisterMetaType<InstallMajorSteps>("InstallMajorSteps");
    connect(backgroundClientInstaller, SIGNAL(succeeded()), this, SLOT(clientInstallFinishedSuccessfully()));
    connect(backgroundClientInstaller, SIGNAL(failedWithErrorMessage(QString,InstallMajorSteps)), this, SLOT(clientInstallFailedWithError(QString,InstallMajorSteps)));

    connect(backgroundClientInstaller, SIGNAL(installPercentage(quint32)), this->_updaterMainWindow, SLOT(setProgress(quint32)));

    QThreadPool::globalInstance()->start(backgroundClientInstaller);
}

void AppWhirrUpdaterApplication::_doAppWhirrRestart()
{
#ifdef Q_WS_WIN
    QStringList processParams;
    processParams;
    if (!QProcess::startDetached(_awRestart_awClientExecutablePath, processParams, _awRestart_awClientExecutableWorkingDirPath)) {
#endif
#ifdef Q_WS_MAC
            QStringList processParams;
            processParams << _awRestart_awClientExecutablePath;
            DLog("InstallerProcessParams: ") << processParams;
            if (!QProcess::startDetached("open", processParams, _awRestart_awClientExecutableWorkingDirPath)) {
#endif
        WLog("Failed to restart AppWhirr, given path: ") << _awRestart_awClientExecutablePath << _awRestart_awClientExecutableWorkingDirPath;
    }

    // close the client
    this->quit();
}

void AppWhirrUpdaterApplication::clientInstallFailedWithError(QString errorMessage, InstallMajorSteps lastStepDone)
{
    this->_fallbackInstallStep = lastStepDone;

    this->_retryClientInstallCountLeft--;
    if(this->_retryClientInstallCountLeft >= 0) {
        this->_updaterMainWindow->setStatusText(tr("Restarting install..."), AppWhirrUpdaterMainWindow::STVT_Error);
        this->_retryClientInstallTimer->start();
        DLog(" ! Restarting install, error: ") << errorMessage << " last step: " << lastStepDone;
    }
    else {
        this->_updaterMainWindow->setStatusText(tr("Install failed!"), AppWhirrUpdaterMainWindow::STVT_Error);
        DLog(" ! Install failed with error: ") << errorMessage;
    }
}

void AppWhirrUpdaterApplication::clientInstallFinishedSuccessfully()
{
    // start the new version
#ifdef Q_WS_WIN
    QString executablePath = PathHelper::combineAndCleanPathes(this->_clientRootPath, "AppWhirr.exe");
#endif

#ifdef Q_WS_MAC
    QString executablePath = PathHelper::combineAndCleanPathes(this->_clientRootPath, "AppWhirr.app");
#endif

    // start the installation
    DLog(" -- Start AppWhirr client");
    this->_updaterMainWindow->setStatusText(tr("Install finished!"));
    this->_updaterMainWindow->setProgress(100);
#ifdef Q_WS_WIN
    if (!QProcess::startDetached(executablePath, QStringList(), this->_clientRootPath)) {
#endif
#ifdef Q_WS_MAC
    if (!QProcess::startDetached("open", QStringList() << executablePath)) {
#endif
        this->_updaterMainWindow->setStatusText(tr("Failed to restart AppWhirr!"), AppWhirrUpdaterMainWindow::STVT_Error);
    }
    else {
        // close it
        this->quit();
    }
}
