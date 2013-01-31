#include "getappstateagent.h"

#include <QVariant>
#include <QSqlQuery>

// utility
#include "Common/Helpers/databasehelper.h"

GetAppStateAgent::GetAppStateAgent(QString appId, QString operationId) :
    DatabaseWorkAgentInterface(operationId),
    _appId(appId)
{}

QString GetAppStateAgent::getAgentTypeId() const {
    static QString typeId("GetAppStateAgent");
    return typeId;
}

AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum GetAppStateAgent::getResult() const {
    return this->_retultAppState;
}

QString GetAppStateAgent::getAppId() const {
    return this->_appId;
}

QStringList GetAppStateAgent::getRepairableTableNames()
{
    // it's a very basic query, don't repair anything
    return QStringList();
}


bool GetAppStateAgent::doYourWork(QSqlDatabase databaseToWorkOn)
{
    static QString queryString("SELECT appStateCode FROM dynamicAppInfos WHERE appId=? LIMIT 1");
    QSqlQuery query(databaseToWorkOn);
    query.prepare(queryString);
    query.bindValue(0, _appId);

    if(!query.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
    }
    else if(query.next()){
        this->_retultAppState = ((AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)query.value(0).toInt());
        return true;
    }
    else {
        DLog("No results");
    }
    return false;
}
