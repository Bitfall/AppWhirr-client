#ifndef DATABASEWORKPERFORMERBASE_H
#define DATABASEWORKPERFORMERBASE_H

#include "selfrepairingdatabase.h"

class DatabaseWorkAgentInterface;

/*! Use this work-performer in server classes to perform agent based asynchronous database handling

    -> run this in the server-thread to perform work on the database

     [!] The concept of the this class is a bit different comapred to BaseDatabase
      * This class will open the database then it will keep it open - because it is designed to be used in a dedicated server thread
      * After the database is open call getTheOpenWorkingDatabase() instead of the BaseDatabas' openDatabaseConnectionAndReturnDatabase()
      * So usually when the server starts emit a signal which is connected to openAndInitialize() slot, and when the server should shut down emit signal connected to closeAndShutDown()
      * Use the work-performer's initialized() and shutDownHappened() signals to indicate whether the server is running or it is shut down
*/
class DatabaseWorkPerformerBase : public SelfRepairingDatabase
{
    Q_OBJECT
public:
    DatabaseWorkPerformerBase(QString baseConnectionName, QString databasePath, QObject *parent = 0);

Q_SIGNALS: // server - worker communication signals
    void finishedWithWork(DatabaseWorkAgentInterface *workAgent, bool isSuccess);
    void initialized();
    void shutDownHappened();

public Q_SLOTS:
    void performThisWork(DatabaseWorkAgentInterface *workAgent);
    void openAndInitialize();
    void closeAndShutDown();

protected:
    //
    // call this to get the open working database - after openAndInitialize() called!!
//    QSqlDatabase getTheOpenWorkingDatabase();
    QString getDatabasePath();

private:
    QSqlDatabase _db;
    QString _databasePath;
};

#endif // DATABASEWORKPERFORMERBASE_H
