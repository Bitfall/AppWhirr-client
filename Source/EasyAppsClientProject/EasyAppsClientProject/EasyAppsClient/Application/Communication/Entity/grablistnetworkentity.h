#ifndef GRABLISTNETWORKENTITY_H
#define GRABLISTNETWORKENTITY_H

#include <QObject>

#include "Application/Communication/Entity/applicationbaseinfosnetworkentity.h"

class GrabListNetworkEntity : public ApplicationBaseInfosNetworkEntity
{
    Q_OBJECT
public:
    GrabListNetworkEntity(QObject *parent = 0);

};

#endif // GRABLISTNETWORKENTITY_H
