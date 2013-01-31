#ifndef AW_COMMON_COMMUNICATION_COMMUNICATIONRESOURCEGATEWAYCACHEDELEGATE_H
#define AW_COMMON_COMMUNICATION_COMMUNICATIONRESOURCEGATEWAYCACHEDELEGATE_H

#include <QNetworkRequest>

namespace AW {
namespace Common {
namespace Communication {

class NetworkRequestCacheDelegate;

class CommunicationResourceGatewayCacheDelegate {
public:
    virtual ~CommunicationResourceGatewayCacheDelegate() {}

    virtual QNetworkRequest::CacheLoadControl getCacheControlForResourceUrl(QUrl url) = 0;
    /*! Cache delegate for a given resource request

        @return Return NULL (by default) if no cache delegate should be applied for the request
    */
    virtual NetworkRequestCacheDelegate *getRequestCacheDelegate() { return NULL; }
};

} // namespace Communication
} // namespace Common
} // namespace AW

#endif // AW_COMMON_COMMUNICATION_COMMUNICATIONRESOURCEGATEWAYCACHEDELEGATE_H
