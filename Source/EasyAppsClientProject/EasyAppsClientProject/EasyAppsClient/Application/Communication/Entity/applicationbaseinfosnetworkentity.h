#ifndef APPLICATIONBASEINFOSNETWORKENTITY_H
#define APPLICATIONBASEINFOSNETWORKENTITY_H

#include <QObject>
#include "Common/Communication/basenetworkentity.h"
#include "applicationbaseinfosnetworkentitymodel.h"

class ApplicationBaseInfosNetworkEntity : public AW::Common::Communication::BaseNetworkEntity
{
    Q_OBJECT
public:
    explicit ApplicationBaseInfosNetworkEntity(QObject *parent = 0);

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
    void setApplicationRatingCount(quint32 applicationRatingCount);
    void setApplicationRating(float applicationRating);
    void setApplicationReviewCount(quint32 applicationReviewCount);
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
    quint32 getApplicationRatingCount();
    float getApplicationRating();
    quint32 getApplicationReviewCount();
    AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum getAppStatusInStore();


    ApplicationBaseInfosNetworkEntityModel getModel() const;

private:
    ApplicationBaseInfosNetworkEntityModel _model;
};

#endif // APPLICATIONBASEINFOSNETWORKENTITY_H
