#include "applicationstoremorepage.h"

#include <QApplication>

#include "Browser/Content/Widget/plainerrorpresenterwidget.h"
#include "Application/Widgets/incontentnavigationwidget.h"

#include "Application/Managers/AppGrab/grabapplicationmanager.h"

#include "Common/Widgets/dynamicsamesizehorizontalboxlayoutbasedsmartscrollarea.h"
#include "Browser/Content/Widget/applicationstoreappwidget.h"
#include "Common/Widgets/busyindicatorwidget.h"
#include "Common/Layouts/flowlayout.h"
//#include "Common/Widgets/delegatingscrollarea.h"
#include "Common/Widgets/multisectionsamesizehorizontalflowlayoutbasedscrollarea.h"
#include "Common/Layouts/samesizehorizontalflowlayout.h"
#include "Application/Widgets/Store/sectionwithfeatureditemiconbackgroundwidget.h"
#include "Common/multicontentmanager.h"

// page navigation
#include "Common/Page/pagenavigationmanager.h"
// menu
#include "Common/Menu/menumanager.h"
#include "Common/Menu/menuitemwidget.h"
// global objects
#include "Application/globalobjectregistry.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"
#include "Application/applicationglobalstateregistry.h"

// Subpages
#include "Browser/Content/Page/applicationstoreapplicationpage.h"

// Communication
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
//#include "Application/Communication/Gateway/communicationresourcegateway.h"
#include "Application/Communication/awresourcedownloadstreamtomemoryfactory.h"
#include "Common/Communication/basenetworkentity.h"
#include "Application/Communication/Entity/applicationlistwithsectionsnetworkentity.h"

#include "Application/Communication/Request/applicationlistnetworkrequest.h"

// config
#include "Settings/configmanager.h"
#include "Settings/pageidcenter.h"
#include "Settings/menuitemidcenter.h"
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"

// Utility
#include "Common/Event/startthisapplicationevent.h"
#include "Common/Helpers/pathhelper.h"
#include "Application/speciallocalappdatabaseoperationids.h"
#include "Application/Utils/awapplicationrunner.h"

#define NORMAL_STORE_CONTENTID                      QString("normal-store-contentId")
#define NETWORK_UNREACHABLE_STORE_CONTENTID         QString("network-unreachable-store-contentId")

ApplicationStoreMorePage::ApplicationStoreMorePage(QString pageId, QString type, QWidget *parent) :
    BrowserPageWidgetBase(pageId, parent),
    _isContentLoaded(false),
    _isStoreItemsLoaded(false),
    _listType(type),
    _resourceDownloader(NULL)
{
    _allStoreItems = new IdBasedWidgetCollection(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    this->setLayout(mainLayout);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    this->setContentsMargins(0, 0, 0, 0);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();
    this->setObjectName("ApplicationStoreMorePage");
    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style/ObjectName(%1)").arg("ApplicationStoreMorePage"));

    //
    // in-content navigation widget
    InContentNavigationWidget *inContentNavigationWidget = new InContentNavigationWidget(tr("Go to Store"));
    connect(inContentNavigationWidget, SIGNAL(backButtonClicked()), this, SLOT(_backButtonPressed()));
    mainLayout->addWidget(inContentNavigationWidget);

    QVBoxLayout *mainPageContentHolderLayout = new QVBoxLayout;
    mainLayout->addLayout(mainPageContentHolderLayout);
    _pageContentManager = new MultiContentManager(mainPageContentHolderLayout, this);

    QWidget *normalContentHolderWidget = new QWidget;
    QVBoxLayout *normalContentHolderLayout = new QVBoxLayout;
    normalContentHolderWidget->setContentsMargins(0, 0, 0, 0);
    normalContentHolderLayout->setContentsMargins(0, 0, 0, 0);
    normalContentHolderWidget->setLayout(normalContentHolderLayout);
    _pageContentManager->setContentWidgetToId(NORMAL_STORE_CONTENTID, normalContentHolderWidget);

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
}

void ApplicationStoreMorePage::initializePageContent()
{
    if (_isContentLoaded) return;

    QPixmap busyIndicatorPixmap(QString(":/BusyIndicator-Spinning"));
    _busyIndicator = new BusyIndicatorWidget(busyIndicatorPixmap);
    _busyIndicator->setParent(this);
    _busyIndicator->hide();

    _isContentLoaded = true;
    this->_showBusyIndicator();

    AW_QOBJECT_SAFE_DELETE(communicationDescriptionGateway);
    communicationDescriptionGateway = new CommunicationDescriptionGateway;
    connect(communicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(communicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(failedToGetDescription(QString)));

    communicationDescriptionGateway->performThisRequest(new ApplicationListNetworkRequest(_listType));
}

void ApplicationStoreMorePage::resizeEvent(QResizeEvent *e)
{
    BrowserPageWidgetBase::resizeEvent(e);

    _busyIndicator->setGeometryToThis(this->geometry());
}

void ApplicationStoreMorePage::_showBusyIndicator()
{
    _busyIndicator->showWithAnimationAndActivate();
}

void ApplicationStoreMorePage::_hideBusyIndicator()
{
    _busyIndicator->raise();
    _busyIndicator->hideWithAnimationAndDeactivate();
}

void ApplicationStoreMorePage::pagePushWillHappen()
{
    // Create menu
    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();

    MenuItemWidget *applicationMenu = new MenuItemWidget(MenuItemIdCenter::storeMorePageMenuItem(), "More");
    connect(applicationMenu, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(relatedMenuItemActivated(QString)));

    menuManager->addChildToItem(applicationMenu, MenuItemIdCenter::storeMenuId());
}

// Page will be navigated, let's start the communication
void ApplicationStoreMorePage::pageWillAppear(QString param)
{
    // select the related menu item
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::storeMorePageMenuItem());
}

void ApplicationStoreMorePage::pageDidAppear()
{
    // start downloading app list in store
   // this->initializeRequests();
}

void ApplicationStoreMorePage::pagePopWillHappen() {
    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();
    menuManager->removeItemById(MenuItemIdCenter::storeMorePageMenuItem());
}

void ApplicationStoreMorePage::pageWillDisappear() {
}

// Menu

void ApplicationStoreMorePage::relatedMenuItemActivated(QString menuItemId) {
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    if(pageNavigationManager->isThisRootStackIsTheActiveOne(PageIdCenter::storePageId())) {
        // this page's stack is the active one, so only pop-back to it
        pageNavigationManager->popBackToThisPage(this->getPageId());
    } else {
        // !! it's not a root page, unnecessary
        // currently this root page's stack is not the active one, so switch to it
//        PageNavigationManager::sharedNavigationManager()->switchToRootPageStack(PageIdCenter::grabSomeAppsPageId());
    }
}

// Navigation

void ApplicationStoreMorePage::_backButtonPressed()
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popPageFromCurrentRootStackAndRemove();
}

// Communication

void ApplicationStoreMorePage::failedToGetDescription(const QString& error) {
    DLog("failedToGetDescription") << error;

    this->_hideBusyIndicator();

    PlainErrorPresenterWidget *errorContentWidget = dynamic_cast<PlainErrorPresenterWidget *>( _pageContentManager->getContentWidget(NETWORK_UNREACHABLE_STORE_CONTENTID) );
    if(errorContentWidget == NULL) {
        WLog("Invalid error-content-widget. Cannot cast.");
    }
    else {
        errorContentWidget->setErrorMessageByDefaultMessageEnum(PlainErrorPresenterWidget::DEM_ServerUnavailable);
        _pageContentManager->switchContent(NETWORK_UNREACHABLE_STORE_CONTENTID);
    }
}

// Parse the response description
void ApplicationStoreMorePage::networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities)
{
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("networkRequestFinished with result count: ") << entities.size();
#endif

    _isStoreItemsLoaded = true;

    AW_QOBJECT_SAFE_DELETE(_resourceDownloader);
    _resourceDownloader = AW::Client::Communication::AWResourceDownloadStreamToMemoryFactory::createDefaultResourceDownloadStreamToMemory(NULL);
    connect(_resourceDownloader, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QByteArray)), this, SLOT(_retrievedResourceForURL(QUrl,QByteArray)));
    connect(_resourceDownloader, SIGNAL(resourceDownloadFailedForUrl(QUrl)), this, SLOT(_resourceDownloadFailedForUrl(QUrl)));
    connect(_resourceDownloader, SIGNAL(finishedWithAllResourceDownloads()), this, SLOT(networkResourceRequestsFinished()));

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

    else if(storeItemsMultiSectionScrollArea != NULL) {
        AW_QOBJECT_SAFE_DELETE(storeItemsMultiSectionScrollArea);
    }

    storeItemsMultiSectionScrollArea = new MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea(this, 0);
    storeItemsMultiSectionScrollArea->setStyleSheet(guiConfManager->getStyleForId("ScrollArea/Dynamic/Default/Transparent/style"));
    storeItemsMultiSectionScrollArea->setFrameShape(QFrame::NoFrame);
    storeItemsMultiSectionScrollArea->setObjectName("storeItemsMultiSectionScrollArea");
    storeNormalContentHolderWidget->layout()->addWidget(storeItemsMultiSectionScrollArea);

    DLog("Processing entities...");
    AW::Common::Communication::BaseNetworkEntity* entity = NULL;
    Q_FOREACH(entity, entities)
    {
        DLog(" - Processing entity");
        ApplicationListWithSectionsNetworkEntity *appListWithSectionsEntity = dynamic_cast<ApplicationListWithSectionsNetworkEntity *>(entity);
        if(appListWithSectionsEntity == NULL) {
            WLog("Cannot cast.");
            continue;
        }

        QList<ApplicationListWithSectionsNetworkEntity::ApplicationListSectionModel> sectionModels = appListWithSectionsEntity->getSections();
        for(int currSectionIndex = 0; currSectionIndex < sectionModels.size(); ++currSectionIndex)
        {
            SectionWithFeaturedItemIconBackgroundWidget *customSectionWidget = new SectionWithFeaturedItemIconBackgroundWidget;
            customSectionWidget->setStyleSheet(guiConfManager->getStyleForId("Store/SectionWidget/Default/style"));

            storeItemsMultiSectionScrollArea->addSectionWithWidget(sectionModels[currSectionIndex].getSectionTitle(),
                                                                   guiConfManager->getStyleForId("Store/SectionTitleLable/Default/style"),
                                                                   customSectionWidget,
                                                                   sectionModels[currSectionIndex].getSectionIdentifier());

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
                _allStoreItems->add(applicationBadgeWidget);

                connect(this, SIGNAL(resourceDownloaded(QString,QByteArray)), applicationBadgeWidget, SLOT(retrievedResourceForURL(QString,QByteArray)));

                storeItemsMultiSectionScrollArea->addWidgetToSection(applicationBadgeWidget, currSectionIndex);
                _resourceDownloader->addResourceDownloadRequest(currAppInSectionModel.getApplicationIconURL());
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
    this->_hideBusyIndicator();
    // switch content to this, normal store content
    _pageContentManager->switchContent(NORMAL_STORE_CONTENTID);

    // Start loading all requests
    _resourceDownloader->startDownloadingResources();

//    this->_checkAndPerformAfterLoginActions();
}

void ApplicationStoreMorePage::_retrievedResourceForURL(QUrl URL, QByteArray response)
{
    QString url = URL.toString();
    Q_EMIT resourceDownloaded(url, response);
}

void ApplicationStoreMorePage::_resourceDownloadFailedForUrl(QUrl url)
{
    WLog("Failed to download the resource at URL: ") << url.toString();
}

void ApplicationStoreMorePage::networkResourceRequestsFinished() {
    AW_QOBJECT_SAFE_DELETE(_resourceDownloader);
}

// Application check

void ApplicationStoreMorePage::batchAppStateChanged(QMap<QString, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum> appIdsAndNewStatesMap)
{
    QMapIterator<QString, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum> i(appIdsAndNewStatesMap);
    while (i.hasNext()) {
        i.next();
        this->appStateChanged(i.key(), i.value());
    }
}

void ApplicationStoreMorePage::_result_getAllAppDescriptions(QList<ApplicationDescriptionModel> results, QString operationId, bool isQuerySuccess)
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

void ApplicationStoreMorePage::appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState)
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

void ApplicationStoreMorePage::wholeAppDescriptionChanged(ApplicationDescriptionModel inputAppDescrModel, ApplicationDescriptionModel updatedAppDescrModel)
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

void ApplicationStoreMorePage::__syncAppIconStatesWithUserAppList(QList<ApplicationDescriptionModel> appList) {
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

// App behaviour

void ApplicationStoreMorePage::itemSelected(IdBasedSelectableAndHighlightableWidgetBase *element) {
    ApplicationStoreAppWidget *appWidget =  dynamic_cast<ApplicationStoreAppWidget*>(element);

    if(appWidget == NULL) {
        WLog("Inappropriate type, cannot cast it.");
        return;
    }

    this->showStorePageForApp(appWidget->getRelatedAppDescription());
}

void ApplicationStoreMorePage::_runThisApp(ApplicationDescriptionModel appDescrModel)
{
    AW::Client::Utils::AWApplicationRunner::postRunThisAppEvent(appDescrModel);
}

void ApplicationStoreMorePage::_installOrUpdateThisApp(ApplicationDescriptionModel appDescrModel)
{
    // grab
    if(appDescrModel.isUpdateAvailable()) {
        // it's an update - set the state
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(appDescrModel.getDynamicDescription_ReadOnly()->getAppId(), AppWhirr::ApplicationGrabStates::InstalledAndUpdating, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "ApplicationStoreMainPage::_installThisApp");
    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->grabApplication(appDescrModel.getDynamicDescription_ReadOnly()->getAppId(), appDescrModel.getDynamicDescription_ReadOnly()->getAppName(), appDescrModel.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags(), appDescrModel.isUpdateAvailable(), appDescrModel.getDynamicDescription_ReadOnly()->getAppStatusInStore());
}

void ApplicationStoreMorePage::showStorePageForApp(ApplicationDescriptionModel appDescriptionModel) {
    ApplicationStoreApplicationPage* applicationPage = new ApplicationStoreApplicationPage("page-id-AppStoreAppPage", appDescriptionModel);

    // Push
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->pushPageToCurrentRootStackAndShow(applicationPage);
}

// ------------------------------------------------------------------------
// --- SectionBasedHorizontalFlowLayoutPresenterDelegate implementation ---

SameSizeLayoutBase *ApplicationStoreMorePage::createLayout() {
    return new SameSizeHorizontalFlowLayout(QSize(250, 110));
}

QString ApplicationStoreMorePage::getSectionTitle(int sectionIndex) {
    return QString("Section %1").arg(sectionIndex);
}

QString ApplicationStoreMorePage::getSectionTitleLabelStyleSheet(int sectionIndex) {
    return GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("Label/Default/DarkDarkGray/style");
}

QString ApplicationStoreMorePage::getSectionWidgetStyleSheet(int sectionIndex) {
    return QString();
}
