#ifndef USERDATABASEMANAGER_H
#define USERDATABASEMANAGER_H

#include "../../Common/Database/selfrepairingdatabase.h"

#include <QVariantList>

class UserDatabaseManager : public SelfRepairingDatabase
{
    Q_OBJECT

public:
    class UserDatabaseItemModel {
    public:
        UserDatabaseItemModel(QString userId, QString userName, bool isCurrentUser, QString userPsw) : _userId(userId), _userName(userName), _isCurrentUser(isCurrentUser), _userPsw(userPsw) {}

        QString getUserId() const { return this->_userId; }
        QString getUserName() const { return this->_userName; }
        bool getIsCurrentUser() const { return this->_isCurrentUser; }
        QString getUserPsw() const { return _userPsw; }

    private:
        QString _userId;
        QString _userName;
        bool _isCurrentUser;
        QString _userPsw;
    };

public:
    explicit UserDatabaseManager(QObject *parent = 0);


    // ---------------------------------
    // --- user manipulation methods ---

    //
    // returns whether it was successful
    bool addOrReplaceUser(QString userId, QString userName, bool isSetAsCurrent, QString userPsw);

    //
    // returns whether it was successful
    bool setCurrentUserByUserId(QString userId);
    bool setNoCurrentUser();
    //
    // returns the current user id
    //  or an empty string if no current user selected
    QString getCurrentUserId();
    QString getCurrentUserName();
    QString _getUserNameByUserId(QString userId);
    QString _getUserIdByUserName(QString userName);
    QString _getUserPswByUserId(QString userId);

    QList<UserDatabaseItemModel> getAllUsers();

    bool _deleteAllUsers();

protected:

    // ------------------------
    // --- database related ---

    QString getDatabasePath();
    bool fillDatabaseWithBasicData(QSqlDatabase databaseToFill) { return false; }
    bool copyDatabaseFromResourceToPath(QString path);

    DatabaseTableSchemeDescriptor _getRequiredSchemeDescriptionForTable(QString tableName);

private:
    bool doQueryAndTrySelfRepairIfRequired(QString queryString, QVariantList queryBindValues, QSqlDatabase db, QSqlQuery &outQuery);
    bool tryToRepairDatabaseAndMarkItForSkipNextTime(QSqlDatabase db);

private:
    DatabaseTableSchemeDescriptor _usersDatabaseTableScheme;
};

#endif // USERDATABASEMANAGER_H
