#ifndef APPLICATIONSTOREMAINPAGE_H
#define APPLICATIONSTOREMAINPAGE_H

#include <QObject>
#include <QUrl>
#include "Browser/Content/Page/browserpagebase.h"
#include "Common/Widgets/sectionbasedhorizontalflowlayoutpresenterdelegate.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QGridLayout;
class QEvent;
QT_END_NAMESPACE

#include "Application/ApplicationDescription/applicationstatesenum.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Common/IdBasedWidgetCollection/idbasedwidgetcollection.h"

//
class AppStoreSearchController;
// ui
class BusyIndicatorWidget;
class MenuItemWidget;
class DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea;
class IdBasedSelectableAndHighlightableWidgetBase;
class ApplicationStoreAppWidget;
class FlowLayout;
class MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea;
class SectionWithFeaturedItemIconBackgroundWidget;
class MultiContentManager;
class CoverWithCustomWidget;

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
class ResourceDownloadStreamToMemory;
}
}
}


class ApplicationStoreMainPage : public BrowserPageWidgetBase, public SectionBasedHorizontalFlowLayoutPresenterDelegate
{
    Q_OBJECT
public:
    explicit ApplicationStoreMainPage(QString pageId, QWidget *parent = 0);

    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

public Q_SLOTS:
    void showStorePageForApp(ApplicationDescriptionModel appDescriptionModel);
    void initializeRequests();


    // ------------------------------------------------------------------------
    // --- SectionBasedHorizontalFlowLayoutPresenterDelegate implementation ---
public:
    SameSizeLayoutBase *createLayout();
    QString getSectionTitle(int sectionIndex);
    QString getSectionTitleLabelStyleSheet(int sectionIndex);
    QString getSectionWidgetStyleSheet(int sectionIndex);

protected Q_SLOTS:
    // Communication
    void networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities);
    void failedToGetDescription(const QString& error);
    void relatedMenuItemActivated(QString menuItemId);
    // resource
    void _retrievedResourceForURL(QUrl URL, QByteArray response);
    void _resourceDownloadFailedForUrl(QUrl url);
    void networkResourceRequestsFinished();

    // Content behaviour
    void itemSelected(IdBasedSelectableAndHighlightableWidgetBase* element);
    void _installOrUpdateThisApp(ApplicationDescriptionModel appDescrModel);
    void _runThisApp(ApplicationDescriptionModel appDescrModel);

    // More pages
    void requestForMoreWithIdentifier(QString identifier);

private Q_SLOTS:
    void batchAppStateChanged(QMap<QString, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum> appIdsAndNewStatesMap);
    void appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState);
    void wholeAppDescriptionChanged(ApplicationDescriptionModel inputAppDescrModel, ApplicationDescriptionModel updatedAppDescrModel);
    void _result_getAllAppDescriptions(QList<ApplicationDescriptionModel> results, QString operationId, bool isQuerySuccess);
    void __syncAppIconStatesWithUserAppList(QList<ApplicationDescriptionModel> appList);
//    void _appStateResult(QString appId, AppWhirr::ApplicationStates::ApplicationStatesEnum appState, bool isQuerySuccess);

    // ------------------------
    // --- app store search ---
    void _searchLineMenuItemActivated(QString relatedMenuItemId);
    void _performAppStoreSearchByFilterTextEmitted(QString givenSearchFilterText);

    // ---------------
    // --- utility ---
private Q_SLOTS:
    void __setForceReloadContent();

protected:
    void resizeEvent(QResizeEvent *e);

    QList<QString> getSubpageRequestURLs();

private:
    void refreshAppIconState(QString appId);
    void _checkAndPerformAfterLoginActions();

    void _showBusyIndicator();
    void _hideBusyIndicator();

private:
    // ui
//    QVBoxLayout*                    mainLayout;
//    DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea*   applicationContainer;
//    QGridLayout *_storeItemsContainerGridLayout;
//    QVBoxLayout *_storeItemsContainerLayout;

    MultiContentManager *_pageContentManager;

//    MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea *_storeItemsMultiSectionScrollArea;
    IdBasedWidgetCollection *_allStoreItems;
    BusyIndicatorWidget *_busyIndicator;
    bool _isStoreItemsLoaded;

    //
    QList<ApplicationDescriptionModel> _userLocalAppScanResult;
    QMap< QString,QList<QWidget *> > _tmpContentResourceUrlToPresenterWidgetsMap; // currently it holds the app-presenters and the section-content-holder widgets as well

    AppStoreSearchController *_appStoreSearchController;


    bool _isForceReloadContent;
    QTimer *_forceReloadContentTimer;

    AW::Common::Communication::ResourceDownloadStreamToMemory *_resourceDownloadStreamToMemory;

//    CoverWithCustomWidget *_contentCoverWidgetWithCustomWidget;


    // tmp
//    QWidget *storeItemsContainerWidget;
//    FlowLayout *sectionFlowLayout;
};

#endif // APPLICATIONSTOREMAINPAGE_H
