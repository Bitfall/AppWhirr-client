#include "applicationsearchnetworkrequest.h"

#include "Application/Communication/Response/applicationlistnetworkresponse.h"
#include "Common/Communication/basenetworkresponse.h"

ApplicationSearchNetworkRequest::ApplicationSearchNetworkRequest(const QString& keyword) :
    BaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
                        "http://easyapps.dyndns-server.com/app_dev.php/api/search/" + keyword + ".json")
{
}

QString ApplicationSearchNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* ApplicationSearchNetworkRequest::getResponseParser() {
    return new ApplicationListNetworkResponse("[" + QString(responseBody) + "]");
}
