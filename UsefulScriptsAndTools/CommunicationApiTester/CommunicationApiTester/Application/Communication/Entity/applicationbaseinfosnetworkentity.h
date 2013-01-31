#ifndef APPLICATIONBASEINFOSNETWORKENTITY_H
#define APPLICATIONBASEINFOSNETWORKENTITY_H

#include <QObject>
#include "Common/Communication/basenetworkentity.h"
#include "applicationbaseinfosnetworkentitymodel.h"

class ApplicationBaseInfosNetworkEntity : public BaseNetworkEntity
{
    Q_OBJECT
public:
    explicit ApplicationBaseInfosNetworkEntity(QObject *parent = 0);

    void setApplicationID(QString applicationID);
    void setApplicationName(QString applicationName);
    void setApplicationIconURL(QString applicationIconURL);
    void setApplicationVersion(QString applicationVersion);
    void setApplicationCategory(QString applicationCategory);
    void setApplicationVersionSummedFlags(const int& applicationSummedFlags);
    void setApplicationRatingCount(const int& applicationRatingCount);
    void setApplicationRating(const float& applicationRating);
    void setApplicationReviewCount(const int& applicationReviewCount);
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


    ApplicationBaseInfosNetworkEntityModel getModel() const;

private:
    ApplicationBaseInfosNetworkEntityModel _model;
};

#endif // APPLICATIONBASEINFOSNETWORKENTITY_H
