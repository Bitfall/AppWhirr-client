#include "storemainpagewithsectionsnetworkrequest.h"

#include "Application/Communication/Response/applicationlistwithsectionsnetworkresponse.h"

StoreMainPageWithSectionsNetworkRequest::StoreMainPageWithSectionsNetworkRequest() :
    BaseNetworkRequest(
        BaseNetworkRequest::BaseCommunicationRequestTypeGet,
        "http://easyapps.dyndns-server.com/app_dev.php/api/daily.json")
{
}

QString StoreMainPageWithSectionsNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* StoreMainPageWithSectionsNetworkRequest::getResponseParser() {
    return new ApplicationListWithSectionsNetworkResponse("[" + QString(responseBody) + "]");
}
