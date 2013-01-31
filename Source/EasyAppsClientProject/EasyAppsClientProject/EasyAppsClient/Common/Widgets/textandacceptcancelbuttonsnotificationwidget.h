#ifndef TEXTANDACCEPTCANCELBUTTONSNOTIFICATIONWIDGET_H
#define TEXTANDACCEPTCANCELBUTTONSNOTIFICATIONWIDGET_H

#include "Browser/Notification/selftimeoutingnotificationwidgetbase.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class TextAndAcceptCancelButtonsNotificationWidget : public SelfTimeoutingNotificationWidgetBase
{
    Q_OBJECT
public:
    explicit TextAndAcceptCancelButtonsNotificationWidget(QString titleText, QString acceptButtonText, QString cancelButtonText, bool isRequestImmediateUpdateWhenAcceptOrCancelFired, QWidget *parent = 0);

Q_SIGNALS:
    void acceptOptionSelected();
    void cancelOptionSelected();

private Q_SLOTS:
    void _acceptButtonClicked();
    void _cancelButtonClicked();

private:
    bool _isRequestImmediateUpdateWhenAcceptOrCancelFired;
};

#endif // TEXTANDACCEPTCANCELBUTTONSNOTIFICATIONWIDGET_H
