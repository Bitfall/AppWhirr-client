#include "appgrabdelegate.h"

#include "Application/Communication/Entity/grabapplicationnetworkentity.h"

// auth
#include "Common/Security/authorization.h"
// communication
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
#include "Application/Communication/Request/grabapplicationnetworkrequest.h"

//
#include <QTimer>

//
//#include "Common/System/systemprofiler.h"
//#include "Common/Helpers/platformflagshelper.h"

// config
#include "Settings/configmanager.h"



AppGrabDelegate::AppGrabDelegate(QObject *parent) :
    QObject(parent),
    _appGrabCommunicationDescriptionGateway(NULL),
    _isShouldForceAuthorizeGrabRequest(true)
{
    connect(this, SIGNAL(__tryToStartNextPendingApplicationGrabDescriptionRequest()), this, SLOT(tryToStartNextPendingApplicationGrabDescriptionRequest()));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginAborted()), this, SLOT(authenticationChallengeCancelled()));

    _authorizationTimeoutTimer = new QTimer(this);
    _authorizationTimeoutTimer->setInterval(ConfigManager::getUserGrabAppAuthorizationTimeoutIntervalMilliseconds());
    _authorizationTimeoutTimer->setSingleShot(true);
    connect(_authorizationTimeoutTimer, SIGNAL(timeout()), this, SLOT(authorizationTimeout()));
}

AppGrabDelegate::~AppGrabDelegate() {
    AW_QOBJECT_SAFE_DELETE(_appGrabCommunicationDescriptionGateway);
}

// ------------------------
// --- public interface ---
void AppGrabDelegate::grabThisItem(GrabApplicationQueueItem grabItem)
{
    _grabItemsWaitingForGrab.append(grabItem);

    Q_EMIT __tryToStartNextPendingApplicationGrabDescriptionRequest();
}


void AppGrabDelegate::tryToStartNextPendingApplicationGrabDescriptionRequest() {

    DLog("--tryToStartNextPendingApplicationGrabDescriptionRequest");

    if (_grabItemsWaitingForGrab.isEmpty()) {
        DLog("Grab queue is empty.");
        return;
    }


    // !block: only one description-request is allowed at a time, the next will start when the current one finishes
    if (!_appGrabCommunicationDescriptionGateway)
    {
        GrabApplicationQueueItem currentGrabItem = _grabItemsWaitingForGrab.first();
        if (currentGrabItem.getAppId().isEmpty()) {
            WLog("The given app id is empty. Cannot continue the download.");
            return;
        }

        // disconnect from authorization
        disconnect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(tryToStartNextPendingApplicationGrabDescriptionRequest()));

        // it's free to communicate, so let's do it
        _appGrabCommunicationDescriptionGateway = new CommunicationDescriptionGateway;

        connect(_appGrabCommunicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(appGrabDescriptionRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
        connect(_appGrabCommunicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(failedToGetAppGrabDescription(QString)));
        connect(_appGrabCommunicationDescriptionGateway, SIGNAL(authenticationChallengeHappened()), this, SLOT(_authenticationChallangeHappened_PutBackToQueue()));

        CommunicationDescriptionGateway::GrabRequestType grabType = CommunicationDescriptionGateway::GRT_Install;
        if(currentGrabItem.getIsUpdate()) {
            grabType = CommunicationDescriptionGateway::GRT_Update;
        }

        Q_EMIT grabStarted(currentGrabItem);

        AW::Client::Communication::AWBaseNetworkRequest::RequestSecurityTypeEnum requestSecurityType = AW::Client::Communication::AWBaseNetworkRequest::RST_AuthorizationRequired;
        if(_isShouldForceAuthorizeGrabRequest) {
            _isShouldForceAuthorizeGrabRequest = false;
            _authorizationTimeoutTimer->start();

            requestSecurityType = AW::Client::Communication::AWBaseNetworkRequest::RST_ForceUserLogin;
        }
//        _appGrabCommunicationDescriptionGateway->requestForGrabbingApplicationByAppId(currentGrabItem.getAppId(), grabType);
        int grabTypeInt = (int)grabType;
        _appGrabCommunicationDescriptionGateway->performThisRequest(
                    new GrabApplicationNetworkRequest(currentGrabItem.getAppId(),
                                                      QString::number(grabTypeInt),
                                                      requestSecurityType
                                                      ));
    }
}

void AppGrabDelegate::_authenticationChallangeHappened_PutBackToQueue()
{
    DLog("--_authenticationChallangeHappened_PutBackToQueue");
    AW_QOBJECT_SAFE_DELETE(_appGrabCommunicationDescriptionGateway);

    connect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(tryToStartNextPendingApplicationGrabDescriptionRequest()));
    Authorization::sharedAuthorization()->showLoginDialogAndStartLogin();
}

void AppGrabDelegate::authenticationChallengeCancelled() {
    _isShouldForceAuthorizeGrabRequest = true;

    if(_grabItemsWaitingForGrab.isEmpty()) {
        DLog("No pending grab requests.");
        return;
    }

    GrabApplicationQueueItem item;
    Q_FOREACH(item, _grabItemsWaitingForGrab) {
        Q_EMIT grabFailed(item, "Authentication error");
    }

    _grabItemsWaitingForGrab.clear();
    AW_QOBJECT_SAFE_DELETE(_appGrabCommunicationDescriptionGateway);
}

void AppGrabDelegate::appGrabDescriptionRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity *> entities)
{
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("networkRequestFinished");
#endif

    GrabApplicationQueueItem relatedGrabItem;
    QString supportedPlatformFlags;

    AW_QOBJECT_SAFE_DELETE(_appGrabCommunicationDescriptionGateway);
    if (!_grabItemsWaitingForGrab.isEmpty()) {
        // finished - remove from the queue
        relatedGrabItem = _grabItemsWaitingForGrab.dequeue();
        // start the next grab request
        Q_EMIT __tryToStartNextPendingApplicationGrabDescriptionRequest();
    } else {
        WLog("No item in waiting-for-grab queue. This should never happen here!");
        Q_EMIT grabFailed(relatedGrabItem, tr("Internal error: grab is interrupted internally."));
        return;
    }

    // If there is no entity to iterate on...
    if (entities.isEmpty()) {
        WLog("No zipped application returned from server.");
        Q_EMIT grabFailed(relatedGrabItem, tr("No appropriate version found for you're Operating System. Sorry."));
//        Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("No appropriate version found for you're Operating System. Sorry."));

        return;
    }

    else {
        // Find the best fit for the uuuuuser ( Thron Legacy;) )
//        unsigned int currentPlatformFlags = SystemProfiler::sharedSystemProfiler()->_getSystemFlagsAsUInt();
        bool isCompatibleVersionFound = false;
        QUrl appDownloadUrl;
        for(int i = 0; i < entities.size() && !isCompatibleVersionFound; i++) {
            AW::Common::Communication::BaseNetworkEntity *currEntity = entities[i];
            GrabApplicationNetworkEntity *currCastedGrabEntity = dynamic_cast<GrabApplicationNetworkEntity *>(currEntity);

            if(currCastedGrabEntity == NULL) {
                WLog("Invalid communication entity - cannot cast it!");
            }

            else
            {
//                DLog(" - zip version: ") << currCastedGrabEntity->getZippedApplicationFlag();

//                if(PlatformFlagsHelper::_isTheseFlagsAreCompatibleIntVersion(currentPlatformFlags, currCastedGrabEntity->getZippedApplicationFlag()))
//                {
                    // this is a compatible version, pick this!

                    // map the url to the related appId
                relatedGrabItem._setRelatedAppVersion(currCastedGrabEntity->getRelatedAppVersion());

//                    _downloadingResourceMapper.insert(currCastedGrabEntity->getZippedApplicationURL(), relatedGrabItem);
//                    tryToStartDownloadingApplication(currCastedGrabEntity->getZippedApplicationURL());
                appDownloadUrl = currCastedGrabEntity->getZippedApplicationURL();

                    // and stop processing others
                    isCompatibleVersionFound = true;

//                    DLog(" - Accepted zip version: ") << currCastedGrabEntity->getZippedApplicationFlag();
//                    supportedPlatformFlags = PlatformFlagsHelper::_convertIntBasedFlagsToStringBased(currCastedGrabEntity->getZippedApplicationFlag());
                    FLAG_FOR_REVIEW_WITH_HINT("Code revision: if no compatible version found for the current platform then this code won't be executed. So supportedPlatformFlags is not requied.");
                    supportedPlatformFlags = 1;
//                }
            }
        }

        if(!isCompatibleVersionFound) {
//            Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("No appropriate version found for you're Operating System. Sorry."));
            Q_EMIT grabFailed(relatedGrabItem, tr("No appropriate version found for you're Operating System. Sorry."));
        }
        else {
//            Q_EMIT appGrabbedSuccessfully(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getSupportedSummedPlatformFlags(), relatedGrabItem.getRelatedAppVersion(), relatedGrabItem.getAppStatusInStore());
            Q_EMIT grabSucceeded(relatedGrabItem, appDownloadUrl);
        }
    }

//    Q_EMIT appGrabbedSuccessfully(relatedGrabItem.getAppId(), relatedGrabItem.getAppName());
}

void AppGrabDelegate::failedToGetAppGrabDescription(QString errorMessage)
{
    WLog(errorMessage);
    _isShouldForceAuthorizeGrabRequest = true;

    GrabApplicationQueueItem relatedGrabItem;

    AW_QOBJECT_SAFE_DELETE(_appGrabCommunicationDescriptionGateway);
    if (!_grabItemsWaitingForGrab.isEmpty()) {
        // finished
        relatedGrabItem = _grabItemsWaitingForGrab.first();
        _grabItemsWaitingForGrab.removeFirst();

        // and start the next grab request
        Q_EMIT __tryToStartNextPendingApplicationGrabDescriptionRequest();
    }

//    Q_EMIT appGrabFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Cannot get required informations. Sorry. It's probably a network error."));
    Q_EMIT grabFailed(relatedGrabItem, tr("Cannot get required app information. Sorry. It's probably a network error."));
}

void AppGrabDelegate::authorizationTimeout()
{
    _isShouldForceAuthorizeGrabRequest = true;
}
