#include "applicationstoremainpage.h"

#include <QResizeEvent>
#include <QTimer>

// UI
#include <QVBoxLayout>
#include <QString>
#include <QEvent>
#include <QLineEdit>
//#include <QScrollArea>
#include <QLabel>

#include "Common/Widgets/dynamicsamesizehorizontalboxlayoutbasedsmartscrollarea.h"
#include "Browser/Content/Widget/applicationstoreappwidget.h"
#include "Common/Widgets/busyindicatorwidget.h"
#include "Common/Layouts/flowlayout.h"
//#include "Common/Widgets/delegatingscrollarea.h"
#include "Common/Widgets/multisectionsamesizehorizontalflowlayoutbasedscrollarea.h"
#include "Common/Layouts/samesizehorizontalflowlayout.h"
#include "Application/Widgets/Store/sectionwithfeatureditemiconbackgroundwidget.h"
#include "Common/multicontentmanager.h"

#include "Browser/Content/Widget/plainerrorpresenterwidget.h"
#include "Common/Widgets/coverwithcustomwidget.h"


// page navigation
#include "Common/Page/pagenavigationmanager.h"
// menu
#include "Common/Menu/menumanager.h"
#include "Common/Menu/menuitemwidget.h"
// global objects
#include "Application/globalobjectregistry.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"
#include "Application/applicationglobalstateregistry.h"
#include "Application/Managers/AppGrab/grabapplicationmanager.h"

// Communication
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
//#include "Application/Communication/awresourcedownloadstreamtofile.h"
#include "Application/Communication/awresourcedownloadstreamtomemoryfactory.h"
#include "Common/Communication/basenetworkentity.h"
#include "Application/Communication/Request/storemainpagewithsectionsnetworkrequest.h"
#include "Application/Communication/Entity/applicationlistwithsectionsnetworkentity.h"

// Layouting
//#include "Common/Sorting/widgetlistsnapshothandlerdelegate.h"
//#include "Common/Sorting/widgetlistsnapshot.h"
#include "Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatebase.h"

// Subpages
#include "Browser/Content/Page/applicationstoreapplicationpage.h"
#include "Browser/Content/Page/applicationstoremorepage.h"
#include "appstoresearchresultpage.h"
#include "Browser/Content/Page/errorpage.h"


// after login actions
#include "Application/AfterLoginActions/grabsyncafterloginaction.h"
#include "Application/AfterLoginActions/showappdetailpageforappidafterloginaction.h"
#include "Application/AfterLoginActions/runappafterloginaction.h"
#include "Application/AfterLoginActions/offlineuserapplistupdatecheckafterloginaction.h"
#include "Application/AfterLoginActions/showsearchinstorepageandsearchafterloginaction.h"

// search
#include "../../AppStore/appstoresearchcontroller.h"

// utility
#include "Common/Event/showstorepageforapplicationevent.h"
#include "Common/Security/authorization.h"
#include "Application/appwhirrapplication.h"
#include "Browser/browserwindow.h"
#include "Common/Event/startthisapplicationevent.h"
#include "Common/Helpers/pathhelper.h"
#include "Common/Helpers/qpixmaphelper.h"
#include "Application/Utils/awapplicationrunner.h"

// config
#include "Settings/configmanager.h"
#include "Settings/pageidcenter.h"
#include "Settings/menuitemidcenter.h"
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"
#include "Application/speciallocalappdatabaseoperationids.h"
#include "Settings/settings.h"

using namespace AW::Common::Communication;
using namespace AW::Client::Communication;

#define NORMAL_STORE_CONTENTID                      QString("normal-store-contentId")
#define NETWORK_UNREACHABLE_STORE_CONTENTID         QString("network-unreachable-store-contentId")


ApplicationStoreMainPage::ApplicationStoreMainPage(QString pageId, QWidget *parent) :
    BrowserPageWidgetBase(pageId, parent),
    _isStoreItemsLoaded(false),
    _isForceReloadContent(false)
{
    _allStoreItems = new IdBasedWidgetCollection(this);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->setObjectName("ApplicationStoreMainPage");
    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style/ObjectName(%1)").arg("ApplicationStoreMainPage"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    this->setLayout(mainLayout);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    this->setContentsMargins(0, 0, 0, 0);

//    QWidget *mainPageContentHolderWidget = new QWidget;
//    mainLayout->addWidget(mainPageContentHolderWidget);
    QVBoxLayout *mainPageContentHolderLayout = new QVBoxLayout;
//    mainPageContentHolderWidget->setLayout(mainPageContentHolderLayout);
    mainLayout->addLayout(mainPageContentHolderLayout);
    _pageContentManager = new MultiContentManager(mainPageContentHolderLayout, this);

    QWidget *normalContentHolderWidget = new QWidget;
    QVBoxLayout *normalContentHolderLayout = new QVBoxLayout;
    normalContentHolderWidget->setContentsMargins(0, 0, 0, 0);
    normalContentHolderLayout->setContentsMargins(0, 0, 0, 0);
    normalContentHolderWidget->setLayout(normalContentHolderLayout);
    _pageContentManager->setContentWidgetToId(NORMAL_STORE_CONTENTID, normalContentHolderWidget);


    //
    // network unreachable content
//    QWidget *networkUnreachableContentHolderWidget = new QWidget;
//    QVBoxLayout *networkUnreachableContentHolderLayout = new QVBoxLayout;
//    networkUnreachableContentHolderWidget->setLayout(networkUnreachableContentHolderLayout);
//    QLabel *networkUnreachableMessageLabel = new QLabel(tr("Server is not available."));
//    networkUnreachableContentHolderLayout->addWidget(networkUnreachableMessageLabel);
    PlainErrorPresenterWidget *errorContentWidget = new PlainErrorPresenterWidget;
    _pageContentManager->setContentWidgetToId(NETWORK_UNREACHABLE_STORE_CONTENTID, errorContentWidget);

    //
    // set the default content to normal
    _pageContentManager->switchContent(NORMAL_STORE_CONTENTID);

    //
    // connect to app-state changes
    LocalAppDatabaseManager *localAppDatabaseManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager();
    connect(localAppDatabaseManager, SIGNAL(appStateChanged(QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)), this, SLOT(appStateChanged(QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)));
    connect(localAppDatabaseManager, SIGNAL(batchAppStateChanged(QMap<QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum>)), this, SLOT(batchAppStateChanged(QMap<QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum>)));
    connect(localAppDatabaseManager, SIGNAL(wholeAppDescriptionChanged(ApplicationDescriptionModel,ApplicationDescriptionModel)), this, SLOT(wholeAppDescriptionChanged(ApplicationDescriptionModel,ApplicationDescriptionModel)));

    connect(localAppDatabaseManager, SIGNAL(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), this, SLOT(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)));

    this->_appStoreSearchController = new AppStoreSearchController(this);
    connect(this->_appStoreSearchController, SIGNAL(__performSearchByFilterText(QString)), this, SLOT(_performAppStoreSearchByFilterTextEmitted(QString)));
}

void ApplicationStoreMainPage::initializePageContent()
{
    QPixmap busyIndicatorPixmap(QString(":/BusyIndicator-Spinning"));
    _busyIndicator = new BusyIndicatorWidget(busyIndicatorPixmap);
    _busyIndicator->setParent(this);
    _busyIndicator->hide();
}

void ApplicationStoreMainPage::resizeEvent(QResizeEvent *e)
{
    BrowserPageWidgetBase::resizeEvent(e);

    _busyIndicator->setGeometryToThis(this->geometry());
}

void ApplicationStoreMainPage::_showBusyIndicator()
{
    _busyIndicator->showWithAnimationAndActivate();
}

void ApplicationStoreMainPage::_hideBusyIndicator()
{
    _busyIndicator->raise();
    _busyIndicator->hideWithAnimationAndDeactivate();
}

void ApplicationStoreMainPage::batchAppStateChanged(QMap<QString, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum> appIdsAndNewStatesMap)
{
//    int idListCnt = appIdList.size();
//    for(int i = 0; i < idListCnt; i++) {
//        this->refreshAppIconState(appIdList[i]);
//    }

    QMapIterator<QString, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum> i(appIdsAndNewStatesMap);
    while (i.hasNext()) {
        i.next();
        this->appStateChanged(i.key(), i.value());
    }
}

//void ApplicationStoreMainPage::_appStateResult(QString appId, AppWhirr::ApplicationStates::ApplicationStatesEnum appState, bool isQuerySuccess)

void ApplicationStoreMainPage::_result_getAllAppDescriptions(QList<ApplicationDescriptionModel> results, QString operationId, bool isQuerySuccess)
{
    DLog("Op id: ") << operationId;
    if(isQuerySuccess) {
        if(operationId == SpecialLocalAppDatabaseOperationIds::getInitialWholeAppDatabaseQueryForUserQueryOperationId()) {
            this->_userLocalAppScanResult = results;
        }
        else if(operationId == SpecialLocalAppDatabaseOperationIds::getStorePageReloadQueryOperationId()) {
            this->__syncAppIconStatesWithUserAppList(results);
        }
        else {
            // sync here as well?
        }
    }
}

void ApplicationStoreMainPage::appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState)
{
    DLog("appStateChanged") << appId << newAppState;
    if(!_isStoreItemsLoaded) {
        DLog("App icon container not-yet initialized.");
        return;
    }

    QList<IdBasedWidgetBase *> foundWidgets = _allStoreItems->getAllById(appId);
    if(foundWidgets.isEmpty()) {
        DLog("Related app-icon not found: ") << appId;
        return;
    }

    for(int i = 0; i < foundWidgets.size(); ++i) {
        ApplicationStoreAppWidget *castedAppWidget = dynamic_cast<ApplicationStoreAppWidget *>(foundWidgets[i]);
        if(castedAppWidget == NULL) {
            WLog("Related app-icon found, but widget is inappropriate type!");
        }
        else {
            castedAppWidget->refreshByState(newAppState, false);
        }
    }
}

void ApplicationStoreMainPage::wholeAppDescriptionChanged(ApplicationDescriptionModel inputAppDescrModel, ApplicationDescriptionModel updatedAppDescrModel)
{
    DLog("wholeAppDescriptionChanged") << updatedAppDescrModel.getDynamicDescription_ReadOnly()->getAppId();
    if(!_isStoreItemsLoaded) {
        DLog("App icon container not-yet initialized.");
        return;
    }

    QList<IdBasedWidgetBase *> foundWidgets = _allStoreItems->getAllById(updatedAppDescrModel.getDynamicDescription_ReadOnly()->getAppId());
    if(foundWidgets.isEmpty()) {
        DLog("Related app-icon not found: ") << updatedAppDescrModel.getDynamicDescription_ReadOnly()->getAppId();
        return;
    }

    for(int i = 0; i < foundWidgets.size(); ++i) {
        ApplicationStoreAppWidget *castedAppWidget = dynamic_cast<ApplicationStoreAppWidget *>(foundWidgets[i]);
        if(castedAppWidget == NULL) {
            WLog("Related app-icon found, but widget is inappropriate type! appId: ") << updatedAppDescrModel.getDynamicDescription_ReadOnly()->getAppId();
        }
        else {
            castedAppWidget->refresh(updatedAppDescrModel);
        }
    }
}

void ApplicationStoreMainPage::pagePushWillHappen()
{
    DLog("Store page - push will happen");

    // this is a root page, pagePush can happen multiple times (every time the root page appears except when appears via pop), but
    //  pagePop will never happen
    // this is required by the menu-mechanism
    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();
    if(menuManager == NULL) {
        WLog("Menu manager not found.");
        return;
    }

    MenuItemWidget *pageRelatedMenuItem = menuManager->getItemById(MenuItemIdCenter::storeMenuId());

    if(pageRelatedMenuItem != NULL)
    {
        //  --- search menu item
        MenuItemWidget *searchSubMenuItemWidget = new MenuItemWidget(MenuItemIdCenter::storeSearchLineSubMenuItemId(), tr("Search:"));
        connect(searchSubMenuItemWidget, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(_searchLineMenuItemActivated(QString)));

        QLineEdit *searchLine = new QLineEdit;
        searchLine->setStyleSheet(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("LineEdit/Search/Gray/style"));
        searchLine->setPlaceholderText(tr("Type at least %1 characters").arg(ConfigManager::getSearchMinimumFilterTextLengthToPerformSearch()));
        connect(searchLine, SIGNAL(textChanged(QString)), this->_appStoreSearchController, SLOT(storeThisSearchFilterTextAsCurrent(QString)));
        connect(searchLine, SIGNAL(returnPressed()), this->_appStoreSearchController, SLOT(searchByTheStoredFilterText()));

//        connect(searchLine, SIGNAL(textChanged(QString)), this, SLOT(_searchInStoreByFilterText(QString)));
//        connect(searchLine, SIGNAL(textEdited(QString)))

        menuManager->addChildToItem(searchSubMenuItemWidget, MenuItemIdCenter::storeMenuId(), searchLine, true);
    }
    else
    {
        WLog("The menu-item is NULL. This page requires a valid menu-item.");
    }


    _forceReloadContentTimer = new QTimer(this);
    _forceReloadContentTimer->setInterval(ConfigManager::getStoreMainPageContentReloadIntervalMilliseconds());
    _forceReloadContentTimer->setSingleShot(false);
    connect(_forceReloadContentTimer, SIGNAL(timeout()), this, SLOT(__setForceReloadContent()));
    _forceReloadContentTimer->start();
}

// Page will be navigated, let's start the communication
void ApplicationStoreMainPage::pageWillAppear(QString param)
{
    // select the related menu item
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::storeMenuId());
}

void ApplicationStoreMainPage::pageDidAppear()
{
    DLog("Last used client version: ") << Settings::sharedSettings()->getLastUsedClientVersion();
    Settings::sharedSettings()->setLastUsedClientVersion(ConfigManager::getClientVersion());
//    if(Settings::sharedSettings()->getLastUsedClientVersion() != ConfigManager::getClientVersion()) {
//        _contentCoverWidgetWithCustomWidget = new CoverWithCustomWidget(, 300, -1, this);
//    }
//    else {
        // start downloading app list in store
        this->initializeRequests();
//    }
}

void ApplicationStoreMainPage::pagePopWillHappen() {
    // this is a root page - won't be called
}

void ApplicationStoreMainPage::pageWillDisappear() {
    // this is a root page - won't be called
}

// Parse the response description
void ApplicationStoreMainPage::networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities)
{
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("networkRequestFinished with result count: ") << entities.size();
#endif

    AW_QOBJECT_SAFE_DELETE(communicationDescriptionGateway);
    _resourceDownloadStreamToMemory = AW::Client::Communication::AWResourceDownloadStreamToMemoryFactory::createDefaultResourceDownloadStreamToMemory(NULL);
    connect(_resourceDownloadStreamToMemory, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QByteArray)), this, SLOT(_retrievedResourceForURL(QUrl,QByteArray)));
    connect(_resourceDownloadStreamToMemory, SIGNAL(resourceDownloadFailedForUrl(QUrl)), this, SLOT(_resourceDownloadFailedForUrl(QUrl)));
    connect(_resourceDownloadStreamToMemory, SIGNAL(finishedWithAllResourceDownloads()), this, SLOT(networkResourceRequestsFinished()));

#if 0
    applicationContainer = new DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea(QSize(250, 110), new SameSizeLayoutSnapshotChangeDelegateBase, false);
//    applicationContainer->setObjectName("applicationContainer");
//    applicationContainer->setStyleSheet(QString("QWidget#applicationContainer { background-color: %1; }").arg(GuiConfigurations::defaultLightGrayWidgetBackgroundColorHex()));
//    applicationContainer->setContentsMargins(10, 10, 10, 10);
    applicationContainer->setContentMarginsForDynamicLayout(10, 10, 10, 10);
    this->mainLayout->addWidget(applicationContainer);
    connect (applicationContainer, SIGNAL(itemActivatedSignal(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(itemSelected(IdBasedSelectableAndHighlightableWidgetBase*)));
#endif

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    // get related page-content(holder)-widget
    QWidget *storeNormalContentHolderWidget = _pageContentManager->getContentWidget(NORMAL_STORE_CONTENTID);
    if(storeNormalContentHolderWidget == NULL) {
        WLog("Page content not found.");
        return;
    }

    // get previously added multi-section-scroll-area, if any
    MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea *storeItemsMultiSectionScrollArea = NULL;
    if(storeNormalContentHolderWidget->layout()->count() > 0) {
        for(int i = 0; i < storeNormalContentHolderWidget->layout()->count() && storeItemsMultiSectionScrollArea == NULL; ++i)
        {
            QWidget *relatedWidget = storeNormalContentHolderWidget->layout()->itemAt(i)->widget();
            DLog("Obj name: ") << relatedWidget->objectName();
            storeItemsMultiSectionScrollArea = dynamic_cast<MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea *>(relatedWidget);
        }

        if(storeItemsMultiSectionScrollArea == NULL) {
            WLog("Invalid multi-section area, not found.");
            return;
        }
    }

    if(_isForceReloadContent) {
        DLog("-- force reload --");
        AW_QOBJECT_SAFE_DELETE(storeItemsMultiSectionScrollArea);
        _allStoreItems->_removeAll(true);

        _isForceReloadContent = false;
    }
    else if(storeItemsMultiSectionScrollArea != NULL) {
        AW_QOBJECT_SAFE_DELETE(storeItemsMultiSectionScrollArea);
    }

    storeItemsMultiSectionScrollArea = new MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea(this, 0);
    storeItemsMultiSectionScrollArea->setStyleSheet(guiConfManager->getStyleForId("ScrollArea/Dynamic/Default/Transparent/style"));
    storeItemsMultiSectionScrollArea->setFrameShape(QFrame::NoFrame);
    storeItemsMultiSectionScrollArea->setObjectName("storeItemsMultiSectionScrollArea");
    storeNormalContentHolderWidget->layout()->addWidget(storeItemsMultiSectionScrollArea);

    connect(storeItemsMultiSectionScrollArea, SIGNAL(showListPageWithId(QString)), this, SLOT(requestForMoreWithIdentifier(QString)));

#if 1
    DLog("Processing entities...");
    AW::Common::Communication::BaseNetworkEntity* entity = NULL;
    Q_FOREACH(entity, entities)
    {
        DLog(" - Processing entity");
//        ApplicationListNetworkEntity* appListEntity = (ApplicationListNetworkEntity*)entity;
        ApplicationListWithSectionsNetworkEntity *appListWithSectionsEntity = dynamic_cast<ApplicationListWithSectionsNetworkEntity *>(entity);
        if(appListWithSectionsEntity == NULL) {
            WLog("Cannot cast.");
            continue;
        }

        QList<ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel> sectionModels = appListWithSectionsEntity->getSections();
        for(int currSectionIndex = 0; currSectionIndex < sectionModels.size(); ++currSectionIndex)
        {
//            QString sectionWidgetStyleSheet;
//            if(currSectionIndex == 0) {
//                sectionWidgetStyleSheet = guiConfManager->getStyleForId("Store/SectionWidget/PopularSection/style");
//            }
//            else if(currSectionIndex == 1) {
//                sectionWidgetStyleSheet = guiConfManager->getStyleForId("Store/SectionWidget/HotSection/style");
//            }
//            else if(currSectionIndex == 2) {
//                sectionWidgetStyleSheet = guiConfManager->getStyleForId("Store/SectionWidget/NewSection/style");
//            }
//            else {
//                sectionWidgetStyleSheet = guiConfManager->getStyleForId("Store/SectionWidget/Default/style");
//            }

            SectionWithFeaturedItemIconBackgroundWidget *customSectionWidget = new SectionWithFeaturedItemIconBackgroundWidget;
            customSectionWidget->setStyleSheet(guiConfManager->getStyleForId("Store/SectionWidget/Default/style"));
//            customSectionWidget->setBackgroundIconPixmap(QPixmap(QString(":/app_browser_large_icon")));

            storeItemsMultiSectionScrollArea->addSectionWithWidget(sectionModels[currSectionIndex].getSectionTitle(), guiConfManager->getStyleForId("Store/SectionTitleLable/Default/style"), customSectionWidget, sectionModels[currSectionIndex].getSectionIdentifier());

            // and download the section's background-image as well
            QString sectionBackgroundImageUrl = sectionModels[currSectionIndex].getSectionBackgroundImageUrl();
            DLog("Section background image url: ") << sectionBackgroundImageUrl;
            if(!sectionBackgroundImageUrl.isEmpty())
            {
                QList<QWidget *> relatedWidgets;
                if(_tmpContentResourceUrlToPresenterWidgetsMap.contains(sectionBackgroundImageUrl)) {
                    relatedWidgets = _tmpContentResourceUrlToPresenterWidgetsMap.value(sectionBackgroundImageUrl);
                }
                else {
                    _resourceDownloadStreamToMemory->addResourceDownloadRequest(sectionBackgroundImageUrl);
                }
                relatedWidgets.append(customSectionWidget);
                _tmpContentResourceUrlToPresenterWidgetsMap[sectionBackgroundImageUrl] = relatedWidgets;
            }
            // ---

            QList<ApplicationBaseInfosNetworkEntityModel> appItemsInSection = sectionModels[currSectionIndex].getItemsInSection();
            for(int i = 0; i < appItemsInSection.size(); ++i)
            {
                ApplicationBaseInfosNetworkEntityModel currAppInSectionModel = appItemsInSection[i];
                // the store main page contains only supported apps for the current system
                currAppInSectionModel.setApplicationVersionSummedFlags(1);
                ApplicationStoreAppWidget *applicationBadgeWidget = new ApplicationStoreAppWidget(currAppInSectionModel, this);
                applicationBadgeWidget->setFixedSize(QSize(250, 110));
                connect(applicationBadgeWidget, SIGNAL(clicked(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(itemSelected(IdBasedSelectableAndHighlightableWidgetBase*)), Qt::UniqueConnection);
                connect(applicationBadgeWidget, SIGNAL(installOrUpdateThisApp(ApplicationDescriptionModel)), this, SLOT(_installOrUpdateThisApp(ApplicationDescriptionModel)), Qt::UniqueConnection);
                connect(applicationBadgeWidget, SIGNAL(runThisApp(ApplicationDescriptionModel)), this, SLOT(_runThisApp(ApplicationDescriptionModel)), Qt::UniqueConnection);
        //        applicationContainer->addWidgetToList(applicationBadgeWidget, true);
        //        applicationContainer->addIdBasedWidgetToCurrentWorkingSnapshot(applicationBadgeWidget);
                _allStoreItems->add(applicationBadgeWidget);

                storeItemsMultiSectionScrollArea->addWidgetToSection(applicationBadgeWidget, currSectionIndex);
//                }

                QList<QWidget *> relatedWidgets;
                if(_tmpContentResourceUrlToPresenterWidgetsMap.contains(currAppInSectionModel.getApplicationIconURL())) {
                    relatedWidgets = _tmpContentResourceUrlToPresenterWidgetsMap.value(currAppInSectionModel.getApplicationIconURL());
                }
                else {
                    _resourceDownloadStreamToMemory->addResourceDownloadRequest(currAppInSectionModel.getApplicationIconURL());
                }
                relatedWidgets.append(applicationBadgeWidget);
                _tmpContentResourceUrlToPresenterWidgetsMap[currAppInSectionModel.getApplicationIconURL()] = relatedWidgets;
        //        connect (communicationResourceGateway, SIGNAL(resourceReceivedForURL(QString,QByteArray)), applicationBadgeWidget, SLOT(retrievedResourceForURL(QString,QByteArray)));
            }
        }
    }

    DLog("App state sync");

    // refresh icon widgets by the local-app infos if needed
    if(!this->_userLocalAppScanResult.isEmpty()) {

        this->__syncAppIconStatesWithUserAppList(this->_userLocalAppScanResult);

        this->_userLocalAppScanResult.clear();
    }
    else {
        // it's a reload, so get the current app states now and sync the UI
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->getAllAppDescriptions("StoreMainPage-Reload");
    }

    // commit and present the result
//    this->applicationContainer->_commitCurrentWorkingSnapshot();
//    this->update();

//    _storeItemsContainerLayout->addLayout(sectionFlowLayout/*, 1, 0, 1, -1*/);
//    _storeItemsContainerLayout->addWidget(sectionContentHolderWidget);
//    _storeItemsContainerLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
//    _storeItemsContainerLayout->update();

    this->_hideBusyIndicator();
    // switch content to this, normal store content
    _pageContentManager->switchContent(NORMAL_STORE_CONTENTID);

//    connect(communicationResourceGateway, SIGNAL(finished()), this, SLOT(networkResourceRequestsFinished()));

    // Start loading all requests
    _resourceDownloadStreamToMemory->startDownloadingResources();

    // show login dialog if the user is not logged in
//    if(!Authorization::sharedAuthorization()->isUserLoggedIn()) {
//        DLog("Show login");
//        Authorization::sharedAuthorization()->showLoginDialogAndStartLogin();
//    }

    this->_checkAndPerformAfterLoginActions();
#endif
}

void ApplicationStoreMainPage::__syncAppIconStatesWithUserAppList(QList<ApplicationDescriptionModel> appList) {
    for(int i = 0; i < appList.size(); i++) {
        QList<IdBasedWidgetBase *> foundItems = _allStoreItems->getAllById(appList[i].getDynamicDescription_ReadOnly()->getAppId());
        for(int j = 0; j < foundItems.size(); ++j)
        {
            ApplicationStoreAppWidget* castedWidget = dynamic_cast<ApplicationStoreAppWidget *>(foundItems[j]);
            if(castedWidget != NULL) {
                castedWidget->refresh(appList[i]);
            }
        }
    }
}

void ApplicationStoreMainPage::_retrievedResourceForURL(QUrl URL, QByteArray response)
{
    QString url = URL.toString();
    if(this->_tmpContentResourceUrlToPresenterWidgetsMap.contains(url)) {
        QList<QWidget *> relatedPresenterWidgets = this->_tmpContentResourceUrlToPresenterWidgetsMap.value(url);
        for(int i = 0; i < relatedPresenterWidgets.size(); ++i)
        {
            ApplicationStoreAppWidget *relatedAppPresenterWidget = dynamic_cast<ApplicationStoreAppWidget *>(relatedPresenterWidgets[i]);
            if(relatedAppPresenterWidget != NULL) {
                relatedAppPresenterWidget->retrievedResourceForURL(url, response);
            }
            else {
                SectionWithFeaturedItemIconBackgroundWidget *sectionWidget = dynamic_cast<SectionWithFeaturedItemIconBackgroundWidget *>(relatedPresenterWidgets[i]);
                if(sectionWidget != NULL) {
                    sectionWidget->setBackgroundIconFromResource(response);
                }
                else {
                    WLog("Invalid widget, cannot cast!");
                }
            }
        }
    }
    else {
        WLog("Resource received but the related url cannot be found in the map!");
    }
}

void ApplicationStoreMainPage::_resourceDownloadFailedForUrl(QUrl url)
{
    WLog("Failed to download the resource at URL: ") << url.toString();
}


void ApplicationStoreMainPage::_checkAndPerformAfterLoginActions()
{
    DLog("After login action check");
    //
    // process after-login actions (if any)
    QList<AfterLoginActionInterface *> afterLoginActions = ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->getAfterLoginActions();
    bool isGrabSyncFound = false;
    bool isOfflineAppListUpdateSyncFound = false;
    for(int i = 0; i < afterLoginActions.size(); i++)
    {
        DLog("After login action: ") << afterLoginActions[i]->getTypeId();

        if(afterLoginActions[i]->getTypeId() == "GrabSyncAfterLoginAction" && !isGrabSyncFound) {
            GrabSyncAfterLoginAction *casted = dynamic_cast<GrabSyncAfterLoginAction *>(afterLoginActions[i]);
            if(casted != NULL) {
                DLog("Grabsync started");
                isGrabSyncFound = true;
                AppwhirrApplication::instance()->getBrowserWindow()->_startGrabbedAppSync();
            }
            else {
                WLog("Cannot cast!");
            }
        }

        else if(afterLoginActions[i]->getTypeId() == "OfflineUserAppListUpdateCheckAfterLoginAction" && !isGrabSyncFound && !isOfflineAppListUpdateSyncFound) {
            OfflineUserAppListUpdateCheckAfterLoginAction *casted = dynamic_cast<OfflineUserAppListUpdateCheckAfterLoginAction *>(afterLoginActions[i]);
            if(casted != NULL) {
                DLog("Offline user app list update check started");
                isOfflineAppListUpdateSyncFound = true;
                AppwhirrApplication::instance()->getBrowserWindow()->_startAppListInfoSyncForCurrentUser();
            }
            else {
                WLog("Cannot cast!");
            }
        }

        else if(afterLoginActions[i]->getTypeId() == "ShowAppDetailPageForAppIdAfterLoginAction") {
            ShowAppDetailPageForAppIdAfterLoginAction *casted = dynamic_cast<ShowAppDetailPageForAppIdAfterLoginAction *>(afterLoginActions[i]);
            if(casted != NULL) {
                DLog("ShowAppDetailPageForAppIdAfterLoginAction");
                AppwhirrApplication::instance()->getBrowserWindow()->_showStoreDetailPageForAppId(casted->getAppId());
            }
            else {
                WLog("Cannot cast!");
            }
        }

        else if(afterLoginActions[i]->getTypeId() == "RunAppAfterLoginAction") {
            RunAppAfterLoginAction *casted = dynamic_cast<RunAppAfterLoginAction *>(afterLoginActions[i]);
            if(casted != NULL) {
                DLog("RunAppAfterLoginAction");
                AppwhirrApplication::instance()->_runAppByAppId(casted->getAppId());
            }
            else {
                WLog("Cannot cast!");
            }
        }

        else if(afterLoginActions[i]->getTypeId() == "ShowSearchInStorePageAndSearchAfterLoginAction") {
            ShowSearchInStorePageAndSearchAfterLoginAction *casted = dynamic_cast<ShowSearchInStorePageAndSearchAfterLoginAction *>(afterLoginActions[i]);
            if(casted != NULL) {
                DLog("ShowSearchInStorePageAndSearchAfterLoginAction");
                AppwhirrApplication::instance()->getBrowserWindow()->_showSearchInStorePageAndSearchForFilterText(casted->getSearchFilterText());
            }
            else {
                WLog("Cannot cast!");
            }
        }

        else {
            WLog("Invalid after login action.");
        }
    }

    // delete and clear the stored actions
    ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->deleteAndClearAfterLoginActions();
}

void ApplicationStoreMainPage::failedToGetDescription(const QString& error) {
    DLog("failedToGetDescription") << error;

    this->_hideBusyIndicator();
//    this->showErrorPage();
//    this->showNetworkUnreachableCoverWidget();
    PlainErrorPresenterWidget *errorContentWidget = dynamic_cast<PlainErrorPresenterWidget *>( _pageContentManager->getContentWidget(NETWORK_UNREACHABLE_STORE_CONTENTID) );
    if(errorContentWidget == NULL) {
        WLog("Invalid error-content-widget. Cannot cast.");
    }
    else {
        errorContentWidget->setErrorMessageByDefaultMessageEnum(PlainErrorPresenterWidget::DEM_ServerUnavailable);
        _pageContentManager->switchContent(NETWORK_UNREACHABLE_STORE_CONTENTID);
    }
}

void ApplicationStoreMainPage::networkResourceRequestsFinished() {
    this->_tmpContentResourceUrlToPresenterWidgetsMap.clear();
    AW_QOBJECT_SAFE_DELETE(_resourceDownloadStreamToMemory);
}

QList<QString> ApplicationStoreMainPage::getSubpageRequestURLs() {
    return QList<QString>();
}

void ApplicationStoreMainPage::relatedMenuItemActivated(QString menuItemId) {
    DLog("Menu");
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    if(pageNavigationManager->isThisRootStackIsTheActiveOne(PageIdCenter::storePageId())) {
        // this root page's stack is the active one, so only pop-back to it
        pageNavigationManager->popBackToThisPage(PageIdCenter::storePageId());
    } else {
        // currently this root page's stack is not the active one, so switch to it
        pageNavigationManager->switchToRootPageStack(PageIdCenter::storePageId());
    }
}

void ApplicationStoreMainPage::itemSelected(IdBasedSelectableAndHighlightableWidgetBase *element) {
    ApplicationStoreAppWidget *appWidget =  dynamic_cast<ApplicationStoreAppWidget*>(element);

    if(appWidget == NULL) {
        WLog("Inappropriate type, cannot cast it.");
        return;
    }

    this->showStorePageForApp(appWidget->getRelatedAppDescription());
}


// SLOT
void ApplicationStoreMainPage::_runThisApp(ApplicationDescriptionModel appDescrModel)
{
    AW::Client::Utils::AWApplicationRunner::postRunThisAppEvent(appDescrModel);
}

// SLOT
void ApplicationStoreMainPage::_installOrUpdateThisApp(ApplicationDescriptionModel appDescrModel)
{
    // grab
    if(appDescrModel.isUpdateAvailable()) {
        // it's an update - set the state
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(appDescrModel.getDynamicDescription_ReadOnly()->getAppId(), AppWhirr::ApplicationGrabStates::InstalledAndUpdating, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "ApplicationStoreMainPage::_installThisApp");
    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->grabApplication(appDescrModel.getDynamicDescription_ReadOnly()->getAppId(), appDescrModel.getDynamicDescription_ReadOnly()->getAppName(), appDescrModel.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags(), appDescrModel.isUpdateAvailable(), appDescrModel.getDynamicDescription_ReadOnly()->getAppStatusInStore());
}

// SLOT

void ApplicationStoreMainPage::requestForMoreWithIdentifier(QString identifier) {
    DLog("Navigate to more page with identifier: ") << identifier;

    ApplicationStoreMorePage* morePage = new ApplicationStoreMorePage("page-id-AppStoreMorePage", identifier);

    // Push
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(PageIdCenter::storePageId(), morePage);
}



void ApplicationStoreMainPage::showStorePageForApp(ApplicationDescriptionModel appDescriptionModel) {
    ApplicationStoreApplicationPage* applicationPage = new ApplicationStoreApplicationPage("page-id-AppStoreAppPage", appDescriptionModel);

    // Push
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(PageIdCenter::storePageId(), applicationPage);
}

void ApplicationStoreMainPage::initializeRequests() {
    if (_isStoreItemsLoaded && !_isForceReloadContent) return;   // We've already downloaded it

    _isStoreItemsLoaded = true;

    this->_showBusyIndicator();

    AW_QOBJECT_SAFE_DELETE(communicationDescriptionGateway);
    communicationDescriptionGateway = new CommunicationDescriptionGateway;
    connect(communicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(communicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(failedToGetDescription(QString)));

    communicationDescriptionGateway->performThisRequest(new StoreMainPageWithSectionsNetworkRequest);
}

// ------------------------
// --- app store search ---
void ApplicationStoreMainPage::_searchLineMenuItemActivated(QString relatedMenuItemId)
{
    // start a search
    this->_appStoreSearchController->searchByTheStoredFilterText();
}

void ApplicationStoreMainPage::_performAppStoreSearchByFilterTextEmitted(QString givenSearchFilterText)
{
    if(givenSearchFilterText.length() < ConfigManager::getSearchMinimumFilterTextLengthToPerformSearch()) {
        // not enough characters to start a search
        return;
    }

    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    if(pageNavigationManager->isThisPageIsTheCurrentActiveOne(this->getPageId()))
    {
        DLog("This is the current active page");
        AppStoreSearchResultPage *searchResultSubPage = new AppStoreSearchResultPage("page-id-AppStoreSearchResultPage", this->_appStoreSearchController);
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->pushPageToCurrentRootStackAndShow(searchResultSubPage, QString("initial-search:%1").arg(givenSearchFilterText));
    }
    else if(pageNavigationManager->isPageInCurrentlyActiveStack(this->getPageId())) {
        // this stack is the active - test whether the search result page is already added
        if(!pageNavigationManager->isPageInCurrentlyActiveStack("page-id-AppStoreSearchResultPage")) {
            // search result page is not in the stack -> pop back to this (root, store) page then push it
            AppStoreSearchResultPage *searchResultSubPage = new AppStoreSearchResultPage("page-id-AppStoreSearchResultPage", this->_appStoreSearchController);
            pageNavigationManager->popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(this->getPageId(), searchResultSubPage, QString("initial-search:%1").arg(givenSearchFilterText));
        }
    }
    else {
        AppStoreSearchResultPage *searchResultSubPage = new AppStoreSearchResultPage("page-id-AppStoreSearchResultPage", this->_appStoreSearchController);
        pageNavigationManager->popBackToThisPageAndPushThisNewOneAndSwitchStackIfRequired(this->getPageId(), searchResultSubPage, QString("initial-search:%1").arg(givenSearchFilterText));
    }
}


// ------------------------------------------------------------------------
// --- SectionBasedHorizontalFlowLayoutPresenterDelegate implementation ---

SameSizeLayoutBase *ApplicationStoreMainPage::createLayout() {
    return new SameSizeHorizontalFlowLayout(QSize(250, 110));
}

QString ApplicationStoreMainPage::getSectionTitle(int sectionIndex) {
    return QString("Section %1").arg(sectionIndex);
}

QString ApplicationStoreMainPage::getSectionTitleLabelStyleSheet(int sectionIndex) {
    return GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Label/Default/DarkDarkGray/style");
}

QString ApplicationStoreMainPage::getSectionWidgetStyleSheet(int sectionIndex) {
    return QString();
}


// ---------------
// --- utility ---

void ApplicationStoreMainPage::__setForceReloadContent() {
    DLog("__setForceReloadContent");
    this->_isForceReloadContent = true;
}
