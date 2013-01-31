#include "appframeworkinfonetworkentity.h"

AppFrameworkInfoNetworkEntity::AppFrameworkInfoNetworkEntity(QString frameworkId, QString frameworkName, QString frameworkCheckerUpdateTimestamp, QUrl frameworkDownloadUrl, QUrl frameworkCheckerDownloadUrl, QObject *parent) :
    AW::Common::Communication::BaseNetworkEntity(parent),
    _frameworkId(frameworkId),
    _frameworkName(frameworkName),
    _frameworkCheckerUpdateTimestamp(frameworkCheckerUpdateTimestamp),
    _frameworkDownloadUrl(frameworkDownloadUrl),
    _frameworkCheckerDownloadUrl(frameworkCheckerDownloadUrl)
{
}

QString AppFrameworkInfoNetworkEntity::getFrameworkId() const
{
    return _frameworkId;
}

QString AppFrameworkInfoNetworkEntity::getFrameworkName() const
{
    return _frameworkName;
}

QString AppFrameworkInfoNetworkEntity::getFrameworkCheckerUpdateTimestamp() const
{
    return _frameworkCheckerUpdateTimestamp;
}

QUrl AppFrameworkInfoNetworkEntity::getFrameworkDownloadUrl() const
{
    return _frameworkDownloadUrl;
}

QUrl AppFrameworkInfoNetworkEntity::getFrameworkCheckerDownloadUrl() const
{
    return _frameworkCheckerDownloadUrl;
}
