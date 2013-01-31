#include "communicationdescriptiongateway.h"

#include <QThreadPool>
#include <QMutex>

// request
#include "Application/Communication/Request/applicationnetworkrequest.h"
#include "Application/Communication/Request/grabapplicationnetworkrequest.h"
#include "Application/Communication/Request/grablistnetworkrequest.h"
#include "Application/Communication/Request/userloginnetworkrequest.h"
#include "Application/Communication/Request/clientupdatenetworkrequest.h"
#include "Application/Communication/Request/grabclientupdatenetworkrequest.h"
#include "Application/Communication/Request/userfeedbackrequest.h"
#include "Application/Communication/Request/usersuggestapprequest.h"

// response
#include "Common/Communication/basenetworkresponse.h"

// entity
#include "Common/Communication/basenetworkentity.h"

#include "Common/Security/authorization.h"

using namespace AW::Common::Communication;

CommunicationDescriptionGateway::CommunicationDescriptionGateway(QObject *parent) :
    QObject(parent),
    networkRequest(NULL),
    networkResponse(NULL),
    _isRequestActive(false),
    _finishedSteps(0)
{
}

CommunicationDescriptionGateway::~CommunicationDescriptionGateway() {
    AW_QOBJECT_SAFE_DELETE(networkRequest);
}

// Requests seen from outside...
//void CommunicationDescriptionGateway::requestForInitialPageDescription() {
//    this->performThisRequest(new ApplicationListNetworkRequest);
//}

void CommunicationDescriptionGateway::requestForApplication(QString applicationID) {
    this->performThisRequest(new ApplicationNetworkRequest(applicationID));
}

void CommunicationDescriptionGateway::requestForUserGrabList() {
    this->performThisRequest(new GrabListNetworkRequest);
}

void CommunicationDescriptionGateway::requestForLoggingUserIn() {
    this->performThisRequest(new UserLoginNetworkRequest);
}

void CommunicationDescriptionGateway::requestForClientUpdateVersionCheck() {
    this->performThisRequest(new ClientUpdateNetworkRequest);
}

void CommunicationDescriptionGateway::requestForClientUpdateZip() {
    this->performThisRequest(new GrabClientUpdateNetworkRequest);
}

void CommunicationDescriptionGateway::requestForUserFeedback(QString feedbackMessage) {
    this->performThisRequest(new UserFeedbackRequest(feedbackMessage));
}

void CommunicationDescriptionGateway::requestForUserSuggestAnApp(QString name, QString website, QString summary) {
    this->performThisRequest(new UserSuggestAppRequest(name, website, summary));
}

void CommunicationDescriptionGateway::performThisRequest(BaseNetworkRequest *requestToPerform)
{
    if(_isRequestActive) {
        WLog("Active request found! Only one request can be active!");
        return;
    }

    _isRequestActive = true;
    _finishedSteps = 0;
    this->networkRequest = requestToPerform;
    setupRequestConnections();
    networkRequest->startThisRequest();
}

// Request methods
void CommunicationDescriptionGateway::setupRequestConnections() {
//    this->disconnect();
    connect(networkRequest, SIGNAL(responseDataChunkAvailable(QByteArray,bool)), this, SLOT(requestReceived(QByteArray,bool)));
    connect(networkRequest, SIGNAL(finished()), this, SLOT(requestFinished()));
    connect(networkRequest, SIGNAL(authenticationChallenge()), this, SLOT(authenticationChallenge()));
    // failed connections
    connect(networkRequest, SIGNAL(failedToSendRequest()), this, SLOT(requestFailed()));
    connect(networkRequest, SIGNAL(noNetworkConnection()), this, SLOT(requestFailed()));
    connect(networkRequest, SIGNAL(requestTimedOut()), this, SLOT(requestFailed()));
}

void CommunicationDescriptionGateway::requestFinished() {
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("Finished");
#endif

    AW_QOBJECT_SAFE_DELETE(networkRequest);

    this->_finishedWithCommunicationStep();
}

void CommunicationDescriptionGateway::requestFailed() {
    QString errorMsg = "Server communication failed.";

//    if(this->networkRequest != NULL) {
//        this->networkRequest->
//    }

    Q_EMIT failedToRetrieveDescription(errorMsg);

    _isRequestActive = false;
    AW_QOBJECT_SAFE_DELETE(networkRequest);
    Q_EMIT finishedCompletely();
}

void CommunicationDescriptionGateway::requestReceived(QByteArray response, bool isBufferDownloadedResourceAndSendItWhenDownloadFinished) {
    if(!isBufferDownloadedResourceAndSendItWhenDownloadFinished) {
        WLog("Only chunk of resource retrieved - isBufferDownloadedResourceAndSendItWhenDownloadFinished is false, should be true");
        return;
    }

    if(networkRequest == NULL) {
        WLog("Response recieved, but request is already deleted! Cannot parse the response! This should never happen.");
        return;
    }

    Q_EMIT _receivedResponseData(response);

    networkResponse = networkRequest->getResponseParser();
    if (!networkResponse) {
        QList<AW::Common::Communication::BaseNetworkEntity*> emptyList;
        Q_EMIT this->finished(emptyList);
        _finishedWithCommunicationStep();
        return;
    }


    connect(networkResponse, SIGNAL(finished()), this, SLOT(responseFinished()));
    connect(networkResponse, SIGNAL(parsingFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(networkResponse, SIGNAL(parsingFailed(QString)), this, SIGNAL(failedToRetrieveDescription(QString)));

    //
    // parse in background
    QThreadPool::globalInstance()->start(networkResponse);
}

// Responose methods
void CommunicationDescriptionGateway::responseFinished() {
    _finishedWithCommunicationStep();
}

void CommunicationDescriptionGateway::authenticationChallenge()
{
    // finished with the request - won't continue this
    _isRequestActive = false;
    AW_QOBJECT_SAFE_DELETE(networkRequest);
    Q_EMIT finishedCompletely();

    //
    Q_EMIT authenticationChallengeHappened();
//    Authorization::sharedAuthorization()->showLoginDialogAndStartLogin();
}

void CommunicationDescriptionGateway::_finishedWithCommunicationStep()
{
    QMutex mutex;
    mutex.lock();
    _finishedSteps++;

    if(_finishedSteps == 2) {
        _isRequestActive = false;
        Q_EMIT finishedCompletely();
    }
    mutex.unlock();
}
