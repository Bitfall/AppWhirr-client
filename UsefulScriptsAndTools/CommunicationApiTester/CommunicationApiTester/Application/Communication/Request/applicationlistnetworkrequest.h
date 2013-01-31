#ifndef APPLICATIONLISTNETWORKREQUEST_H
#define APPLICATIONLISTNETWORKREQUEST_H

#include "Common/Communication/basenetworkrequest.h"

class ApplicationListNetworkRequest : public BaseNetworkRequest
{
    Q_OBJECT
public:
    explicit ApplicationListNetworkRequest();

    Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // APPLICATIONLISTNETWORKREQUEST_H
