#ifndef APPFRAMEWORKINFONETWORKREQUEST_H
#define APPFRAMEWORKINFONETWORKREQUEST_H

#include "Application/Communication/Request/awbasenetworkrequest.h"

class AppFrameworkInfoNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    explicit AppFrameworkInfoNetworkRequest(QString appId);

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual void addAdditionalRequiredHeaders(QNetworkRequest *_networkRequest);
};

#endif // APPFRAMEWORKINFONETWORKREQUEST_H
