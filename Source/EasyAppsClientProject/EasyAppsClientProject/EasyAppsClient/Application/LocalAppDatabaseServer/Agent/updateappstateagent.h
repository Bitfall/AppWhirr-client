#ifndef UPDATEAPPSTATEAGENT_H
#define UPDATEAPPSTATEAGENT_H

#include "Common/Database/databaseworkagentinterface.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"
#include "../addorupdateconditionflags.h"

class UpdateAppStateAgent : public DatabaseWorkAgentInterface
{
public:
    //
    // for updateStateConditionFlags check the AppWhirr::UpdateStateConditionFlags enum
    explicit UpdateAppStateAgent(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState, int updateStateConditionFlags, QString operationId);

    bool doYourWork(QSqlDatabase databaseToWorkOn);
    QString getAgentTypeId() const;

    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum getInput() const;
    QString getAppId() const;

    QStringList getRepairableTableNames();

private:
    QString _appId;
    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum _newAppState;
    int _updateStateConditionFlags;
};

#endif // UPDATEAPPSTATEAGENT_H
