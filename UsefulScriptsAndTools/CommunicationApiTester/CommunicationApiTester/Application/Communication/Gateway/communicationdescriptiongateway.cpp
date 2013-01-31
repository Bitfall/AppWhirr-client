#include "communicationdescriptiongateway.h"

#include <QThreadPool>
#include <QMutex>

// request
#include "Application/Communication/Request/applicationlistnetworkrequest.h"
#include "Application/Communication/Request/applicationnetworkrequest.h"
#include "Application/Communication/Request/grabapplicationnetworkrequest.h"
#include "Application/Communication/Request/grablistnetworkrequest.h"
#include "Application/Communication/Request/userloginnetworkrequest.h"
#include "Application/Communication/Request/clientupdatenetworkrequest.h"
#include "Application/Communication/Request/grabclientupdatenetworkrequest.h"

// response
#include "Common/Communication/basenetworkresponse.h"

// entity
#include "Common/Communication/basenetworkentity.h"

#include "Common/Security/authorization.h"

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

void CommunicationDescriptionGateway::requestForApplication(const QString& applicationID) {
    this->performThisRequest(new ApplicationNetworkRequest(applicationID));
}

void CommunicationDescriptionGateway::requestForGrabbingApplicationByAppId(const QString& applicationID, GrabRequestType grabType) {
    int grabTypeInt = (int)grabType;
    this->performThisRequest(new GrabApplicationNetworkRequest(applicationID, QString::number(grabTypeInt)));
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
    connect(networkRequest, SIGNAL(responseDownloaded(QByteArray)), this, SLOT(requestReceived(QByteArray)));
    connect(networkRequest, SIGNAL(finished()), this, SLOT(requestFinished()));
    connect(networkRequest, SIGNAL(authenticationChallenge()), this, SLOT(authenticationChallenge()));
    // failed connections
    connect(networkRequest, SIGNAL(failedToSendRequest()), this, SLOT(requestFailed()));
    connect(networkRequest, SIGNAL(noNetworkConnection()), this, SLOT(requestFailed()));
    connect(networkRequest, SIGNAL(requestTimedOut()), this, SLOT(requestFailed()));
}

void CommunicationDescriptionGateway::requestFinished() {
#ifdef ENABLE_COMMUNICATION_LOGGING
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

    Q_EMIT this->failedToRetrieveDescription(errorMsg);

    _isRequestActive = false;
    AW_QOBJECT_SAFE_DELETE(networkRequest);
    Q_EMIT finishedCompletely();
}

void CommunicationDescriptionGateway::requestReceived(QByteArray response) {
    if(networkRequest == NULL) {
        WLog("Response recieved, but request is already deleted! Cannot parse the response! This should never happen.");
        return;
    }

    Q_EMIT _receivedResponseData(response);

    networkResponse = networkRequest->getResponseParser();
    if (!networkResponse) {
        QList<BaseNetworkEntity*> emptyList;
        Q_EMIT this->finished(emptyList);
        _finishedWithCommunicationStep();
        return;
    }


    connect(networkResponse, SIGNAL(finished()), this, SLOT(responseFinished()));
    connect(networkResponse, SIGNAL(parsingFinished(QList<BaseNetworkEntity*>)), this, SIGNAL(finished(QList<BaseNetworkEntity*>)));
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
    Q_EMIT this->authenticationChallengeHappened();
    Authorization::sharedAuthorization()->showLoginDialogAndStartLogin();
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
