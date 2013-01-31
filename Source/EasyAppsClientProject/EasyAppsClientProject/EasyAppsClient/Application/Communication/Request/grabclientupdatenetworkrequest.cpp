#include "grabclientupdatenetworkrequest.h"

#include "Application/Communication/Response/grabapplicationnetworkresponse.h"
#include "Common/Communication/basenetworkresponse.h"

#include "Common/System/systemprofiler.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


GrabClientUpdateNetworkRequest::GrabClientUpdateNetworkRequest() :
    AWBaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
                        QUrl("https://api.appwhirr.com/client/update"),
#else
                        QUrl("http://easyapps.dyndns-server.com:8042/client/update"),
#endif
                        RST_BaseSecurity )
{
}

QString GrabClientUpdateNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse* GrabClientUpdateNetworkRequest::getResponseParser() {
    return new GrabApplicationNetworkResponse( QString::fromUtf8(_responseBody) );
}

void GrabClientUpdateNetworkRequest::addAdditionalRequiredHeaders(QNetworkRequest *networkRequest)
{
    PlatformFlagsHelper::SummedSinglePlatformInformations *systemInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
    if(systemInfo == NULL) {
        WLog("Cannot get the required system information.");
        return;
    }
    networkRequest->setRawHeader("osmainflag", QString::number((quint32)systemInfo->getMainOsFlag()).toAscii());
    networkRequest->setRawHeader("ossubflag", QString::number(systemInfo->_getSummedSubFlags()).toAscii());
}
