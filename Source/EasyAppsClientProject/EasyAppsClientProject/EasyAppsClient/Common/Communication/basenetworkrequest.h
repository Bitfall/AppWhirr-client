#ifndef BASENETWORKREQUEST_H
#define BASENETWORKREQUEST_H

#include <QObject>
#include <QString>
#include <QNetworkReply>
#include <QSslError>

QT_BEGIN_NAMESPACE
class QByteArray;
class QScriptValue;
class QNetworkAccessManager;
class QTimer;
class QNetworkRequest;
class QUrl;
QT_END_NAMESPACE



namespace AW {
namespace Common {
namespace Communication {


class Background_BaseNetworkResponse;
class NetworkRequestCacheDelegate;


class BaseNetworkRequest : public QObject
{
    Q_OBJECT

public:
    typedef enum {
        BaseCommunicationRequestTypeInvalid = 0,
        BaseCommunicationRequestTypeGet = 1,    /*! Initializes a get request type */
        BaseCommunicationRequestTypePost = 2,   /*! Initializes a post request type */
        BaseCommunicationRequestTypeHead = 3    /*! Initializes a head request type */
    } BaseCommunicationRequestType;


    /*! Create a new communication request
    */
    explicit BaseNetworkRequest(BaseCommunicationRequestType requestType,
                                QUrl requestUrlString,
                                QNetworkRequest::CacheLoadControl cachePolicy,
                                int timeoutInterval,
                                bool isBufferDownloadedResourceAndSendItWhenDownloadFinished = true,
                                NetworkRequestCacheDelegate *requestCacheDelegate = NULL);
    virtual ~BaseNetworkRequest();

//    void setNetworkRequestCacheDelegate(NetworkRequestCacheDelegate *cacheDelegate);

    void startThisRequest();

    /*! Creates a specific parser for the response received
      */
    virtual Background_BaseNetworkResponse* getResponseParser() = 0;

    /*! Aborts the requests
    */
    void abort();

    /*! Returns the requested URL
    */
    QUrl getRequestedURL();

Q_SIGNALS:
    /*! Request has timed out
      */
    void requestTimedOut();

    void requestProgress(int percentage);

    void finished();
    void failedToSendRequest();
    /*! Chunk of the requested data is available.

        @param responseDataChunk : the downloaded resource data chunk.
        @param isBufferDownloadedResourceAndSendItWhenDownloadFinished : if true then it's the whole requested resource. If false it's only a chunk of it.
    */
    void responseDataChunkAvailable(QByteArray responseDataChunk, bool isBufferDownloadedResourceAndSendItWhenDownloadFinished);

    void authenticationChallenge();
    void noNetworkConnection();

public Q_SLOTS:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    void _sslErrors(QNetworkReply *reply, QList<QSslError> sslErrors);
    void requestFinished(QNetworkReply* reply);
    void dataReadyForRead();

private Q_SLOTS:
    void _requestError(QNetworkReply::NetworkError networkError);

protected:
    /*! This method will be called before the request is sent.

        You can add additional required headers for the request.
        @note
            The UserAgent and the request-version is added automatically to the request-header.
    */
    virtual void addAdditionalRequiredHeaders(QNetworkRequest *networkRequest) {}

    /*! Override this if you want to specify post data for the request.
    */
    virtual QByteArray getPostData() const { return QByteArray(); }

    virtual QString getUserAgent() { return QString(); }

    /*! Add required headers to the request

        @return : return false if headers cannot be applied - in this case it will abort the request. If returns true the request can be started.
    */
    virtual bool appendHeadersToRequest() { return true; }

protected:
    BaseCommunicationRequestType        _requestType;
    QUrl                                _requestUrl;
    QNetworkRequest*                    _networkRequest;
    QByteArray                          _responseBody;

private:
    QNetworkAccessManager               *_networkAccessManager;
    QNetworkReply*                      _networkReply;
    QTimer*                             _timeoutTimer;
    QNetworkRequest::CacheLoadControl   _cachePolicy;
    QString _defaultRequestProtocolPrefix;
    bool _isBufferDownloadedResourceAndSendItWhenDownloadFinished;
    NetworkRequestCacheDelegate *_cacheDelegate;
};


} // namespace Communication
} // namespace Common
} // namespace AW

#endif // BASENETWORKREQUEST_H
