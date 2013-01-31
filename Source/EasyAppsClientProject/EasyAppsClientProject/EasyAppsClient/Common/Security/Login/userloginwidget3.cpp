#include "userloginwidget3.h"

// UI
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include <QLineEdit>
//#include <QCheckBox>
#include <QPushButton>

#include "Common/Widgets/simpleimagepresenterwidget.h"
#include "Common/Widgets/contentcoverstatusmessagewidget.h"
#include "Common/Widgets/checkboxbutton.h"

//
#include <QStyleOption>
#include <QPainter>

// network
#include "Common/Security/authorization.h"
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
#include "Common/Communication/basenetworkentity.h"

#include <QDesktopServices>
#include <QUrl>
#include <QResizeEvent>

// global objects
#include "Application/globalobjectregistry.h"
#include "Application/User/usermanager.h"

// settings
#include "Settings/settings.h"
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"

// helpers
#include "Application/uifactory.h"

UserLoginWidget3::UserLoginWidget3(QWidget *parent) :
    QWidget(parent),
    _communicationDescriptionGateway(NULL)
{
    this->__setupUi();
}

void UserLoginWidget3::__setupUi()
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->setStyleSheet(QString("UserLoginWidget3 { background: url(':/login/background') repeat-x #67b800; margin: 0px; padding: 0px; }"));

    //
    // create widgets
    SimpleImagePresenterWidget *logoPresenterWidget = new SimpleImagePresenterWidget;
    logoPresenterWidget->setIsAutoResizeToImageSize(true);
    logoPresenterWidget->setImageByPath(QString(":/app_browser_large_icon"), ImageTypes::PixelBased);

    QLabel *usernameLabel = new QLabel(tr("Username:"));
    usernameLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/LightGray/style"));
    _usernameLineEdit = new QLineEdit;
    _usernameLineEdit->setStyleSheet(guiConfManager->getStyleForId("LineEdit/Default/Gray/style"));

    QLabel *passwordLabel = new QLabel(tr("Password:"));
    passwordLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/LightGray/style"));
    _forgotPasswordLinkLabel = new QLabel(QString("<a href='#' style='color: #fff; text-decoration:none;'>%1</a>").arg(tr("Forgot you password?")));
    _forgotPasswordLinkLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/LightGray/style"));
    connect(_forgotPasswordLinkLabel, SIGNAL(linkActivated(QString)), this, SLOT(openForgotPasswordLink(QString)));
    _passwordLineEdit = new QLineEdit;
    _passwordLineEdit->setEchoMode(QLineEdit::Password);
    _passwordLineEdit->setStyleSheet(guiConfManager->getStyleForId("LineEdit/Default/Gray/style"));
    connect(_passwordLineEdit, SIGNAL(returnPressed()), this, SLOT(signIn()));
    _rememberMeCheckBox = new CheckboxButton(tr("Remember me"));
    _rememberMeCheckBox->setStyleSheet(guiConfManager->getStyleForId("CheckboxButton/Default/NoBackgroundLightGray/style"));
    _rememberMeCheckBox->setChecked(true);
    connect(_rememberMeCheckBox, SIGNAL(toggled(bool)), this, SLOT(_isRememberMeCheckBoxToggled(bool)));

    _loginButton = new QPushButton(tr("Login"));
    _loginButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    connect(_loginButton, SIGNAL(clicked()), this, SLOT(signIn()));
    _loginButton->setFlat(true); // required by Mac for buttons with style-sheet

    _cancelButton = new QPushButton(tr("Offline"));
    _cancelButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    connect(_cancelButton, SIGNAL(clicked()), this, SLOT(closeButtonPressed()));
    _cancelButton->setFlat(true); // required by Mac for buttons with style-sheet

    _signUpLinkLabel = new QLabel(QString("<a href='#' style='color: #fff; text-decoration:none;'>%1</a>").arg(tr("Sign up!")));
    _signUpLinkLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/LightGray/style"));
    connect(_signUpLinkLabel, SIGNAL(linkActivated(QString)), this, SLOT(openSignUpLink(QString)));


    //
    // layouting
    QVBoxLayout *mainLayout = AppWhirr::UiFactory::createDefaultVBoxLayout();
    this->setLayout(mainLayout);

    QHBoxLayout *loginMainContainerLayout = new QHBoxLayout;
    loginMainContainerLayout->addSpacerItem(AppWhirr::UiFactory::createHorizontalSpacer());
    QVBoxLayout *logoContainerLayout = new QVBoxLayout;
    logoContainerLayout->addWidget(logoPresenterWidget);
    logoContainerLayout->addSpacerItem(AppWhirr::UiFactory::createVerticalSpacer());
    loginMainContainerLayout->addLayout(logoContainerLayout);
    loginMainContainerLayout->addSpacing(20);

    QVBoxLayout *loginControlLayout = new QVBoxLayout;
    loginControlLayout->addSpacing(20);
    loginControlLayout->addWidget(usernameLabel);
    loginControlLayout->addSpacing(2);
    _usernameLineEdit->setMinimumWidth(300);
    loginControlLayout->addWidget(_usernameLineEdit);

    loginControlLayout->addSpacing(5);

    QHBoxLayout *passwordLineLayout = new QHBoxLayout;
    passwordLineLayout->addWidget(passwordLabel);
    passwordLineLayout->addSpacerItem(AppWhirr::UiFactory::createHorizontalSpacer());
    passwordLineLayout->addWidget(_forgotPasswordLinkLabel);
    loginControlLayout->addLayout(passwordLineLayout);

    loginControlLayout->addSpacing(2);
    loginControlLayout->addWidget(_passwordLineEdit);

    loginControlLayout->addSpacing(3);

    loginControlLayout->addWidget(_rememberMeCheckBox);

    loginControlLayout->addSpacing(15);

    QHBoxLayout *bottomActionLineLayout = new QHBoxLayout;
    bottomActionLineLayout->addWidget(_loginButton);
    bottomActionLineLayout->addSpacing(3);
    bottomActionLineLayout->addWidget(_cancelButton);
    bottomActionLineLayout->addSpacerItem(AppWhirr::UiFactory::createHorizontalSpacer());
    bottomActionLineLayout->addWidget(_signUpLinkLabel);
    loginControlLayout->addLayout(bottomActionLineLayout);
    loginControlLayout->addSpacerItem(AppWhirr::UiFactory::createVerticalSpacer());

    loginMainContainerLayout->addLayout(loginControlLayout);
//    loginMainContainerLayout->addSpacerItem(new QSpacerItem(270, 5));
    loginMainContainerLayout->addSpacerItem(AppWhirr::UiFactory::createHorizontalSpacer());

    mainLayout->addLayout(loginMainContainerLayout);
    mainLayout->addSpacerItem(AppWhirr::UiFactory::createVerticalSpacer());

    //
    //
    _statusMessagePresenterWidget = new ContentCoverStatusMessageWidget(300, 2000);
    _statusMessagePresenterWidget->setParent(this);
    _statusMessagePresenterWidget->hide();
}


void UserLoginWidget3::showEvent(QShowEvent *e)
{
    this->actualizeUiByCurrentLoginMode();

//    QDialog::showEvent(e);
}

void UserLoginWidget3::resizeEvent(QResizeEvent *e)
{
    _statusMessagePresenterWidget->setGeometryToThis(this->geometry());
    QWidget::resizeEvent(e);
}

void UserLoginWidget3::paintEvent(QPaintEvent *e)
{
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void UserLoginWidget3::actualizeUiByCurrentLoginMode()
{
//    this->ui->comboBox->clear();
    _passwordLineEdit->setText("");
    disconnect(_rememberMeCheckBox, SIGNAL(toggled(bool)), this, SLOT(_isRememberMeCheckBoxToggled(bool)));
    _rememberMeCheckBox->setChecked(Settings::sharedSettings()->getIsRememberLastUser());
    connect(_rememberMeCheckBox, SIGNAL(toggled(bool)), this, SLOT(_isRememberMeCheckBoxToggled(bool)));

    this->switchToCustomUserMode("");

    QString currUserName = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->getCurrentUserId();
    _usernameLineEdit->setText(currUserName);

    Authorization::UserLoginModes userLoginMode = Authorization::sharedAuthorization()->getCurrentUserLoginMode();
    if(userLoginMode == Authorization::ULM_Initial) {
        _usernameLineEdit->setEnabled(true);
        // also set some mode related texts
        _loginButton->setText(tr("Login"));
        _cancelButton->setText(tr("Offline"));
    }
    else if(userLoginMode == Authorization::ULM_ConfirmationOnly) {
        _usernameLineEdit->setEnabled(false);
        // also set some mode related texts
        _loginButton->setText(tr("Login"));
        _cancelButton->setText(tr("Cancel"));
    }
    else {
        WLog("Invalid user-login-mode: ") << userLoginMode;
    }

    this->setFocusToRequiredField();

//    QList<UserDatabaseManager::UserDatabaseItemModel> users = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->getAllUsers();

//    if(users.isEmpty()) {
//        // no user
//        this->switchToCustomUserMode("");
//    }
//    else {
//        this->switchToSelectUserMode();

//        // users found, show them in list
//        int userCnt = users.size();
//        int currUserIdx = -1;
//        for(int i = 0; i < userCnt; i++) {
//            UserDatabaseManager::UserDatabaseItemModel currUserModel = users[i];
//            ui->comboBox->addItem(currUserModel.getUserName());
//            if(currUserModel.getIsCurrentUser()) {
//                currUserIdx = i;
//            }
//        }

//        if(currUserIdx >= 0) {
//            ui->comboBox->setCurrentIndex(currUserIdx);
//        }
//    }
}

void UserLoginWidget3::setFocusToRequiredField()
{
//    if(this->ui->comboBox->isVisible()) {
    if(!_usernameLineEdit->text().isEmpty()) {
        _passwordLineEdit->setFocusPolicy(Qt::StrongFocus);
        _passwordLineEdit->setFocus();
    }
    else {
        _usernameLineEdit->setFocusPolicy(Qt::StrongFocus);
        _usernameLineEdit->setFocus();
    }
}

void UserLoginWidget3::userNameSelected(QString selectedUserName)
{
    _usernameLineEdit->setText(selectedUserName);
}

void UserLoginWidget3::switchToCustomUserMode(QString link)
{
//    this->ui->comboBox->hide();
    _usernameLineEdit->show();
//    this->ui->NotYou->hide();
}

//void UserLoginWidget3::switchToSelectUserMode()
//{
//    this->ui->comboBox->show();
//    _usernameLineEdit->hide();
//    this->ui->NotYou->show();
//}

void UserLoginWidget3::signIn() {

    if(_usernameLineEdit->text().isEmpty()) {
        _statusMessagePresenterWidget->showWithAnimationAndActivate(tr("You have to specify a User name!"));
        return;
    }

    Authorization::sharedAuthorization()->setUsername(_usernameLineEdit->text());
    Authorization::sharedAuthorization()->setPassword(_passwordLineEdit->text());

    AW_QOBJECT_SAFE_DELETE(_communicationDescriptionGateway);
    _communicationDescriptionGateway = new CommunicationDescriptionGateway;
    connect(_communicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(userLoggedIn(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(_communicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_userLoginFailedWithErrorMessage(QString)));
    connect(_communicationDescriptionGateway, SIGNAL(authenticationChallengeHappened()), this, SLOT(_invalidLoginInformation()));
    connect(_communicationDescriptionGateway, SIGNAL(finishedCompletely()), this, SLOT(_userLoginFinishedCompletely()));

    _communicationDescriptionGateway->requestForLoggingUserIn();
}

void UserLoginWidget3::_userLoginFailedWithErrorMessage(QString errorMessage)
{
    WLog("User login failed with error: ") << errorMessage;
//    Q_EMIT userLoginWasAborted();
    Q_EMIT userLoginCommunicationError(errorMessage);

    _statusMessagePresenterWidget->showWithAnimationAndActivate(tr("Login failed: %1").arg(errorMessage));
}

void UserLoginWidget3::_invalidLoginInformation()
{
    _statusMessagePresenterWidget->showWithAnimationAndActivate(tr("Login failed: %1").arg(tr("Invalid username or password.")));
}

void UserLoginWidget3::_userLoginFinishedCompletely()
{
    DLog("User login finished completely.");
}

void UserLoginWidget3::closeButtonPressed()
{
    if(Authorization::sharedAuthorization()->getCurrentUserLoginMode() == Authorization::ULM_Initial) {
        // store the user-name, if any
        QString userName = _usernameLineEdit->text();
        if(!userName.isEmpty()) {
            // check wether the given userName is already exists in the user-database
            if(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->getUserIdByUserName(userName).isEmpty()) {
                // user is never logged in on this machine
                _statusMessagePresenterWidget->showWithAnimationAndActivate(tr("You have to log in at least once before you could use the Offline mode."));
                return;
            }
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->setUserAsCurrent(userName, userName, QString());
        }
        else {
            _statusMessagePresenterWidget->showWithAnimationAndActivate(tr("You have to specify a User name!"));
            return;
        }
    }
    Q_EMIT userLoginWasAborted();
}

void UserLoginWidget3::openSignUpLink(QString link) {
    QDesktopServices::openUrl(QUrl("https://appwhirr.com/u/register/"));
}

void UserLoginWidget3::openForgotPasswordLink(QString link)
{
    QDesktopServices::openUrl(QUrl("https://appwhirr.com/u/forget/request"));
}

void UserLoginWidget3::failedToLogin() {
//    ui->errorLabel->setText("Bad username and/or password");
}

void UserLoginWidget3::userLoggedIn(const QList<AW::Common::Communication::BaseNetworkEntity*>& entities) {
    DLog("User login happened.");
    Q_EMIT this->userIsLoggedIn();

//    this->close();
}

void UserLoginWidget3::_isRememberMeCheckBoxToggled(bool isChecked)
{
    Settings::sharedSettings()->setIsRememberLastUser(isChecked);
}
