#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QList>

#include "Common/Sorting/widgetlistsnapshot.h"

QT_BEGIN_NAMESPACE
class QWidget;
class QVBoxLayout;
class QTimer;
class QPropertyAnimation;
QT_END_NAMESPACE

class NotificationWidget;
class DynamicSameSizeVerticalLineLayout;
class SelfTimeoutingNotificationWidgetBase;

class DelegatingWidget;

class NotificationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float notificationAreaFixedHeight READ getNotificationAreaFixedHeight WRITE setNotificationAreaFixedHeight)

public:
    enum NotificationPriority {
        LowPriority = 0,
        NormalPriority = 1,
        HighPriority = 2,
        ErrorPriority = 3
    };

public:
    explicit NotificationManager(QObject *parent = 0);

    //
    // add notification methods

    //
    // If printableExplanationOfNotification is empty then the notificationText will be used as the printable-explanation
    void addSimpleTextNotificationWithTimeout(QString theNotificationText, int timeoutMilliseconds, NotificationPriority priority = NormalPriority, QString printableExplanationOfNotification = QString(), QString notificationItemId = QString());

    //
    // -- defaults: usually you want to use these
    /*! Add a default timeout based error notification.

        @note Default means default timeout + default priority
    */
    void addDefaultErrorMessageNotificationWithTimeout(QString notificationText, QString printableExplanationOfNotification = QString(), QString notificationItemId = QString());
    /*! Add a default timeout based success notification.

        @note Default means default timeout + default priority
    */
    void addDefaultSuccessMessageNotificationWithTimeout(QString notificationText, QString printableExplanationOfNotification = QString(), QString notificationItemId = QString());
    //
//    void addNotificationWithCustomWidgetAndTimeout(QWidget *customWidget, QString printableExplanationOfNotification, int timeoutMilliseconds, QString notificationItemId = QString());

    /*! Interface method to add self-timeouting notifications

        @param \a isUniqueById : if this is true then the notification will be added only if no other notification with the same \a notificationItemId found.
    */
    void addNotificationWithSelfTimeoutingCustomWidget(SelfTimeoutingNotificationWidgetBase *customSelfTimeoutingWidget, QString printableExplanationOfNotification, QString notificationItemId = QString(), bool isUniqueById = false);

    QWidget *_getNotificationAreaWidget() const;
    NotificationWidget *getNotificationWidgetByItemId(QString notificationItemId) const;

    void hideNotifications();
    void showNofitications();

Q_SIGNALS:
    // !!! used only for inside processing, don't connect to it outside!
    void _sortingHappened(WidgetListSnapshot sortedItems);

private Q_SLOTS:
    /*! Call this slot for an immediate 'whole notification system' update.

        This can be a timeconsuming operation, use it only when you really need it!
    */
    void updateManager();
    void itemHiddenAndSafeToRemove(QWidget *widget);


    void updateTheLayout();
    void _notificationAreaResized(QResizeEvent *e);

private:
    void activateManager();
    void deactivateManager();

    // this is the base add-notification method, every other add method will wrap / call this
    void addNotification(NotificationWidget *notificationWidget, int timeoutMilliseconds, bool isShowItImmediatelly);

    void createNotificationArea();

    void refreshNotificationAreaHeight();

    void rotateFirstVisibleNotificationIndexInQueue();
    void spinNotificationsToEnsureThisIndexIsVisible(int index);

    //
    float getNotificationAreaFixedHeight() const;
    void setNotificationAreaFixedHeight(float value);

    void resetOrderUpdateCycles();

private:
    DelegatingWidget *_notificationArea;
    DynamicSameSizeVerticalLineLayout *_notificationLayout;
//    QVBoxLayout *notificationLayout;
    QList<NotificationWidget *> _notificationWidgets;
    WidgetListSnapshot _currentlyVisibleNotificationSnapshot;

    QTimer *_updateTimer;

    bool _isNotificationsHidden;

    //
    int _previousNotificationAreaSize;
    int _currFirstVisibleWidgetIndex;
    float _currentNotificationAreaFixedHeightPropertyValue;
    QPropertyAnimation *_notificationAreaSizeChangerAnimation;

    int _orderUpdateCyclesLeft; // how many normal update cycles left until an order update
//    bool _isNotificationOrderChanged;

    // don't remove this! required to mark the notif-widgets which are NOT just swapped out from the layout but also marked for delete
    QList<NotificationWidget *> _currDeletingTimeoutedNotifWidgets;
};

#endif // NOTIFICATIONMANAGER_H
