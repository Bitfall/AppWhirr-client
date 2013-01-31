#include "applistversioninfonetworkrequest.h"

#include "Application/Communication/Response/applicationlistnetworkresponse.h"
#include "Common/Communication/basenetworkresponse.h"
#include "Common/Helpers/platformflagshelper.h"
#include "Common/System/systemprofiler.h"

#include "Common/Json/json.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


AppListVersionInfoNetworkRequest::AppListVersionInfoNetworkRequest(QList<QString> appIdList) :
    AWBaseNetworkRequest( BaseNetworkRequest::BaseCommunicationRequestTypePost,
#ifdef USE_PUBLIC_PRODUCTION_SERVER
                        QUrl("https://api.appwhirr.com/user/updatelist"),
#else
                        QUrl("http://easyapps.dyndns-server.com:8042/user/updatelist"),
#endif
                        RST_BaseSecurity),
    _appIdList(appIdList)
{
}

QString AppListVersionInfoNetworkRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse *AppListVersionInfoNetworkRequest::getResponseParser() {
    return new ApplicationListNetworkResponse( QString::fromUtf8(_responseBody) );
}

QByteArray AppListVersionInfoNetworkRequest::getPostData() const {

    QVariantList listOfAppIdsForJSonSerialize;
    for(int i = 0; i < _appIdList.size(); ++i) {
        QVariantMap iMap;
        iMap["applicationID"] = _appIdList[i];
        listOfAppIdsForJSonSerialize.append(iMap);
    }

    QByteArray serializedAppIdList = QtJson::Json::serialize(listOfAppIdsForJSonSerialize);

    QByteArray finalPostData = QByteArray("applications=") + serializedAppIdList;
    DLog("-- test final post data: ") << finalPostData;
    return finalPostData;
}
