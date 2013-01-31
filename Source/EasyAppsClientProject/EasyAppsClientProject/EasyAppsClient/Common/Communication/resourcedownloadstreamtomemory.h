#ifndef AW_COMMON_COMMUNICATION_RESOURCEDOWNLOADSTREAMTOMEMORY_H
#define AW_COMMON_COMMUNICATION_RESOURCEDOWNLOADSTREAMTOMEMORY_H

#include <QObject>

#include "Common/Communication/communicationresourcegateway.h"
#include "Common/Communication/communicationresourcegatewaydelegate.h"

namespace AW {
namespace Common {
namespace Communication {

class ResourceDownloadStreamToMemory : public QObject, public CommunicationResourceGatewayDelegate
{
    Q_OBJECT
public:
    explicit ResourceDownloadStreamToMemory(CommunicationResourceGatewayCacheDelegate *commResourceGatewayCacheDelegate, unsigned int maxParallelRequestsCount = 3, QObject *parent = 0);


    void addResourceDownloadRequest(QUrl url);
    void startDownloadingResources();

Q_SIGNALS:
    void resourceDownloadProgressForUrl(int progress, QUrl url);
    /*! Download failed for resource
    */
    void resourceDownloadFailedForUrl(QUrl url);
    /*! Will be emitted when the given resource is 100% retrieved
    */
    void resourceDownloadFinishedWithSuccess(QUrl url, QByteArray resourceBytes);
    /*! Emits when all resources were retrieved
      */
    void finishedWithAllResourceDownloads();


private Q_SLOTS:
    // --------------------------------------------
    // resource network request handling
    void _downloadProgressForUrl(int percent, QUrl url);
    void _resourceDownloadFailedForUrl(QUrl url);
    void _resourceChunkReceivedForUrl(QUrl url, QByteArray resourceChunk, bool isBufferDownloadedResourceAndSendItWhenDownloadFinished);
    void _finishedResourceDownloading(QUrl url);

    void _allNetworkRequestsFinished();


    // --------------------------------------------
    // CommunicationResourceGatewayDelegate
public:
    virtual BaseNetworkRequest *createResourceDownloadRequest(BaseNetworkRequest::BaseCommunicationRequestType requestType,
                                                              QUrl requestUrl,
                                                              QNetworkRequest::CacheLoadControl cacheControl,
                                                              int timeoutInterval,
                                                              NetworkRequestCacheDelegate *requestCacheDelegate);
    
private:
    CommunicationResourceGateway *_commResourceGateway;
};

} // namespace Communication
} // namespace Common
} // namespace AW

#endif // AW_COMMON_COMMUNICATION_RESOURCEDOWNLOADSTREAMTOMEMORY_H
