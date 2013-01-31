#ifndef CLIENTUPDATECHECKNETWORKRESPONSE_H
#define CLIENTUPDATECHECKNETWORKRESPONSE_H

#include "Common/Communication/basenetworkresponse.h"

class ClientUpdateCheckNetworkResponse : public AW::Common::Communication::Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit ClientUpdateCheckNetworkResponse(const QString& JSONResponse);

protected:
    QList<AW::Common::Communication::BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(QString jsonString);

};

#endif // CLIENTUPDATECHECKNETWORKRESPONSE_H
