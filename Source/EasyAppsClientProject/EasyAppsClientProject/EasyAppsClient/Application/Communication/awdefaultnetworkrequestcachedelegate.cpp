#include "awdefaultnetworkrequestcachedelegate.h"

#include <QNetworkDiskCache>
#include "Application/appspecificpathhelper.h"

namespace AW {
namespace Client {
namespace Communication {

QAbstractNetworkCache *AWDefaultNetworkRequestCacheDelegate::createCacheHandler(QObject *parent)
{
    QNetworkDiskCache* cache = new QNetworkDiskCache(parent);
    cache->setCacheDirectory(AppSpecificPathHelper::getCacheDirectory());
    return cache;
}

} // namespace Communication
} // namespace Client
} // namespace AW
