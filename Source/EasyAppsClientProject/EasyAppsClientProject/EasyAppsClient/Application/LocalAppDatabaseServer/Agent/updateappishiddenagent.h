#ifndef UPDATEAPPISHIDDENAGENT_H
#define UPDATEAPPISHIDDENAGENT_H

#include "Common/Database/databaseworkagentinterface.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"

class UpdateAppIsHiddenAgent : public DatabaseWorkAgentInterface
{
public:
    explicit UpdateAppIsHiddenAgent(QString appId, bool newValue, QString operationId);

    bool doYourWork(QSqlDatabase databaseToWorkOn);
    QString getAgentTypeId() const;

    bool getInput() const;
    QString getAppId() const;

    QStringList getRepairableTableNames();

private:
    QString _appId;
    bool _newValue;
};

#endif // UPDATEAPPISHIDDENAGENT_H
