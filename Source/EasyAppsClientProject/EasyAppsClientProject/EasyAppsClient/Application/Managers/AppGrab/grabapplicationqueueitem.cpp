#include "grabapplicationqueueitem.h"

GrabApplicationQueueItem::GrabApplicationQueueItem()
{
}

GrabApplicationQueueItem::GrabApplicationQueueItem(QString appId, QString appName, QString appSupportedSummedPlatformFlags, bool isUpdate, AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore) :
    _appId(appId),
    _appName(appName),
    _supportedSummedPlatformFlags(appSupportedSummedPlatformFlags),
    _isUpdate(isUpdate),
    _appStatusInStore(appStatusInStore)
{
}

// -------------------------
// --- getters / setters ---

QString GrabApplicationQueueItem::getAppId() const {
    return _appId;
}

QString GrabApplicationQueueItem::getAppName() const {
    return _appName;
}

QString GrabApplicationQueueItem::getSupportedSummedPlatformFlags() const {
    return _supportedSummedPlatformFlags;
}

bool GrabApplicationQueueItem::getIsUpdate() const {
    return _isUpdate;
}

AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum GrabApplicationQueueItem::getAppStatusInStore() const {
    return _appStatusInStore;
}

QString GrabApplicationQueueItem::getRelatedAppVersion() const {
    return _relatedAppVersion;
}

void GrabApplicationQueueItem::_setRelatedAppVersion(QString value) {
    _relatedAppVersion = value;
}

QUrl GrabApplicationQueueItem::getRelatedDownloadUrl() const
{
    return _relatedDownloadUrl;
}

void GrabApplicationQueueItem::_setRelatedDownloadUrl(QUrl url)
{
    _relatedDownloadUrl = url;
}

QString GrabApplicationQueueItem::_getRelatedDownloadedZip() const {
    return _relatedDownloadedZip;
}

void GrabApplicationQueueItem::_setRelatedDownloadedZip(QString value) {
    _relatedDownloadedZip = value;
}
