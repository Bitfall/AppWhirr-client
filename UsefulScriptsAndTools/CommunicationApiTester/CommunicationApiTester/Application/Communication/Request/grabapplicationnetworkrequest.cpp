#include "grabapplicationnetworkrequest.h"

#include "Application/Communication/Response/grabapplicationnetworkresponse.h"

#include <QString>

GrabApplicationNetworkRequest::GrabApplicationNetworkRequest(const QString& applicationID, QString grabType) :
    BaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
                        "http://easyapps.dyndns-server.com/app_dev.php/api/grab/" + applicationID + "/" + grabType + ".json",
                        true)
{
}

QString GrabApplicationNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* GrabApplicationNetworkRequest::getResponseParser() {
    return new GrabApplicationNetworkResponse("[" + QString(responseBody) + "]");
}
