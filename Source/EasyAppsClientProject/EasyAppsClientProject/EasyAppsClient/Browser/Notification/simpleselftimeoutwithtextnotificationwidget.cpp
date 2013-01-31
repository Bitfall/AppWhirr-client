#include "simpleselftimeoutwithtextnotificationwidget.h"

#include <QHBoxLayout>
#include <QLabel>

#include <QTimer>

SimpleSelfTimeoutWithTextNotificationWidget::SimpleSelfTimeoutWithTextNotificationWidget(QString text, QString optionalNotificationId, QWidget *parent) :
    SelfTimeoutingNotificationWidgetBase(parent),
    _finishTimer(NULL)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(5, 0, 5, 0);

    _textLabel = new QLabel(text);
    mainLayout->addWidget(_textLabel);

    this->setOptionalId(optionalNotificationId);
}

void SimpleSelfTimeoutWithTextNotificationWidget::setFinished()
{
    Q_EMIT __notificationFinished();
}

void SimpleSelfTimeoutWithTextNotificationWidget::setText(QString text)
{
    this->_textLabel->setText(text);
}

void SimpleSelfTimeoutWithTextNotificationWidget::finishAfterTimeout(quint32 timeoutIntervallMilliseconds)
{
    if(_finishTimer != NULL) {
        DLog("Timer already started.");
        return;
    }

    _finishTimer = new QTimer(this);
    _finishTimer->setInterval(timeoutIntervallMilliseconds);
    _finishTimer->setSingleShot(true);
    connect(_finishTimer, SIGNAL(timeout()), this, SIGNAL(__notificationFinished()));
    _finishTimer->start();
}
