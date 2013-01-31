#ifndef APPGRABDELEGATE_H
#define APPGRABDELEGATE_H

#include <QObject>
#include <QQueue>
#include <QUrl>

#include "grabapplicationqueueitem.h"

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class CommunicationDescriptionGateway;

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
}
}
}

/*! Delegate to handle app grab requesting and to get required information to complete app grab and install.

    It's designed to be an internal delegate of the \a GrabApplicationManager.
*/
class AppGrabDelegate : public QObject
{
    Q_OBJECT
public:
    explicit AppGrabDelegate(QObject *parent = 0);
    ~AppGrabDelegate();

    // ------------------------
    // --- public interface ---
public Q_SLOTS:
    /*! Start a new grab - async.

        This is the main interface to start a grab.
    */
    void grabThisItem(GrabApplicationQueueItem grabItem);
    
Q_SIGNALS:
    /*! Grab request started.
    */
    void grabStarted(GrabApplicationQueueItem grabItem);

    /*! Grab request succeeded - allowed to download the app from the given \a appDownloadUrl .
    */
    void grabSucceeded(GrabApplicationQueueItem grabItem, QUrl appDownloadUrl);

    /*! Grab request failed - cannot download the app, interrupt the grab/installation process.
    */
    void grabFailed(GrabApplicationQueueItem grabItem, QString errorMessage);

    // ----------------
    // --- internal ---
private Q_SLOTS:
    /*! Check for waiting grab requests and start the next one if found.
    */
    void tryToStartNextPendingApplicationGrabDescriptionRequest();

    /*! Auth challange happened - wait for user-login then try it again.
    */
    void _authenticationChallangeHappened_PutBackToQueue();

    /*!
    */
    void authenticationChallengeCancelled();

    /*! Request finished with the result: \a entities .
    */
    void appGrabDescriptionRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity *> entities);

    /*! Request failed with \a errorMessage.

        It's either a network / server error or the user is not allowed to download the app. The app cannot be downloaded - interrupt the grab/installation process for this app.
    */
    void failedToGetAppGrabDescription(QString errorMessage);

Q_SIGNALS:
    void __tryToStartNextPendingApplicationGrabDescriptionRequest();

private Q_SLOTS:
    void authorizationTimeout();

private:
    CommunicationDescriptionGateway *_appGrabCommunicationDescriptionGateway;
    QQueue<GrabApplicationQueueItem> _grabItemsWaitingForGrab;

    QTimer *_authorizationTimeoutTimer;
    bool _isShouldForceAuthorizeGrabRequest;
};

#endif // APPGRABDELEGATE_H
