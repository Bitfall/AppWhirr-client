#include "autostartappsnotificationwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <QResizeEvent>

#include <QTimer>

// config
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"
#include "Application/globalobjectregistry.h"

AutostartAppsNotificationWidget::AutostartAppsNotificationWidget(QWidget *parent) :
    SelfTimeoutingNotificationWidgetBase(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(5, 0, 5, 0);

    //
    _titleLabel = new QLabel;
    _titleLabel->setText(tr("Your autostart apps will launch in <b>%1</b> seconds").arg(60));
    _titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(_titleLabel);

    // some spacing
    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    QPushButton *acceptButton = new QPushButton;
    acceptButton->setText(tr("Launch now"));
    acceptButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    acceptButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/NormalPriority/style"));
    acceptButton->setFlat(true); // required by Mac for buttons with style-sheet
    acceptButton->setFixedHeight(26);
    connect(acceptButton, SIGNAL(clicked()), this, SLOT(_runButtonClicked()));
    mainLayout->addWidget(acceptButton);

    QPushButton *cancelButton = new QPushButton;
    cancelButton->setText(tr("Don't launch them"));
    cancelButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    cancelButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/NormalPriority/style"));
    cancelButton->setFlat(true); // required by Mac for buttons with style-sheet
    cancelButton->setFixedHeight(26);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(_abortButtonClicked()));
    mainLayout->addWidget(cancelButton);

    QPushButton *detailsButton = new QPushButton;
    detailsButton->setText(tr("Show them"));
    detailsButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    detailsButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    detailsButton->setFixedHeight(26);
    detailsButton->setFlat(true); // required by Mac for buttons with style-sheet
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(_detailsButtonClicked()));
    mainLayout->addWidget(detailsButton);

    _countdownValue = 60;
    _countdownTimer = new QTimer;
    _countdownTimer->setInterval(1000);
    connect(_countdownTimer, SIGNAL(timeout()), this, SLOT(_countdownTick()));

    connect(this, SIGNAL(__notificationFinished()), _countdownTimer, SLOT(stop()));

    _countdownTimer->start();
}

void AutostartAppsNotificationWidget::resizeEvent(QResizeEvent *e)
{
//    DLog("--- size: ") << e->size();
    SelfTimeoutingNotificationWidgetBase::resizeEvent(e);
}

void AutostartAppsNotificationWidget::_countdownTick()
{
    --_countdownValue;
    if(_countdownValue <= 0) {
        _runButtonClicked();
    }
    else {
        QString secsText = "seconds";
        if(_countdownValue <= 1) {
            secsText = "second";
        }

        _titleLabel->setText(tr("Your autostart apps will launch in <b>%1</b> %2").arg(_countdownValue).arg(secsText));
    }
}

void AutostartAppsNotificationWidget::_runButtonClicked()
{
    Q_EMIT __notificationFinished();
    Q_EMIT runOptionSelected();
    Q_EMIT __updateNotificationsRequest();
}

void AutostartAppsNotificationWidget::_abortButtonClicked()
{
    Q_EMIT __notificationFinished();
    Q_EMIT abortOptionSelected();
    Q_EMIT __updateNotificationsRequest();
}

void AutostartAppsNotificationWidget::_detailsButtonClicked()
{
    Q_EMIT detailsOptionSelected();
}
