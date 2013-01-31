#include "sortingdatabasemanager.h"

#include <QSqlQuery>
#include <QVariant>

// utility
#include "Common/Helpers/databasehelper.h"

SortingDatabaseManager::SortingDatabaseManager(QString databasePath, QObject *parent) :
    BaseDatabase("SortingDatabaseManager", parent),
    _databasePath(databasePath)
{
}

QString SortingDatabaseManager::getDatabasePath()
{
    return _databasePath;
}

bool SortingDatabaseManager::fillDatabaseWithBasicData(QSqlDatabase databaseToFill)
{
    static QString queryString("CREATE TABLE sorting_info (filter_part TEXT UNIQUE, selected_value TEXT)");
    QSqlQuery query(databaseToFill);
    query.prepare(queryString);
    if(!query.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
        return false;
    }

    return true;
}

bool SortingDatabaseManager::setValueForKey(QString key, QString value)
{
    bool isSucceeded = true;

    {
        static QString queryStringWithPlaceholders("INSERT OR REPLACE INTO sorting_info (filter_part, selected_value) VALUES (?, ?)");
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
        QSqlQuery query(db);
        query.prepare(queryStringWithPlaceholders);
        query.bindValue(0, key);
        query.bindValue(1, value);

        if(!query.exec()) {
            DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
            isSucceeded = false;
        }
    }

    this->closeDatabaseConnection();
    return isSucceeded;
}

bool SortingDatabaseManager::getValueForKey(QString key, QString &outValue)
{
    bool isSucceeded = false;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
        static QString queryString("SELECT selected_value FROM sorting_info WHERE filter_part=? LIMIT 1");

        QSqlQuery query(db);
        query.prepare(queryString);
        query.bindValue(0, key);

        if(!query.exec()) {
            DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
        }
        else if (query.next()) {
            outValue = query.value(0).toString();
            isSucceeded = true;
        }
    }

    this->closeDatabaseConnection();
    return isSucceeded;
}

bool SortingDatabaseManager::getAllFilterKeyValuePairs(QMap<QString, QString> &outKeyValueMap)
{
    bool isSucceeded = false;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
        static QString queryString("SELECT filter_part, selected_value FROM sorting_info");

        QSqlQuery query(db);
        query.prepare(queryString);

        if(!query.exec()) {
            DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
        }
        else {
            while(query.next()) {
                outKeyValueMap[query.value(0).toString()] = query.value(1).toString();
            }

            isSucceeded = true;
        }
    }

    this->closeDatabaseConnection();
    return isSucceeded;
}

