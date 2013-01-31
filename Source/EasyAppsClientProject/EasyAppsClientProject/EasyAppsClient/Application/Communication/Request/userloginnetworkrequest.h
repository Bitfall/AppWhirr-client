#ifndef USERLOGINNETWORKREQUEST_H
#define USERLOGINNETWORKREQUEST_H

#include <QObject>
#include "Application/Communication/Request/awbasenetworkrequest.h"

class UserLoginNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    explicit UserLoginNetworkRequest();

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // USERLOGINNETWORKREQUEST_H
