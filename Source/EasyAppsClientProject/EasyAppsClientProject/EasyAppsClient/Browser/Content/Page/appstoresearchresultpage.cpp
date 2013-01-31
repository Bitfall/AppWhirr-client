#include "appstoresearchresultpage.h"

#include <QTimer>
#include <QResizeEvent>
#include <QApplication>

// UI
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QSpacerItem>

#include "Common/Widgets/dynamicsamesizehorizontalboxlayoutbasedsmartscrollarea.h"
#include "Browser/Content/Widget/applicationstoreappwidget.h"
#include "Common/Widgets/busyindicatorwidget.h"
#include "Application/Widgets/incontentnavigationwidget.h"

// Layouting
//#include "Common/Sorting/widgetlistsnapshothandlerdelegate.h"
//#include "Common/Sorting/widgetlistsnapshot.h"
#include "Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatebase.h"

// network
#include "Application/Communication/Request/applicationsearchnetworkrequest.h"
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
//#include "Application/Communication/Gateway/communicationresourcegateway.h"
#include "Application/Communication/awresourcedownloadstreamtomemoryfactory.h"
#include "Application/Communication/Request/applicationsearchnetworkrequest.h"
#include "Common/Communication/basenetworkentity.h"
#include "Application/Communication/Entity/applicationbaseinfosnetworkentity.h"

// search controller
#include "../../AppStore/appstoresearchcontroller.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"

// subpages
#include "Browser/Content/Page/applicationstoreapplicationpage.h"

// global objects
#include "Application/globalobjectregistry.h"
#include "Common/Menu/menumanager.h"
#include "Common/Menu/menuitemwidget.h"
#include "Common/Page/pagenavigationmanager.h"

// utility
#include "Common/Helpers/pathhelper.h"

#include "Application/appwhirrapplication.h"
#include "Browser/browserwindow.h"
#include "Application/Utils/awapplicationrunner.h"

// config
#include "Settings/configmanager.h"
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"
#include "Settings/menuitemidcenter.h"
#include "Settings/pageidcenter.h"
#include "Application/Managers/AppGrab/grabapplicationmanager.h"

#define MAX_CACHED_PRESENTER_ITEM_COUNT_BEFORE_CLEANUP 50
#define SEARCH_DELAY_MILLISECONDS 500

using namespace AW::Common::Communication;

AppStoreSearchResultPage::AppStoreSearchResultPage(QString pageId, AppStoreSearchController *appStoreSearchController, QWidget *parent) :
    PageWidgetBase(pageId, parent),
    _appStoreSearchController(appStoreSearchController),
    _isSearchQueryActive(false),
    _isCurrentSearchAborted(false),
    _isLocalGrabbedAppListInitialized(false),
    _communicationDescriptionGateway(NULL),
    _resourceDownloader(NULL)
{
    _searchDelayTimer = new QTimer(this);
    _searchDelayTimer->setSingleShot(true);
    _searchDelayTimer->setInterval(SEARCH_DELAY_MILLISECONDS);
    connect(_searchDelayTimer, SIGNAL(timeout()), this, SLOT(_searchByCachedFilterTextNow()));
}


// --------------
// --- search ---

void AppStoreSearchResultPage::performSearchByFilterText(QString searchFilterText)
{
    DLog("_performSearchByFilterText: ") << searchFilterText;
    this->_startSearchRequestByFilterText(searchFilterText);
}

void AppStoreSearchResultPage::_startSearchRequestByFilterText(QString searchFilterText)
{
    if(searchFilterText.length() < ConfigManager::getSearchMinimumFilterTextLengthToPerformSearch())
    {
        // the filter text is not long enough - present a message about this and don't start searching
        DLog("Search filter is not long enough - will be skipped.");
        this->_switchToNotEnoughCharactersToPerformSearchMode();
        // also terminate current requests
        this->_abortCurrentSearchImmediatelly();

        // also commit an empty content, to look prettier next time a search result presented
        _resultIconContainerSmartScrollArea->clearCurrentWorkingSnapshot();
        _appStoreSearchController->clearCurrentlyUsedPresenterItemList();
        _resultIconContainerSmartScrollArea->_commitCurrentWorkingSnapshot();

        return;
    }

    // switch back to this page if it's not the current active one
    if(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->isPageInCurrentlyActiveStack(this->getPageId())) {
        if(!GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->isThisPageIsTheCurrentActiveOne(this->getPageId())) {
            // pop back
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popBackToThisPage(this->getPageId());
        }
    }

//    if(_isSearchQueryActive) {
//        // cannot perform it right now - cache it then perform it later
        _cachedLastGivenSearchQuery = searchFilterText;
        this->_searchDelayTimer->start();
//    }
//    else {
//        _searchByCachedFilterTextNow();
//    }
}

void AppStoreSearchResultPage::_abortCurrentSearchImmediatelly()
{
    _searchDelayTimer->stop();
    AW_QOBJECT_SAFE_DELETE(_communicationDescriptionGateway);
//    _communicationDescriptionGateway->setParent(NULL);
//    delete _communicationDescriptionGateway;
//    _communicationDescriptionGateway = NULL;
    _isSearchQueryActive = false;
    _isCurrentSearchAborted = true;
}

void AppStoreSearchResultPage::_searchByCachedFilterTextNow()
{
    if(_cachedLastGivenSearchQuery.isEmpty()) {
        DLog("Search filter is empty - will be skipped.");
        return;
    }

    if(_isSearchQueryActive) {
        WLog("Active search query detected - will perform the search after the current query finished.");
        return;
    }

    // this search will be performed

    this->_presentLoadingIndicator();

    _currentQueryToPerform = _cachedLastGivenSearchQuery;
    DLog("### _startSearchRequestByFilterText: ") << _currentQueryToPerform;

    _isSearchQueryActive = true;
    _isCurrentSearchAborted = false;
    _cachedLastGivenSearchQuery.clear(); // it will be performed now

    _searchResultForTextLabel->setText(tr("Search results for: %1").arg(_currentQueryToPerform));

    this->__initializeIfRequiredThenSearchByCurrentQueryToPerform();
}

void AppStoreSearchResultPage::__initializeIfRequiredThenSearchByCurrentQueryToPerform()
{
    if(_isLocalGrabbedAppListInitialized) {
        // initialized - start the search now
        this->__startSearchBySearchQuery(_currentQueryToPerform);
    }
    else {
        // have to initialize it - search will start after initialization
        LocalAppDatabaseManager *localAppDatabaseManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager();
        connect(localAppDatabaseManager, SIGNAL(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), this, SLOT(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)));
        connect(localAppDatabaseManager, SIGNAL(wholeAppDescriptionChanged(ApplicationDescriptionModel,ApplicationDescriptionModel)), this, SLOT(_localGrabbedAppDescriptionChanged(ApplicationDescriptionModel,ApplicationDescriptionModel)));
        connect(localAppDatabaseManager, SIGNAL(appStateChanged(QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)), this, SLOT(_appStateChanged(QString,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum)));
        localAppDatabaseManager->getAllAppDescriptions("AppStoreSearchResultPage - localGrabbedAppList initialization");
    }
}

void AppStoreSearchResultPage::_result_getAllAppDescriptions(QList<ApplicationDescriptionModel> result, QString operationId, bool isQuerySuccess)
{
    if(operationId == "AppStoreSearchResultPage - localGrabbedAppList initialization") {
        disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager(), SIGNAL(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), this, SLOT(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)));

        if(isQuerySuccess) {
            for(int i = 0; i < result.size(); i++) {
                _localGrabbedAppMap[result[i].getDynamicDescription_ReadOnly()->getAppId()] = result[i];
            }
        }
        else {
            WLog("LocalAppDatabaseQuery returned but failed.");
        }

        DLog(" --- LocalGrabbedAppList initialized");
        _isLocalGrabbedAppListInitialized = true;
        this->__startSearchBySearchQuery(_currentQueryToPerform);
    }
}

void AppStoreSearchResultPage::_localGrabbedAppDescriptionChanged(ApplicationDescriptionModel inputAppDescription, ApplicationDescriptionModel updatedAppDescription)
{
    if(!_isLocalGrabbedAppListInitialized) {
        // not yet interested
        return;
    }

    QString appId = updatedAppDescription.getDynamicDescription_ReadOnly()->getAppId();
    if(appId.isEmpty()) {
        WLog("App description changed but the related appId is empty!");
        return;
    }

    _localGrabbedAppMap[appId] = updatedAppDescription;
    // also refresh in cache if available
    ApplicationStoreAppWidget *relatedPresenterWidget = this->_appStoreSearchController->getPresenterItemFromCacheByAppId(appId);
    if(relatedPresenterWidget != NULL) {
        // found in cache
        relatedPresenterWidget->refresh(updatedAppDescription);
    }
}

void AppStoreSearchResultPage::_appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState)
{
    ApplicationStoreAppWidget *appIconPresenterWidget = this->_appStoreSearchController->getPresenterItemFromCacheByAppId(appId);

    if(appIconPresenterWidget != NULL) {
        appIconPresenterWidget->refreshByState(newAppState, true);
    }
}

void AppStoreSearchResultPage::__startSearchBySearchQuery(QString searchQuery)
{
    AW_QOBJECT_SAFE_DELETE(_communicationDescriptionGateway);
//    if(_communicationDescriptionGateway == NULL)
//    {
        _communicationDescriptionGateway = new CommunicationDescriptionGateway;
        connect(_communicationDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(_searchResultDescriptionNetworkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
        connect(_communicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_searchResultDescriptionNetworkRequestFailedWithError(QString)));
        connect(_communicationDescriptionGateway, SIGNAL(finishedCompletely()), this, SLOT(_searchResultDescriptionRequestFinishedCompletely()));
//    }

    _communicationDescriptionGateway->performThisRequest(new ApplicationSearchNetworkRequest(searchQuery));
}

void AppStoreSearchResultPage::_searchResultDescriptionNetworkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity *> resultEntities)
{
    DLog("### _searchResultDescriptionNetworkRequestFinished");

    if(_isCurrentSearchAborted) {
        DLog("Search description found, but the search is aborted.");
    }

//    this->testLabel->setText(tr("Search result cnt: %1").arg(resultEntities.size()));
    this->_hideLoadingIndicator();

    if(resultEntities.isEmpty()) {
        this->_switchToZeroResultFoundMode();
        return;
    }

    this->_switchToNormalSearchResultMode();

    // (deleting the resource-gateway will abort the previous requests)

    AW_QOBJECT_SAFE_DELETE(_resourceDownloader);
//    if(_communicationResourceGateway == NULL) {
        _resourceDownloader = AW::Client::Communication::AWResourceDownloadStreamToMemoryFactory::createDefaultResourceDownloadStreamToMemory(NULL);
        connect(_resourceDownloader, SIGNAL(finishedWithAllResourceDownloads()), this, SLOT(_searchResultResourceLoadingFinished()));
        connect(_resourceDownloader, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QByteArray)), this, SLOT(_retrievedResourceForURL(QUrl,QByteArray)));
        connect(_resourceDownloader, SIGNAL(resourceDownloadFailedForUrl(QUrl)), this, SLOT(_resourceDownloadFailedForUrl(QUrl)));
//    }

    _resultIconContainerSmartScrollArea->clearCurrentWorkingSnapshot();
    _appStoreSearchController->clearCurrentlyUsedPresenterItemList();

    // parse and present
    AW::Common::Communication::BaseNetworkEntity* entity = NULL;
    Q_FOREACH(entity, resultEntities) {
//        ApplicationListNetworkEntity* appListEntity = (ApplicationListNetworkEntity*)entity;
        ApplicationBaseInfosNetworkEntity *appListEntity = dynamic_cast<ApplicationBaseInfosNetworkEntity *>(entity);
        if(appListEntity == NULL) {
            WLog("Cannot cast.");
            continue;
        }

        QString appId = appListEntity->getApplicationID();

        //
        // check in cache
        bool isFoundInCache = true;
        ApplicationStoreAppWidget *applicationBadgeWidget = this->_appStoreSearchController->getPresenterItemFromCacheByAppId(appId);
        if(applicationBadgeWidget == NULL) {
            // not found in cache
            DLog("New item, not found in cache: ") << appId;

            // check in grabbed-app-list
            if(this->_localGrabbedAppMap.contains(appId)) {
                // found in local-grabbed-app-map, use it
                DLog("Found in local-grabbed-apps");
                applicationBadgeWidget = new ApplicationStoreAppWidget(_localGrabbedAppMap.value(appId), appListEntity->getApplicationCategory(), appListEntity->getApplicationIconURL());
            }
            else {
                // not found in cache - create it
                applicationBadgeWidget = new ApplicationStoreAppWidget(appListEntity->getModel(), this);
            }

            // and initialize
            _resourceDownloader->addResourceDownloadRequest(appListEntity->getApplicationIconURL());
            _resultResourceUrlToPresenterWidgetMap[appListEntity->getApplicationIconURL()] = applicationBadgeWidget;

            // also store in cache
            this->_appStoreSearchController->storePresenterItemInCache(appId, applicationBadgeWidget);

            isFoundInCache = false;

            connect(applicationBadgeWidget, SIGNAL(installOrUpdateThisApp(ApplicationDescriptionModel)), this, SLOT(_installOrUpdateThisApp(ApplicationDescriptionModel)));
            connect(applicationBadgeWidget, SIGNAL(runThisApp(ApplicationDescriptionModel)), this, SLOT(_runThisApp(ApplicationDescriptionModel)));
        }
        else {
            DLog("Found in cache: ") << appId;
            isFoundInCache = true;
        }

//        _resultIconContainerSmartScrollArea->addWidgetToList(applicationBadgeWidget, true);
        _resultIconContainerSmartScrollArea->addIdBasedWidgetToCurrentWorkingSnapshot(applicationBadgeWidget, isFoundInCache);

        // also store in the currently-used list
        _appStoreSearchController->addThisPresenterItemToCurrentlyUsedOnes(appId, applicationBadgeWidget);
    }

    // commit and present the result
    DLog("Current snapshot item cnt: ") << this->_resultIconContainerSmartScrollArea->getCurrentWorkingSnapshot().size();
    this->_resultIconContainerSmartScrollArea->_commitCurrentWorkingSnapshot();
//    this->_resultIconContainerSmartScrollArea->_commitSnapshotWithThisOrderAndSetAsCurrentWorking(
    this->update();


    // Start loading all requests
//    if(this->_cachedLastGivenSearchQuery.isEmpty()) {
        // performance tweak - if a cached search query is available then it will be started right after this, so don't start to download resources for the current search
    _resourceDownloader->startDownloadingResources();
//    }
}

void AppStoreSearchResultPage::_searchResultDescriptionNetworkRequestFailedWithError(QString errorMessage)
{
//    this->testLabel->setText(tr("Search failed with error: %1").arg(errorMessage));
    this->_switchToSearchErrorModeWithMessage(errorMessage);

    this->_currentSearchFinishedTryNextOne();
}

void AppStoreSearchResultPage::_searchResultDescriptionRequestFinishedCompletely()
{
    // finished completely - ready to start a new one
    this->_currentSearchFinishedTryNextOne();
}

void AppStoreSearchResultPage::_currentSearchFinishedTryNextOne()
{
    _isSearchQueryActive = false;
    if(!this->_cachedLastGivenSearchQuery.isEmpty()) {
        // try the next one
        this->_startSearchRequestByFilterText(this->_cachedLastGivenSearchQuery);
//        this->_cachedSearchQuery.clear();
    }
}

void AppStoreSearchResultPage::_retrievedResourceForURL(QUrl URL, QByteArray response)
{
    DLog("### _retrievedResourceForURL: ") << URL.toString();

    if(this->_resultResourceUrlToPresenterWidgetMap.contains(URL.toString())) {
        ApplicationStoreAppWidget *relatedPresenterWidget = this->_resultResourceUrlToPresenterWidgetMap.value(URL.toString(), NULL);
        if(relatedPresenterWidget != NULL) {
            relatedPresenterWidget->retrievedResourceForURL(URL.toString(), response);
        }
        else {
            WLog("The given URL found in the map but the related presenter widget is NULL");
        }
    }
    else {
        WLog("Resource received but the related url cannot be found in the map!");
    }
}

void AppStoreSearchResultPage::_resourceDownloadFailedForUrl(QUrl url)
{
    WLog("Failed to download the resource at URL: ") << url.toString();
}

void AppStoreSearchResultPage::_searchResultResourceLoadingFinished()
{
    this->_resultResourceUrlToPresenterWidgetMap.clear();
}

void AppStoreSearchResultPage::_checkCacheSizeAndDoCacheCleanupIfNeeded()
{
    int cachedItemCnt = this->_appStoreSearchController->getCachedItemCount();
    DLog("### _checkCacheSizeAndDoCleanupIfNeeded - cached item cnt: ") << cachedItemCnt << this->_appStoreSearchController->_getCurrentlyUsedItemCnt();
    if(cachedItemCnt > MAX_CACHED_PRESENTER_ITEM_COUNT_BEFORE_CLEANUP) {
        this->_appStoreSearchController->removeUnusedItems(true);
    }
}

// ---------------------------------------
// --- page navigation related methods ---

void AppStoreSearchResultPage::initializePageContent()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);
    this->setContentsMargins(0, 0, 0, 0);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style"));

    //
    // header
    InContentNavigationWidget *inContentNavigationWidget = new InContentNavigationWidget(tr("Go to Store"));
    connect(inContentNavigationWidget, SIGNAL(backButtonClicked()), this, SLOT(_backButtonPressed()));
    mainLayout->addWidget(inContentNavigationWidget);

    //
    // content
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));

    // status message content mode
    _statusMessageContentHolderWidget = new QWidget;
    QVBoxLayout *statusMessageContentHolderLayout = new QVBoxLayout;
    _statusMessageContentHolderWidget->setLayout(statusMessageContentHolderLayout);
    mainContentLayout->addWidget(_statusMessageContentHolderWidget);

    // add some spacer
    statusMessageContentHolderLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    // status label
    _statusMessageLabel = new QLabel;
    _statusMessageLabel->setText(tr("No apps found. Try another search or suggest an app below."));
    _statusMessageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _statusMessageLabel->setStyleSheet(guiConfManager->getStyleForId("Label/ExtraLarge/DarkGreen/style"));
    statusMessageContentHolderLayout->addWidget(_statusMessageLabel);

    _statusProgressBar = new QProgressBar;
    _statusProgressBar->setMinimum(0);
    _statusProgressBar->setMaximum(0);
    statusMessageContentHolderLayout->addWidget(_statusProgressBar);

    QPixmap busyIndicatorPixmap(QString(":/BusyIndicator-Spinning"));
    _busyIndicator = new BusyIndicatorWidget(busyIndicatorPixmap);
    _busyIndicator->setParent(this);
    _busyIndicator->hide();

    // add some spacer
    statusMessageContentHolderLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    // hide it, by default
    _statusMessageContentHolderWidget->hide();

    // normal content
    _normalContentHolderWidget = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout;
    _normalContentHolderWidget->setLayout(contentLayout);

    _searchResultForTextLabel = new QLabel(tr("Search results for:"));
    _searchResultForTextLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Large/MediumGray/style"));

    SameSizeLayoutSnapshotChangeDelegateBase *layoutSnapshotChangeDelegate = new SameSizeLayoutSnapshotChangeDelegateBase;
    connect(layoutSnapshotChangeDelegate, SIGNAL(layoutChangeFinished()), this, SLOT(_checkCacheSizeAndDoCacheCleanupIfNeeded()));
    _resultIconContainerSmartScrollArea = new DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea(QSize(250, 110), layoutSnapshotChangeDelegate, false);
//    _resultIconContainerSmartScrollArea->setContentsMargins(10, 10, 10, 10);
    _resultIconContainerSmartScrollArea->setContentMarginsForDynamicLayout(10, 10, 10, 10);
    connect (_resultIconContainerSmartScrollArea, SIGNAL(itemActivatedSignal(IdBasedSelectableAndHighlightableWidgetBase*)), this, SLOT(itemSelected(IdBasedSelectableAndHighlightableWidgetBase*)));

    contentLayout->addWidget(_searchResultForTextLabel);
    contentLayout->addSpacing(5);
    contentLayout->addWidget(_resultIconContainerSmartScrollArea);
    mainContentLayout->addWidget(_normalContentHolderWidget);


    // suggest an app
    QHBoxLayout *suggestAppLayout = new QHBoxLayout;
    QLabel *suggestAppLabel = new QLabel(tr("Cannot find the app you're looking for?"));
    suggestAppLabel->setStyleSheet(guiConfManager->getStyleForId("Label/Default/DarkDarkGray/style"));
    QPushButton *suggestAppButton = new QPushButton(tr("Suggest an app!"));
    suggestAppButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/NormalPriority/style"));
    suggestAppButton->setFlat(true);
    connect(suggestAppButton, SIGNAL(clicked()), this, SLOT(_suggestAppButtonPressed()));

    suggestAppLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    suggestAppLayout->addWidget(suggestAppLabel);
    suggestAppLayout->addSpacing(5);
    suggestAppLayout->addWidget(suggestAppButton);
    suggestAppLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

    mainContentLayout->addLayout(suggestAppLayout);

    mainLayout->addLayout(mainContentLayout);
}

void AppStoreSearchResultPage::resizeEvent(QResizeEvent *e)
{
//    QRect targetRect(this->geometry().left(), this->geometry().top(), e->size().width(), e->size().height());
    QRect targetGeom = _normalContentHolderWidget->geometry();
    _busyIndicator->setGeometryToThis(targetGeom);

    PageWidgetBase::resizeEvent(e);
}

void AppStoreSearchResultPage::_backButtonPressed()
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popPageFromCurrentRootStackAndRemove();
}

void AppStoreSearchResultPage::_suggestAppButtonPressed()
{
    AppwhirrApplication::instance()->getBrowserWindow()->showSuggestAnAppOptionsPage(_currentQueryToPerform);
}

void AppStoreSearchResultPage::pagePushWillHappen()
{
    connect(_appStoreSearchController, SIGNAL(__performSearchByFilterText(QString)), this, SLOT(performSearchByFilterText(QString)));
//    connect(_appStoreSearchController, SIGNAL(__searchFilterChangedButNotYetPerformed(QString)), this, SLOT(_performSearchByFilterText(QString)));

    // Create menu
//    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();

//    MenuItemWidget *applicationMenu = new MenuItemWidget(MenuItemIdCenter::AppStoreSearchResultPageMenuId(), "Licence");
//    connect(applicationMenu, SIGNAL(itemBecomeTheTopActiveOne(QString)), this, SLOT(relatedMenuItemActivated(QString)));

//    menuManager->addChildToItem(applicationMenu, MenuItemIdCenter::appDetailPageMenuItem());
}

void AppStoreSearchResultPage::pageWillAppear(QString param)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::storeSearchLineSubMenuItemId());

    QString initialSearchParamCommandTypePart("initial-search:");
    if(param.startsWith(initialSearchParamCommandTypePart)) {
        _cachedLastGivenSearchQuery = param.mid(initialSearchParamCommandTypePart.length());
    }
}

void AppStoreSearchResultPage::pageDidAppear()
{
//    // Load the licence from the given url
//    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
//    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
//    networkManager->get(QNetworkRequest(QUrl(this->_licenceUrl)));

    if(!this->_cachedLastGivenSearchQuery.isEmpty())
    {
        _startSearchRequestByFilterText(this->_cachedLastGivenSearchQuery);
//        _cachedLastGivenSearchQuery.clear();
    }
}

void AppStoreSearchResultPage::pagePopWillHappen()
{
    DLog("Page will be popped");

    // clear the cache
    this->_appStoreSearchController->clearCachedItems();

    //
    // remove the menu-items added by this page
//    MenuManager *menuManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager();
//    menuManager->removeItemById(MenuItemIdCenter::AppStoreSearchResultPageMenuId());
}

void AppStoreSearchResultPage::pageWillDisappear()
{

}


void AppStoreSearchResultPage::relatedMenuItemActivated(QString menuItemId) {
//    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
//    if(pageNavigationManager->isThisRootStackIsTheActiveOne(PageIdCenter::storePageId())) {
//        // this root page's stack is the active one, so only pop-back to it
//        pageNavigationManager->popBackToThisPage(this->getPageId());
//    } else {
//        // !! it's not a root page, unnecessary
//        // currently this root page's stack is not the active one, so switch to it
////        PageNavigationManager::sharedNavigationManager()->switchToRootPageStack(PageIdCenter::grabSomeAppsPageId());
//    }
}


// ----------------------
// --- item selection ---

void AppStoreSearchResultPage::itemSelected(IdBasedSelectableAndHighlightableWidgetBase *element) {
    ApplicationStoreAppWidget *appWidget =  dynamic_cast<ApplicationStoreAppWidget*>(element);

    if(appWidget == NULL) {
        WLog("Inappropriate type, cannot cast it.");
        return;
    }

    this->showStorePageForApp(appWidget->getRelatedAppDescription());
}

void AppStoreSearchResultPage::showStorePageForApp(ApplicationDescriptionModel appDescriptionModel) {
    ApplicationStoreApplicationPage* storeAppDetailPage = new ApplicationStoreApplicationPage("page-id-AppStoreAppPage-SearchResult", appDescriptionModel);

    // Push
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->pushPageToCurrentRootStackAndShow(storeAppDetailPage);
}

// SLOT
void AppStoreSearchResultPage::_runThisApp(ApplicationDescriptionModel appDescrModel)
{
    AW::Client::Utils::AWApplicationRunner::postRunThisAppEvent(appDescrModel);
}

// SLOT
void AppStoreSearchResultPage::_installOrUpdateThisApp(ApplicationDescriptionModel appDescrModel)
{
    // grab
    if(appDescrModel.isUpdateAvailable()) {
        // it's an update - set the state
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->updateAppState(appDescrModel.getDynamicDescription_ReadOnly()->getAppId(), AppWhirr::ApplicationGrabStates::InstalledAndUpdating, AppWhirr::UpdateStateCondition::USC_UpdateAnyway, "AppStoreSearchResultPage::_installOrUpdateThisApp");
    }

    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGrabApplicationManager()->grabApplication(appDescrModel.getDynamicDescription_ReadOnly()->getAppId(), appDescrModel.getDynamicDescription_ReadOnly()->getAppName(), appDescrModel.getDynamicDescription_ReadOnly()->getSupportedPlatformFlags(), appDescrModel.isUpdateAvailable(), appDescrModel.getDynamicDescription_ReadOnly()->getAppStatusInStore());
}


// -------------------------------------------
// --- UI utility and mode changer methods ---

void AppStoreSearchResultPage::_switchToNotEnoughCharactersToPerformSearchMode()
{
    this->__switchUiToStatusPresenterModeWithStatusMessage(tr("The search text is not long enough. It has to be at least %1 character long.").arg(ConfigManager::getSearchMinimumFilterTextLengthToPerformSearch()), false);
}

void AppStoreSearchResultPage::_switchToZeroResultFoundMode()
{
    this->__switchUiToStatusPresenterModeWithStatusMessage(tr("No apps found. Try another search or suggest an app below."), false);
}

void AppStoreSearchResultPage::_switchToNormalSearchResultMode()
{
//    if(!this->_normalContentHolderWidget->isHidden()) {
//        return;
//    }

    this->__switchUiToNormalContentPresenterMode();
}

void AppStoreSearchResultPage::_presentLoadingIndicator()
{
    this->_busyIndicator->raise();
    this->_busyIndicator->showWithAnimationAndActivate();
//    this->__switchUiToStatusPresenterModeWithStatusMessage(tr("Searching..."), true);
}

void AppStoreSearchResultPage::_hideLoadingIndicator()
{
    this->_busyIndicator->hideWithAnimationAndDeactivate();
//    this->__switchUiToNormalContentPresenterMode();
}

void AppStoreSearchResultPage::_switchToSearchErrorModeWithMessage(QString errorMessage)
{
    this->__switchUiToStatusPresenterModeWithStatusMessage(tr("Error during search: %1").arg(errorMessage), false);
}

void AppStoreSearchResultPage::__switchUiToStatusPresenterModeWithStatusMessage(QString statusMessage, bool isShowProgressBar)
{
    this->_hideLoadingIndicator();

    _statusMessageContentHolderWidget->show();
    _statusMessageLabel->setText(statusMessage);
    if(isShowProgressBar) {
        _statusProgressBar->show();
    }
    else {
        _statusProgressBar->hide();
    }

    _normalContentHolderWidget->hide();
}

void AppStoreSearchResultPage::__switchUiToNormalContentPresenterMode()
{
    _statusMessageContentHolderWidget->hide();
    _normalContentHolderWidget->show();
}
