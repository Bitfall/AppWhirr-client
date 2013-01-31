#include "getdynamicappinfoagent.h"

#include <QVariant>
#include <QSqlQuery>

// utility
#include "Common/Helpers/databasehelper.h"

#include "appdataqueryhelpers.h"

GetDynamicAppInfoAgent::GetDynamicAppInfoAgent(QString appId, QString operationId) :
    DatabaseWorkAgentInterface(operationId),
    _appId(appId)
{}

QString GetDynamicAppInfoAgent::getAgentTypeId() const {
    static QString typeId("GetDynamicAppInfoAgent");
    return typeId;
}

ApplicationDescriptionModel GetDynamicAppInfoAgent::getResult() {
    return this->_appDescriptionModel;
}

bool GetDynamicAppInfoAgent::doYourWork(QSqlDatabase databaseToWorkOn)
{
    return AppDataQueryHelpers::getDynamicAppDataByAppId(databaseToWorkOn, this->_appId, this->_appDescriptionModel);
}
