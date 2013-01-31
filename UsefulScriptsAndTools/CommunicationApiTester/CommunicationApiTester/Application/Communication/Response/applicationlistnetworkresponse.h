#ifndef APPLICATIONLISTNETWORKRESPONSE_H
#define APPLICATIONLISTNETWORKRESPONSE_H

#include "Common/Communication/basenetworkresponse.h"

class ApplicationListNetworkResponse : public Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit ApplicationListNetworkResponse(const QString& JSONResponse);

protected:
    QList<BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue);

};

#endif // APPLICATIONLISTNETWORKRESPONSE_H
