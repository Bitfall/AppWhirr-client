#ifndef BASENETWORKREQUEST_H
#define BASENETWORKREQUEST_H

#include <QObject>
#include <QString>
#include <QNetworkReply>

#include <QList>
#include <QSslError>

QT_BEGIN_NAMESPACE
class QByteArray;
class QScriptValue;
class QNetworkAccessManager;
class QTimer;
class QNetworkRequest;
QT_END_NAMESPACE

class Background_BaseNetworkResponse;

class BaseNetworkRequest : public QObject
{
    Q_OBJECT

public:
    typedef enum {
        BaseCommunicationRequestTypeGet,    /*! Initializes a get request type */
        BaseCommunicationRequestTypePost,   /*! Initializes a post request type */
        BaseCommunicationRequestTypeHead    /*! Initializes a head request type */
    } BaseCommunicationRequestType;

    /*! Create a new communication request
    */
    explicit BaseNetworkRequest(BaseCommunicationRequestType requestType,
                                      const QString& requestUrl,
                                      bool securedRequest = false,
                                      const QByteArray& postData = 0,
                                      bool saveResponseToFile = false,
                                      QNetworkRequest::CacheLoadControl cachePolicy = QNetworkRequest::PreferCache,
                                      int timeoutInterval = 10000);
    virtual ~BaseNetworkRequest();

    void startThisRequest();

    /*! Creates a specific parser for the response received
      */
    virtual Background_BaseNetworkResponse* getResponseParser() = 0;

    /*! Aborts the requests
    */
    void abort();

    /*! Returns the requested URL
    */
    QString getRequestedURL();

Q_SIGNALS:
    /*! Request has timed out
      */
    void requestTimedOut();

    void requestProgress(int percentage);

    void finished();
    void failedToSendRequest();
    void responseDownloaded(QByteArray responseBody);

    void authenticationChallenge();
    void noNetworkConnection();

public Q_SLOTS:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    void _sslErrors(QNetworkReply *reply, QList<QSslError> sslErrors);
    void requestFinished(QNetworkReply* reply);
    void downloadResponse();

protected:
    /*! Return the request's version (id).

        Every request have to specify it's version in the header - you have to specify the required version here and this will be added to the request's header automatically.

        @return The required request version.
    */
    virtual QString getRequestVersion() const = 0;

    /*! This method will be called before the request is sent.

        You can add additional required headers for the request.
        @note
            The UserAgent and the request-version is added automatically to the request-header.
    */
    virtual void addAdditionalRequiredHeaders(QNetworkRequest *networkRequest) {}

protected:
    QNetworkAccessManager               networkAccessManager;
    QByteArray                          postData;
    QTimer*                             timeoutTimer;
    int                                 requestType;
    QString                             requestUrl;
    bool                                saveResponseToFile;
    QNetworkRequest*                    networkRequest;
    QNetworkReply*                      networkReply;
    QByteArray                          responseBody;
    bool                                securedRequest;

private:
    QString getUserAgent_();
    bool appendHeadersToRequest();

    QNetworkRequest::CacheLoadControl   cachePolicy;
};

#endif // BASENETWORKREQUEST_H
