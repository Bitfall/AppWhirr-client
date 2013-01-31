#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QHBoxLayout;
class QResizeEvent;
QT_END_NAMESPACE

// configurations
#include "Settings/configmanager.h"

class NotificationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationWidget(QString notificationItemId, QString printableExplanationOfNotification, QWidget *parent = 0);

    //
    // if timeoutMilliseconds < 0 then it will never timeout - will be never deactivated, so you have to do it manually
    void activateTheNotification(int timeoutMilliseconds = ConfigManager::getNotificationWidgetDefaultTimeoutMilliseconds());

    void setContentWidget(QWidget *_contentWidget);
//    void deactivate();

    bool getIsTimeouted() const;

    //
    // it will be stored in notification-history. It's like a log entry, like: "downloading error for appId: 12, message: zip not found"
    //  This entry can be stored in the log or in history, and it's aim is to provide a "lightweight" logging, have to be specified!
    // This won't be presented automatically when the notification appears, it's more like a log entry, can provide more information
    //  then what's presented in the notification.
    QString getPrintableExplanationOfNotification() const;

    QString getNotificationItemId() const;

    QWidget *getContentWidget() const;

public Q_SLOTS:
    void _timoutHappend();

protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    bool isTimeouted;
    QWidget *_contentWidget;
    QHBoxLayout *theLayout;
    QString _printableExplanationOfNotification;
    QString _notificationItemId;
};

#endif // NOTIFICATIONWIDGET_H
