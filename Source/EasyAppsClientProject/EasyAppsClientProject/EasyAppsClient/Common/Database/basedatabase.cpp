#include "basedatabase.h"
#include <QSqlError>
#include <QString>
#include <QSqlQuery>
#include <QDir>
#include <QVariant>

#include <QThread>

BaseDatabase::BaseDatabase(const QString& baseConnectionName, QObject *parent) :
    QObject(parent),
    _baseConnectionName(baseConnectionName)
{
}

BaseDatabase::~BaseDatabase() {
//    It's a warning causing the destructor:)
//    Qt forum sais it cannot be disabled with connection closing in a function:)
//    QSqlDatabasePrivate::removeDatabase: connection 'SortingDatabaseManager' is
//    still in use, all queries will cease to work

//    db.close();
//    this->closeDatabase();
//    QSqlDatabase::removeDatabase(finalconnectionname);

//    this->closeDatabaseConnection();
}

QString BaseDatabase::getFinalConnectionName(QString baseConnectionName)
{
    // add the operation-id and the current thread('s address)
    return QString("%1-%2").arg(baseConnectionName).arg((long)QThread::currentThread());
}

QSqlDatabase BaseDatabase::openDatabaseConnectionAndReturnDatabase()
{
    bool isSucceeded = true;

    QSqlDatabase db;

    QString finalConnectionName = this->getFinalConnectionName(this->_baseConnectionName);

#ifdef ENABLE_DATABASE_OPERATION_LOGGING
    DLog(" -> Try to open database: ") << finalConnectionName;
#endif

    // Reuse database connection if able
    //  DON'T REUSE - this makes it non threadsafe!
    //  A connection can be safely used in only 1 thread!
//    if (QSqlDatabase::connectionNames().contains(finalConnectionName)) {
//        // Opens it automatically if connection is closed
//        db = QSqlDatabase::database(finalConnectionName);
//    } else {


        // Or create a new one, if none found with the specified name
        db = QSqlDatabase::addDatabase("QSQLITE", finalConnectionName);

        QString path(getDatabasePath());

        bool isAlreadyExists = false;
        if(QDir().exists(path)) {
            isAlreadyExists = true;
        }
        else {
            isAlreadyExists = this->copyDatabaseFromResourceToPath(path);
        }
        db.setDatabaseName(path);

        // Open database
        if(!db.open()) {
//            isSucceeded = false;
            WLog("Cannot open database: ") << path;
            return db; // cannot open the database
        } else {
#ifdef ENABLE_DATABASE_OPERATION_LOGGING
            DLog("Database opened: ") << path;
#endif
        }

        if(!isAlreadyExists)
        {
            isSucceeded = this->fillDatabaseWithBasicData(db);
        }
//    }

    return db;
}


void BaseDatabase::closeDatabaseConnection()
{
    QString finalConnectionName = this->getFinalConnectionName(this->_baseConnectionName);

#ifdef ENABLE_DATABASE_OPERATION_LOGGING
    DLog(" -> close database: ") << finalConnectionName;
#endif
//    db.close();

//    db.removeDatabase(finalConnectionName);
    QSqlDatabase::removeDatabase(finalConnectionName);
}

bool BaseDatabase::_completelyDeleteDatabase()
{
    // Close database
    this->closeDatabaseConnection();

    QString path(getDatabasePath());

    return QFile::remove(path);
}

//QSqlDatabase BaseDatabase::getDatabase() {
//    return db;
//}
