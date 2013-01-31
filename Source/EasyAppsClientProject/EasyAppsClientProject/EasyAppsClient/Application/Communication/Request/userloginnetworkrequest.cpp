#include "userloginnetworkrequest.h"

#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


UserLoginNetworkRequest::UserLoginNetworkRequest() :
    AWBaseNetworkRequest(
        BaseNetworkRequest::BaseCommunicationRequestTypeGet,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
        QUrl("https://api.appwhirr.com/user/login"),
#else
        QUrl("http://easyapps.dyndns-server.com:8042/user/login"),
#endif
        RST_AuthorizationRequired)
{
}

QString UserLoginNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse* UserLoginNetworkRequest::getResponseParser() {
    return NULL;
}
