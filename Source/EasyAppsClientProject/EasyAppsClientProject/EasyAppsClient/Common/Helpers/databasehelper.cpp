#include "databasehelper.h"

#include <QSqlQuery>
#include <QVariant>
#include <QStringList>

DatabaseHelper::DatabaseHelper()
{
}


DatabaseTableSchemeDescriptor DatabaseHelper::getDatabaseSchemeDescriptionForTable(QString tableName, QSqlDatabase db)
{
    DatabaseTableSchemeDescriptor databaseSchemeDescr;

    // get dynamic infos
    static QString queryString = QString("PRAGMA TABLE_INFO(%1)");

    QSqlQuery query(db);
    query.prepare( queryString.arg(tableName) );

    if(!query.exec()) {
        logQueryExecutionFailedWithLastErrorForQuery(query);
    }
    else {
        while (query.next())
        {
            // get column descriptions
            int cid = query.value(0).toInt();
            QString name = query.value(1).toString();
            QString type = query.value(2).toString();
            // add it to the scheme-description
            databaseSchemeDescr.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(cid, name, type));

#ifdef ENABLE_DATABASE_OPERATION_LOGGING
            DLog("Database scheme item: ") << cid << name << type;
#endif
        }
    }

    return databaseSchemeDescr;
}

// --------------------------------
// --- Utility query operations ---

QString DatabaseHelper::createSingleColumnSelectQueryString(QString tableName, QString valueColumnName, QString keyColumnName, QString keyFilterText, int limitResultToNumber)
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

bool DatabaseHelper::executeThisQuery(QString queryString, QSqlDatabase db)
{
    bool isSucceeded = true;

    QSqlQuery queryForDyn(db);
    queryForDyn.prepare(queryString);
    if(!queryForDyn.exec()) {
        logQueryExecutionFailedWithLastErrorForQuery(queryForDyn);
        isSucceeded = false;
    }

    return isSucceeded;
}

bool DatabaseHelper::dropThisTableAndCreateNew(QString tableName, DatabaseTableSchemeDescriptor tableSchemeDescription, bool isFailIfDropFails, QSqlDatabase db)
{
    // drop original table
    QString dropOriginalTableQueryString = QString("DROP TABLE %1").arg(tableName);
    if(!executeThisQuery(dropOriginalTableQueryString, db)) {
        WLog("Cannot drop the original table.");

        if(isFailIfDropFails) {
            return false;
        }
    }

    // create the new, required (corrected) table
    QString createRequiredTableQueryString = QString("CREATE TABLE %1 (%2)").arg(tableName).arg(tableSchemeDescription.getTableColumnNameAndTypeListString());
#ifdef ENABLE_DATABASE_OPERATION_LOGGING
    DLog("Create required table query: ") << createRequiredTableQueryString;
#endif
    if(!DatabaseHelper::executeThisQuery(createRequiredTableQueryString, db)) {
        WLog("Cannot create required table.");
        return false;
    }

    // nothing more to do
    return true;
}


// ---------------------------
// --- Transaction helpers ---

bool DatabaseHelper::executeInTransaction(QStringList queriesToExecuteInTransaction, QSqlDatabase db)
{
    bool isSucceeded = true;

    // start transaction
    if(!db.transaction()) {
        DatabaseHelper::__logQueryExecutionFailedWithLastErrorForQuery(db, "transaction()");
        isSucceeded = false;
    }

    if(isSucceeded) {
        // do the queries in the transaction
        for(int i = 0; i < queriesToExecuteInTransaction.size() && isSucceeded; i++) {
            QSqlQuery queryForCurrInTransaction(db);
            queryForCurrInTransaction.prepare(queriesToExecuteInTransaction[i]);
            if(!queryForCurrInTransaction.exec()) {
                WLog("Query faild in transaction: ") << queriesToExecuteInTransaction[i];
                logQueryExecutionFailedWithLastErrorForQuery(queryForCurrInTransaction);
                isSucceeded = false;
            }
        }
    }

    if(isSucceeded) {
        // everything is fine - commit the transaction
        if(!db.commit()) {
            DatabaseHelper::__logQueryExecutionFailedWithLastErrorForQuery(db, "commit()");
            isSucceeded = false;
        }
    }
    else {
        WLog("Rollback");
        if(!db.rollback()) {
            DatabaseHelper::__logQueryExecutionFailedWithLastErrorForQuery(db, "rollback()");
        }
    }

    return isSucceeded;
}

// -----------------------
// --- Logging methods ---

void DatabaseHelper::__logQueryExecutionFailedWithLastErrorForQuery(QSqlDatabase db, QString queryString)
{
    WLog("Query execution failed with error: ") << db.lastError().text() << " [for query: " << queryString << "]";
}

void DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(QSqlQuery query)
{
    WLog("Query execution failed with error: ") << query.lastError().text() << " [for query: " << query.lastQuery() << "]";
}
