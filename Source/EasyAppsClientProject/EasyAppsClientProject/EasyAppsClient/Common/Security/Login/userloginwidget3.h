#ifndef USERLOGINWIDGET3_H
#define USERLOGINWIDGET3_H

#include <QWidget>
#include <QShowEvent>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QLabel;
//class QCheckBox;
QT_END_NAMESPACE

class CommunicationDescriptionGateway;
class ContentCoverStatusMessageWidget;
class CheckboxButton;

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
}
}
}


class UserLoginWidget3 : public QWidget
{
    Q_OBJECT

public:
    explicit UserLoginWidget3(QWidget *parent = 0);

    void failedToLogin();
    void actualizeUiByCurrentLoginMode();

    void setFocusToRequiredField();

Q_SIGNALS:
    void userIsLoggedIn();
    void userLoginWasAborted();
    void userLoginCommunicationError(QString errorMessage);

protected:
    void showEvent(QShowEvent *e);
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

protected Q_SLOTS:
    void signIn();
    void openSignUpLink(QString link);
    void openForgotPasswordLink(QString link);
    void userLoggedIn(const QList<AW::Common::Communication::BaseNetworkEntity*>& entities);
    void closeButtonPressed();

private Q_SLOTS:
    void switchToCustomUserMode(QString link);
//    void switchToSelectUserMode();
    void userNameSelected(QString selectedUserName);

    void _userLoginFailedWithErrorMessage(QString errorMessage);
    void _invalidLoginInformation();
    void _userLoginFinishedCompletely();

    void _isRememberMeCheckBoxToggled(bool isChecked);

private:
    void __setupUi();

private:
    // UI
    QLineEdit *_usernameLineEdit;
    QLineEdit *_passwordLineEdit;
    QLabel *_forgotPasswordLinkLabel;
    CheckboxButton *_rememberMeCheckBox;
    QPushButton *_loginButton;
    QPushButton *_cancelButton;
    QLabel *_signUpLinkLabel;


    ContentCoverStatusMessageWidget *_statusMessagePresenterWidget;

    // network
    CommunicationDescriptionGateway*    _communicationDescriptionGateway;
};

#endif // USERLOGINWIDGET3_H
