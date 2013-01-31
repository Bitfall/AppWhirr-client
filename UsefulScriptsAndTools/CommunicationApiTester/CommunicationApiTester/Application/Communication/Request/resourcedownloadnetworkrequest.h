#ifndef RESOURCEDOWNLOADNETWORKREQUEST_H
#define RESOURCEDOWNLOADNETWORKREQUEST_H

#include "Common/Communication/basenetworkrequest.h"

/*! Use this request to download a resource (like a ZIP or an image).

    @note
        Won't parse the response!
*/
class ResourceDownloadNetworkRequest : public BaseNetworkRequest
{
public:
    explicit ResourceDownloadNetworkRequest(BaseCommunicationRequestType requestType,
                                      const QString& requestUrl,
                                      bool securedRequest = false,
                                      const QByteArray& postData = 0,
                                      bool saveResponseToFile = false,
                                      QNetworkRequest::CacheLoadControl cachePolicy = QNetworkRequest::PreferCache,
                                      int timeoutInterval = 10000);

    Background_BaseNetworkResponse* getResponseParser() { return NULL; }

protected:
    virtual QString getRequestVersion() const;
};

#endif // RESOURCEDOWNLOADNETWORKREQUEST_H
