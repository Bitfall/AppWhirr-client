#ifndef GRABAPPLICATIONRESPONSE_H
#define GRABAPPLICATIONRESPONSE_H

#include "Common/Communication/basenetworkresponse.h"

QT_BEGIN_NAMESPACE
class QString;
class QStringValue;
QT_END_NAMESPACE

class GrabApplicationNetworkResponse : public AW::Common::Communication::Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit GrabApplicationNetworkResponse(const QString& JSONResponse);

protected:
    QList<AW::Common::Communication::BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(QString jsonString);
};

#endif // GRABAPPLICATIONRESPONSE_H
