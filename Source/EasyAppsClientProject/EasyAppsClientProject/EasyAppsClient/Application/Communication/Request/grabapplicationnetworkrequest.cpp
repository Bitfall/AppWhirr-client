#include "grabapplicationnetworkrequest.h"

#include "Application/Communication/Response/grabapplicationnetworkresponse.h"

#include <QString>
#include "Common/System/systemprofiler.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


GrabApplicationNetworkRequest::GrabApplicationNetworkRequest(QString appId, QString grabType, RequestSecurityTypeEnum requestSecurityType) :
    AWBaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypeGet,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
                        QUrl("https://api.appwhirr.com/user/grab/" + appId),
#else
                        QUrl("http://easyapps.dyndns-server.com:8042/user/grab/" + appId),
#endif
                        requestSecurityType ),

    _grabType(grabType)
{
}

QString GrabApplicationNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse* GrabApplicationNetworkRequest::getResponseParser() {
    return new GrabApplicationNetworkResponse( QString::fromUtf8(_responseBody) );
}

void GrabApplicationNetworkRequest::addAdditionalRequiredHeaders(QNetworkRequest *networkRequest)
{
    PlatformFlagsHelper::SummedSinglePlatformInformations *systemInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
    if(systemInfo == NULL) {
        WLog("Cannot get the required system information.");
        return;
    }
    networkRequest->setRawHeader("osmainflag", QString::number((quint32)systemInfo->getMainOsFlag()).toAscii());
    networkRequest->setRawHeader("ossubflag", QString::number(systemInfo->_getSummedSubFlags()).toAscii());
    networkRequest->setRawHeader("behaviour", _grabType.toAscii());

//    DLog("-- test auth info: ") << networkRequest->rawHeader("username") << networkRequest->rawHeader("password");
}
