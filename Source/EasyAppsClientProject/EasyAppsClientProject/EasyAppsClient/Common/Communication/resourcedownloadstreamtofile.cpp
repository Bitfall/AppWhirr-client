#include "resourcedownloadstreamtofile.h"

#include "Common/File/streamtofilemanager.h"
#include "Common/Communication/resourcedownloadnetworkrequest.h"

using namespace AW::Common::Utils;

namespace AW {
namespace Common {
namespace Communication {

ResourceDownloadStreamToFile::ResourceDownloadStreamToFile(CommunicationResourceGatewayCacheDelegate *commResourceGatewayCacheDelegate, unsigned int maxParallelRequestsCount, QObject *parent) :
    QObject(parent)
{
    _commResourceGateway = new CommunicationResourceGateway(this, commResourceGatewayCacheDelegate, maxParallelRequestsCount, this);
    connect(_commResourceGateway, SIGNAL(downloadProgressForUrl(int,QUrl)), this, SLOT(_downloadProgressForUrl(int,QUrl)));
    connect(_commResourceGateway, SIGNAL(resourceDownloadFailedForUrl(QUrl)), this, SLOT(_resourceDownloadFailedForUrl(QUrl)));
    connect(_commResourceGateway, SIGNAL(resourceChunkReceivedForUrl(QUrl,QByteArray,bool)), this, SLOT(_resourceChunkReceivedForUrl(QUrl,QByteArray,bool)));
    connect(_commResourceGateway, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl)), this, SLOT(_finishedResourceDownloading(QUrl)));
    connect(_commResourceGateway, SIGNAL(finishedWithAllRequests()), this, SLOT(_allNetworkRequestsFinished()));
}

bool ResourceDownloadStreamToFile::addAndInitializeResourceDownloadRequest(QUrl url, QString localTargetFilePath, bool isRemoveLocalTargetFileIfExists)
{
    DLog("Add resource for streaming from: ") << url << " to file: " << localTargetFilePath << "[is remove file if exists:" << isRemoveLocalTargetFileIfExists << "]";
    StreamToFileManager *streamToFileManager = new StreamToFileManager(this);
    connect(streamToFileManager, SIGNAL(streamingFinishedWithError(QString,QString)), this, SLOT(_streamToFileFailedWithError(QString,QString)));
    connect(streamToFileManager, SIGNAL(streamingFinishedWithSuccess(QString)), this, SLOT(_streamToFileFinished(QString)));
    if( !streamToFileManager->startStreamingToFile(localTargetFilePath, isRemoveLocalTargetFileIfExists) ) {
        AW_QOBJECT_SAFE_DELETE(streamToFileManager);
        return false;
    }

    DLog("Resource prepared for streaming.");
    _resourceStreamToFileManagerHash[url] = streamToFileManager;
    _commResourceGateway->addResourceUrl(url);
    return true;
}

void ResourceDownloadStreamToFile::startDownloadingResources()
{
    _commResourceGateway->startRequests();
}

// --------------------------------------------
// resource network request handling

void ResourceDownloadStreamToFile::_downloadProgressForUrl(int percent, QUrl url)
{
    Q_EMIT resourceDownloadProgressForUrl(percent, url);
}

void ResourceDownloadStreamToFile::_resourceDownloadFailedForUrl(QUrl url)
{
    WLog("resource download FAILED: ") << url;

    StreamToFileManager *streamToFileManager = _resourceStreamToFileManagerHash.value(url);
    if(streamToFileManager != NULL) {
        streamToFileManager->abortStreaming();
    }
    else {
        WLog("Cannot find related stream-to-file manager: ") << url;
    }
}

void ResourceDownloadStreamToFile::_resourceChunkReceivedForUrl(QUrl url, QByteArray resourceChunk, bool isBufferDownloadedResourceAndSendItWhenDownloadFinished)
{
    if(isBufferDownloadedResourceAndSendItWhenDownloadFinished) {
        WLog("isBufferDownloadedResourceAndSendItWhenDownloadFinished is true - it should be false!");
    }

    StreamToFileManager *streamToFileManager = _resourceStreamToFileManagerHash.value(url);
    if(streamToFileManager != NULL) {
        streamToFileManager->writeDataToFile(resourceChunk);
    }
    else {
        WLog("Cannot find related stream-to-file manager: ") << url;
    }
}

void ResourceDownloadStreamToFile::_finishedResourceDownloading(QUrl url)
{
    StreamToFileManager *streamToFileManager = _resourceStreamToFileManagerHash.value(url);
    if(streamToFileManager != NULL) {
        streamToFileManager->closeStreaming();
    }
    else {
        WLog("Cannot find related stream-to-file manager: ") << url;
    }
}

void ResourceDownloadStreamToFile::_allNetworkRequestsFinished()
{
    DLog("All requests finished");

    // file operations can be still active
    if(_resourceStreamToFileManagerHash.isEmpty()) {
        Q_EMIT finishedWithAllResourceDownloads();
    }
    // if there's some pending file operations we have to wait for finishing of them and emit finished after every file operation finished
}

// --------------------------------------------
// file-stream handling

void ResourceDownloadStreamToFile::_streamToFileFailedWithError(QString filePath, QString errorMessage)
{
    QUrl relatedUrl = this->_getUrlForLocalTargetFilePath(filePath);
    WLog("Stream to file FAILED with error:") << errorMessage << " | file path:" << filePath << " url:" << relatedUrl;

    this->_removeResourceStreamToFileManagerByFilePath(filePath);
    if(relatedUrl.isEmpty()) {
        WLog("Url not found for file-path:") << filePath;
    }
    else {
        Q_EMIT resourceDownloadFailed(relatedUrl, filePath, errorMessage);
    }

    if(_resourceStreamToFileManagerHash.isEmpty()) {
        Q_EMIT finishedWithAllResourceDownloads();
    }
}

void ResourceDownloadStreamToFile::_streamToFileFinished(QString filePath)
{
    QUrl relatedUrl = this->_getUrlForLocalTargetFilePath(filePath);
    DLog("Resource download and stream-to-file finished for file-path:") << filePath << " url:" << relatedUrl;

    this->_removeResourceStreamToFileManagerByFilePath(filePath);

    if(relatedUrl.isEmpty()) {
        WLog("Url not found for file-path:") << filePath;
    }
    else {
        Q_EMIT resourceDownloadFinishedWithSuccess(relatedUrl, filePath);
    }

    if(_resourceStreamToFileManagerHash.isEmpty()) {
        Q_EMIT finishedWithAllResourceDownloads();
    }
}


// --------------------------------------------
// CommunicationResourceGatewayDelegate

BaseNetworkRequest *ResourceDownloadStreamToFile::createResourceDownloadRequest(BaseNetworkRequest::BaseCommunicationRequestType requestType, QUrl requestUrl, QNetworkRequest::CacheLoadControl cacheControl, int timeoutInterval, NetworkRequestCacheDelegate *requestCacheDelegate)
{
    return new ResourceDownloadNetworkRequest(requestType, requestUrl, cacheControl, timeoutInterval, false, requestCacheDelegate);
}

// --------------------------------------------
// utils

void ResourceDownloadStreamToFile::_removeResourceStreamToFileManagerByFilePath(QString filePath)
{
    DLog("Remove streamer for path: ") << filePath;
    QUrl foundKeyUrl;
    StreamToFileManager *streamToFileManager = NULL;
    QHash< QUrl, StreamToFileManager * >::iterator i = _resourceStreamToFileManagerHash.begin();
    while (i != _resourceStreamToFileManagerHash.end() && foundKeyUrl.isEmpty()) {
        StreamToFileManager *currStreamToFileManager = i.value();
        if(currStreamToFileManager->getTargetFilePath() == filePath) {
            foundKeyUrl = i.key();
            streamToFileManager = currStreamToFileManager;
        }

        ++i;
    }

    if(foundKeyUrl.isEmpty()) {
        WLog("Key url not found for path: ") << filePath;
    }
    else {
        DLog("Url found, will remove related stream manager.");
        _resourceStreamToFileManagerHash.remove(foundKeyUrl);
        AW_QOBJECT_SAFE_DELETE(streamToFileManager);
    }
}

QUrl ResourceDownloadStreamToFile::_getUrlForLocalTargetFilePath(QString localTargetFilePath)
{
    QUrl foundKeyUrl;
    QHash< QUrl, StreamToFileManager * >::iterator i = _resourceStreamToFileManagerHash.begin();
    while (i != _resourceStreamToFileManagerHash.end() && foundKeyUrl.isEmpty()) {
        StreamToFileManager *currStreamToFileManager = i.value();
        if(currStreamToFileManager->getTargetFilePath() == localTargetFilePath) {
            foundKeyUrl = i.key();
        }

        ++i;
    }
    return foundKeyUrl;
}

} // namespace Communication
} // namespace Common
} // namespace AW
