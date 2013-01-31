#ifndef GRABAPPLICATIONQUEUEITEM_H
#define GRABAPPLICATIONQUEUEITEM_H

#include <QString>
#include <QUrl>

#include "Application/ApplicationDescription/applicationstatesenum.h"


/*! The \b internal grab-manager item-model.

    It's used in the grab-manager, internally.
*/
class GrabApplicationQueueItem
{
public:
    /*! Default constructor - required by QMap, QList, ...
    */
    explicit GrabApplicationQueueItem();

    explicit GrabApplicationQueueItem(QString appId, QString appName, QString appSupportedSummedPlatformFlags, bool isUpdate, AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore);

    QString getAppId() const;
    QString getAppName() const;
    QString getSupportedSummedPlatformFlags() const;
    bool getIsUpdate() const;
    AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum getAppStatusInStore() const;

    QString getRelatedAppVersion() const;
    void _setRelatedAppVersion(QString value);

    QUrl getRelatedDownloadUrl() const;
    void _setRelatedDownloadUrl(QUrl url);

    QString _getRelatedDownloadedZip() const;
    void _setRelatedDownloadedZip(QString value);

private:
    QString _appId;
    QString _appName;
    QString _relatedAppVersion;
    QString _supportedSummedPlatformFlags;
    bool _isUpdate;
    AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum _appStatusInStore;

    QUrl _relatedDownloadUrl;
    QString _relatedDownloadedZip;
};

#endif // GRABAPPLICATIONQUEUEITEM_H
