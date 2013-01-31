#ifndef APPLICATIONLISTWITHSECTIONSNETWORKRESPONSE_H
#define APPLICATIONLISTWITHSECTIONSNETWORKRESPONSE_H

#include <QObject>
#include "Common/Communication/basenetworkresponse.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class ApplicationListWithSectionsNetworkResponse : public Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit ApplicationListWithSectionsNetworkResponse(const QString& JSONResponse);

protected:
    QList<BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue);

};

#endif // APPLICATIONLISTWITHSECTIONSNETWORKRESPONSE_H
