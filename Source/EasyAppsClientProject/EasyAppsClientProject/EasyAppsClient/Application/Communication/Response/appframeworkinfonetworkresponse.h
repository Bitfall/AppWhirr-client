#ifndef APPFRAMEWORKINFONETWORKRESPONSE_H
#define APPFRAMEWORKINFONETWORKRESPONSE_H

#include "Common/Communication/basenetworkresponse.h"

class AppFrameworkInfoNetworkResponse : public AW::Common::Communication::Background_BaseNetworkResponse
{
    Q_OBJECT
public:
    explicit AppFrameworkInfoNetworkResponse(const QString& JSONResponse);

protected:
    QList<AW::Common::Communication::BaseNetworkEntity *> receivedResponseWithSuccess_DoParse(QString jsonString);
};

#endif // APPFRAMEWORKINFONETWORKRESPONSE_H
