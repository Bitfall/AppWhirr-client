#ifndef DATABASEWORKAGENTINTERFACE_H
#define DATABASEWORKAGENTINTERFACE_H

#include <QString>
#include <QStringList>
#include <QSqlDatabase>

class DatabaseWorkAgentInterface
{
public:
    explicit DatabaseWorkAgentInterface(QString operationId) : _operationId(operationId) {}
    //
    // implement this and do the sql work on the provided databaseToWorkOn
    virtual bool doYourWork(QSqlDatabase databaseToWorkOn) = 0;
    //
    // agent class specific id
    virtual QString getAgentTypeId() const = 0;

    QString getOperationId() const { return _operationId; }

    //
    // give back the table-names you want to (self-)repair in case of failure
    //  -> this will be used by SelfRepairingDatabase based work-performers
    virtual QStringList getRepairableTableNames() = 0;

private:
    const QString _operationId;
};

#endif // DATABASEWORKAGENTINTERFACE_H
