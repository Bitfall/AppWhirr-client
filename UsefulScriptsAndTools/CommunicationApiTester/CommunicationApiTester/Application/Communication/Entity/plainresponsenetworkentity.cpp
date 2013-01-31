#include "plainresponsenetworkentity.h"

PlainResponseNetworkEntity::PlainResponseNetworkEntity(QString responseString, QObject *parent) :
    BaseNetworkEntity(parent),
    _responseString(responseString)
{
}

QString PlainResponseNetworkEntity::getResponseAsString() const {
    return _responseString;
}
