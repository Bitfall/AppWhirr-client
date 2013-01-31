#ifndef APPLICATIONBASEINFOSNETWORKENTITYMODEL_H
#define APPLICATIONBASEINFOSNETWORKENTITYMODEL_H

#include <QString>

#include "Application/ApplicationDescription/applicationstatesenum.h"

class ApplicationBaseInfosNetworkEntityModel
{
public:
    explicit ApplicationBaseInfosNetworkEntityModel();

    void setApplicationID(QString applicationID);
    void setApplicationName(QString applicationName);
    void setApplicationIconURL(QString applicationIconURL);
    void setApplicationVersion(QString applicationVersion);
    void setApplicationCategory(QString applicationCategory);
    void setApplicationSupportURL(QString applicationSupportURL);
    void setApplicationDeveloperURL(QString applicationDeveloperURL);
    void setApplicationWindowsReadableFlags(QString applicationWindowsReadableFlags);
    void setApplicationMacReadableFlags(QString applicationMacReadableFlags);
    void setApplicationVersionSummedFlags(quint64 applicationSummedFlags);
    void setApplicationSize(int applicationSize);
    void setApplicationRatingCount(int applicationRatingCount);
    void setApplicationRating(float applicationRating);
    void setApplicationReviewCount(int applicationReviewCount);
    //
    //
    void setAppStatusInStore(AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore);

    QString getApplicationID();
    QString getApplicationName();
    QString getApplicationVersion();
    QString getApplicationIconURL();
    QString getApplicationCategory();
    QString getApplicationSupportURL();
    QString getApplicationDeveloperURL();
    QString getApplicationWindowsReadableFlags();
    QString getApplicationMacReadableFlags();
    quint64 getApplicationVersionSummedFlags();
    int getApplicationSize();
    int getApplicationRatingCount();
    float getApplicationRating();
    int getApplicationReviewCount();
    AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum getAppStatusInStore();

private:
    QString             applicationID;
    QString             applicationName;
    QString             applicationIconURL;
    QString             applicationVersion;
    QString             applicationCategory;
    QString             applicationSupportURL;
    QString             applicationDeveloperURL;
    QString             applicationWindowsReadableFlags;
    QString             applicationMacReadableFlags;
    quint64             applicationVersionSummedFlags;
    int                 applicationSize;
    int                 applicationRatingCount;
    float               applicationRating;
    int                 applicationReviewCount;

    AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum _appStatusInStore;
};

#endif // APPLICATIONBASEINFOSNETWORKENTITYMODEL_H
