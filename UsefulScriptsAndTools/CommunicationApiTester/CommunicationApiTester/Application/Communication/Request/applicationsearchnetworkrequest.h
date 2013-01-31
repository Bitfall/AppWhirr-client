#ifndef APPLICATIONSEARCHNETWORKREQUEST_H
#define APPLICATIONSEARCHNETWORKREQUEST_H

#include "Common/Communication/basenetworkrequest.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class Background_BaseNetworkResponse;

class ApplicationSearchNetworkRequest : public BaseNetworkRequest
{
    Q_OBJECT
public:
    /**
      * You should percent encode the keyword before passing it as a parameter
      * use QUrl::toPercentEncoding(keyword, QByteArray(), " -._~");
      */
    ApplicationSearchNetworkRequest(const QString& keyword);

    Background_BaseNetworkResponse* getResponseParser();

protected:
    virtual QString getRequestVersion() const;
};

#endif // APPLICATIONSEARCHNETWORKREQUEST_H
