#ifndef APPLICATIONSEARCHNETWORKREQUEST_H
#define APPLICATIONSEARCHNETWORKREQUEST_H

#include "Application/Communication/Request/awbasenetworkrequest.h"

class AW::Common::Communication::Background_BaseNetworkResponse;

class ApplicationSearchNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    ApplicationSearchNetworkRequest(QString searchKeyword);

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual void addAdditionalRequiredHeaders(QNetworkRequest *_networkRequest);
    virtual QByteArray getPostData() const;

private:
    QString _searchKeyword;
};

#endif // APPLICATIONSEARCHNETWORKREQUEST_H
