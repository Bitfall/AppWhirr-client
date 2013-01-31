#include "appdownloaddelegate.h"

#include <QThreadPool>

//#include "Application/Communication/Gateway/communicationresourcegateway.h"
#include "Application/Communication/awresourcedownloadstreamtofile.h"

#include "Common/Helpers/pathhelper.h"
#include "Application/appspecificpathhelper.h"
#include "Common/Runnables/runnable_saveresourcetofile.h"


AppDownloadDelegate::AppDownloadDelegate(QObject *parent) :
    QObject(parent),
    _resourceDownloader(NULL)
{
}

AppDownloadDelegate::~AppDownloadDelegate()
{
    AW_QOBJECT_SAFE_DELETE(_resourceDownloader);
}

void AppDownloadDelegate::startDownloadingApp(GrabApplicationQueueItem grabItem)
{
    _downloadingResourceMapper.insert(grabItem.getRelatedDownloadUrl(), grabItem);
    this->tryToStartDownloadingApplication(grabItem.getRelatedDownloadUrl());
}

void AppDownloadDelegate::tryToStartDownloadingApplication(QUrl zippedApplicationURL) {

    GrabApplicationQueueItem relatedGrabItem;
    if(_downloadingResourceMapper.contains(zippedApplicationURL)) {
        relatedGrabItem = _downloadingResourceMapper.value(zippedApplicationURL);
    } else {
        WLog("Some serious bug! The downloading-resource-mapper does not contains the url!");
        Q_EMIT appDownloadFailed(relatedGrabItem, tr("Internal error: Download cannot be started."));
//        Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Internal error: Download cannot be started."));
        return;
    }

//    Q_EMIT appInstallationStarted(relatedGrabItem.getAppId(), relatedGrabItem.getAppName());
    Q_EMIT appDownloadStarted(relatedGrabItem);


    // start downloading
    if (_resourceDownloader == NULL) {
        _resourceDownloader = AW::Client::Communication::AWResourceDownloadStreamToFile::createDefaultResourceDownloadStreamToFile(NULL);

        connect(_resourceDownloader, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QString)), this, SLOT(__appDownloadResourceSuccess(QUrl,QString)));
        connect(_resourceDownloader, SIGNAL(resourceDownloadProgressForUrl(int,QUrl)), this, SLOT(__downloadProgressForUrl(int,QUrl)));
        connect(_resourceDownloader, SIGNAL(finishedWithAllResourceDownloads()), this, SLOT(__allAppDownloadResourceRequestsFinished()));
        connect(_resourceDownloader, SIGNAL(resourceDownloadFailed(QUrl,QString,QString)), this, SLOT(__appDownloadFailed(QUrl,QString,QString)));
    }

    if( !_resourceDownloader->addAndInitializeResourceDownloadRequest(zippedApplicationURL, relatedGrabItem._getRelatedDownloadedZip(), true) ) {
        Q_EMIT appDownloadFailed(relatedGrabItem, tr("Download failed: cannot save the app."));
        AW_QOBJECT_SAFE_DELETE(_resourceDownloader);
    }
    else {
        _resourceDownloader->startDownloadingResources();
    }
}

void AppDownloadDelegate::__allAppDownloadResourceRequestsFinished()
{
    AW_QOBJECT_SAFE_DELETE(_resourceDownloader);
}

void AppDownloadDelegate::__appDownloadFailed(QUrl url, QString localFilePath, QString errorMessage)
{
    WLog("App download failed: ") << url << " file-path: " << localFilePath << " error: " << errorMessage;

    GrabApplicationQueueItem relatedGrabItem;
    if(_downloadingResourceMapper.contains(url)) {
        relatedGrabItem = _downloadingResourceMapper.value(url);
        _downloadingResourceMapper.remove(url);
    } else {
        WLog("Some serious bug! The downloading-resource-mapper does not contains the url!");
    }

    Q_EMIT appDownloadFailed(relatedGrabItem, tr("Download failed."));
    //    Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Download failed."));
}

void AppDownloadDelegate::__downloadProgressForUrl(int progress, QUrl url)
{
    if(_downloadingResourceMapper.contains(url))
    {
        GrabApplicationQueueItem relatedGrabItem;
        relatedGrabItem = _downloadingResourceMapper.value(url);
        Q_EMIT appDownloadProgress(relatedGrabItem, progress);
    }
    else {
        WLog("Some serious bug! The downloading-resource-mapper does not contains the url!");
    }
}

void AppDownloadDelegate::__appDownloadResourceSuccess(QUrl url, QString localFilePath)
{
    GrabApplicationQueueItem relatedGrabItem;
    if(_downloadingResourceMapper.contains(url)) {
        relatedGrabItem = _downloadingResourceMapper.value(url);
    } else {
        WLog("Some serious bug! The downloading-resource-mapper does not contains the url!");
//        Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Download failed."));
        Q_EMIT appDownloadFailed(relatedGrabItem, tr("Download failed."));
        return;
    }

    if(relatedGrabItem.getAppId().isEmpty()) {
//        Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("AppId invalid."));
        _downloadingResourceMapper.remove(url);
        Q_EMIT appDownloadFailed(relatedGrabItem, tr("AppId invalid."));
        return;
    }

//    Runnable_SaveResourceToFile *worker = new Runnable_SaveResourceToFile(relatedGrabItem._getRelatedDownloadedZip(), resource);
//    connect(worker, SIGNAL(finishedWithSuccess(QString)), this, SLOT(_appZipSavedToFile(QString)));
//    connect(worker, SIGNAL(finishedButFailed(QString)), this, SLOT(_appZipSaveToFileFailed(QString)));
//    QThreadPool::globalInstance()->start(worker);

    this->_appZipSavedToFile(localFilePath);
}

void AppDownloadDelegate::_appZipSavedToFile(QString filePath)
{
    DLog("Zip saved to file:") << filePath;
    QUrl relatedUrl = this->_getUrlByZipPath(filePath);

    GrabApplicationQueueItem relatedGrabItem;
    if(relatedUrl.isEmpty()) {
        WLog("Cannot find related url to file-path:") << filePath;
        Q_EMIT appDownloadFailed(relatedGrabItem, tr("Download failed."));
        return;
    }

    if(_downloadingResourceMapper.contains(relatedUrl)) {
        relatedGrabItem = _downloadingResourceMapper.value(relatedUrl);
        _downloadingResourceMapper.remove(relatedUrl);
    } else {
        WLog("Some serious bug! The downloading-resource-mapper does not contains the url!");
//        Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Download failed."));
        Q_EMIT appDownloadFailed(relatedGrabItem, tr("Download failed."));
        return;
    }

    Q_EMIT appDownloadFinished(relatedGrabItem);
}

void AppDownloadDelegate::_appZipSaveToFileFailed(QString filePath)
{
    WLog("Cannot save the resource to zip: ") << filePath;

    GrabApplicationQueueItem relatedGrabItem;
    QUrl relatedUrl = this->_getUrlByZipPath(filePath);
    if(relatedUrl.isEmpty()) {
        WLog("Cannot find related url to file-path:") << filePath;
        Q_EMIT appDownloadFailed(relatedGrabItem, tr("Download failed."));
        return;
    }

    if(_downloadingResourceMapper.contains(relatedUrl)) {
        relatedGrabItem = _downloadingResourceMapper.value(relatedUrl);
        _downloadingResourceMapper.remove(relatedUrl);
    } else {
        WLog("Some serious bug! The downloading-resource-mapper does not contains the url!");
//        Q_EMIT appInstallFailed(relatedGrabItem.getAppId(), relatedGrabItem.getAppName(), relatedGrabItem.getIsUpdate(), tr("Download failed."));
        Q_EMIT appDownloadFailed(relatedGrabItem, tr("Download failed."));
        return;
    }

    Q_EMIT appDownloadFailed(relatedGrabItem, tr("Cannot save the zip to file."));
}

QUrl AppDownloadDelegate::_getUrlByZipPath(QString zipPath)
{
    QUrl foundUrl;
    QMapIterator<QUrl, GrabApplicationQueueItem> i(_downloadingResourceMapper);
    while (i.hasNext() && foundUrl.isEmpty()) {
        i.next();
        if(i.value()._getRelatedDownloadedZip() == zipPath) {
            foundUrl = i.key();
        }
    }

    return foundUrl;
}
