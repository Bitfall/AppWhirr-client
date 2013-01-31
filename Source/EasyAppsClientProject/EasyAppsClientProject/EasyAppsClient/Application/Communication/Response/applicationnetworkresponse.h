#ifndef APPLICATIONNETWORKRESPONSE_H
#define APPLICATIONNETWORKRESPONSE_H

#include <QObject>
#include "Common/Communication/basenetworkresponse.h"

QT_BEGIN_NAMESPACE
class QString;
class QScriptValue;
QT_END_NAMESPACE

class ApplicationNetworkResponse : public AW::Common::Communication::Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit ApplicationNetworkResponse(const QString& JSONResponse);

protected:
    QList<AW::Common::Communication::BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(QString jsonString);
};

#endif // APPLICATIONNETWORKRESPONSE_H
