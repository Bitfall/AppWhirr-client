#ifndef GRABMANAGER_H
#define GRABMANAGER_H

#include <QObject>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QUrl>
#include <QLocalSocket>

class CommunicationDescriptionGateway;
class CommunicationResourceGateway;
//class AW::Common::Communication::BaseNetworkEntity;

QT_BEGIN_NAMESPACE
class QLocalServer;
class QTimer;
QT_END_NAMESPACE

#include "grabapplicationqueueitem.h"

class AppGrabDelegate;
class AppDownloadDelegate;
class AppFrameworkDelegate;
class AppInstallDelegate;

/*! The high-level manager of grabbing an app.

    It's separated to multiple internal delegates to handle the required steps of the app grabbing.\n
    Currently these steps are required and supported:\n
    1) request to get the resource's url and other required app information\n
    2) download the resource (app's zip)\n
    3) checking for required frameworks and install them if required\n
    4) intall the app\n

    About the queues:\n
    1: grab) grab-descriptions are downloaded one-by-one (synchronous). Stored in a queue and only 1 can be get at a given time.
    2: download) resouce-downloadings are parallel - the CommunicationResourceGateway manages them (the default is maximum 5 downloading at a time)
    3: install) installing is also a one-by-by process (synchronous) - only 1 app can be installed at once.
*/
class GrabApplicationManager : public QObject
{
    Q_OBJECT

public:
    explicit GrabApplicationManager(QObject *parent = 0);

    // ------------------------
    // --- public interface ---

    /*! The entry method to start grabbing an app.

        @param appSupportedSummedPlatformFlags : the summed supported platform flags, containing every platform flag supported by the app
        @param isUpdate : set it to true if this install will be an update. Set it to false if it's an initial install.
    */
    void grabApplication(const QString& applicationID, QString appName, QString appSupportedSummedPlatformFlags, bool isUpdate, AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore);

    // -----------------------------------------
    // --- public interface feedback signals ---
Q_SIGNALS:
    void appGrabStarted(QString appId, QString appName);
    void appGrabbedSuccessfully(QString appId, QString appName, QString appSupportedSummedPlatformFlags, QString newestVersionAvailableForThisPlatform, AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore);

    void appDownloadProgress(QString appId, int progress);

    void appInstallationStarted(QString appId, QString appName);
    void appInstallProgressStateMessage(QString appId, QString progressStateMessage);
    void appInstalledSuccessfullyToDirectory(QString appId, QString appName, QString installedAppRootDir, QString installedAppVersion);

    // will be emitted if one of the grab steps fails
    void appGrabFailed(QString appId, QString appName, bool isUpdate, QString errorMessage);
    // will be emitted if one of the install steps fails, which includes the downloading as well
    void appInstallFailed(QString appId, QString appName, bool isUpdate, QString errorMessage);

    // ---
    // if framework install required for an app framework-handling will be suspended until you allow or deny the install
Q_SIGNALS:
    void __frameworksRequireInstallForApp(QString appId, QString appName);
public Q_SLOTS:
    void __allowFrameworkInstallForApp(QString appId);
    void __denyFrameworkInstallForApp(QString appId);

    // ---
    // if install hangs because of resolution needed you have to communicate with these signals/slots
Q_SIGNALS:
    void __resolutionNeededForRunningApp(QString appId, QString appName);
public Q_SLOTS:
    void __resolutionDoneRetryInstall(QString appId);
    void __resolutionFailed(QString appId);


    // ------------------------------
    // --- internal grab handlers ---
Q_SIGNALS:
    void __grabThisItem(GrabApplicationQueueItem grabItem);
private Q_SLOTS:
    void __grabStarted(GrabApplicationQueueItem grabItem);
    void __grabSucceeded(GrabApplicationQueueItem grabItem, QUrl appDownloadUrl);
    void __grabFailed(GrabApplicationQueueItem grabItem, QString errorMessage);


    // --------------------------------------
    // --- internal app download handlers ---
Q_SIGNALS:
    void __downloadThisItem(GrabApplicationQueueItem grabItem);
private Q_SLOTS:
    void __downloadStarted(GrabApplicationQueueItem grabItem);
    void __downloadProgress(GrabApplicationQueueItem grabItem, int percent);
    void __downloadFinished(GrabApplicationQueueItem grabItem);
    void __downloadFailed(GrabApplicationQueueItem grabItem, QString errorMessage);

    // ------------------------------------------------
    // --- internal app framework checking handlers ---
Q_SIGNALS:
    void __checkFrameworksForThisApp(GrabApplicationQueueItem grabItem);
private Q_SLOTS:
    void __checkFrameworksSucceededForApp(GrabApplicationQueueItem grabItem);
    void __checkFrameworksFailedForApp(GrabApplicationQueueItem grabItem, QString errorMessage);

    // -------------------------------------
    // --- internal app install handlers ---
Q_SIGNALS:
    void __installThisApp(GrabApplicationQueueItem grabItem);
private Q_SLOTS:
    void __installProgressStateMessage(GrabApplicationQueueItem grabItem, QString progressStateMessage);
    void __installSucceededToDirectory(GrabApplicationQueueItem grabItem, QString relatedAppInstalledRootPath);
    void __installFailed(GrabApplicationQueueItem grabItem, QString errorMessage);

private:
    // delegates
    AppGrabDelegate *_appGrabDelegate;
    AppDownloadDelegate *_appDownloadDelegate;
    AppFrameworkDelegate *_appFrameworkDelegate;
    AppInstallDelegate *_appInstallDelegate;
    //---

};

#endif // GRABMANAGER_H
