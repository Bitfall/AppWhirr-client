#ifndef APPLICATIONNETWORKREQUEST_H
#define APPLICATIONNETWORKREQUEST_H

#include <QObject>
#include "Common/Communication/basenetworkrequest.h"

class ApplicationNetworkRequest : public BaseNetworkRequest
{
    Q_OBJECT
public:
    explicit ApplicationNetworkRequest(const QString& applicationID);

    Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // APPLICATIONNETWORKREQUEST_H
