#include "userfeedbackrequest.h"

#include "Common/Json/json.h"
#include <QUrl>

#include "Settings/configmanager.h"


using namespace AW::Client::Communication;


UserFeedbackRequest::UserFeedbackRequest(QString feedbackMessage) :
    AWBaseNetworkRequest(
        BaseNetworkRequest::BaseCommunicationRequestTypePost,
    #ifdef USE_PUBLIC_PRODUCTION_SERVER
        QUrl("https://api.appwhirr.com/social/feedback"),
    #else
        QUrl("http://easyapps.dyndns-server.com:8042/social/feedback"),
    #endif
        RST_AuthorizationRequired),
    _feedback(feedbackMessage)
{
}

QString UserFeedbackRequest::getRequestVersion() const {
    return ConfigManager::getServerApiVersion();
}

AW::Common::Communication::Background_BaseNetworkResponse* UserFeedbackRequest::getResponseParser() {
    return NULL;
}

QByteArray UserFeedbackRequest::getPostData() const
{
    QVariantList feedbackList;

    QVariantMap feedback;
    feedback["message"] = _feedback;

    feedbackList.append(feedback);
    QByteArray serializedFeedback = QtJson::Json::serialize(feedbackList);

    QByteArray finalPostData = QByteArray("feedback=") + serializedFeedback;
    return finalPostData;
}
