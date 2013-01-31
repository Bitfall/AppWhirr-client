#include "applicationsearchnetworkrequest.h"

#include "Application/Communication/Response/applicationlistnetworkresponse.h"
#include "Common/Communication/basenetworkresponse.h"
#include "Common/Helpers/platformflagshelper.h"
#include "Common/System/systemprofiler.h"

#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


ApplicationSearchNetworkRequest::ApplicationSearchNetworkRequest(QString searchKeyword) :
    AWBaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypePost,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
                        QUrl("https://api.appwhirr.com/application/search"),
#else
                        QUrl("http://easyapps.dyndns-server.com:8042/application/search"),
#endif
                        RST_BaseSecurity),
    _searchKeyword(searchKeyword)
{
}

QString ApplicationSearchNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

void ApplicationSearchNetworkRequest::addAdditionalRequiredHeaders(QNetworkRequest *networkRequest) {
    PlatformFlagsHelper::SummedSinglePlatformInformations *systemInfo = SystemProfiler::sharedSystemProfiler()->_getCurrentSystemPlatformInformations();
    if(systemInfo == NULL) {
        WLog("Cannot get the required system information.");
        return;
    }

    networkRequest->setRawHeader("osmainflag", QString::number((quint32)systemInfo->getMainOsFlag()).toAscii());
    networkRequest->setRawHeader("ossubflag", QString::number(systemInfo->_getSummedSubFlags()).toAscii());
}

AW::Common::Communication::Background_BaseNetworkResponse* ApplicationSearchNetworkRequest::getResponseParser() {
    return new ApplicationListNetworkResponse( QString::fromUtf8(_responseBody) );
}

QByteArray ApplicationSearchNetworkRequest::getPostData() const
{
    QByteArray encodedSearchKeyword = QUrl::toPercentEncoding(_searchKeyword, QByteArray(), " -._~");
    return QByteArray("query=") + encodedSearchKeyword;
}
