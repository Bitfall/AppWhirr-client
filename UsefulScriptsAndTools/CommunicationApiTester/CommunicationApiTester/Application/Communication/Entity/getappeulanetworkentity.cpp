#include "getappeulanetworkentity.h"

GetAppEulaNetworkEntity::GetAppEulaNetworkEntity(QString eulaText, QObject *parent) :
    BaseNetworkEntity(parent),
    _eulaText(eulaText)
{
}

QString GetAppEulaNetworkEntity::getEulaText() const {
    return _eulaText;
}
