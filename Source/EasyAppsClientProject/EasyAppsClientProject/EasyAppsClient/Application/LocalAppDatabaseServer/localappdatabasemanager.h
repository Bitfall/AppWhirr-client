#ifndef LOCALAPPDATABASEMANAGER_H
#define LOCALAPPDATABASEMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>

#include "Application/ApplicationDescription/applicationstatesenum.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Common/Database/databaseserverdelegate.h"
#include "addorupdateconditionflags.h"

class DatabaseServer;
class DatabaseWorkAgentInterface;

//
// It's a wrapper class around the DatabaseServer class and extends it with local-add-database related methods
//  * it creates and runs a DatabaseServer internally - you can start/stop it with start/stopServer() methods
//  * [!] initialize() MUST BE called before any usage!
//  * also startServer() have to be called before any operation
//  * Currently every method is ASYNC, except the initialize() method
//
// Ideas to improve:
//  # Decrease RAM usage by maintaining a cache - this way the smaller objects like the app-related icons should not store the whole app-data if it depends only on a few attributes
//      * create and maintain a cache like this: "QMap<QString, ApplicationDescriptionModel> _localAppDatabaseCache;" -> appId to app-data map
//      ** This will allow synchronous getters
//      ** [!] But it should work in a thread-safe mannager !!!
//      ** And also it should be kept synchronized with the database -> every setter result should update it
//
class LocalAppDatabaseManager : public QObject, public DatabaseServerDelegate
{
    Q_OBJECT
public:
    explicit LocalAppDatabaseManager(QObject *parent=0);

    // ---------------------------------------------
    // --- DatabaseServerDelegate implementation ---
    DatabaseWorkPerformerBase *createDatabaseWorkerWithDatabaseAtPath(QString databasePath);

    // --------------------------------------------
    // --- Initialization and server management ---
    //
    // initialize MUST BE called before any usage!
    void initialize();
    // won't start immediatelly - will emit serverStarted signal when it's started
    void startServer(QString databasePath);
    // won't stop immediatelly - will emit serverStopped signal when it's stopped
    void stopServer();

Q_SIGNALS: // server signals
    void serverStarted();
    void serverStopped();

public:
    // --------------------------------------
    // --- Main interface methods (async) ---
    // connect to the related callback signals for result

    //
    // will add the app as grabbed if it's not yet grabbed
    //  for example: if the app is already added and it's installed then it won't do anything
    void addOrUpdateGrabbedApp(QString appId, QString appName, QString supportedPlatformFlagsForTheApp, QString newestAppVersionForThisPlatform, AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore);
    //
    // an app has to be grabbed (call addGrabbedApp) before it could be installed
    //  if you call this method before a grab then some values will be set to default, which is not good for a value like supportedPlatformFlags'
    bool addInstalledApp(QString appId, QString appName, QString installedAppRootDir, QString installedAppVersion);

    //
    // call this if the app have to be downgraded - e.g. install fails, or it is removed
    void downgradeAppToGrabbedState(ApplicationDescriptionModel currentAppDescr);
    void updateAppState(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState, int updateStateConditionFlags, QString operationId);

    // ------------------------------------------------
    // --- Direct (async) operation starter methods ---
    // connect to the related _result_ signals if you need the result

    //
    void getAllAppDescriptions(QString operationId);
    // whole app description
    void getAppDescriptionModelByAppId(QString appId, QString operationId);
    //
    // for the updateConditionFlags check the AppWhirr::AddOrUpdateConditionFlags enum
    void addOrUpdateByAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel, int updateAppDetailPartsConditionFlags, int updateStateConditionFlags, QString operationId);
    // only dynamic part
//    void getDynamicPartOfAppDescriptionModelByAppId(QString appId, QString operationId);
//    void addOrUpdateDynamicPartOfAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel, QString operationId);
    // only static part
//    void getStaticPartOfAppDescriptionModelByAppId(QString appId, QString operationId);
//    void addOrUpdateStaticPartOfAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel, QString operationId);
    // smaller queries
    void getAppStateByAppId(QString appId, QString operationId);

    void updateAppIsHidden(QString appId, bool newValue, QString operationId);
    void updateAppIsStartWhenClientStarts(QString appId, bool newValue, QString operationId);


Q_SIGNALS: // direct operation result callbacks - will be called even if the query fails (isQuerySuccess will be false in this case)

    void _result_getAllAppDescriptions(QList<ApplicationDescriptionModel> result, QString operationId, bool isQuerySuccess);

    void _result_getAppDescriptionModelByAppId(ApplicationDescriptionModel appDescriptionModel, QString operationId, bool isQuerySuccess);
    //
    // updatedAppDescriptionModel: if an add/insert happens it will be the same as the inputAppDescriptionModel
    //  but if it's an update it will hold the updated result, even if the query fails
    void _result_addOrUpdateByAppDescriptionModel(ApplicationDescriptionModel inputAppDescriptionModel, ApplicationDescriptionModel updatedAppDescriptionModel, QString operationId, bool isQuerySuccess);
    // only dynamic part
//    void _result_getDynamicPartOfAppDescriptionModelByAppId(ApplicationDescriptionModel appDescriptionModel, QString operationId, bool isQuerySuccess);
//    void _result_addOrUpdateDynamicPartOfAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel, QString operationId, bool isQuerySuccess);
//    // only static part
//    void _result_getStaticPartOfAppDescriptionModelByAppId(ApplicationDescriptionModel appDescriptionModel, QString operationId, bool isQuerySuccess);
//    void _result_addOrUpdateStaticPartOfAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel, QString operationId, bool isQuerySuccess);
    // smaller queries
    void _result_getAppStateByAppId(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState, QString operationId, bool isQuerySuccess);
    void _result_updateAppStateByAppId(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState, QString operationId, bool isQuerySuccess);

    void _result_updateAppIsHidden(QString appId, bool newValue, QString operationId, bool isQuerySuccess);
    void _result_updateAppIsStartWhenClientStarts(QString appId, bool newValue, QString operationId, bool isQuerySuccess);

Q_SIGNALS: // other useful change signals - will be emitted every time a related change happens
    // [!] these won't be called if the query fails
    void appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState);
    void batchAppStateChanged(QMap<QString, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum> appIdAndNewStateMap);
    //
    // updatedAppDescriptionModel: if an add/insert happens it will be the same as the inputAppDescriptionModel
    //  but if it's an update it will hold the updated result, even if the query fails
    void wholeAppDescriptionChanged(ApplicationDescriptionModel inputAppDescriptionModel, ApplicationDescriptionModel updatedAppDescriptionModel);
    //
    // if only dynamic changes it's not safe to use the static part to make updates in the database!
//    void dynamicAppDescriptionChanged(ApplicationDescriptionModel appDescriptionModel);
//    void staticAppDescriptionChange(ApplicationDescriptionModel appDescriptionModel);

private Q_SLOTS:
    void _finishedWithWorkAgent(DatabaseWorkAgentInterface *workAgent, bool isQuerySuccess);

    // ----------------------------------------------
    // --- connected to local-app-database-server ---
//    void _result_addOrUpdateByAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel, QString operationId, bool isQuerySuccess);
//    void _result_updateAppStateByAppId(QString appId, AppWhirr::ApplicationStates::ApplicationStatesEnum newAppState, QString operationId, bool isQuerySuccess);

private:
    void _registerRequiredMetaTypes();

    ApplicationDescriptionModel _createGrabbedAppDescription(QString appId, QString appName, QString supportedPlatformFlags, QString newestAppVersionForThisPlatform);

private:
    DatabaseServer *_localAppDatabaseServer;
};

#endif // LOCALAPPDATABASEMANAGER_H
