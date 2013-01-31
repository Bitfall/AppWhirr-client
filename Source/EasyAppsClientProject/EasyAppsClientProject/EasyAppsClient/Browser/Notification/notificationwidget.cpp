#include "notificationwidget.h"

#include <QTimer>
#include <QHBoxLayout>

#include <QStyleOption>
#include <QPainter>

#include <QResizeEvent>
#include <QPaintEvent>

// configuration
#include "Settings/guiconfigurations.h"
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

// debug and utility
#include <QDebug>

NotificationWidget::NotificationWidget(QString notificationItemId, QString printableExplanationOfNotification, QWidget *parent) :
    QWidget(parent),
    isTimeouted(false),
    _contentWidget(NULL),
    _printableExplanationOfNotification(printableExplanationOfNotification),
    _notificationItemId(notificationItemId)
{
    theLayout = new QHBoxLayout;
//    theLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    this->setLayout(theLayout);

    this->setContentsMargins(0, 0, 0, 0);
    theLayout->setContentsMargins(0, 0, 0, 0);
    theLayout->setSpacing(0);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
    this->setFixedHeight(guiConfManager->getIntForId("Notification/Widget/fixedHeight"));
    this->setStyleSheet(guiConfManager->getStyleForId("Notification/Widget/Default/style"));
//    this->setMinimumWidth(200); // only for tests and to visually appear
}

void NotificationWidget::activateTheNotification(int timeoutMilliseconds)
{
    this->isTimeouted = false;

    if(timeoutMilliseconds > 0) {
        QTimer *timoutTimer = new QTimer(this);
        timoutTimer->singleShot(timeoutMilliseconds, this, SLOT(_timoutHappend()));
    }
}

void NotificationWidget::resizeEvent(QResizeEvent *re) {
//    DLog("--- notif widget resize: ") << re->size();
#if 0
    qDebug() << " .,. notif-widget resize" << this->geometry();
    QWidget::resizeEvent(re);
    this->contentWidget->updateGeometry();
    this->contentWidget->adjustSize();
    this->theLayout->update();

    qDebug() << " .,. notif-widget-content geom" << this->contentWidget->geometry();
#else
    QWidget::resizeEvent(re);
#endif
}

void NotificationWidget::paintEvent(QPaintEvent *e)
{
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QString NotificationWidget::getPrintableExplanationOfNotification() const {
    return this->_printableExplanationOfNotification;
}

QString NotificationWidget::getNotificationItemId() const {
    return this->_notificationItemId;
}

QWidget *NotificationWidget::getContentWidget() const {
    return this->_contentWidget;
}

void NotificationWidget::setContentWidget(QWidget *contentWidget)
{
    if(this->_contentWidget != NULL) {
        // remove the previous one
        theLayout->removeWidget(this->_contentWidget);
    }

    this->_contentWidget = contentWidget;
    this->_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->theLayout->addWidget(this->_contentWidget);
}

void NotificationWidget::_timoutHappend()
{
    this->isTimeouted = true;
}

bool NotificationWidget::getIsTimeouted() const {
    return this->isTimeouted;
}
