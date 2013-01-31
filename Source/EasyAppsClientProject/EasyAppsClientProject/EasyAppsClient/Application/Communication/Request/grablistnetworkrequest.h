#ifndef GRABLISTNETWORKREQUEST_H
#define GRABLISTNETWORKREQUEST_H

#include "Application/Communication/Request/awbasenetworkrequest.h"

class GrabListNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    GrabListNetworkRequest();

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // GRABLISTNETWORKREQUEST_H
