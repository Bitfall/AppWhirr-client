#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>

#include "userdatabasemanager.h"

class UserManager : public QObject
{
    Q_OBJECT
public:
    explicit UserManager(QObject *parent = 0);

    void setUserAsCurrent(QString userId, QString userName, QString userPsw);
    void removeUsers();
    void setNoUserIsCurrent();

    //
    // returns whether it was successful
//    bool addOrReplaceUser(QString userId, QString userName, bool isSetAsCurrent);

    //
    // returns whether it was successful
//    bool setCurrentUserByUserId(QString userId);
//    bool setNoCurrentUser();

    //
    // returns the current user id
    //  or an empty string if no current user selected
    QString getCurrentUserId();
    QString getCurrentUserName();

    /*! Gets the userId by userName

        If the given \a userName is not found it returns an empty string
    */
    QString getUserIdByUserName(QString userName);

    QString getUserPswByUserId(QString userId);
//    QString _getUserNameByUserId(QString userId);

//    QList<UserDatabaseManager::UserDatabaseItemModel> getAllUsers();
};

#endif // USERMANAGER_H
