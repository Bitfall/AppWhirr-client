#ifndef AW_COMMON_COMMUNICATION_NETWORKREQUESTCACHEDELEGATE_H
#define AW_COMMON_COMMUNICATION_NETWORKREQUESTCACHEDELEGATE_H

#include <QAbstractNetworkCache>

namespace AW {
namespace Common {
namespace Communication {

class NetworkRequestCacheDelegate
{
public:
    NetworkRequestCacheDelegate() {}
    virtual ~NetworkRequestCacheDelegate() {}

    virtual QAbstractNetworkCache *createCacheHandler(QObject *parent) = 0;
};

} // namespace Communication
} // namespace Common
} // namespace AW

#endif // AW_COMMON_COMMUNICATION_NETWORKREQUESTCACHEDELEGATE_H
