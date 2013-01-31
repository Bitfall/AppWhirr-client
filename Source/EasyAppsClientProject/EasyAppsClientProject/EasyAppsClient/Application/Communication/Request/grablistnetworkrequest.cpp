#include "grablistnetworkrequest.h"

#include "Application/Communication/Response/applicationlistnetworkresponse.h"

#include <QString>
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


GrabListNetworkRequest::GrabListNetworkRequest() :
    AWBaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
                        QUrl("https://api.appwhirr.com/user/grablist"),
#else
                        QUrl("http://easyapps.dyndns-server.com:8042/user/grablist"),
#endif
                        RST_AuthorizationRequired)
{
}

QString GrabListNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse* GrabListNetworkRequest::getResponseParser() {
    return new ApplicationListNetworkResponse( QString::fromUtf8(_responseBody) );
}
