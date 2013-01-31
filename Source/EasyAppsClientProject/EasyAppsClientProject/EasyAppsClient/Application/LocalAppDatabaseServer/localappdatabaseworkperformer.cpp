#include "localappdatabaseworkperformer.h"

// sql
#include <QSqlQuery>

// utility
#include "Common/Helpers/pathhelper.h"
#include "Application/appspecificpathhelper.h"
#include "Common/Helpers/databasehelper.h"

LocalAppDatabaseWorkPerformer::LocalAppDatabaseWorkPerformer(QString databasePath) :
    DatabaseWorkPerformerBase("LocalAppDatabaseWorkPerformer", databasePath, 0)
{

    //
    // dynamic infos

    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "appId", "TEXT UNIQUE NOT NULL"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "appName", "TEXT"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "installedRootPath", "TEXT"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "appStateCode", "NUMERIC DEFAULT 0"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "supportedPlatformFlags", "TEXT"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "isHidden", "NUMERIC DEFAULT 0"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "isStartWhenClientStarts", "NUMERIC DEFAULT 0"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "runCount", "NUMERIC DEFAULT 0"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "installedVersion", "TEXT"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "grabbedIconPath", "TEXT"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "newestAvailableVersionForThisPlatform", "TEXT"));
    this->_dynamicAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "appStatusInStore", "NUMERIC DEFAULT 0"));

    //
    // static infos

    this->_staticAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "appId", "TEXT UNIQUE NOT NULL"));
    this->_staticAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "appName", "TEXT"));
    this->_staticAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "executablePath", "TEXT"));
    this->_staticAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "executableWorkingDirectory", "TEXT"));
    this->_staticAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "svgIconPath", "TEXT"));
    this->_staticAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "largeIconPath", "TEXT"));
    this->_staticAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "mediumIconPath", "TEXT"));
    this->_staticAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "smallIconPath", "TEXT"));
    this->_staticAppInfoTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "licenceFilePath", "TEXT"));
}

// ------------------------
// --- database related ---

bool LocalAppDatabaseWorkPerformer::fillDatabaseWithBasicData(QSqlDatabase databaseToFill)
{
    //
    // dynamic infos

//    QString queryStringForCreateDynamicAppInfos("CREATE TABLE dynamicAppInfos (appId TEXT UNIQUE NOT NULL, appName TEXT, installedRootPath TEXT, appStateCode NUMERIC DEFAULT 0, supportedPlatformFlags TEXT, isHidden NUMERIC DEFAULT 0, isStartWhenClientStarts NUMERIC DEFAULT 0, runCount NUMERIC DEFAULT 0, installedVersion TEXT, grabbedIconPath TEXT, newestAvailableVersionForThisPlatform TEXT)");

    QString queryStringForCreateDynamicAppInfos = QString("CREATE TABLE dynamicAppInfos (%1)").arg(this->_dynamicAppInfoTableScheme.getTableColumnNameAndTypeListString());

    QSqlQuery query(databaseToFill);
    query.prepare(queryStringForCreateDynamicAppInfos);
    if(!query.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
        return false;
    }


    //
    // static infos

//    static QString queryStringForCreateStaticAppInfos("CREATE TABLE staticAppInfos (appId TEXT UNIQUE NOT NULL, appName TEXT, executablePath TEXT, executableWorkingDirectory TEXT, svgIconPath TEXT, largeIconPath TEXT, mediumIconPath TEXT, smallIconPath TEXT, licenceFilePath TEXT)");

    QString queryStringForCreateStaticAppInfos = QString("CREATE TABLE staticAppInfos (%1)").arg(this->_staticAppInfoTableScheme.getTableColumnNameAndTypeListString());

    QSqlQuery queryForStaticAppInfoCreation(databaseToFill);
    queryForStaticAppInfoCreation.prepare(queryStringForCreateStaticAppInfos);
    if(!queryForStaticAppInfoCreation.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(queryForStaticAppInfoCreation);
        return false;
    }

    return true;
}

// ------------------------------
// --- self-repairing related ---

DatabaseTableSchemeDescriptor LocalAppDatabaseWorkPerformer::_getRequiredSchemeDescriptionForTable(QString tableName)
{
    DLog("Scheme description requested for table: ") << tableName;

    if(tableName == "dynamicAppInfos") {
        return this->_dynamicAppInfoTableScheme;
    }
    else if(tableName == "staticAppInfos") {
        return this->_staticAppInfoTableScheme;
    }

    WLog("Invalid table name - empty description returned.");
    return DatabaseTableSchemeDescriptor();
}
