#include "authorization.h"

#include <QString>
#include <QNetworkRequest>
#include <QNetworkReply>

Authorization::Authorization(QObject *parent) :
    QObject(parent),
    username(""),
    password(""),
    _isUserLoggedIn(false),
    _currentUserLoginMode(ULM_Initial)
//    userLoginDialog(NULL)
{
}

Authorization* Authorization::sharedAuthorization() {
    static Authorization* authorization = 0;

    if (!authorization) {
        authorization = new Authorization;
    }
    return authorization;
}

void Authorization::setUsername(QString username) {
    this->username = username;
    DLog("User name: ") << username;
}

void Authorization::setPassword(QString password) {
    this->password = password;
    DLog("Psw: ") << password;
}

bool Authorization::addHttpBasicAuthentication(QNetworkRequest* request) {
    if (username.isEmpty() || password.isEmpty()) return false;

    // Deprecated
    QString loginData = username + ":" + password;
    request->setRawHeader("Authorization", QByteArray("Basic ").append(loginData.toAscii().toBase64()));

    request->setRawHeader("username", username.toAscii());
    request->setRawHeader("password", password.toAscii());

    DLog(QString("%1: %2").arg(username, password));

    return true;
}

bool Authorization::checkAuthorizationInHeader(QNetworkReply* reply) {
    // 401 Unauthorized
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
        // remove credentials
        this->removeCredentials();

        return false;
    }

    return true;
}

void Authorization::removeCredentials(bool deleteUsername) {
    if (deleteUsername)
        username = "";

    password = "";
    this->_isUserLoggedIn = false;
}

void Authorization::_userLoginHappened()
{
//    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->setUserAsCurrent(this->username, this->username);
    _isUserLoggedIn = true;
    Q_EMIT userLoggedIn();

//    bool isNewUserAdded = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->addOrReplaceUser(this->username, this->username, true);

//    if(!isNewUserAdded) {
//        WLog("Cannot add the new user.");
//        return;
//    }
//    else {
//        _isUserLoggedIn = true;
//        Q_EMIT userLoggedIn();
//    }
}

void Authorization::_userLoginAbortHappened() {
    this->removeCredentials();
    Q_EMIT userLoginAborted();
}

void Authorization::_userLoginCommunicationError(QString errorMessage)
{
    DLog("Login error: ") << errorMessage;
    this->removeCredentials();
    Q_EMIT userLoginCommunicationError(errorMessage);
}

void Authorization::hideLoginDialog() {
    Q_EMIT _hideLoginDialogRequested();
}

void Authorization::showLoginDialogAndStartLogin() {
    Q_EMIT userLoginStarted();
    Q_EMIT _showLoginDialogRequested();

//    if (!userLoginDialog) {
//        userLoginDialog = new UserLoginDialog;
//        userLoginDialog->setModal(true);
//        userLoginDialog->show();

//        connect(userLoginDialog, SIGNAL(finished(int)), this, SLOT(closeDialog(int)));
//        connect(userLoginDialog, SIGNAL(userIsLoggedIn()), this, SIGNAL(userSignedIn()));
//    } else {
//        userLoginDialog->failedToLogin();
//    }
}

//void Authorization::closeDialog(int result) {
//    AW_QOBJECT_SAFE_DELETE(userLoginDialog);
//}

bool Authorization::isUserLoggedIn() const {
    return this->_isUserLoggedIn;
}

void Authorization::setUserLoginMode(UserLoginModes userLoginMode)
{
    this->_currentUserLoginMode = userLoginMode;
}

Authorization::UserLoginModes Authorization::getCurrentUserLoginMode() const {
    return this->_currentUserLoginMode;
}
