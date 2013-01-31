#ifndef GRABCLIENTUPDATENETWORKREQUEST_H
#define GRABCLIENTUPDATENETWORKREQUEST_H

#include "Common/Communication/basenetworkrequest.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class Background_BaseNetworkResponse;


class GrabClientUpdateNetworkRequest : public BaseNetworkRequest
{
    Q_OBJECT
public:
    explicit GrabClientUpdateNetworkRequest();

    Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // GRABCLIENTUPDATENETWORKREQUEST_H
