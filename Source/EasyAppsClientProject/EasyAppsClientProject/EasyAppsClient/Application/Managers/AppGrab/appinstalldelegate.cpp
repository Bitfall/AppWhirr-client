#include "appinstalldelegate.h"

#include <QLocalServer>
#include <QTimer>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QFileInfo>

// path
#include "Common/Helpers/pathhelper.h"
#include "Application/appspecificpathhelper.h"

// local communication with the installer
#include "Common/Communication/localmessagecodesenum.h"
#include "Common/Json/json.h"
#include "Application/Managers/AppGrab/appinstallstates.h"

// config
#define UPDATER_TIMEOUT_INTERVAL 60000 // 60 seconds

AppInstallDelegate::AppInstallDelegate(QObject *parent) :
    QObject(parent),
    _localServer(NULL),
    _isCurrentlyInstalling(false),
    _currUpdaterMessagingLocalSocket(NULL)
{    
    _localServer = new QLocalServer(this);

    QString listenIdentifier = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalRootDirectory(), "AppWhirrInstaller");

#ifdef Q_WS_MAC
    // On Unix if the server crashes without closing listen will fail with AddressInUseError.
    // To create a new server the file should be removed.
    QFile::remove(listenIdentifier);
#endif

#ifdef Q_WS_WIN
    // Todo:
    // On Windows two local servers can listen to the same pipe at the same time,
    // but any connections will go to one of the servers.
    FLAG_FOR_REVIEW_WITH_HINT("Maybe this is not a problem, on Windows, but review it.");
#endif

    _waitingForUpdaterTimeoutTimer = new QTimer(this);
    _waitingForUpdaterTimeoutTimer->setSingleShot(true);
    _waitingForUpdaterTimeoutTimer->setInterval(UPDATER_TIMEOUT_INTERVAL);
    connect(_waitingForUpdaterTimeoutTimer, SIGNAL(timeout()), this, SLOT(updaterMessageTimeout()));
    _waitingForUpdaterTimeoutTimer->stop();

    connect(_localServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(this, SIGNAL(__tryNextAppInstallFromWaitingQueue()), this, SLOT(_doInstallFromWaitingQueue()));
    if (!_localServer->listen(listenIdentifier)) {
        WLog("Failed to create server for installation");

        _localServer->close();
        _localServer->disconnect();
        AW_QOBJECT_SAFE_DELETE(_localServer);
    }
}

AppInstallDelegate::~AppInstallDelegate()
{
    if (_localServer) {
        _localServer->close();
        _localServer->disconnect();
        AW_QOBJECT_SAFE_DELETE(_localServer);
    }
}

void AppInstallDelegate::installThisApp(GrabApplicationQueueItem grabItem)
{
    _installingAppZipToGrabItemMapper.insert(grabItem._getRelatedDownloadedZip(), grabItem);
    _applicationZipsWaitingForInstallation.append(grabItem._getRelatedDownloadedZip());

    Q_EMIT __tryNextAppInstallFromWaitingQueue();
}

void AppInstallDelegate::_doInstallFromWaitingQueue() {

    DLog("_doInstallFromWaitingQueue");

    if(this->getIsCurrentlyInstalling()) {
        // if installing do nothing
        return;
    } else {
        if(_applicationZipsWaitingForInstallation.isEmpty()) {
            // no waiting applications
            DLog("install called, but there's nothing more to install.");
            this->setIsCurrentlyInstalling(false);
            return;
        }
        this->setIsCurrentlyInstalling(true);
    }

    _currInstallingZipFullPath = this->_applicationZipsWaitingForInstallation.dequeue();
    GrabApplicationQueueItem relatedGrabItem = this->_installingAppZipToGrabItemMapper.value(_currInstallingZipFullPath);

    QString zipFileName = QFileInfo(_currInstallingZipFullPath).fileName();

    // (moved to the part handles download-started event, and fires when download-started));
//    Q_EMIT appInstallationStarted(relatedGrabItem.getAppId(), relatedGrabItem.getAppName());


    // start the installation
    DLog(" -- Install start: App zip (for install) name: ") << zipFileName << " AppName: " << relatedGrabItem.getAppName() << "AppId: " << relatedGrabItem.getAppId() << "SupportedPlatformFlags:" << relatedGrabItem.getSupportedSummedPlatformFlags() << "AppVersion:" << relatedGrabItem.getRelatedAppVersion() << "IsUpdate:" << relatedGrabItem.getIsUpdate() << "Full Path:" << _currInstallingZipFullPath;
    QString appWhirrUpdaterModeCode("1");
#ifdef Q_WS_WIN
    QStringList installerProcessParams;
    installerProcessParams << appWhirrUpdaterModeCode << zipFileName << relatedGrabItem.getAppId();
    DLog("InstallerProcessParams: ") << installerProcessParams;
    if (!QProcess::startDetached(AppSpecificPathHelper::getUpdaterExecutablePath(), installerProcessParams)) {
#endif
#ifdef Q_WS_MAC
    QStringList installerProcessParams;
    installerProcessParams << AppSpecificPathHelper::getUpdaterExecutablePath() << "--args" << appWhirrUpdaterModeCode << zipFileName << relatedGrabItem.getAppId();
    DLog("InstallerProcessParams: ") << installerProcessParams;
    if (!QProcess::startDetached("open", installerProcessParams)) {
#endif
        DLog("Failed to start the installer for zip: " << zipFileName);
        this->setIsCurrentlyInstalling(false);
        if(this->_installingAppZipToGrabItemMapper.contains(_currInstallingZipFullPath)) {
            this->_installingAppZipToGrabItemMapper.remove(_currInstallingZipFullPath);
//            Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Installer cannot be started."));
            Q_EMIT installFailed(relatedGrabItem, tr("Installer cannot be started."));
        }
    }
    else {
        // start install timeout timer
        this->_waitingForUpdaterTimeoutTimer->start();
    }
}

void AppInstallDelegate::updaterMessageTimeout() {
    DLog(" -- Updater timeouted.");
    if(this->_currUpdaterMessagingLocalSocket != NULL) {
        this->_currUpdaterMessagingLocalSocket->abort();
    }

//    this->updaterMessageFailedWithError(QLocalSocket::SocketTimeoutError);
    this->_installFailedWithError(QLocalSocket::SocketTimeoutError, true);
}

void AppInstallDelegate::updaterMessageFailedWithError(QLocalSocket::LocalSocketError errorCode) {
    // ! changed:
    //  currently it won't finish the installation right now, only if the install timeouts (not responst)
    //  this will prevent inter-process-communication errors, unless it's the final, "install finished" message
    //      ( -> in this case it will detect that the install failed)

    this->_installFailedWithError(errorCode, false);
}

void AppInstallDelegate::_installFailedWithError(QLocalSocket::LocalSocketError errorCode, bool isStopInstallingProcessImmediatelly)
{
    // stop the timeout timer
    if(isStopInstallingProcessImmediatelly) {
        this->_waitingForUpdaterTimeoutTimer->stop();
    }

    DLog("_installFailedWithError - Updater message failed: ") << errorCode << isStopInstallingProcessImmediatelly;

    GrabApplicationQueueItem relatedGrabItem = this->_installingAppZipToGrabItemMapper.value(_currInstallingZipFullPath);
    if(isStopInstallingProcessImmediatelly) {
        this->_installingAppZipToGrabItemMapper.remove(_currInstallingZipFullPath);
        this->setIsCurrentlyInstalling(false);
    }

    if(this->_currUpdaterMessagingLocalSocket == NULL) {
        if(isStopInstallingProcessImmediatelly) {
//            Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Internal installer error."));
            Q_EMIT installFailed(relatedGrabItem, tr("Internal installer error - communication with installer interrupted."));
            // start the next waiting install
            this->setIsCurrentlyInstalling(false);
            Q_EMIT __tryNextAppInstallFromWaitingQueue();
        }

        return;
    }
    else {
//        this->_currUpdaterMessagingLocalSocket->abort();
//        this->_currUpdaterMessagingLocalSocket->disconnectFromServer();
//        AW_QOBJECT_SAFE_DELETE(this->_currUpdaterMessagingLocalSocket);

        // will abort / disconnect if isStopImmediatelly is true
//        FLAG_FOR_REVIEW_WITH_HINT("it's probably a connection error, not an updater / installer error, but maybe it would be required - it will do it if ");
    }

    if(isStopInstallingProcessImmediatelly)
    {
        if(this->_currUpdaterMessagingLocalSocket != NULL) {
            this->_currUpdaterMessagingLocalSocket->disconnectFromServer();
            AW_QOBJECT_SAFE_DELETE(this->_currUpdaterMessagingLocalSocket);
        }

        QString errorMessage;
        if(errorCode == QLocalSocket::SocketTimeoutError) {
            errorMessage = tr("Install and update manager not responding.");
        }
        else {
            errorMessage = tr("Install and update manager failed to connect to the client.");
        }
//        Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), errorMessage);
        Q_EMIT installFailed(relatedGrabItem, errorMessage);

        // start the next waiting install
        this->setIsCurrentlyInstalling(false);
        Q_EMIT __tryNextAppInstallFromWaitingQueue();
    }
}

void AppInstallDelegate::updaterMessageReadyToRead() {
    // stop the timeout timer
    this->_waitingForUpdaterTimeoutTimer->stop();

    DLog("Message arrived from updater.");

//    this->updaterMessageFailedWithError(QLocalSocket::UnknownSocketError);
//    return;

    if(!this->_installingAppZipToGrabItemMapper.contains(_currInstallingZipFullPath)) {
        WLog("An error removed this from the map - don't do anything, the required grab-infos cannot be used anymore (removed). BUT check whether everything is OK - after an error there should be no more updater message for the app!!!");
        return;
    }
    GrabApplicationQueueItem relatedGrabItem = this->_installingAppZipToGrabItemMapper.value(_currInstallingZipFullPath);


    QString userLocalAppDirPath = AppSpecificPathHelper::getCurrentUserLocalAppFolderPath();
    if(userLocalAppDirPath.isEmpty()) {
//        Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Install failed. No user selected, or the user's folder is unaccessable."));
        Q_EMIT installFailed(relatedGrabItem, tr("Install failed. No user selected, or the user's folder is unaccessable."));

        // start the next waiting install
        this->setIsCurrentlyInstalling(false);
        Q_EMIT __tryNextAppInstallFromWaitingQueue();

        return;
    }

    QString relatedAppInstalledRootPath = PathHelper::combineAndCleanPathes(userLocalAppDirPath, relatedGrabItem.getAppId());
//    LOG_AS_NOT_IMPLEMENTED_WITH_HINT("We should know exactly the installed-app's local root directory. Currently this is only a strong guess, which maches if the ZIP's name is the same as the app-dir in the zip.");

    if(this->_currUpdaterMessagingLocalSocket == NULL) {
//        Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Internal installer error."));
        Q_EMIT installFailed(relatedGrabItem, tr("Internal installer error - communication with installer interrupted."));

        // start the next waiting install
        this->setIsCurrentlyInstalling(false);
        Q_EMIT __tryNextAppInstallFromWaitingQueue();

        return;
    }
    bool isContinueThisInstallProcess = false;

    QByteArray messageBytes = this->_currUpdaterMessagingLocalSocket->readAll();
    DLog("IPC msg: ") << messageBytes;

    // Todo: implement error codes
    //       do what the error codes tells you:
    //          * app is running - close it warning
    //          * failed error
    //          * etc
    char resultCode = 0;
    if(!messageBytes.isEmpty()) {
        resultCode = messageBytes.at(0);
    }

    if(resultCode == FINISHED_INSTALLATION) {
        DLog("Installation suceeded");
        Q_EMIT appInstalledSuccessfullyToDirectory(relatedGrabItem, relatedAppInstalledRootPath);
    }
    else if(resultCode == JSON_MESSAGE) {
        QVariant jsonMsgVar = QtJson::Json::parse(messageBytes.mid(1)); // skip the first char (the type-id-char)
        QVariantMap jsonMsgVarMap = jsonMsgVar.toMap();
        QString jsonMsgType = jsonMsgVarMap.value("type").toString();
        if(jsonMsgType == "AppInstallStateChanged") {
            int jsonMsgData = jsonMsgVarMap.value("data").toString().toInt();
            QString printableInstallState;
            if(jsonMsgData == AppInstallStates::PreparingBackup) {
                printableInstallState = tr("Preparing backup");
            }
            else if(jsonMsgData == AppInstallStates::PerformingBackup) {
                printableInstallState = tr("Performing backup");
            }
            else if(jsonMsgData == AppInstallStates::PreparingAppDirectory) {
                printableInstallState = tr("Preparing app directory");
            }
            else if(jsonMsgData == AppInstallStates::PerformingInstall) {
                printableInstallState = tr("Installing the app");
            }
            else if(jsonMsgData == AppInstallStates::AfterInstallCleanup) {
                printableInstallState = tr("Performing cleanup");
            }
            else if(jsonMsgData == AppInstallStates::ValidatingAppInstall) {
                printableInstallState = tr("Validating app install");
            }

            if(printableInstallState.isEmpty()) {
                WLog("Invalid install-state");
            }
            else {
                Q_EMIT appInstallProgressStateMessage(relatedGrabItem, printableInstallState);
            }
        }
        else {
            WLog("Invalid JSON msg 'type'");
        }
        this->_waitingForUpdaterTimeoutTimer->start(); // restarts the timer
        isContinueThisInstallProcess = true;
    }
    else if(resultCode == STILL_PROCESSING) {
        // still processing - don't do anything, but restart the timeout timer
        this->_waitingForUpdaterTimeoutTimer->start(); // restarts the timer
        DLog("Still installing - status message.");
        isContinueThisInstallProcess = true;
//        return;
    }
    else {
        DLog("Installation failed with error code: " << QString::number(resultCode));
        bool isFailImmediatelly = true;
        QString errorString;
        switch(resultCode)
        {
//        case 0:
//#ifdef ENABLE_APPWHIRR_DEBUG_LOGGING
//            WLog("Error code 0 - hopefully it's not critical but you should investigate this if you see it. Probably it's an IPC error.");
//#endif
//            isFailImmediatelly = false;
//            isContinueThisInstallProcess = true;
//            break;
        case FAILED_TO_INSTALL_APPLICATION:
            errorString = tr("Internal application install error.");
            break;
        case RUNNING_COPY_DETECTED:
            //                errorString = tr("Application is running. Close it.");

            DLog("-- Running copy detected - related app infos; ") << relatedGrabItem.getAppId() << relatedGrabItem.getAppName() << relatedGrabItem.getIsUpdate() << relatedGrabItem.getSupportedSummedPlatformFlags();
            // put it to a waiting-for-resolution queue
            _installWaitsForResolutionMap[relatedGrabItem.getAppId()] = relatedGrabItem;
            // and emit the resolution-needed signal, and wait for the response
            Q_EMIT __resolutionNeededForRunningApp(relatedGrabItem.getAppId(), relatedGrabItem.getAppName());
            isFailImmediatelly = false;
            isContinueThisInstallProcess = false;
            break;
        default:
            errorString = tr("Unknown error: %1").arg(QString::number(resultCode));
        }

        if(isFailImmediatelly) {
//            Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), errorString);
            Q_EMIT installFailed(relatedGrabItem, errorString);
        }
    }


//    if(this->_currUpdaterMessagingLocalSocket == NULL) {
//        Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Internal installer error. Not good..."));
////        return;
//    } else {
////        this->_currUpdaterMessagingLocalSocket->abort();
//        this->_currUpdaterMessagingLocalSocket->disconnectFromServer();
//        AW_QOBJECT_SAFE_DELETE(this->_currUpdaterMessagingLocalSocket);
//    }

    this->_currUpdaterMessagingLocalSocket->disconnectFromServer();
    AW_QOBJECT_SAFE_DELETE(this->_currUpdaterMessagingLocalSocket);

    if(!isContinueThisInstallProcess)
    {
        this->_installingAppZipToGrabItemMapper.remove(_currInstallingZipFullPath);

        // start the next waiting install
        this->setIsCurrentlyInstalling(false);
        Q_EMIT __tryNextAppInstallFromWaitingQueue();
    }
}

void AppInstallDelegate::newConnection() {

    DLog(" -- New connection");
//    while (localServer->hasPendingConnections()) {

    this->_currUpdaterMessagingLocalSocket = _localServer->nextPendingConnection();
    if(this->_currUpdaterMessagingLocalSocket != NULL) {
        connect(this->_currUpdaterMessagingLocalSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(updaterMessageFailedWithError(QLocalSocket::LocalSocketError)));
        connect(this->_currUpdaterMessagingLocalSocket, SIGNAL(readyRead()), this, SLOT(updaterMessageReadyToRead()));
    }

//        if (!localSocket->waitForReadyRead(100000)) {

//        }

}


// --- install error resolution required
// if install hangs because of resolution needed you have to communicate with these signals/slots
//  resolution may be required if the app is still running and the install cannot replace it with a newer version

void AppInstallDelegate::__resolutionDoneRetryInstall(QString appId)
{
    if(!this->_installWaitsForResolutionMap.contains(appId)) {
        WLog("Cannot find the given app-id in the resolution-needed map.");
        return;
    }

    // put back to install queue
    GrabApplicationQueueItem relatedGrabItem = this->_installWaitsForResolutionMap.value(appId);
    this->_installWaitsForResolutionMap.remove(appId);
    this->installThisApp(relatedGrabItem);
//    Q_EMIT __tryNextAppInstallFromWaitingQueue();
}

void AppInstallDelegate::__resolutionFailed(QString appId)
{
    if(!this->_installWaitsForResolutionMap.contains(appId)) {
        WLog("Cannot find the given app-id in the resolution-needed map.");
        return;
    }

    GrabApplicationQueueItem relatedGrabItem = this->_installWaitsForResolutionMap.value(appId);
//    Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Cannot close the running application."));
    Q_EMIT installFailed(relatedGrabItem, tr("Cannot close the running application."));
    this->_installWaitsForResolutionMap.remove(appId);
}

bool AppInstallDelegate::getIsCurrentlyInstalling()
{
    _isCurrentlyInstallingMutex.lock();
    bool ret = this->_isCurrentlyInstalling;
    _isCurrentlyInstallingMutex.unlock();

    return ret;
}

void AppInstallDelegate::setIsCurrentlyInstalling(bool value)
{
    _isCurrentlyInstallingMutex.lock();
    this->_isCurrentlyInstalling = value;
    _isCurrentlyInstallingMutex.unlock();
}
