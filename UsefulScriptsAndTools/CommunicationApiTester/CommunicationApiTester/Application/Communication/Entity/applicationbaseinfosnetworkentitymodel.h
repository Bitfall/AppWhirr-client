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
    void setApplicationVersionSummedFlags(const int& applicationSummedFlags);
    void setApplicationRatingCount(const int& applicationRatingCount);
    void setApplicationRating(const float& applicationRating);
    void setApplicationReviewCount(const int& applicationReviewCount);
    //
    //
    void setAppStatusInStore(AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore);

    QString getApplicationID();
    QString getApplicationName();
    QString getApplicationVersion();
    QString getApplicationIconURL();
    QString getApplicationCategory();
    int getApplicationVersionSummedFlags();
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
    int                 applicationVersionSummedFlags;
    int                 applicationRatingCount;
    float               applicationRating;
    int                 applicationReviewCount;

    AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum _appStatusInStore;
};

#endif // APPLICATIONBASEINFOSNETWORKENTITYMODEL_H
