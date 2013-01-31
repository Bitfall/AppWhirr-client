#include "updateappstateagent.h"

#include <QVariant>
#include <QSqlQuery>

// utility
#include "Common/Helpers/databasehelper.h"
#include "appdataqueryhelpers.h"

UpdateAppStateAgent::UpdateAppStateAgent(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState, int updateStateConditionFlags, QString operationId) :
    DatabaseWorkAgentInterface(operationId),
    _appId(appId),
    _newAppState(newAppState),
    _updateStateConditionFlags(updateStateConditionFlags)
{}

QString UpdateAppStateAgent::getAgentTypeId() const {
    static QString typeId("UpdateAppStateAgent");
    return typeId;
}

AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum UpdateAppStateAgent::getInput() const {
    return this->_newAppState;
}

QString UpdateAppStateAgent::getAppId() const {
    return this->_appId;
}

QStringList UpdateAppStateAgent::getRepairableTableNames()
{
    // don't repair anything
    return QStringList();
}

bool UpdateAppStateAgent::doYourWork(QSqlDatabase databaseToWorkOn)
{

    bool isUpdate = true;

    if(this->_updateStateConditionFlags != AppWhirr::UpdateStateCondition::USC_UpdateAnyway)
    {
        // state
        AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum currState;
        bool isCurrentStateAvailable = AppDataQueryHelpers::getAppStateByAppId(databaseToWorkOn, this->_appId, currState);

        if(isCurrentStateAvailable)
        {
            int diffState = ((int)this->_newAppState) - ((int)currState);

//            DLog("State diff and case: ") << diffState << this->_updateConditionFlags;

            if( (diffState > 0) && ( (this->_updateStateConditionFlags & AppWhirr::UpdateStateCondition::USC_UpdateIfNewStateHigher) == 0) ) {
                // if diffState > 0 -> the new state is higher than the current
                //  but flag is not set - don't update
                isUpdate = false;
            }
            else if( (diffState == 0) && ( (this->_updateStateConditionFlags & AppWhirr::UpdateStateCondition::USC_UpdateIfNewStateEqual) == 0) ) {
                // equal states
                //  but flag is not set - don't update
                isUpdate = false;
            }
            else if( (this->_updateStateConditionFlags & AppWhirr::UpdateStateCondition::USC_UpdateIfNewStateLower) == 0) {
                // the new state is lower than the current
                //  but flag is not set - don't update
                isUpdate = false;
            }
        }
        else {
            isUpdate = false;
        }
    }

    if(!isUpdate) {
        DLog("Don't update - based on update state conditions flag.");
        return false;
    }

    DLog("UpdateAppStateAgent") << this->_newAppState << this->_appId;
    static QString queryString("UPDATE dynamicAppInfos SET appStateCode=? WHERE appId=?");
    QSqlQuery query(databaseToWorkOn);
    query.prepare(queryString);
    // bind
    query.bindValue(0, QVariant(this->_newAppState));
    // where
    query.bindValue(1, _appId);

    if(!query.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
        return false;
    }
    return true;
}
