#include "usersuggestapprequest.h"

#include "Common/Json/json.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


UserSuggestAppRequest::UserSuggestAppRequest(QString name, QString website, QString description) :
    AWBaseNetworkRequest(
        BaseNetworkRequest::BaseCommunicationRequestTypePost,
    #ifdef USE_PUBLIC_PRODUCTION_SERVER
        QUrl("https://api.appwhirr.com/social/suggest"),
    #else
        QUrl("http://easyapps.dyndns-server.com:8042/social/suggest"),
    #endif
        RST_AuthorizationRequired),
    _name(name),
    _website(website),
    _description(description)
{
}

QString UserSuggestAppRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse* UserSuggestAppRequest::getResponseParser() {
    return NULL;
}

QByteArray UserSuggestAppRequest::getPostData() const
{
    QVariantList suggestList;

    QVariantMap suggest;
    suggest["name"] = _name;
    suggest["website"] = _website;
    suggest["summary"] = _description;

    suggestList.append(suggest);
    QByteArray serializedSuggest = QtJson::Json::serialize(suggestList);

    QByteArray finalPostData = QByteArray("suggest=") + serializedSuggest;
    return finalPostData;
}
