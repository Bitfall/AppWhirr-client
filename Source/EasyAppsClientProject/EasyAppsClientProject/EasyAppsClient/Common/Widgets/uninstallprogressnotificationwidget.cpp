#include "uninstallprogressnotificationwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

// config
#include "Settings/guiconfigurations.h"
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

UninstallProgressNotificationWidget::UninstallProgressNotificationWidget(QString appId, QString appName, QWidget *parent) :
    SelfTimeoutingNotificationWidgetBase(parent),
    _appId(appId),
    _appName(appName),
    _isStateChanged(false)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(5, 0, 5, 0);

    this->_statusLabel = new QLabel;
    this->_statusLabel->setText(_appName + " " + tr("uninstalling..."));
    this->_statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(_statusLabel);

    this->_finishedTimer = new QTimer(this);
    this->_finishedTimer->setSingleShot(true);
    this->_finishedTimer->setInterval(5000);
    this->_finishedTimer->stop();
    connect(this->_finishedTimer, SIGNAL(timeout()), this, SLOT(_finishTimerTimeout()));

    this->_stateUpdateTimer = new QTimer(this);
    _stateUpdateTimer->setSingleShot(true);
    _stateUpdateTimer->setInterval(2000);
    _stateUpdateTimer->stop();
    connect(_stateUpdateTimer, SIGNAL(timeout()), this, SLOT(_doStateUpdate()));

    this->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/NormalPriority/style"));
}

void UninstallProgressNotificationWidget::setProgressStateMessage(QString progressStateMessage)
{
    this->_setNewState(_appName + " " + tr("uninstalling: ") + progressStateMessage, GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/NormalPriority/style"));
}

void UninstallProgressNotificationWidget::_finishTimerTimeout() {
    Q_EMIT this->__notificationFinished();
}

void UninstallProgressNotificationWidget::setResolutionNeeded()
{
    this->_setNewState(_appName + " " + tr("resolution needed to continue uninstall..."), GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/HighPriority/style"));
}

void UninstallProgressNotificationWidget::setUninstallSucceeded()
{
    this->_setNewState(_appName + " " + tr("uninstalled!"), GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/HighPriority/style"));
    this->_doStateUpdate();
    this->_finishedTimer->start();
}

void UninstallProgressNotificationWidget::setUninstallFailedWithError(QString errorMessage)
{
    this->_setNewState(_appName + " " + tr("failed to uninstall! Error message: ") + errorMessage, GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/CriticalPriority/style"));
    this->_doStateUpdate();
    this->_finishedTimer->start();
}

void UninstallProgressNotificationWidget::_doStateUpdate()
{
    if(!this->_isStateChanged) {
        return;
    }

    // restart the update timer
    this->_stateUpdateTimer->start();
    this->_isStateChanged = false;

    this->_statusLabel->setText(this->_currentStateInformation.getTitleText());
    this->setStyleSheet(this->_currentStateInformation.getStyleSheet());
}

void UninstallProgressNotificationWidget::_setNewState(QString title, QString styleSheet)
{
    this->_currentStateInformation = StateInformationModel(title, styleSheet);
    this->_isStateChanged = true;

    if(!this->_stateUpdateTimer->isActive()) {
        this->_stateUpdateTimer->start();
    }
}
