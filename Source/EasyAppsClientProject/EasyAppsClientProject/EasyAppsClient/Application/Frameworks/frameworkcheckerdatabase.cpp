#include "frameworkcheckerdatabase.h"

#include <QSqlQuery>
#include <QVariant>

// utility
#include "Common/Helpers/databasehelper.h"


FrameworkCheckerDatabase::FrameworkCheckerDatabase(QString databasePath, QObject *parent) :
    BaseDatabase("FrameworkCheckerDatabase", parent),
    _databasePath(databasePath)
{
}


bool FrameworkCheckerDatabase::addFrameworkChecker(FrameworkCheckerInfo frameworkCheckerInfo)
{
    bool isSucceeded = true;

    {
        static QString queryStringWithPlaceholders("INSERT OR REPLACE INTO framework_checkers (checker_id, related_fw_name, path, update_timestamp) VALUES (?, ?, ?, ?)");
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
        QSqlQuery query(db);
        query.prepare(queryStringWithPlaceholders);
        query.bindValue(0, frameworkCheckerInfo.getFrameworkCheckerId());
        query.bindValue(1, frameworkCheckerInfo.getRelatedFrameworkName());
        query.bindValue(2, frameworkCheckerInfo.getFrameworkCheckerPath());
        query.bindValue(3, frameworkCheckerInfo.getFrameworkCheckerUpdateTimestamp());

        if(!query.exec()) {
            DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
            isSucceeded = false;
        }
    }

    this->closeDatabaseConnection();
    return isSucceeded;
}

bool FrameworkCheckerDatabase::getFrameworkChecker(QString frameworkCheckerId, FrameworkCheckerInfo &outValue)
{
    bool isSucceeded = false;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
        static QString queryString("SELECT related_fw_name, path, update_timestamp FROM framework_checkers WHERE checker_id=? LIMIT 1");

        QSqlQuery query(db);
        query.prepare(queryString);
        query.bindValue(0, frameworkCheckerId);

        if(!query.exec()) {
            DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
        }
        else if (query.next()) {
            QString relatedFwName = query.value(0).toString();
            QString fwCheckerPath = query.value(1).toString();
            QString fwCheckerUpdateTimestamp = query.value(2).toString();

            outValue.setFrameworkCheckerId(frameworkCheckerId);
            outValue.setRelatedFrameworkName(relatedFwName);
            outValue.setFrameworkCheckerPath(fwCheckerPath);
            outValue.setFrameworkCheckerUpdateTimestamp(fwCheckerUpdateTimestamp);

            isSucceeded = true;
        }
    }

    this->closeDatabaseConnection();
    return isSucceeded;
}


// ----------------
// --- internal ---

QString FrameworkCheckerDatabase::getDatabasePath()
{
    return _databasePath;
}

bool FrameworkCheckerDatabase::fillDatabaseWithBasicData(QSqlDatabase databaseToFill)
{
    static QString queryString("CREATE TABLE framework_checkers (checker_id TEXT UNIQUE, related_fw_name TEXT, path TEXT, update_timestamp TEXT)");
    QSqlQuery query(databaseToFill);
    query.prepare(queryString);
    if(!query.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
        return false;
    }

    return true;
}
