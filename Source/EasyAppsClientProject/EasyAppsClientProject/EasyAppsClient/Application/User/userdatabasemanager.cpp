#include "userdatabasemanager.h"

// sql
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

// utility
#include "../../Common/Helpers/pathhelper.h"
#include "../appspecificpathhelper.h"
#include "../../Common/Helpers/databasehelper.h"

UserDatabaseManager::UserDatabaseManager(QObject *parent) :
    SelfRepairingDatabase("UserManager", parent)
{
    //
    // users table scheme
    _usersDatabaseTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "userId", "TEXT UNIQUE NOT NULL"));
    _usersDatabaseTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "userName", "TEXT"));
    _usersDatabaseTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "isCurrentUser", "NUMERIC DEFAULT 0"));
    _usersDatabaseTableScheme.addThisItem(DatabaseTableSchemeDescriptor::DatabaseTableSchemeDescriptorItem(-1, "userPsw", "TEXT"));
}

// ------------------------
// --- database related ---

QString UserDatabaseManager::getDatabasePath()
{
    return AppSpecificPathHelper::getLocalRootDirectory() + "/" + "Users.sqlite";
}

bool UserDatabaseManager::copyDatabaseFromResourceToPath(QString path)
{
    return PathHelper::copyFileFromResource(":/baseDatabases/UsersBaseDatabase", path, true);
}

DatabaseTableSchemeDescriptor UserDatabaseManager::_getRequiredSchemeDescriptionForTable(QString tableName)
{
    DLog("Scheme description requested for table: ") << tableName;

    if(tableName == "users") {
        return _usersDatabaseTableScheme;
    }

    WLog("Invalid table name - empty description returned.");
    return DatabaseTableSchemeDescriptor();
}

bool UserDatabaseManager::doQueryAndTrySelfRepairIfRequired(QString queryString, QVariantList queryBindValues, QSqlDatabase db, QSqlQuery &outQuery)
{
    bool isQuerySuccess = false;

    QSqlQuery query(db);
    query.prepare(queryString);
    for(int i = 0; i < queryBindValues.size(); ++i) {
        query.bindValue(i, queryBindValues.at(i));
    }

    if(!query.exec()) {
        DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(query);
        DLog(" Query info: ") << queryString << " bind value cnt: " << queryBindValues.size();
        // repair and retry
        if(!this->tryToRepairDatabaseAndMarkItForSkipNextTime(db)) {
            WLog("Repair failed.");
        }
        else {
            // retry - recreate the whole query
            DLog("Table repaired - Retry query");

            QSqlQuery retryQuery(db);
            retryQuery.prepare(queryString);
            for(int i = 0; i < queryBindValues.size(); ++i) {
                retryQuery.bindValue(i, queryBindValues.at(i));
            }

            if(!retryQuery.exec()) {
                DatabaseHelper::logQueryExecutionFailedWithLastErrorForQuery(retryQuery);
                WLog("Query failed again.");
            }
            else {
                DLog("Query second attempt success!");
                isQuerySuccess = true;
                outQuery = retryQuery;
            }
        }
    }
    else {
        isQuerySuccess = true;
        outQuery = query;
    }

    return isQuerySuccess;
}

bool UserDatabaseManager::tryToRepairDatabaseAndMarkItForSkipNextTime(QSqlDatabase db)
{
    bool isRepairSuccess = false;

    if(!this->_tryToRepairTheDatabaseStructureOfTable("users", db)) {
        isRepairSuccess = false;
    }
    else {
        isRepairSuccess = true;
    }

    // also mark them to skip at the next time
    this->_addSkipRepairForTable("users");

    return isRepairSuccess;
}


// ---------------------------------
// --- user manipulation methods ---

bool UserDatabaseManager::addOrReplaceUser(QString userId, QString userName, bool isSetAsCurrent, QString userPsw)
{
    bool isSucceeded = true;

//    QString searchUserName = this->_getUserNameByUserId(userId);
//    if(!searchUserName.isEmpty()) {
//        DLog("User already added to the database.");
////        return true;
//    }

//    else {
        // new user

        {
            QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
//            QSqlQuery query(db);
            static QString queryString("INSERT OR REPLACE INTO users (userId, userName, isCurrentUser, userPsw) VALUES(?, ?, ?, ?)");
//            query.prepare(queryString);
//            query.bindValue(0, userId);
//            query.bindValue(1, userName);
//            query.bindValue(2, QVariant(0));
//            query.bindValue(3, userPsw);
            QVariantList queryBindValues;
            queryBindValues.append(userId);
            queryBindValues.append(userName);
            queryBindValues.append(QVariant(0));
            queryBindValues.append(userPsw);

            QSqlQuery resultQuery;
            isSucceeded = this->doQueryAndTrySelfRepairIfRequired(queryString, queryBindValues, db, resultQuery);
        }

        this->closeDatabaseConnection();
//    }

    if(isSucceeded && isSetAsCurrent) {
        isSucceeded = this->setCurrentUserByUserId(userId);
    }

    DLog("Add or replace user - finished with: ") << isSucceeded;

    return isSucceeded;
}

QString UserDatabaseManager::_getUserNameByUserId(QString userId)
{
    QString _retUserName;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
//        QSqlQuery query(db);
        static QString queryString("SELECT userName FROM users WHERE userId=? LIMIT 1");
//        query.prepare(queryString);
//        query.bindValue(0, userId);

        QVariantList queryBindValues;
        queryBindValues.append(userId);

        QSqlQuery resultQuery;
        bool isQuerySuccess = this->doQueryAndTrySelfRepairIfRequired(queryString, queryBindValues, db, resultQuery);

        if (isQuerySuccess && resultQuery.next()) {
            _retUserName = resultQuery.value(0).toString();
        }
    }

    this->closeDatabaseConnection();
    return _retUserName;
}

QString UserDatabaseManager::_getUserIdByUserName(QString userName)
{
    QString _retUserId;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
//        QSqlQuery query(db);
        static QString queryString("SELECT userId FROM users WHERE userName=? LIMIT 1");
//        query.prepare(queryString);
//        query.bindValue(0, userName);

        QVariantList queryBindValues;
        queryBindValues.append(userName);

        QSqlQuery resultQuery;
        bool isQuerySuccess = this->doQueryAndTrySelfRepairIfRequired(queryString, queryBindValues, db, resultQuery);

        if (isQuerySuccess && resultQuery.next()) {
            _retUserId = resultQuery.value(0).toString();
        }
    }

    this->closeDatabaseConnection();
    return _retUserId;
}

QString UserDatabaseManager::_getUserPswByUserId(QString userId)
{
    QString retUserPsw;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
//        QSqlQuery query(db);
        static QString queryString("SELECT userPsw FROM users WHERE userId=? LIMIT 1");
//        query.prepare(queryString);
//        query.bindValue(0, userId);
        QVariantList queryBindValues;
        queryBindValues.append(userId);

        QSqlQuery resultQuery;
        bool isQuerySuccess = this->doQueryAndTrySelfRepairIfRequired(queryString, queryBindValues, db, resultQuery);

        if (isQuerySuccess && resultQuery.next()) {
            retUserPsw = resultQuery.value(0).toString();
        }
    }

    this->closeDatabaseConnection();
    return retUserPsw;
}

QList<UserDatabaseManager::UserDatabaseItemModel> UserDatabaseManager::getAllUsers()
{
    QList<UserDatabaseManager::UserDatabaseItemModel> _retUsers;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
//        QSqlQuery query(db);
        static QString queryString("SELECT userId, userName, isCurrentUser, userPsw FROM users");
//        query.prepare(queryString);

        QVariantList queryBindValues;

        QSqlQuery resultQuery;
        bool isQuerySuccess = this->doQueryAndTrySelfRepairIfRequired(queryString, queryBindValues, db, resultQuery);

        if(isQuerySuccess) {
            while(resultQuery.next()) {
                _retUsers.append(UserDatabaseItemModel(resultQuery.value(0).toString(), resultQuery.value(1).toString(), resultQuery.value(2).toBool(), resultQuery.value(3).toString()));
            }
        }
    }

    this->closeDatabaseConnection();
    return _retUsers;
}

bool UserDatabaseManager::setCurrentUserByUserId(QString userId)
{
    bool isSucceeded = true;

    // set other users as not-current
    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
        static QString queryString("UPDATE users SET isCurrentUser=?");
//        QSqlQuery query(db);
//        query.prepare(queryString);
//        query.bindValue(0, QVariant(0));

        QVariantList queryBindValues;
        queryBindValues.append(QVariant(0));

        QSqlQuery resultQuery;
        isSucceeded = this->doQueryAndTrySelfRepairIfRequired(queryString, queryBindValues, db, resultQuery);

        if(isSucceeded) {
            // and set this user as current
//            QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
            static QString queryStringForSetCurrentUser("UPDATE users SET isCurrentUser=? WHERE userId=?");
//            QSqlQuery query(db);
//            query.prepare(queryStringForSetCurrentUser);
//            query.bindValue(0, QVariant(1));
//            query.bindValue(1, userId);

            QVariantList queryBindValuesForSetCurrentUser;
            queryBindValuesForSetCurrentUser.append(QVariant(1));
            queryBindValuesForSetCurrentUser.append(userId);

            QSqlQuery resultQuery2;
            isSucceeded = this->doQueryAndTrySelfRepairIfRequired(queryStringForSetCurrentUser, queryBindValuesForSetCurrentUser, db, resultQuery2);
        }
    }

    this->closeDatabaseConnection();
    return isSucceeded;
}

bool UserDatabaseManager::setNoCurrentUser()
{
    bool isSucceeded = true;

    // set other users as not-current
    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
        static QString queryString("UPDATE users SET isCurrentUser=?");
//        QSqlQuery query(db);
//        query.prepare(queryString);
//        query.bindValue(0, QVariant(0));

        QVariantList queryBindValues;
        queryBindValues.append(QVariant(0));

        QSqlQuery resultQuery;
        isSucceeded = this->doQueryAndTrySelfRepairIfRequired(queryString, queryBindValues, db, resultQuery);
    }

    this->closeDatabaseConnection();
    return isSucceeded;
}

QString UserDatabaseManager::getCurrentUserId()
{
    QString _retUserId;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
//        QSqlQuery query(db);
        static QString queryString("SELECT userId FROM users WHERE isCurrentUser=? LIMIT 1");
//        query.prepare(queryString);
//        query.bindValue(0, QVariant(1));

        QVariantList queryBindValues;
        queryBindValues.append(QVariant(1));

        QSqlQuery resultQuery;
        bool isQuerySuccess = this->doQueryAndTrySelfRepairIfRequired(queryString, queryBindValues, db, resultQuery);

        if (isQuerySuccess && resultQuery.next()) {
            _retUserId = resultQuery.value(0).toString();
        }
    }

    DLog("Curr UserId: ") << _retUserId;

    this->closeDatabaseConnection();
    return _retUserId;
}

QString UserDatabaseManager::getCurrentUserName()
{
    QString _retUserName;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
//        QSqlQuery query(db);
        static QString queryString("SELECT userName FROM users WHERE isCurrentUser=? LIMIT 1");
//        query.prepare(queryString);
//        query.bindValue(0, QVariant(1));

        QVariantList queryBindValues;
        queryBindValues.append(QVariant(1));

        QSqlQuery resultQuery;
        bool isQuerySuccess = this->doQueryAndTrySelfRepairIfRequired(queryString, queryBindValues, db, resultQuery);

        if (isQuerySuccess && resultQuery.next()) {
            _retUserName = resultQuery.value(0).toString();
        }
    }

    this->closeDatabaseConnection();
    return _retUserName;
}

bool UserDatabaseManager::_deleteAllUsers()
{
    bool isSuccess = false;

    {
        QSqlDatabase db = this->openDatabaseConnectionAndReturnDatabase();
//        QSqlQuery query(db);
        static QString queryString("DELETE FROM users");
//        query.prepare(queryString);

        QVariantList queryBindValues;
        QSqlQuery resultQuery;
        isSuccess = this->doQueryAndTrySelfRepairIfRequired(queryString, queryBindValues, db, resultQuery);
    }

    this->closeDatabaseConnection();
    return isSuccess;
}
