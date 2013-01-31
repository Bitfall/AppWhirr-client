#include "clientselfupdateavailablenotificationwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

// config
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

ClientSelfUpdateAvailableNotificationWidget::ClientSelfUpdateAvailableNotificationWidget(QWidget *parent) :
    SelfTimeoutingNotificationWidgetBase(parent)
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
    this->setStyleSheet(guiConfManager->getStyleForId("Notification/Widget/ClientSelfUpdateAvailable/style"));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(5, 0, 5, 0);

    QLabel *titleLabel = new QLabel;
    titleLabel->setText(tr("New AppWhirr version is available. Get it!"));
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(titleLabel);

    // some spacing
    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    QPushButton *doItButton = new QPushButton;
    doItButton->setText(tr("Start update!"));
    doItButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    doItButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/NormalPriority/style"));
    doItButton->setFlat(true);
    doItButton->setFixedHeight(26);
    connect(doItButton, SIGNAL(clicked()), this, SLOT(_doItButtonClicked()));
    mainLayout->addWidget(doItButton);
}

void ClientSelfUpdateAvailableNotificationWidget::_doItButtonClicked() {
    Q_EMIT startClientSelfUpdate();
    Q_EMIT __notificationFinished();
}
