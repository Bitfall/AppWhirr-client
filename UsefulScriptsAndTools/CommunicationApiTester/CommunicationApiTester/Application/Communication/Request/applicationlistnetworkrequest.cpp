#include "applicationlistnetworkrequest.h"

#include "Common/Communication/basenetworkresponse.h"
#include "Application/Communication/Response/applicationlistnetworkresponse.h"

ApplicationListNetworkRequest::ApplicationListNetworkRequest() :
    BaseNetworkRequest(
        BaseNetworkRequest::BaseCommunicationRequestTypeGet,
        "http://easyapps.dyndns-server.com/app_dev.php/api/daily.json")
{
}

QString ApplicationListNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* ApplicationListNetworkRequest::getResponseParser() {
    return new ApplicationListNetworkResponse("[" + QString(responseBody) + "]");
}
