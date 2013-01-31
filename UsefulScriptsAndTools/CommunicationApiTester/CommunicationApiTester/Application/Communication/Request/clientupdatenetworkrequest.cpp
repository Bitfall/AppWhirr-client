#include "clientupdatenetworkrequest.h"

#include "Application/Communication/Response/applicationnetworkresponse.h"
#include "Common/Communication/basenetworkresponse.h"

ClientUpdateNetworkRequest::ClientUpdateNetworkRequest() :
    BaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
                        "http://easyapps.dyndns-server.com/app_dev.php/api/update.json")
{
}

QString ClientUpdateNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* ClientUpdateNetworkRequest::getResponseParser() {
    return new ApplicationNetworkResponse("[" + QString(responseBody) + "]");
}
