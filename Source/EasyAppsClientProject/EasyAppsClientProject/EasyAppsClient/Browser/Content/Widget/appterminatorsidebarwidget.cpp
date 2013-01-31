#include "appterminatorsidebarwidget.h"

#include <QHBoxLayout>

#include "Common/Application/applicationterminator.h"

// authorization
#include "Common/Security/authorization.h"

AppTerminatorSidebarWidget::AppTerminatorSidebarWidget(QSize expandedSize, QWidget *parent) :
    AnimatedTwoFixedSizeWidgetBase(1000, QSize(0, 0), expandedSize, false, false, true, parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // left spacer
//    mainLayout->addSpacerItem(new QSpacerItem(50, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _applicationTerminatorWidget = new ApplicationTerminator;
    connect(_applicationTerminatorWidget, SIGNAL(startTermination()), this, SLOT(_startTerminationSlot()));
    connect(_applicationTerminatorWidget, SIGNAL(abortTermination()), this, SLOT(_abortTerminationSlot()));
    mainLayout->addWidget(_applicationTerminatorWidget);
}

void AppTerminatorSidebarWidget::actualizeAndShow(QString appId, QString appName, AppTerminationController::TerminationReasonCodeEnum terminationReasonCode)
{
    this->_applicationTerminatorWidget->actualize(appId, appName, terminationReasonCode);
    this->switchToExpandedStateSlot();
}

void AppTerminatorSidebarWidget::hideWithAnimation()
{
    this->switchToCollapsedStateSlot();
}

void AppTerminatorSidebarWidget::_abortTerminationSlot()
{
    this->switchToCollapsedStateSlot();
    Q_EMIT abortTermination();
}

void AppTerminatorSidebarWidget::_startTerminationSlot()
{
    this->switchToCollapsedStateSlot();
    Q_EMIT startTermination();
}
