#include "resourcedownloadnetworkrequest.h"

ResourceDownloadNetworkRequest::ResourceDownloadNetworkRequest(BaseCommunicationRequestType requestType, const QString& requestUrl, bool securedRequest, const QByteArray& postData, bool saveResponseToFile, QNetworkRequest::CacheLoadControl cachePolicy, int timeoutInterval) :
    BaseNetworkRequest(requestType, requestUrl, securedRequest, postData, saveResponseToFile, cachePolicy, timeoutInterval)
{
}

QString ResourceDownloadNetworkRequest::getRequestVersion() const {
    return "1.0";
}
