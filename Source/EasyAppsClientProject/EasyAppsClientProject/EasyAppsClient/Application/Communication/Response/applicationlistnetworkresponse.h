#ifndef APPLICATIONLISTNETWORKRESPONSE_H
#define APPLICATIONLISTNETWORKRESPONSE_H

#include "Common/Communication/basenetworkresponse.h"

class ApplicationListNetworkResponse : public AW::Common::Communication::Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit ApplicationListNetworkResponse(const QString& JSONResponse);

protected:
    QList<AW::Common::Communication::BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(QString jsonString);

};

#endif // APPLICATIONLISTNETWORKRESPONSE_H
