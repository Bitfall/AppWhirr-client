#ifndef AW_CLIENT_COMMUNICATION_AWDEFAULTNETWORKREQUESTCACHEDELEGATE_H
#define AW_CLIENT_COMMUNICATION_AWDEFAULTNETWORKREQUESTCACHEDELEGATE_H

#include "Common/Communication/networkrequestcachedelegate.h"

namespace AW {
namespace Client {
namespace Communication {

class AWDefaultNetworkRequestCacheDelegate : public AW::Common::Communication::NetworkRequestCacheDelegate
{
public:
    explicit AWDefaultNetworkRequestCacheDelegate() {}
    virtual ~AWDefaultNetworkRequestCacheDelegate() {}
    
    virtual QAbstractNetworkCache *createCacheHandler(QObject *parent);
};

} // namespace Communication
} // namespace Client
} // namespace AW

#endif // AW_CLIENT_COMMUNICATION_AWDEFAULTNETWORKREQUESTCACHEDELEGATE_H
