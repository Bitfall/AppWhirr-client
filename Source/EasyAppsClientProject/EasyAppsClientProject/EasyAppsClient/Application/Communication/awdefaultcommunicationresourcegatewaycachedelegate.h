#ifndef AW_CLIENT_COMMUNICATION_AWDEFAULTCOMMUNICATIONRESOURCEGATEWAYCACHEDELEGATE_H
#define AW_CLIENT_COMMUNICATION_AWDEFAULTCOMMUNICATIONRESOURCEGATEWAYCACHEDELEGATE_H

#include <QObject>
#include "Common/Communication/communicationresourcegatewaycachedelegate.h"

namespace AW {
namespace Client {
namespace Communication {

class AWDefaultCommunicationResourceGatewayCacheDelegate : public QObject, public AW::Common::Communication::CommunicationResourceGatewayCacheDelegate
{
    Q_OBJECT
public:
    explicit AWDefaultCommunicationResourceGatewayCacheDelegate(QObject *parent = 0);
    
    virtual QNetworkRequest::CacheLoadControl getCacheControlForResourceUrl(QUrl url);

    /*! Cache delegate for a given resource request

        @return Return NULL (by default) if no cache delegate should be applied for the request
    */
    virtual AW::Common::Communication::NetworkRequestCacheDelegate *getRequestCacheDelegate();
};

} // namespace Communication
} // namespace Client
} // namespace AW

#endif // AW_CLIENT_COMMUNICATION_AWDEFAULTCOMMUNICATIONRESOURCEGATEWAYCACHEDELEGATE_H
