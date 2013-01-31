#ifndef MYAPPSCONTENTPAGE_H
#define MYAPPSCONTENTPAGE_H

#include "Browser/Content/Page/browserpagebase.h"

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Browser/Content/Widget/appiconandbuttonpresenterwidget.h"
#include "Common/Sorting/widgetlistsnapshot.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLineEdit;
QT_END_NAMESPACE

class CleverApplicationIconPresenterSortingDelegate;
class DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea;
class IdBasedSelectableAndHighlightableWidgetBase;
class MyAppsManagementSidebar;
class ApplicationDescriptionModel;
//class AW::Common::Communication::BaseNetworkEntity;
class AppUninstaller;
class ApplicationTerminator;
class IdBasedWidgetCollection;
class ContentCoverStatusMessageWidget;

class MyAppsPage : public BrowserPageWidgetBase
{
    Q_OBJECT

private:
    enum MyAppsPageModes {
        ShowAllApps,
        ShowOnlyAutostartApps,
        ShowSearchResults,
        ShowOnlyHiddenApps,
        ShowOnlyInstalledApps
    };

public:
    explicit MyAppsPage(QString pageId, QWidget *parent = 0);

    // ----------------------------
    // --- page action handling ---
    //
    // for more information see the interface's documentation (in it's header file)
    //
    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pageReactivated(QString param);
    void pagePopWillHappen();
    void pageWillDisappear();

protected:
    void resizeEvent(QResizeEvent *event);
    bool event(QEvent *event);
    void focusOutEvent(QFocusEvent *event);

Q_SIGNALS:
    void showStorePageForApp(ApplicationDescriptionModel appDescriptionModel);

public Q_SLOTS:
    void relatedMainMenuItemActivated(QString menuItemId);

//    void initialAppScanProcessing(QStringList appIdList);

protected:
    // todo ezt majd cppbe csak most gyors heggesztes
    QList<QString>getSubpageRequestURLs() { QList<QString> a; return a; }

private Q_SLOTS:

    // ----------------------------------------
    // --- app state change related methods ---
//    void singleAppGrabHappened(QString appId);
//    void singleAppInstallingStarted(QString appId);
//    void singleAppInstallHappened(QString appId);
//    void singleAppInstallFailed(QString appId);
//    void appUninstallHappened(QString appId);
//    void appUngrabHappened(QString appId);
    void appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState);
    void appAddedOrUpdated(ApplicationDescriptionModel inputAppDescriptionModel, ApplicationDescriptionModel updatedAppDescriptionModel);
    void batchAppsAddedOrUpdated(QList<ApplicationDescriptionModel> appDescriptionModels);
    void _result_getAllAppDescriptions(QList<ApplicationDescriptionModel> results, QString operationId, bool isQuerySuccess);

    void _result_updateAppIsHidden(QString appId, bool newValue, QString operationId, bool isQuerySuccess);
    void _result_updateAppIsStartWhenClientStarts(QString appId, bool newValue, QString operationId, bool isQuerySuccess);


    // -------------------------------------------------------
    // --- user grabbed apps list management and delegates ---
//    void _batchAppsGrabbed(QStringList grabbedAppIds);

    // ----------------------------------------
    // --- page sorting / filtering related ---
    void searchByFilterText(QString filter);

    void _showOnlyAutostartAppsMenuItemActivated(QString relatedMenuItemId);
    void _showOnlyHiddenAppsMenuItemActivated(QString relatedMenuItemId);
    void _showOnlyInstalledAppsMenuItemActivated(QString relatedMenuItemId);
    void _sortingHappened(WidgetListSnapshot sortingResultSnapshot);
    void _filterContentBySearchTextMenuItemActivated(QString relatedMenuItemId);


    // --------------------------------
    // --- app management delegates ---
    void activateCurrentApplicationIconPresenter();
    void manageOptionSelected(IdBasedSelectableAndHighlightableWidgetBase *source);
    void runOptionSelected(IdBasedSelectableAndHighlightableWidgetBase *source);
    void installOptionSelected(IdBasedSelectableAndHighlightableWidgetBase *source);

    void _runThisApp(ApplicationDescriptionModel appDescrModel);
    void _installThisApp(ApplicationDescriptionModel appDescrModel);
    void removeThisAppByAppId(ApplicationDescriptionModel appDescrModel);
    void showAppDetailsByAppId(ApplicationDescriptionModel appDescrModel);
    void createDesktopShortcutForThisApp(ApplicationDescriptionModel);
    void _isStartThisAppWhenClientStartsChangedSlot(QString appId, bool newValue);
    void _isThisAppHiddenChangedSlot(QString appId, bool newValue);

    // ----------------------------------
    // --- user authorization related ---
    void _userLoginStarted();
    void _userLoggedIn();
    void _userLoginAborted();

    // ---
    void _startNewLocalAppSearch();

    // ---------------------
    // --- app uninstall ---

    void _uninstallConfirmAccepted();
    void _uninstallConfirmCanceled();

    void _appUninstallProgressStateMessage(QString appId, QString progressStateMessage);
    void _appUninstallFinishedWithSuccess(ApplicationDescriptionModel inputAppDescriptionModel);
    void _appUninstallFailedWithError(ApplicationDescriptionModel inputAppDescriptionModel, QString errorMessage);
    void _appUninstallSuspendedAndWaitingForResolution(ApplicationDescriptionModel inputAppDescriptionModel);

    void _appTerminationFinishedWithSuccess(QString appId, QString appName);
    void _appTerminationFailed(QString appId, QString appName);

    // ------------------------------
    // --- app management sidebar ---

    void _appManagementSidebarSwitchedToExpandedState();
    void _appManagementSidebarSwitchedToCollapsedState();

    // ---------------
    // --- utility ---

    void _layoutChangeFinished();

    // ------------------------------------------
    // --- app sub-state icon action handlers ---
    void _removedFromStoreSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase *source);
    void _updateSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase *source);
    void _notAvailableForThisPlatformSubStateIconActivated(IdBasedSelectableAndHighlightableWidgetBase *source);

private:
//    void startInitialLocalApplicationScanResultProcessing();

    void _showAllApps();
    void _showOnlyAutostartApps();
    void _showOnlyHiddenApps();
    void _showOnlyInstalledApps();
    void _actualizeContentByCurrentPageMode(bool isActualizeRelatedMenuAsWell);

    //
    void refreshSortingByLastSearchFilter();

    //
    // will present the _newInstalledAppsSinceLastPageView if it's greater than 0
    //  if it's <= 0 it will remove the counter
    void refreshRelatedMenuWithNewAppCount();

    //
    // returns the new created or updated widget if it could add it
    AppIconAndButtonPresenterWidget *_addOrUpdateAppToWorkingSnapshot(ApplicationDescriptionModel appDescriptionModel);

    bool processKeyPressEvent(QKeyEvent *keyEvent);
    void _grabAndTransferKeyboardEventsToSearchLine();
    void _releaseKeyboardEvents();

private:
    QHBoxLayout *mainLayout;
    MyAppsManagementSidebar* appManagementSidebarWidget;

    //
    // in-class (private) delegates /
    //  (don't delete them if they have parent!)
    CleverApplicationIconPresenterSortingDelegate *cleverSortingDelegate;

    DynamicSameSizeHorizontalBoxLayoutBasedSmartScrollArea *appIconPresentersHolderDynamicScrollArea;
    IdBasedWidgetCollection *_allIdBasedWidgets;

    QString _lastSearchFilter;

    int _newInstalledAppsSinceLastPageView;

    QLineEdit *searchLineInMenu;
    bool _isShouldGrabTheFocus;

    MyAppsPageModes _currPageMode;
    AppUninstaller *_appUninstaller;

    ApplicationTerminator *_appTerminator;
    QString _appTerminationStartedForAppId;

    QWidget *_noAppsStatusHolderWidget;
    QLabel *_noAppsStatusMessageLabel;
    QWidget *_normalContentHolderWidget;

    QMap<QString, ApplicationDescriptionModel> _appUninstallConfirmMap;

    ContentCoverStatusMessageWidget *_statusMessagePresenterWidget;
};

#endif // MYAPPSCONTENTPAGE_H
