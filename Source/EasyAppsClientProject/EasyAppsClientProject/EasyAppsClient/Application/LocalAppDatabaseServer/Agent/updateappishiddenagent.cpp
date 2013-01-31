#include "updateappishiddenagent.h"

#include <QVariant>
#include <QSqlQuery>

// utility
#include "Common/Helpers/databasehelper.h"

UpdateAppIsHiddenAgent::UpdateAppIsHiddenAgent(QString appId, bool newValue, QString operationId) :
    DatabaseWorkAgentInterface(operationId),
    _appId(appId),
    _newValue(newValue)
{}

QString UpdateAppIsHiddenAgent::getAgentTypeId() const {
    static QString typeId("UpdateAppIsHiddenAgent");
    return typeId;
}

bool UpdateAppIsHiddenAgent::getInput() const {
    return this->_newValue;
}

QString UpdateAppIsHiddenAgent::getAppId() const {
    return this->_appId;
}

QStringList UpdateAppIsHiddenAgent::getRepairableTableNames()
{
    // it's a very basic query, don't repair anything
    return QStringList();
}

bool UpdateAppIsHiddenAgent::doYourWork(QSqlDatabase databaseToWorkOn)
{
//    DLog("UpdateAppIsHiddenAgent") << this->_newValue << this->_appId;
    static QString queryString("UPDATE dynamicAppInfos SET isHidden=? WHERE appId=?");
    QSqlQuery query(databaseToWorkOn);
    query.prepare(queryString);
    // bind
    query.bindValue(0, QVariant(this->_newValue));
    // where
    query.bindValue(1, _appId);

    if(!query.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
        return false;
    }
    return true;
}
