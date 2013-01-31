#ifndef UPDATEAPPISSTARTWHENCLIENTSTARTSAGENT_H
#define UPDATEAPPISSTARTWHENCLIENTSTARTSAGENT_H

#include "Common/Database/databaseworkagentinterface.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"

class UpdateAppIsStartWhenClientStartsAgent : public DatabaseWorkAgentInterface
{
public:
    explicit UpdateAppIsStartWhenClientStartsAgent(QString appId, bool newValue, QString operationId);

    bool doYourWork(QSqlDatabase databaseToWorkOn);
    QString getAgentTypeId() const;

    bool getInput() const;
    QString getAppId() const;

    QStringList getRepairableTableNames();

private:
    QString _appId;
    bool _newValue;
};

#endif // UPDATEAPPISSTARTWHENCLIENTSTARTSAGENT_H
