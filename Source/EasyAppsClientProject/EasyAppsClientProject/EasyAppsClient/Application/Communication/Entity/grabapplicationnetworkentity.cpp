#include "grabapplicationnetworkentity.h"

GrabApplicationNetworkEntity::GrabApplicationNetworkEntity(QUrl zippedApplicationURL, QString relatedAppVersion, QObject *parent) :
    AW::Common::Communication::BaseNetworkEntity(parent),
    _zippedApplicationURL(zippedApplicationURL),
    _relatedAppVersion(relatedAppVersion)
{
}

QUrl GrabApplicationNetworkEntity::getZippedApplicationURL() {
    return this->_zippedApplicationURL;
}

QString GrabApplicationNetworkEntity::getRelatedAppVersion() {
    return _relatedAppVersion;
}
