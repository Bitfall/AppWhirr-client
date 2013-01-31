#include "apitestnetworkrequest.h"

#include "Application/Communication/Response/apitestnetworkresponse.h"

ApiTestNetworkRequest::ApiTestNetworkRequest(BaseCommunicationRequestType requestType, const QString& requestUrl, QMap<QString, QString> additionalHeaderKeyValueMap, bool securedRequest, const QByteArray& postData, bool saveResponseToFile, QNetworkRequest::CacheLoadControl cachePolicy, int timeoutInterval) :
    BaseNetworkRequest(requestType, requestUrl, securedRequest, postData, saveResponseToFile, cachePolicy, timeoutInterval),
    _additionalHeaderKeyValueMap(additionalHeaderKeyValueMap)
{
}

QString ApiTestNetworkRequest::getRequestVersion() const {
    return "1.0";
}

Background_BaseNetworkResponse* ApiTestNetworkRequest::getResponseParser() {
    return new ApiTestNetworkResponse("[" + QString(responseBody) + "]");
}

void ApiTestNetworkRequest::addAdditionalRequiredHeaders(QNetworkRequest *networkRequest)
{
    DLog("--addAdditionalRequiredHeaders");
    QMapIterator<QString, QString> it(_additionalHeaderKeyValueMap);
    while(it.hasNext()) {
        it.next();

        DLog("Additional header: ") << it.key().toAscii() << it.value().toAscii();

        networkRequest->setRawHeader(it.key().toAscii(), it.value().toAscii());
    }
}
