#ifndef ADDORUPDATEWHOLEAPPDESCRIPTION_H
#define ADDORUPDATEWHOLEAPPDESCRIPTION_H

#include "Common/Database/databaseworkagentinterface.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Application/LocalAppDatabaseServer/addorupdateconditionflags.h"

class AddOrUpdateWholeAppDescription : public DatabaseWorkAgentInterface
{
public:
    //
    // for updateAppDetailPartsConditionFlags check the AppWhirr::UpdateAppDetailPartsConditionFlags enum
    // for updateStateConditionFlags check the AppWhirr::UpdateStateConditionFlags enum
    explicit AddOrUpdateWholeAppDescription(ApplicationDescriptionModel appDescriptionModel, int updateAppDetailPartsConditionFlags, int updateStateConditionFlags, QString operationId);

    bool doYourWork(QSqlDatabase databaseToWorkOn);
    QString getAgentTypeId() const;

    //
    // returns the app-descr which was the input
    ApplicationDescriptionModel getInputAppDescriptionModel();
    //
    // returns the app-descr contains the updated result -> e.g.: if the isHidden property was ignored from the input then this will hold the old/current value
    // if the query fails it will contain what SHOULD BE the result (like the query would succeed)
    // if it was an insert not an update then it holds the same data as the input
    ApplicationDescriptionModel getUpdatedResultAppDescrptionModel();

    QStringList getRepairableTableNames();

private:
    bool doUpdate(QSqlDatabase databaseToWorkOn, ApplicationDescriptionModel currentAppDescription);
    bool doInsert(QSqlDatabase databaseToWorkOn);
    //
    // call this only for update!!
    // if isSkipQueries true then it won't execute any queries, it will only set the _updatedAppDescriptionModel's related properties
    bool doDynamicUpdateDependingOnUpdateConditionFlags(QSqlDatabase databaseToWorkOn, ApplicationDescriptionModel currentAppDescription, bool isSkipQueries);

private:
    ApplicationDescriptionModel _inputAppDescriptionModel;
    ApplicationDescriptionModel _updatedAppDescriptionModel;
    int _updateAppDetailPartsConditionFlags;
    int _updateStateConditionFlags;
};

#endif // ADDORUPDATEWHOLEAPPDESCRIPTION_H
