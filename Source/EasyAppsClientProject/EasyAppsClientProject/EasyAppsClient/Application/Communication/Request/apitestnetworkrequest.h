#ifndef APITESTNETWORKREQUEST_H
#define APITESTNETWORKREQUEST_H

#include "Common/Communication/basenetworkrequest.h"

/*! Request for testing the communication API
*/
class ApiTestNetworkRequest : public BaseNetworkRequest
{
public:
    explicit ApiTestNetworkRequest(BaseCommunicationRequestType requestType,
                                      const QString& requestUrl,
                                      QMap<QString, QString> additionalHeaderKeyValueMap,
                                      bool securedRequest = false,
                                      const QByteArray& postData = 0,
                                      bool saveResponseToFile = false,
                                      QNetworkRequest::CacheLoadControl cachePolicy = QNetworkRequest::PreferCache,
                                      int timeoutInterval = 10000);

    virtual Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual void addAdditionalRequiredHeaders(QNetworkRequest *networkRequest);

private:
    QMap<QString, QString> _additionalHeaderKeyValueMap;
};

#endif // APITESTNETWORKREQUEST_H
