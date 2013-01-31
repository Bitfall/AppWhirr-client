#include "appdataqueryhelpers.h"

#include <QSqlQuery>
#include <QVariant>

#include "Common/Helpers/databasehelper.h"

bool AppDataQueryHelpers::getStaticAppDataByAppId(QSqlDatabase database, QString appId, ApplicationDescriptionModel &outResult)
{
    // get the static part as well
    static QString queryStringForStatic("SELECT appId, appName, executablePath, executableWorkingDirectory, svgIconPath, largeIconPath, mediumIconPath, smallIconPath, licenceFilePath FROM staticAppInfos WHERE appId=? LIMIT 1");
    QSqlQuery queryForStatic(database);
    queryForStatic.prepare(queryStringForStatic);
    queryForStatic.bindValue(0, appId);

    if(!queryForStatic.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForStatic);
    }
    else if(queryForStatic.next()) {
        ApplicationDescriptionModel::StaticDescriptionModel *staticDescription = new ApplicationDescriptionModel::StaticDescriptionModel;
        staticDescription->_setAppId(queryForStatic.value(0).toString());
        staticDescription->_setAppName(queryForStatic.value(1).toString());
        staticDescription->setExecutablePath(queryForStatic.value(2).toString());
        staticDescription->setExecutableWorkingDirectory(queryForStatic.value(3).toString());
        staticDescription->setSvgIconPath(queryForStatic.value(4).toString());

        staticDescription->setLargeIconPath(queryForStatic.value(5).toString());
        staticDescription->setMediumIconPath(queryForStatic.value(6).toString());
        staticDescription->setSmallIconPath(queryForStatic.value(7).toString());
        staticDescription->setLicenceFilePath(queryForStatic.value(8).toString());

        outResult.setStaticDescription(staticDescription);

        // finished
        return true;
    }
    else {
        DLog("No results.");
    }

    return false;
}

bool AppDataQueryHelpers::getDynamicAppDataByAppId(QSqlDatabase database, QString appId, ApplicationDescriptionModel &outResult)
{
    static QString queryStringForDynamic("SELECT appId, appName, installedRootPath, appStateCode, supportedPlatformFlags, isHidden, isStartWhenClientStarts, runCount, installedVersion, grabbedIconPath, newestAvailableVersionForThisPlatform, appStatusInStore FROM dynamicAppInfos WHERE appId=? LIMIT 1");
    QSqlQuery queryForDynamic(database);
    queryForDynamic.prepare(queryStringForDynamic);
    queryForDynamic.bindValue(0, appId);

    if(!queryForDynamic.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForDynamic);
    }
    else if(queryForDynamic.next()){
        // dynamic
        ApplicationDescriptionModel::DynamicDescriptionModel *dynamicDescription = new ApplicationDescriptionModel::DynamicDescriptionModel;
        dynamicDescription->setAppId(queryForDynamic.value(0).toString());
        dynamicDescription->setAppName(queryForDynamic.value(1).toString());
        dynamicDescription->setInstalledRootPath(queryForDynamic.value(2).toString());
        dynamicDescription->setAppState((AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)queryForDynamic.value(3).toInt());
        dynamicDescription->setSupportedPlatformFlags(queryForDynamic.value(4).toString());
        //
        dynamicDescription->setIsHidden(queryForDynamic.value(5).toBool());
        dynamicDescription->setIsStartWhenClientStarts(queryForDynamic.value(6).toBool());
        dynamicDescription->setRunCount(queryForDynamic.value(7).toInt());
        dynamicDescription->setInstalledVersion(queryForDynamic.value(8).toString());
        dynamicDescription->setGrabbedIconPath(queryForDynamic.value(9).toString());
        dynamicDescription->setNewestAvailableVersionForThisPlatform(queryForDynamic.value(10).toString());
        //
        dynamicDescription->setAppStatusInStore((AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum)queryForDynamic.value(11).toInt());

        outResult.setDynamicDescription(dynamicDescription);
        return true;
    }
    else {
        DLog("No results.");
    }
    return false;
}

bool AppDataQueryHelpers::getAppStateByAppId(QSqlDatabase database, QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum &outResult)
{
    QString queryStringForAppState("SELECT appStateCode FROM dynamicAppInfos WHERE appId=? LIMIT 1");
    QSqlQuery queryForDynamic(database);
    queryForDynamic.prepare(queryStringForAppState);
    queryForDynamic.bindValue(0, appId);

    if(!queryForDynamic.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForDynamic);
    }
    else if(queryForDynamic.next()){
        outResult = (AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)queryForDynamic.value(3).toInt();
        return true;
    }
    else {
        DLog("No results.");
    }
    return false;
}
