#include "grablistnetworkrequest.h"

#include "Application/Communication/Response/applicationlistnetworkresponse.h"

#include <QString>

GrabListNetworkRequest::GrabListNetworkRequest() :
    BaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
                        "http://easyapps.dyndns-server.com/app_dev.php/api/grab/list.json",
                        true)
{
}

QString GrabListNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* GrabListNetworkRequest::getResponseParser() {
    return new ApplicationListNetworkResponse("[" + QString(responseBody) + "]");
}
