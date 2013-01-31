#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

QT_BEGIN_NAMESPACE
class QWidget;
class QVBoxLayout;
class QLineEdit;
class QTimer;
QT_END_NAMESPACE

class BrowserSideBar;
class BrowserContent;
class UserLoginDialog;
class UserLoginSidebarWidget;
class CommunicationDescriptionGateway;
//class AW::Common::Communication::BaseNetworkEntity;
class AnimatedTwoFixedSizeWidgetBase;
class AppTerminatorSidebarWidget;
class SpecialPageActionController;
class NavigationWidget;
class WidgetResizerDelegate;
class AppDatabaseSyncManager;

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    BrowserWindow(QWidget *parent = 0);

    void ensureWindowPresented();

//    bool hasAuthenticationChallenge();

    void blockUserNavigationWithHidingRelatedParts();
    void unblockUserNavigationAndShowHiddenRelatedParts();

    // -------------------------
    // --- show page methods ---


    void connectToUserLoginSignals();
    void _startGrabbedAppSync();
    void _startAppListInfoSyncForCurrentUser();

    void actualizeUserNameTitleText();

    void _appDidUninstall(ApplicationDescriptionModel appDescrModel);

protected:
    bool event(QEvent *event);
//    bool eventFilter(QObject *obj, QEvent *e);
    void paintEvent(QPaintEvent *e);
//    void mousePressEvent(QMouseEvent *e);
//    void mouseMoveEvent(QMouseEvent *e);
//    void mouseReleaseEvent(QMouseEvent *e);
//    void leaveEvent(QEvent *e);

    // -------------------------------------------
    // --- some page action/navigation methods ---
public:
    void showMyAppsPage();
    void showStorePage();
    void showMyAppsPageAutostartAppsOnly();
    void showFeedbackPage(QString defaultFeedbackText = QString());
    void showSuggestAnAppOptionsPage(QString appName = QString());

    /*! Show store-page for the given appId

        You should not use it if you can, because this will get the app-description from the database and will show the page only after it got the description.
    */
    void _showStoreDetailPageForAppId(QString appId);

    void _showSearchInStorePageAndSearchForFilterText(QString searchFilterText);


private Q_SLOTS:
    // ------------------------
    // --- grab connections ---
    void _appGrabStarted(QString appId, QString appName);
    void _appGrabbedSuccessfully(QString appId, QString appName, QString appSupportedSummedPlatformFlags, QString newestVersionAvailableForThisPlatform, AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore);
    void _appGrabFailed(QString appId, QString appName, bool isUpdate, QString errorMessage);
    void _appDownloadProgress(QString appId, int progress);
    void _appInstallationStarted(QString appId, QString appName);
    void _appInstallProgressStateMessage(QString appId, QString progressStateMessage);
    void _appInstalledSuccessfullyToDirectory(QString appId, QString appName, QString installedAppRootDir, QString installedAppVersion);
    void _appInstallFailed(QString appId, QString appName, bool isUpdate, QString errorMessage);

    void __resolutionNeededForRunningApp(QString appId, QString appName);
    void __frameworksRequireInstallForApp(QString appId, QString appName);
    void __applicationTerminatedSuccesfully(QString appId, QString appName);
    void __cannotTerminateApplication(QString appId, QString appName);

    // --- fw install allow/deny
    void _allowFrameworkInstall();
    void _denyFrameworkInstall();

    // ------------------------------------------
    // --- local app database operation slots ---
private Q_SLOTS:
    void _result_getAppDescriptionModelByAppId(ApplicationDescriptionModel resultAppDescription, QString dbOperationId, bool isQuerySuccess);
//    void _result_getAllAppDescriptions(QList<ApplicationDescriptionModel> appDescriptions, QString dbOperationId, bool isQuerySuccess);

private:
    QList<QString> __appIdsForInstallResolutionList;
    QString __appRequiresFrameworkInstallAllowOrDeny_AppId;

private Q_SLOTS:
    //
    void __clientSelfUpdateFinishedWithCheckingAndNoUpdateFound();
    void _clientSelfUpdateNewVersionFound();
    void _clientSelfUpdateNoNewVersionFound();
    void _clientSelfUpdateFailedToGetNewVersion(QString errorMessage);

    // ------------------------------------
    // --- window state manager buttons ---
    void windowStateMinimizeButtonPressed();
    void windowStateMaximizeButtonPressed();
    void windowStateCloseButtonPressed();


    // ------------------
    // --- user login ---
    void userLoggedIn();
    void userLoginCommunicationError(QString errorMessage);
    void userLoginAborted();
    void showUserLoginSidebar();
    void hideUserLoginSidebar();
    void _mainMenuSidebarSwitchedToExpandedState();
    void _mainMenuSidebarSwitchedToCollapsedState();

//    // -------------------------------------------------------
//    // --- user grabbed apps list management and delegates ---
//    void _getAllGrabbedAppForCurrentUser();
//    void grabbedAppsListRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*> resultList);
//    void _grabbedAppsListSyncFinished();
//    void _grabbedAppsCommunicationFinishedCompletely();
//    void grabbedAppsListRequestFailedWithErrorMessage(QString errorMessage);

//    // --------------------------------------------------
//    // --- current user offline app-list update check ---
//public:
//    void _startOfflineAppListUpdateCheckForCurrentUser();
//private Q_SLOTS:
//    void __doOfflineAppListUpdateCheckForTheseApps(QList<ApplicationDescriptionModel> appDescriptions);
//    void _offlineAppsListUpdateCheckRequestResult(QList<AW::Common::Communication::BaseNetworkEntity*> resultList);
//    void _offlineAppsListUpdateCheckSyncFinished();
//    void _offlineAppsListUpdateCheckRequestFinished();
//    void _offlineAppsListUpdateCheckRequestFailedWithErrorMessage(QString errorMessage);

private Q_SLOTS:
    void _startClientSelfUpdate();

private:
    void __switchToClientUpdatePageAndBlockNavigation();

    void __performPostAppInstallActions(ApplicationDescriptionModel appDescrModel);
    void __performPostAppUninstallActions(ApplicationDescriptionModel appDescrModel);

    /*! Restore the last used window size from Settings
    */
    void restoreLastUsedWindowSize();

private:

    // widgets
    QWidget*                centralWidget;
    NavigationWidget *navigationWidget;
    AnimatedTwoFixedSizeWidgetBase*                sideBarWidget;
    QWidget*                pageContentHolderWidget;

    // Layout
    QVBoxLayout*            sideBarLayout;
    QVBoxLayout*            pageContentHolderLayout;

    UserLoginSidebarWidget *_userLoginSidebar;
    AppTerminatorSidebarWidget *_appTerminatorSidebarWidget;


//#ifdef Q_WS_WIN
    QRect __winWorkaround_geometryBeforeMaximized;
    bool __winWorkaround_isMaximized;
//#endif

    bool _isShowLoginSidebarRequested;

    WidgetResizerDelegate *_windowResizeDelegate;

    //
    // delegates
    SpecialPageActionController *_specialPageActionController;
    AppDatabaseSyncManager *_appDatabaseSyncManager;
};

#endif // MAINWINDOW_H
