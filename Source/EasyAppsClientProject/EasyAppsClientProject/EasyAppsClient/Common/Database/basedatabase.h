#ifndef BASEDATABASE_H
#define BASEDATABASE_H

#include "databasetableschemedescriptor.h"

#include <QObject>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
class QSqlError;
class QString;
QT_END_NAMESPACE

//
// one of the virtual database-creation methods have to be implemented (if none of them implemented openDatabase() will always return false)
//
// Limitations:
//  * Currently only 1 background thread can access a database at a given time! -> check the Thread safety information section
//
// Usage notes:
//      * [!] For usage informations check the Qt doc/help of the QSqlDatabase class, and use it that way. This base class has only some helper methods (basic query creation, logging, ...) and unified database and connection opening/closing methods, but the QSqlDatabase and the query execution is done "the Qt way" (you can see basic usage examples in executeInTransaction() or in executeThisQuery() methods)
//      * [!] Also the recommended usage is that you create an object from the database-handler class when you want to perform operations in it then free it up immediatelly.
//      * Example: you need informations from the Xy database and you use XyDatabaseHandler class to do it.
//      ** Create a new object from XyDatabaseHandler when you want to perform the operation
//      ** Perform the operation (e.g. get infos / update infos)
//      ** And after this delete (free up) the database handler object
//
// Thread safety informations:
//      * Summary of thread-safe usage of Qt and of SQL databases: http://doc.qt.nokia.com/latest/threads-modules.html
//      * This also worth to check, detailed Qt multithread sql handling tutorial: http://www.linuxjournal.com/article/9602
//      * "A connection can only be used from within the thread that created it. Moving connections between threads or creating queries from a different thread is not supported."
//      * To use an SQLite database thread-safe you have to define different connection-names in every thread! addDatabase will remove the previous connection for the same connection name!
//      * The current implementation supports 2 type of connections: main-thread and background-thread. You have to specify from which type of threads the open will be called -> it will automatically post-fix the connection name to provide different connection names for the main-thread and for background-thread
//      * Also specify a databaseOperationId which is an identifier for what you want to do with the database
//      ** These thread-modes and operation-ids provide the capability to open the same database from different threads.
//      ** [!] This way the only thing what you CANNOT DO is opening a database with the same thread-type and same operation-id in different threads. [!] So don't do something like creating background threads for some parallel background operations and open a database from each of them with the same thread-type and with the same operation-id
//      * Also you have (it's a very strong suggestion to keep the system safe) to call openDatabaseConnectionAndReturnDatabase() before every (batch of) operation(s) you want to make (typically call this at the beginning of a method which will use the database) and you have to call closeDatabaseConnection() when you finished with the operations - this is required to achieve thread-safety
//      ** the only exception is the fillDatabaseWithBasicData method, which gets the open database as a parameter, and have to use it, and is have to NOT call either openDatabaseConnectionAndReturnDatabase() or closeDatabaseConnection()
//      * [!warning!] : you HAVE TO CLOSE the database even if you want to return earlier than the end of the query execution method (e.g. if there were some errors)! DON'T FORGET TO CLOSE THE DATABASE BEFORE YOU RETURN FROM A QUERY-EXECUTION METHOD!
//
// [!] Error-handling informations:
//      * After the database is opened successfully for the first time it will create the desired database at path specified by getDatabasePath() and it won't override it after this
//      * So the creation is a critical point, if it creates the file at path returned by getDatabasePath() but does something wrong (for example it creates an empty file) it won't recreate it anymore !
//      * For adding self-repairing capability use the SelfRepairingDatabase base class
//      * Or optionally you can delete the whole database with _completelyDeleteDatabase() but this is not advised (this will force to recreate the whole database next time and this is time consuming)
//
class BaseDatabase : public QObject
{
    Q_OBJECT
public:
    //
    // threadMode: specify from which type of thread the database will be used
    // databaseOperationId: this have to be a unique operation-id - can be anything but provide different IDs for every type of operation you want to perform - e.g.: if you do some grab-list synchronization then give an ID like "grab-list-sync"
    explicit BaseDatabase(const QString& baseConnectionName, QObject *parent = 0);
    virtual ~BaseDatabase();

    //
    // this will completely delete the database, you won't be able to restore it
    bool _completelyDeleteDatabase();

protected:

    // -----------------------
    // --- Implement these ---

    //
    // you have to implement this and return the database's path, where it will be stored
    virtual QString getDatabasePath() = 0;
    //
    // return whether it's succeeded
    virtual bool copyDatabaseFromResourceToPath(QString path) { return false; }
    //
    // return whether it's succeeded
    virtual bool fillDatabaseWithBasicData(QSqlDatabase databaseToFill) { return false; }

    // ------------------------

    QSqlDatabase openDatabaseConnectionAndReturnDatabase();
    void closeDatabaseConnection();

private:
    QString getFinalConnectionName(QString baseConnectionName);

private:
    const QString _baseConnectionName;
};

#endif // BASEDATABASE_H
