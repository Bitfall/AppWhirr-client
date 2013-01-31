#ifndef APPDATABASESYNCMANAGER_H
#define APPDATABASESYNCMANAGER_H

#include <QObject>

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

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


class AppDatabaseSyncManager : public QObject
{
    Q_OBJECT
public:
    explicit AppDatabaseSyncManager(QObject *parent = 0);

    /*! Starts a grab-sync for the current User. It will sync additional grabbed apps not yet presented in this machine if any. But it requires a logged-in User!

      @param isForceSync : if false (default) it will sync only if at least the configured sync time elapsed (configure: in ConfigManager). If true it will sync and will ignore the configured sync time.
    */
    void startCurrentUserGrabSync(bool isForceSync = false);

    /*! Starts a sync for the current User's local apps. It's not a grab-sync, won't add new apps grabbed by the User in another machine, it will only sync the currently available (local) apps' info.
    */
    void startCurrentUserAppListInfoSync();

    /*! Switches to 'check frequently' mode.

        After this method call this manager switches to automatic sync mode and will sync / check frequently.
    */
    void switchToSyncFrequentlyMode();

    // -----------------------------------
    // --- current user grab-list-sync ---
private Q_SLOTS:
    void grabbedAppsListRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> resultList);
    void _grabbedAppsListSyncFinished();
    void _grabbedAppsCommunicationFinishedCompletely();
    void grabbedAppsListRequestFailedWithErrorMessage(QString errorMessage);

    // --------------------------
    // --- app-list info sync ---
private Q_SLOTS:
    void _result_getAllAppDescriptions(QList<ApplicationDescriptionModel> appDescriptions, QString dbOperationId, bool isQuerySuccess);
    void __doOfflineAppListUpdateCheckForTheseApps(QList<ApplicationDescriptionModel> appDescriptions);
    void _offlineAppsListUpdateCheckRequestResult(QList<AW::Common::Communication::BaseNetworkEntity*> resultList);
    void _offlineAppsListUpdateCheckSyncFinished();
    void _offlineAppsListUpdateCheckRequestFinished();
    void _offlineAppsListUpdateCheckRequestFailedWithErrorMessage(QString errorMessage);

    //
private Q_SLOTS:
    void __startSyncBestFit();

private:
    CommunicationDescriptionGateway *_grabbedAppsCommunicationDescriptionGateway;
    CommunicationDescriptionGateway *_offlineGrabbedAppUpdateCheckCommunicationDescriptionGateway;

    QTimer *_syncAutomaticallyTimer;

    quint64 _currentUserLastSyncTimestamp;
};

#endif // APPDATABASESYNCMANAGER_H
