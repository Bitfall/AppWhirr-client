#ifndef GETAPPSTATEAGENT_H
#define GETAPPSTATEAGENT_H

#include "Common/Database/databaseworkagentinterface.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"

class GetAppStateAgent : public DatabaseWorkAgentInterface
{
public:
    explicit GetAppStateAgent(QString appId, QString operationId);

    bool doYourWork(QSqlDatabase databaseToWorkOn);
    QString getAgentTypeId() const;

    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum getResult() const;
    QString getAppId() const;

    QStringList getRepairableTableNames();

private:
    QString _appId;
    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum _retultAppState;
};

#endif // GETAPPSTATEAGENT_H
