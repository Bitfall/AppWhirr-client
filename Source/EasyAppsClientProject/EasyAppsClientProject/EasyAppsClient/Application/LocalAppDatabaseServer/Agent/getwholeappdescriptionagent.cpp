#include "getwholeappdescriptionagent.h"

#include <QVariant>
#include <QSqlQuery>

#include "Common/Helpers/databasehelper.h"

GetWholeAppDescriptionAgent::GetWholeAppDescriptionAgent(QString appId, QString operationId) :
    DatabaseWorkAgentInterface(operationId),
    _appId(appId)
{}

QString GetWholeAppDescriptionAgent::getAgentTypeId() const {
    static QString typeId("GetWholeAppDescriptionAgent");
    return typeId;
}

QString GetWholeAppDescriptionAgent::getInputAppId() const {
    return this->_appId;
}

ApplicationDescriptionModel GetWholeAppDescriptionAgent::getResult() {
    return this->_appDescriptionModel;
}

QStringList GetWholeAppDescriptionAgent::getRepairableTableNames()
{
    // repair these
    return QStringList() << "dynamicAppInfos" << "staticAppInfos";
}

bool GetWholeAppDescriptionAgent::doYourWork(QSqlDatabase databaseToWorkOn)
{
    static QString queryStringForDynamic("SELECT appId, appName, installedRootPath, appStateCode, supportedPlatformFlags, isHidden, isStartWhenClientStarts, runCount, installedVersion, grabbedIconPath, newestAvailableVersionForThisPlatform, appStatusInStore FROM dynamicAppInfos WHERE appId=? LIMIT 1");
    QSqlQuery queryForDynamic(databaseToWorkOn);
    queryForDynamic.prepare(queryStringForDynamic);
    queryForDynamic.bindValue(0, _appId);

    ApplicationDescriptionModel::StaticDescriptionModel *staticDescription = new ApplicationDescriptionModel::StaticDescriptionModel;
    ApplicationDescriptionModel::DynamicDescriptionModel *dynamicDescription = new ApplicationDescriptionModel::DynamicDescriptionModel;

    bool isDynamicRequestSuccess = false;
    if(!queryForDynamic.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForDynamic);
    }
    else if(queryForDynamic.next()){
        dynamicDescription->setAppId(queryForDynamic.value(0).toString());
        dynamicDescription->setAppName(queryForDynamic.value(1).toString());
        dynamicDescription->setInstalledRootPath(queryForDynamic.value(2).toString());
        dynamicDescription->setAppState((AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)queryForDynamic.value(3).toInt());
        dynamicDescription->setSupportedPlatformFlags(queryForDynamic.value(4).toString());

        dynamicDescription->setIsHidden(queryForDynamic.value(5).toBool());
        dynamicDescription->setIsStartWhenClientStarts(queryForDynamic.value(6).toBool());
        dynamicDescription->setRunCount(queryForDynamic.value(7).toInt());
        dynamicDescription->setInstalledVersion(queryForDynamic.value(8).toString());
        dynamicDescription->setGrabbedIconPath(queryForDynamic.value(9).toString());
        dynamicDescription->setNewestAvailableVersionForThisPlatform(queryForDynamic.value(10).toString());
        //
        dynamicDescription->setAppStatusInStore((AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum)queryForDynamic.value(11).toInt());

        isDynamicRequestSuccess = true;
    }
    else {
        DLog("No results.");
    }

    if(isDynamicRequestSuccess) {
        // get the static part as well
        static QString queryStringForStatic("SELECT appId, appName, executablePath, executableWorkingDirectory, svgIconPath, largeIconPath, mediumIconPath, smallIconPath, licenceFilePath FROM staticAppInfos WHERE appId=? LIMIT 1");
        QSqlQuery queryForStatic(databaseToWorkOn);
        queryForStatic.prepare(queryStringForStatic);
        queryForStatic.bindValue(0, _appId);

        if(!queryForStatic.exec()) {
            DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForStatic);
        }
        else if(queryForStatic.next()){
            staticDescription->_setAppId(queryForStatic.value(0).toString());
            staticDescription->_setAppName(queryForStatic.value(1).toString());
            staticDescription->setExecutablePath(queryForStatic.value(2).toString());
            staticDescription->setExecutableWorkingDirectory(queryForStatic.value(3).toString());
            staticDescription->setSvgIconPath(queryForStatic.value(4).toString());

            staticDescription->setLargeIconPath(queryForStatic.value(5).toString());
            staticDescription->setMediumIconPath(queryForStatic.value(6).toString());
            staticDescription->setSmallIconPath(queryForStatic.value(7).toString());
            staticDescription->setLicenceFilePath(queryForStatic.value(8).toString());

            // finished
            this->_appDescriptionModel.setDynamicDescription(dynamicDescription);
            this->_appDescriptionModel.setStaticDescription(staticDescription);

            return true;
        }
        else {
            DLog("No results.");
        }
    }

    delete dynamicDescription;
    delete staticDescription;

    return false;
}
