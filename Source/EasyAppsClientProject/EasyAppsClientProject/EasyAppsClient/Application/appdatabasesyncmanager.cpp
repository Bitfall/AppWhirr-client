#include "appdatabasesyncmanager.h"

// network
#include "Common/Communication/basenetworkentity.h"
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
#include "Application/Communication/Request/applistversioninfonetworkrequest.h"

// sync performer
#include <QThreadPool>
#include "Browser/BackgroundWorkers/background_grabsyncperformer.h"

//
#include "Application/globalobjectregistry.h"
#include "Application/CriticalAction/criticalactionmanager.h"
#include "Application/CriticalAction/criticalactioncodes.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"

// notification
#include "Browser/Notification/notificationmanager.h"
#include "Browser/Notification/simpleselftimeoutwithtextnotificationwidget.h"
#include "Browser/Notification/notificationwidget.h"

// utility
#include <QTimer>
#include <QDateTime>

#include "Common/Security/authorization.h"

// config
#include "Settings/configmanager.h"

AppDatabaseSyncManager::AppDatabaseSyncManager(QObject *parent) :
    QObject(parent),
    _grabbedAppsCommunicationDescriptionGateway(NULL),
    _offlineGrabbedAppUpdateCheckCommunicationDescriptionGateway(NULL),
    _currentUserLastSyncTimestamp(0)
{
    _syncAutomaticallyTimer = new QTimer(this);
    _syncAutomaticallyTimer->setInterval(ConfigManager::getAppDatabaseSyncIntervalMilliseconds());
    connect(_syncAutomaticallyTimer, SIGNAL(timeout()), this, SLOT(__startSyncBestFit()));
}

void AppDatabaseSyncManager::switchToSyncFrequentlyMode()
{
    _syncAutomaticallyTimer->start();
}

// -----------------------------------
// --- current user grab-list-sync ---

void AppDatabaseSyncManager::startCurrentUserGrabSync(bool isForceSync)
{
    quint64 currTimestamp = QDateTime::currentMSecsSinceEpoch();

    if( !isForceSync ) {
        quint64 timeDiff = qMax(currTimestamp, _currentUserLastSyncTimestamp) - qMin(currTimestamp, _currentUserLastSyncTimestamp);
        if( timeDiff < ConfigManager::getUserAppDataSyncIntervalMilliseconds() ) {
            // no sync required
            return;
        }
    }

    // sync required
    _currentUserLastSyncTimestamp = currTimestamp;

    // notification
    NotificationManager *notiManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
    NotificationWidget *grabsyncNoti = notiManager->getNotificationWidgetByItemId("AppDatabaseSyncManager::startCurrentUserGrabSync");
    if(grabsyncNoti == NULL) {
        SimpleSelfTimeoutWithTextNotificationWidget *grabsyncNotiContent = new SimpleSelfTimeoutWithTextNotificationWidget(tr("Synchronizing your apps..."));
        notiManager->addNotificationWithSelfTimeoutingCustomWidget(grabsyncNotiContent, "AppDatabaseSyncManager::startCurrentUserGrabSync", "AppDatabaseSyncManager::startCurrentUserGrabSync");
    }
    else {
        WLog("Notification already added!");
    }

    // start the communication
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->addCriticalAction(AppWhirr::CriticalActionCodes::UserGrabSyncActive);

    // user-grabbed apps list communication
    if(_grabbedAppsCommunicationDescriptionGateway != NULL) {
        AW_QOBJECT_SAFE_DELETE(_grabbedAppsCommunicationDescriptionGateway);
    }

    this->_grabbedAppsCommunicationDescriptionGateway = new CommunicationDescriptionGateway(this);
    connect(this->_grabbedAppsCommunicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(grabbedAppsListRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(this->_grabbedAppsCommunicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(grabbedAppsListRequestFailedWithErrorMessage(QString)));
    connect(this->_grabbedAppsCommunicationDescriptionGateway, SIGNAL(finishedCompletely()), this, SLOT(_grabbedAppsCommunicationFinishedCompletely()));


    this->_grabbedAppsCommunicationDescriptionGateway->requestForUserGrabList();
}

// SLOT
void AppDatabaseSyncManager::grabbedAppsListRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> resultList)
{
    if(resultList.isEmpty()) {
        DLog("User-grabbed apps list request finished, but 0 items found.");

        // notification
        NotificationManager *notiManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
        NotificationWidget *grabsyncNoti = notiManager->getNotificationWidgetByItemId("AppDatabaseSyncManager::startCurrentUserGrabSync");
        if(grabsyncNoti != NULL) {
            SimpleSelfTimeoutWithTextNotificationWidget *grabsyncNotiContent = dynamic_cast<SimpleSelfTimeoutWithTextNotificationWidget *>( grabsyncNoti->getContentWidget() );
            if(grabsyncNotiContent != NULL) {
                grabsyncNotiContent->setFinished();
            }
            else {
                WLog("Cannot cast!");
            }
        }
        else {
            WLog("Cannot find the related notification!");
        }
        return;
    }

    // add a new critical-action (increment reference count) - for the time of retrieved grab-sync
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->addCriticalAction(AppWhirr::CriticalActionCodes::UserGrabSyncActive);
    Background_GrabSyncPerformer *worker = new Background_GrabSyncPerformer(resultList);
    connect(worker, SIGNAL(finished()), this, SLOT(_grabbedAppsListSyncFinished()));
    QThreadPool::globalInstance()->start(worker);
}

// SLOT
void AppDatabaseSyncManager::_grabbedAppsListSyncFinished()
{
    // finished with background synchronizing
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->removeCriticalAction(AppWhirr::CriticalActionCodes::UserGrabSyncActive);
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->isAnyCriticalActionActive();

    // notification
    NotificationManager *notiManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
    NotificationWidget *grabsyncNoti = notiManager->getNotificationWidgetByItemId("AppDatabaseSyncManager::startCurrentUserGrabSync");
    if(grabsyncNoti != NULL) {
        SimpleSelfTimeoutWithTextNotificationWidget *grabsyncNotiContent = dynamic_cast<SimpleSelfTimeoutWithTextNotificationWidget *>( grabsyncNoti->getContentWidget() );
        if(grabsyncNotiContent != NULL) {
            grabsyncNotiContent->setText(tr("App synchronization finished!"));
            grabsyncNotiContent->finishAfterTimeout(3000);
        }
        else {
            WLog("Cannot cast!");
        }
    }
    else {
        WLog("Cannot find the related notification!");
    }
}

// SLOT
void AppDatabaseSyncManager::_grabbedAppsCommunicationFinishedCompletely()
{
    // finished with the communication
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->removeCriticalAction(AppWhirr::CriticalActionCodes::UserGrabSyncActive);
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->isAnyCriticalActionActive();
}

// SLOT
void AppDatabaseSyncManager::grabbedAppsListRequestFailedWithErrorMessage(QString errorMessage)
{
    // notification
    NotificationManager *notiManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
    NotificationWidget *grabsyncNoti = notiManager->getNotificationWidgetByItemId("AppDatabaseSyncManager::startCurrentUserGrabSync");
    if(grabsyncNoti != NULL) {
        SimpleSelfTimeoutWithTextNotificationWidget *grabsyncNotiContent = dynamic_cast<SimpleSelfTimeoutWithTextNotificationWidget *>( grabsyncNoti->getContentWidget() );
        if(grabsyncNotiContent != NULL) {
            grabsyncNotiContent->setFinished();
        }
        else {
            WLog("Cannot cast!");
        }
    }
    else {
        WLog("Cannot find the related notification!");
    }

    // error notification
    QString printableErrorMessage = tr("Cannot synchronize your apps. Error message: %1").arg(errorMessage);
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(printableErrorMessage, 5000, NotificationManager::ErrorPriority);
}



// --------------------------
// --- app-list info sync ---

// SLOT
void AppDatabaseSyncManager::startCurrentUserAppListInfoSync()
{
    DLog("startOfflineAppListUpdateCheckForCurrentUser");

    // notification
    NotificationManager *notiManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
    NotificationWidget *grabsyncNoti = notiManager->getNotificationWidgetByItemId("AppDatabaseSyncManager::startCurrentUserAppListInfoSync");
    if(grabsyncNoti == NULL) {
        SimpleSelfTimeoutWithTextNotificationWidget *grabsyncNotiContent = new SimpleSelfTimeoutWithTextNotificationWidget(tr("Checking your apps for updates..."));
        notiManager->addNotificationWithSelfTimeoutingCustomWidget(grabsyncNotiContent, "AppDatabaseSyncManager::startCurrentUserAppListInfoSync", "AppDatabaseSyncManager::startCurrentUserAppListInfoSync");
    }
    else {
        WLog("Notification already added!");
    }

    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager(), SIGNAL(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), this, SLOT(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), Qt::UniqueConnection);

    // first we need to get the apps from the local app database
    //  when it's finished it will call the __doOfflineAppListUpdateCheckForTheseApps which will start the communication
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->getAllAppDescriptions("AppDatabaseSyncManager::startOfflineAppListUpdateCheckForCurrentUser");
}

void AppDatabaseSyncManager::_result_getAllAppDescriptions(QList<ApplicationDescriptionModel> appDescriptions, QString dbOperationId, bool isQuerySuccess)
{
    if(dbOperationId == "AppDatabaseSyncManager::startOfflineAppListUpdateCheckForCurrentUser") {
        if(isQuerySuccess) {
            this->__doOfflineAppListUpdateCheckForTheseApps(appDescriptions);
        }
        else {
            WLog("Failed to get all-apps from database.");

            // notification
            NotificationManager *notiManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
            NotificationWidget *grabsyncNoti = notiManager->getNotificationWidgetByItemId("AppDatabaseSyncManager::startCurrentUserAppListInfoSync");
            if(grabsyncNoti != NULL) {
                SimpleSelfTimeoutWithTextNotificationWidget *grabsyncNotiContent = dynamic_cast<SimpleSelfTimeoutWithTextNotificationWidget *>( grabsyncNoti->getContentWidget() );
                if(grabsyncNotiContent != NULL) {
                    grabsyncNotiContent->setFinished();
                }
                else {
                    WLog("Cannot cast!");
                }
            }
            else {
                WLog("Cannot find the related notification!");
            }
        }

        disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager(), SIGNAL(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), this, SLOT(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)));
    }
}

void AppDatabaseSyncManager::__doOfflineAppListUpdateCheckForTheseApps(QList<ApplicationDescriptionModel> appDescriptions)
{
    DLog("__doOfflineAppListUpdateCheckForTheseApps");

    if(appDescriptions.isEmpty()) {
        DLog("No apps given for app-list-update-check.");

        // notification
        NotificationManager *notiManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
        NotificationWidget *grabsyncNoti = notiManager->getNotificationWidgetByItemId("AppDatabaseSyncManager::startCurrentUserAppListInfoSync");
        if(grabsyncNoti != NULL) {
            SimpleSelfTimeoutWithTextNotificationWidget *grabsyncNotiContent = dynamic_cast<SimpleSelfTimeoutWithTextNotificationWidget *>( grabsyncNoti->getContentWidget() );
            if(grabsyncNotiContent != NULL) {
                grabsyncNotiContent->setFinished();
            }
            else {
                WLog("Cannot cast!");
            }
        }
        else {
            WLog("Cannot find the related notification!");
        }
        return;
    }

    // start the communication
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->addCriticalAction(AppWhirr::CriticalActionCodes::UserOfflineAppListSyncActive);

    if(_offlineGrabbedAppUpdateCheckCommunicationDescriptionGateway != NULL) {
        AW_QOBJECT_SAFE_DELETE(_offlineGrabbedAppUpdateCheckCommunicationDescriptionGateway);
    }

    // offline app list update check apps list communication
//    if(this->_offlineGrabbedAppUpdateCheckCommunicationDescriptionGateway == NULL) {
        this->_offlineGrabbedAppUpdateCheckCommunicationDescriptionGateway = new CommunicationDescriptionGateway(this);
        connect(this->_offlineGrabbedAppUpdateCheckCommunicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(_offlineAppsListUpdateCheckRequestResult(QList<AW::Common::Communication::BaseNetworkEntity*>)));
        connect(this->_offlineGrabbedAppUpdateCheckCommunicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_offlineAppsListUpdateCheckRequestFailedWithErrorMessage(QString)));
        connect(this->_offlineGrabbedAppUpdateCheckCommunicationDescriptionGateway, SIGNAL(finishedCompletely()), this, SLOT(_offlineAppsListUpdateCheckRequestFinished()));
//    }

    QList<QString> appIdListForUpdateCheck;
    for(int i = 0; i < appDescriptions.size(); ++i) {
        appIdListForUpdateCheck << appDescriptions[i].getDynamicDescription_ReadOnly()->getAppId();
    }
    this->_offlineGrabbedAppUpdateCheckCommunicationDescriptionGateway->performThisRequest(new AppListVersionInfoNetworkRequest(appIdListForUpdateCheck));
}

// SLOT
void AppDatabaseSyncManager::_offlineAppsListUpdateCheckRequestResult(QList<AW::Common::Communication::BaseNetworkEntity*> resultList)
{
    if(resultList.isEmpty()) {
        DLog("Offline apps list update check request finished, but 0 items found.");

        // notification
        NotificationManager *notiManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
        NotificationWidget *grabsyncNoti = notiManager->getNotificationWidgetByItemId("AppDatabaseSyncManager::startCurrentUserAppListInfoSync");
        if(grabsyncNoti != NULL) {
            SimpleSelfTimeoutWithTextNotificationWidget *grabsyncNotiContent = dynamic_cast<SimpleSelfTimeoutWithTextNotificationWidget *>( grabsyncNoti->getContentWidget() );
            if(grabsyncNotiContent != NULL) {
                grabsyncNotiContent->setFinished();
            }
            else {
                WLog("Cannot cast!");
            }
        }
        else {
            WLog("Cannot find the related notification!");
        }
        return;
    }

    // add a new critical-action (increment reference count) - for the time of retrieved grab-sync
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->addCriticalAction(AppWhirr::CriticalActionCodes::UserOfflineAppListSyncActive);
    Background_GrabSyncPerformer *worker = new Background_GrabSyncPerformer(resultList);
    connect(worker, SIGNAL(finished()), this, SLOT(_offlineAppsListUpdateCheckSyncFinished()));
    QThreadPool::globalInstance()->start(worker);
}

// SLOT
void AppDatabaseSyncManager::_offlineAppsListUpdateCheckSyncFinished()
{
    // finished with background synchronizing
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->removeCriticalAction(AppWhirr::CriticalActionCodes::UserOfflineAppListSyncActive);
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->isAnyCriticalActionActive();

    // notification
    NotificationManager *notiManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
    NotificationWidget *grabsyncNoti = notiManager->getNotificationWidgetByItemId("AppDatabaseSyncManager::startCurrentUserAppListInfoSync");
    if(grabsyncNoti != NULL) {
        SimpleSelfTimeoutWithTextNotificationWidget *grabsyncNotiContent = dynamic_cast<SimpleSelfTimeoutWithTextNotificationWidget *>( grabsyncNoti->getContentWidget() );
        if(grabsyncNotiContent != NULL) {
            grabsyncNotiContent->setText(tr("App update checking finished!"));
            grabsyncNotiContent->finishAfterTimeout(3000);
        }
        else {
            WLog("Cannot cast!");
        }
    }
    else {
        WLog("Cannot find the related notification!");
    }
}

// SLOT
void AppDatabaseSyncManager::_offlineAppsListUpdateCheckRequestFinished()
{
    // finished with the communication
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->removeCriticalAction(AppWhirr::CriticalActionCodes::UserOfflineAppListSyncActive);
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->isAnyCriticalActionActive();
}

// SLOT
void AppDatabaseSyncManager::_offlineAppsListUpdateCheckRequestFailedWithErrorMessage(QString errorMessage)
{
    // notification
    NotificationManager *notiManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
    NotificationWidget *grabsyncNoti = notiManager->getNotificationWidgetByItemId("AppDatabaseSyncManager::startCurrentUserAppListInfoSync");
    if(grabsyncNoti != NULL) {
        SimpleSelfTimeoutWithTextNotificationWidget *grabsyncNotiContent = dynamic_cast<SimpleSelfTimeoutWithTextNotificationWidget *>( grabsyncNoti->getContentWidget() );
        if(grabsyncNotiContent != NULL) {
            grabsyncNotiContent->setFinished();
        }
        else {
            WLog("Cannot cast!");
        }
    }
    else {
        WLog("Cannot find the related notification!");
    }

    // error notification
    QString printableErrorMessage = tr("Cannot check your apps for updates. Error message: %1").arg(errorMessage);
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(printableErrorMessage, 5000, NotificationManager::ErrorPriority);
}


// ----------------------
void AppDatabaseSyncManager::__startSyncBestFit()
{
    if(Authorization::sharedAuthorization()->isUserLoggedIn()) {
        // start grab-sync
        this->startCurrentUserGrabSync();
    }
    else {
        // start app-list sync
        this->startCurrentUserAppListInfoSync();
    }
}
