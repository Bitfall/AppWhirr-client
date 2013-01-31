#ifndef USERLOGINNETWORKREQUEST_H
#define USERLOGINNETWORKREQUEST_H

#include <QObject>
#include "Common/Communication/basenetworkrequest.h"

class UserLoginNetworkRequest : public BaseNetworkRequest
{
    Q_OBJECT
public:
    explicit UserLoginNetworkRequest();

    Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // USERLOGINNETWORKREQUEST_H
