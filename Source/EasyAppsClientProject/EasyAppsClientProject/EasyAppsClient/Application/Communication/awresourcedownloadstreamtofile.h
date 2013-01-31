#ifndef AW_CLIENT_COMMUNICATION_AWRESOURCEDOWNLOADSTREAMTOFILE_H
#define AW_CLIENT_COMMUNICATION_AWRESOURCEDOWNLOADSTREAMTOFILE_H

#include "Common/Communication/resourcedownloadstreamtofile.h"

namespace AW {
namespace Client {
namespace Communication {

class AWResourceDownloadStreamToFile
{
private:
    AWResourceDownloadStreamToFile() {}

public:
    
    /*! Creates the default AppWhirr resource-download-stream-to-file object (with default resource cache handling)
    */
    static AW::Common::Communication::ResourceDownloadStreamToFile *createDefaultResourceDownloadStreamToFile(QObject *parent);
};

} // namespace Communication
} // namespace Client
} // namespace AW

#endif // AW_CLIENT_COMMUNICATION_AWRESOURCEDOWNLOADSTREAMTOFILE_H
