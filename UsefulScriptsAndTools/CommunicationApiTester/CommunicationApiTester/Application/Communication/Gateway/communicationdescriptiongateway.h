#ifndef COMMUNICATIONDESCRIPTIONGATEWAY_H
#define COMMUNICATIONDESCRIPTIONGATEWAY_H

#include <QObject>
#include <QList>
#include "Common/Communication/basenetworkentity.h"

QT_BEGIN_NAMESPACE
class QByteArray;
class QString;
QT_END_NAMESPACE

class BaseNetworkRequest;
class Background_BaseNetworkResponse;

//
// !!! Only 1 request can be active !!!
//
//  If you set a request for perform then you SHOULD NOT use it after you gave it to the performer method - it will perform it and it will delete it when finished!
class CommunicationDescriptionGateway : public QObject
{
    Q_OBJECT

public:
    enum GrabRequestType {
        GRT_Install = 1,
        GRT_Update = 2
    };

public:
    explicit CommunicationDescriptionGateway(QObject *parent = 0);
    virtual ~CommunicationDescriptionGateway();

//    void requestForInitialPageDescription();
    void requestForApplication(const QString& applicationID);
    void requestForGrabbingApplication(const QString& applicationID);
    void requestForLoggingUserIn();
    void requestForGrabbingApplicationByAppId(const QString& applicationID, GrabRequestType grabType);
    void requestForUserGrabList();
    void requestForClientUpdateVersionCheck();
    void requestForClientUpdateZip();

    void performThisRequest(BaseNetworkRequest *requestToPerform);

Q_SIGNALS:
    void failedToRetrieveDescription(const QString& error);
    //
    // this finish will be emitted if the request is returned with result and not with an error
    //  BUT this is just a partial finish - to perform a new request you have to wait until finishedCompletely() is called (which is called even if error happened)
    void finished(const QList<BaseNetworkEntity*>& entities);
    //
    // this will be emitted when the request if complete and the result is sent out
    //  after this signal it's safe to start a new request
    void finishedCompletely();
    void authenticationChallengeHappened();

    /*! Will be emitted when a request got a response.

        This is the unprocessed / non-parsed plain data. It's mainly for debugging, don't use it if you don't have to. Connect to \a finished() instead to get the parsed response entities.
    */
    void _receivedResponseData(QByteArray responseData);

private Q_SLOTS:
    void requestFinished();
    void requestFailed();
    void requestReceived(QByteArray response);
    void responseFinished();
    void authenticationChallenge();

private:
    void setupRequestConnections();

    void _finishedWithCommunicationStep();

private:
    BaseNetworkRequest*         networkRequest;
    Background_BaseNetworkResponse*        networkResponse;
    bool _isRequestActive;
    int _finishedSteps;
};

#endif // COMMUNICATIONDESCRIPTIONGATEWAY_H
