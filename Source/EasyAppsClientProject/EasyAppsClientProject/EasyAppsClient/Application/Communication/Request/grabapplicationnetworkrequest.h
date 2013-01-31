#ifndef GRABAPPLICATIONNETWORKREQUEST_H
#define GRABAPPLICATIONNETWORKREQUEST_H

#include <QObject>

#include "Application/Communication/Request/awbasenetworkrequest.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class GrabApplicationNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    explicit GrabApplicationNetworkRequest(QString appId, QString grabType, RequestSecurityTypeEnum requestSecurityType);

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual void addAdditionalRequiredHeaders(QNetworkRequest *_networkRequest);

private:
    QString _grabType;
};

#endif // GRABAPPLICATIONNETWORKREQUEST_H
