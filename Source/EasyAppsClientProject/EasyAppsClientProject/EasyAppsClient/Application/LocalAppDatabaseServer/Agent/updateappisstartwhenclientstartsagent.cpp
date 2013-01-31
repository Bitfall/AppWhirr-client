#include "updateappisstartwhenclientstartsagent.h"

#include <QVariant>
#include <QSqlQuery>

// utility
#include "Common/Helpers/databasehelper.h"

UpdateAppIsStartWhenClientStartsAgent::UpdateAppIsStartWhenClientStartsAgent(QString appId, bool newValue, QString operationId) :
    DatabaseWorkAgentInterface(operationId),
    _appId(appId),
    _newValue(newValue)
{}

QString UpdateAppIsStartWhenClientStartsAgent::getAgentTypeId() const {
    static QString typeId("UpdateAppIsStartWhenClientStartsAgent");
    return typeId;
}

bool UpdateAppIsStartWhenClientStartsAgent::getInput() const {
    return this->_newValue;
}

QString UpdateAppIsStartWhenClientStartsAgent::getAppId() const {
    return this->_appId;
}

QStringList UpdateAppIsStartWhenClientStartsAgent::getRepairableTableNames()
{
    // it's a very basic query, don't repair anything
    return QStringList();
}

bool UpdateAppIsStartWhenClientStartsAgent::doYourWork(QSqlDatabase databaseToWorkOn)
{
//    DLog("UpdateAppIsStartWhenClientStartsAgent") << this->_newValue << this->_appId;
    static QString queryString("UPDATE dynamicAppInfos SET isStartWhenClientStarts=? WHERE appId=?");
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
