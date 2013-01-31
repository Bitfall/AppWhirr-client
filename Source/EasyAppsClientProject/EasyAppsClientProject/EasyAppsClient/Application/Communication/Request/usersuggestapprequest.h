#ifndef USERSUGGESTAPPREQUEST_H
#define USERSUGGESTAPPREQUEST_H

#include <QObject>
#include "Application/Communication/Request/awbasenetworkrequest.h"

class UserSuggestAppRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    explicit UserSuggestAppRequest(QString name, QString website, QString description);

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual QByteArray getPostData() const;

private:
    QString _name;
    QString _website;
    QString _description;
};

#endif // USERSUGGESTAPPREQUEST_H
