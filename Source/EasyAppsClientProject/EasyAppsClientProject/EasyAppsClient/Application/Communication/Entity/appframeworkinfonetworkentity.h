#ifndef APPFRAMEWORKINFONETWORKENTITY_H
#define APPFRAMEWORKINFONETWORKENTITY_H

#include "Common/Communication/basenetworkentity.h"

#include <QUrl>

class AppFrameworkInfoNetworkEntity : public AW::Common::Communication::BaseNetworkEntity
{
    Q_OBJECT
public:
    explicit AppFrameworkInfoNetworkEntity(QString frameworkId, QString frameworkName, QString frameworkCheckerUpdateTimestamp, QUrl frameworkDownloadUrl, QUrl frameworkCheckerDownloadUrl, QObject *parent = 0);

    QString getFrameworkId() const;
    QString getFrameworkName() const;
    QString getFrameworkCheckerUpdateTimestamp() const;
    QUrl getFrameworkDownloadUrl() const;
    QUrl getFrameworkCheckerDownloadUrl() const;

private:
    QString _frameworkId;
    QString _frameworkName;
    QString _frameworkCheckerUpdateTimestamp;
    QUrl _frameworkDownloadUrl;
    QUrl _frameworkCheckerDownloadUrl;
};

#endif // APPFRAMEWORKINFONETWORKENTITY_H
