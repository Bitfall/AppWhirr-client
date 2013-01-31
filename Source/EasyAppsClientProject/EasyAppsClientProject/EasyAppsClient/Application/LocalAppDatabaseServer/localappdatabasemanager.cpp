#include "localappdatabasemanager.h"

#include <QMetaType>

#include "Common/Database/databaseserver.h"
#include "localappdatabaseworkperformer.h"

// database work agents
#include "./Agent/getappstateagent.h"
#include "./Agent/getallappdescriptionmodelsagent.h"
//#include "./Agent/getdynamicappinfoagent.h"
//#include "./Agent/getstaticappinfoagent.h"
#include "./Agent/getwholeappdescriptionagent.h"

#include "./Agent/addorupdatewholeappdescription.h"
#include "./Agent/updateappstateagent.h"
#include "./Agent/updateappishiddenagent.h"
#include "./Agent/updateappisstartwhenclientstartsagent.h"

// utility
#include "staticappinfofilehandler.h"
#include "Common/Helpers/pathhelper.h"

// config
#include "Settings/configmanager.h"

LocalAppDatabaseManager::LocalAppDatabaseManager(QObject *parent) :
    QObject(parent),
    _localAppDatabaseServer(NULL)
{
}

// ---------------------------------------------
// --- LocalAppDatabaseServerDelegate implementation ---
DatabaseWorkPerformerBase *LocalAppDatabaseManager::createDatabaseWorkerWithDatabaseAtPath(QString databasePath)
{
    return new LocalAppDatabaseWorkPerformer(databasePath);
}


// --------------------------------------------
// --- Initialization and server management ---

void LocalAppDatabaseManager::initialize()
{
    // register metatypes
    this->_registerRequiredMetaTypes();

    // create the server
    _localAppDatabaseServer = new DatabaseServer(this, this);
    // server signal connections
    connect(_localAppDatabaseServer, SIGNAL(serverStarted()), this, SIGNAL(serverStarted()));
    connect(_localAppDatabaseServer, SIGNAL(serverStopped()), this, SIGNAL(serverStopped()));
    connect(_localAppDatabaseServer, SIGNAL(finishedWithWorkAgent(DatabaseWorkAgentInterface*,bool)), this, SLOT(_finishedWithWorkAgent(DatabaseWorkAgentInterface*,bool)));
}

void LocalAppDatabaseManager::_registerRequiredMetaTypes()
{
    qRegisterMetaType<ApplicationDescriptionModel>("ApplicationDescriptionModel");
    qRegisterMetaType< QList<ApplicationDescriptionModel> >("QList<ApplicationDescriptionModel>");
    qRegisterMetaType< QMap<QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum> >("QMap<QString,AppWhirr::ApplicationStates::ApplicationStatesEnum>");
    qRegisterMetaType<AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum>("AppWhirr::ApplicationStates::ApplicationStatesEnum");
}


void LocalAppDatabaseManager::startServer(QString databasePath)
{
    if(_localAppDatabaseServer == NULL) {
        WLog("You have to call initialize() first!");
        return;
    }

    this->_localAppDatabaseServer->startServer(databasePath);
}

void LocalAppDatabaseManager::stopServer()
{
    this->_localAppDatabaseServer->addStopServerToQueue();
}


// --------------------------------------
// --- Main interface methods (async) ---


ApplicationDescriptionModel LocalAppDatabaseManager::_createGrabbedAppDescription(QString appId, QString appName, QString supportedPlatformFlags, QString newestAppVersionForThisPlatform)
{
    ApplicationDescriptionModel::StaticDescriptionModel *staticDescription = new ApplicationDescriptionModel::StaticDescriptionModel;
    ApplicationDescriptionModel::DynamicDescriptionModel *dynamicDescription = new ApplicationDescriptionModel::DynamicDescriptionModel;

    // set appId and appName in both
    staticDescription->_setAppId(appId);
    dynamicDescription->setAppId(appId);
    staticDescription->_setAppName(appName);
    dynamicDescription->setAppName(appName);

    // and set additional dynamic infos
    dynamicDescription->setSupportedPlatformFlags(supportedPlatformFlags);
    dynamicDescription->setAppState(AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled);
    dynamicDescription->setGrabbedIconPath(":/app_browser_large_icon");
    DLog("Set newest version: ") << newestAppVersionForThisPlatform;
    dynamicDescription->setNewestAvailableVersionForThisPlatform(newestAppVersionForThisPlatform);

    ApplicationDescriptionModel appDescriptionModel;
    appDescriptionModel.setDynamicDescription(dynamicDescription);
    appDescriptionModel.setStaticDescription(staticDescription);

    return appDescriptionModel;
}

void LocalAppDatabaseManager::addOrUpdateGrabbedApp(QString appId, QString appName, QString supportedPlatformFlagsForTheApp, QString newestAppVersionForThisPlatform, AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore)
{
    DLog("Newest version: ") << newestAppVersionForThisPlatform << supportedPlatformFlagsForTheApp;
    ApplicationDescriptionModel appDescrModel = this->_createGrabbedAppDescription(appId, appName, supportedPlatformFlagsForTheApp, newestAppVersionForThisPlatform);

    int updateAppDetailPartsConditionFlags = (AppWhirr::UpdateAppDetailPartsCondition::SkipIsHiddenIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipIsStartWhenClientStartsIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipRunCountIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipGrabbedIconPathIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipInstalledRootPathIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipAppStateCodeIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipInstalledVersionIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipStaticAppInfosIfUpdate);

    if(appStatusInStore == AppWhirr::ApplicationStatusInStore::NoAppStatusInStore) {
        updateAppDetailPartsConditionFlags |= AppWhirr::UpdateAppDetailPartsCondition::SkipAppStatusInStoreIfUpdate;
    } else {
        appDescrModel.getDynamicDescription_DataForModification()->setAppStatusInStore(appStatusInStore);
    }

    int updateStateConditionFlags = (int)AppWhirr::UpdateStateCondition::USC_UpdateAnyway;
    this->addOrUpdateByAppDescriptionModel(appDescrModel, updateAppDetailPartsConditionFlags, updateStateConditionFlags, "LocalAppDatabaseManager::addOrUpdateGrabbedApp");
}


bool LocalAppDatabaseManager::addInstalledApp(QString appId, QString appName, QString installedAppRootDir, QString installedAppVersion)
{
    QString relatedAppInfoPath = PathHelper::combineAndCleanPathes(installedAppRootDir, ConfigManager::getStaticAppInfoFileNameInInstallPack());
    if(!PathHelper::isFileExistsAndItsReallyAFile(relatedAppInfoPath)) {
        WLog("Cannot continue: The new, installed application's info file is not found at path: ") << relatedAppInfoPath << "for [ appId:" << appId << "] given app-name:" << appName;

        // downgrade to grabbed state
        this->updateAppState(appId, AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "LocalAppDatabaseManager::addInstalledApp");
        return false;
    }

    // #first: read accessible informations
    bool isSucceeded = true;
    ApplicationDescriptionModel appDescriptionModel;
    StaticAppInfoFileHandler *appInfoFileHandlerForRead = new StaticAppInfoFileHandler(relatedAppInfoPath);
    isSucceeded = appInfoFileHandlerForRead->parseStaticAppInfoToApplicationDescriptionModel(appDescriptionModel);
    delete appInfoFileHandlerForRead;

    if(!isSucceeded) {
        WLog("Cannot continue: The new, installed application's info file is found, but Unaccessable - at path: ") << relatedAppInfoPath << "for [ appId:" << appId << "] given app-name:" << appName;

        // downgrade to grabbed state
        this->updateAppState(appId, AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "LocalAppDatabaseManager::addInstalledApp");
        return false;
    }

    // #second: set install related informations

    // static
    appDescriptionModel.getStaticDescription_DataForModification()->_setAppId(appId);

    // dynamic
    ApplicationDescriptionModel::DynamicDescriptionModel *dynamicDescription = new ApplicationDescriptionModel::DynamicDescriptionModel;
    dynamicDescription->setAppId(appId);
    dynamicDescription->setAppName(appName);
//    dynamicDescription->setSupportedPlatformFlags(supportedPlatformFlagsForTheApp);
    dynamicDescription->setAppState(AppWhirr::ApplicationGrabStates::Installed);
    dynamicDescription->setGrabbedIconPath(":/app_browser_large_icon");
    dynamicDescription->setInstalledRootPath(installedAppRootDir);
    dynamicDescription->setInstalledVersion(installedAppVersion);

    LOG_AS_NOT_IMPLEMENTED_WITH_HINT("Should set the app-version as well, if it's not contained in the downloaded appInfo file. Currently it's not handled anywhere.");

    appDescriptionModel.setDynamicDescription(dynamicDescription);



    int updateAppDetailPartsConditionFlags = (AppWhirr::UpdateAppDetailPartsCondition::SkipIsHiddenIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipIsStartWhenClientStartsIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipRunCountIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipGrabbedIconPathIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipNewestVersionForThisPlatformIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipSupportedPlatformFlagsIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipAppStatusInStoreIfUpdate);

    int updateStateConditionFlags = (int)AppWhirr::UpdateStateCondition::USC_UpdateAnyway;
    this->addOrUpdateByAppDescriptionModel(appDescriptionModel, updateAppDetailPartsConditionFlags, updateStateConditionFlags, "LocalAppDatabaseManager::addInstalledApp");

    return true;
}

void LocalAppDatabaseManager::downgradeAppToGrabbedState(ApplicationDescriptionModel currentAppDescr)
{
    ApplicationDescriptionModel appDescrModel = this->_createGrabbedAppDescription(currentAppDescr.getDynamicDescription_ReadOnly()->getAppId(), currentAppDescr.getDynamicDescription_ReadOnly()->getAppName(), currentAppDescr.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags(), currentAppDescr.getDynamicDescription_ReadOnly()->getNewestAvailableVersionForThisPlatform());


    int updateAppDetailPartsConditionFlags = (AppWhirr::UpdateAppDetailPartsCondition::SkipIsHiddenIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipIsStartWhenClientStartsIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipRunCountIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipGrabbedIconPathIfUpdate |
                                              AppWhirr::UpdateAppDetailPartsCondition::SkipAppStatusInStoreIfUpdate);

    int updateStateConditionFlags = (int)AppWhirr::UpdateStateCondition::USC_UpdateAnyway;
    this->addOrUpdateByAppDescriptionModel(appDescrModel, updateAppDetailPartsConditionFlags, updateStateConditionFlags, "LocalAppDatabaseManager::downgradeAppToGrabbedState");
}

void LocalAppDatabaseManager::updateAppState(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState, int updateStateConditionFlags, QString operationId)
{
    DLog("updateAppState");
    this->_localAppDatabaseServer->addThisWorkAgentToQueue(new UpdateAppStateAgent(appId, newAppState, updateStateConditionFlags, operationId));
}


// ---------------------------------
// --- The magic switcher method ---

void LocalAppDatabaseManager::_finishedWithWorkAgent(DatabaseWorkAgentInterface *workAgent, bool isQuerySuccess)
{
    DLog("Finished work: ") << workAgent->getAgentTypeId() << workAgent->getOperationId() << isQuerySuccess;

    if(workAgent->getAgentTypeId() == "GetAppStateAgent") {
        GetAppStateAgent *castedAgent = dynamic_cast<GetAppStateAgent *>(workAgent);
        if(castedAgent != NULL) {
            Q_EMIT _result_getAppStateByAppId(castedAgent->getAppId(), castedAgent->getResult(), castedAgent->getOperationId(), isQuerySuccess);
        } else {
            WLog("Cannot cast the worker. Error.");
        }
    }

    else if(workAgent->getAgentTypeId() == "UpdateAppStateAgent") {
        UpdateAppStateAgent *castedAgent = dynamic_cast<UpdateAppStateAgent *>(workAgent);
        if(castedAgent != NULL) {
            Q_EMIT _result_updateAppStateByAppId(castedAgent->getAppId(), castedAgent->getInput(), castedAgent->getOperationId(), isQuerySuccess);
            if(isQuerySuccess) {
                // and the generalized signal(s)
                Q_EMIT appStateChanged(castedAgent->getAppId(), castedAgent->getInput());
            }
        } else {
            WLog("Cannot cast the worker. Error.");
        }
    }

    else if(workAgent->getAgentTypeId() == "GetAllAppDescriptionModelsAgent") {
        GetAllAppDescriptionModelsAgent *castedAgent = dynamic_cast<GetAllAppDescriptionModelsAgent *>(workAgent);
        if(castedAgent != NULL) {
            Q_EMIT _result_getAllAppDescriptions(castedAgent->getResult(), castedAgent->getOperationId(), isQuerySuccess);
        } else {
            WLog("Cannot cast the worker. Error.");
        }
    }

    else if(workAgent->getAgentTypeId() == "AddOrUpdateWholeAppDescription") {
        AddOrUpdateWholeAppDescription *castedAgent = dynamic_cast<AddOrUpdateWholeAppDescription *>(workAgent);
        if(castedAgent != NULL) {
            ApplicationDescriptionModel inputAppDescription = castedAgent->getInputAppDescriptionModel();
            ApplicationDescriptionModel updatedAppDescription = castedAgent->getUpdatedResultAppDescrptionModel();
            Q_EMIT _result_addOrUpdateByAppDescriptionModel(inputAppDescription, updatedAppDescription, castedAgent->getOperationId(), isQuerySuccess);
            if(isQuerySuccess) {
                // and the generalized signal(s)
                Q_EMIT appStateChanged(inputAppDescription.getDynamicDescription_ReadOnly()->getAppId(), inputAppDescription.getDynamicDescription_ReadOnly()->getAppState());
                Q_EMIT wholeAppDescriptionChanged(inputAppDescription, updatedAppDescription);
            }
        } else {
            WLog("Cannot cast the worker. Error.");
        }
    }

    else if(workAgent->getAgentTypeId() == "GetWholeAppDescriptionAgent") {
        GetWholeAppDescriptionAgent *castedAgent = dynamic_cast<GetWholeAppDescriptionAgent *>(workAgent);
        if(castedAgent != NULL) {
            ApplicationDescriptionModel resultAppDescription = castedAgent->getResult();
            if(!isQuerySuccess) {
                // ensure the result app-id the the input one
                resultAppDescription.getDynamicDescription_DataForModification()->setAppId(castedAgent->getInputAppId());
            }
            Q_EMIT _result_getAppDescriptionModelByAppId(resultAppDescription, castedAgent->getOperationId(), isQuerySuccess);
        } else {
            WLog("Cannot cast the worker. Error.");
        }
    }

    else if(workAgent->getAgentTypeId() == "UpdateAppIsHiddenAgent") {
        UpdateAppIsHiddenAgent *castedAgent = dynamic_cast<UpdateAppIsHiddenAgent *>(workAgent);
        if(castedAgent != NULL) {
            Q_EMIT _result_updateAppIsHidden(castedAgent->getAppId(), castedAgent->getInput(), castedAgent->getOperationId(), isQuerySuccess);
        } else {
            WLog("Cannot cast the worker. Error.");
        }
    }

    else if(workAgent->getAgentTypeId() == "UpdateAppIsStartWhenClientStartsAgent") {
        UpdateAppIsStartWhenClientStartsAgent *castedAgent = dynamic_cast<UpdateAppIsStartWhenClientStartsAgent *>(workAgent);
        if(castedAgent != NULL) {
            Q_EMIT _result_updateAppIsStartWhenClientStarts(castedAgent->getAppId(), castedAgent->getInput(), castedAgent->getOperationId(), isQuerySuccess);
        } else {
            WLog("Cannot cast the worker. Error.");
        }
    }

    // and finally, delete the work-agent
    delete workAgent;
}


// ------------------------------------------------
// --- Direct (async) operation starter methods ---

void LocalAppDatabaseManager::getAllAppDescriptions(QString operationId)
{
    DLog("getAllAppDescriptions");
    this->_localAppDatabaseServer->addThisWorkAgentToQueue(new GetAllAppDescriptionModelsAgent(operationId));
}

void LocalAppDatabaseManager::getAppDescriptionModelByAppId(QString appId, QString operationId)
{
    DLog("getAppDescriptionModelByAppId");
    this->_localAppDatabaseServer->addThisWorkAgentToQueue(new GetWholeAppDescriptionAgent(appId, operationId));
}

void LocalAppDatabaseManager::addOrUpdateByAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel, int updateAppDetailPartsConditionFlags, int updateStateConditionFlags, QString operationId)
{
    DLog("addOrUpdateByAppDescriptionModel");
    this->_localAppDatabaseServer->addThisWorkAgentToQueue(new AddOrUpdateWholeAppDescription(appDescriptionModel, updateAppDetailPartsConditionFlags, updateStateConditionFlags, operationId));
}

//void LocalAppLocalAppDatabaseManager::getDynamicPartOfAppDescriptionModelByAppId(QString appId, QString operationId)
//{
//    DLog("getDynamicPartOfAppDescriptionModelByAppId");
//    Q_EMIT _addThisWorkToWorkerQueue(new GetDynamicAppInfoAgent(appId, operationId));
//}

//void LocalAppLocalAppDatabaseManager::addOrUpdateDynamicPartOfAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel, QString operationId)
//{
//    DLog("addOrUpdateDynamicPartOfAppDescriptionModel");
//    Q_EMIT _addThisWorkToWorkerQueue(new AddOrUpdateDynamicAppInfoAgent(appDescriptionModel, operationId));
//}

//void LocalAppLocalAppDatabaseManager::getStaticPartOfAppDescriptionModelByAppId(QString appId, QString operationId)
//{
//    DLog("getStaticPartOfAppDescriptionModelByAppId");
//    Q_EMIT _addThisWorkToWorkerQueue(new GetStaticAppInfoAgent(appId, operationId));
//}

//void LocalAppLocalAppDatabaseManager::addOrUpdateStaticPartOfAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel, QString operationId)
//{
//    DLog("addOrUpdateStaticPartOfAppDescriptionModel");
//    Q_EMIT _addThisWorkToWorkerQueue(new AddOrUpdateStaticAppInfoAgent(appDescriptionModel, operationId));
//}

void LocalAppDatabaseManager::getAppStateByAppId(QString appId, QString operationId)
{
    DLog("getAppStateByAppId");
    this->_localAppDatabaseServer->addThisWorkAgentToQueue(new GetAppStateAgent(appId, operationId));
}

void LocalAppDatabaseManager::updateAppIsHidden(QString appId, bool newValue, QString operationId)
{
    this->_localAppDatabaseServer->addThisWorkAgentToQueue(new UpdateAppIsHiddenAgent(appId, newValue, operationId));
}

void LocalAppDatabaseManager::updateAppIsStartWhenClientStarts(QString appId, bool newValue, QString operationId)
{
    this->_localAppDatabaseServer->addThisWorkAgentToQueue(new UpdateAppIsStartWhenClientStartsAgent(appId, newValue, operationId));
}

// ----------------------------------------------
// --- connected to local-app-database-server ---
//void LocalAppDatabaseManager::_result_addOrUpdateByAppDescriptionModel(ApplicationDescriptionModel appDescriptionModel, QString operationId, bool isQuerySuccess)
//{
//    if(isQuerySuccess) {

//    }
//}

//void LocalAppDatabaseManager::_result_updateAppStateByAppId(QString appId, AppWhirr::ApplicationStates::ApplicationStatesEnum newAppState, QString operationId, bool isQuerySuccess)
//{
//    if(isQuerySuccess) {
//        Q_EMIT appStateChanged(appId, newAppState);
//    }
//}
