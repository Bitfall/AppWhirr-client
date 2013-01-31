#ifndef RESOURCEDOWNLOADNETWORKREQUEST_H
#define RESOURCEDOWNLOADNETWORKREQUEST_H

#include "Common/Communication/basenetworkrequest.h"

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE


namespace AW {
namespace Common {
namespace Communication {

/*! Use this request to download a resource (like a ZIP or an image).

    @note
        Won't parse the response!
*/
class ResourceDownloadNetworkRequest : public BaseNetworkRequest
{
public:
    explicit ResourceDownloadNetworkRequest(BaseCommunicationRequestType requestType,
                                            QUrl requestUrl,
                                            QNetworkRequest::CacheLoadControl cachePolicy = QNetworkRequest::PreferCache,
                                            int timeoutInterval = 10000,
                                            bool isBufferDownloadedResourceAndSendItWhenDownloadFinished = true,
                                            NetworkRequestCacheDelegate *cacheDelegate = NULL) :
        BaseNetworkRequest(requestType, requestUrl, cachePolicy, timeoutInterval, isBufferDownloadedResourceAndSendItWhenDownloadFinished, cacheDelegate)
    {}

    Background_BaseNetworkResponse* getResponseParser() { return NULL; }
};

} // namespace Communication
} // namespace Common
} // namespace AW

#endif // RESOURCEDOWNLOADNETWORKREQUEST_H
