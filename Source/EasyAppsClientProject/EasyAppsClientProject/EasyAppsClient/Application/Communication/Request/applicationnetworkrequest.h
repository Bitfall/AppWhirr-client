#ifndef APPLICATIONNETWORKREQUEST_H
#define APPLICATIONNETWORKREQUEST_H

#include "Application/Communication/Request/awbasenetworkrequest.h"

class ApplicationNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    explicit ApplicationNetworkRequest(const QString& applicationID);

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual void addAdditionalRequiredHeaders(QNetworkRequest *_networkRequest);
};

#endif // APPLICATIONNETWORKREQUEST_H
