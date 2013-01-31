#include "myappspage.h"

// UI
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>

#include "Common/Widgets/contentcoverstatusmessagewidget.h"

// QT
#include <QPushButton>
#include <QKeyEvent>

// browser-window
#include "Browser/browserwindow.h"

// menu
#include "Common/Menu/menumanager.h"
#include "Common/Menu/menuitemwidget.h"
#include "Settings/menuitemidcenter.h"
// page nav
#include "Common/Page/pagenavigationmanager.h"
#include "Settings/pageidcenter.h"
// global objects
#include "Application/globalobjectregistry.h"
#include "Browser/Notification/notificationmanager.h"
#include "Application/Managers/AppGrab/grabapplicationmanager.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"
#include "Application/Controllers/appterminationcontroller.h"
#include "Application/CriticalAction/criticalactioncodes.h"
#include "Application/CriticalAction/criticalactionmanager.h"

// delegates
#include "Common/Sorting/cleverapplicationiconpresentersortingdelegate.h"
#include "../../AppUninstall/appuninstaller.h"

// application level
#include "Application/appwhirrapplication.h"

// presenter widget
#include "Common/Widgets/dynamicsamesizehorizontalboxlayoutbasedsmartscrollarea.h"
#include "Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatebase.h"
#include "Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatewithfading.h"

#include "Common/Widgets/textandacceptcancelbuttonsnotificationwidget.h"

// user authorization
#include "Common/Security/authorization.h"

// other, mainly utility or temporarily used
#include "Application/Utils/awapplicationrunner.h"
#include "Application/Utils/awshortcuthandler.h"
#include "Browser/Content/Widget/appiconandbuttonpresenterwidget.h"
#include "Common/Sorting/widgetlistsnapshothandlerdelegate.h"
#include "Common/IdBasedWidgetCollection/idbasedwidgetcollection.h"
#include "Common/Event/startthisapplicationevent.h"
#include "Browser/Content/Widget/myappsmanagementsidebar.h"
#include "Common/Widgets/iconimagewidget.h"
#include "Browser/Content/Page/applicationstoreapplicationpage.h"
#include "Common/Helpers/pathhelper.h"
#include "Common/Event/showstorepageforapplicationevent.h"
#include "Application/User/usermanager.h"
#include "Common/Helpers/platformflagshelper.h"

#include "Browser/Notification/notificationwidget.h"
#include "Common/Widgets/uninstallprogressnotificationwidget.h"
#include "Common/Application/applicationterminator.h"


// config
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"

#define MY_APPS_SEARCH_SUB_MENU_ITEM_ID                         "id-MyApps-SearchMenuItem"
#define MY_APPS_SHOW_AUTOSTART_APPS_SUB_MENU_ITEM_ID            "id-MyApps-ShowAutostartAppsSubMenuItem"
#define MY_APPS_SHOW_ONLY_INSTALLED_APPS_SUB_MENU_ITEM_ID       "id-MyApps-ShowOnlyInstalledAppsSubMenuItem"
#define MY_APPS_SHOW_ONLY_HIDDEN_APPS_SUB_MENU_ITEM_ID          "id-MyApps-ShowOnlyHiddenAppsSubMenuItem"



MyAppsPage::MyAppsPage(QString pageId, QWidget *parent) :
        BrowserPageWidgetBase(pageId, parent),
    _newInstalledAppsSinceLastPageView(0),
    searchLineInMenu(NULL),
    _isShouldGrabTheFocus(false),
    _currPageMode(ShowAllApps),
    _appTerminator(NULL)
{
//    mainLayout = new QVBoxLayout;
//    this->setLayout(mainLayout);
//    mainLayout->setContentsMargins(0, 0, 0, 0);
//    mainLayout->setSpacing(0);
//    this->setContentsMargins(0, 0, 0, 0);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->setStyleSheet(guiConfManager->getStyleForId("MyApps/MainWidget/style"));

    _allIdBasedWidgets = new IdBasedWidgetCollection(this);

    mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    this->setContentsMargins(0, 0, 0, 0);

    //
    // no apps
    _noAppsStatusHolderWidget = new QWidget;
    QVBoxLayout *noAppsContentHolderLayout = new QVBoxLayout;
    _noAppsStatusHolderWidget->setLayout(noAppsContentHolderLayout);
    mainLayout->addWidget(_noAppsStatusHolderWidget);

    _noAppsStatusMessageLabel = new QLabel;
    _noAppsStatusMessageLabel->setText(tr("No apps found."));
    _noAppsStatusMessageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _noAppsStatusMessageLabel->setStyleSheet(guiConfManager->getStyleForId("Label/ExtraLarge/DarkGreen/style"));
    noAppsContentHolderLayout->addWidget(_noAppsStatusMessageLabel);


    //
    // normal content
    _normalContentHolderWidget = new QWidget;
    QVBoxLayout *contentHolderLayout = new QVBoxLayout;
    contentHolderLayout->setSpacing(0);
    contentHolderLayout->setContentsMargins(0, 0, 0, 0);
    _normalContentHolderWidget->setLayout(contentHolderLayout);
    _normalContentHolderWidget->setContentsMargins(0, 0, 0, 0);

    // the main widget - which will contain the app list
    SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate = new SameSizeLayoutSnapshotChangeDelegateBase;
    connect(layoutSnapshotChangeDelegate, SIGNAL(layoutChangeFinished()), this, SLOT(_layoutChangeFinished()));
//    SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate = new SameSizeLayoutSnapshotChangeDelegateWithFading;

//    connect(layoutSnapshotChangeDelegate, SIGNAL(layoutChangeFinished()), this, SLOT(update()));
    appIconPresentersHolderDynamicScrollArea = new DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea(GuiConfigurations::applicationIconPresenterWithButtonWidgetFixedSize(), layoutSnapshotChangeDelegate, false);
    appIconPresentersHolderDynamicScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentHolderLayout->addWidget(appIconPresentersHolderDynamicScrollArea);

    //
    // delegates
    this->cleverSortingDelegate = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCleverSortingDelegate();


    connect(this->cleverSortingDelegate, SIGNAL(sortingHappened(WidgetListSnapshot)), appIconPresentersHolderDynamicScrollArea, SLOT(_commitSnapshotWithThisOrderAndSetAsCurrentWorking(WidgetListSnapshot)));
    connect(this->cleverSortingDelegate, SIGNAL(sortingHappened(WidgetListSnapshot)), this, SLOT(_sortingHappened(WidgetListSnapshot)));

    //
    // sidebar
    appManagementSidebarWidget = new MyAppsManagementSidebar(QSize(0, 0), QSize(200, 50), false);
    appManagementSidebarWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    connect(appManagementSidebarWidget, SIGNAL(runThisApp(ApplicationDescriptionModel)), this, SLOT(_runThisApp(ApplicationDescriptionModel)));
    connect(appManagementSidebarWidget, SIGNAL(installThisApp(ApplicationDescriptionModel)), this, SLOT(_installThisApp(ApplicationDescriptionModel)));
//    connect(appManagementSidebarWidget, SIGNAL(updateThisApp(ApplicationDescriptionModel)), this, SLOT(_installThisApp(ApplicationDescriptionModel)));
    connect(appManagementSidebarWidget, SIGNAL(removeThisApp(ApplicationDescriptionModel)), this, SLOT(removeThisAppByAppId(ApplicationDescriptionModel)));
    connect(appManagementSidebarWidget, SIGNAL(showAppDetails(ApplicationDescriptionModel)), this, SLOT(showAppDetailsByAppId(ApplicationDescriptionModel)));
    connect(appManagementSidebarWidget, SIGNAL(createDesktopShortcutForThisApp(ApplicationDescriptionModel)), this, SLOT(createDesktopShortcutForThisApp(ApplicationDescriptionModel)));
    connect(appManagementSidebarWidget, SIGNAL(isStartThisAppWhenClientStartsChangedSignal(QString,bool)), this, SLOT(_isStartThisAppWhenClientStartsChangedSlot(QString,bool)));
    connect(appManagementSidebarWidget, SIGNAL(isThisAppHiddenChangedSignal(QString,bool)), this, SLOT(_isThisAppHiddenChangedSlot(QString,bool)));

    connect(appManagementSidebarWidget, SIGNAL(switchToExpandedStateFinished()), this, SLOT(_appManagementSidebarSwitchedToExpandedState()));
    connect(appManagementSidebarWidget, SIGNAL(switchToCollapsedStateFinished()), this, SLOT(_appManagementSidebarSwitchedToCollapsedState()));

    //
    // add main content holder widgets to main layout
    mainLayout->addWidget(_normalContentHolderWidget);
    mainLayout->addWidget(appManagementSidebarWidget);

    _normalContentHolderWidget->hide();

    // get notification about new installed apps
//    connect(MyApplication::instance()->getLocalApplicationScanner(), SIGNAL(applicationScanEndedAndNewItemsFound()),
//            this, SLOT(newLocalAppsFoundDuringScan())); // maybe not the best here...
//    connect(GlobalObjectManager::sharedGlobalObjectRegistry()->getContentManager(), SIGNAL(applicationGrabbedAndInstalledSignal(QString)), this, SLOT(newAppInstalled(QString)));

    LocalAppDatabaseManager *localAppDatabaseManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager();

    connect(localAppDatabaseManager, SIGNAL(wholeAppDescriptionChanged(ApplicationDescriptionModel,ApplicationDescriptionModel)), this, SLOT(appAddedOrUpdated(ApplicationDescriptionModel,ApplicationDescriptionModel)));
    connect(localAppDatabaseManager, SIGNAL(appStateChanged(QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)), this, SLOT(appStateChanged(QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)));
//    connect(localAppDatabaseManager, SIGNAL(dynamicAppDescriptionChanged(ApplicationDescriptionModel)), this, SLOT(appAddedOrUpdated(ApplicationDescriptionModel)));
    connect(localAppDatabaseManager, SIGNAL(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), this, SLOT(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)));

    connect(localAppDatabaseManager, SIGNAL(_result_updateAppIsHidden(QString,bool,QString,bool)), this, SLOT(_result_updateAppIsHidden(QString,bool,QString,bool)));
    connect(localAppDatabaseManager, SIGNAL(_result_updateAppIsStartWhenClientStarts(QString,bool,QString,bool)), this, SLOT(_result_updateAppIsStartWhenClientStarts(QString,bool,QString,bool)));


    //
    // app uninstaller
    _appUninstaller = new AppUninstaller(this);
    connect(_appUninstaller, SIGNAL(progressStateMessage(QString,QString)), this, SLOT(_appUninstallProgressStateMessage(QString,QString)));
    connect(_appUninstaller, SIGNAL(failedWithError(ApplicationDescriptionModel,QString)), this, SLOT(_appUninstallFailedWithError(ApplicationDescriptionModel,QString)));
    connect(_appUninstaller, SIGNAL(finishedWithSuccess(ApplicationDescriptionModel)), this, SLOT(_appUninstallFinishedWithSuccess(ApplicationDescriptionModel)));
    connect(_appUninstaller, SIGNAL(uninstallSuspendedInternally(ApplicationDescriptionModel)), this, SLOT(_appUninstallSuspendedAndWaitingForResolution(ApplicationDescriptionModel)));

    // and the terminator
    AppTerminationController *terminationController = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getAppTerminationController();
    connect(terminationController, SIGNAL(applicationTerminatedSuccesfully(QString,QString)), this, SLOT(_appTerminationFinishedWithSuccess(QString,QString)));
    connect(terminationController, SIGNAL(cannotTerminateApplication(QString,QString)), this, SLOT(_appTerminationFailed(QString,QString)));
    connect(terminationController, SIGNAL(nothingToTerminate(QString,QString)), this, SLOT(_appTerminationFailed(QString,QString)));
}

void MyAppsPage::initializePageContent()
{
    // somehow if you initialize the things here can mess things up and even make it crash sometimes

    _statusMessagePresenterWidget = new ContentCoverStatusMessageWidget(300, 5000);
    _statusMessagePresenterWidget->setParent(this);
    _statusMessagePresenterWidget->setIsHideIfMousePressed(true);
    _statusMessagePresenterWidget->hide();
}

// SLOT
void MyAppsPage::_userLoginStarted()
{
    this->_releaseKeyboardEvents();
}

// SLOT
void MyAppsPage::_startNewLocalAppSearch()
{
    LOG_AS_NOT_IMPLEMENTED_WITH_HINT("This sould also clear the current icons - triggered by user-switch as well");
//    MyApplication::instance()->startNewLocalAppScan();
}

// SLOT
void MyAppsPage::_userLoggedIn()
{
//    disconnect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(_userLoggedIn()));


    // grab the keyboard events to transfer them to the search-line
    this->_grabAndTransferKeyboardEventsToSearchLine();
}

// SLOT
void MyAppsPage::_userLoginAborted()
{
    this->_grabAndTransferKeyboardEventsToSearchLine();
}



// ----------------------------------------
// --- app state change related methods ---

void MyAppsPage::appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState)
{
    AppIconAndButtonPresenterWidget *appIconPresenterWidget = dynamic_cast<AppIconAndButtonPresenterWidget *>(_allIdBasedWidgets->getFirstById(appId));

    if(appIconPresenterWidget != NULL) {
        appIconPresenterWidget->refreshContentByNewAppState(newAppState);
    }
}

void MyAppsPage::appAddedOrUpdated(ApplicationDescriptionModel inputAppDescriptionModel, ApplicationDescriptionModel updatedAppDescriptionModel)
{
    DLog(" [iii] appAddedOrUpdated");

    this->_addOrUpdateAppToWorkingSnapshot(updatedAppDescriptionModel);

    // commit the changes
    appIconPresentersHolderDynamicScrollArea->_commitCurrentWorkingSnapshot();
    // and actualize by the current page-mode
    this->_actualizeContentByCurrentPageMode(false);
}

void MyAppsPage::_result_getAllAppDescriptions(QList<ApplicationDescriptionModel> results, QString operationId, bool isQuerySuccess)
{
    DLog("_result_getAllAppDescriptions") << operationId;
    if(isQuerySuccess) {
//        if(operationId == )
        this->batchAppsAddedOrUpdated(results);
    }
}

void MyAppsPage::batchAppsAddedOrUpdated(QList<ApplicationDescriptionModel> appDescriptionModels)
{
    if(appDescriptionModels.isEmpty()) {
        DLog("Batch app add or update happened but the list is empty.");
        return;
    }

    int appCount = appDescriptionModels.size();
    for(int i = 0; i < appCount; i++) {
        this->_addOrUpdateAppToWorkingSnapshot(appDescriptionModels[i]);
    }

    // commit the changes
    appIconPresentersHolderDynamicScrollArea->_commitCurrentWorkingSnapshot();
    // and actualize by the current page-mode
    this->_actualizeContentByCurrentPageMode(false);
}

void MyAppsPage::_result_updateAppIsHidden(QString appId, bool newValue, QString operationId, bool isQuerySuccess)
{
    if(!isQuerySuccess) {
        return;
    }

    AppIconAndButtonPresenterWidget *iconPresenter = dynamic_cast<AppIconAndButtonPresenterWidget *>(this->_allIdBasedWidgets->getFirstById(appId));

    if(iconPresenter != NULL) {
        ApplicationDescriptionModel appDescr = iconPresenter->getRelatedAppDescriptionModel();
        appDescr.getDynamicDescription_DataForModification()->setIsHidden(newValue);
        iconPresenter->setContentByAppDescriptionModel(appDescr);
        this->_actualizeContentByCurrentPageMode(false);
    }
}

void MyAppsPage::_result_updateAppIsStartWhenClientStarts(QString appId, bool newValue, QString operationId, bool isQuerySuccess)
{
    if(!isQuerySuccess) {
        return;
    }

    AppIconAndButtonPresenterWidget *iconPresenter = dynamic_cast<AppIconAndButtonPresenterWidget *>(this->_allIdBasedWidgets->getFirstById(appId));

    if(iconPresenter != NULL) {
        ApplicationDescriptionModel appDescr = iconPresenter->getRelatedAppDescriptionModel();
        appDescr.getDynamicDescription_DataForModification()->setIsStartWhenClientStarts(newValue);
        iconPresenter->setContentByAppDescriptionModel(appDescr);
        this->_actualizeContentByCurrentPageMode(false);
    }
}

//// SLOT
//void MyAppsPage::_batchAppsGrabbed(QStringList grabbedAppIds)
//{
//    if(grabbedAppIds.isEmpty()) {
//        DLog("Batch apps grabbed happened, but the grabbedAppIds list is empty.");
//        return;
//    }

//    int grabbedAppIdsCount = grabbedAppIds.size();
//    for(int i = 0; i < grabbedAppIdsCount; i++) {
//        this->_addOrUpdateAppToWorkingSnapshotById(grabbedAppIds[i]);
//    }

//    // commit the changes
//    appIconPresentersHolderDynamicScrollArea->_commitCurrentWorkingSnapshot();
//    // and actualize by the current page-mode
//    this->_actualizeContentByCurrentPageMode(false);
//}

// SLOT
//void MyAppsPage::singleAppGrabHappened(QString appId) {
////    DLog("App grabbed - should be presented");

//    /*AppIconAndButtonPresenterWidget *iconPresenterWidget = */this->_addOrUpdateAppToWorkingSnapshotById(appId);

//    // commit the changes
//    appIconPresentersHolderDynamicScrollArea->_commitCurrentWorkingSnapshot();
//    // and actualize by the current page-mode
//    this->_actualizeContentByCurrentPageMode(false);

////    this->_newInstalledAppsSinceLastPageView++;
////    this->refreshRelatedMenuWithNewAppCount();
//}

//// SLOT
//void MyAppsPage::singleAppInstallingStarted(QString appId)
//{
//    this->_addOrUpdateAppToWorkingSnapshotById(appId);

//    // commit the changes
//    appIconPresentersHolderDynamicScrollArea->_commitCurrentWorkingSnapshot();
//    this->_actualizeContentByCurrentPageMode(false);
//}

//// SLOT
//void MyAppsPage::singleAppInstallHappened(QString appId) {

//    this->_addOrUpdateAppToWorkingSnapshotById(appId);

//    // commit the changes
//    appIconPresentersHolderDynamicScrollArea->_commitCurrentWorkingSnapshot();
//    this->_actualizeContentByCurrentPageMode(false);

//    this->_newInstalledAppsSinceLastPageView++;
//    this->refreshRelatedMenuWithNewAppCount();

////    DLog() << "Apps scanned and added to content-area";

//}

//// SLOT
//void MyAppsPage::singleAppInstallFailed(QString appId)
//{
//    this->_addOrUpdateAppToWorkingSnapshotById(appId);

//    // commit the changes
//    appIconPresentersHolderDynamicScrollArea->_commitCurrentWorkingSnapshot();
//    // and actualize by the current page-mode
//    this->_actualizeContentByCurrentPageMode(false);
//}

//// SLOT
//void MyAppsPage::appUninstallHappened(QString appId) {

//}

//// SLOT
////void MyAppsPage::appUngrabHappened(QString appId) {

////}

//// SLOT
//void MyAppsPage::initialAppScanProcessing(QStringList appIdList) {
//    int appCnt = appIdList.size();
//    for(int i = 0; i < appCnt; i++) {
//        this->_addOrUpdateAppToWorkingSnapshotById(appIdList[i]);
//    }

//    // commit the changes
//    appIconPresentersHolderDynamicScrollArea->_commitCurrentWorkingSnapshot();
//    this->_actualizeContentByCurrentPageMode(false);

////    DLog() << "Apps scanned and added to content-area";
//}

AppIconAndButtonPresenterWidget *MyAppsPage::_addOrUpdateAppToWorkingSnapshot(ApplicationDescriptionModel appDescriptionModel)
{
    const QString appId = appDescriptionModel.getDynamicDescription_ReadOnly()->getAppId();
    DLog("--- add or update app to working snapshot: ") << appId;

//    LocalAppDatabaseServer *localAppDatabaseManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseServer();
    AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState = appDescriptionModel.getDynamicDescription_ReadOnly()->getAppState();

    bool isOnlyUpdateIconPresenter = true;
    AppIconAndButtonPresenterWidget *appIconPresenterWidget = dynamic_cast<AppIconAndButtonPresenterWidget *>(_allIdBasedWidgets->getFirstById(appId));

    if(appIconPresenterWidget == NULL) {
//        appIconPresenterWidget = new AppIconAndButtonPresenterWidget(GuiConfigurations::casualLauncherIconPresenterLabelStyleSheet());
        appIconPresenterWidget = new AppIconAndButtonPresenterWidget(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Label/Default/DarkDarkGray/style"));
        isOnlyUpdateIconPresenter = false;
    }

    if(!isOnlyUpdateIconPresenter)
    {
//    if(appState == AppWhirr::NewestVersionInstalled || appState == AppWhirr::InstalledButUpdateAvailable)
        if(appState != AppWhirr::ApplicationGrabStates::NoState && appState != AppWhirr::ApplicationGrabStates::StateRequestError)
        {
            appIconPresenterWidget->setContentByAppDescriptionModel(appDescriptionModel);
        }
        else
        {
            //        delete appIconPresenterWidget; // too risky to delete it, the layout may still use it (e.g. because it was successfully grabbed, but the install failed)
            WLog("App's state is invalid. Cannot add the icon-presenter to it: ") << appId << " | It's state-code: " << appState;
            return NULL;
        }
    }


    if(isOnlyUpdateIconPresenter) {
//        appIconPresenterWidget->setIconState(iconState);
        appIconPresenterWidget->setContentByAppDescriptionModel(appDescriptionModel);
    }
    else {
        connect(appIconPresenterWidget, SIGNAL(clicked(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(manageOptionSelected(IdBasedSelectableAndHighlightableWidgetBase*)));
        // run it when it's activated
        connect(appIconPresenterWidget, SIGNAL(activated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(runOptionSelected(IdBasedSelectableAndHighlightableWidgetBase*)));
        connect(appIconPresenterWidget, SIGNAL(runOptionActivated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(runOptionSelected(IdBasedSelectableAndHighlightableWidgetBase*)));
        connect(appIconPresenterWidget, SIGNAL(installOptionActivated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(installOptionSelected(IdBasedSelectableAndHighlightableWidgetBase*)));
        //
        connect(appIconPresenterWidget, SIGNAL(manageOptionActivated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(manageOptionSelected(IdBasedSelectableAndHighlightableWidgetBase*)));

        connect(appIconPresenterWidget, SIGNAL(notAvailableForThisPlatformSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(_notAvailableForThisPlatformSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase*)));
        connect(appIconPresenterWidget, SIGNAL(updateSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(_updateSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase*)));
        connect(appIconPresenterWidget, SIGNAL(removedFromStoreSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(_removedFromStoreSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase*)));

        // also install an event filter to be able to propagate it to smart-scroll-area's grab-scroll system
        appIconPresenterWidget->installEventFilterOnIconImage(appIconPresentersHolderDynamicScrollArea);

        //        appIconPresentersHolderDynamicScrollArea->addWidgetToList(appIconPresenterWidget, true);
//        appIconPresentersHolderDynamicScrollArea->addIdBasedWidgetToCurrentWorkingSnapshot(appIconPresenterWidget);
        appIconPresentersHolderDynamicScrollArea->addIdBasedWidgetToLayout(appIconPresenterWidget);
        _allIdBasedWidgets->add(appIconPresenterWidget);

        _newInstalledAppsSinceLastPageView += 1;
        this->refreshRelatedMenuWithNewAppCount();
    }

    return appIconPresenterWidget;
}

#if 0
void MyAppsPage::newAppInstalled(QString appId) {
//    this->rescanLocalApps();
//    MyApplication::instance()->getLocalApplicationScanner()->scanForNewApps();
}

void MyAppsPage::startInitialLocalApplicationScanResultProcessing() {
    QList<ApplicationDescriptionModel> foundApplicationIconModels = MyApplication::instance()->getLocalApplicationScanner()->getFoundApplicationDescriptionModels();
    int appCnt = foundApplicationIconModels.size();
    for(int i = 0; i < appCnt; i++) {
        AppIconAndButtonPresenterWidget *appIconPresenterWidget = new AppIconAndButtonPresenterWidget(GuiConfigurations::casualLauncherIconPresenterLabelStyleSheet());
        appIconPresenterWidget->setIconModel(foundApplicationIconModels[i]);

        connect(appIconPresenterWidget, SIGNAL(activated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT((IdBasedSelectableAndHighlightableWidgetBase*)));
        connect(appIconPresenterWidget, SIGNAL(runOptionActivated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(runOptionSelected(IdBasedSelectableAndHighlightableWidgetBase*)));

//        appIconPresentersHolderDynamicScrollArea->addWidgetToList(appIconPresenterWidget, true);
        appIconPresentersHolderDynamicScrollArea->addIdBasedWidgetToCurrentWorkingSnapshot(appIconPresenterWidget);
    }

    // TEST
    // add scanned item to the in-memory bridge app database
    LocalAppDatabaseServer *bridgeDatabase = GlobalObjectManager::sharedGlobalObjectRegistry()->getLocalApplicationDatabaseManager();
    for(int i = 0; i < appCnt; i++) {
        ApplicationDescriptionModel currAppDescriptionModel = foundApplicationIconModels[i];
        bridgeDatabase->addInstalledApplicationAndSetInstallSpecificInfos(currAppDescriptionModel.getStaticDescription()->getAppId(), currAppDescriptionModel.getDynamicDescription()->getInstalledPath());
    }


    // commit the changes
    appIconPresentersHolderDynamicScrollArea->_commitCurrentWorkingSnapshot();

//    DLog() << "Apps scanned and added to content-area";
}

// Q_SLOT
void MyAppsPage::newLocalAppsFoundDuringScan() {
    DLog("Rescan local apps.");

    QList<ApplicationDescriptionModel> foundNewApplicationIconModels = MyApplication::instance()->getLocalApplicationScanner()->getNewApplicationDescriptionModelsSinceLastRescan();
    int newAppCnt = foundNewApplicationIconModels.size();
    for(int i = 0; i < newAppCnt; i++) {
        AppIconAndButtonPresenterWidget *appIconPresenterWidget = new AppIconAndButtonPresenterWidget(GuiConfigurations::casualLauncherIconPresenterLabelStyleSheet());
        appIconPresenterWidget->setIconModel(foundNewApplicationIconModels[i]);

        connect(appIconPresenterWidget, SIGNAL(activated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT((IdBasedSelectableAndHighlightableWidgetBase*)));
        connect(appIconPresenterWidget, SIGNAL(runOptionActivated(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(runOptionSelected(IdBasedSelectableAndHighlightableWidgetBase*)));

//        appIconPresentersHolderDynamicScrollArea->addWidgetToList(appIconPresenterWidget, true);
        appIconPresentersHolderDynamicScrollArea->addIdBasedWidgetToCurrentWorkingSnapshot(appIconPresenterWidget);
    }

    if(newAppCnt > 0) {
        DLog("New apps found");
    }

    // refresh the complete list of apps
#if 0 // refreshSorting... will sort it, this is not required
    QList<ApplicationDescriptionModel> allAppsFound = MyApplication::instance()->getLocalApplicationScanner()->getFoundApplicationDescriptionModels();
    // force refresh the list
    QList<QString> allFoundAppTitles;
    int foundAppIconCnt = allAppsFound.size();
    for(int i = 0; i < foundAppIconCnt; i++) {
        allFoundAppTitles << allAppsFound[i].getStaticDescription()->getAppName();
    }
    this->appIconPresentersHolderDynamicScrollArea->getIdBasedWidgetCollection()->_refreshListOrderByIdList(allFoundAppTitles);
#endif

    DLog("Rescan finished");

    // and refresh the GUI
    this->refreshSortingByLastFilter();

    _newInstalledAppsSinceLastPageView += foundNewApplicationIconModels.size();
    this->refreshRelatedMenuWithNewAppCount();
    DLog("Rescan related updates finished");
}
#endif

void MyAppsPage::activateCurrentApplicationIconPresenter()
{
    this->appIconPresentersHolderDynamicScrollArea->activateCurrentItem();
}

void MyAppsPage::manageOptionSelected(IdBasedSelectableAndHighlightableWidgetBase *source)
{
    if(!this->appManagementSidebarWidget->getIsExpanded()) {
        this->appManagementSidebarWidget->switchToExpandedStateSlot();
    }
//    else {
//        this->sidebarWidget->switchToCollapsedState();
//    }

    AppIconAndButtonPresenterWidget *castedSource = dynamic_cast<AppIconAndButtonPresenterWidget *>(source);
    if(castedSource == NULL) {
        WLog("The source icon-presenter is not the required type, cannot cast it.");
        return;
    }

    this->appManagementSidebarWidget->setContent(castedSource->getRelatedAppDescriptionModel(), castedSource->getIconImageWidget()->getPresentedImageAsPixmap());

    if(this->appManagementSidebarWidget->getIsExpanded()) {
        // centerize the current item
        this->appIconPresentersHolderDynamicScrollArea->ensureCurrentSelectedItemCenterizedInScrollArea();
    }
}

// SLOT
void MyAppsPage::runOptionSelected(IdBasedSelectableAndHighlightableWidgetBase *source)
{
    AppIconAndButtonPresenterWidget *castedSource = dynamic_cast<AppIconAndButtonPresenterWidget *>(source);
    if(castedSource == NULL) {
        WLog("The source icon-presenter is not the required type, cannot cast it.");
        return;
    }

    if(castedSource->getRelatedAppDescriptionModel().isUpdateAvailable()) {
        this->_installThisApp(castedSource->getRelatedAppDescriptionModel());
    }
    else {
        this->_runThisApp(castedSource->getRelatedAppDescriptionModel());
    }
}

void MyAppsPage::installOptionSelected(IdBasedSelectableAndHighlightableWidgetBase *source)
{
    AppIconAndButtonPresenterWidget *castedSource = dynamic_cast<AppIconAndButtonPresenterWidget *>(source);
    if(castedSource == NULL) {
        WLog("The source icon-presenter is not the required type, cannot cast it.");
        return;
    }

    this->_installThisApp(castedSource->getRelatedAppDescriptionModel());
}

// SLOT
void MyAppsPage::_runThisApp(ApplicationDescriptionModel appDescrModel)
{
    AW::Client::Utils::AWApplicationRunner::postRunThisAppEvent(appDescrModel, _lastSearchFilter);
}

// SLOT
void MyAppsPage::_installThisApp(ApplicationDescriptionModel appDescrModel)
{
    // grab
    if(appDescrModel.isUpdateAvailable()) {
        // it's an update - set the state
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(appDescrModel.getDynamicDescription_ReadOnly()->getAppId(), AppWhirr::ApplicationGrabStates::InstalledAndUpdating, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "MyAppsPage::_installThisApp");
    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->grabApplication(appDescrModel.getDynamicDescription_ReadOnly()->getAppId(), appDescrModel.getDynamicDescription_ReadOnly()->getAppName(), appDescrModel.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags(), appDescrModel.isUpdateAvailable(), appDescrModel.getDynamicDescription_ReadOnly()->getAppStatusInStore());
}

// SLOT
void MyAppsPage::removeThisAppByAppId(ApplicationDescriptionModel appDescrModel)
{
    // create notification
    QString appId = appDescrModel.getDynamicDescription_ReadOnly()->getAppId();
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("confirm_uninstall-" + appId);
    if(relatedAppInstallNotificationWidget == NULL) {

        TextAndAcceptCancelButtonsNotificationWidget *confirmUninstallNotificationWidget = new TextAndAcceptCancelButtonsNotificationWidget(tr("Are you sure you want to uninstall %1?").arg(appDescrModel.getDynamicDescription_ReadOnly()->getAppName()), tr("Uninstall"), tr("Cancel"), true);
        connect(confirmUninstallNotificationWidget, SIGNAL(acceptOptionSelected()), this, SLOT(_uninstallConfirmAccepted()));
        connect(confirmUninstallNotificationWidget, SIGNAL(cancelOptionSelected()), this, SLOT(_uninstallConfirmCanceled()));
        confirmUninstallNotificationWidget->setOptionalId(appId);

        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addNotificationWithSelfTimeoutingCustomWidget(confirmUninstallNotificationWidget, "Uninstall confirmation notification for appId: " + appId + "|" + appDescrModel.getDynamicDescription_ReadOnly()->getAppName(), "confirm_uninstall-" + appId);

        // and store in the confirm-map
        this->_appUninstallConfirmMap[appId] = appDescrModel;
    }
}

// SLOT
void MyAppsPage::showAppDetailsByAppId(ApplicationDescriptionModel appDescrModel)
{
    Q_EMIT showStorePageForApp(appDescrModel);
}

// SLOT
void MyAppsPage::createDesktopShortcutForThisApp(ApplicationDescriptionModel appDescrModel)
{
    if( !AW::Client::Utils::AWShortcutHandler::createSchemeBasedDesktopShortcutForApp(appDescrModel) )
    {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("Cannot create the Desctop Shortcut. Sorry."), 5000, NotificationManager::ErrorPriority);
    } else {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("Desktop Shortcut created."), 5000, NotificationManager::NormalPriority);
    }
}

// SLOT
void MyAppsPage::_isStartThisAppWhenClientStartsChangedSlot(QString appId, bool newValue)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppIsStartWhenClientStarts(appId, newValue, "MyApps");

//    LOG_AS_NOT_IMPLEMENTED;
//    DLogS << " -- appid: " << appId << " newValue: " << newValue;
//    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseServer()->setIsStartWhenClientStarts(appId, newValue);
//    // refresh the current view
//    this->_actualizeContentByCurrentPageMode(false);
}

// SLOT
void MyAppsPage::_isThisAppHiddenChangedSlot(QString appId, bool newValue)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppIsHidden(appId, newValue, "MyApps");
//    LOG_AS_NOT_IMPLEMENTED;
////    DLogS << " -- appid: " << appId << " newValue: " << newValue;
//    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseServer()->setIsHidden(appId, newValue);
//    // refresh the current view
//    this->_actualizeContentByCurrentPageMode(false);
}

void MyAppsPage::refreshSortingByLastSearchFilter()
{
    this->searchByFilterText(_lastSearchFilter);
}

void MyAppsPage::resizeEvent(QResizeEvent *event) {
//    DLogS << " [tmp] Resize, contentW size: " << contentWidget->size();

//    contentWidget->adjustSize();

    _statusMessagePresenterWidget->setGeometryToThis(this->geometry());
    QWidget::resizeEvent(event);
}

void MyAppsPage::refreshRelatedMenuWithNewAppCount() {
    DLog("refresh menu with NewCount");

    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();
    if(menuManager == NULL) {
        WLog("Menu manager not found.");
        return;
    }

    MenuItemWidget *menuItem = menuManager->getItemById(MenuItemIdCenter::myAppsMenuId());
    if(menuItem != NULL) {
        if(this->_newInstalledAppsSinceLastPageView > 0 && !GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->isThisPageIsTheCurrentActiveOne(this->getPageId())) {
//            menuItem->setTitle(tr("My Apps") + QString(" (%1)").arg(this->_newInstalledAppsSinceLastPageView));
            menuItem->setTitle(tr("My Apps"));
            menuItem->setBadgeNumber(this->_newInstalledAppsSinceLastPageView);
        } else {
            this->_newInstalledAppsSinceLastPageView = 0;
            menuItem->setTitle(tr("My Apps"));
            menuItem->setBadgeNumber(0);
        }
    } else {
        WLog("Menu item not found.");
    }
}

bool MyAppsPage::event(QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        if( this->processKeyPressEvent(keyEvent) )
        {
            event->accept();
            return true;
        }
    }

    return BrowserPageWidgetBase::event(event);
}

void MyAppsPage::focusOutEvent(QFocusEvent *event)
{
    if(this->_isShouldGrabTheFocus) {
        this->_grabAndTransferKeyboardEventsToSearchLine();
        event->accept();
        return;
    }
    else {
        BrowserPageWidgetBase::focusOutEvent(event);
    }
}

void MyAppsPage::_grabAndTransferKeyboardEventsToSearchLine()
{
    this->_isShouldGrabTheFocus = true;

//    this->setFocusPolicy(Qt::StrongFocus);
//    this->setFocus();
    this->searchLineInMenu->setFocusPolicy(Qt::StrongFocus);
    this->searchLineInMenu->setFocus();
    this->grabKeyboard();
}

void MyAppsPage::_releaseKeyboardEvents()
{
    this->_isShouldGrabTheFocus = false;

    this->releaseKeyboard();
}

bool MyAppsPage::processKeyPressEvent(QKeyEvent *keyEvent)
{
    if( this->searchLineInMenu != NULL && this->focusWidget() != this->searchLineInMenu )
    {
//        this->searchLineInMenu->setFocus();
//        DLog("pass this to searchLine");

        if(keyEvent->key() == Qt::Key_Escape) {
            // clear the search-line
            this->searchLineInMenu->clear();
        }
        else {
            // also send this event to the search-line
            QApplication::sendEvent(this->searchLineInMenu, keyEvent);
        }

        return true;
    }

    return false;
}

void MyAppsPage::_showOnlyAutostartAppsMenuItemActivated(QString relatedMenuItemId)
{
    this->_showOnlyAutostartApps();
}

void MyAppsPage::_showOnlyAutostartApps()
{
    this->_currPageMode = ShowOnlyAutostartApps;
    this->cleverSortingDelegate->sortTheseByFilters(_allIdBasedWidgets->getAll(), QString(), true, false, true, true);
}

void MyAppsPage::_showOnlyHiddenAppsMenuItemActivated(QString relatedMenuItemId)
{
    this->_showOnlyHiddenApps();
}

void MyAppsPage::_showOnlyHiddenApps()
{
    this->_currPageMode = ShowOnlyHiddenApps;
    this->cleverSortingDelegate->sortTheseByFilters(_allIdBasedWidgets->getAll(), QString(), false, true, true, true);
}

void MyAppsPage::_showOnlyInstalledAppsMenuItemActivated(QString relatedMenuItemId)
{
    this->_showOnlyInstalledApps();
}

void MyAppsPage::_showOnlyInstalledApps()
{
    this->_currPageMode = ShowOnlyInstalledApps;
    this->cleverSortingDelegate->sortTheseByFilters(_allIdBasedWidgets->getAll(), QString(), false, false, false, false);
}

void MyAppsPage::_filterContentBySearchTextMenuItemActivated(QString relatedMenuItemId)
{
    this->refreshSortingByLastSearchFilter();
}

void MyAppsPage::_showAllApps()
{
    this->_currPageMode = ShowAllApps;
    this->cleverSortingDelegate->sortTheseByFilters(_allIdBasedWidgets->getAll(), QString(), false, false, false, true);
}

// Q_SLOT
void MyAppsPage::searchByFilterText(QString filter)
{
    this->_currPageMode = ShowSearchResults;
    if(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->isThisPageIsTheCurrentActiveOne(this->getPageId())) {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MY_APPS_SEARCH_SUB_MENU_ITEM_ID);
    }

    _lastSearchFilter = filter;

    //qDebug("--perform");
    // deselect the current selection
    //        contentLayout->getSnapshotHandlerDelegate()->deselectCurrentItem();
    //        contentScrollArea->getSnapshotHandlerDelegate()->deselectCurrentItem();

    //qDebug("sort it");
    // make it filter and sort
    this->cleverSortingDelegate->sortTheseByFilters(_allIdBasedWidgets->getAll(), filter, false, false, true, true);
}

void MyAppsPage::_sortingHappened(WidgetListSnapshot sortingResultSnapshot)
{
    if(sortingResultSnapshot.getOrderedItemCount() <= 0) {
        DLog("Present some 'No apps to present' text");
    }
    else {
        // select the first item in the result list
        this->appIconPresentersHolderDynamicScrollArea->getSnapshotHandlerDelegate()->selectFirstItem();
    }
}

//void MyAppsContentPage::fillMenuItems(MenuItemWidget *menuItem)
//{
//}

// Q_SLOT
void MyAppsPage::relatedMainMenuItemActivated(QString menuItemId)
{    
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();

    if(pageNavigationManager->isThisPageIsTheCurrentActiveOne(PageIdCenter::myAppsPageId())) {
        // this is the current page - switch mode to show-all
        this->_showAllApps();
    }
    else if(pageNavigationManager->isThisRootStackIsTheActiveOne(PageIdCenter::myAppsPageId())) {
        // this root page's stack is the active one, so only pop-back to it
        pageNavigationManager->popBackToThisPage(PageIdCenter::myAppsPageId());
    } else {
        // currently this root page's stack is not the active one, so switch to it
        pageNavigationManager->switchToRootPageStack(PageIdCenter::myAppsPageId());
    }
}

void MyAppsPage::pagePushWillHappen()
{
    // this is a root page, pagePush can happen multiple times (every time the root page appears except when appears via pop), but
    //  pagePop will never happen
    // this is required by the menu-mechanism
    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();
    if(menuManager == NULL) {
        WLog("Menu manager not found.");
        return;
    }

    MenuItemWidget *pageRelatedMenuItem = menuManager->getItemById(MenuItemIdCenter::myAppsMenuId());

    if(pageRelatedMenuItem != NULL)
    {
//        connect(pageRelatedMenuItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(_showAllAppsMenuItemActivated(QString)));

        //  --- search menu item
        MenuItemWidget *searchSubMenuItemWidget = new MenuItemWidget(MY_APPS_SEARCH_SUB_MENU_ITEM_ID, tr("Search:"));
        //connect(webNewsItem, SIGNAL(fillSubItemsToThisItem(MenuItemWidget*)), this, SLOT(fillSubMenuItemsToWebNews(MenuItemWidget*)));
        connect(searchSubMenuItemWidget, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(_filterContentBySearchTextMenuItemActivated(QString)));

        this->searchLineInMenu = new QLineEdit;
        searchLineInMenu->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("LineEdit/Search/Gray/style"));
        this->searchLineInMenu->setPlaceholderText(tr("Type here to search..."));
        connect(this->searchLineInMenu, SIGNAL(textChanged(QString)), this, SLOT(searchByFilterText(QString)));
        connect(this->searchLineInMenu, SIGNAL(returnPressed()), this, SLOT(activateCurrentApplicationIconPresenter()));

        // set some event filtering
//        this->appIconPresentersHolderDynamicScrollArea->installEventFilter(this);
//        this->searchLineInMenu->installEventFilter(this);
//        this->installEventFilter(this);

        menuManager->addChildToItem(searchSubMenuItemWidget, MenuItemIdCenter::myAppsMenuId(), this->searchLineInMenu, true);

        // --- show autostart apps menu item
        MenuItemWidget *showAutostartAppsMenuItem = new MenuItemWidget(MY_APPS_SHOW_AUTOSTART_APPS_SUB_MENU_ITEM_ID, tr("Autostart apps"));
        connect(showAutostartAppsMenuItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(_showOnlyAutostartAppsMenuItemActivated(QString)));
        menuManager->addChildToItem(showAutostartAppsMenuItem, MenuItemIdCenter::myAppsMenuId());

        // --- show autostart apps menu item
        MenuItemWidget *showOnlyInstalledAppsMenuItem = new MenuItemWidget(MY_APPS_SHOW_ONLY_INSTALLED_APPS_SUB_MENU_ITEM_ID, tr("Installed apps"));
        connect(showOnlyInstalledAppsMenuItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(_showOnlyInstalledAppsMenuItemActivated(QString)));
        menuManager->addChildToItem(showOnlyInstalledAppsMenuItem, MenuItemIdCenter::myAppsMenuId());

        MenuItemWidget *showOnlyHiddenAppsMenuItem = new MenuItemWidget(MY_APPS_SHOW_ONLY_HIDDEN_APPS_SUB_MENU_ITEM_ID, tr("Hidden apps"));
        connect(showOnlyHiddenAppsMenuItem, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(_showOnlyHiddenAppsMenuItemActivated(QString)));
        menuManager->addChildToItem(showOnlyHiddenAppsMenuItem, MenuItemIdCenter::myAppsMenuId());

    }
    else
    {
        WLog("The menu-item is NULL. This page requires a valid menu-item.");
    }
}

void MyAppsPage::pageWillAppear(QString param)
{
//    GlobalObjectManager::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MY_APPS_SEARCH_SUB_MENU_ITEM_ID);

    if(param == QString("MyApps/ShowOnlyAutostartApps")) {
        this->_currPageMode = ShowOnlyAutostartApps;
    }

    if(_newInstalledAppsSinceLastPageView > 0) {
        _newInstalledAppsSinceLastPageView = 0;
        this->refreshRelatedMenuWithNewAppCount();
    }
}

void MyAppsPage::pageDidAppear()
{    
    // authorization
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(_userLoggedIn()));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginAborted()), this, SLOT(_userLoginAborted()));
    connect(Authorization::sharedAuthorization(), SIGNAL(userLoginStarted()), this, SLOT(_userLoginStarted()));

    // test whether the user is logged in / selected
    QString currUserId = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->getCurrentUserId();
    if(currUserId.isEmpty()) {
        // no current user selected
//        connect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(_startNewLocalAppSearch())); // also required if no user selected
        Authorization::sharedAuthorization()->showLoginDialogAndStartLogin();
    }
    else {

        Authorization::sharedAuthorization()->hideLoginDialog();
        // grab the keyboard events to transfer them to the search-line
        this->_grabAndTransferKeyboardEventsToSearchLine();

        // and refresh user-grabbed app list
//        this->_getAllAndPresentGrabbedAppForCurrentUser();
    }

    this->_actualizeContentByCurrentPageMode(true);
}

void MyAppsPage::pageReactivated(QString param)
{
    if(param == QString("MyApps/ShowOnlyAutostartApps")) {
        this->_currPageMode = ShowOnlyAutostartApps;
        this->_actualizeContentByCurrentPageMode(true);
    }
}

void MyAppsPage::_actualizeContentByCurrentPageMode(bool isActualizeRelatedMenuAsWell)
{
    if(isActualizeRelatedMenuAsWell)
    {
        if(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->isThisPageIsTheCurrentActiveOne(this->getPageId()))
        {
            if(this->_currPageMode == ShowOnlyAutostartApps) {
                GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->activateItemById(MY_APPS_SHOW_AUTOSTART_APPS_SUB_MENU_ITEM_ID);
            }
            else if(this->_currPageMode == ShowAllApps) {
                GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::myAppsMenuId());
            }
            else if(this->_currPageMode == ShowSearchResults) {
                GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->activateItemById(MY_APPS_SEARCH_SUB_MENU_ITEM_ID);
            }
            else if(this->_currPageMode == ShowOnlyHiddenApps) {
                GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->activateItemById(MY_APPS_SHOW_ONLY_HIDDEN_APPS_SUB_MENU_ITEM_ID);
            }
            else if(this->_currPageMode == ShowOnlyInstalledApps) {
                GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->activateItemById(MY_APPS_SHOW_ONLY_INSTALLED_APPS_SUB_MENU_ITEM_ID);
            }
        }
    }

    else
    {
        if(this->_currPageMode == ShowOnlyAutostartApps) {
            this->_showOnlyAutostartApps();
        }
        else if(this->_currPageMode == ShowAllApps) {
            this->_showAllApps();
        }
        else if(this->_currPageMode == ShowSearchResults) {
            this->refreshSortingByLastSearchFilter();
        }
        else if(this->_currPageMode == ShowOnlyHiddenApps) {
            this->_showOnlyHiddenApps();
        }
        else if(this->_currPageMode == ShowOnlyInstalledApps) {
            this->_showOnlyInstalledApps();
        }
    }

    // refresh the sidebar as well
    LOG_AS_TMP_OFF_WITH_HINT("Commented out because the sidebar refreshes itself by app-state-changes, but maybe I'm wrong and this is needed for some reason");
//    this->sidebarWidget->refreshCurrentContent();
}

void MyAppsPage::pagePopWillHappen()
{
    // this is a root page, pop will never happen
}

void MyAppsPage::pageWillDisappear()
{
    // disconnect from every authorization related signals
    disconnect(Authorization::sharedAuthorization(), 0, this, 0);

    // will release the keyboard and allow other widgets and pages to process key events
    this->_releaseKeyboardEvents();

    // test whether the user is logged in / selected
    QString currUserId = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getUserManager()->getCurrentUserId();

    if(currUserId.isEmpty()) {
        // no current user selected
//        MyApplication::instance()->getBrowserWindow()->hideUserLoginSidebar();
//        connect(Authorization::sharedAuthorization(), SIGNAL(userLoggedIn()), this, SLOT(_userLoggedIn()));
//        Authorization::sharedAuthorization()->hideLoginDialog();
    }
}


// ---------------------
// --- app uninstall ---

void MyAppsPage::_uninstallConfirmAccepted()
{
    TextAndAcceptCancelButtonsNotificationWidget *relatedNotificationCustomWidget = dynamic_cast<TextAndAcceptCancelButtonsNotificationWidget *>(sender());
    if(relatedNotificationCustomWidget == NULL) {
        WLog("Invalid type of sender!");
        return;
    }

    // get the related appId and appName
    QString relatedAppId = relatedNotificationCustomWidget->getOptionalId();

    if(relatedAppId.isEmpty() || !_appUninstallConfirmMap.contains(relatedAppId)) {
        WLog("Cannot find the related appId in the uninstall-confirm-map: ") << relatedAppId;
        return;
    }
    ApplicationDescriptionModel appDescriptionModel = _appUninstallConfirmMap.value(relatedAppId);
    _appUninstallConfirmMap.remove(relatedAppId);

    // update state
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(relatedAppId, AppWhirr::ApplicationGrabStates::GrabbedAndUninstalling, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "MyAppsPage::removeThisAppByAppId");

    // create notification
    QString appId = relatedAppId;
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("uninstall-" + appId);
    if(relatedAppInstallNotificationWidget == NULL) {

        // add only if it's not yet added
        UninstallProgressNotificationWidget *uninstallNotificationWidget = new UninstallProgressNotificationWidget(appId, appDescriptionModel.getDynamicDescription_ReadOnly()->getAppName());
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addNotificationWithSelfTimeoutingCustomWidget(uninstallNotificationWidget, "Uninstall process notification for appId: " + appId, "uninstall-" + appId);

        // mark as critical-action
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->addCriticalAction(AppWhirr::CriticalActionCodes::AppUninstallActive);
    }

    // and start to uninstall
    this->_appUninstaller->uninstallThisApp(appDescriptionModel);
}

void MyAppsPage::_uninstallConfirmCanceled()
{
    TextAndAcceptCancelButtonsNotificationWidget *relatedNotificationCustomWidget = dynamic_cast<TextAndAcceptCancelButtonsNotificationWidget *>(sender());
    if(relatedNotificationCustomWidget == NULL) {
        WLog("Invalid type of sender!");
        return;
    }

    // get the related appId and appName
    QString relatedAppId = relatedNotificationCustomWidget->getOptionalId();

    if(relatedAppId.isEmpty() || !_appUninstallConfirmMap.contains(relatedAppId)) {
        WLog("Cannot find the related appId in the uninstall-confirm-map: ") << relatedAppId;
        return;
    }

    _appUninstallConfirmMap.remove(relatedAppId);
}

void MyAppsPage::_appUninstallProgressStateMessage(QString appId, QString progressStateMessage)
{
    // notification
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("uninstall-" + appId);
    if(relatedAppInstallNotificationWidget != NULL) {
        UninstallProgressNotificationWidget *castedNotificationWidget = dynamic_cast<UninstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
        if(castedNotificationWidget != NULL) {
            castedNotificationWidget->setProgressStateMessage(progressStateMessage);
        }
    }
}

void MyAppsPage::_appUninstallFinishedWithSuccess(ApplicationDescriptionModel inputAppDescriptionModel)
{
    // app state
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->downgradeAppToGrabbedState(inputAppDescriptionModel);

    AppwhirrApplication::instance()->getBrowserWindow()->_appDidUninstall(inputAppDescriptionModel);

    // notification
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("uninstall-" + inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId());
    if(relatedAppInstallNotificationWidget != NULL) {
        UninstallProgressNotificationWidget *castedNotificationWidget = dynamic_cast<UninstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
        if(castedNotificationWidget != NULL) {
            castedNotificationWidget->setUninstallSucceeded();

            // finished with critical action
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->removeCriticalAction(AppWhirr::CriticalActionCodes::AppUninstallActive);

            DLog("[uninstall] Critical action test: ") << GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->isThisCriticalActionActive(AppWhirr::CriticalActionCodes::AppUninstallActive);
            DLog("Is any critical action? ") << GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->isAnyCriticalActionActive();
        }
    }
}

void MyAppsPage::_appUninstallFailedWithError(ApplicationDescriptionModel inputAppDescriptionModel, QString errorMessage)
{
    DLog("--uninst failed");
    // app state
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId(), AppWhirr::ApplicationGrabStates::Installed, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "MyAppsPage::_appUninstallFailedWithError");

    // notification
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("uninstall-" + inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId());
    if(relatedAppInstallNotificationWidget != NULL) {
        UninstallProgressNotificationWidget *castedNotificationWidget = dynamic_cast<UninstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
        if(castedNotificationWidget != NULL) {
            castedNotificationWidget->setUninstallFailedWithError(errorMessage);

            // finished with critical action
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCriticalActionManager()->removeCriticalAction(AppWhirr::CriticalActionCodes::AppUninstallActive);
        }
    }
}

void MyAppsPage::_appUninstallSuspendedAndWaitingForResolution(ApplicationDescriptionModel inputAppDescriptionModel)
{
    if(_appTerminationStartedForAppId == inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId()) {
        // this is the same app as before -> abort it
        this->_appUninstaller->abortSuspendedUninstall();
        return;
    }

    _appTerminationStartedForAppId = inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId();

    // notification
    NotificationWidget *relatedAppInstallNotificationWidget = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->getNotificationWidgetByItemId("uninstall-" + inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId());
    if(relatedAppInstallNotificationWidget != NULL) {
        UninstallProgressNotificationWidget *castedNotificationWidget = dynamic_cast<UninstallProgressNotificationWidget *>(relatedAppInstallNotificationWidget->getContentWidget());
        if(castedNotificationWidget != NULL) {
            castedNotificationWidget->setResolutionNeeded();
        }
    }

    // start termination
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getAppTerminationController()->terminateAppUnderDirectory(inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppId(), inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppName(), inputAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), AppTerminationController::TRC_TerminateForUninstall);
}

void MyAppsPage::_appTerminationFinishedWithSuccess(QString appId, QString appName)
{
    if(_appTerminationStartedForAppId == appId) {
        this->_appUninstaller->retrySuspendedUninstall();
        _appTerminationStartedForAppId.clear();
    }
}

void MyAppsPage::_appTerminationFailed(QString appId, QString appName)
{
    if(_appTerminationStartedForAppId == appId) {
        this->_appUninstaller->abortSuspendedUninstall();
        _appTerminationStartedForAppId.clear();
    }
}

// ------------------------------
// --- app management sidebar ---

void MyAppsPage::_appManagementSidebarSwitchedToExpandedState()
{
    this->appIconPresentersHolderDynamicScrollArea->ensureItemCenterizedInScrollArea(this->appManagementSidebarWidget->getRelatedAppId());
}

void MyAppsPage::_appManagementSidebarSwitchedToCollapsedState()
{
    this->appIconPresentersHolderDynamicScrollArea->ensureItemCenterizedInScrollArea(this->appManagementSidebarWidget->getRelatedAppId());
}


// ---------------
// --- utility ---

void MyAppsPage::_layoutChangeFinished()
{
    if(this->appIconPresentersHolderDynamicScrollArea->getSnapshotHandlerDelegate()->getAllItemCount() > 0) {
        this->_noAppsStatusHolderWidget->hide();
        this->_normalContentHolderWidget->show();
    }
    else {
        this->_noAppsStatusHolderWidget->show();
        this->_normalContentHolderWidget->hide();
    }
}


// ------------------------------------------
// --- app sub-state icon action handlers ---

void MyAppsPage::_removedFromStoreSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase *source) {
    this->_statusMessagePresenterWidget->showWithAnimationAndActivate(tr("This application has been removed from our Store."));
}

void MyAppsPage::_updateSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase *source) {
    AppIconAndButtonPresenterWidget *castedWidget = dynamic_cast<AppIconAndButtonPresenterWidget *>(source);
    if(source == NULL) {
        WLog("Cannot cast.");
        return;
    }

    this->_installThisApp(castedWidget->getRelatedAppDescriptionModel());
}

void MyAppsPage::_notAvailableForThisPlatformSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase *source) {
    this->_statusMessagePresenterWidget->showWithAnimationAndActivate(tr("This application is not available for this machine. Check the alternatives."));
}
