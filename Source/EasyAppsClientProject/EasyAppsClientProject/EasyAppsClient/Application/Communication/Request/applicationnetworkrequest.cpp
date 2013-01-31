#include "applicationnetworkrequest.h"

#include "Application/Communication/Response/applicationnetworkresponse.h"
#include "Common/System/systemprofiler.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


ApplicationNetworkRequest::ApplicationNetworkRequest(const QString& applicationID) :
    AWBaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
                        QUrl("https://api.appwhirr.com/application/information/" + applicationID),
#else
                        QUrl("http://easyapps.dyndns-server.com:8042/application/information/" + applicationID),
#endif
                        RST_BaseSecurity )
{
}

QString ApplicationNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse* ApplicationNetworkRequest::getResponseParser() {
    return new ApplicationNetworkResponse( QString::fromUtf8(_responseBody) );
}

void ApplicationNetworkRequest::addAdditionalRequiredHeaders(QNetworkRequest *networkRequest)
{
    PlatformFlagsHelper::SummedSinglePlatformInformations *systemInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
    if(systemInfo == NULL) {
        WLog("Cannot get the required system information.");
        return;
    }
    networkRequest->setRawHeader("osmainflag", QString::number((quint32)systemInfo->getMainOsFlag()).toAscii());
    networkRequest->setRawHeader("ossubflag", QString::number(systemInfo->_getSummedSubFlags()).toAscii());
}

