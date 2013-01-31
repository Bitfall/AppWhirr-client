#ifndef APPLICATIONLISTNETWORKREQUEST_H
#define APPLICATIONLISTNETWORKREQUEST_H

#include <QObject>
#include "Application/Communication/Request/awbasenetworkrequest.h"

class ApplicationListNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    explicit ApplicationListNetworkRequest(QString _listType);

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual void addAdditionalRequiredHeaders(QNetworkRequest *_networkRequest);

private:
    QString _listType;
};

#endif // APPLICATIONLISTNETWORKREQUEST_H
