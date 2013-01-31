#include "getappeulanetworkrequest.h"

#include "Common/Communication/basenetworkresponse.h"
#include "Application/Communication/Response/getappeulanetworkresponse.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


GetAppEulaNetworkRequest::GetAppEulaNetworkRequest(QString appId) :
    AWBaseNetworkRequest(
        BaseNetworkRequest::BaseCommunicationRequestTypeGet,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
        QUrl("https://api.appwhirr.com/application/eula/" + appId),
#else
        QUrl("http://easyapps.dyndns-server.com:8042/application/eula/" + appId),
#endif
        RST_BaseSecurity )
{
}

QString GetAppEulaNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse* GetAppEulaNetworkRequest::getResponseParser() {
    return new GetAppEulaNetworkResponse( QString::fromUtf8(_responseBody) );
}
