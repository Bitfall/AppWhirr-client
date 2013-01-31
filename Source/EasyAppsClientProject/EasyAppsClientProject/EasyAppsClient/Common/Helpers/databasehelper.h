#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QSqlError>
#include <QSqlDatabase>

#include "../Database/databasetableschemedescriptor.h"

class DatabaseHelper
{
private:
    DatabaseHelper();

public:
    static DatabaseTableSchemeDescriptor getDatabaseSchemeDescriptionForTable(QString tableName, QSqlDatabase db);

    // -------------------------------------
    // --- Query creation helper methods ---

    //
    // tableName has to be specified
    // valueColumnName has to be specified
    static QString createSingleColumnSelectQueryString(QString tableName, QString valueColumnName, QString keyColumnName = QString(), QString keyFilterText = QString(), int limitResultToNumber = -1);

    // --------------------------------
    // --- Utility query operations ---
    static bool executeThisQuery(QString queryString, QSqlDatabase db);

    //
    // if isFailIfDropFails is true then it will stop and return false if the given tableName table cannot be dropped
    //  if it's false then it will ignore if the table cannot be dropped (probably it's not yet exists) and will try to create the new table and will fail only if the new table cannot be created
    static bool dropThisTableAndCreateNew(QString tableName, DatabaseTableSchemeDescriptor tableSchemeDescription, bool isFailIfDropFails, QSqlDatabase db);

    // ---------------------------
    // --- Transaction helpers ---
    static bool executeInTransaction(QStringList queriesToExecuteInTransaction, QSqlDatabase db);

    // -----------------------
    // --- Logging methods ---

    //
    // use this only if you cannot use the one without "__" - logQueryExecutionFailedWithLastErrorForQuery()
    //  this one probably won't log some important parts of the error
    static void __logQueryExecutionFailedWithLastErrorForQuery(QSqlDatabase db, QString queryString);
    static void logQueryExecutionFailedWithLastErrorForQuery(QSqlQuery query);
};

#endif // DATABASEHELPER_H
