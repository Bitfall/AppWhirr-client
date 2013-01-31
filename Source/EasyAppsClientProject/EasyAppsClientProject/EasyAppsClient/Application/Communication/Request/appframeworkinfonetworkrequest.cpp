#include "appframeworkinfonetworkrequest.h"

#include "Application/Communication/Response/appframeworkinfonetworkresponse.h"
#include "Common/System/systemprofiler.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


AppFrameworkInfoNetworkRequest::AppFrameworkInfoNetworkRequest(QString appId) :
    AWBaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
                        QUrl("https://api.appwhirr.com/user/framework/" + appId),
#else
                        QUrl("http://easyapps.dyndns-server.com:8042/user/framework/" + appId),
#endif
                        RST_AuthorizationRequired)
{
}

QString AppFrameworkInfoNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse* AppFrameworkInfoNetworkRequest::getResponseParser() {
    return new AppFrameworkInfoNetworkResponse( QString::fromUtf8(_responseBody) );
}

void AppFrameworkInfoNetworkRequest::addAdditionalRequiredHeaders(QNetworkRequest *networkRequest)
{
    PlatformFlagsHelper::SummedSinglePlatformInformations *systemInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
    if(systemInfo == NULL) {
        WLog("Cannot get the required system information.");
        return;
    }
    networkRequest->setRawHeader("osmainflag", QString::number((quint32)systemInfo->getMainOsFlag()).toAscii());
    networkRequest->setRawHeader("ossubflag", QString::number(systemInfo->_getSummedSubFlags()).toAscii());
}

