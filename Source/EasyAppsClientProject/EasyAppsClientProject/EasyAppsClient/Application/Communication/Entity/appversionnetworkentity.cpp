#include "appversionnetworkentity.h"

AppVersionNetworkEntity::AppVersionNetworkEntity(QString versionInfo, QObject *parent) :
    AW::Common::Communication::BaseNetworkEntity(parent),
    _versionInfo(versionInfo)
{
}

QString AppVersionNetworkEntity::getVersionInfo() const {
    return _versionInfo;
}
