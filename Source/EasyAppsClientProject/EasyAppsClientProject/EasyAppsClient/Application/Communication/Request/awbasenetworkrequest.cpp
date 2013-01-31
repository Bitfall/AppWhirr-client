#include "awbasenetworkrequest.h"

#include "Settings/configmanager.h"
#include "Common/Security/authorization.h"
#include "Common/System/systemprofiler.h"

namespace AW {
namespace Client {
namespace Communication {

AWBaseNetworkRequest::AWBaseNetworkRequest(BaseCommunicationRequestType requestType,
                                           QUrl requestUrlString,
                                           RequestSecurityTypeEnum requestSecurityType,
                                           QNetworkRequest::CacheLoadControl cacheControl,
                                           int timeoutInterval,
                                           bool isBufferDownloadedResourceAndSendItWhenDownloadFinished,
                                           AW::Common::Communication::NetworkRequestCacheDelegate *requestCacheDelegate) :
    AW::Common::Communication::BaseNetworkRequest(requestType, requestUrlString, cacheControl, timeoutInterval, isBufferDownloadedResourceAndSendItWhenDownloadFinished, requestCacheDelegate),
    _requestSecurityType(requestSecurityType)
{
}

AWBaseNetworkRequest::~AWBaseNetworkRequest()
{
}

QString AWBaseNetworkRequest::getUserAgent()
{
    QString appName = "EasyApps";
    QString versionNumber = ConfigManager::getClientVersion();
    QString operationSystemIdentifier = "deprecated";
    unsigned int currentSystemSubFlags = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemSubFlagsAsUInt();

    QString userAgentContent = QString(appName + ";" +
                                       versionNumber + ";" +
                                       operationSystemIdentifier + ";" +
                                       "0f0bc00f072bd8d1d34ea3407d964387" + ";" +
                                       QString::number(currentSystemSubFlags) + ";");

#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("User agent's content: ") << userAgentContent;
#endif

    return userAgentContent;
}

bool AWBaseNetworkRequest::appendHeadersToRequest() {
    if (_requestSecurityType == RST_AuthorizationRequired)
    {
        // include the required authorization information

        // Set Credentials
        if (!Authorization::sharedAuthorization()->addHttpBasicAuthentication(_networkRequest)) {
            // Failed to add credentials
            // Show the login form

            // Todo: Tul koran jon fel es vegtelen ciklust okoz - ???
            Q_EMIT authenticationChallenge();

            return false;
        }
    }

    // Set User-Agent
    _networkRequest->setRawHeader("User-Agent", this->getUserAgent().toAscii());
    _networkRequest->setRawHeader("version", this->getRequestVersion().toAscii());

    this->addAdditionalRequiredHeaders(_networkRequest);

    return true;
}

} // namespace Communication
} // namespace Client
} // namespace AW
