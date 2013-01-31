#include "addorupdatewholeappdescription.h"

// sql
#include <QSqlQuery>
#include <QVariant>

// utility
#include "Common/Helpers/databasehelper.h"
#include "appdataqueryhelpers.h"

AddOrUpdateWholeAppDescription::AddOrUpdateWholeAppDescription(ApplicationDescriptionModel appDescriptionModel, int updateAppDetailPartsConditionFlags, int updateStateConditionFlags, QString operationId) :
    DatabaseWorkAgentInterface(operationId),
    _inputAppDescriptionModel(appDescriptionModel),
    _updatedAppDescriptionModel(appDescriptionModel),
    _updateAppDetailPartsConditionFlags(updateAppDetailPartsConditionFlags),
    _updateStateConditionFlags(updateStateConditionFlags)
{}

QString AddOrUpdateWholeAppDescription::getAgentTypeId() const {
    static QString typeId("AddOrUpdateWholeAppDescription");
    return typeId;
}

ApplicationDescriptionModel AddOrUpdateWholeAppDescription::getInputAppDescriptionModel() {
    return this->_inputAppDescriptionModel;
}

ApplicationDescriptionModel AddOrUpdateWholeAppDescription::getUpdatedResultAppDescrptionModel()
{
    return this->_updatedAppDescriptionModel;
}

QStringList AddOrUpdateWholeAppDescription::getRepairableTableNames()
{
    // repair these
    return QStringList() << "dynamicAppInfos" << "staticAppInfos";
}

bool AddOrUpdateWholeAppDescription::doYourWork(QSqlDatabase databaseToWorkOn)
{
//    DLog("doYourWork") << this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId() << this->_appDescriptionModel.getDynamicDescription_ReadOnly()->getAppName();

    // get the current state and do some checking
    ApplicationDescriptionModel tmpCurrAppDescription;
    bool isCurrentDescriptionsFound = AppDataQueryHelpers::getDynamicAppDataByAppId(databaseToWorkOn, this->_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId(), tmpCurrAppDescription);

    DLog("--test ") << tmpCurrAppDescription.getDynamicDescription_ReadOnly()->getInstalledVersion() << tmpCurrAppDescription.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags() << tmpCurrAppDescription.getDynamicDescription_ReadOnly()->getNewestAvailableVersionForThisPlatform();

    if(isCurrentDescriptionsFound) {
        isCurrentDescriptionsFound = AppDataQueryHelpers::getStaticAppDataByAppId(databaseToWorkOn, this->_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId(), tmpCurrAppDescription);

        if(!isCurrentDescriptionsFound) {
            WLog("Current dynamic infos found, but static infos NOT!");
        }
    }

    if(isCurrentDescriptionsFound) {
            DLog("--test 2") << tmpCurrAppDescription.getDynamicDescription_ReadOnly()->getInstalledVersion() << tmpCurrAppDescription.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags() << tmpCurrAppDescription.getDynamicDescription_ReadOnly()->getNewestAvailableVersionForThisPlatform();

        // current dyn info found
        //  -> this will be an update, not an insert
        return this->doUpdate(databaseToWorkOn, tmpCurrAppDescription);
    }

    else {
        // current dyn info not found
        //  -> this will be an insert, NOT an update
        return this->doInsert(databaseToWorkOn);
    }
}


// -----------------------
// --- private methods ---

bool AddOrUpdateWholeAppDescription::doDynamicUpdateDependingOnUpdateConditionFlags(QSqlDatabase databaseToWorkOn, ApplicationDescriptionModel currentAppDescription, bool isSkipQueries)
{
    //
    // construct the query depending on the update-condition-flags

    //
    // columns part
    QString queryString("UPDATE dynamicAppInfos SET appName=?");

    // column name part

    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipInstalledRootPathIfUpdate) == 0 ) {
        // don't skip
        queryString += ", installedRootPath=?";
    } else {
        this->_updatedAppDescriptionModel.getDynamicDescription_DataForModification()->setInstalledRootPath(currentAppDescription.getDynamicDescription_ReadOnly()->getInstalledRootPath());
    }

    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipAppStateCodeIfUpdate) == 0 ) {
        // don't skip
        queryString += ", appStateCode=?";
    } else {
        this->_updatedAppDescriptionModel.getDynamicDescription_DataForModification()->setAppState(currentAppDescription.getDynamicDescription_ReadOnly()->getAppState());
    }

    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipSupportedPlatformFlagsIfUpdate) == 0 ) {
        // don't skip
        queryString += ", supportedPlatformFlags=?";
    } else {
        this->_updatedAppDescriptionModel.getDynamicDescription_DataForModification()->setSupportedPlatformFlags(currentAppDescription.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags());
    }

    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipIsHiddenIfUpdate) == 0 ) {
        queryString += ", isHidden=?";
    } else {
        this->_updatedAppDescriptionModel.getDynamicDescription_DataForModification()->setIsHidden(currentAppDescription.getDynamicDescription_ReadOnly()->getIsHidden());
    }

    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipIsStartWhenClientStartsIfUpdate) == 0 ) {
        queryString += ", isStartWhenClientStarts=?";
    } else {
        this->_updatedAppDescriptionModel.getDynamicDescription_DataForModification()->setIsStartWhenClientStarts(currentAppDescription.getDynamicDescription_ReadOnly()->getIsStartWhenClientStarts());
    }

    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipInstalledVersionIfUpdate) == 0 ) {
        queryString += ", installedVersion=?";
    } else {
        this->_updatedAppDescriptionModel.getDynamicDescription_DataForModification()->setInstalledVersion(currentAppDescription.getDynamicDescription_ReadOnly()->getInstalledVersion());
    }

    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipRunCountIfUpdate) == 0 ) {
        queryString += ", runCount=?";
    } else {
        this->_updatedAppDescriptionModel.getDynamicDescription_DataForModification()->setRunCount(currentAppDescription.getDynamicDescription_ReadOnly()->getRunCount());
    }

    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipGrabbedIconPathIfUpdate) == 0 ) {
        queryString += ", grabbedIconPath=?";
    } else {
        this->_updatedAppDescriptionModel.getDynamicDescription_DataForModification()->setGrabbedIconPath(currentAppDescription.getDynamicDescription_ReadOnly()->getGrabbedIconPath());
    }

    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipNewestVersionForThisPlatformIfUpdate) == 0 ) {
        queryString += ", newestAvailableVersionForThisPlatform=?";
    } else {
        this->_updatedAppDescriptionModel.getDynamicDescription_DataForModification()->setNewestAvailableVersionForThisPlatform(currentAppDescription.getDynamicDescription_ReadOnly()->getNewestAvailableVersionForThisPlatform());
    }

    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipAppStatusInStoreIfUpdate) == 0 ) {
        queryString += ", appStatusInStore=?";
    } else {
        this->_updatedAppDescriptionModel.getDynamicDescription_DataForModification()->setAppStatusInStore(currentAppDescription.getDynamicDescription_ReadOnly()->getAppStatusInStore());
    }

    // finish the query string - where part
    queryString += " WHERE appId=?";

    DLog(" [%%%] Update query: ") << queryString;


    if(isSkipQueries) {
        return false;
    }

    //
    // do the dynamic info part

    QSqlQuery queryForDynamic(databaseToWorkOn);
    queryForDynamic.prepare(queryString);

    // fix part
//    queryForDynamic.bindValue(0, _appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId());
    queryForDynamic.bindValue(0, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppName());
//    queryForDynamic.bindValue(1, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath());
//    queryForDynamic.bindValue(2, QVariant(_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppState()));

    // conditional part
    int queryArgIdx = 1;
    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipInstalledRootPathIfUpdate) == 0 ) {
        // don't skip
        queryForDynamic.bindValue(queryArgIdx, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath());
        ++queryArgIdx;
    }
    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipAppStateCodeIfUpdate) == 0 ) {
        // don't skip
        queryForDynamic.bindValue(queryArgIdx, QVariant(_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppState()));
        ++queryArgIdx;
    }
    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipSupportedPlatformFlagsIfUpdate) == 0 ) {
        // don't skip
        queryForDynamic.bindValue(queryArgIdx, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags());
        ++queryArgIdx;
    }
    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipIsHiddenIfUpdate) == 0 ) {
        // don't skip
        queryForDynamic.bindValue(queryArgIdx, QVariant(_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getIsHidden()));
        ++queryArgIdx;
    }
    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipIsStartWhenClientStartsIfUpdate) == 0 ) {
        // don't skip
        queryForDynamic.bindValue(queryArgIdx, QVariant(_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getIsStartWhenClientStarts()));
        ++queryArgIdx;
    }
    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipInstalledVersionIfUpdate) == 0 ) {
        // don't skip
        queryForDynamic.bindValue(queryArgIdx, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledVersion());
        ++queryArgIdx;
    }
    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipRunCountIfUpdate) == 0 ) {
        // don't skip
        queryForDynamic.bindValue(queryArgIdx, QVariant(_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getRunCount()));
        ++queryArgIdx;
    }
    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipGrabbedIconPathIfUpdate) == 0 ) {
        // don't skip
        queryForDynamic.bindValue(queryArgIdx, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getGrabbedIconPath());
        ++queryArgIdx;
    }
    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipNewestVersionForThisPlatformIfUpdate) == 0 ) {
        // don't skip
        queryForDynamic.bindValue(queryArgIdx, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getNewestAvailableVersionForThisPlatform());
        ++queryArgIdx;
    }
    if( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipAppStatusInStoreIfUpdate) == 0 ) {
        // don't skip
        queryForDynamic.bindValue(queryArgIdx, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppStatusInStore());
        ++queryArgIdx;
    }

    // WHERE appId=? part
    queryForDynamic.bindValue(queryArgIdx, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId());


    //
    // execute
    if(!queryForDynamic.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForDynamic);
        return false;
    }
    else {
        return true;
    }
}

bool AddOrUpdateWholeAppDescription::doUpdate(QSqlDatabase databaseToWorkOn, ApplicationDescriptionModel currentAppDescription)
{
    DLog("--do update");
//    this->_updatedAppDescriptionModel = currentDynamicAppDescription;

    bool isSkipQueryExecution = false;
    if(this->_updateStateConditionFlags != AppWhirr::UpdateStateCondition::USC_NoConditions) // if no condition flags defined then don't test any, leave isSkipQueries as false
    {

        //
        // some checking and comparing

        // state
        AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum currState = currentAppDescription.getDynamicDescription_ReadOnly()->getAppState();
        int diffState = ((int)this->_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppState()) - ((int)currState);

        //            DLog("State diff and case: ") << diffState << this->_updateConditionFlags;

        if( (diffState > 0) && ( (this->_updateStateConditionFlags & AppWhirr::UpdateStateCondition::USC_UpdateIfNewStateHigher) == 0) ) {
            // if diffState > 0 -> the new state is higher than the current
            //  but flag is not set - don't update
            isSkipQueryExecution = true;
        }
        else if( (diffState == 0) && ( (this->_updateStateConditionFlags & AppWhirr::UpdateStateCondition::USC_UpdateIfNewStateEqual) == 0) ) {
            // equal states
            //  but flag is not set - don't update
            isSkipQueryExecution = true;
        }
        else if( (this->_updateStateConditionFlags & AppWhirr::UpdateStateCondition::USC_UpdateIfNewStateLower) == 0) {
            // the new state is lower than the current
            //  but flag is not set - don't update
            isSkipQueryExecution = true;
        }
    }


    //
    // do the update

    bool isSuccess = true;

    if(!isSkipQueryExecution)
    {
        // start a transaction
        if(!databaseToWorkOn.transaction()) {
            DatabaseHelper::__logQueryExecutionFailedWithLastErrorForQuery(databaseToWorkOn, "transaction()");
            isSuccess = false;
        }
    }

    if(!isSuccess) {
        isSkipQueryExecution = true;
    }

    // do the update-condition-flag depending update query
    isSuccess = this->doDynamicUpdateDependingOnUpdateConditionFlags(databaseToWorkOn, currentAppDescription, isSkipQueryExecution);

    // update the static part as well in the _updatedAppDescr
//    this->_updatedAppDescriptionModel.setStaticDescription(this->_inputAppDescriptionModel.getStaticDescription_DataForModification());

    if((this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipStaticAppInfosIfUpdate) != 0) {
        //
        // apply the current data
        this->_updatedAppDescriptionModel.setStaticDescription(currentAppDescription.getStaticDescription_DataForModification());
    }

    if(isSkipQueryExecution) {
        return false;
    }

    if(isSuccess && ( (this->_updateAppDetailPartsConditionFlags & AppWhirr::UpdateAppDetailPartsCondition::SkipStaticAppInfosIfUpdate) == 0) ) {

        //
        // do the STATIC INFO part
        QString queryStringForStaticUpdate("INSERT OR REPLACE INTO staticAppInfos (appId, appName, executablePath, executableWorkingDirectory, svgIconPath, largeIconPath, mediumIconPath, smallIconPath, licenceFilePath) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
        QSqlQuery queryForStatic(databaseToWorkOn);
        queryForStatic.prepare(queryStringForStaticUpdate);

        queryForStatic.bindValue(0, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->_getAppId());
        queryForStatic.bindValue(1, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->_getAppName());
        queryForStatic.bindValue(2, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getExecutablePath());
        queryForStatic.bindValue(3, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getExecutableWorkingDirectory());
        queryForStatic.bindValue(4, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getSvgIconPath());

        queryForStatic.bindValue(5, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getLargeIconPath());
        queryForStatic.bindValue(6, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getMediumIconPath());
        queryForStatic.bindValue(7, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getSmallIconPath());
        queryForStatic.bindValue(8, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getLicenceFilePath());

        if(!queryForStatic.exec()) {
            DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForStatic);
            isSuccess = false;
        }
        else {
            isSuccess = true;
        }
    }

    if(isSuccess) {
        if(!databaseToWorkOn.commit()) {
            DatabaseHelper::__logQueryExecutionFailedWithLastErrorForQuery(databaseToWorkOn, "commit()");
            isSuccess = false;
        }
    }


    if(!isSuccess) {
        if(!databaseToWorkOn.rollback()) {
            DatabaseHelper::__logQueryExecutionFailedWithLastErrorForQuery(databaseToWorkOn, "rollback()");
            isSuccess = false;
        }
    }

    return isSuccess;
}

bool AddOrUpdateWholeAppDescription::doInsert(QSqlDatabase databaseToWorkOn)
{
    DLog("--do insert");

    bool isSuccess = false;

    // start a transaction
    if(!databaseToWorkOn.transaction()) {
        DatabaseHelper::__logQueryExecutionFailedWithLastErrorForQuery(databaseToWorkOn, "transaction()");
        return false;
    }

    // do the dynamic info part
    static QString queryStringForDynamicInsert("INSERT OR REPLACE INTO dynamicAppInfos (appId, appName, installedRootPath, appStateCode, supportedPlatformFlags, isHidden, isStartWhenClientStarts, runCount, installedVersion, grabbedIconPath, newestAvailableVersionForThisPlatform, appStatusInStore) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    QSqlQuery queryForDynamic(databaseToWorkOn);
    queryForDynamic.prepare(queryStringForDynamicInsert);

    queryForDynamic.bindValue(0, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId());
    queryForDynamic.bindValue(1, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppName());
    queryForDynamic.bindValue(2, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath());
    queryForDynamic.bindValue(3, QVariant(_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppState()));
    queryForDynamic.bindValue(4, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags());

    queryForDynamic.bindValue(5, QVariant(_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getIsHidden()));
    queryForDynamic.bindValue(6, QVariant(_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getIsStartWhenClientStarts()));
    queryForDynamic.bindValue(7, QVariant(_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getRunCount()));
    queryForDynamic.bindValue(8, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledVersion());
    queryForDynamic.bindValue(9, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getGrabbedIconPath());
    queryForDynamic.bindValue(10, _inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getNewestAvailableVersionForThisPlatform());

    queryForDynamic.bindValue(11, QVariant(_inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppStatusInStore()));

    if(!queryForDynamic.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForDynamic);
        isSuccess = false;
    }
    else {
        isSuccess = true;
    }

    if(isSuccess) {

        // do the static info part
        static QString queryStringForStaticInsert("INSERT OR REPLACE INTO staticAppInfos (appId, appName, executablePath, executableWorkingDirectory, svgIconPath, largeIconPath, mediumIconPath, smallIconPath, licenceFilePath) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
        QSqlQuery queryForStatic(databaseToWorkOn);
        queryForStatic.prepare(queryStringForStaticInsert);

        queryForStatic.bindValue(0, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->_getAppId());
        queryForStatic.bindValue(1, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->_getAppName());
        queryForStatic.bindValue(2, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getExecutablePath());
        queryForStatic.bindValue(3, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getExecutableWorkingDirectory());
        queryForStatic.bindValue(4, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getSvgIconPath());

        queryForStatic.bindValue(5, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getLargeIconPath());
        queryForStatic.bindValue(6, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getMediumIconPath());
        queryForStatic.bindValue(7, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getSmallIconPath());
        queryForStatic.bindValue(8, _inputAppDescriptionModel.getStaticDescription_ReadOnly()->getLicenceFilePath());

        if(!queryForStatic.exec()) {
            DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForStatic);
            isSuccess = false;
        }
        else {
            isSuccess = true;
        }
    }

    if(isSuccess) {
        if(!databaseToWorkOn.commit()) {
            DatabaseHelper::__logQueryExecutionFailedWithLastErrorForQuery(databaseToWorkOn, "commit()");
            isSuccess = false;
        }
    }


    if(!isSuccess) {
        if(!databaseToWorkOn.rollback()) {
            DatabaseHelper::__logQueryExecutionFailedWithLastErrorForQuery(databaseToWorkOn, "rollback()");
            isSuccess = false;
        }
    }

    return isSuccess;
}
