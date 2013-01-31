#ifndef GETALLAPPDESCRIPTIONMODELSAGENT_H
#define GETALLAPPDESCRIPTIONMODELSAGENT_H


//
// !!! -> do it in transaction and rollback if 1 query fails
//

#include <QList>

#include "Common/Database/databaseworkagentinterface.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

class GetAllAppDescriptionModelsAgent : public DatabaseWorkAgentInterface
{
public:
    explicit GetAllAppDescriptionModelsAgent(QString operationId);

    bool doYourWork(QSqlDatabase databaseToWorkOn);
    QString getAgentTypeId() const;

    QList<ApplicationDescriptionModel> getResult();

    QStringList getRepairableTableNames();

private:
    QList<ApplicationDescriptionModel> _result;
};

#endif // GETALLAPPDESCRIPTIONMODELSAGENT_H
