#include "plainerrorpresenterwidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QSpacerItem>

// config
#include "Common/guiconfigurationmanager.h"
#include "Application/globalobjectregistry.h"

PlainErrorPresenterWidget::PlainErrorPresenterWidget(QWidget *parent) :
    QWidget(parent)
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _errorMessageLabel = new QLabel;
    _errorMessageLabel->setAlignment(Qt::AlignHCenter);
    _errorMessageLabel->setStyleSheet(guiConfManager->getStyleForId("Label/ExtraLarge/DarkGreen/style"));
    mainLayout->addWidget(_errorMessageLabel);

    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
}


void PlainErrorPresenterWidget::setErrorMessage(QString errorMessage) {
    _errorMessageLabel->setText(errorMessage);
}

void PlainErrorPresenterWidget::setErrorMessageByDefaultMessageEnum(DefaultErrorMessagesEnum defaultErrorMessage) {
    if(defaultErrorMessage == DEM_ServerUnavailable) {
        _errorMessageLabel->setText(tr("Cannot reach the server. Oh my..."));
    }
    else {
        _errorMessageLabel->setText(tr("Unknown error."));
        WLog("Invalid error-message-enum");
    }
}
