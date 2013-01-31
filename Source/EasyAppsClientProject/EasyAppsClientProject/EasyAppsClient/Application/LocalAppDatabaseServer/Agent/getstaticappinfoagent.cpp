#include "getstaticappinfoagent.h"

#include <QVariant>
#include <QSqlQuery>

#include "Common/Helpers/databasehelper.h"
#include "appdataqueryhelpers.h"

GetStaticAppInfoAgent::GetStaticAppInfoAgent(QString appId, QString operationId) :
    DatabaseWorkAgentInterface(operationId),
    _appId(appId)
{}

QString GetStaticAppInfoAgent::getAgentTypeId() const {
    static QString typeId("GetStaticAppInfoAgent");
    return typeId;
}

ApplicationDescriptionModel GetStaticAppInfoAgent::getResult() {
    return this->_appDescriptionModel;
}

bool GetStaticAppInfoAgent::doYourWork(QSqlDatabase databaseToWorkOn)
{
    return AppDataQueryHelpers::getStaticAppDataByAppId(databaseToWorkOn, this->_appId, this->_appDescriptionModel);
}
