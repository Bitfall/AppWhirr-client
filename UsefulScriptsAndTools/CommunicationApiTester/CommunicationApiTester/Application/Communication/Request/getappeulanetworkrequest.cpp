#include "getappeulanetworkrequest.h"

#include "Common/Communication/basenetworkresponse.h"
#include "Application/Communication/Response/getappeulanetworkresponse.h"

GetAppEulaNetworkRequest::GetAppEulaNetworkRequest(QString appId) :
    BaseNetworkRequest(
        BaseNetworkRequest::BaseCommunicationRequestTypeGet,
        "http://easyapps.dyndns-server.com/app_dev.php/api/application/eula/" + appId + ".json")
{
}

QString GetAppEulaNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* GetAppEulaNetworkRequest::getResponseParser() {
    return new GetAppEulaNetworkResponse("[" + QString(responseBody) + "]");
}
