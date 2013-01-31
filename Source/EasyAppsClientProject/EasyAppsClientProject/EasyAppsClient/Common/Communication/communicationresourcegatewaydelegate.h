#ifndef AW_COMMON_COMMUNICATION_COMMUNICATIONRESOURCEGATEWAYDELEGATE_H
#define AW_COMMON_COMMUNICATION_COMMUNICATIONRESOURCEGATEWAYDELEGATE_H

#include "Common/Communication/basenetworkrequest.h"

namespace AW {
namespace Common {
namespace Communication {

class CommunicationResourceGatewayDelegate {
public:
    virtual ~CommunicationResourceGatewayDelegate() {}

    /*! Create a network-request by the parameters

        @param cachePolicy : it's the cache-control given by CommunicationResourceGatewayCacheDelegate's \a getCacheControlForResourceUrl()
        @param timeoutInterval : in milliseconds
        @param requestCacheDelegate : cache-delegate for the request. Given by CommunicationResourceGatewayCacheDelegate's \a getRequestCacheDelegate()
    */
    virtual BaseNetworkRequest *createResourceDownloadRequest(BaseNetworkRequest::BaseCommunicationRequestType requestType,
                                                              QUrl requestUrl,
                                                              QNetworkRequest::CacheLoadControl cacheControl,
                                                              int timeoutInterval,
                                                              NetworkRequestCacheDelegate *requestCacheDelegate) = 0;
};

} // namespace Communication
} // namespace Common
} // namespace AW

#endif // AW_COMMON_COMMUNICATION_COMMUNICATIONRESOURCEGATEWAYDELEGATE_H
