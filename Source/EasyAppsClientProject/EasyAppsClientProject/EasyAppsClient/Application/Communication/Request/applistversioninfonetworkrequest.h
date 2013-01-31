#ifndef APPLISTVERSIONINFONETWORKREQUEST_H
#define APPLISTVERSIONINFONETWORKREQUEST_H

#include "Application/Communication/Request/awbasenetworkrequest.h"

class AW::Common::Communication::Background_BaseNetworkResponse;

/*! Request to get version information for a list of app-ids.

    This request can be used to get update informations for apps in case the user did not log in - so no grab-sync can be performed.
*/
class AppListVersionInfoNetworkRequest : public AW::Client::Communication::AWBaseNetworkRequest
{
    Q_OBJECT
public:
    AppListVersionInfoNetworkRequest(QList<QString> appIdList);

    AW::Common::Communication::Background_BaseNetworkResponse *getResponseParser();

protected:
    virtual QString getRequestVersion() const;
    virtual QByteArray getPostData() const;

private:
    QList<QString> _appIdList;
};

#endif // APPLISTVERSIONINFONETWORKREQUEST_H
