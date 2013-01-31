#ifndef CLIENTUPDATENETWORKREQUEST_H
#define CLIENTUPDATENETWORKREQUEST_H

#include "Common/Communication/basenetworkrequest.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class Background_BaseNetworkResponse;

class ClientUpdateNetworkRequest : public BaseNetworkRequest
{
    Q_OBJECT
public:
    explicit ClientUpdateNetworkRequest();

    Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // CLIENTUPDATENETWORKREQUEST_H
