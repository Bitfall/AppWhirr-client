#include "applicationnetworkrequest.h"
#include "Application/Communication/Response/applicationnetworkresponse.h"

ApplicationNetworkRequest::ApplicationNetworkRequest(const QString& applicationID) :
    BaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
                        "http://easyapps.dyndns-server.com/app_dev.php/api/application/info/" + applicationID + ".json")
{
}

QString ApplicationNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* ApplicationNetworkRequest::getResponseParser() {
    return new ApplicationNetworkResponse("[" + QString(responseBody) + "]");
}

