#ifndef APPSTORESEARCHRESULTPAGE_H
#define APPSTORESEARCHRESULTPAGE_H

#include "Common/Page/pagewidgetbase.h"

#include <QList>
#include <QMap>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QLabel;
class QProgressBar;
QT_END_NAMESPACE

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

class AppStoreSearchController;
// ui
class DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea;
class ApplicationStoreAppWidget;
class BusyIndicatorWidget;
class IdBasedSelectableAndHighlightableWidgetBase;
// newtork, communication
class CommunicationDescriptionGateway;

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
class ResourceDownloadStreamToMemory;
}
}
}



//
// How this page works:
//  * Before the first search the page will get the current User's local app descriptions
//  * When the app descriptions are available the search will start
//  * When search result arrives every item of it will be checked
//      -> first against the Cache (if it's already in the cache it will simply use the item from it)
//      -> if not found in the Cache it will check in the app descriptions retrieved from the current User's local app database. If match - will use it (but in this case it is required to build the related Presenter Widget, and will be cached as well).
//      -> if still not found the given app infos and description will be used (returned by the search query), and a new Presenter Widget will be constructed and cached
//
//  * The cache will be cleaned after it reaches the defined maximum cached item count. This is a critical opertaion -> can be performed only when no layout-change (animation) is active. If it's performed on items used in an active layout-change (animation) it will potentially crash (because cleaning the cache will also free-up unused items)
//
class AppStoreSearchResultPage : public PageWidgetBase
{
    Q_OBJECT

public:
    explicit AppStoreSearchResultPage(QString pageId, AppStoreSearchController *appStoreSearchController, QWidget *parent = 0);

    // ---------------------------------------
    // --- page navigation related methods ---
    //
    // for more information see the interface's documentation (in it's header file)
    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

private Q_SLOTS:
    void relatedMenuItemActivated(QString menuItemId);
    void _backButtonPressed();
    void _suggestAppButtonPressed();

    // --------------
    // --- search ---
private Q_SLOTS:
    void performSearchByFilterText(QString searchFilterText);
private:
    void __initializeIfRequiredThenSearchByCurrentQueryToPerform();
    //
    // This method will actually start the search-query, right after it's called. Won't check anything, every initialization have to be done before you call this.
    //  So usually you should use a (private) interface method
    void __startSearchBySearchQuery(QString searchQuery);

    void _startSearchRequestByFilterText(QString searchFilterText);
    void _currentSearchFinishedTryNextOne();
    void _abortCurrentSearchImmediatelly();
private Q_SLOTS:
    void _result_getAllAppDescriptions(QList<ApplicationDescriptionModel> result, QString operationId, bool isQuerySuccess);
    void _localGrabbedAppDescriptionChanged(ApplicationDescriptionModel inputAppDescription, ApplicationDescriptionModel updatedAppDescription);
    void _appStateChanged(QString appId,AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState);
    void _searchByCachedFilterTextNow();
    //
    void _searchResultDescriptionNetworkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity *> resultEntities);
    void _searchResultDescriptionNetworkRequestFailedWithError(QString errorMessage);
    void _searchResultDescriptionRequestFinishedCompletely();
    // resource
    void _retrievedResourceForURL(QUrl URL, QByteArray response);
    void _resourceDownloadFailedForUrl(QUrl url);
    void _searchResultResourceLoadingFinished();
    // caching
    void _checkCacheSizeAndDoCacheCleanupIfNeeded();

    // ----------------------------------------
    // --- item selection and other actions ---
private Q_SLOTS:
    void itemSelected(IdBasedSelectableAndHighlightableWidgetBase *element);

    void _runThisApp(ApplicationDescriptionModel appDescrModel);
    void _installOrUpdateThisApp(ApplicationDescriptionModel appDescrModel);
private:
    void showStorePageForApp(ApplicationDescriptionModel appDescriptionModel);

    // -------------------------------------------
    // --- UI utility and mode changer methods ---
private:
    void _switchToNotEnoughCharactersToPerformSearchMode();
    void _switchToZeroResultFoundMode();
    void _switchToNormalSearchResultMode();
    void _presentLoadingIndicator();
    void _hideLoadingIndicator();
    void _switchToSearchErrorModeWithMessage(QString errorMessage);

    void __switchUiToStatusPresenterModeWithStatusMessage(QString statusMessage, bool isShowProgressBar);
    void __switchUiToNormalContentPresenterMode();

protected:
    void resizeEvent(QResizeEvent *e);

private:
    // search controller, result caching, ...
    AppStoreSearchController *_appStoreSearchController;
    QString _cachedLastGivenSearchQuery; // Stores given query(s) until the page is capable of handling them. This can happen because 1) the page is not yer initialized or 2) there's another query not-yer-finished
    QString _currentQueryToPerform;
    bool _isSearchQueryActive;
    bool _isCurrentSearchAborted;
    QTimer *_searchDelayTimer;

    QMap<QString,ApplicationStoreAppWidget*> _resultResourceUrlToPresenterWidgetMap;
    bool _isLocalGrabbedAppListInitialized;
    QMap<QString,ApplicationDescriptionModel> _localGrabbedAppMap;

    // ui
    QLabel *_searchResultForTextLabel;
    DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea *_resultIconContainerSmartScrollArea;

    QWidget *_normalContentHolderWidget;
    QWidget *_statusMessageContentHolderWidget;
    QLabel *_statusMessageLabel;
    QProgressBar *_statusProgressBar;
    BusyIndicatorWidget *_busyIndicator;

    // network
    CommunicationDescriptionGateway *_communicationDescriptionGateway;
    AW::Common::Communication::ResourceDownloadStreamToMemory *_resourceDownloader;
};

#endif // APPSTORESEARCHRESULTPAGE_H
