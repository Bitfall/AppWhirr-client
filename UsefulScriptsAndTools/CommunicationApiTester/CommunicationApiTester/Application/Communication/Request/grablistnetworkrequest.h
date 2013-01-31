#ifndef GRABLISTNETWORKREQUEST_H
#define GRABLISTNETWORKREQUEST_H

#include "Common/Communication/basenetworkrequest.h"

class GrabListNetworkRequest : public BaseNetworkRequest
{
    Q_OBJECT
public:
    GrabListNetworkRequest();

    Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // GRABLISTNETWORKREQUEST_H
