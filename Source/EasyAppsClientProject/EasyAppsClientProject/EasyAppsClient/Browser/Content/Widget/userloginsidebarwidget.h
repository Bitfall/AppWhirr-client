#ifndef USERLOGINSIDEBARWIDGET_H
#define USERLOGINSIDEBARWIDGET_H

#include "Common/Widgets/animatedtwofixedsizewidgetbase.h"

#include <QResizeEvent>

class UserLoginWidget3;

class UserLoginSidebarWidget : public AnimatedTwoFixedSizeWidgetBase
{
    Q_OBJECT
public:
    explicit UserLoginSidebarWidget(QSize expandedSize, QWidget *parent = 0);

    void actualizeUsersList();

protected:
    void resizeEvent(QResizeEvent *e);

private Q_SLOTS:
    void _userIsLoggedIn();
    void _userLoginWasAborted();
    void _userLoginCommunicationError(QString errorMessage);

    void _switchToExpandedStateFinished();
    void _switchToCollapsedStateFinished();

private:
    UserLoginWidget3 *_userLoginWidget;
    bool _isUserLoginWasSuccessful;
};

#endif // USERLOGINSIDEBARWIDGET_H
