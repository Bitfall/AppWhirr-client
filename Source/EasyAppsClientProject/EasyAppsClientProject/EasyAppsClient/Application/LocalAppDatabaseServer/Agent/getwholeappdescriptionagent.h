#ifndef GETWHOLEAPPDESCRIPTIONAGENT_H
#define GETWHOLEAPPDESCRIPTIONAGENT_H

#include "Common/Database/databaseworkagentinterface.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

class GetWholeAppDescriptionAgent : public DatabaseWorkAgentInterface
{
public:
    explicit GetWholeAppDescriptionAgent(QString appId, QString operationId);

    bool doYourWork(QSqlDatabase databaseToWorkOn);
    QString getAgentTypeId() const;

    QString getInputAppId() const;
    ApplicationDescriptionModel getResult();

    QStringList getRepairableTableNames();

private:
    QString _appId;
    ApplicationDescriptionModel _appDescriptionModel;
};

#endif // GETWHOLEAPPDESCRIPTIONAGENT_H
