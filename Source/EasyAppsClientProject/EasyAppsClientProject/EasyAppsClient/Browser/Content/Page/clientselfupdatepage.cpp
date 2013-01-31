#include "clientselfupdatepage.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QDesktopServices>

// global objects
#include "Application/globalobjectregistry.h"
#include "Common/ClientSelfUpdateManager/clientselfupdatemanager.h"
#include "Common/Page/pagenavigationmanager.h"
#include "Browser/Notification/notificationmanager.h"
#include "Application/User/usermanager.h"

#include "Application/applicationglobalstateregistry.h"

// authorization
#include "Common/Security/authorization.h"

// after login actions
#include "Application/AfterLoginActions/grabsyncafterloginaction.h"
#include "Application/AfterLoginActions/offlineuserapplistupdatecheckafterloginaction.h"

// utility
#include "Application/appwhirrapplication.h"
#include "Browser/browserwindow.h"
#include "Common/multicontentmanager.h"
#include "Application/uifactory.h"

// config
#include "Settings/pageidcenter.h"
#include "Settings/settings.h"
#include "Common/guiconfigurationmanager.h"

#define MAX_CLIENT_NEW_VERSION_CHECK_RETRY_COUNT 2
#define CLIENT_NEW_VERSION_CHECK_RETRY_DELAY_MILLISECONDS 2000

#define CONTENT_ID_STATUS_CONTENT       QString("PageContentId-StatusContent")
#define CONTENT_ID_REGISTER_CONTENT     QString("PageContentId-RegisterContent")

ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage(QString pageId, QWidget *parent) :
    PageWidgetBase(pageId, parent),
    _versionCheckDelayedRetryHelper(NULL),
    _isErrorHappenedDuringClientSelfUpdateCheck(false),
    _isStartGrabSyncAfterLocalAppSearch(false),
    _isInAutoLoginMode(true)
{

}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::initializePageContent()
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    //this->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));
    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style"));

    QVBoxLayout *contentLayout = new QVBoxLayout;
    mainLayout->addLayout(contentLayout);
    _pageContentManager = new MultiContentManager(contentLayout, this);

    //
    // content 1: status content
    QWidget *statusContentHolderWidget = new QWidget;
    statusContentHolderWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *statusContentHolderLayout = new QVBoxLayout;
    statusContentHolderWidget->setLayout(statusContentHolderLayout);
    _pageContentManager->setContentWidgetToId(CONTENT_ID_STATUS_CONTENT, statusContentHolderWidget);
    _statusLabel = new QLabel;
    _statusLabel->setText(tr("Cheking for available new version..."));
    _statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _statusLabel->setStyleSheet(guiConfManager->getStyleForId("Label/ExtraLarge/DarkGreen/style"));
    statusContentHolderLayout->addWidget(_statusLabel);
//    mainLayout->addWidget(this->_statusLabel);

    _mainActionButton = new QPushButton;
    _mainActionButton->setText(tr("Start update"));
    _mainActionButton->setFixedHeight(guiConfManager->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    _mainActionButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    _mainActionButton->setFlat(true);
    _mainActionButton->setEnabled(false);
    _mainActionButton->setMinimumHeight(50);
    connect(_mainActionButton, SIGNAL(clicked()), this, SLOT(mainActionButtonPressed()));
    statusContentHolderLayout->addWidget(_mainActionButton);
//    mainLayout->addWidget(this->_mainActionButton);

    //
    // content 2: register
    QWidget *registerContentHolderWidget = new QWidget;
    registerContentHolderWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *registerContentHolderLayout = new QVBoxLayout;
    registerContentHolderWidget->setLayout(registerContentHolderLayout);
    _pageContentManager->setContentWidgetToId(CONTENT_ID_REGISTER_CONTENT, registerContentHolderWidget);

    QHBoxLayout *registerTextSubContentHolderLayout = new QHBoxLayout;
    QLabel *registerNewAccountLabel = new QLabel(tr("To be able to use AppWhirr you have to register an AppWhirr account. If you already have one log in above."));
    registerNewAccountLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Large/MediumGray/style"));
    registerNewAccountLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    registerTextSubContentHolderLayout->addWidget(registerNewAccountLabel);

    QHBoxLayout *registerButtonSubContentHolderLayout = new QHBoxLayout;
    QPushButton *registerNewUserAccountButton = new QPushButton(tr("Register new AppWhirr account"));
    registerNewUserAccountButton->setFixedHeight(50);
    registerNewUserAccountButton->setFixedWidth(300);
    registerNewUserAccountButton->setStyleSheet(guiConfManager->getStyleForId("Button/StartupPage/RegisterAccount/style"));
    registerNewUserAccountButton->setFlat(true);
    registerNewUserAccountButton->setEnabled(true);
    connect(registerNewUserAccountButton, SIGNAL(clicked()), this, SLOT(_registerNewAccount()));
    registerButtonSubContentHolderLayout->addWidget(registerNewUserAccountButton);

    registerContentHolderLayout->addSpacerItem(AppWhirr::UiFactory::createVerticalSpacer());
    registerContentHolderLayout->addLayout(registerTextSubContentHolderLayout);
    registerContentHolderLayout->addSpacing(15);
    registerContentHolderLayout->addLayout(registerButtonSubContentHolderLayout);
    registerContentHolderLayout->addSpacerItem(AppWhirr::UiFactory::createVerticalSpacer());


    //
    _pageContentManager->switchContent(CONTENT_ID_STATUS_CONTENT);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::actualizeGuiToState(QString stateText, QString mainActionButtonText, bool isMainActionButtonEnabled, PageContentTypeEnum contentType)
{
    this->_statusLabel->setText(stateText);
    if(!mainActionButtonText.isEmpty()) {
        _mainActionButton->setText(mainActionButtonText);
    }
    _mainActionButton->setEnabled(isMainActionButtonEnabled);
    _mainActionButton->setHidden(!isMainActionButtonEnabled);

    if(contentType == PCT_StatusContent) {
        _pageContentManager->switchContent(CONTENT_ID_STATUS_CONTENT);
    }
    else if(contentType == PCT_RegisterNewAccountContent) {
        _pageContentManager->switchContent(CONTENT_ID_REGISTER_CONTENT);
    }
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::mainActionButtonPressed()
{
    this->finishedVersionChecking(true);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_startToCheckForNewAvailableVersion()
{    
    if(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager()->getIsInSelfUpdatingProcess()) {
        WLog("Already in self-updating-process, won't start a new one.");
        return;
    }

    if(this->_versionCheckDelayedRetryHelper == NULL) {
        _versionCheckDelayedRetryHelper = new DelayedRetryHelper(MAX_CLIENT_NEW_VERSION_CHECK_RETRY_COUNT, CLIENT_NEW_VERSION_CHECK_RETRY_DELAY_MILLISECONDS, this);

        connect(this->_versionCheckDelayedRetryHelper, SIGNAL(retryNow()), this, SLOT(_retryGetNewVersion()));
        connect(this->_versionCheckDelayedRetryHelper, SIGNAL(noMoreRetry()), this, SLOT(_noMoreVersionCheckRetry()));
    }
    this->_versionCheckDelayedRetryHelper->resetRetryCount();


    this->_disconnectFromClientSelfUpdateManager();
    this->_connectToClientSelfUpdateManager();

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager()->startCheckIsNewVersionAvailableImmediately();
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_connectToClientSelfUpdateManager()
{
    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(newVersionFound()), this, SLOT(_clientSelfUpdateNewVersionFound()), Qt::UniqueConnection);
    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(noNewVersionFound()), this, SLOT(_clientSelfUpdateNoNewVersionFound()), Qt::UniqueConnection);
    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(failedToGetNewVersion(QString)), this, SLOT(_clientSelfUpdateFailedToGetNewVersion(QString)), Qt::UniqueConnection);

    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(downloadFailedWithError(QString)), this, SLOT(_clientSelfUpdateDownloadFailedWithError(QString)), Qt::UniqueConnection);
    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(downloadProgress(int)), this, SLOT(_clientSelfUpdateDownloadProgress(int)),Qt::UniqueConnection);
    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(downloadFinished()), this, SLOT(_clientSelfUpdateDownloadFinishedSuccessfully()),Qt::UniqueConnection);

    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(installFailedWithError(QString)), this, SLOT(_clientSelfUpdateInstallFailedWithError(QString)), Qt::UniqueConnection);
    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(readyToQuitAndPerformInstall()), this, SLOT(_clientSelfUpdateReadyToQuitAndInstall()), Qt::UniqueConnection);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_disconnectFromClientSelfUpdateManager()
{
    disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(newVersionFound()), this, SLOT(_clientSelfUpdateNewVersionFound()));
    disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(noNewVersionFound()), this, SLOT(_clientSelfUpdateNoNewVersionFound()));
    disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(failedToGetNewVersion(QString)), this, SLOT(_clientSelfUpdateFailedToGetNewVersion(QString)));

    disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(downloadFailedWithError(QString)), this, SLOT(_clientSelfUpdateDownloadFailedWithError(QString)));
    disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(downloadProgress(int)), this, SLOT(_clientSelfUpdateDownloadProgress(int)));
    disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(downloadFinished()), this, SLOT(_clientSelfUpdateDownloadFinishedSuccessfully()));

    disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(installFailedWithError(QString)), this, SLOT(_clientSelfUpdateInstallFailedWithError(QString)));
    disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(readyToQuitAndPerformInstall()), this, SLOT(_clientSelfUpdateReadyToQuitAndInstall()));
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_connectToUserLogin()
{
    this->_disconnectFromUserLogin();

    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginStarted()), this, SLOT(_userLoginStarted()), Qt::UniqueConnection);
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(_userLoggedIn()), Qt::UniqueConnection);
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginAborted()), this, SLOT(_userLoginAborted()), Qt::UniqueConnection);
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginCommunicationError(QString)), this, SLOT(_userLoginCommunicationError(QString)), Qt::UniqueConnection);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_disconnectFromUserLogin()
{
    disconnect(Authorization::sharedAuthorization(), SIGNAL(userLoginStarted()), this, SLOT(_userLoginStarted()));
    disconnect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(_userLoggedIn()));
    disconnect(Authorization::sharedAuthorization(), SIGNAL(userLoginAborted()), this, SLOT(_userLoginAborted()));
    disconnect(Authorization::sharedAuthorization(), SIGNAL(userLoginCommunicationError(QString)), this, SLOT(_userLoginCommunicationError(QString)));
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_clientSelfUpdateNewVersionFound()
{
    // new version available -> download it and install it
    this->actualizeGuiToState(tr("New version available. Have to update to continue. It will close AppWhirr, but will restart after the update."), tr("Start to update"), false);

    // start to download it
    this->_startToDownloadNewAvailableVersion();
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::finishedVersionChecking(bool isErrorHappened)
{
    if(_currPageMode == ClientUpdateCheckThenLoadLocalAppDatabase) {
        // start to load local app database
//        this->_switchToStorePage(isErrorHappened);
        // if the user logs in then after app-loading grab-sync should be started manually
        _isErrorHappenedDuringClientSelfUpdateCheck = isErrorHappened;
        this->showLoginThenStartAppLocalScan();
    }
    else {
        this->_finishedAndSwitchToStorePage(isErrorHappened);
    }
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_finishedAndSwitchToStorePage(bool isErrorHappened)
{
    // also start to check
    Q_EMIT finishedWithCheckingAndNoUpdateFound();

//    _currPageMode
//    if(isErrorHappened) {
//        AppwhirrApplication::instance()->getBrowserWindow()->unblockUserNavigationAndShowHiddenRelatedParts();
//    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->switchToRootPageStack(PageIdCenter::storePageId());
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_clientSelfUpdateNoNewVersionFound()
{
    // no new version, go to store page
    this->finishedVersionChecking(false);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_clientSelfUpdateFailedToGetNewVersion(QString errorMessage)
{
    DLog("Client new version check failed with error: ") << errorMessage;
//    this->_versionCheckRetryDelayTimer->start();
    this->_versionCheckDelayedRetryHelper->retryWithDelay();
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_retryGetNewVersion()
{
    // retry get new version
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager()->startCheckIsNewVersionAvailableImmediately();
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_noMoreVersionCheckRetry() {
    // probably no network connection, go the the store page
    this->finishedVersionChecking(true);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_startToDownloadNewAvailableVersion()
{
    this->actualizeGuiToState(tr("Downloading..."), QString(), false);

    // clear every install related tmp folders
    if( !GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager()->clearRelatedTmpDirectories() ) {
        this->_clientSelfUpdateInstallFailedWithError("Cannot clean install related directories.");
        return;
    }

    // start download
//    this->_disconnectFromClientSelfUpdateManager();
//    this->_connectToClientSelfUpdateManager();

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager()->startToDownloadNewAvailableVersion();
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_clientSelfUpdateInstallFailedWithError(QString errorMessage)
{
    this->actualizeGuiToState(tr("Install failed with error: %1").arg(errorMessage), tr("Go to the Store page"), true);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_clientSelfUpdateReadyToQuitAndInstall()
{
//    this->actualizeGuiToState(tr("Preparations finished. Starting the installation..."), true);

    // start updater
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager()->startUpdaterInClientUpdateMode();
    // close the client
    AppwhirrApplication::instance()->quit();
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_clientSelfUpdateDownloadFailedWithError(QString errorMessage)
{
    this->actualizeGuiToState(tr("Download failed with error: %1").arg(errorMessage), tr("Go to the Store page"), true);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_clientSelfUpdateDownloadProgress(int progressPercent)
{
    _statusLabel->setText(tr("SelfUpdate - download progress: %1%").arg(progressPercent));
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_clientSelfUpdateDownloadFinishedSuccessfully()
{
    _statusLabel->setText(tr("Download finished. Preparing install..."));

    // install
//    this->_disconnectFromClientSelfUpdateManager();
//    this->_connectToClientSelfUpdateManager();

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager()->startToPrepareInstallDownloadedNewVersion();
}


// ------------------------------
// --- app local scan related ---

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::showLoginThenStartAppLocalScan()
{
    UserManager *userManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager();
    QString currUserId = userManager->getCurrentUserId();
    QString currUserName = userManager->getCurrentUserName();
    QString currUserPsw = userManager->getUserPswByUserId(currUserId);

    if(currUserId.isEmpty() || currUserName.isEmpty() || currUserPsw.isEmpty())
    {
        // there's no current-user (or username or psw is missing)
        //  show the login

        _isInAutoLoginMode = false;
        this->actualizeGuiToState(tr("Log in with your User"), tr("Have to select a User or Cancel"), false, PCT_RegisterNewAccountContent);
        this->_connectToUserLogin();
        Authorization::sharedAuthorization()->showLoginDialogAndStartLogin();
    }
    else {
        // there's an active current-user
        //  try to log-in the user

        _isInAutoLoginMode = true;
        this->actualizeGuiToState(tr("Logging you in..."), tr("Have to select a User or Cancel"), false, PCT_StatusContent);
        this->_connectToUserLogin();

        Authorization::sharedAuthorization()->startLoginProcessWithoutUserLoginDialog(currUserName, currUserPsw);
    }
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_userLoginStarted()
{
//    this->actualizeGuiToState(tr("Loading your awesome apps..."), tr("Just a few seconds..."), false);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_userLoggedIn()
{
    AppwhirrApplication::instance()->getBrowserWindow()->actualizeUserNameTitleText();
    this->_loginFinishedTryToStartLocalAppScan(true);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_userLoginAborted()
{
    AppwhirrApplication::instance()->getBrowserWindow()->actualizeUserNameTitleText();
    this->_loginFinishedTryToStartLocalAppScan(false);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_userLoginCommunicationError(QString errorMessage)
{
    WLog("User login communication error: ") << errorMessage;

    if(_isInAutoLoginMode) {
        // log in - in offline mode
        AppwhirrApplication::instance()->getBrowserWindow()->actualizeUserNameTitleText();
        this->_loginFinishedTryToStartLocalAppScan(false);
    }
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_loginFinishedTryToStartLocalAppScan(bool isStartGrabSyncAfterLocalAppSearchIfUserSelected)
{
    DLog("_loginFinishedTryToStartLocalAppScan");

    // disconnect
    this->_disconnectFromUserLogin();

    QString currentUserId = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->getCurrentUserId();
    if(currentUserId.isEmpty()) {
        DLog("No user selected - offline mode.");
        this->_finishedAndSwitchToStorePage(_isErrorHappenedDuringClientSelfUpdateCheck);

        Authorization::sharedAuthorization()->setUserLoginMode(Authorization::ULM_Initial);
    }
    else {
        // current user available
        Authorization::sharedAuthorization()->setUserLoginMode(Authorization::ULM_ConfirmationOnly);

        this->actualizeGuiToState(tr("Loading your awesome apps..."), tr("Just a few seconds..."), false);
        this->repaint();
        if(isStartGrabSyncAfterLocalAppSearchIfUserSelected) {
            // the user logged in - after app-loading grab-sync should be started manually
            _isStartGrabSyncAfterLocalAppSearch = true;
        }
        // start the scan
        connect(AppwhirrApplication::instance(), SIGNAL(_userAppsLoadedToLocalAppDatabase()), this, SLOT(_userAppsLoadedToLocalAppDatabase()));
        AppwhirrApplication::instance()->userLoggedInInitializeRequiredObjects(currentUserId);
    }
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_userAppsLoadedToLocalAppDatabase()
{
    DLog("_userAppsLoadedToLocalAppDatabase");
    // disconnect
    disconnect(AppwhirrApplication::instance(), SIGNAL(_userAppsLoadedToLocalAppDatabase()), this, SLOT(_userAppsLoadedToLocalAppDatabase()));

    Authorization::sharedAuthorization()->hideLoginDialog();

    // and also if the user logged in then grab-sync should be started
    if(_isStartGrabSyncAfterLocalAppSearch) {
        ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->addAfterLoginAction(new GrabSyncAfterLoginAction);
//        MyApplication::instance()->getBrowserWindow()->_forceStartGrabbedAppSync();
    }
    else if(this->_currPageMode == ClientUpdateCheckThenLoadLocalAppDatabase) {
        ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->addAfterLoginAction(new OfflineUserAppListUpdateCheckAfterLoginAction);
    }

    // and finally - switch to the store page
    this->_finishedAndSwitchToStorePage(this->_isErrorHappenedDuringClientSelfUpdateCheck);
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::_registerNewAccount()
{
    QDesktopServices::openUrl(QUrl("https://appwhirr.com/u/register/"));
}

// ------------------------------
// --- page operation related ---

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::pagePushWillHappen()
{
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::pageWillAppear(QString param)
{
    // reset some variables
    _isErrorHappenedDuringClientSelfUpdateCheck = false;
    _isStartGrabSyncAfterLocalAppSearch = false;

    if(param == "mode:ClientUpdateCheckThenLoadLocalAppDatabase") {
        this->_currPageMode = ClientUpdateCheckThenLoadLocalAppDatabase;
    }
    else {
        // currently in every other case the mode will be OnlyClientUpdateCheck
        this->_currPageMode = OnlyClientUpdateCheck;
    }
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::pageDidAppear()
{
    DLog("page did appear");
//    MyApplication::instance()->getBrowserWindow()->blockUserNavigationWithHidingRelatedParts();
//    MyApplication::instance()->postEventToBrowserWindow(new EnsureApplicationPresentedEvent());

    this->actualizeGuiToState(tr("Cheking for available new version..."), tr("Just a few seconds..."), false);

    if(this->_currPageMode == ClientUpdateCheckThenLoadLocalAppDatabase && !Settings::sharedSettings()->getIsRememberLastUser()) {
        // remove users
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->setNoUserIsCurrent();
    }

    this->_startToCheckForNewAvailableVersion();

    //
    //        TEST: to skip the initial update-check
//    if(this->_currPageMode == ClientUpdateCheckThenLoadLocalAppDatabase) {
////        this->_clientSelfUpdateNoNewVersionFound();
//        QTimer::singleShot(0, this, SLOT(_clientSelfUpdateNoNewVersionFound()));
//    }
//    else {
//        this->_startToCheckForNewAvailableVersion();
//    }
}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::pagePopWillHappen()
{

}

void ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage::pageWillDisappear()
{
    DLog("Client self update page will disappear");
    this->_disconnectFromUserLogin();
    this->_disconnectFromClientSelfUpdateManager();
}
