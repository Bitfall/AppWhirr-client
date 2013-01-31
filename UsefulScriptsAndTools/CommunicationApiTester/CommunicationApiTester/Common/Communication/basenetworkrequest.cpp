#include "basenetworkrequest.h"
#include <QTimer>
#include <QByteArray>
#include <QScriptValue>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkDiskCache>

#include <QSslSocket>
#include <QFile>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QDateTime>

#include "Common/Security/authorization.h"
#include "Common/System/systemprofiler.h"

// utility
#include "Application/appspecificpathhelper.h"

// config
#include "Application/Settings/configmanager.h"

BaseNetworkRequest::BaseNetworkRequest(BaseCommunicationRequestType requestType,
                                                   const QString& requestUrl,
                                                   bool securedRequest,
                                                   const QByteArray& postData,
                                                   bool saveResponseToFile,
                                                   QNetworkRequest::CacheLoadControl cachePolicy,
                                                   int timeoutInterval)
    : QObject(0),
      networkAccessManager(this),
      postData(postData),
      requestType(requestType),
      requestUrl(requestUrl),
      saveResponseToFile(saveResponseToFile),
      networkRequest(0),
      networkReply(0),
      cachePolicy(cachePolicy),
      securedRequest(securedRequest)
{
#ifdef ENABLE_COMMUNICATION_LOGGING
    DLog("Network request created with url: " + requestUrl);
#endif

    // Setup caching
    QNetworkDiskCache* cache = new QNetworkDiskCache(this);
    cache->setCacheDirectory(AppSpecificPathHelper::getCacheDirectory());
    networkAccessManager.setCache(cache);

    // Connect what is needed to be connected
    connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    // ssl
    connect(&networkAccessManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(_sslErrors(QNetworkReply*,QList<QSslError>)));

    // Setup timer for watching timeout events
    timeoutTimer = new QTimer;
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(timeoutInterval);

//    this->startThisRequest(); // REMOVED, call it directly when you need it, not in the constructor
}

BaseNetworkRequest::~BaseNetworkRequest() {
#ifdef ENABLE_COMMUNICATION_LOGGING
    DLog("Network request deleted with url: " + requestUrl);
#endif

    if (timeoutTimer) {
        timeoutTimer->disconnect();
        timeoutTimer->stop();
        AW_QOBJECT_SAFE_DELETE(timeoutTimer);
    }

    AW_SAFE_DELETE(networkRequest);

    if (networkReply) {
        networkReply->disconnect();
        networkReply->abort();
        AW_QOBJECT_SAFE_DELETE(networkReply);
    }
}

void BaseNetworkRequest::abort() {
    if (networkReply) {
        WLog("Cannot abort a request, which do not have a reply");
        return;
    }

    networkReply->abort();
    AW_QOBJECT_SAFE_DELETE(networkReply);
}

QString BaseNetworkRequest::getRequestedURL() {
    return (networkRequest ? networkRequest->url().toString() : "");
}

void BaseNetworkRequest::startThisRequest() {
    // Send request only if network is accessible
    if (networkAccessManager.networkAccessible() == QNetworkAccessManager::NotAccessible) {
        Q_EMIT this->failedToSendRequest();
        return;
    }

    // Stop timeout timer, if running
    if (timeoutTimer->isActive()) {
        timeoutTimer->stop();
    }

    // Delete previous network request, if any
    AW_SAFE_DELETE(networkRequest);

    // Delete previous network reply, if any
    AW_QOBJECT_SAFE_DELETE(networkReply);

    // Init response body
    responseBody.clear();

    // Create new request
    networkRequest = new QNetworkRequest;
    networkRequest->setUrl(requestUrl);
    networkRequest->setAttribute(QNetworkRequest::CacheLoadControlAttribute, cachePolicy);

    if (!appendHeadersToRequest()) {
        AW_SAFE_DELETE(networkRequest);

        return;
    }

    QFile certFile(QString(":/aw_cert4_der"));
    Q_ASSERT(certFile.open(QIODevice::ReadOnly));
    QByteArray certContent = certFile.readAll();
    DLog("Cert: ") << certContent;
    QSslCertificate cert(certContent, QSsl::Der);

    DLog("Is cert valid: ") << cert.isValid();
    DLog("Cert info: ") << cert.effectiveDate() << cert.expiryDate() << cert.issuerInfo(QSslCertificate::Organization);

    QSslConfiguration sslConfig = networkRequest->sslConfiguration();
    QList<QSslCertificate> caCerts = sslConfig.caCertificates();
    caCerts.append(cert);
    sslConfig.setCaCertificates(caCerts);
    networkRequest->setSslConfiguration(sslConfig);

    switch(requestType) {
        case BaseCommunicationRequestTypeGet:
            networkReply = networkAccessManager.get(*networkRequest);
            break;
        case BaseCommunicationRequestTypePost:
            networkReply = networkAccessManager.post(*networkRequest, postData);
            break;
        case BaseCommunicationRequestTypeHead:
            networkReply = networkAccessManager.head(*networkRequest);
            break;
        default:
            ALog("Bad request type definition");
    }

    // Connect timeout timer
    connect(timeoutTimer, SIGNAL(timeout()), this, SIGNAL(requestTimedOut()));
    // Connect network management
    connect(networkReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    connect(networkReply, SIGNAL(readyRead()), this, SLOT(downloadResponse()));

    // Start timeout timer
    timeoutTimer->start();

    QNetworkReply::NetworkError error = networkReply->error();
    if (error != QNetworkReply::NoError) {
        ALog("Error in response: " << error << " @ " << networkReply->url());
    }
}

bool BaseNetworkRequest::appendHeadersToRequest() {
    if (securedRequest) {
        // Set Credentials
        if (!Authorization::sharedAuthorization()->addHttpBasicAuthentication(networkRequest)) {
            // Failed to add credentials
            // Show the login form

            // Todo: Tul koran jon fel es vegtelen ciklust okoz - ???
            Q_EMIT this->authenticationChallenge();

            return false;
        }
    }

    // Set User-Agent
    networkRequest->setRawHeader("User-Agent", getUserAgent_().toAscii());
    networkRequest->setRawHeader("version", this->getRequestVersion().toAscii());

    this->addAdditionalRequiredHeaders(networkRequest);

    return true;
}

void BaseNetworkRequest::_sslErrors(QNetworkReply *reply, QList<QSslError> sslErrors)
{
    DLog("SSL Errors for url: ") << reply->url();

    for(int i = 0; i < sslErrors.size(); ++i)
    {
        DLog("Error: ") << sslErrors[i].errorString();
    }

//    reply->ignoreSslErrors(sslErrors);
}

void BaseNetworkRequest::requestFinished(QNetworkReply* reply) {
#ifdef ENABLE_COMMUNICATION_LOGGING
    DLog("Content-length: " << networkReply->header(QNetworkRequest::ContentLengthHeader));
    DLog("Response body: ") << responseBody;
#endif

    if (reply != networkReply) {
        WLog("Another reply found...");
        // delete it
        reply->deleteLater();
        return;
    }

#ifdef ENABLE_COMMUNICATION_LOGGING
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
        else if(error == 204) // 204 is "Unauthorized" - will be handled
        {
            Authorization::sharedAuthorization()->removeCredentials();
            Q_EMIT this->authenticationChallenge();
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
        Q_EMIT this->responseDownloaded(responseBody);
    }

    Q_EMIT this->finished();
}

void BaseNetworkRequest::downloadResponse() {
    QByteArray bytes = networkReply->readAll();

    responseBody.append(bytes);
    //responseBody = responseBody + bytes;

    if (saveResponseToFile) {
        // Todo
    }
}

void BaseNetworkRequest::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    // Reset timeout timer
    timeoutTimer->start();

    if (bytesTotal > 0) {
        Q_EMIT this->requestProgress(bytesReceived * 100 / bytesTotal);
    }
}

QString BaseNetworkRequest::getUserAgent_() {
    QString appName = "EasyApps";
    QString versionNumber = ConfigManager::getClientVersion();
    QString operationSystemIdentifier = "deprecated";
    unsigned int currentSystemSubFlags = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemSubFlagsAsUInt();

    QString userAgentContent = QString(appName + ";" +
                   versionNumber + ";" +
                   operationSystemIdentifier + ";" +
                   "0f0bc00f072bd8d1d34ea3407d964387" + ";" +
                   QString::number(currentSystemSubFlags) + ";");

#ifdef ENABLE_COMMUNICATION_LOGGING
    DLog("User agent's content: ") << userAgentContent;
#endif

    return userAgentContent;
}
