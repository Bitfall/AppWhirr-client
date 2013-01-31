#include "databaseworkperformerbase.h"

#include <QVariant>
#include <QSqlQuery>

#include "databaseworkagentinterface.h"

DatabaseWorkPerformerBase::DatabaseWorkPerformerBase(QString baseConnectionName, QString databasePath, QObject *parent) :
    SelfRepairingDatabase(baseConnectionName, parent),
    _databasePath(databasePath)
{
}

QString DatabaseWorkPerformerBase::getDatabasePath() {
    return this->_databasePath;
}

void DatabaseWorkPerformerBase::openAndInitialize() {
    this->_db = this->openDatabaseConnectionAndReturnDatabase();
    DLog("--open and initialized--");
    Q_EMIT initialized();
}

void DatabaseWorkPerformerBase::closeAndShutDown() {
    DLog("--close and shutdown--");
    this->_db.close();
    this->closeDatabaseConnection();

    Q_EMIT shutDownHappened();
}

void DatabaseWorkPerformerBase::performThisWork(DatabaseWorkAgentInterface *workAgent)
{
    bool isSuccess = workAgent->doYourWork(this->_db);
    if(!isSuccess) {
        DLog("Worker failed - try to repair.");
        // try to repair
        QStringList repairableTableNames = workAgent->getRepairableTableNames();
        int repairableTableCnt = repairableTableNames.size();
        if(repairableTableCnt > 0)
        {
            // repair the given tables
            bool isRepairSuccess = true;
            for(int i = 0; i < repairableTableCnt && isRepairSuccess; i++) {
                if(!this->_tryToRepairTheDatabaseStructureOfTable(repairableTableNames[i], this->_db)) {
                    isRepairSuccess = false;
                }
                // also mark them to skip at the next time
                this->_addSkipRepairForTable(repairableTableNames[i]);
            }

            if(isRepairSuccess) {
                DLog("Repaired successfully - retry work...");
                // retry
                isSuccess = workAgent->doYourWork(this->_db);
                if(isSuccess) {
                    DLog("Worker succeeded after repair!");
                }
                else {
                    DLog("Worker failed, again, even after repair.");
                }
            }
            else {
                DLog("Repair failed -> worker failed.");
                isSuccess = false;
            }
        }
        else {
            DLog("No repairable tables.");
            isSuccess = false;
        }
    }
    else {
        DLog("Worker Success.");
    }
    Q_EMIT finishedWithWork(workAgent, isSuccess);
}
