#include "appinstallmanager.h"

#include "../EasyAppsClient/Common/Communication/localmessagesender.h"
#include "../EasyAppsClient/Common/Communication/localmessagecodesenum.h"
#include "./AppInstaller/applicationinstaller.h"
#include "../EasyAppsClient/Common/Helpers/pathhelper.h"
#include "../EasyAppsClient/Application/appspecificpathhelper.h"

#include <QtCore/QCoreApplication>
#include <QTimer>

// utility
#include "../EasyAppsClient/Common/Database/simplelogdatabasemanager.h"
#include "../EasyAppsClient/Common/Json/json.h"

// config
#include "updaterconfigs.h"
#define STILL_PROCESSING_MESSAGE_INTERVALL_MILLISECONDS 5000
#define IPC_COMMUNICATION_ERROR_MAX_COUNT_BEFORE_ABORT 5

AppInstallManager::AppInstallManager(QObject *parent) :
    QObject(parent),
    _stillProcessingMessageWasSent(false),
    _remainingMaxIpcErrorsBeforeAbort(IPC_COMMUNICATION_ERROR_MAX_COUNT_BEFORE_ABORT)
{
    this->_localMessageSender = new LocalMessageSender(AppSpecificPathHelper::getLocalRootDirectory() + "/AppWhirrInstaller");
    connect(_localMessageSender, SIGNAL(finishedWithSuccess()), this, SLOT(localInterProcessCommunicationFinished()));
    connect(_localMessageSender, SIGNAL(failed(QString)), this, SLOT(localInterProcessCommunicationFailedWithError(QString)));

    applicationInstaller = new ApplicationInstaller;

    connect(applicationInstaller, SIGNAL(applicationHasARunningCopy()), this, SLOT(hasARunningCopy()));
    qRegisterMetaType<AppInstallStates::AppInstallStatesEnum>("AppInstallStates::AppInstallStatesEnum");
    connect(applicationInstaller, SIGNAL(installProgressState(AppInstallStates::AppInstallStatesEnum)), this, SLOT(_installProgressState(AppInstallStates::AppInstallStatesEnum)));
    connect(applicationInstaller, SIGNAL(applicationInstallationFailed(QString,ApplicationInstaller*)), this, SLOT(installationFailed(QString,ApplicationInstaller*)));
    connect(applicationInstaller, SIGNAL(applicationInstallationSucceeded(ApplicationInstaller*)), this, SLOT(installationSucceeded(ApplicationInstaller*)));

    _stillProcessingTimer = new QTimer(this);
    connect(_stillProcessingTimer, SIGNAL(timeout()), this, SLOT(_sendStillProcessingMessage()));
    _stillProcessingTimer->setInterval(STILL_PROCESSING_MESSAGE_INTERVALL_MILLISECONDS);
    _stillProcessingTimer->setSingleShot(false);
}

void AppInstallManager::startInstallZip(const QString& zipPath, QString appId)
{
    this->zipPath = zipPath;
    this->appId = appId;

    DLog("AppWhirrU: zip path") << zipPath;
    SimpleLogDatabaseManager::writeToLog(APP_INSTALL_LOG_FILE_NAME, QString("Updater started for zip: %1").arg(zipPath));
    applicationInstaller->setApplicationZipFileNameAndAppId(zipPath, appId);
    applicationInstaller->startInstall();

    // and start the response timer
    _stillProcessingTimer->start();
}

void AppInstallManager::_sendStillProcessingMessage() {
    if(this->_localMessageSender) {
        QByteArray msg;
        msg.append( (char)STILL_PROCESSING );
        _stillProcessingMessageWasSent = true;
        this->_localMessageSender->sendSingleMessage(msg);
    }
}

void AppInstallManager::installationFailed(QString errorMessage, ApplicationInstaller* sender) {
    DLogS << "Install failed" << errorMessage;
//    SimpleLogDatabaseManager::writeToLog(APP_INSTALL_LOG_FILE_NAME, QString("Installation failed with error: %1 | for zip: %2").arg(errorMessage).arg(this->zipPath));

    AW_QOBJECT_SAFE_DELETE(applicationInstaller);

//    localMessageSender = new LocalMessageSender(PathManager::getLocalRootDirectory() + "/AppWhirrInstaller");
//    connect(localMessageSender, SIGNAL(finished()), this, SLOT(communicationFinished()));
//    connect(localMessageSender, SIGNAL(failed(QString)), this, SLOT(communicationFailed(QString)));

    if(this->_localMessageSender) {
        QByteArray msg;
        msg.append( (char)FAILED_TO_INSTALL_APPLICATION );
        _localMessageSender->sendSingleMessage(msg);
    }
}

void AppInstallManager::installationSucceeded(ApplicationInstaller* sender) {
    AW_QOBJECT_SAFE_DELETE(applicationInstaller);

//    SimpleLogDatabaseManager::writeToLog(APP_INSTALL_LOG_FILE_NAME, QString("Installation succeeded for zip: %1").arg(this->zipPath));

//    localMessageSender = new LocalMessageSender(PathManager::getLocalRootDirectory() + "/AppWhirrInstaller");
//    connect(localMessageSender, SIGNAL(finished()), this, SLOT(communicationFinished()));
//    connect(localMessageSender, SIGNAL(failed(QString)), this, SLOT(communicationFailed(QString)));

    if(this->_localMessageSender) {
        QByteArray msg;
        msg.append( (char)FINISHED_INSTALLATION );
        _localMessageSender->sendSingleMessage(msg);
    }
}

void AppInstallManager::hasARunningCopy() {
    AW_QOBJECT_SAFE_DELETE(applicationInstaller);

//    localMessageSender = new LocalMessageSender(PathManager::getLocalRootDirectory() + "/AppWhirrInstaller");
//    connect(localMessageSender, SIGNAL(finished()), this, SLOT(communicationFinished()));
//    connect(localMessageSender, SIGNAL(failed(QString)), this, SLOT(communicationFailed(QString)));

    if(this->_localMessageSender) {
        QByteArray msg;
        msg.append( (char)RUNNING_COPY_DETECTED );
        _localMessageSender->sendSingleMessage(msg);
    }
}

void AppInstallManager::_installProgressState(AppInstallStates::AppInstallStatesEnum installState)
{
    if(this->_localMessageSender) {
        QByteArray msg;
        msg.append( (char)JSON_MESSAGE );
        QVariantMap jsonMessageContent;
        jsonMessageContent["type"] = "AppInstallStateChanged";
        jsonMessageContent["data"] = QString::number(installState);
        msg.append(QtJson::Json::serialize(jsonMessageContent));

        DLog("Progress state IPC msg: ") << msg;
//        _stillProcessingMessageWasSent = true;
//        _localMessageSender->sendSingleMessage(msg);
    }
}

void AppInstallManager::localInterProcessCommunicationFinished() {

    if(_stillProcessingMessageWasSent) {
        // do nothing, it was only a still-processing message
        _stillProcessingMessageWasSent = false;
        return;
    }

    DLog("IPC comm finished.");

    // stop the still-processing status timer
    this->_stillProcessingTimer->stop();

    AW_QOBJECT_SAFE_DELETE(_localMessageSender);

    Q_EMIT this->finished();
}

void AppInstallManager::localInterProcessCommunicationFailedWithError(QString message) {

    SimpleLogDatabaseManager::writeToLog(APP_INSTALL_LOG_FILE_NAME, QString("Local IPC failed with message: %1 | while processing zip: %2").arg(message).arg(this->zipPath));

    if(_stillProcessingMessageWasSent)
    {
        if( _remainingMaxIpcErrorsBeforeAbort > 0 )
        {
            --_remainingMaxIpcErrorsBeforeAbort;
            // do nothing, it was only a still-processing message
            _stillProcessingMessageWasSent = false;
            DLog("StillProcessing - Failed to send IPC message. Error: ") << message;
            return;
        }
    }

    WLog("Failed to send IPC message. Error: ") << message << " | zip: " << this->zipPath;

    // stop the still-processing status timer
    this->_stillProcessingTimer->stop();

    AW_QOBJECT_SAFE_DELETE(_localMessageSender);

    Q_EMIT failed(); // unnecessary

    WLog("Not the perfect solution, but QCoreApplication does not have an exit slot, only a quit, which does not handle return parameter.");
    QCoreApplication::exit(2);
}

