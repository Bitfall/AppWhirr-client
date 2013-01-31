#ifndef APPLICATIONLISTWITHSECTIONSNETWORKRESPONSE_H
#define APPLICATIONLISTWITHSECTIONSNETWORKRESPONSE_H

#include <QObject>
#include "Common/Communication/basenetworkresponse.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class ApplicationListWithSectionsNetworkResponse : public AW::Common::Communication::Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit ApplicationListWithSectionsNetworkResponse(const QString& JSONResponse);

protected:
    QList<AW::Common::Communication::BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(QString jsonString);

};

#endif // APPLICATIONLISTWITHSECTIONSNETWORKRESPONSE_H
