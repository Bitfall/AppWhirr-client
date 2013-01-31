#ifndef CLIENTUPDATENETWORKREQUEST_H
#define CLIENTUPDATENETWORKREQUEST_H

#include "Application/Communication/Request/awbasenetworkrequest.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class AW::Common::Communication::Background_BaseNetworkResponse;

class ClientUpdateNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    explicit ClientUpdateNetworkRequest();

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual void addAdditionalRequiredHeaders(QNetworkRequest *_networkRequest);
};

#endif // CLIENTUPDATENETWORKREQUEST_H
