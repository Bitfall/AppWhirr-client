#ifndef APPINSTALLDELEGATE_H
#define APPINSTALLDELEGATE_H

#include <QObject>
#include <QQueue>
#include <QMap>
#include <QUrl>
#include <QMutex>

#include <QLocalSocket>

QT_BEGIN_NAMESPACE
class QLocalServer;
class QTimer;
QT_END_NAMESPACE

#include "grabapplicationqueueitem.h"

/*! Delegate to handle app installation.

    It's designed to be an internal delegate of the \a GrabApplicationManager.
*/
class AppInstallDelegate : public QObject
{
    Q_OBJECT
public:
    explicit AppInstallDelegate(QObject *parent = 0);
    ~AppInstallDelegate();

    // ------------------------
    // --- public interface ---
public Q_SLOTS:
    void installThisApp(GrabApplicationQueueItem grabItem);
Q_SIGNALS:
    void appInstallProgressStateMessage(GrabApplicationQueueItem grabItem, QString progressStateMessage);
    void installFailed(GrabApplicationQueueItem grabItem, QString errorMessage);
    void appInstalledSuccessfullyToDirectory(GrabApplicationQueueItem grabItem, QString relatedAppInstalledRootPath);

    // ---------------------------------
    // if install hangs because of resolution needed you have to communicate with these signals/slots
    //  resolution may be required if the app is still running and the install cannot replace it with a newer version
Q_SIGNALS:
    void __resolutionNeededForRunningApp(QString appId, QString appName);
public Q_SLOTS:
    void __resolutionDoneRetryInstall(QString appId);
    void __resolutionFailed(QString appId);

    // ----------------
    // --- internal ---
private Q_SLOTS:
    /*! Try installing from the next waiting app from the waiting-queue.

        If there are waiting apps and currently none is installing start a new install.
    */
    void _doInstallFromWaitingQueue();

Q_SIGNALS:
    /*! Ping the waiting-queue. If there are waiting apps and currently none is installing start a new install.
    */
    void __tryNextAppInstallFromWaitingQueue();

private Q_SLOTS:
    void updaterMessageTimeout();
    void updaterMessageFailedWithError(QLocalSocket::LocalSocketError errorCode);
    void _installFailedWithError(QLocalSocket::LocalSocketError errorCode, bool isStopInstallingProcessImmediatelly);
    void updaterMessageReadyToRead();
    void newConnection();
private:
    bool getIsCurrentlyInstalling();
    void setIsCurrentlyInstalling(bool value);
    
private:
    QQueue<QString>                         _applicationZipsWaitingForInstallation;
    QMap<QUrl, GrabApplicationQueueItem>    _installingAppZipToGrabItemMapper;
    QMap<QUrl, GrabApplicationQueueItem>    _installWaitsForResolutionMap;
    QString                                 _currInstallingZipFullPath;
    QLocalServer                            *_localServer;

    QLocalSocket                            *_currUpdaterMessagingLocalSocket;
    QTimer                                  *_waitingForUpdaterTimeoutTimer;

    QMutex                                  _isCurrentlyInstallingMutex;
    bool                                    _isCurrentlyInstalling;
};

#endif // APPINSTALLDELEGATE_H
