#ifndef GRABAPPLICATIONNETWORKREQUEST_H
#define GRABAPPLICATIONNETWORKREQUEST_H

#include <QObject>

#include "Common/Communication/basenetworkrequest.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class GrabApplicationNetworkRequest : public BaseNetworkRequest
{
    Q_OBJECT
public:
    explicit GrabApplicationNetworkRequest(const QString& applicationID, QString grabType);

    Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // GRABAPPLICATIONNETWORKREQUEST_H
