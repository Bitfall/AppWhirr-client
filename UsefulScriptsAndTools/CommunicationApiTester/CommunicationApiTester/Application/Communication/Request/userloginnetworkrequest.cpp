#include "userloginnetworkrequest.h"

UserLoginNetworkRequest::UserLoginNetworkRequest() :
    BaseNetworkRequest(
        BaseNetworkRequest::BaseCommunicationRequestTypeGet,
        "http://easyapps.dyndns-server.com/app_dev.php/api/login.json",
        true)
{
}

QString UserLoginNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* UserLoginNetworkRequest::getResponseParser() {
    return NULL;
}
