#ifndef USERFEEDBACKREQUEST_H
#define USERFEEDBACKREQUEST_H

#include <QObject>
#include "Application/Communication/Request/awbasenetworkrequest.h"

class UserFeedbackRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    explicit UserFeedbackRequest(QString feedbackMessage);

    AW::Common::Communication::Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual QByteArray getPostData() const;

private:
    QString _feedback;
};

#endif // USERFEEDBACKREQUEST_H
