#ifndef GETSTATICAPPINFOAGENT_H
#define GETSTATICAPPINFOAGENT_H

#include "Common/Database/databaseworkagentinterface.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

class GetStaticAppInfoAgent : public DatabaseWorkAgentInterface
{
public:
    explicit GetStaticAppInfoAgent(QString appId, QString operationId);

    bool doYourWork(QSqlDatabase databaseToWorkOn);
    QString getAgentTypeId() const;

    ApplicationDescriptionModel getResult();

private:
    QString _appId;
    ApplicationDescriptionModel _appDescriptionModel;
};

#endif // GETSTATICAPPINFOAGENT_H
