#include "applicationlistnetworkrequest.h"
#include "Common/System/systemprofiler.h"

#include "Application/Communication/Response/applicationlistwithsectionsnetworkresponse.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


ApplicationListNetworkRequest::ApplicationListNetworkRequest(QString listType) :
    AWBaseNetworkRequest(
    BaseNetworkRequest::BaseCommunicationRequestTypeGet,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
    QUrl("https://api.appwhirr.com/application/list"),
#else
    QUrl("http://easyapps.dyndns-server.com:8042/application/list"),
#endif
    RST_BaseSecurity),

    _listType(listType)
{
}

QString ApplicationListNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

void ApplicationListNetworkRequest::addAdditionalRequiredHeaders(QNetworkRequest *networkRequest) {
    PlatformFlagsHelper::SummedSinglePlatformInformations *systemInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
    if(systemInfo == NULL) {
        WLog("Cannot get the required system information.");
        return;
    }

    networkRequest->setRawHeader("osmainflag", QString::number((quint32)systemInfo->getMainOsFlag()).toAscii());
    networkRequest->setRawHeader("ossubflag", QString::number(systemInfo->_getSummedSubFlags()).toAscii());
    networkRequest->setRawHeader("listtype", _listType.toAscii());
}

AW::Common::Communication::Background_BaseNetworkResponse* ApplicationListNetworkRequest::getResponseParser() {
    return new ApplicationListWithSectionsNetworkResponse( QString::fromUtf8(_responseBody) );
}
