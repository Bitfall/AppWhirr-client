#include "uninstallingview.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>

#include "../guiconfigurationmanagersingleton.h"

UninstallingView::UninstallingView(QWidget *parent) :
    QWidget(parent)
{
    GuiConfigurationManager *guiConf = GuiConfigurationManagerSingleton::sharedGuiConfigurationManager();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QLabel *titleLabel = new QLabel(tr("Uninstalling..."));
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _statusTextEdit = new QTextEdit;
    _statusTextEdit->setReadOnly(true);
    mainLayout->addWidget(_statusTextEdit);

    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    _actionBarWidget = new QWidget;
    QHBoxLayout *actionBarLayout = new QHBoxLayout;
    _actionBarWidget->setLayout(actionBarLayout);
    QPushButton *recheckRunningAppsButton = new QPushButton(tr("Check for runing apps again"));
    connect(recheckRunningAppsButton, SIGNAL(clicked()), this, SIGNAL(recheckRunningAppsButtonClicked()));
    actionBarLayout->addWidget(recheckRunningAppsButton);
    actionBarLayout->addSpacerItem(new QSpacerItem(0, 5, QSizePolicy::Expanding));
    QPushButton *forceUninstallButton = new QPushButton(tr("Uninstall anyway"));
    connect(forceUninstallButton, SIGNAL(clicked()), this, SIGNAL(forceUninstallButtonClicked()));
    actionBarLayout->addWidget(forceUninstallButton);
    mainLayout->addWidget(_actionBarWidget);
    _actionBarWidget->hide();

    //
    // style
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(guiConf->getStyleForId("Label/Large/DarkGray/style"));
    _statusTextEdit->setStyleSheet(guiConf->getStyleForId("TextEdit/NoBorder/LightGray/style"));
    recheckRunningAppsButton->setStyleSheet(guiConf->getStyleForId("Button/DangerousAction/style"));
    recheckRunningAppsButton->setFlat(true);
    recheckRunningAppsButton->setFixedHeight(guiConf->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    forceUninstallButton->setStyleSheet(guiConf->getStyleForId("Button/DangerousAction/style"));
    forceUninstallButton->setFlat(true);
    forceUninstallButton->setFixedHeight(guiConf->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
}

void UninstallingView::setStatusText(QString statusText)
{
    _statusTextEdit->setText(statusText);
}

void UninstallingView::showActionBar()
{
    _actionBarWidget->show();
}

void UninstallingView::hideActionBar()
{
    _actionBarWidget->hide();
}
