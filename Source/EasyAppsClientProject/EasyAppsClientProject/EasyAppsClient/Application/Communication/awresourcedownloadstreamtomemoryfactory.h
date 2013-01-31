#ifndef AW_CLIENT_COMMUNICATION_AWRESOURCEDOWNLOADSTREAMTOMEMORYFACTORY_H
#define AW_CLIENT_COMMUNICATION_AWRESOURCEDOWNLOADSTREAMTOMEMORYFACTORY_H

#include "Common/Communication/resourcedownloadstreamtomemory.h"

namespace AW {

namespace Client {
namespace Communication {

class AWResourceDownloadStreamToMemoryFactory
{
private:
    AWResourceDownloadStreamToMemoryFactory();

public:
    /*! Creates the default AppWhirr resource-download-stream-to-file object (with default resource cache handling)
    */
    static AW::Common::Communication::ResourceDownloadStreamToMemory *createDefaultResourceDownloadStreamToMemory(QObject *parent);
};

} // namespace Communication
} // namespace Client
} // namespace AW

#endif // AW_CLIENT_COMMUNICATION_AWRESOURCEDOWNLOADSTREAMTOMEMORYFACTORY_H
