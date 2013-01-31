#ifndef APPLICATIONNETWORKRESPONSE_H
#define APPLICATIONNETWORKRESPONSE_H

#include <QObject>
#include "Common/Communication/basenetworkresponse.h"

QT_BEGIN_NAMESPACE
class QString;
class QScriptValue;
QT_END_NAMESPACE

class ApplicationNetworkResponse : public Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit ApplicationNetworkResponse(const QString& JSONResponse);

protected:
    QList<BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue);
};

#endif // APPLICATIONNETWORKRESPONSE_H
