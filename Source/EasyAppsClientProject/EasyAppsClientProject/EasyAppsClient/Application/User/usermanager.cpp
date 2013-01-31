#include "usermanager.h"

#include "userdatabasemanager.h"

UserManager::UserManager(QObject *parent) :
    QObject(parent)
{
}

void UserManager::setUserAsCurrent(QString userId, QString userName, QString userPsw)
{
    // first remove every other user from the database
//    this->removeUsers();

//    DLog("!!REMOVE THIS!! Set user as active: ") << userId << userName << userPsw;

    //
    // then add the new one and set it as current one
    UserDatabaseManager *userDbManager = new UserDatabaseManager;
    bool isSuccess = userDbManager->addOrReplaceUser(userId, userName, true, userPsw);
    delete userDbManager;

    if(!isSuccess) {
        WLog("Failed to add the user to the database!!!");
    }
}

void UserManager::removeUsers()
{
    UserDatabaseManager *userDbManager = new UserDatabaseManager;
    bool isSuccess = userDbManager->_deleteAllUsers();
    delete userDbManager;

    if(!isSuccess) {
        WLog("Failed to remove all user from the database!!!");
    }
}

void UserManager::setNoUserIsCurrent()
{
    UserDatabaseManager *userDbManager = new UserDatabaseManager;
    bool isSuccess = userDbManager->setNoCurrentUser();
    delete userDbManager;

    if(!isSuccess) {
        WLog("Failed to set 'no user is current'");
    }
}

//
// returns whether it was successful
//bool UserManager::addOrReplaceUser(QString userId, QString userName, bool isSetAsCurrent)
//{
//    UserDatabaseManager *userDbManager = new UserDatabaseManager;
//    bool isSuccess = userDbManager->addOrReplaceUser(userId, userName, isSetAsCurrent);
//    delete userDbManager;

//    return isSuccess;
//}

////
//// returns whether it was successful
//bool UserManager::setCurrentUserByUserId(QString userId)
//{
//    UserDatabaseManager *userDbManager = new UserDatabaseManager;
//    bool isSuccess = userDbManager->setCurrentUserByUserId(userId);
//    delete userDbManager;

//    return isSuccess;
//}

//bool UserManager::setNoCurrentUser()
//{
//    UserDatabaseManager *userDbManager = new UserDatabaseManager;
//    bool isSuccess = userDbManager->setNoCurrentUser();
//    delete userDbManager;

//    return isSuccess;
//}

//
// returns the current user id
//  or an empty string if no current user selected
QString UserManager::getCurrentUserId()
{
    UserDatabaseManager *userDbManager = new UserDatabaseManager;
    QString currUserId = userDbManager->getCurrentUserId();
    delete userDbManager;

    return currUserId;
}

QString UserManager::getCurrentUserName()
{
    UserDatabaseManager *userDbManager = new UserDatabaseManager;
    QString currUserName = userDbManager->getCurrentUserName();
    delete userDbManager;

    return currUserName;
}

QString UserManager::getUserIdByUserName(QString userName)
{
    UserDatabaseManager *userDbManager = new UserDatabaseManager;
    QString relatedUserId = userDbManager->_getUserIdByUserName(userName);
    delete userDbManager;

    return relatedUserId;
}

QString UserManager::getUserPswByUserId(QString userId)
{
    UserDatabaseManager *userDbManager = new UserDatabaseManager;
    QString relatedUserPsw = userDbManager->_getUserPswByUserId(userId);
    delete userDbManager;

    return relatedUserPsw;
}

//QString UserManager::_getUserNameByUserId(QString userId)
//{
//    UserDatabaseManager *userDbManager = new UserDatabaseManager;
//    QString userName = userDbManager->_getUserNameByUserId(userId);
//    delete userDbManager;

//    return userName;
//}

//QList<UserDatabaseManager::UserDatabaseItemModel> UserManager::getAllUsers()
//{
//    UserDatabaseManager *userDbManager = new UserDatabaseManager;
//    QList<UserDatabaseManager::UserDatabaseItemModel> result = userDbManager->getAllUsers();
//    delete userDbManager;

//    return result;
//}
