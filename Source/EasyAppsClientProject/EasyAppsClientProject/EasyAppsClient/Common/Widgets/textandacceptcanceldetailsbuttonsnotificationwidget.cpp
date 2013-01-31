#include "textandacceptcanceldetailsbuttonsnotificationwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <QResizeEvent>

// config
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"
#include "Application/globalobjectregistry.h"

TextAndAcceptCancelDetailsButtonsNotificationWidget::TextAndAcceptCancelDetailsButtonsNotificationWidget(QString titleText, QString acceptButtonText, QString cancelButtonText, QString detailsButtonText, bool isFinishNotificationWhenDetailsSelected, bool isRequestImmediateUpdateWhenAcceptCancelOrDetailesFired, QWidget *parent) :
    SelfTimeoutingNotificationWidgetBase(parent),
    _isFinishNotificationWhenDetailsSelected(isFinishNotificationWhenDetailsSelected),
    _isRequestImmediateUpdateWhenAcceptCancelOrDetailesFired(isRequestImmediateUpdateWhenAcceptCancelOrDetailesFired)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

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

    QPushButton *detailsButton = new QPushButton;
    detailsButton->setText(detailsButtonText);
    detailsButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    detailsButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    detailsButton->setFixedHeight(26);
    detailsButton->setFlat(true); // required by Mac for buttons with style-sheet
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(_detailsButtonClicked()));
    mainLayout->addWidget(detailsButton);
}

void TextAndAcceptCancelDetailsButtonsNotificationWidget::resizeEvent(QResizeEvent *e)
{
//    DLog("--- size: ") << e->size();
    SelfTimeoutingNotificationWidgetBase::resizeEvent(e);
}

void TextAndAcceptCancelDetailsButtonsNotificationWidget::_acceptButtonClicked()
{
    Q_EMIT this->__notificationFinished();
    Q_EMIT acceptOptionSelected();
    if(_isRequestImmediateUpdateWhenAcceptCancelOrDetailesFired) {
        Q_EMIT __updateNotificationsRequest();
    }
}

void TextAndAcceptCancelDetailsButtonsNotificationWidget::_cancelButtonClicked()
{
    Q_EMIT this->__notificationFinished();
    Q_EMIT cancelOptionSelected();
    if(_isRequestImmediateUpdateWhenAcceptCancelOrDetailesFired) {
        Q_EMIT __updateNotificationsRequest();
    }
}

void TextAndAcceptCancelDetailsButtonsNotificationWidget::_detailsButtonClicked()
{
    if(this->_isFinishNotificationWhenDetailsSelected) {
        Q_EMIT this->__notificationFinished();
    }
    Q_EMIT detailsOptionSelected();
    if(_isRequestImmediateUpdateWhenAcceptCancelOrDetailesFired) {
        Q_EMIT __updateNotificationsRequest();
    }
}
