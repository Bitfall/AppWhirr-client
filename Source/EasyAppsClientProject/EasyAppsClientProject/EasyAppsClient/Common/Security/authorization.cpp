#include "authorization.h"

#include <QString>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "Application/appwhirrapplication.h"
#include "Browser/browserwindow.h"

#include "Application/globalobjectregistry.h"
#include "Application/User/usermanager.h"

#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
//#include "Common/Communication/basenetworkentity.h"

Authorization::Authorization(QObject *parent) :
    QObject(parent),
    _username(""),
    _password(""),
    _isUserLoggedIn(false),
    _currentUserLoginMode(ULM_Initial),
    _communicationDescriptionGateway(NULL)
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
    this->_username = username;
}

void Authorization::setPassword(QString password) {
    this->_password = password;
}

bool Authorization::addHttpBasicAuthentication(QNetworkRequest* request) {
    if (_username.isEmpty() || _password.isEmpty()) return false;

    // Deprecated
    QString loginData = _username + ":" + _password;
    request->setRawHeader("Authorization", QByteArray("Basic ").append(loginData.toAscii().toBase64()));

    request->setRawHeader("username", _username.toUtf8());
    request->setRawHeader("password", _password.toUtf8());

//    DLog(QString("Added authentication headers with username (%1) and password (%2)").arg(username).arg(password));

    return true;
}

//bool Authorization::checkAuthorizationInHeader(QNetworkReply* reply) {
//    // 401 Unauthorized
//    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
//        // remove credentials
//        this->removeCredentials();

//        return false;
//    }

//    return true;
//}

void Authorization::removeCredentials(bool deleteUsername) {
    DLog("Remove credentials");
    if (deleteUsername)
        _username = "";

    _password = "";
    this->_isUserLoggedIn = false;
}

void Authorization::_userLoginHappened()
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->setUserAsCurrent(_username, _username, _password);
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
    DLog("User login abort happened");
//    this->removeCredentials();
    Q_EMIT userLoginAborted();
}

void Authorization::_userLoginCommunicationError(QString errorMessage)
{
    DLog("Login error: ") << errorMessage;
//    this->removeCredentials();
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

void Authorization::startLoginProcessWithoutUserLoginDialog(QString userName, QString userPsw)
{
    this->setUsername(userName);
    this->setPassword(userPsw);

    AW_QOBJECT_SAFE_DELETE(_communicationDescriptionGateway);
    _communicationDescriptionGateway = new CommunicationDescriptionGateway;
    connect(_communicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(_comUserLoginSuccess(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(_communicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_comUserLoginFailedWithError(QString)));
    connect(_communicationDescriptionGateway, SIGNAL(authenticationChallengeHappened()), this, SLOT(_comInvalidLogin()));
    connect(_communicationDescriptionGateway, SIGNAL(finishedCompletely()), this, SLOT(_comUserLoginProcessFinished()));

    _communicationDescriptionGateway->requestForLoggingUserIn();
}

void Authorization::_comUserLoginSuccess(QList<AW::Common::Communication::BaseNetworkEntity *> networkEntities)
{
    DLog("User login - success");
    this->_userLoginHappened();
}

void Authorization::_comUserLoginFailedWithError(QString errorMessage)
{
    WLog("User login failed with error: ") << errorMessage;
    this->_userLoginCommunicationError(errorMessage);
}

void Authorization::_comInvalidLogin()
{
    WLog("User login - invalid login");
    this->_userLoginAbortHappened();
}

void Authorization::_comUserLoginProcessFinished()
{
    DLog("user login process finished");
}
