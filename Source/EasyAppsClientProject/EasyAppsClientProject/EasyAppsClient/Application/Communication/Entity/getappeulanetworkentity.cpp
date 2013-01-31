#include "getappeulanetworkentity.h"

GetAppEulaNetworkEntity::GetAppEulaNetworkEntity(QString eulaText, QObject *parent) :
    AW::Common::Communication::BaseNetworkEntity(parent),
    _eulaText(eulaText)
{
}

QString GetAppEulaNetworkEntity::getEulaText() const {
    return _eulaText;
}
