#ifndef GETAPPEULANETWORKREQUEST_H
#define GETAPPEULANETWORKREQUEST_H

#include "Common/Communication/basenetworkrequest.h"

class GetAppEulaNetworkRequest : public BaseNetworkRequest
{
    Q_OBJECT
public:
    explicit GetAppEulaNetworkRequest(QString appId);

    Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // GETAPPEULANETWORKREQUEST_H
