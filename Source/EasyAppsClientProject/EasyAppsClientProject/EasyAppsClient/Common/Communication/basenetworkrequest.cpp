#include "basenetworkrequest.h"
#include <QTimer>
#include <QByteArray>
#include <QScriptValue>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkDiskCache>
#include <QUrl>

// config
#include "networkrequestcachedelegate.h"


namespace AW {
namespace Common {
namespace Communication {


BaseNetworkRequest::BaseNetworkRequest(BaseCommunicationRequestType requestType,
                                       QUrl requestUrl,
                                       QNetworkRequest::CacheLoadControl cachePolicy,
                                       int timeoutInterval,
                                       bool isBufferDownloadedResourceAndSendItWhenDownloadFinished,
                                       NetworkRequestCacheDelegate *requestCacheDelegate)
    : QObject(0),
      _requestType(requestType),
      _requestUrl(requestUrl),
      _networkRequest(0),
      _networkReply(0),
      _cachePolicy(cachePolicy),
      _isBufferDownloadedResourceAndSendItWhenDownloadFinished(isBufferDownloadedResourceAndSendItWhenDownloadFinished),
      _cacheDelegate(requestCacheDelegate)
{
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("Network request created with url: " + requestUrl.toString());
#endif

    _defaultRequestProtocolPrefix = QString("http://");

    _networkAccessManager = new QNetworkAccessManager(this);

    // Setup caching    
    if(_cacheDelegate == NULL) {
        WLog("No cache delegate defined!");
    }
    else {
        _networkAccessManager->setCache(_cacheDelegate->createCacheHandler(this));
    }

    // Connect what is needed to be connected
    connect(_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    // ssl
    connect(_networkAccessManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(_sslErrors(QNetworkReply*,QList<QSslError>)));

    // Setup timer for watching timeout events
    _timeoutTimer = new QTimer;
    _timeoutTimer->setSingleShot(true);
    _timeoutTimer->setInterval(timeoutInterval);

//    this->startThisRequest(); // REMOVED, call it directly when you need it, not in the constructor
}

BaseNetworkRequest::~BaseNetworkRequest() {
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("Network request deleted with url: " + _requestUrl.toString());
#endif

    if (_timeoutTimer) {
        _timeoutTimer->disconnect();
        _timeoutTimer->stop();
        AW_QOBJECT_SAFE_DELETE(_timeoutTimer);
    }

    AW_SAFE_DELETE(_networkRequest);

    if (_networkReply) {
        _networkReply->disconnect();
        _networkReply->abort();
        AW_QOBJECT_SAFE_DELETE(_networkReply);
    }
}

//void BaseNetworkRequest::setNetworkRequestCacheDelegate(NetworkRequestCacheDelegate *cacheDelegate)
//{
//    // Setup caching
//    if(cacheDelegate == NULL) {
//        WLog("No cache delegate provided!");
////        _networkAccessManager->setCache(NULL);
//    }
//    else {
//        _cacheDelegate = cacheDelegate;
//        _networkAccessManager->setCache(_cacheDelegate->getCacheHandler());
//    }
//}

void BaseNetworkRequest::abort() {
    if (_networkReply) {
        WLog("Cannot abort a request, which do not have a reply");
        return;
    }

    _networkReply->abort();
    AW_QOBJECT_SAFE_DELETE(_networkReply);
}

QUrl BaseNetworkRequest::getRequestedURL() {
    return (_networkRequest ? _networkRequest->url() : QUrl(""));
}

void BaseNetworkRequest::startThisRequest() {
    // Send request only if network is accessible
    if (_networkAccessManager->networkAccessible() == QNetworkAccessManager::NotAccessible) {
        Q_EMIT this->failedToSendRequest();
        return;
    }

    // Stop timeout timer, if running
    if (_timeoutTimer->isActive()) {
        _timeoutTimer->stop();
    }

    // Delete previous network request, if any
    AW_SAFE_DELETE(_networkRequest);

    // Delete previous network reply, if any
    AW_QOBJECT_SAFE_DELETE(_networkReply);

    // Init response body
    _responseBody.clear();

    // Create new request
    _networkRequest = new QNetworkRequest;
    if(_requestUrl.toString().startsWith("/")) {
        // in case the request-url does not have a protocol(prefix) a default one will be applied
        //  -> without this some strange 'thread blocking' happens when _networkAccessManager->get() called
        _requestUrl = QUrl(_defaultRequestProtocolPrefix + _requestUrl.toString());
    }
    _networkRequest->setUrl(_requestUrl);
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("Final network request url: ") << _requestUrl.toString();
#elif ENABLE_COMMUNICATION_CORE_LOGGING
    DLog("Final network request url (end): ") << QString("...%1").arg(_requestUrl.toString().right(20));
#endif
    _networkRequest->setAttribute(QNetworkRequest::CacheLoadControlAttribute, _cachePolicy);

    if (!this->appendHeadersToRequest()) {
        AW_SAFE_DELETE(_networkRequest);

        return;
    }

#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("Request start with type: ") << _requestType;
#endif
    switch(_requestType) {
        case BaseCommunicationRequestTypeGet:
            _networkReply = _networkAccessManager->get(*_networkRequest);
            break;
        case BaseCommunicationRequestTypePost:
            _networkRequest->setRawHeader("Content-Type", "application/x-www-form-urlencoded");
            _networkReply = _networkAccessManager->post(*_networkRequest, this->getPostData());
            break;
        case BaseCommunicationRequestTypeHead:
            _networkReply = _networkAccessManager->head(*_networkRequest);
            break;
        default:
            WLog("Bad request type definition");
    }
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("Request started.");
#endif

    // Connect timeout timer
    connect(_timeoutTimer, SIGNAL(timeout()), this, SIGNAL(requestTimedOut()));
    // Connect network management
    connect(_networkReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    connect(_networkReply, SIGNAL(readyRead()), this, SLOT(dataReadyForRead()));
    connect(_networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(_requestError(QNetworkReply::NetworkError)));

    // Start timeout timer
    _timeoutTimer->start();

    QNetworkReply::NetworkError error = _networkReply->error();
    if (error != QNetworkReply::NoError) {
        _timeoutTimer->stop();
        WLog("Error in response: " << error << " @ " << _networkReply->url());
        Q_EMIT failedToSendRequest();
    }
}

void BaseNetworkRequest::_sslErrors(QNetworkReply *reply, QList<QSslError> sslErrors)
{
    DLog("SSL Errors for url: ") << reply->url();

    for(int i = 0; i < sslErrors.size(); ++i)
    {
        DLog("Error: ") << sslErrors[i].errorString();
    }

#ifdef ENABLE_IGNORE_SSL_ERRORS_IN_SERVER_COMMUNICATION
    reply->ignoreSslErrors(sslErrors);
#else
    WLog("Request failed, SSL errors are not ignored!");
#endif
}

void BaseNetworkRequest::requestFinished(QNetworkReply* reply) {
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("Content-length: " << _networkReply->header(QNetworkRequest::ContentLengthHeader));
    DLog("Response body: ") << _responseBody;
#endif

    if (reply != _networkReply) {
        WLog("Another reply found...");
        // delete it
        reply->deleteLater();
        return;
    }

#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog(reply->url().toString() << " loaded from cache:" << reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool());
#endif

    bool canMoveOn = true;

    QNetworkReply::NetworkError error = reply->error();
    if (error != QNetworkReply::NoError) {
        WLog(QString("Response finished with error %1").arg(error));
        if (error <= QNetworkReply::UnknownNetworkError) {
            // No network connection detected
            Q_EMIT noNetworkConnection();
        }
        else if(error == 204) // 204 is "Unauthorized"
        {
            WLog("Response error: 204 'Unauthorized'");
//            Authorization::sharedAuthorization()->removeCredentials();
            Q_EMIT authenticationChallenge();
        }
        else {
            Q_EMIT failedToSendRequest();
        }

        canMoveOn = false;
    }

    // Authorize from header
    // NOTE: moved inside the error-code-checking part
//    if (canMoveOn && securedRequest) {
//        if (!Authorization::sharedAuthorization()->checkAuthorizationInHeader(reply)) {
//            // failed to authenticate
//            // automatically deletes password from memory

//            canMoveOn = false;
//            Q_EMIT this->authenticationChallenge();
//        }
//    }

    if (canMoveOn) {
        // Let's see what we've got
        if(_isBufferDownloadedResourceAndSendItWhenDownloadFinished) {
            Q_EMIT this->responseDataChunkAvailable(_responseBody, _isBufferDownloadedResourceAndSendItWhenDownloadFinished);
        }
    }

    Q_EMIT this->finished();
}

void BaseNetworkRequest::dataReadyForRead() {
    QByteArray bytes = _networkReply->readAll();

//    DLog("Response bytes available for read");

    if(_isBufferDownloadedResourceAndSendItWhenDownloadFinished)
    {
        _responseBody.append(bytes);
    }
    else {
        Q_EMIT responseDataChunkAvailable(bytes, _isBufferDownloadedResourceAndSendItWhenDownloadFinished);
    }
}

void BaseNetworkRequest::_requestError(QNetworkReply::NetworkError networkError)
{
    WLog("Request error [unhandled!! check this!!]: ") << networkError;
}

void BaseNetworkRequest::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    // Reset timeout timer
    _timeoutTimer->start();

    if (bytesTotal > 0) {
        Q_EMIT this->requestProgress(bytesReceived * 100 / bytesTotal);
    }
}

} // namespace Communication
} // namespace Common
} // namespace AW
