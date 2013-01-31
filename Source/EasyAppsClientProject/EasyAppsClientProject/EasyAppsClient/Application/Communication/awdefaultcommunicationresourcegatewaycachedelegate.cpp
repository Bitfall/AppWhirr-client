#include "awdefaultcommunicationresourcegatewaycachedelegate.h"

#include "awdefaultnetworkrequestcachedelegate.h"

namespace AW {
namespace Client {
namespace Communication {

AWDefaultCommunicationResourceGatewayCacheDelegate::AWDefaultCommunicationResourceGatewayCacheDelegate(QObject *parent) :
    QObject(parent)
{
}

QNetworkRequest::CacheLoadControl AWDefaultCommunicationResourceGatewayCacheDelegate::getCacheControlForResourceUrl(QUrl url)
{
    if (url.toString().contains("/zips/")) {
        return QNetworkRequest::AlwaysNetwork;
    }

    return QNetworkRequest::PreferCache;
}

Common::Communication::NetworkRequestCacheDelegate *AWDefaultCommunicationResourceGatewayCacheDelegate::getRequestCacheDelegate()
{
    return new AWDefaultNetworkRequestCacheDelegate();
}

} // namespace Communication
} // namespace Client
} // namespace AW
