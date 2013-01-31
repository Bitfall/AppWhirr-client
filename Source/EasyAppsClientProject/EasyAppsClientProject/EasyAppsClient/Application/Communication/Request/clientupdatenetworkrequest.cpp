#include "clientupdatenetworkrequest.h"

#include "Application/Communication/Response/clientupdatechecknetworkresponse.h"
#include "Common/Communication/basenetworkresponse.h"

#include "Common/System/systemprofiler.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


ClientUpdateNetworkRequest::ClientUpdateNetworkRequest() :
    AWBaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
                        QUrl("https://api.appwhirr.com/client/check"),
#else
                        QUrl("http://easyapps.dyndns-server.com:8042/client/check"),
#endif
                        RST_BaseSecurity )
{
}

QString ClientUpdateNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse* ClientUpdateNetworkRequest::getResponseParser() {
    return new ClientUpdateCheckNetworkResponse( QString::fromUtf8(_responseBody) );
}

void ClientUpdateNetworkRequest::addAdditionalRequiredHeaders(QNetworkRequest *networkRequest)
{
    PlatformFlagsHelper::SummedSinglePlatformInformations *systemInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
    if(systemInfo == NULL) {
        WLog("Cannot get the required system information.");
        return;
    }
    networkRequest->setRawHeader("osmainflag", QString::number((quint32)systemInfo->getMainOsFlag()).toAscii());
    networkRequest->setRawHeader("ossubflag", QString::number(systemInfo->_getSummedSubFlags()).toAscii());
}
