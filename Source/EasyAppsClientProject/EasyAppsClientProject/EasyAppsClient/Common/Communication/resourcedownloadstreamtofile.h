#ifndef AW_COMMON_COMMUNICATION_RESOURCEDOWNLOADSTREAMTOFILE_H
#define AW_COMMON_COMMUNICATION_RESOURCEDOWNLOADSTREAMTOFILE_H

#include <QObject>

#include "communicationresourcegateway.h"
#include "communicationresourcegatewaydelegate.h"

namespace AW {
namespace Common {

namespace Utils {
class StreamToFileManager;
}

namespace Communication {

class ResourceDownloadStreamToFile : public QObject, public CommunicationResourceGatewayDelegate
{
    Q_OBJECT
public:
    explicit ResourceDownloadStreamToFile(CommunicationResourceGatewayCacheDelegate *commResourceGatewayCacheDelegate, unsigned int maxParallelRequestsCount = 3, QObject *parent = 0);

    /*! Adds the resource for downloading.

        Tries to open and reserve the file at \a localTargetFilePath immediatelly and will return false if it cannot open the file for streaming. In this case it won't add the url for downloading. \n
        If it returns true it means it could open the local-target-file for streaming and the url is added for downloading.

        @note If it returns true it only means that the local-target-file could be opened for streaming. It doesn't check the url at all.

        @param isRemoveLocalTargetFileIfExists : if true it will check whether the local-target-file exists and will remove it before creating a new one for streaming. If it's false then the streaming will be \a appended to the local-target-file and it's previous content won't cleared.
    */
    bool addAndInitializeResourceDownloadRequest(QUrl url, QString localTargetFilePath, bool isRemoveLocalTargetFileIfExists);

    /*! Starts the downloading and streaming into file of previously added resources
    */
    void startDownloadingResources();

Q_SIGNALS:
    void resourceDownloadProgressForUrl(int progress, QUrl url);
    /*! Download failed for resource
    */
    void resourceDownloadFailed(QUrl url, QString localFilePath, QString errorMessage);
    /*! Will be emitted when the given resource is 100% retrieved
    */
    void resourceDownloadFinishedWithSuccess(QUrl url, QString localFilePath);
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
    // file-stream handling
    void _streamToFileFailedWithError(QString filePath, QString errorMessage);
    void _streamToFileFinished(QString filePath);

    // --------------------------------------------
    // CommunicationResourceGatewayDelegate
public:
    virtual BaseNetworkRequest *createResourceDownloadRequest(BaseNetworkRequest::BaseCommunicationRequestType requestType,
                                                              QUrl requestUrl,
                                                              QNetworkRequest::CacheLoadControl cacheControl,
                                                              int timeoutInterval,
                                                              NetworkRequestCacheDelegate *requestCacheDelegate);

    // --------------------------------------------
    // utils
private:
    void _removeResourceStreamToFileManagerByFilePath(QString filePath);
    QUrl _getUrlForLocalTargetFilePath(QString localTargetFilePath);

private:
    QHash< QUrl, AW::Common::Utils::StreamToFileManager * > _resourceStreamToFileManagerHash;
    CommunicationResourceGateway *_commResourceGateway;
};

} // namespace Communication
} // namespace Common
} // namespace AW

#endif // AW_COMMON_COMMUNICATION_RESOURCEDOWNLOADSTREAMTOFILE_H
