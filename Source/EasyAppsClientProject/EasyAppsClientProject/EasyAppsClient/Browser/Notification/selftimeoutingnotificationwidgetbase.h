#ifndef SELFTIMEOUTINGNOTIFICATIONWIDGETBASE_H
#define SELFTIMEOUTINGNOTIFICATIONWIDGETBASE_H

#include <QWidget>

class SelfTimeoutingNotificationWidgetBase : public QWidget {
    Q_OBJECT

public:
    explicit SelfTimeoutingNotificationWidgetBase(QWidget *parent = 0) : QWidget(parent) {}

    //
    // This Id is totally optional. It's only a utility to allow to you to implement some mapping if more than 1 self-timeouting notification widget is active.
    //  Won't be used internally.
    void setOptionalId(QString id);
    QString getOptionalId() const;

Q_SIGNALS:
    /*! Emit this signal, when this item is finished
    */
    void __notificationFinished();

    /*! Emit this signal to indicate something changed which requires an immediate notification-area update

        For example if a button clicked which results an immediate 'timeout'/'finished' and it's important to handle it immediately and not in the next update-period.
    */
    void __updateNotificationsRequest();

protected:
    void paintEvent(QPaintEvent *e);

private:
    QString _optionalId;
};

#endif // SELFTIMEOUTINGNOTIFICATIONWIDGETBASE_H
