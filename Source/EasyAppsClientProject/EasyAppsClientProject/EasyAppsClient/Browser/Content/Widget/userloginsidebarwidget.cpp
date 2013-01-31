#include "userloginsidebarwidget.h"

#include <QHBoxLayout>

#include "Common/Security/Login/userloginwidget3.h"

// authorization
#include "Common/Security/authorization.h"

UserLoginSidebarWidget::UserLoginSidebarWidget(QSize expandedSize, QWidget *parent) :
    AnimatedTwoFixedSizeWidgetBase(1000, QSize(0, 0), expandedSize, false, false, true, parent),
    _isUserLoginWasSuccessful(false)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // left spacer
//    mainLayout->addSpacerItem(new QSpacerItem(50, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _userLoginWidget = new UserLoginWidget3;
    mainLayout->addWidget(_userLoginWidget);
    connect(_userLoginWidget, SIGNAL(userIsLoggedIn()), this, SLOT(_userIsLoggedIn()));
    connect(_userLoginWidget, SIGNAL(userLoginWasAborted()), this, SLOT(_userLoginWasAborted()));
    connect(_userLoginWidget, SIGNAL(userLoginCommunicationError(QString)), this, SLOT(_userLoginCommunicationError(QString)));

    // right space
//    mainLayout->addSpacerItem(new QSpacerItem(50, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

//    this->setObjectName("UserLoginSidebarWidget");
//    this->setStyleSheet("* { background:#a0a0a0; }");

//    this->setIsSetHeight(true);
//    this->setIsSetWidth(false);

    connect(this, SIGNAL(switchToExpandedStateFinished()), this, SLOT(_switchToExpandedStateFinished()));
    connect(this, SIGNAL(switchToCollapsedStateFinished()), this, SLOT(_switchToCollapsedStateFinished()));
}

void UserLoginSidebarWidget::resizeEvent(QResizeEvent *e)
{
//    DLog("RESIZE: ") << e->size();
}

void UserLoginSidebarWidget::actualizeUsersList()
{
    this->_userLoginWidget->actualizeUiByCurrentLoginMode();
}

void UserLoginSidebarWidget::_userIsLoggedIn()
{
    this->_isUserLoginWasSuccessful = true;
    this->switchToCollapsedStateSlot();
}

void UserLoginSidebarWidget::_userLoginWasAborted()
{
    this->_isUserLoginWasSuccessful = false;
    this->switchToCollapsedStateSlot();
}

void UserLoginSidebarWidget::_userLoginCommunicationError(QString errorMessage)
{
    DLog("Login error: ") << errorMessage;
    Authorization::sharedAuthorization()->_userLoginCommunicationError(errorMessage);

//    this->_isUserLoginWasSuccessful = false;
//    this->switchToCollapsedStateSlot();
}

void UserLoginSidebarWidget::_switchToExpandedStateFinished()
{
    // grab focus
    this->_userLoginWidget->setFocus();
    this->_userLoginWidget->setFocusToRequiredField();
}

void UserLoginSidebarWidget::_switchToCollapsedStateFinished()
{
    if(_isUserLoginWasSuccessful) {
        Authorization::sharedAuthorization()->_userLoginHappened();
    }
    else {
        Authorization::sharedAuthorization()->_userLoginAbortHappened();
    }
}
