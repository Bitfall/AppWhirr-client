#ifndef GETAPPEULANETWORKENTITY_H
#define GETAPPEULANETWORKENTITY_H

#include "Common/Communication/basenetworkentity.h"

class GetAppEulaNetworkEntity : public AW::Common::Communication::BaseNetworkEntity
{
    Q_OBJECT
public:
    explicit GetAppEulaNetworkEntity(QString eulaText, QObject *parent = 0);

    QString getEulaText() const;

private:
    QString _eulaText;
};

#endif // GETAPPEULANETWORKENTITY_H
