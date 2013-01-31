#include "installprogressnotificationwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QProgressBar>

// config
#include "Settings/guiconfigurations.h"
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

InstallProgressNotificationWidget::InstallProgressNotificationWidget(QString appId, QString appName, QWidget *parent) :
    SelfTimeoutingNotificationWidgetBase(parent),
    _appId(appId),
    _appName(appName),
    _isStateChanged(false)
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
    this->setStyleSheet(guiConfManager->getStyleForId("Notification/Widget/InstallProgress/HighPriority/style"));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(5, 0, 5, 0);

    this->_statusLabel = new QLabel;
    this->_statusLabel->setText(_appName + " " + tr("grabbing..."));
    this->_statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    mainLayout->addWidget(_statusLabel);

    _downloadProgressBar = new QProgressBar;
    _downloadProgressBar->setMinimum(0);
    _downloadProgressBar->setMaximum(100);
    _downloadProgressBar->setValue(0);
    _downloadProgressBar->setStyleSheet(guiConfManager->getStyleForId("ProgressBar/Default/Green/style"));
    _downloadProgressBar->setFixedHeight(26);
    mainLayout->addWidget(_downloadProgressBar);
    _downloadProgressBar->hide();

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
}

void InstallProgressNotificationWidget::_finishTimerTimeout() {
    Q_EMIT this->__notificationFinished();
}

void InstallProgressNotificationWidget::setGrabbed()
{   
    this->_setNewState(_appName + " " + tr("grabbed!"), GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/LowPriority/style"), false, 0, 0, 0);
}

void InstallProgressNotificationWidget::setDownloading()
{
    this->_setNewState(_appName + " " + tr("downloading"), GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/NormalPriority/style"), true, 0, 100, 0);
}

void InstallProgressNotificationWidget::setDownloadProgress(int progress)
{
    QString titleText;
    if(progress < 100) {
//        titleText = _appName + " " + tr("downloading: ") + QString("%1%").arg(progress);
        titleText = _appName + " " + tr("downloading");
        this->_setNewState(titleText, GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/NormalPriority/style"), true, 0, 100, progress);
    }
    else {
        // finished downloading, waiting for installation
        titleText = _appName + " " + tr("installing");
        this->_setNewState(titleText, GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/NormalPriority/style"), true, 0, 0, 1);
    }
}

void InstallProgressNotificationWidget::setInstallStarted()
{
    this->_setNewState(_appName + " " + tr("initializing install..."), GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/LowPriority/style"), true, 0, 0, 1);
}

void InstallProgressNotificationWidget::setInstalled()
{
    this->_setNewState(_appName + " " + tr("installed!"), GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/HighPriority/style"), false, 0, 0, 0);
    this->_doStateUpdate();
    this->_finishedTimer->start();
}

void InstallProgressNotificationWidget::setInstallProgressStateMessage(QString installProgressStateMessage)
{
    this->_setNewState(_appName + " " + tr("installing") + ": " + installProgressStateMessage, GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/NormalPriority/style"), true, 0, 0, 1);
}

void InstallProgressNotificationWidget::setGrabFailedWithError(QString errorMessage)
{
    this->_setNewState(_appName + " " + tr("failed to grab! Error message: ") + errorMessage, GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/CriticalPriority/style"), false, 0, 0, 0);
    this->_finishedTimer->start();
}

void InstallProgressNotificationWidget::setInstallFailedWithError(QString errorMessage)
{
    this->_setNewState(_appName + " " + tr("failed to install! Error message: ") + errorMessage, GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Notification/Widget/InstallProgress/CriticalPriority/style"), false, 0, 0, 0);
    this->_finishedTimer->start();
}

void InstallProgressNotificationWidget::_doStateUpdate()
{
    if(!this->_isStateChanged) {
        return;
    }

    // restart the update timer
    this->_stateUpdateTimer->start();
    this->_isStateChanged = false;

    this->_statusLabel->setText(this->_currentStateInformation.getTitleText());
    this->setStyleSheet(this->_currentStateInformation.getStyleSheet());

    if(_currentStateInformation.getIsShowProgressBar()) {
        _downloadProgressBar->show();

        this->_downloadProgressBar->setMinimum(_currentStateInformation.getProgressBarMin());
        this->_downloadProgressBar->setMaximum(_currentStateInformation.getProgressBarMax());
        this->_downloadProgressBar->setValue(_currentStateInformation.getProgressBarValue());
    }
    else {
        _downloadProgressBar->hide();
    }
}

void InstallProgressNotificationWidget::_setNewState(QString title, QString styleSheet, bool isShowProgressBar, int progressBarMin, int progressBarMax, int progressBarValue)
{
    this->_currentStateInformation = InstallStateInformationModel(title, styleSheet, isShowProgressBar, progressBarMin, progressBarMax, progressBarValue);
    this->_isStateChanged = true;

    if(!this->_stateUpdateTimer->isActive()) {
        this->_stateUpdateTimer->start();
    }
}
