#ifndef GRABAPPLICATIONRESPONSE_H
#define GRABAPPLICATIONRESPONSE_H

#include <QObject>

#include "Common/Communication/basenetworkresponse.h"

QT_BEGIN_NAMESPACE
class QString;
class QStringValue;
QT_END_NAMESPACE

class GrabApplicationNetworkResponse : public Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit GrabApplicationNetworkResponse(const QString& JSONResponse);

protected:
    QList<BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue);
};

#endif // GRABAPPLICATIONRESPONSE_H
