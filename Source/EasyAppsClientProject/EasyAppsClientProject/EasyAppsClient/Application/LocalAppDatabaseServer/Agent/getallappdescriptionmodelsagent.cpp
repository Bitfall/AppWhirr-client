#include "getallappdescriptionmodelsagent.h"

#include <QVariant>
#include <QSqlQuery>

#include "Common/Helpers/databasehelper.h"

GetAllAppDescriptionModelsAgent::GetAllAppDescriptionModelsAgent(QString operationId) :
    DatabaseWorkAgentInterface(operationId)
{}

QString GetAllAppDescriptionModelsAgent::getAgentTypeId() const {
    static QString typeId("GetAllAppDescriptionModelsAgent");
    return typeId;
}

QList<ApplicationDescriptionModel> GetAllAppDescriptionModelsAgent::getResult() {
    return this->_result;
}

QStringList GetAllAppDescriptionModelsAgent::getRepairableTableNames()
{
    // repair these
    return QStringList() << "dynamicAppInfos" << "staticAppInfos";
}

bool GetAllAppDescriptionModelsAgent::doYourWork(QSqlDatabase databaseToWorkOn)
{
    static QString queryStringForDynamic("SELECT appId, appName, installedRootPath, appStateCode, supportedPlatformFlags, isHidden, isStartWhenClientStarts, runCount, installedVersion, grabbedIconPath, newestAvailableVersionForThisPlatform, appStatusInStore FROM dynamicAppInfos ORDER BY appId");
    // get the static part as well
    QSqlQuery queryForDynamic(databaseToWorkOn);
    queryForDynamic.prepare(queryStringForDynamic);

    static QString queryStringForStatic("SELECT appId, appName, executablePath, executableWorkingDirectory, svgIconPath, largeIconPath, mediumIconPath, smallIconPath, licenceFilePath FROM staticAppInfos ORDER BY appId");
    QSqlQuery queryForStatic(databaseToWorkOn);
    queryForStatic.prepare(queryStringForStatic);

    if(!queryForDynamic.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForDynamic);
        return false;
    }

    if(!queryForStatic.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForStatic);
        return false;
    }

    bool isSuccess = true;
    while(queryForDynamic.next() && isSuccess)
    {
        // consistency checks
        if(!queryForStatic.next()) {
            WLog("Database table inconsistency - No more static infos");
            isSuccess = false;
        }

        if(queryForStatic.value(0).toString() != queryForDynamic.value(0).toString()) {
            WLog("Database table inconsistency - given appIds does not match");
            isSuccess = false;
        }

        if(isSuccess) {
            // static
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

            // dynamic
            ApplicationDescriptionModel::DynamicDescriptionModel *dynamicDescription = new ApplicationDescriptionModel::DynamicDescriptionModel;
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

            ApplicationDescriptionModel appDescrModel;
            appDescrModel.setStaticDescription(staticDescription);
            appDescrModel.setDynamicDescription(dynamicDescription);

            this->_result.append(appDescrModel);
        }
    }

    return isSuccess;
}
