#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QNetworkRequest;
class QNetworkReply;
class QString;
QT_END_NAMESPACE

class UserLoginDialog;

class Authorization : public QObject
{
    Q_OBJECT
public:
    enum UserLoginModes {
        ULM_Initial = 0, // initial: username and online/offline mode can be defined
        ULM_ConfirmationOnly = 1 // username is fixed, cannot be changed, it's only a confirmation
    };

public:
    static Authorization* sharedAuthorization();

    bool addHttpBasicAuthentication(QNetworkRequest* request);
    bool checkAuthorizationInHeader(QNetworkReply* reply);
    void removeCredentials(bool deleteUsername = false);

    void setUsername(QString username);
    void setPassword(QString password);

    void showLoginDialogAndStartLogin();
    void hideLoginDialog();

    bool isUserLoggedIn() const;

    void setUserLoginMode(UserLoginModes userLoginMode);
    UserLoginModes getCurrentUserLoginMode() const;

public Q_SLOTS:
    void _userLoginHappened();
    void _userLoginAbortHappened();
    void _userLoginCommunicationError(QString errorMessage);

Q_SIGNALS:
    void userLoginStarted();
    void userLoggedIn();
    void userLoginAborted();
    //
    // will be sent if communication error happens (e.g. server is unaccessible)
    //  it will be be followed by an 'user-login-aborted' signal as well, this is just a status signal
    void userLoginCommunicationError(QString errorMessage);

    void _showLoginDialogRequested();
    void _hideLoginDialogRequested();

//public Q_SLOTS:
//    void closeDialog(int result);

private:
    explicit Authorization(QObject *parent = 0);

    QString             username;
    QString             password;
    bool _isUserLoggedIn;

//    UserLoginDialog*    userLoginDialog;

    UserLoginModes _currentUserLoginMode;
};

#endif // AUTHORIZATION_H
