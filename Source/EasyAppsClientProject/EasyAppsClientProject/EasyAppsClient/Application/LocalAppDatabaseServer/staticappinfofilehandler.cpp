#include "staticappinfofilehandler.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include "Common/Helpers/databasehelper.h"

StaticAppInfoFileHandler::StaticAppInfoFileHandler(QString appInfoDatabaseFilePath, QObject *parent) :
    BaseDatabase(appInfoDatabaseFilePath, parent),
  _appInfoDatabaseFilePath(appInfoDatabaseFilePath)
{
}

QString StaticAppInfoFileHandler::getDatabasePath() {
    return this->_appInfoDatabaseFilePath;
}

bool StaticAppInfoFileHandler::parseStaticAppInfoToApplicationDescriptionModel(ApplicationDescriptionModel &refAppDescriptionModel)
{
    bool isSucceeded = true;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();

        // get static infos
        static QString queryString("SELECT appId, appName, executablePath, executableWorkingDirectory, svgIconPath, largeIconPath, mediumIconPath, smallIconPath, licenceFilePath FROM staticAppInfo LIMIT 1");

        QSqlQuery query(db);
        query.prepare(queryString);
        if(!query.exec()) {
            DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
            isSucceeded = false;
        }
        else if (query.next()) {
            QString appId = query.value(0).toString();
            QString appName = query.value(1).toString();
            QString exePath = query.value(2).toString();
            QString exeWorkingDir = query.value(3).toString();
            QString svgIconPath = query.value(4).toString();
            QString largetIconPath = query.value(5).toString();
            QString mediumIconPath = query.value(6).toString();
            QString smallIconPath = query.value(7).toString();
            QString licenceFilePath = query.value(8).toString();

//            DLogS << appId << appName << exePath << svgIconPath;

            ApplicationDescriptionModel::StaticDescriptionModel *staticDescription = new ApplicationDescriptionModel::StaticDescriptionModel;
            staticDescription->_setAppId(appId);
            staticDescription->_setAppName(appName);
            staticDescription->setExecutablePath(exePath);
            staticDescription->setExecutableWorkingDirectory(exeWorkingDir);
            staticDescription->setSvgIconPath(svgIconPath);
            staticDescription->setLargeIconPath(largetIconPath);
            staticDescription->setMediumIconPath(mediumIconPath);
            staticDescription->setSmallIconPath(smallIconPath);
            staticDescription->setLicenceFilePath(licenceFilePath);

            refAppDescriptionModel.setStaticDescription(staticDescription);

//            DLogS << "Right after parse: " << _retstaticDescription->getAppId() << _retstaticDescription->getAppName() << _retstaticDescription->getExecutablePath() << _retAppDescriptionModel.getDynamicDescription()->getInstalledPath() << _retAppDescriptionModel.getDynamicDescription()->getIsStartWhenClientStarts();
        }
        else {
            WLog("No static infos returned by sql-select.");
            isSucceeded = false;
        }
    }

    this->closeDatabaseConnection();
    return isSucceeded;
}

bool StaticAppInfoFileHandler::updateStaticInfoByApplicationDescriptionModel(ApplicationDescriptionModel appDescriptionModel)
{
    bool isSucceeded = true;

    {
//        DLogS << "Before update: " << staticDescription->getAppId() << staticDescription->getAppName() << staticDescription->getExecutablePath() << appDescriptionModel.getDynamicDescription()->getInstalledRootPath() << appDescriptionModel.getDynamicDescription()->getIsStartWhenClientStarts();

        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();

        // do update on static
        static QString queryString("UPDATE OR REPLACE staticAppInfo SET appId=?, appName=?, executablePath=?, executableWorkingDirectory=?, svgIconPath=?, largeIconPath=?, mediumIconPath=?, smallIconPath=?, licenceFilePath=?");

//        DLog("Query string: ") << updateStaticInfosQueryString;

        QSqlQuery updateQueryForStatic(db);
        updateQueryForStatic.prepare(queryString);
        updateQueryForStatic.bindValue(0, appDescriptionModel.getStaticDescription_ReadOnly()->_getAppId());
        updateQueryForStatic.bindValue(1, appDescriptionModel.getStaticDescription_ReadOnly()->_getAppName());
        updateQueryForStatic.bindValue(2, appDescriptionModel.getStaticDescription_ReadOnly()->getExecutablePath());
        updateQueryForStatic.bindValue(3, appDescriptionModel.getStaticDescription_ReadOnly()->getExecutableWorkingDirectory());
        updateQueryForStatic.bindValue(4, appDescriptionModel.getStaticDescription_ReadOnly()->getSvgIconPath());
        updateQueryForStatic.bindValue(5, appDescriptionModel.getStaticDescription_ReadOnly()->getLargeIconPath());
        updateQueryForStatic.bindValue(6, appDescriptionModel.getStaticDescription_ReadOnly()->getMediumIconPath());
        updateQueryForStatic.bindValue(7, appDescriptionModel.getStaticDescription_ReadOnly()->getSmallIconPath());
        updateQueryForStatic.bindValue(8, appDescriptionModel.getStaticDescription_ReadOnly()->getLicenceFilePath());

        if(!updateQueryForStatic.exec()) {
            DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(updateQueryForStatic);
            isSucceeded = false;
        }
    }

    this->closeDatabaseConnection();
    return isSucceeded;
}
