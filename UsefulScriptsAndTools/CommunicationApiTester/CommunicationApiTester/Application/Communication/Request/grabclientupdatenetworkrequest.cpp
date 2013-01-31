#include "grabclientupdatenetworkrequest.h"

#include "Application/Communication/Response/grabapplicationnetworkresponse.h"
#include "Common/Communication/basenetworkresponse.h"

GrabClientUpdateNetworkRequest::GrabClientUpdateNetworkRequest() :
    BaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
                        "http://easyapps.dyndns-server.com/app_dev.php/api/update/grab.json")
{
}

QString GrabClientUpdateNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* GrabClientUpdateNetworkRequest::getResponseParser() {
    return new GrabApplicationNetworkResponse("[" + QString(responseBody) + "]");
}
