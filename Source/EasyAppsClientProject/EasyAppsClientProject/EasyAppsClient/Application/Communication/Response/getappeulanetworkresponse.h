#ifndef GETAPPEULANETWORKRESPONSE_H
#define GETAPPEULANETWORKRESPONSE_H

#include "Common/Communication/basenetworkresponse.h"

class GetAppEulaNetworkResponse : public AW::Common::Communication::Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit GetAppEulaNetworkResponse(const QString& JSONResponse);

protected:
    QList<AW::Common::Communication::BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(QString jsonString);

};

#endif // GETAPPEULANETWORKRESPONSE_H
