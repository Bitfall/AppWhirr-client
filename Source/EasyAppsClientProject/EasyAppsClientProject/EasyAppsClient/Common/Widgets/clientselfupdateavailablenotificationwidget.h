#ifndef CLIENTSELFUPDATEAVAILABLENOTIFICATIONWIDGET_H
#define CLIENTSELFUPDATEAVAILABLENOTIFICATIONWIDGET_H

#include "Browser/Notification/selftimeoutingnotificationwidgetbase.h"

class ClientSelfUpdateAvailableNotificationWidget : public SelfTimeoutingNotificationWidgetBase
{
    Q_OBJECT
public:
    explicit ClientSelfUpdateAvailableNotificationWidget(QWidget *parent = 0);

Q_SIGNALS:
    void startClientSelfUpdate();

private Q_SLOTS:
    void _doItButtonClicked();
};

#endif // CLIENTSELFUPDATEAVAILABLENOTIFICATIONWIDGET_H
