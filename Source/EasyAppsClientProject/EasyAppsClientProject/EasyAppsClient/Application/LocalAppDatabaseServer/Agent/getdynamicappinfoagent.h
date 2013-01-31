#ifndef GETDYNAMICAPPINFOAGENT_H
#define GETDYNAMICAPPINFOAGENT_H

#include "Common/Database/databaseworkagentinterface.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

class GetDynamicAppInfoAgent : public DatabaseWorkAgentInterface
{
public:
    explicit GetDynamicAppInfoAgent(QString appId, QString operationId);

    bool doYourWork(QSqlDatabase databaseToWorkOn);
    QString getAgentTypeId() const;

    ApplicationDescriptionModel getResult();

private:
    QString _appId;
    ApplicationDescriptionModel _appDescriptionModel;
};

#endif // GETDYNAMICAPPINFOAGENT_H
