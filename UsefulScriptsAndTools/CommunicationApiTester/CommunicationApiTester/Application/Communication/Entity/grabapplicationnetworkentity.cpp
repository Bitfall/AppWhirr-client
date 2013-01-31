#include "grabapplicationnetworkentity.h"

GrabApplicationNetworkEntity::GrabApplicationNetworkEntity(QObject *parent) :
    BaseNetworkEntity(parent)
{
}

void GrabApplicationNetworkEntity::setZippedApplicationURL(const QString& zippedApplicationURL) {
    this->zippedApplicationURL = zippedApplicationURL;
}

QString GrabApplicationNetworkEntity::getZippedApplicationURL() {
    return this->zippedApplicationURL;
}

void GrabApplicationNetworkEntity::setRelatedAppVersion(QString relatedAppVersion) {
    _relatedAppVersion = relatedAppVersion;
}

QString GrabApplicationNetworkEntity::getRelatedAppVersion() {
    return _relatedAppVersion;
}


//void GrabApplicationNetworkEntity::setZippedApplicationFlag(const quint32& zippedApplicationFlag) {
//    this->zippedApplicationFlag = zippedApplicationFlag;
//}

//unsigned int GrabApplicationNetworkEntity::getZippedApplicationFlag() {
//    return this->zippedApplicationFlag;
//}
