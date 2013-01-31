#ifndef STOREMAINPAGEWITHSECTIONSNETWORKREQUEST_H
#define STOREMAINPAGEWITHSECTIONSNETWORKREQUEST_H

#include <QObject>
#include "Application/Communication/Request/awbasenetworkrequest.h"

class StoreMainPageWithSectionsNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    explicit StoreMainPageWithSectionsNetworkRequest();

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual void addAdditionalRequiredHeaders(QNetworkRequest *_networkRequest);
};

#endif // STOREMAINPAGEWITHSECTIONSNETWORKREQUEST_H
