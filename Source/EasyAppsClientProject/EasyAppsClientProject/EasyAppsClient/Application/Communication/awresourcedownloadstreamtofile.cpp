#include "awresourcedownloadstreamtofile.h"

#include "awdefaultcommunicationresourcegatewaycachedelegate.h"
#include "Settings/configmanager.h"

namespace AW {
namespace Client {
namespace Communication {

Common::Communication::ResourceDownloadStreamToFile *AWResourceDownloadStreamToFile::createDefaultResourceDownloadStreamToFile(QObject *parent)
{
    AWDefaultCommunicationResourceGatewayCacheDelegate *gatewayCacheDelegate = new AWDefaultCommunicationResourceGatewayCacheDelegate(NULL);

    AW::Common::Communication::ResourceDownloadStreamToFile *downloadStreamToFile = new AW::Common::Communication::ResourceDownloadStreamToFile(gatewayCacheDelegate, ConfigManager::getNetworkDefaultMaximumParallelRequests(), parent);

    gatewayCacheDelegate->setParent(downloadStreamToFile); // will be released with the download-stream-to-file object
    return downloadStreamToFile;
}

} // namespace Communication
} // namespace Client
} // namespace AW
