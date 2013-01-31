#ifndef AUTOSTARTAPPSNOTIFICATIONWIDGET_H
#define AUTOSTARTAPPSNOTIFICATIONWIDGET_H

#include "Browser/Notification/selftimeoutingnotificationwidgetbase.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QTimer;
QT_END_NAMESPACE

class AutostartAppsNotificationWidget : public SelfTimeoutingNotificationWidgetBase
{
    Q_OBJECT
public:
    explicit AutostartAppsNotificationWidget(QWidget *parent = 0);

Q_SIGNALS:
    void runOptionSelected();
    void abortOptionSelected();
    void detailsOptionSelected();

private Q_SLOTS:
    void _runButtonClicked();
    void _abortButtonClicked();
    void _detailsButtonClicked();

protected:
    void resizeEvent(QResizeEvent *e);

private Q_SLOTS:
    void _countdownTick();

private:
    QLabel *_titleLabel;
    QTimer *_countdownTimer;
    quint32 _countdownValue;
};

#endif // AUTOSTARTAPPSNOTIFICATIONWIDGET_H
