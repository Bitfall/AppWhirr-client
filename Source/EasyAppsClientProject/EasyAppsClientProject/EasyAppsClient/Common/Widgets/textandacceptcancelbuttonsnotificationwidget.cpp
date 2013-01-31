#include "textandacceptcancelbuttonsnotificationwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

// config
#include "Settings/guiconfigurations.h"
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

TextAndAcceptCancelButtonsNotificationWidget::TextAndAcceptCancelButtonsNotificationWidget(QString titleText, QString acceptButtonText, QString cancelButtonText, bool isRequestImmediateUpdateWhenAcceptOrCancelFired, QWidget *parent) :
    SelfTimeoutingNotificationWidgetBase(parent),
    _isRequestImmediateUpdateWhenAcceptOrCancelFired(isRequestImmediateUpdateWhenAcceptOrCancelFired)
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(5, 0, 5, 0);

    QLabel *titleLabel = new QLabel;
    titleLabel->setText(titleText);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(titleLabel);

    // some spacing
    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    QPushButton *acceptButton = new QPushButton;
    acceptButton->setText(acceptButtonText);
    acceptButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    acceptButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/NormalPriority/style"));
    acceptButton->setFlat(true); // required by Mac for buttons with style-sheet
    acceptButton->setFixedHeight(26);
    connect(acceptButton, SIGNAL(clicked()), this, SLOT(_acceptButtonClicked()));
    mainLayout->addWidget(acceptButton);

    QPushButton *cancelButton = new QPushButton;
    cancelButton->setText(cancelButtonText);
    cancelButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    cancelButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/NormalPriority/style"));
    cancelButton->setFlat(true); // required by Mac for buttons with style-sheet
    cancelButton->setFixedHeight(26);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(_cancelButtonClicked()));
    mainLayout->addWidget(cancelButton);
}

void TextAndAcceptCancelButtonsNotificationWidget::_acceptButtonClicked()
{
    Q_EMIT acceptOptionSelected();
    Q_EMIT this->__notificationFinished();
    if(_isRequestImmediateUpdateWhenAcceptOrCancelFired) {
        Q_EMIT __updateNotificationsRequest();
    }
}

void TextAndAcceptCancelButtonsNotificationWidget::_cancelButtonClicked()
{
    Q_EMIT cancelOptionSelected();
    Q_EMIT this->__notificationFinished();
    if(_isRequestImmediateUpdateWhenAcceptOrCancelFired) {
        Q_EMIT __updateNotificationsRequest();
    }
}
