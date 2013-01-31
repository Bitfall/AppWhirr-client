#ifndef APITESTNETWORKRESPONSE_H
#define APITESTNETWORKRESPONSE_H

#include "Common/Communication/basenetworkresponse.h"

class ApiTestNetworkResponse : public Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit ApiTestNetworkResponse(const QString& JSONResponse);

protected:
    QList<BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue);
};

#endif // APITESTNETWORKRESPONSE_H
