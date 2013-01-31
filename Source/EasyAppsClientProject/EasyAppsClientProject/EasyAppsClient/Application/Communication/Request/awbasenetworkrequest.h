#ifndef AW_CLIENT_COMMUNICATION_AWBASENETWORKREQUEST_H
#define AW_CLIENT_COMMUNICATION_AWBASENETWORKREQUEST_H

#include "Common/Communication/basenetworkrequest.h"


namespace AW {
namespace Client {
namespace Communication {

class AWBaseNetworkRequest : public AW::Common::Communication::BaseNetworkRequest
{
    Q_OBJECT

public:
    /*! Security-type / leve of the request

            * BaseSecurity is the lowes available security - currently it means the request will go through SSL validation if required, but no user-login related authorization data is attached\n
            * UserLoginRequired will attach authorization information if available. Still the server will decide whether it needs it and whether the provided authorization information is correct or a user-login is needed.\n
            * ForceUserLogin will force user-login for a request - it won't login the user immedately, only if the server tells this action requires authorization, but if the server tells it requires then it will force the authorization and won't use any stored authorization informaiton.\n
        */
    enum RequestSecurityTypeEnum {
        RST_BaseSecurity = 0,
        RST_AuthorizationRequired = 1,
        RST_ForceUserLogin = 2
    };

public:

    /*! Create a new communication request
                */
    explicit AWBaseNetworkRequest(BaseCommunicationRequestType requestType,
                                  QUrl requestUrlString,
                                  RequestSecurityTypeEnum requestSecurityType,
                                  QNetworkRequest::CacheLoadControl cacheControl = QNetworkRequest::PreferNetwork,
                                  int timeoutInterval = 10000,
                                  bool isBufferDownloadedResourceAndSendItWhenDownloadFinished = true,
                                  AW::Common::Communication::NetworkRequestCacheDelegate *requestCacheDelegate = NULL);

    virtual ~AWBaseNetworkRequest();
    
protected:
    /*! Return the request's version (id).

        Every request have to specify it's version in the header - you have to specify the required version here and this will be added to the request's header automatically.

        @return The required request version.
    */
    virtual QString getRequestVersion() const = 0;

    virtual QString getUserAgent();

    virtual void addAdditionalRequiredHeaders(QNetworkRequest *networkRequest) {}

private:
    virtual bool appendHeadersToRequest();

private:
    RequestSecurityTypeEnum             _requestSecurityType;
};

} // namespace Communication
} // namespace Client
} // namespace AW

#endif // AW_CLIENT_COMMUNICATION_AWBASENETWORKREQUEST_H
