#include "storemainpagewithsectionsnetworkrequest.h"
#include "Common/System/systemprofiler.h"

#include "Application/Communication/Response/applicationlistwithsectionsnetworkresponse.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


StoreMainPageWithSectionsNetworkRequest::StoreMainPageWithSectionsNetworkRequest() :
    AWBaseNetworkRequest(
        BaseNetworkRequest::BaseCommunicationRequestTypeGet,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
        QUrl("https://api.appwhirr.com/application/daily"),
#else
        QUrl("http://easyapps.dyndns-server.com:8042/application/daily"),
#endif
        RST_BaseSecurity )
{
}

QString StoreMainPageWithSectionsNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

void StoreMainPageWithSectionsNetworkRequest::addAdditionalRequiredHeaders(QNetworkRequest *networkRequest) {
    PlatformFlagsHelper::SummedSinglePlatformInformations *systemInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
    if(systemInfo == NULL) {
        WLog("Cannot get the required system information.");
        return;
    }

    networkRequest->setRawHeader("osmainflag", QString::number((quint32)systemInfo->getMainOsFlag()).toAscii());
    networkRequest->setRawHeader("ossubflag", QString::number(systemInfo->_getSummedSubFlags()).toAscii());
}

AW::Common::Communication::Background_BaseNetworkResponse* StoreMainPageWithSectionsNetworkRequest::getResponseParser() {
    return new ApplicationListWithSectionsNetworkResponse( QString::fromUtf8(_responseBody) );
}
