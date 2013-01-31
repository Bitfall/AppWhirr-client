#ifndef APPWHIRRAPPLICATION_H
#define APPWHIRRAPPLICATION_H

#include <QApplication>
#include <QMap>
#include <QList>
#include <QStringList>
#include <QLocalSocket>

QT_BEGIN_NAMESPACE
class QMainWindow;
class QLocalServer;
class QLocalSocket;
class QEvent;
class QTimer;
QT_END_NAMESPACE

class BrowserWindow;
class ApplicationRunner;

// delegates
class AutoStartupDelegate;
class GlobalHotkeyDelegate;
class SystemTrayIconDelegate;
class LocalApplicationScannerDelegate;
class UrlSchemeHandlerDelegate;

// managers
class LocalApplicationScanner;

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

class AppwhirrApplication : public QApplication
{
    Q_OBJECT
public:
    //explicit MyApplication(QObject *parent = 0);
    explicit AppwhirrApplication(int &argc, char **argv, int = QT_VERSION);
    ~AppwhirrApplication();

    static AppwhirrApplication *instance();

    //
//    void setBrowserWindow(BrowserWindow *browserWindow); // will be created when needed
    //
    void postEventToBrowserWindow(QEvent *event);
    //
    bool getIsAlreadyRunning() const;
    //
//    SortingDatabaseManager *getSortingDatabaseManager();
    //
//    LocalApplicationScanner *getLocalApplicationScanner();
    //
    void registerGlobalHotkeyForBrowserWindow();
    //
    void unregisterGlobalHotkeyForBrowserWindow();
    //
    void showSystemTrayMessage(QString message, QString title = "");
    //
    void applicationPathChanged(QString previousPath, QString currentPath);
    //
    void applicationStartedAtTheFirstTime();
    //
    void actualizeAutoStartupStateBySettings();
    //
    void userLoggedInInitializeRequiredObjects(QString userId);
    //
    void hideBrowserWindow();

    void __processGivenProgramArguments();

    //
    //
//    void grabThisApp(QString appId, QString appTitle);

    BrowserWindow* getBrowserWindow();

    //
    void PresentWarningMessageBoxOnTopOfCurrentActiveMainWindow(QString title, QString text);
    void PresentInformationMessageBoxOnTopOfCurrentActiveMainWindow(QString title, QString text);

    void __restartAppWhirrClient();

Q_SIGNALS:
    void _userAppsLoadedToLocalAppDatabase();

protected:
    //
    bool event(QEvent *event);

#ifdef Q_WS_WIN
    //
    // used for handling windows global-hotkey messages
//    bool winEvent(MSG *message, long *result); // maybe this should be winEventFilter
    bool winEventFilter(MSG *message, long *result);
#endif

public Q_SLOTS:
    //
    // these show.. methods will also ensure that the Browser window is visible
    void showBrowserWindow();
    void showMyAppsPage();
    void showStorePage();
    void showFeedbackPage();
    void showMyAppsAutorunApps();

private Q_SLOTS:
    //
    void newBatchAppsAddedToAppDatabase(bool isFoundNewItems);

    void autostartAppRunnerFinished(ApplicationRunner *source);
    void appStartedSuccessfully(QString appName, ApplicationRunner *source);
    void appStartFailed(QString appName, QString errorMessage, ApplicationRunner *source);

    void _localAppDatabaseServerStarted();
    void _localAppDatabaseServerStopped();
    void _localAppDatabaseWholeContentResult(QList<ApplicationDescriptionModel> results, QString operationId, bool isQuerySuccess);

private:

    QMainWindow *getTopmostMainWindow();

    // -------------------------------------
    // --- application start/run methods ---

    //
    // this event handler is responsible to precess the
    // "start-this-application" events
    //
    // [!] call this only from the MAIN-THREAD
    void processStartThisApplicationEvent(QEvent *e);
public:
    //
    // don't use this method if possible, because it will try to get the required app description information from the local app database
    //  post a StartThisApplicationEvent event with the required informations instead
    void _runAppByAppId(QString appId);
private:
    void __runSingleApplication(QString executableAbsolutePath, QString executableAbsoluteWorkingDirectoryPath, QString appName);

    void _registerGlobalHotkeyToWinId(WId platformDependentWindowId);
    void _unregisterGlobalHotkeyToWinId(WId platformDependentWindowId);

private Q_SLOTS:
    //
    // this method will be called right after the event loop is ready and running
    //  check for another running instance and quit of initialize regarding whether another one is running or not
    void __checkForRunningInstanceAndInitialize();
private:
    void _initApplication();


    // ------------------------------------------------
    // --- local app database handling result slots ---
private Q_SLOTS:
    void _result_getAppDescriptionModelByAppId(ApplicationDescriptionModel resultAppDescriptionModel, QString dbOperationId, bool isQuerySuccess);

    // ----------------------------------
    // --- custom url scheme handling ---
private Q_SLOTS:
    void _openAppDetailPageUrlSchemeAction(QString appId);
    void _runAppUrlSchemeAction(QString appId);
    void _searchInStoreByFilterText(QString searchFilterText);

    // -------------------------
    // --- main local server ---

private:
    bool _createServer();
private Q_SLOTS:
    //
    void _newLocalServerConnection();
    void _localServerMessageFailedWithError(QLocalSocket::LocalSocketError err);
    void _localServerMessageReadyToRead();

    // -------------------------------------------------------
    // --- communication with the already running instance ---

private Q_SLOTS:
    void _processProgramArguments(QStringList givenArguments);
    void _sendGivenProgramArgumentsToAlreadyRunningInstanceThenQuit();
    void _communicationWithAlreadyRunningInstanceFailedWithError(QString errorMessage);
    void _communicationWithAlreadyRunningInstanceSucceeded();

private:
    // used for testing whether the app is already running
    bool isAlreadyRunning;

    //
    // delegates ! _withouth_ ! parents (needed for manual releaseing)
    //  we create; no parent -> have to be deleted in the destructor
    SystemTrayIconDelegate *systemTrayIconDelegate;

    //
    // delegates with parent (this)
    //  -> don't delete them, the system will release them automatically
    AutoStartupDelegate *autoStartupDelegate;
    GlobalHotkeyDelegate *globalHotkeyDelegate;
    UrlSchemeHandlerDelegate *_urlSchemeHandlerDelegate;

    // children of this QObject, so DON'T delete them!
    QLocalServer *singleInstanceLocalServer;

    // only references, don't delete them
    BrowserWindow                       *browserWindow;

    // utility
//    QMap<QString, QString> _downloadingAppIdToTitleMap;
};

#endif // APPWHIRRAPPLICATION_H
