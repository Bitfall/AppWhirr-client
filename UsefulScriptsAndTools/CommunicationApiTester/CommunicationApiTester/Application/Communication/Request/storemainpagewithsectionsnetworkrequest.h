#ifndef STOREMAINPAGEWITHSECTIONSNETWORKREQUEST_H
#define STOREMAINPAGEWITHSECTIONSNETWORKREQUEST_H

#include <QObject>
#include "Common/Communication/basenetworkrequest.h"

class StoreMainPageWithSectionsNetworkRequest : public BaseNetworkRequest
{
    Q_OBJECT
public:
    explicit StoreMainPageWithSectionsNetworkRequest();

    Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // STOREMAINPAGEWITHSECTIONSNETWORKREQUEST_H
