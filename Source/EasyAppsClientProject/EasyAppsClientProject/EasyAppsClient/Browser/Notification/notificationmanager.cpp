#include "notificationmanager.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

#include "notificationwidget.h"
#include "selftimeoutingnotificationwidgetbase.h"

// custom dynamic layout
#include "Common/Layouts/dynamicsamesizeverticallinelayout.h"
#include "Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatebase.h"
#include "Common/Sorting/widgetlistsnapshothandlerdelegate.h"

// custom widgets
#include "Common/Widgets/delegatingwidget.h"

// debug and utility
#include <QVariant>
#include <QPropertyAnimation>
#include <QResizeEvent>

// configurations
#include "Settings/configmanager.h"
#include "Settings/guiconfigurations.h"
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

NotificationManager::NotificationManager(QObject *parent) :
    QObject(parent),

    _notificationArea(NULL),
    _notificationLayout(NULL),
    _isNotificationsHidden(true),
    _previousNotificationAreaSize(0),
    _currFirstVisibleWidgetIndex(0),
    _currentNotificationAreaFixedHeightPropertyValue(0)
{
    // notif manager update timer
    {
        this->_updateTimer = new QTimer(this);
        this->_updateTimer->setInterval(ConfigManager::getNotificationManagerUpdateIntervalMilliseconds());
        connect(this->_updateTimer, SIGNAL(timeout()), this, SLOT(updateManager()));
        this->_updateTimer->stop();
    }

    // notif area size change animation
    {
        _notificationAreaSizeChangerAnimation = new QPropertyAnimation(this, "notificationAreaFixedHeight", this);
        _notificationAreaSizeChangerAnimation->setDuration(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getIntForId("Notification/SnapshotChange/AreaSizeChangeAnimation/duration"));
        _notificationAreaSizeChangerAnimation->setStartValue(0);
        _notificationAreaSizeChangerAnimation->setEndValue(100);
        _notificationAreaSizeChangerAnimation->setEasingCurve(QEasingCurve::InOutExpo);
    }

    this->createNotificationArea();
    this->resetOrderUpdateCycles();
}

void NotificationManager::createNotificationArea()
{
    // create the notification-area layout and initialize it

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
//    this->notificationLayout = new QVBoxLayout;
    SameSizeLayoutSnapshotChangeDelegateBase *snapshotChangedDelegate = new SameSizeLayoutSnapshotChangeDelegateBase;
    connect(snapshotChangedDelegate, SIGNAL(layoutChangeFinished()), this, SLOT(updateTheLayout()));
    connect(snapshotChangedDelegate, SIGNAL(itemHiddenAndSafeToRemove(QWidget*)),
            this, SLOT(itemHiddenAndSafeToRemove(QWidget*))); // used for deleting timeouted items
    WidgetListSnapshotHandlerDelegate *snapshotHandlerDelegate = new WidgetListSnapshotHandlerDelegate;
    this->_notificationLayout = new DynamicSameSizeVerticalLineLayout(snapshotHandlerDelegate, snapshotChangedDelegate,
                                                                      QSize(-1, guiConfManager->getIntForId("Notification/Widget/fixedHeight"))); // stretch in width
    this->_notificationLayout->getSnapshotChangeDelegate()->setMoveAnimationDuration(guiConfManager->getIntForId("Notification/SnapshotChange/MoveAnimation/duration"));
    this->_notificationLayout->getSnapshotChangeDelegate()->setShowHideAnimationDuration(guiConfManager->getIntForId("Notification/SnapshotChange/ShowHideAnimation/duration"));
//    this->notificationLayout->setSizeConstraint(QLayout::SetMinAndMaxSize); // ! don't do this unless you want to see same "fancy" flickering

    // create the notification-area widget, initialize it, and connect it to the layout
    this->_notificationArea = new DelegatingWidget;
    connect(this->_notificationArea, SIGNAL(resizeHappened(QResizeEvent*)), this, SLOT(_notificationAreaResized(QResizeEvent*)));
    this->_notificationArea->setLayout(this->_notificationLayout);
    this->_notificationArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->_notificationArea->setFixedHeight(0);
    this->_notificationArea->setContentsMargins(0, 0, 0, 0);
    this->_notificationLayout->setContentsMargins(0, 0, 0, 0);
//    this->_notificationLayout->setDynamicContentMargins(0, 1, 0, 0);
    this->_notificationLayout->setDynamicContentMargins(guiConfManager->getMarginsForId("Notification/Area/contentMargins"));
    this->_notificationLayout->setSpacing(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getIntForId("Notification/Area/spacingBetweenItems"));
//    this->_notificationLayout->setSpacing(0);

    _notificationLayout->setGeometry(this->_notificationArea->geometry()); // !!! for some strange reason this is required - if it's not here then the notification area can be 0 sized when the first notification arrive

    // set the syle of the notification area
    QString notificationAreaCssId = "notif-area";
    this->_notificationArea->setProperty("cssId", QVariant(notificationAreaCssId));
    this->_notificationArea->setStyleSheet(guiConfManager->getStyleForId("Notification/Area/style"));
}

void NotificationManager::updateTheLayout()
{
//    this->notificationArea->update();
//    DLog("--- Area size: ") << this->_notificationArea->size();
}

void NotificationManager::_notificationAreaResized(QResizeEvent *e)
{
//    DLog("--- notif area - delegate resize: ") << e->size();

//    this->notificationLayout->setGeometry(this->notificationArea->rect());

//    int visibleItemCnt = currentlyVisibleNotificationSnapshot.getOrderedItemCount();
//    for(int i = 0; i < visibleItemCnt; i++) {
//        currentlyVisibleNotificationSnapshot.getOrderedItems().at(i)->setMaximumWidth(e->size().width());
//    }
}

// Q_SLOT
void NotificationManager::updateManager()
{
//    DLog("Notif Manager Update");
    // check for timeouted notifications
    QList<NotificationWidget *> timeoutedNotifWidgets;
    int notificationWidgetCnt = this->_notificationWidgets.size();
    for(int i = 0; i < notificationWidgetCnt; i++)
    {
        NotificationWidget *notifWidget = this->_notificationWidgets[i];
        if(notifWidget->getIsTimeouted()) {
            // timeouted
//            DLogS << "Widget timeouted";
            timeoutedNotifWidgets << notifWidget;
            _currDeletingTimeoutedNotifWidgets << notifWidget;

            if(i < _currFirstVisibleWidgetIndex) {
                // items removed before the first visible item -> decrement the first-visible item index to point it to the right index
                --_currFirstVisibleWidgetIndex;
            }
        }
    }

    // remove the timeouted notifications
    int timeoutedNotifWidgetCnt = timeoutedNotifWidgets.size();
    for(int i = 0; i < timeoutedNotifWidgetCnt; i++)
    {
        NotificationWidget *timeoutedNotifWidget = timeoutedNotifWidgets[i];
        // remove from the inner list
        this->_notificationWidgets.removeOne(timeoutedNotifWidget);
    }
    // notificationWidgets is actualized, free to use as current from here


    // update the gui
    int maxCnt = qMin(this->_notificationWidgets.size(), ConfigManager::getNotificationManagerMaximumVisibleNotificationItemCount());
    this->_currentlyVisibleNotificationSnapshot.clearAll(); // clear
    for(int i = 0; i < maxCnt; i++) {
        int indexOfWidgetWillBeVisible = (_currFirstVisibleWidgetIndex + i) % this->_notificationWidgets.size();
//        DLogS << " make it visible: " << indexOfWidgetWillBeVisible;
        this->_currentlyVisibleNotificationSnapshot.addItem( this->_notificationWidgets[indexOfWidgetWillBeVisible] );
    }
    this->_notificationLayout->getSnapshotHandlerDelegate()->commitSnapshot(this->_currentlyVisibleNotificationSnapshot);
    if(this->_notificationWidgets.size() > ConfigManager::getNotificationManagerMaximumVisibleNotificationItemCount()) {
        // there are more notifications in the system than the maximum visible
        // so rotate them
        if(this->_orderUpdateCyclesLeft <= 0) {
            this->rotateFirstVisibleNotificationIndexInQueue();
            this->resetOrderUpdateCycles();
        }
        else {
            this->_orderUpdateCyclesLeft--;
        }
    }

    this->refreshNotificationAreaHeight();

    // if there's no more active notification to manage then go to sleep
    if(this->_notificationWidgets.size() <= 0) {
        this->deactivateManager();
    }
}

void NotificationManager::rotateFirstVisibleNotificationIndexInQueue()
{
    ++_currFirstVisibleWidgetIndex;
//    _isNotificationOrderChanged = true;
}

void NotificationManager::hideNotifications() {
    this->_isNotificationsHidden = true;
    this->refreshNotificationAreaHeight();
}

void NotificationManager::showNofitications() {
    this->_isNotificationsHidden = false;
    this->refreshNotificationAreaHeight();
}

void NotificationManager::refreshNotificationAreaHeight() {
    int itemCntToCalculateWith = this->_currentlyVisibleNotificationSnapshot.getOrderedItemCount();
//    int currDesiredNotificationAreaHeight = itemCntToCalculateWith * GuiConfigurations::browserWindowNotificationWidgetHeight();
    int currDesiredNotificationAreaHeight = 0;
    if(itemCntToCalculateWith > 0 && !_isNotificationsHidden) {
        currDesiredNotificationAreaHeight = this->_notificationLayout->sizeHint().height();
    }

    if(currDesiredNotificationAreaHeight != _previousNotificationAreaSize) {
        // the size is change, so resize it
        _previousNotificationAreaSize = currDesiredNotificationAreaHeight;

        // stop if it's in animation
        _notificationAreaSizeChangerAnimation->stop();

        _notificationAreaSizeChangerAnimation->setStartValue(this->_notificationArea->maximumHeight());
        _notificationAreaSizeChangerAnimation->setEndValue(currDesiredNotificationAreaHeight);

        _notificationAreaSizeChangerAnimation->start();
    }
}

// Q_SLOT
void NotificationManager::itemHiddenAndSafeToRemove(QWidget *widget) {
    int currDeletingItemCnt = this->_currDeletingTimeoutedNotifWidgets.size();
    bool isFound = false;
    for(int i = 0; i < currDeletingItemCnt && !isFound; i++) {
        if(_currDeletingTimeoutedNotifWidgets[i] == widget) {
            isFound = true;
        }
    }

    if(isFound) {
        // remove from layout -> not necessary - this method is called by the layout

        // remove from the tmp "deleting" collection
        NotificationWidget *castedWidget = dynamic_cast<NotificationWidget *>(widget);
        if(castedWidget != NULL) {
            this->_currDeletingTimeoutedNotifWidgets.removeOne(castedWidget);

            // and delete the notification widget
            AW_QOBJECT_SAFE_DELETE(widget);
        }
        else {
            WLog("Cannot cast the widget to the desired type. This should never happen!");
        }
    }
    else {
//        DLog("Item not found int _currDeletingTimeoutedNotifWidgets. It's only temporarily swapped-out from the layout.");
    }
}

void NotificationManager::addSimpleTextNotificationWithTimeout(QString theNotificationText, int timeoutMilliseconds, NotificationPriority priority, QString printableExplanationOfNotification, QString notificationItemId)
{
//    DLog("Tmp off");
    QString explanationOfNotif = theNotificationText;
    if(!printableExplanationOfNotification.isEmpty()) {
        explanationOfNotif = printableExplanationOfNotification;
    }
//    DLog("Printable expl.: ") << explanationOfNotif;
    NotificationWidget *notifWidget = new NotificationWidget(notificationItemId, explanationOfNotif);
    notifWidget->setObjectName("notifWidget");

    switch(priority) {
    case LowPriority:
        notifWidget->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/SimpleText/LowPriority/style"));
        break;
    case NormalPriority:
        notifWidget->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/SimpleText/NormalPriority/style"));
        break;
    case HighPriority:
        notifWidget->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/SimpleText/HighPriority/style"));
        break;
    case ErrorPriority:
        notifWidget->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/SimpleText/CriticalPriority/style"));
        break;

    default:
        DLog("Unknown priority. Normal is applied.");
        notifWidget->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/SimpleText/NormalPriority/style"));
    }

    QLabel *notifTextLable = new QLabel(theNotificationText);
    notifTextLable->setContentsMargins(5, 0, 5, 0);
    notifWidget->setContentWidget(notifTextLable);

    this->addNotification(notifWidget, timeoutMilliseconds, true);
}

void NotificationManager::addDefaultErrorMessageNotificationWithTimeout(QString notificationText, QString printableExplanationOfNotification, QString notificationItemId)
{
    this->addSimpleTextNotificationWithTimeout(notificationText, 15000, NotificationManager::ErrorPriority, printableExplanationOfNotification, notificationItemId);
}

void NotificationManager::addDefaultSuccessMessageNotificationWithTimeout(QString notificationText, QString printableExplanationOfNotification, QString notificationItemId)
{
    this->addSimpleTextNotificationWithTimeout(notificationText, 5000, NotificationManager::NormalPriority, printableExplanationOfNotification, notificationItemId);
}

//void NotificationManager::addNotificationWithCustomWidgetAndTimeout(QWidget *customWidget, QString printableExplanationOfNotification, int timeoutMilliseconds, QString notificationItemId)
//{
//    NotificationWidget *notifWidget = new NotificationWidget(notificationItemId, printableExplanationOfNotification);
//    notifWidget->setContentWidget(customWidget);

//    this->addNotification(notifWidget, timeoutMilliseconds, true);
//}

void NotificationManager::addNotificationWithSelfTimeoutingCustomWidget(SelfTimeoutingNotificationWidgetBase *customSelfTimeoutingWidget, QString printableExplanationOfNotification, QString notificationItemId, bool isUniqueById)
{
    if(isUniqueById)
    {
        NotificationWidget *relatedAppInstallNotificationWidget = this->getNotificationWidgetByItemId(notificationItemId);
        if(relatedAppInstallNotificationWidget != NULL) {
            DLog("Notification widget with this ID found, and isUniqueById is true - won't add this notification widget.");
            return;
        }
    }

    NotificationWidget *notifWidget = new NotificationWidget(notificationItemId, printableExplanationOfNotification);
    notifWidget->setContentWidget(customSelfTimeoutingWidget);

    // and connect the self-timeouting signal to the notification-widget
    connect(customSelfTimeoutingWidget, SIGNAL(__notificationFinished()), notifWidget, SLOT(_timoutHappend()));
    connect(customSelfTimeoutingWidget, SIGNAL(__updateNotificationsRequest()), this, SLOT(updateManager()));

    this->addNotification(notifWidget, -1, true);
}

void NotificationManager::addNotification(NotificationWidget *notificationWidget, int timeoutMilliseconds, bool isShowItImmediatelly)
{
    DLog("New notification added: ") << notificationWidget->getPrintableExplanationOfNotification();
    // TMP - debug
    notificationWidget->setObjectName(notificationWidget->getNotificationItemId());

    this->_notificationWidgets << notificationWidget;

    notificationWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    notificationWidget->setFixedHeight(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getIntForId("Notification/Widget/fixedHeight"));
    QWidget *notifContentWidget = notificationWidget->getContentWidget();
    if(notifContentWidget != NULL) {
        notifContentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        notifContentWidget->setFixedHeight(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getIntForId("Notification/Widget/fixedHeight"));
    }

    this->_notificationLayout->addWidget(notificationWidget);
    notificationWidget->hide(); // hide it, and allow the delegates to manage it's visibility

    notificationWidget->activateTheNotification(timeoutMilliseconds);


    if(isShowItImmediatelly) {
        this->spinNotificationsToEnsureThisIndexIsVisible(this->_notificationWidgets.size() - 1); // ensure it will be visible

        // stop the timer
        this->_updateTimer->stop();
        // fire the update
        this->updateManager();
        // restart the timer
        this->_updateTimer->start();
    }

    this->activateManager();
}

void NotificationManager::spinNotificationsToEnsureThisIndexIsVisible(int index)
{
    this->resetOrderUpdateCycles(); // we change the order here, so an auto-order update is not required

    if(index < ConfigManager::getNotificationManagerMaximumVisibleNotificationItemCount()) {
        this->_currFirstVisibleWidgetIndex = 0;
    }
    else {
        this->_currFirstVisibleWidgetIndex = qAbs((index + 1 - ConfigManager::getNotificationManagerMaximumVisibleNotificationItemCount())) % this->_notificationWidgets.size();
    }
//    DLogS << " _currFirstVisibleWidgetIndex: " << this->_currFirstVisibleWidgetIndex;
}

void NotificationManager::activateManager()
{
    if(!_updateTimer->isActive()) {
        // activate the manager if it's not active already
        _updateTimer->start();

        DLogS << " NotifManager - activated";
    }
}

void NotificationManager::deactivateManager()
{
    if(_updateTimer->isActive()) {
        // deactivate the manager if it's not deactivated already
        _updateTimer->stop();

        DLogS << " NotifManager - deactivated";
    }
}

QWidget *NotificationManager::_getNotificationAreaWidget() const {
    return this->_notificationArea;
}

NotificationWidget *NotificationManager::getNotificationWidgetByItemId(QString notificationItemId) const
{
    NotificationWidget *_ret = NULL;

    int notifWidgetCnt = this->_notificationWidgets.size();
    bool isFound = false;
    for(int i = 0; i < notifWidgetCnt && !isFound; i++) {
        if(this->_notificationWidgets[i]->getNotificationItemId() == notificationItemId) {
            isFound = true;
            _ret = this->_notificationWidgets[i];
        }
    }

    return _ret;
}

float NotificationManager::getNotificationAreaFixedHeight() const {
    return this->_currentNotificationAreaFixedHeightPropertyValue;
}

void NotificationManager::setNotificationAreaFixedHeight(float value) {
    int intValue = value;
    if(intValue < 0) {
        intValue = 0;
    }

    this->_currentNotificationAreaFixedHeightPropertyValue = intValue;
    this->_notificationArea->setFixedHeight(intValue);
}

void NotificationManager::resetOrderUpdateCycles()
{
    this->_orderUpdateCyclesLeft = ConfigManager::getNotificationManagerOrderUpdateCycles();
}
