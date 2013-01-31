#ifndef APPLICATIONSTOREMOREPAGE_H
#define APPLICATIONSTOREMOREPAGE_H

#include <QObject>
#include <QUrl>
#include "Browser/Content/Page/browserpagebase.h"
#include "Common/Widgets/sectionbasedhorizontalflowlayoutpresenterdelegate.h"

#include "Application/ApplicationDescription/applicationstatesenum.h"
#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Common/IdBasedWidgetCollection/idbasedwidgetcollection.h"

class BusyIndicatorWidget;
class MultiContentManager;
class IdBasedSelectableAndHighlightableWidgetBase;


namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
class ResourceDownloadStreamToMemory;
}
}
}


class ApplicationStoreMorePage : public BrowserPageWidgetBase, public SectionBasedHorizontalFlowLayoutPresenterDelegate
{
    Q_OBJECT
public:
    explicit ApplicationStoreMorePage(QString pageId, QString type, QWidget *parent = 0);
    
    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

    SameSizeLayoutBase *createLayout();
    QString getSectionTitle(int sectionIndex);
    QString getSectionTitleLabelStyleSheet(int sectionIndex);
    QString getSectionWidgetStyleSheet(int sectionIndex);

Q_SIGNALS:
    void resourceDownloaded(QString url, QByteArray response);

protected Q_SLOTS:
    void relatedMenuItemActivated(QString menuItemId);

    // Communication
    void failedToGetDescription(const QString& error);
    void networkRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> entities);
    void _retrievedResourceForURL(QUrl URL, QByteArray response);
    void _resourceDownloadFailedForUrl(QUrl url);
    void networkResourceRequestsFinished();

    // Content behaviour
    void itemSelected(IdBasedSelectableAndHighlightableWidgetBase* element);
    void _installOrUpdateThisApp(ApplicationDescriptionModel appDescrModel);
    void _runThisApp(ApplicationDescriptionModel appDescrModel);

private Q_SLOTS:
    void _backButtonPressed();

    // Application changes
    void batchAppStateChanged(QMap<QString, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum> appIdsAndNewStatesMap);
    void appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState);
    void wholeAppDescriptionChanged(ApplicationDescriptionModel inputAppDescrModel, ApplicationDescriptionModel updatedAppDescrModel);
    void _result_getAllAppDescriptions(QList<ApplicationDescriptionModel> results, QString operationId, bool isQuerySuccess);
    void __syncAppIconStatesWithUserAppList(QList<ApplicationDescriptionModel> appList);

protected:
    virtual QList<QString> getSubpageRequestURLs() { return QList<QString>(); }

    void resizeEvent(QResizeEvent *e);
    void showStorePageForApp(ApplicationDescriptionModel appDescriptionModel);

private:
    void _showBusyIndicator();
    void _hideBusyIndicator();

private:
    bool _isContentLoaded;
    BusyIndicatorWidget *_busyIndicator;
    bool _isStoreItemsLoaded;
    MultiContentManager *_pageContentManager;

    QString _listType;

    IdBasedWidgetCollection *_allStoreItems;
    QList<ApplicationDescriptionModel> _userLocalAppScanResult;

    AW::Common::Communication::ResourceDownloadStreamToMemory *_resourceDownloader;
};

#endif // APPLICATIONSTOREMOREPAGE_H
