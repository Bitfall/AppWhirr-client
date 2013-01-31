#include "background_grabsyncperformer.h"

// global objects
#include "Application/globalobjectregistry.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"

// utility
#include "Common/Helpers/platformflagshelper.h"
#include "Application/Communication/Entity/applicationbaseinfosnetworkentity.h"

Background_GrabSyncPerformer::Background_GrabSyncPerformer(QList<AW::Common::Communication::BaseNetworkEntity*> resultList) :
    _resultList(resultList)
{}

void Background_GrabSyncPerformer::run() {
    DLog("Background grab-sync started");
    LocalAppDatabaseManager *localAppDatabaseManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager();
//    connect(this, SIGNAL(newAppGrabbedSignal(QString,QString,QString)), localAppDatabaseManager, SLOT(_addGrabbedApp(QString,QString,QString)));
//    localAppDatabaseManager->startBatchGrabApplicationAdd();

    int userGrabbedAppsSesultListCnt = _resultList.size();
    DLog("Grabbed list - count: ") << userGrabbedAppsSesultListCnt;
    for(int i = 0; i < userGrabbedAppsSesultListCnt; i++) {
        AW::Common::Communication::BaseNetworkEntity* currUserGrabbedAppEntity = _resultList[i];
        ApplicationBaseInfosNetworkEntity *currCastedUserGrabbedGrabEntity = dynamic_cast<ApplicationBaseInfosNetworkEntity *>(currUserGrabbedAppEntity);

        if(currCastedUserGrabbedGrabEntity == NULL) {
            WLog("Invalid communication entity - cannot cast it!");
        }

        else
        {
            QString appName = currCastedUserGrabbedGrabEntity->getApplicationName();
            QString appId = currCastedUserGrabbedGrabEntity->getApplicationID();
//            QString appSupportedPlatformFlags = PlatformFlagsHelper::_convertIntBasedFlagsToStringBased(currCastedUserGrabbedGrabEntity->getApplicationVersionSummedFlags());
            FLAG_FOR_REVIEW_WITH_HINT("After code revision: it's not a summed flag, it's just an OS flag indicator for the current OS (whether the current platform is supported or not)");
            QString appSupportedPlatformFlags = PlatformFlagsHelper::_convertIntBasedFlagsToStringBased(currCastedUserGrabbedGrabEntity->getApplicationVersionSummedFlags());

            QString appNewestVersionForThisPlatform = currCastedUserGrabbedGrabEntity->getApplicationVersion();
            DLogS << QString("Newest version of app (%1) (%2): %3").arg(appId).arg(appName).arg(appNewestVersionForThisPlatform);
//            DLogS << QString("Found user-grabbed app: %1 [with appId:%2]").arg(appName).arg(appId);

            AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore = currCastedUserGrabbedGrabEntity->getAppStatusInStore();
            DLog("App status in store: ") << appStatusInStore;

//            Q_EMIT newAppGrabbedSignal(appId, appName, appSupportedPlatformFlags);
            localAppDatabaseManager->addOrUpdateGrabbedApp(appId, appName, appSupportedPlatformFlags, appNewestVersionForThisPlatform, appStatusInStore);
        }
    }

//    localAppDatabaseManager->commitBatchGrabApplicationAdd();
//    disconnect(this, SIGNAL(newAppGrabbedSignal(QString,QString,QString)), localAppDatabaseManager, SLOT(_addGrabbedApp(QString,QString,QString)));
    DLog("Background grab-sync ended");

    Q_EMIT finished();
}
