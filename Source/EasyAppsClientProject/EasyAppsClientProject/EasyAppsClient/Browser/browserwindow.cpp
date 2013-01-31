#include "browserwindow.h"

// UI
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QSizeGrip>
//#include "Browser/Featured/browserfeaturedmainwidget.h"

// notification
#include "Common/Widgets/clientselfupdateavailablenotificationwidget.h"
#include "Common/Widgets/textandacceptcancelbuttonsnotificationwidget.h"

// other
#include <QEvent>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QApplication>
#include <QFocusEvent>
#include <QLabel>
#include <QTimer>
#include <QThreadPool>
#include <QStateMachine>
#include <QPropertyAnimation>
#include <QSignalTransition>
#include <QPainter>
#include <QDesktopWidget>
#include <QShortcut>

#include "Common/Event/forcehideorminimizebrowserevent.h"
#include "Common/Event/forcehideorminimizeevent.h"
#include "Common/Event/ensureapplicationpresentedevent.h"

// global objects
#include "Application/globalobjectregistry.h"
#include "Application/Managers/AppGrab/grabapplicationmanager.h"
#include "Browser/Notification/notificationmanager.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"
#include "Application/Controllers/appterminationcontroller.h"
#include "Application/CriticalAction/criticalactionmanager.h"
#include "Application/CriticalAction/criticalactioncodes.h"
#include "Common/guiconfigurationmanager.h"
#include "Application/User/usermanager.h"
#include "Application/applicationglobalstateregistry.h"
#include "Common/ClientSelfUpdateManager/clientselfupdatemanager.h"
#include "Settings/settings.h"

// navigation
#include "Browser/Content/Widget/navigationwidget.h"

// PAGE NAVIGATION
#include "Common/Page/pagenavigationmanager.h"
#include "Common/Page/Delegate/simplefadepagetransitionmanagerdelegate.h"
#include "Settings/pageidcenter.h"
#include "Browser/Content/Page/applicationstoremainpage.h"
#include "Browser/Content/Page/helppage.h"
#include "Browser/Content/Page/clientselfupdatepage.h"
#include "Browser/Content/Page/suggestanapppage.h"
#include "Browser/Content/Page/feedbackpage.h"

// MENU
#include "Common/Menu/menumanager.h"
#include "Common/Menu/plainverticalmenupresenterwidget.h"
#include "Common/Menu/menuitemwidget.h"
#include "Settings/menuitemidcenter.h"

#include "Browser/Content/Page/myappspage.h"
#include "Browser/Content/Page/optionscontentpage.h"
// --

// User Authorization
#include "Common/Security/authorization.h"

// delegates
#include "specialpageactioncontroller.h"
#include "Common/WidgetResizerDelegate/widgetresizerdelegate.h"
#include "Application/appdatabasesyncmanager.h"

// communication
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
#include "Application/Communication/Entity/grablistnetworkentity.h"
#include "Application/Communication/Request/applistversioninfonetworkrequest.h"

// utility
#include "Common/Widgets/simpleimagepresenterwidget.h"
#include "Common/Widgets/installprogressnotificationwidget.h"
#include "Browser/Notification/notificationwidget.h"
#include "Common/Helpers/platformflagshelper.h"
#include "Application/appwhirrapplication.h"
#include "Application/Utils/awshortcuthandler.h"

//#include "Common/Security/Login/userlogindialog.h"
#include "Browser/Content/Widget/userloginsidebarwidget.h"
#include "Browser/Content/Widget/appterminatorsidebarwidget.h"

#include "Browser/BackgroundWorkers/background_grabsyncperformer.h"

// configurations
#include "Settings/configmanager.h"
#include "Settings/guiconfigurations.h"

BrowserWindow::BrowserWindow(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint | Qt::CustomizeWindowHint),
      _userLoginSidebar(NULL),
//#ifdef Q_WS_WIN
      __winWorkaround_isMaximized(false),
//#endif
      _isShowLoginSidebarRequested(false)
{
    GuiConfigurationManager *guiConfManager = new GuiConfigurationManager(":/GuiConfigurations");
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->setGuiConfigurationManager(guiConfManager);

    // set default size
    // -> centerize in screen but with the last window width
    this->restoreLastUsedWindowSize();

    this->setWindowTitle("AppWhirr");
    this->setMinimumSize(guiConfManager->getSizeForId("MainWindow/minimumSize"));
//    this->setMinimumSize(QSize(1000, 618)); // TEST

    //
    // initialize notification manager
    NotificationManager *notificationManager = new NotificationManager;
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->setNotificationManager(notificationManager);

    // Initialize GrabApplicationManager
    GrabApplicationManager *grabApplicationManager = new GrabApplicationManager;
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->setGrabApplicationManager(grabApplicationManager);
    // and connect it
    //  grab
    connect(grabApplicationManager, SIGNAL(appGrabStarted(QString,QString)), this, SLOT(_appGrabStarted(QString,QString)));
    connect(grabApplicationManager, SIGNAL(appGrabbedSuccessfully(QString,QString,QString,QString,AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum)), this, SLOT(_appGrabbedSuccessfully(QString,QString,QString,QString,AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum)));
    connect(grabApplicationManager, SIGNAL(appGrabFailed(QString,QString,bool,QString)), this, SLOT(_appGrabFailed(QString,QString,bool,QString)));
    //  download and install
    connect(grabApplicationManager, SIGNAL(appDownloadProgress(QString,int)), this, SLOT(_appDownloadProgress(QString,int)));
    connect(grabApplicationManager, SIGNAL(appInstallationStarted(QString,QString)), this, SLOT(_appInstallationStarted(QString,QString)));
    connect(grabApplicationManager, SIGNAL(appInstallProgressStateMessage(QString,QString)), this, SLOT(_appInstallProgressStateMessage(QString,QString)));
    connect(grabApplicationManager, SIGNAL(appInstalledSuccessfullyToDirectory(QString,QString,QString,QString)), this, SLOT(_appInstalledSuccessfullyToDirectory(QString,QString,QString,QString)));
    connect(grabApplicationManager, SIGNAL(appInstallFailed(QString,QString,bool,QString)), this, SLOT(_appInstallFailed(QString,QString,bool,QString)));
    // install conflict resolution
    connect(grabApplicationManager, SIGNAL(__resolutionNeededForRunningApp(QString,QString)), this, SLOT(__resolutionNeededForRunningApp(QString,QString)));
    // framework install allow/deny resolution
    connect(grabApplicationManager, SIGNAL(__frameworksRequireInstallForApp(QString,QString)), this, SLOT(__frameworksRequireInstallForApp(QString,QString)));


    //
    // UI

    // create and set the central widget and layout
    centralWidget = new QWidget;
    centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    centralWidget->setCursor(QCursor());
    this->setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    centralWidget->setLayout(mainLayout);

    //
    // Navigation
    navigationWidget = new NavigationWidget;
    connect(navigationWidget, SIGNAL(minimizeButtonClicked()), this, SLOT(windowStateMinimizeButtonPressed()));
    connect(navigationWidget, SIGNAL(maximizeButtonClicked()), this, SLOT(windowStateMaximizeButtonPressed()));
    connect(navigationWidget, SIGNAL(closeButtonClicked()), this, SLOT(windowStateCloseButtonPressed()));
    mainLayout->addWidget(navigationWidget);

    //
    // Content

    QWidget *contentWidget = new QWidget;
    QHBoxLayout* contentLayout = new QHBoxLayout;
    contentWidget->setLayout(contentLayout);
    contentWidget->setContentsMargins(0, 0, 0, 0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);

    centralWidget->setContentsMargins(3, 3, 3, 3);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // SideBar layout
    sideBarWidget = new AnimatedTwoFixedSizeWidgetBase(700, QSize(0, 0), QSize(guiConfManager->getIntForId("MainMenuHolderSidebar/expandedFixedWidth"), 50), false, true, false);
    connect(sideBarWidget, SIGNAL(switchToExpandedStateFinished()), this, SLOT(_mainMenuSidebarSwitchedToExpandedState()));
    connect(sideBarWidget, SIGNAL(switchToCollapsedStateFinished()), this, SLOT(_mainMenuSidebarSwitchedToCollapsedState()));
    sideBarWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sideBarWidget->setObjectName("sideBarWidget");
//    sideBarWidget->setMaximumWidth(200);
//    sideBarWidget->setMinimumWidth(200);
//    sideBarWidget->setFixedWidth(180);
    sideBarWidget->setStyleSheet("*#sideBarWidget { background:rgb(230, 230, 230) url(':/sidebar_logo') no-repeat center bottom; }");
    contentLayout->addWidget(sideBarWidget);

    sideBarLayout = new QVBoxLayout;
    sideBarLayout->setContentsMargins(2, 2, 0, 0);
    sideBarLayout->setSpacing(0);
    sideBarWidget->setLayout(sideBarLayout);
    sideBarWidget->setContentsMargins(0, 0, 0, 0);

    //
    // initialize the menu

    PlainVerticalMenuPresenterWidget *menuPresenterWidget = new PlainVerticalMenuPresenterWidget;
    MenuManager *menuManager = new MenuManager(menuPresenterWidget);
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->setMenuManager(menuManager);

    menuPresenterWidget->setObjectName("menuPresenterWidget");
//    menuPresenterWidget->setStyleSheet("PlainVerticalMenuPresenterWidget#menuPresenterWidget { background-color:rgb(20, 230, 210); }");
//    menuPresenterWidget->setStyleSheet("QWidget#menuPresenterWidget { background-color: transparent; }");

    sideBarLayout->addWidget(menuPresenterWidget);
//    sideBarLayout->addStretch(1);
//    sideBarLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding));


    // MAIN content widget and layout - which holds the "right side" of the window
    QWidget *mainContentContainerWidget = new QWidget;
    mainContentContainerWidget->setObjectName("mainContentContainerWidget");
    mainContentContainerWidget->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style/ObjectName(%1)").arg("mainContentContainerWidget"));
    QVBoxLayout *mainContentContainerLayout = new QVBoxLayout;
    mainContentContainerWidget->setLayout(mainContentContainerLayout);
    mainContentContainerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainContentContainerWidget->setContentsMargins(0, 0, 0, 0);
    mainContentContainerLayout->setContentsMargins(0, 0, 0, 0);
    mainContentContainerLayout->setSpacing(0);
    contentLayout->addWidget(mainContentContainerWidget);


    //
    // login sidebar
    _userLoginSidebar = new UserLoginSidebarWidget(QSize(1000, 270));
    _userLoginSidebar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainContentContainerLayout->addWidget(this->_userLoginSidebar);
    // connect authorization event signals
    connect(Authorization::sharedAuthorization(), SIGNAL(_showLoginDialogRequested()), this, SLOT(showUserLoginSidebar()));
    connect(Authorization::sharedAuthorization(), SIGNAL(_hideLoginDialogRequested()), this, SLOT(hideUserLoginSidebar()));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginCommunicationError(QString)), this, SLOT(userLoginCommunicationError(QString)));
//    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginAborted()), this, SLOT(hideUserLoginSidebar()));
//    connect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(hideUserLoginSidebar()));

    //
    // app terminator and it's sidebar
    AppTerminationController *appTerminationController = new AppTerminationController;
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->setAppTerminationController(appTerminationController);
    // presenter
    _appTerminatorSidebarWidget = new AppTerminatorSidebarWidget(QSize(1000, 200));
    _appTerminatorSidebarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // connect presenter with the controller
    connect(appTerminationController, SIGNAL(_presenter_ShowWithInfo(QString,QString,AppTerminationController::TerminationReasonCodeEnum)), _appTerminatorSidebarWidget, SLOT(actualizeAndShow(QString,QString,AppTerminationController::TerminationReasonCodeEnum)));
    connect(appTerminationController, SIGNAL(_presenter_Hide()), _appTerminatorSidebarWidget, SLOT(hideWithAnimation()));
    connect(_appTerminatorSidebarWidget, SIGNAL(startTermination()), appTerminationController, SLOT(_presenter_StartTermination()));
    connect(_appTerminatorSidebarWidget, SIGNAL(abortTermination()), appTerminationController, SLOT(_presenter_AbortTermination()));
    // add it to the layout
    mainContentContainerLayout->addWidget(this->_appTerminatorSidebarWidget);
    //
    // also connect terminator - for install issue resolution
    connect(appTerminationController, SIGNAL(applicationTerminatedSuccesfully(QString,QString)), this, SLOT(__applicationTerminatedSuccesfully(QString,QString)));
    connect(appTerminationController, SIGNAL(cannotTerminateApplication(QString,QString)), this, SLOT(__cannotTerminateApplication(QString,QString)));


    //
    // page content layout
    pageContentHolderWidget = new QWidget;
    pageContentHolderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pageContentHolderWidget->setObjectName("pageContentHolderWidget");
    pageContentHolderWidget->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style/ObjectName(%1)").arg("pageContentHolderWidget"));
//    pageContentHolderWidget->setStyleSheet(QString("QWidget#pageContentHolderWidget { background-color: #ff00ff; }"));
//    mainLayout->addWidget(contentWidget);
    mainContentContainerLayout->addWidget(pageContentHolderWidget);

    pageContentHolderLayout = new QVBoxLayout;
    pageContentHolderLayout->setContentsMargins(0, 0, 0, 0);
    pageContentHolderLayout->setSpacing(0);
    pageContentHolderWidget->setLayout(pageContentHolderLayout);
//    pageContentHolderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pageContentHolderWidget->setContentsMargins(0, 0, 0, 0);
    pageContentHolderLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    //
    // add notification area and size-grip
//    QWidget *bottomContentHolderWidget = new QWidget;
    QHBoxLayout *bottomContentHolderLayout = new QHBoxLayout;
    mainContentContainerLayout->addLayout(bottomContentHolderLayout);
    bottomContentHolderLayout->addWidget(notificationManager->_getNotificationAreaWidget());
//    bottomContentHolderLayout->addWidget(new QSizeGrip(this), 0, Qt::AlignBottom | Qt::AlignRight);



    //
    // initialize page-navigation-manager
    //  and the root pages
    //  and connect the root menu items
    PageNavigationManager *pageNavigationManager = new PageNavigationManager(new SimpleFadePageTransitionManagerDelegate, pageContentHolderLayout);
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->setPageNavigationManager(pageNavigationManager);

    // my apps page
    MyAppsPage *myAppsPage = new MyAppsPage(PageIdCenter::myAppsPageId());
    pageNavigationManager->addRootPage(myAppsPage);
    //  related menu
    MenuItemWidget *myAppsItem = new MenuItemWidget(MenuItemIdCenter::myAppsMenuId(), tr("My Apps"));
    connect(myAppsItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), myAppsPage, SLOT(relatedMainMenuItemActivated(QString)));
    menuManager->addRootItem(myAppsItem);

    // grab some apps / store page
    ApplicationStoreMainPage* applicationStorePage = new ApplicationStoreMainPage(PageIdCenter::storePageId());
    pageNavigationManager->addRootPage(applicationStorePage);
    //  related menu
    MenuItemWidget* grabSomeAppsItem = new MenuItemWidget(MenuItemIdCenter::storeMenuId(), tr("Grab some apps"));
    connect (grabSomeAppsItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), applicationStorePage, SLOT(relatedMenuItemActivated(QString)));
    menuManager->addRootItem(grabSomeAppsItem);

    // options page
    OptionsContentPage *optionsPage = new OptionsContentPage(PageIdCenter::optionsPageId());
    pageNavigationManager->addRootPage(optionsPage);
    //  related menu
    MenuItemWidget *optionsItem = new MenuItemWidget(MenuItemIdCenter::optionsMenuId(), tr("Options"));
    connect(optionsItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), optionsPage, SLOT(relatedMenuItemActivated(QString)));
    menuManager->addRootItem(optionsItem);

#ifdef ENABLE_DEVELOPER_PREVIEW_FEATURES
    // help page
    HelpPage *helpPage = new HelpPage(PageIdCenter::helpPageId());
    pageNavigationManager->addRootPage(helpPage);
    //  related menu
    MenuItemWidget *helpMenuItem = new MenuItemWidget(MenuItemIdCenter::helpMenuId(), tr("Help"));
    connect(helpMenuItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), helpPage, SLOT(relatedMenuItemActivated(QString)));
    menuManager->addRootItem(helpMenuItem);
#endif

    // client-update page (hidden - no related menu item)
    ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage *clientUpdatePage = new ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage(PageIdCenter::clientUpdateAndInitialLocalAppDatabaseLoaderPageId());
    connect(clientUpdatePage, SIGNAL(finishedWithCheckingAndNoUpdateFound()), this, SLOT(__clientSelfUpdateFinishedWithCheckingAndNoUpdateFound()));
    pageNavigationManager->addRootPage(clientUpdatePage);


    // some connections
    _specialPageActionController = new SpecialPageActionController(this);
    connect(myAppsPage, SIGNAL(showStorePageForApp(ApplicationDescriptionModel)), _specialPageActionController, SLOT(emitShowStoreDetailPageForApp(ApplicationDescriptionModel)));
    //
    connect(_specialPageActionController, SIGNAL(showStoreDetailPageForAppSignal(ApplicationDescriptionModel)), applicationStorePage, SLOT(showStorePageForApp(ApplicationDescriptionModel)));
    connect(_specialPageActionController, SIGNAL(searchInStoreByFilterTextSignal(QString)), applicationStorePage, SLOT(_performAppStoreSearchByFilterTextEmitted(QString)));


    // set and show the initial page
    pageNavigationManager->switchToRootPageStack(PageIdCenter::clientUpdateAndInitialLocalAppDatabaseLoaderPageId(), "mode:ClientUpdateCheckThenLoadLocalAppDatabase");

//    pageNavigationManager->switchToRootPageStack(PageIdCenter::storePageId());
//    pageNavigationManager->switchToRootPageStack(PageIdCenter::myAppsPageId());

    //menuManager->activateItemInNavigationPathById(myAppsRootMenuItemId); // unnecesarry, the page will do it


    // start to check new version
//    this->_isFirstTimeClientUpdateCheck = true;
//    this->_checkForClientUpdateTimer = new QTimer(this);
//    this->_checkForClientUpdateTimer->setSingleShot(false);
//    this->_checkForClientUpdateTimer->setInterval(5000); // for the first time it will check in 5 seconds, then it will modify this to longer intervals
//    connect(this->_checkForClientUpdateTimer, SIGNAL(timeout()), this, SLOT(checkForClientUpdate()));
//    this->_checkForClientUpdateTimer->start();

    _appDatabaseSyncManager = new AppDatabaseSyncManager(this);

    LocalAppDatabaseManager *localAppDatabaseManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager();
    connect(localAppDatabaseManager, SIGNAL(_result_getAppDescriptionModelByAppId(ApplicationDescriptionModel,QString,bool)), this, SLOT(_result_getAppDescriptionModelByAppId(ApplicationDescriptionModel,QString,bool)));
//    connect(localAppDatabaseManager, SIGNAL(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), this, SLOT(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)));

    //
    // window resize delegation
    _windowResizeDelegate = new WidgetResizerDelegate(this, this);
    this->setMouseTracking(true);
    centralWidget->setMouseTracking(true); // have to be set for the central widget as well!
//    centralWidget->installEventFilter(this);

    // --- transparent window
    // it only works if the window is frameless
    // (under Windows) | requires a force-repaint at show - without force repaint the GUI sometimes "freezes"
    setAttribute(Qt::WA_TranslucentBackground, true);


    // --- shortcuts
    new QShortcut(QKeySequence("Ctrl+M"), this, SLOT(showMinimized()));
    new QShortcut(QKeySequence("Ctrl+W"), this, SLOT(hide()));
    new QShortcut(QKeySequence("Ctrl+H"), this, SLOT(hide()));
}

void BrowserWindow::__clientSelfUpdateFinishedWithCheckingAndNoUpdateFound()
{
//    if(!Authorization::sharedAuthorization()->isUserLoggedIn()
    this->connectToUserLoginSignals();
    ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->setIsUpdateChekingAndInitialLoginFinished(true);
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->showNofitications();
    AppwhirrApplication::instance()->getBrowserWindow()->unblockUserNavigationAndShowHiddenRelatedParts();

    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(newVersionFound()), this, SLOT(_clientSelfUpdateNewVersionFound()), Qt::UniqueConnection);
    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(noNewVersionFound()), this, SLOT(_clientSelfUpdateNoNewVersionFound()), Qt::UniqueConnection);
    connect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager(), SIGNAL(failedToGetNewVersion(QString)), this, SLOT(_clientSelfUpdateFailedToGetNewVersion(QString)), Qt::UniqueConnection);

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getClientSelfUpdateManager()->startCheckIsNewVersionAvailablePeriodically();
}

void BrowserWindow::_clientSelfUpdateNewVersionFound() {
    DLog("_clientSelfUpdateNewVersionFound");

    NotificationManager *notifManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager();
    NotificationWidget *notifWidget = notifManager->getNotificationWidgetByItemId("ClientSelfUpdateAvailableNotificationWidget");
    if(notifWidget == NULL) {
        ClientSelfUpdateAvailableNotificationWidget *clientSelfUpdateNotifWidget = new ClientSelfUpdateAvailableNotificationWidget;
        connect(clientSelfUpdateNotifWidget, SIGNAL(startClientSelfUpdate()), this, SLOT(_startClientSelfUpdate()));
        notifManager->addNotificationWithSelfTimeoutingCustomWidget(clientSelfUpdateNotifWidget, "ClientSelfUpdateAvailableNotificationWidget", "ClientSelfUpdateAvailableNotificationWidget");
    }
}

void BrowserWindow::_clientSelfUpdateNoNewVersionFound() {
    DLog("_clientSelfUpdateNoNewVersionFound");
}

void BrowserWindow::_clientSelfUpdateFailedToGetNewVersion(QString errorMessage) {
    DLog("Client-SelfUpdate FailedToGetNewVersion: ") << errorMessage;
}

void BrowserWindow::_startClientSelfUpdate()
{
    this->__switchToClientUpdatePageAndBlockNavigation();
}

void BrowserWindow::__switchToClientUpdatePageAndBlockNavigation()
{
    this->blockUserNavigationWithHidingRelatedParts();
//    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->_getNotificationAreaWidget()->hide();
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->switchToRootPageStack(PageIdCenter::clientUpdateAndInitialLocalAppDatabaseLoaderPageId());
}

void BrowserWindow::__performPostAppInstallActions(ApplicationDescriptionModel appDescrModel)
{
#ifdef Q_OS_WIN
            // if Windows - create start menu shortcut for the app
            if( !AW::Client::Utils::AWShortcutHandler::createSchemeBasedStartMenuShortcutForApp(appDescrModel) )
            {
                WLog("Cannot create start menu shortcut for app: ") << appDescrModel.getDynamicDescription_ReadOnly()->getAppId() << appDescrModel.getDynamicDescription_ReadOnly()->getAppName();
            }
            else {
                DLog("Start menu shortcut created for app: ") << appDescrModel.getDynamicDescription_ReadOnly()->getAppId() << appDescrModel.getDynamicDescription_ReadOnly()->getAppName();
            }
#endif
}

void BrowserWindow::__performPostAppUninstallActions(ApplicationDescriptionModel appDescrModel)
{
#ifdef Q_OS_WIN
    if( !AW::Client::Utils::AWShortcutHandler::removeSchemeBasedStartMenuShortcutForApp(appDescrModel) ) {
        WLog("Cannot remove the app's start menu shortcut");
    }
#endif
}

void BrowserWindow::restoreLastUsedWindowSize()
{
    // set default size
    // -> centerize in screen but with the last window width
    QRect currDesktopGeometry = QApplication::desktop()->availableGeometry(this); // screen geom of this widget
    DLog("Curr desktop geom: ") << currDesktopGeometry;
    QRect windowGeometryToUse = Settings::sharedSettings()->getBrowserWindowGeometry();
    if(windowGeometryToUse.width() == 0 || windowGeometryToUse.height() == 0) {
        // invalid width / height
        //  -> centerize
        int windowPosX = (int)( ((double)currDesktopGeometry.width() / 2.0) - ((double)this->geometry().width() / 2.0) );
        int windowPosY = (int)( ((double)currDesktopGeometry.height() / 2.0) - ((double)this->geometry().height() / 2.0) );
        WLog("Invalid screen geom - centerizes: ") << windowPosX << windowPosY;
        windowGeometryToUse = QRect(windowPosX, windowPosY, this->geometry().width(), this->geometry().height());
    }

    else
    {
        // valid width and height - check whether it will be in the current desktop or not and correct it if required
        int windowGeomToUseWidth = windowGeometryToUse.width();
        int windowGeomToUseHeight = windowGeometryToUse.height();

        if( (windowGeometryToUse.x() + windowGeometryToUse.width()) > (currDesktopGeometry.x() + currDesktopGeometry.width()) ) {
            // out from the desktop in the right side
            windowGeometryToUse.setX(currDesktopGeometry.x() + currDesktopGeometry.width() - windowGeometryToUse.width());
        }
        if( (windowGeometryToUse.y() + windowGeometryToUse.height()) > (currDesktopGeometry.y() + currDesktopGeometry.height()) ) {
            // out from the desktop in the bottom
            windowGeometryToUse.setY(currDesktopGeometry.y() + currDesktopGeometry.height() - windowGeometryToUse.height());
        }
        if(windowGeometryToUse.x() < 0) {
            // out from the desktop in the left side
            windowGeometryToUse.setX(0);
        }
        if(windowGeometryToUse.y() < 0) {
            // out from the desktop in the top
            windowGeometryToUse.setY(0);
        }

        windowGeometryToUse.setWidth(windowGeomToUseWidth);
        windowGeometryToUse.setHeight(windowGeomToUseHeight);
    }

    DLog("Window geom will be used: ") << windowGeometryToUse;
    this->setGeometry(windowGeometryToUse);
}

void BrowserWindow::blockUserNavigationWithHidingRelatedParts() {
//    this->hideUserLoginSidebar();
//    this->sideBarWidget->switchToCollapsedState();
//    GlobalObjectManager::sharedGlobalObjectRegistry()->getNotificationManager()->_getNotificationAreaWidget()->hide();

    this->sideBarWidget->switchToCollapsedStateSlot();
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->_getNotificationAreaWidget()->hide();
}

void BrowserWindow::unblockUserNavigationAndShowHiddenRelatedParts() {
//    this->showUserLoginSidebar();
    this->sideBarWidget->switchToExpandedStateSlot();
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->_getNotificationAreaWidget()->show();
}


// ------------------------
// --- grab connections ---

void BrowserWindow::_appGrabStarted(QString appId, QString appName)
{
    // store in app-database


    // add the related notification

    // test whether it's already added
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("grab-" + appId);
    if(relatedAppInstallNotificationWidget == NULL) {
        // add only if it's not yet added
        InstallProgressNotificationWidget *installNotificationWidget = new InstallProgressNotificationWidget(appId, appName);
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addNotificationWithSelfTimeoutingCustomWidget(installNotificationWidget, "Install process notification for appId: " + appId, "grab-" + appId);

        // mark as critical-action
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->addCriticalAction(AppWhirr::CriticalActionCodes::AppInstallOrUpdateActive);
    }
}

void BrowserWindow::_appGrabbedSuccessfully(QString appId, QString appName, QString appSupportedSummedPlatformFlags, QString newestVersionAvailableForThisPlatform, AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore)
{
    DLog("_appGrabbedSuccessfully");
    // app-database operation
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->addOrUpdateGrabbedApp(appId, appName, appSupportedSummedPlatformFlags, newestVersionAvailableForThisPlatform, appStatusInStore);

    // notification
//    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("grab-" + appId);
//    if(relatedAppInstallNotificationWidget != NULL) {
//        InstallProgressNotificationWidget *castedInstallProgressNotificationWidget = dynamic_cast<InstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());

//        if(castedInstallProgressNotificationWidget != NULL)
//        {
//            castedInstallProgressNotificationWidget->setInstallStarted();
//        }
//    }
}

void BrowserWindow::_appGrabFailed(QString appId, QString appName, bool isUpdate, QString errorMessage)
{
    // app-database operation
    if(isUpdate) {
        // back to installed state
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(appId, AppWhirr::ApplicationGrabStates::Installed, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "BrowserWindow::_appGrabFailed");
    }
    else {
        // back to grabbed state
        FLAG_FOR_REVIEW_WITH_HINT("This should be able to check whether the app is grabbed or not. If the user starts in Offline mode, then starts a grab for an app but clicks Cancel in the user-login-dialog it will emit a 'grab failed' signal which is connected to this slot. But in this case the app is not really grabbed, but will be 'downgraded' as grabbed, although it's not.");
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(appId, AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "BrowserWindow::_appGrabFailed");
//        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(appId, AppWhirr::ApplicationGrabStates::NoState, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "BrowserWindow::_appGrabFailed");
    }

    // notification
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("grab-" + appId);
    if(relatedAppInstallNotificationWidget != NULL) {
        InstallProgressNotificationWidget *castedInstallProgressNotificationWidget = dynamic_cast<InstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
        if(castedInstallProgressNotificationWidget != NULL) {
            castedInstallProgressNotificationWidget->setGrabFailedWithError(errorMessage);

            // finished with critical action
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->removeCriticalAction(AppWhirr::CriticalActionCodes::AppInstallOrUpdateActive);
        }
    }
}

void BrowserWindow::_appDownloadProgress(QString appId, int progress)
{
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("grab-" + appId);
    if(relatedAppInstallNotificationWidget != NULL) {
        InstallProgressNotificationWidget *castedInstallProgressNotificationWidget = dynamic_cast<InstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
        if(castedInstallProgressNotificationWidget != NULL) {
            castedInstallProgressNotificationWidget->setDownloadProgress(progress);
        }
    }
}

void BrowserWindow::_appInstallationStarted(QString appId, QString appName)
{
    DLog("_appInstallationStarted");
    // app-database operation
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(appId, AppWhirr::ApplicationGrabStates::GrabbedAndInstalling, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "BrowserWindow::_appInstallationStarted");

    // notification
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("grab-" + appId);
    if(relatedAppInstallNotificationWidget != NULL) {
        InstallProgressNotificationWidget *castedInstallProgressNotificationWidget = dynamic_cast<InstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
        if(castedInstallProgressNotificationWidget != NULL) {
            castedInstallProgressNotificationWidget->setInstallStarted();
        }
    }
}

void BrowserWindow::_appInstallProgressStateMessage(QString appId, QString progressStateMessage)
{
    // notification
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("grab-" + appId);
    if(relatedAppInstallNotificationWidget != NULL) {
        InstallProgressNotificationWidget *castedInstallProgressNotificationWidget = dynamic_cast<InstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
        if(castedInstallProgressNotificationWidget != NULL) {
            castedInstallProgressNotificationWidget->setInstallProgressStateMessage(progressStateMessage);
        }
    }
}

void BrowserWindow::_appInstalledSuccessfullyToDirectory(QString appId, QString appName, QString installedAppRootDir, QString installedAppVersion)
{
    // app-database operation
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->addInstalledApp(appId, appName, installedAppRootDir, installedAppVersion);

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->getAppDescriptionModelByAppId(appId, "BrowserWindow::_appInstalledSuccessfullyToDirectory->ForPostInstallActions");

    // notification
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("grab-" + appId);
    if(relatedAppInstallNotificationWidget != NULL) {
        InstallProgressNotificationWidget *castedInstallProgressNotificationWidget = dynamic_cast<InstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
        if(castedInstallProgressNotificationWidget != NULL) {
            castedInstallProgressNotificationWidget->setInstalled();

            // finished with critical action
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->removeCriticalAction(AppWhirr::CriticalActionCodes::AppInstallOrUpdateActive);

            DLog("[install] Critical action test: ") << GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->isThisCriticalActionActive(AppWhirr::CriticalActionCodes::AppInstallOrUpdateActive);
            DLog("Is any critical action? ") << GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->isAnyCriticalActionActive();
        }
    }
}

void BrowserWindow::_appInstallFailed(QString appId, QString appName, bool isUpdate, QString errorMessage)
{
    // app-database operation
    if(isUpdate) {
        // go back to installed state
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(appId, AppWhirr::ApplicationGrabStates::Installed, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "BrowserWindow::_appInstallFailed");
    }
    else {
        // back to grabbed state
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(appId, AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "BrowserWindow::_appInstallFailed");
    }

    // notification
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("grab-" + appId);
    if(relatedAppInstallNotificationWidget != NULL) {
        InstallProgressNotificationWidget *castedInstallProgressNotificationWidget = dynamic_cast<InstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
        if(castedInstallProgressNotificationWidget != NULL) {
            castedInstallProgressNotificationWidget->setInstallFailedWithError(errorMessage);

            // finished with critical action
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->removeCriticalAction(AppWhirr::CriticalActionCodes::AppInstallOrUpdateActive);
        }
    }
}

void BrowserWindow::__resolutionNeededForRunningApp(QString appId, QString appName)
{
    __appIdsForInstallResolutionList.append(appId);
    // get app details from database
    DLog("Given appId: ") << appId;
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->getAppDescriptionModelByAppId(appId, "BrowserWindow::__resolutionNeededForRunningApp");
}

// -------------
// --- framework install prompt

void BrowserWindow::__frameworksRequireInstallForApp(QString appId, QString appName)
{
    // create notification
    NotificationWidget *relatedNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("allow_deny_fw_install_for_app-" + appId);
    if(relatedNotificationWidget == NULL) {

        TextAndAcceptCancelButtonsNotificationWidget *allowDenyAppFwInstallNotificationWidget = new TextAndAcceptCancelButtonsNotificationWidget(tr("%1 requires some frameworks to install. Do you allow it?").arg(appName), tr("Allow"), tr("Deny"), true);
        connect(allowDenyAppFwInstallNotificationWidget, SIGNAL(acceptOptionSelected()), this, SLOT(_allowFrameworkInstall()));
        connect(allowDenyAppFwInstallNotificationWidget, SIGNAL(cancelOptionSelected()), this, SLOT(_denyFrameworkInstall()));
        allowDenyAppFwInstallNotificationWidget->setOptionalId(appId);

        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addNotificationWithSelfTimeoutingCustomWidget(allowDenyAppFwInstallNotificationWidget, appId + "|" + appName + " requires some frameworks to install. Do you allow it?", "allow_deny_fw_install_for_app-" + appId);

        __appRequiresFrameworkInstallAllowOrDeny_AppId = appId;
    }
    else {
        WLog("Fw install allow/deny notification already presented. Won't present a new one.");
    }
}

void BrowserWindow::_allowFrameworkInstall()
{
    TextAndAcceptCancelButtonsNotificationWidget *relatedNotificationCustomWidget = dynamic_cast<TextAndAcceptCancelButtonsNotificationWidget *>(sender());
    if(relatedNotificationCustomWidget == NULL) {
        WLog("Invalid type of sender!");
        return;
    }

    // get the related appId and appName
    QString relatedAppId = relatedNotificationCustomWidget->getOptionalId();

    if(relatedAppId.isEmpty() || relatedAppId != __appRequiresFrameworkInstallAllowOrDeny_AppId) {
        WLog("Cannot find the related appId for allow/deny fw install") << relatedAppId << __appRequiresFrameworkInstallAllowOrDeny_AppId;
        return;
    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->__allowFrameworkInstallForApp(relatedAppId);
}

void BrowserWindow::_denyFrameworkInstall()
{
    TextAndAcceptCancelButtonsNotificationWidget *relatedNotificationCustomWidget = dynamic_cast<TextAndAcceptCancelButtonsNotificationWidget *>(sender());
    if(relatedNotificationCustomWidget == NULL) {
        WLog("Invalid type of sender!");
        return;
    }

    // get the related appId and appName
    QString relatedAppId = relatedNotificationCustomWidget->getOptionalId();

    if(relatedAppId.isEmpty() || relatedAppId != __appRequiresFrameworkInstallAllowOrDeny_AppId) {
        WLog("Cannot find the related appId for allow/deny fw install") << relatedAppId << __appRequiresFrameworkInstallAllowOrDeny_AppId;
        return;
    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->__denyFrameworkInstallForApp(relatedAppId);
}



// ------------------------------------------
// --- local app database operation slots ---

void BrowserWindow::_result_getAppDescriptionModelByAppId(ApplicationDescriptionModel resultAppDescription, QString dbOperationId, bool isQuerySuccess)
{
    if(dbOperationId == "BrowserWindow::__resolutionNeededForRunningApp") {
        if(isQuerySuccess) {
            // start termination
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getAppTerminationController()->terminateAppUnderDirectory(resultAppDescription.getDynamicDescription_ReadOnly()->getAppId(), resultAppDescription.getDynamicDescription_ReadOnly()->getAppName(), resultAppDescription.getDynamicDescription_ReadOnly()->getInstalledRootPath(), AppTerminationController::TRC_TerminateForInstallOrUpdate);
        }
        else {
            __appIdsForInstallResolutionList.removeOne(resultAppDescription.getDynamicDescription_ReadOnly()->getAppId());
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->__resolutionFailed(resultAppDescription.getDynamicDescription_ReadOnly()->getAppId());
        }
    }
    else if(dbOperationId == "BrowserWindow::_appInstalledSuccessfullyToDirectory->ForPostInstallActions") {
        if(isQuerySuccess)
        {
            this->__performPostAppInstallActions(resultAppDescription);
        }
    }
}

//void BrowserWindow::_result_getAllAppDescriptions(QList<ApplicationDescriptionModel> appDescriptions, QString dbOperationId, bool isQuerySuccess)
//{
//    if(dbOperationId == "BrowserWindow::_startOfflineAppListUpdateCheckForCurrentUser") {
//        if(isQuerySuccess) {
//            this->__doOfflineAppListUpdateCheckForTheseApps(appDescriptions);
//        }
//        else {
//            WLog("Failed to get all-apps from database.");
//        }
//    }
//}




void BrowserWindow::__applicationTerminatedSuccesfully(QString appId, QString appName)
{
    bool isFound = false;
    for(int i = 0; i < __appIdsForInstallResolutionList.size() && !isFound; i++) {
        if(__appIdsForInstallResolutionList[i] == appId) {
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->__resolutionDoneRetryInstall(appId);
            isFound = true;
        }
    }

    if(isFound) {
        __appIdsForInstallResolutionList.removeOne(appId);
    }
}

void BrowserWindow::__cannotTerminateApplication(QString appId, QString appName)
{
    bool isFound = false;
    for(int i = 0; i < __appIdsForInstallResolutionList.size() && !isFound; i++) {
        if(__appIdsForInstallResolutionList[i] == appId) {
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->__resolutionFailed(appId);
            isFound = true;
        }
    }

    if(isFound) {
        __appIdsForInstallResolutionList.removeOne(appId);
    }
}

//void BrowserWindow::appInstallStarted(QString appId) {
////    QString notifText = tr("Application install started: ");
////    notifText += appName;
////    GlobalObjectManager::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(notifText, 10000, NotificationManager::LowPriority);

//    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("grab-" + appId);
//    if(relatedAppInstallNotificationWidget != NULL) {
//        InstallProgressNotificationWidget *castedInstallProgressNotificationWidget = dynamic_cast<InstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
//        if(castedInstallProgressNotificationWidget != NULL) {
//            castedInstallProgressNotificationWidget->setInstallStarted();
//        }
//    }
//}

//void BrowserWindow::appGrabbedSuccessfully(QString appId)
//{
////    QString relatedAppName = GlobalObjectManager::sharedGlobalObjectRegistry()->getLocalApplicationDatabaseManager()->getAppNameById(appId);
////    QString notifText = tr("Application grab finished: ");
////    notifText += relatedAppName + "[id: " + appId + "]";
////    GlobalObjectManager::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(notifText, 10000, NotificationManager::LowPriority);

//    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("grab-" + appId);
//    if(relatedAppInstallNotificationWidget != NULL) {
//        InstallProgressNotificationWidget *castedInstallProgressNotificationWidget = dynamic_cast<InstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
//        if(castedInstallProgressNotificationWidget != NULL) {
//            castedInstallProgressNotificationWidget->setGrabbed();
//        }
//    }
//}

void BrowserWindow::ensureWindowPresented()
{
    this->showNormal();
    this->raise();
    this->activateWindow();

    this->setFocus();
}

void BrowserWindow::paintEvent(QPaintEvent *e)
{
    // make the window as semi-transparent
    // it also requires to set
    // setAttribute(Qt::WA_TranslucentBackground); // it only works if the window is frameless (under Windows)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
//    painter.setBrush(Qt::LinearGradientPattern);

    QRect topRect(0, 0, this->rect().width(), 50);
    QPainterPath path;
    path.addRoundedRect(topRect, 6, 6);
    painter.fillPath(path, QColor(0, 0, 0, 100));

//    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    QRect lowerRect(0, 40, this->rect().width(), this->rect().height() - 30);
    painter.fillRect(lowerRect, QColor(0, 0, 0, 100));


    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//bool BrowserWindow::eventFilter(QObject *obj, QEvent *e)
//{
//    if(e->type() == QEvent::MouseButtonPress)
//    {
//        QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);
//        if(me)
//        {
//            _windowResizeDelegate->delegateMousePressEvent(me);
//        }
//        else {
//            WLog("Cannot cast");
//        }
//    }
////    else if(e->type() == QEvent::MouseMove)
//    else if(e->type() == QEvent::HoverMove)
//    {
//        DLog("Move");
//        QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);
//        if(me)
//        {
//            _windowResizeDelegate->delegateMouseMoveEvent(me);
//        }
//        else {
//            WLog("Cannot cast");
//        }
//    }
//    else if(e->type() == QEvent::MouseButtonRelease)
//    {
//        QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);
//        if(me)
//        {
//            _windowResizeDelegate->delegateMouseReleaseEvent(me);
//        }
//        else {
//            WLog("Cannot cast");
//        }
//    }
////    else if(e->type() == QEvent::HoverLeave)
////    {
////        DLog("Leave!");
////    }

//    return QMainWindow::eventFilter(obj, e);
//}

// Event handling
bool BrowserWindow::event(QEvent *e) {
    if (e->type() == QEvent::FocusIn) {
        QApplication::postEvent(QApplication::instance(), new ForceHideOrMinimizeEvent());
        e->accept();
        return true;
    }

//    else if (e->type() == QEvent::Close) {
////        QApplication::postEvent(QApplication::instance(), new ForceHideOrMinimizeBrowserEvent());
////        e->ignore();
//        Settings::sharedSettings()->setBrowserWindowGeometry(this->geometry());
//        e->accept();
//        return true;
//    }

    else if(e->type() == EnsureApplicationPresentedEvent::getCustomEventType()) {
        this->ensureWindowPresented();
        e->accept();
        return true;
    }

    else if(e->type() == QEvent::WindowStateChange) {
        if(this->windowState() != Qt::WindowMinimized) {
            this->repaint(); // force repaint - needed for WA_TranslucentBackground
        }
    }

    if(e->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);
        if(me)
        {
            _windowResizeDelegate->delegateMousePressEvent(me);
        }
        else {
            WLog("Cannot cast");
        }
    }
//    else if(e->type() == QEvent::MouseMove)
    else if(e->type() == QEvent::HoverMove)
    {
//        DLog("Move");
        QHoverEvent *me = dynamic_cast<QHoverEvent *>(e);
        if(me)
        {
//            _windowResizeDelegate->delegateMouseMoveEvent(me);
            _windowResizeDelegate->delegateMouseMoveHoverEvent(me);
        }
        else {
            WLog("Cannot cast");
        }
    }
    else if(e->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);
        if(me)
        {
            _windowResizeDelegate->delegateMouseReleaseEvent(me);
        }
        else {
            WLog("Cannot cast");
        }
    }
    else if (e->type() == QEvent::Move ||
             e->type() == QEvent::Resize) {
        Settings::sharedSettings()->setBrowserWindowGeometry(this->geometry());
        e->accept();
    }

    return QMainWindow::event(e);
}

//void BrowserWindow::mousePressEvent(QMouseEvent *e)
//{
//    _windowResizeDelegate->delegateMousePressEvent(e);
//}

//void BrowserWindow::mouseMoveEvent(QMouseEvent *e)
//{
//    _windowResizeDelegate->delegateMouseMoveEvent(e);
//}

//void BrowserWindow::mouseReleaseEvent(QMouseEvent *e)
//{
//    _windowResizeDelegate->delegateMouseReleaseEvent(e);
//}

//void BrowserWindow::leaveEvent(QEvent *e)
//{
//    _windowResizeDelegate->delegateMouseLeaveEvent(e);
//}

// -------------------------------------------
// --- some page action/navigation methods ---

void BrowserWindow::showMyAppsPage()
{
    if(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->isThisPageIsTheCurrentActiveOne(PageIdCenter::clientUpdateAndInitialLocalAppDatabaseLoaderPageId())) {
        // do nothing - it's still client-update mode
        return;
    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popBackToThisPageAndSwitchStackIfRequired(PageIdCenter::myAppsPageId());
}

void BrowserWindow::showStorePage()
{
    if(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->isThisPageIsTheCurrentActiveOne(PageIdCenter::clientUpdateAndInitialLocalAppDatabaseLoaderPageId())) {
        // do nothing - it's still client-update mode
        return;
    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popBackToThisPageAndSwitchStackIfRequired(PageIdCenter::storePageId());
}

void BrowserWindow::showMyAppsPageAutostartAppsOnly()
{
    if(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->isThisPageIsTheCurrentActiveOne(PageIdCenter::clientUpdateAndInitialLocalAppDatabaseLoaderPageId())) {
        // do nothing - it's still client-update mode
        return;
    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popBackToThisPageAndSwitchStackIfRequired(PageIdCenter::myAppsPageId(), QString("MyApps/ShowOnlyAutostartApps"));
}

void BrowserWindow::showFeedbackPage(QString defaultFeedbackText)
{
    if(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->isThisPageIsTheCurrentActiveOne(PageIdCenter::clientUpdateAndInitialLocalAppDatabaseLoaderPageId())) {
        // do nothing - it's still client-update mode
        return;
    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(PageIdCenter::optionsPageId(), new FeedbackPage(PageIdCenter::feedbackPageId()), FeedbackPage::getPageParameterForDefaultText(defaultFeedbackText));
}

void BrowserWindow::showSuggestAnAppOptionsPage(QString appName)
{    
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(PageIdCenter::optionsPageId(), new SuggestAnAppPage(PageIdCenter::suggestAnAppOptionsSubPageId()), "appName:"+appName);
}

void BrowserWindow::_showStoreDetailPageForAppId(QString appId)
{
    DLog("Show store page for appId: ") << appId;
//    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->getAppDescriptionModelByAppId(appId, "BrowserWindow::_showStoreDetailPageForAppId");
    ApplicationDescriptionModel tmpAppDescr;
    tmpAppDescr.getDynamicDescription_DataForModification()->setAppId(appId);
    tmpAppDescr.getStaticDescription_DataForModification()->_setAppId(appId);
    this->_specialPageActionController->emitShowStoreDetailPageForApp(tmpAppDescr);
}

void BrowserWindow::_showSearchInStorePageAndSearchForFilterText(QString searchFilterText)
{
    DLog("Search in store: ") << searchFilterText;
    this->_specialPageActionController->emitSearchInStoreByFilterText(searchFilterText);
}

//void BrowserWindow::_result_getAppDescriptionModelByAppId(ApplicationDescriptionModel appDescriptionModel, QString dbOperationid, bool isQuerySuccess)
//{
//    DLog("-------Result") << dbOperationid << isQuerySuccess;

//    if(dbOperationid == "BrowserWindow::_showStoreDetailPageForAppId") {
//        if(isQuerySuccess) {
//            this->_specialPageActionController->showStoreDetailPageForAppSlot(appDescriptionModel);
//        }
//        else {
//            // not found in the db
//            ApplicationDescriptionModel appDescrForDetailPage;
//            appDescrForDetailPage.getDynamicDescription_DataForModification()->setAppId();
//        }
//    }
//}

// ------------------------------------
// --- window state manager buttons ---

void BrowserWindow::windowStateMinimizeButtonPressed()
{
    this->showMinimized();
}

void BrowserWindow::windowStateMaximizeButtonPressed()
{
//#ifdef Q_WS_WIN
    // Windows related issue: if showMaximized() called on a FramelessWindow it will be presented as full-screen (covering the StartMenu as well)
    //  issue: https://bugreports.qt.nokia.com/browse/QTBUG-8361?focusedCommentId=166850#comment-166850
    if(__winWorkaround_isMaximized) {
        this->setGeometry(__winWorkaround_geometryBeforeMaximized);
        __winWorkaround_isMaximized = false;
    }
    else {
        __winWorkaround_geometryBeforeMaximized = this->geometry();
        this->setGeometry(QApplication::desktop()->availableGeometry(this));
        __winWorkaround_isMaximized = true;
    }
//#else
//    if(this->isMaximized()) {
//        this->showNormal();
//    }
//    else {
//        this->showMaximized();
//    }
//#endif
}

void BrowserWindow::windowStateCloseButtonPressed()
{
    this->hide();
}

// ------------------
// --- user login ---

void BrowserWindow::actualizeUserNameTitleText()
{
    QString userName = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->getCurrentUserName();
    if(Authorization::sharedAuthorization()->isUserLoggedIn()) {
//        userName += " [online]";
    }
    else {
        userName += " [offline]";
    }
    this->navigationWidget->setUserNameText(userName);
}

void BrowserWindow::_appDidUninstall(ApplicationDescriptionModel appDescrModel)
{
    this->__performPostAppUninstallActions(appDescrModel);
}

void BrowserWindow::connectToUserLoginSignals()
{
    disconnect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(userLoggedIn()));
    disconnect(Authorization::sharedAuthorization(), SIGNAL(userLoginAborted()), this, SLOT(userLoginAborted()));

    connect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(userLoggedIn()));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginAborted()), this, SLOT(userLoginAborted()));
}

void BrowserWindow::_startGrabbedAppSync()
{
    DLog("_startGrabbedAppSync");

//    this->_getAllGrabbedAppForCurrentUser();
    _appDatabaseSyncManager->startCurrentUserGrabSync();
    // also switch to frequent mode or reset it's timer
    _appDatabaseSyncManager->switchToSyncFrequentlyMode();
}

void BrowserWindow::_startAppListInfoSyncForCurrentUser()
{
    DLog("_startAppListInfoSyncForCurrentUser");

    _appDatabaseSyncManager->startCurrentUserAppListInfoSync();
    // also switch to frequent mode or reset it's timer
    _appDatabaseSyncManager->switchToSyncFrequentlyMode();
}

void BrowserWindow::showUserLoginSidebar() {
//    if (!userLoginDialog) {
//        userLoginDialog = new UserLoginDialog(this);
//        //userLoginDialog->setModal(true);
//        userLoginDialog->show();
//    } else {
//        userLoginDialog->failedToLogin();
//    }

    if(!this->sideBarWidget->getIsExpanded()) {
        // if the main-menu sidebar is already collapsed then present the login sidebar immediatelly if not yet presented
        if(!this->_userLoginSidebar->getIsExpanded()) {
            this->_userLoginSidebar->actualizeUsersList();
            this->_userLoginSidebar->switchToExpandedStateSlot();
            this->_isShowLoginSidebarRequested = false;
        }
    }
    else if(!this->_userLoginSidebar->getIsExpanded()) {
        // wait for the main sidebar to hide then present the user login sidebar

        _isShowLoginSidebarRequested = true;
        // hide the menu -> the user login sidebar will be presented after the sidebar switch to collaped state
        this->sideBarWidget->switchToCollapsedStateSlot();
    }
}

void BrowserWindow::hideUserLoginSidebar() {
//    if (!userLoginDialog) {
//        userLoginDialog = new UserLoginDialog(this);
//        //userLoginDialog->setModal(true);
//        userLoginDialog->show();
//    } else {
//        userLoginDialog->failedToLogin();
//    }

    if(this->_userLoginSidebar->getIsExpanded()) {
        this->_userLoginSidebar->switchToCollapsedStateSlot();
    }

    // also show the menu
    this->sideBarWidget->switchToExpandedStateSlot();
}

void BrowserWindow::_mainMenuSidebarSwitchedToCollapsedState()
{
    if(this->_isShowLoginSidebarRequested) {
        this->_userLoginSidebar->actualizeUsersList();
        this->_userLoginSidebar->switchToExpandedStateSlot();
        this->_isShowLoginSidebarRequested = false;
    }
}

void BrowserWindow::_mainMenuSidebarSwitchedToExpandedState()
{

}

void BrowserWindow::userLoggedIn()
{
    DLog("User logged in");
//    Authorization::sharedAuthorization()->_userLoginHappened();
    this->hideUserLoginSidebar();
    // and get all grabbed apps for the user
    _appDatabaseSyncManager->startCurrentUserGrabSync();

    this->actualizeUserNameTitleText();
}

void BrowserWindow::userLoginCommunicationError(QString errorMessage)
{
    DLog("User login comm error: ") << errorMessage;
//    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("Login error happened: %1").arg(errorMessage), 10000, NotificationManager::ErrorPriority);
}

void BrowserWindow::userLoginAborted()
{
//    Authorization::sharedAuthorization()->_userLoginAbortHappened();
    this->hideUserLoginSidebar();

    this->actualizeUserNameTitleText();
}

// -----------------------------------
// --- current user grab-list-sync ---



// --------------------------------------------------
// --- current user offline app-list update check ---


