#include "communicationresourcegateway.h"

#include "Application/Communication/Request/resourcedownloadnetworkrequest.h"

CommunicationResourceGateway::CommunicationResourceGateway(unsigned int numberOfRequestsParalell) :
    QObject(0),
    numberOfRequestsParalell(numberOfRequestsParalell)
{
}

CommunicationResourceGateway::~CommunicationResourceGateway() {
//    this->disconnect(); // not necessary, QObject's destructor does it

    if (networkRequests.size()) {
        BaseNetworkRequest* request = NULL;
        Q_FOREACH(request, networkRequests) {
            delete request, request = NULL;
        }
    }

    networkRequests.clear();
    resources.clear();
}

void CommunicationResourceGateway::addResourceURL(const QString& URL) {
    resources.append(URL);
}

void CommunicationResourceGateway::requestReceived(const QByteArray& response) {
    BaseNetworkRequest* networkRequest = (BaseNetworkRequest*)QObject::sender();
    Q_EMIT this->resourceReceivedForURL(networkRequest->getRequestedURL(), response);
}

void CommunicationResourceGateway::requestFinished() {
    BaseNetworkRequest* networkRequest = (BaseNetworkRequest*)QObject::sender();

    --numberOfRequestsParalell;

    networkRequests.removeOne(networkRequest);

    networkRequest->disconnect();
//    AW_QOBJECT_SAFE_DELETE(networkRequest);
    AW_QOBJECT_SAFE_DELETE(networkRequest);

    // Try to load the next in the list
    startRequests();
}

void CommunicationResourceGateway::downloadProgress(int progress)
{
    BaseNetworkRequest* networkRequest = (BaseNetworkRequest*)QObject::sender();
    Q_EMIT downloadProgressForUrl(progress, networkRequest->getRequestedURL());
}

void CommunicationResourceGateway::requestFailed() {
    BaseNetworkRequest* networkRequest = (BaseNetworkRequest*)QObject::sender();

    Q_EMIT resourceDownloadFailedForUrl(networkRequest->getRequestedURL());

    --numberOfRequestsParalell;

    networkRequests.removeOne(networkRequest);

    networkRequest->disconnect();
//    AW_QOBJECT_SAFE_DELETE(networkRequest);
    AW_QOBJECT_SAFE_DELETE(networkRequest);

    // Try to load the next in the list
    startRequests();
}

void CommunicationResourceGateway::startRequests() {
    if (resources.size() <= 0 &&
        networkRequests.size() <= 0) {
        Q_EMIT this->finished();
        return;
    }

    while (networkRequests.size() < numberOfRequestsParalell &&
           resources.size() > 0) {
        ++numberOfRequestsParalell;

        QString URL = resources.last();
        resources.removeLast();

        // Setup the request
        BaseNetworkRequest* networkRequest = NULL;
        if (isResourceCacheable(URL)) {
            networkRequest = new ResourceDownloadNetworkRequest(BaseNetworkRequest::BaseCommunicationRequestTypeGet,
                                                    URL);
        } else {
            networkRequest = new ResourceDownloadNetworkRequest(BaseNetworkRequest::BaseCommunicationRequestTypeGet,
                                                    URL,
                                                    false,
                                                    NULL,
                                                    QNetworkRequest::AlwaysNetwork);
        }
        connect(networkRequest, SIGNAL(responseDownloaded(QByteArray)), this, SLOT(requestReceived(QByteArray)));
        connect(networkRequest, SIGNAL(requestProgress(int)), this, SLOT(downloadProgress(int)));
        connect(networkRequest, SIGNAL(failedToSendRequest()), this, SLOT(requestFailed()));
        connect(networkRequest, SIGNAL(requestTimedOut()), this, SLOT(requestFailed()));
        connect(networkRequest, SIGNAL(finished()), this, SLOT(requestFinished()));

        // Add it to the list
        networkRequests.append(networkRequest);
        networkRequest->startThisRequest();
    }
}

bool CommunicationResourceGateway::isResourceCacheable(const QString& URL) {
    if (URL.contains("/applications/zips/")) return false;

    return true;
}
