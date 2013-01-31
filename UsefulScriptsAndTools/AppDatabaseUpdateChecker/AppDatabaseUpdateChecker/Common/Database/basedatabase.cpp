#include "basedatabase.h"
#include <QSqlError>
#include <QString>
#include <QSqlQuery>
#include <QDir>
#include <QVariant>

//#include "Common/PathManager/pathmanager.h"

#include <QDebug>

BaseDatabase::BaseDatabase(const QString& connectionName, QObject *parent) :
    QObject(parent),
    dbConnectionName(connectionName)
{
}

BaseDatabase::~BaseDatabase() {
//    It's a warning causing the destructor:)
//    Qt forum sais it cannot be disabled with connection closing in a function:)
//    QSqlDatabasePrivate::removeDatabase: connection 'SortingDatabaseManager' is
//    still in use, all queries will cease to work

//    db.close();
//    this->closeDatabase();
//    QSqlDatabase::removeDatabase(dbConnectionName);
}

QSqlDatabase BaseDatabase::openDatabaseConnectionAndReturnDatabase()
{
    bool isSucceeded = true;

    QSqlDatabase db;

#ifdef ENABLE_DATABASE_OPERATION_LOGGING
    DLog(" -> Try to open database: ") << this->dbConnectionName;
#endif

    // Reuse database connection if able
    if (QSqlDatabase::connectionNames().contains(dbConnectionName)) {
        // Opens it automatically if connection is closed
        db = QSqlDatabase::database(dbConnectionName);
    } else {
        // Or create a new one, if none found with the specified name
        db = QSqlDatabase::addDatabase("QSQLITE", dbConnectionName);

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
    }

    return db;
}


void BaseDatabase::closeDatabaseConnection()
{
#ifdef ENABLE_DATABASE_OPERATION_LOGGING
    DLog(" -> close database: ") << this->dbConnectionName;
#endif
//    db.close();

//    db.removeDatabase(this->dbConnectionName);
    QSqlDatabase::removeDatabase(this->dbConnectionName);
}

QString BaseDatabase::createSingleColumnSelectQueryString(QString tableName, QString valueColumnName, QString keyColumnName, QString keyFilterText, int limitResultToNumber)
{
    if(tableName.isEmpty() || valueColumnName.isEmpty()) {
        qDebug("invalid parameters");
        return QString(); // empty and null
    }

    QString queryString("SELECT %1 FROM %2");
    queryString = queryString.arg(valueColumnName).arg(tableName);
    if(!keyColumnName.isEmpty() && !keyFilterText.isEmpty()) {
        queryString = QString("%1 %2").arg(queryString).arg(QString(" WHERE %1='%2'").arg(keyColumnName).arg(keyFilterText));
    }

    if(limitResultToNumber > 0) {
        queryString = QString("%1 %2").arg(queryString).arg(QString(" LIMIT %1").arg(limitResultToNumber));
    }

    return queryString;
}

QSqlError BaseDatabase::getLastError(QSqlDatabase db)
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
}

BaseDatabase::DatabaseSchemeDescription BaseDatabase::_getCurrentDatabaseSchemeDescriptionForTable(QString tableName)
{
    BaseDatabase::DatabaseSchemeDescription databaseSchemeDescr;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();

        // get dynamic infos
        QString queryString = QString("PRAGMA TABLE_INFO(%1)").arg(tableName);

        QSqlQuery query(db);
        query.prepare(queryString);
        if(!query.exec()) {
            this->logQueryExecutionFailedWithLastError(db);
        }
        else {
            while (query.next())
            {
                // get column descriptions
                int cid = query.value(0).toInt();
                QString name = query.value(1).toString();
                QString type = query.value(2).toString();
                // add it to the scheme-description
                databaseSchemeDescr.addItem(DatabaseSchemeDescriptionItem(cid, name, type));

#ifdef ENABLE_DATABASE_OPERATION_LOGGING
                DLog("Database scheme item: ") << cid << name << type;
#endif
            }
        }
    }

    this->closeDatabaseConnection();

    return databaseSchemeDescr;
}

void BaseDatabase::logQueryExecutionFailedWithLastError(QSqlDatabase db)
{
    WLog("Query execution failed: ") << this->getLastError(db);
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
