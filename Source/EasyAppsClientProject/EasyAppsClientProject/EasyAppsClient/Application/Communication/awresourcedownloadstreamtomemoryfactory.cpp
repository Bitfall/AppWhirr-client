#include "awresourcedownloadstreamtomemoryfactory.h"

#include "awdefaultcommunicationresourcegatewaycachedelegate.h"
#include "Settings/configmanager.h"

namespace AW {
namespace Client {
namespace Communication {

AWResourceDownloadStreamToMemoryFactory::AWResourceDownloadStreamToMemoryFactory()
{
}

Common::Communication::ResourceDownloadStreamToMemory *AWResourceDownloadStreamToMemoryFactory::createDefaultResourceDownloadStreamToMemory(QObject *parent)
{
    AWDefaultCommunicationResourceGatewayCacheDelegate *gatewayCacheDelegate = new AWDefaultCommunicationResourceGatewayCacheDelegate(NULL);
    AW::Common::Communication::ResourceDownloadStreamToMemory *downloadStreamToMemory = new AW::Common::Communication::ResourceDownloadStreamToMemory(gatewayCacheDelegate, ConfigManager::getNetworkDefaultMaximumParallelRequests(), parent);
    gatewayCacheDelegate->setParent(downloadStreamToMemory); // will be released with the download-stream-to-file object
    return downloadStreamToMemory;
}

} // namespace Communication
} // namespace Client
} // namespace AW
