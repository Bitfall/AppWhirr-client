#ifndef GETAPPEULANETWORKREQUEST_H
#define GETAPPEULANETWORKREQUEST_H

#include "Application/Communication/Request/awbasenetworkrequest.h"

class GetAppEulaNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    explicit GetAppEulaNetworkRequest(QString appId);

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // GETAPPEULANETWORKREQUEST_H
