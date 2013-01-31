#ifndef GETAPPEULANETWORKRESPONSE_H
#define GETAPPEULANETWORKRESPONSE_H

#include "Common/Communication/basenetworkresponse.h"

class GetAppEulaNetworkResponse : public Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit GetAppEulaNetworkResponse(const QString& JSONResponse);

protected:
    QList<BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue);

};

#endif // GETAPPEULANETWORKRESPONSE_H
