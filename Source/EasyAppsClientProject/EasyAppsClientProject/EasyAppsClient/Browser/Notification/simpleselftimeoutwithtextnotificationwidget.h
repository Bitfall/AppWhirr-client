#ifndef SIMPLESELFTIMEOUTWITHTEXTNOTIFICATIONWIDGET_H
#define SIMPLESELFTIMEOUTWITHTEXTNOTIFICATIONWIDGET_H

#include "Browser/Notification/selftimeoutingnotificationwidgetbase.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QTimer;
QT_END_NAMESPACE

class SimpleSelfTimeoutWithTextNotificationWidget : public SelfTimeoutingNotificationWidgetBase
{
    Q_OBJECT
public:
    explicit SimpleSelfTimeoutWithTextNotificationWidget(QString text, QString optionalNotificationId = QString(), QWidget *parent = 0);

    void setText(QString text);

public Q_SLOTS:
    void setFinished();
    void finishAfterTimeout(quint32 timeoutIntervallMilliseconds);

private:
    QLabel *_textLabel;
    QTimer *_finishTimer;
};

#endif // SIMPLESELFTIMEOUTWITHTEXTNOTIFICATIONWIDGET_H
