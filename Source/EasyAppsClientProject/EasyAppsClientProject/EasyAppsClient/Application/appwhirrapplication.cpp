#include "appwhirrapplication.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QFileInfo>
#include <QProcess>
#include <QDir>
#include <QUrl>
#include <QHideEvent>
#include <QFileOpenEvent>

#include <QLocalServer>
#include <QLocalSocket>

#include <QAbstractEventDispatcher>
#include <QThread>

#include "Common/Helpers/pathhelper.h"
#include "appspecificpathhelper.h"

// events
#include "Common/Event/startthisapplicationevent.h"
#include "Common/Event/ensureapplicationpresentedevent.h"
#include "Common/Event/forcehideorminimizebrowserevent.h"

#include "Settings/settings.h"

#include "Browser/browserwindow.h"

#include "Common/Sorting/cleverapplicationiconpresentersortingdelegate.h"

// global objects
#include "Application/globalobjectregistry.h"
#include "Application/LocalAppDatabaseServer/localappdatabasemanager.h"
#include "Application/User/usermanager.h"
#include "Application/CriticalAction/criticalactionmanager.h"

// notification
#include "Browser/Notification/notificationmanager.h"
//#include "Common/Widgets/textandacceptcanceldetailsbuttonsnotificationwidget.h"
#include "Application/Widgets/autostartappsnotificationwidget.h"

// delegates
#include "Application/Delegates/autostartupdelegate.h"
#include "Application/Delegates/globalhotkeydelegate.h"
#include "Application/Delegates/systemtrayicondelegate.h"
#include "./Delegates/urlschemehandlerdelegate.h"

// after login actions
#include "Application/AfterLoginActions/showappdetailpageforappidafterloginaction.h"
#include "Application/AfterLoginActions/runappafterloginaction.h"
#include "Application/AfterLoginActions/showsearchinstorepageandsearchafterloginaction.h"

// utility
#include "Common/ApplicationRunner/applicationrunner.h"
#include "Application/applicationglobalstateregistry.h"
#include "Common/Communication/localmessagesender.h"
#include "Common/Communication/localmessagecodesenum.h"
#include "Common/Communication/basenetworkentity.h"

// config
#ifdef USE_DEBUG_LOCAL_SERVER_CONNECTION_NAMES
#define SINGLE_INSTANCE_LOCAL_SERVER_NAME "AppWhirr - SingleInstanceLocalServer - debug"
#else
#define SINGLE_INSTANCE_LOCAL_SERVER_NAME "AppWhirr - SingleInstanceLocalServer"
#endif

#include "Settings/pageidcenter.h"
#include "Settings/configmanager.h"
#include "Application/speciallocalappdatabaseoperationids.h"

// Security
#include "Common/Security/removefromquarantine.h"

using namespace AW::Common::Communication;

AppwhirrApplication::AppwhirrApplication(int &argc, char **argv, int qtVersion) :
    QApplication(argc, argv, qtVersion),
    isAlreadyRunning(false),
    // delegates
//    sortingDatabaseManager(NULL),
    systemTrayIconDelegate(NULL),
    autoStartupDelegate(NULL),
    globalHotkeyDelegate(NULL),
    //
    singleInstanceLocalServer(NULL),
    // references
    browserWindow(NULL)
{
    // waiting for the event loop with the initialization
    QTimer::singleShot(0, this, SLOT(__checkForRunningInstanceAndInitialize())); // will fire right after the event loop is created
}

AppwhirrApplication::~AppwhirrApplication()
{
#ifdef Q_WS_MAC
//    removeMacEvents();
#endif

    if(this->browserWindow != NULL) {
        AppwhirrApplication::instance()->_unregisterGlobalHotkeyToWinId(this->browserWindow->winId());
    }

    if(systemTrayIconDelegate != NULL) {
        delete systemTrayIconDelegate;
    }
//    if(this->sortingDatabaseManager != NULL)
//    {
////        this->sortingDatabaseManager->closeDatabase();
//        delete this->sortingDatabaseManager;
//    }
    if (this->browserWindow) {
        delete this->browserWindow;
    }
}

void AppwhirrApplication::__checkForRunningInstanceAndInitialize()
{
    //
    // create a LocalSocket and try to connect to the local server
    // if it fails try it again a few times
    // specify a timeout as well (a very short one)
    // if the server can't be reached then this instance is the first instance of this application
    // so register a LocalServer to prevent multiple instances of this application
    QLocalSocket *singleInstanceLocalSocket = new QLocalSocket(NULL);
    //connect(singleInstanceLocalSocket, SIGNAL(readyRead()), this, SLOT(readFortune()));
    //connect(singleInstanceLocalSocket, SIGNAL(error(QLocalSocket::LocalSocketError)),
    //        this, SLOT(displayError(QLocalSocket::LocalSocketError)));

    //singleInstanceLocalSocket->abort();
    singleInstanceLocalSocket->connectToServer(SINGLE_INSTANCE_LOCAL_SERVER_NAME);
    int reconnectCount = 3;
    for(; reconnectCount > 0; --reconnectCount)
    {
        if (!singleInstanceLocalSocket->waitForConnected(300)) {
            DLog("failed to connect to the server");
        } else {
            break;
        }
    }

    delete singleInstanceLocalSocket;

    if(reconnectCount <= 0) {
        // totally failed to connect to the server, so probably this is a new instance
        DLog("totally failed to connect to the server, so probably this is a new instance");

        if (!_createServer()) {
            WLog("Cannot create local server!!!");
        }

        connect(singleInstanceLocalServer, SIGNAL(newConnection()), this, SLOT(_newLocalServerConnection()));
    }
    else
    {
        // successfully connected to the server so this is not the first instance
        // -> shut it down
        //this->postEvent(this, new QEvent(QEvent::Close)); // we cannot quit in the constructor so post an Event instead

        isAlreadyRunning = true;
        this->_sendGivenProgramArgumentsToAlreadyRunningInstanceThenQuit();
    }

    if (!isAlreadyRunning) {
        this->_initApplication();
    }
}

void AppwhirrApplication::_initApplication()
{
    DLog("Initialize");

    // Register meta types
    qRegisterMetaType< AW::Common::Communication::BaseNetworkEntity* >("AW::Common::Communication::BaseNetworkEntity*");
    qRegisterMetaType< QList<AW::Common::Communication::BaseNetworkEntity*> >("QList<AW::Common::Communication::BaseNetworkEntity*>");

    //
    // register UserManager
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->setUserManager(new UserManager);
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->setCriticalActionManager(new CriticalActionManager);

    //
    // this method will be called only if the app is not running already

    _urlSchemeHandlerDelegate = new UrlSchemeHandlerDelegate(this);
    _urlSchemeHandlerDelegate->registerUrlSchemeHandler();
    connect(_urlSchemeHandlerDelegate, SIGNAL(openAppDetailPageForAppId(QString)), this, SLOT(_openAppDetailPageUrlSchemeAction(QString)));
    connect(_urlSchemeHandlerDelegate, SIGNAL(runAppForAppId(QString)), this, SLOT(_runAppUrlSchemeAction(QString)));
    connect(_urlSchemeHandlerDelegate, SIGNAL(searchInStore(QString)), this, SLOT(_searchInStoreByFilterText(QString)));

    // this is the first instance of the application

    //
    // init settings
    {
        Settings::sharedSettings();
    }

    //
    // init delegates
    {
        // parent: NULL - manual release
        // init sorting-database-manager
        this->systemTrayIconDelegate = new SystemTrayIconDelegate(NULL);
        connect(this->systemTrayIconDelegate, SIGNAL(showBrowserInCurrentState()), this, SLOT(showBrowserWindow()));
        connect(this->systemTrayIconDelegate, SIGNAL(showMyAppsPageSelected()), this, SLOT(showMyAppsPage()));
        connect(this->systemTrayIconDelegate, SIGNAL(showStorePageSelected()), this, SLOT(showStorePage()));
        connect(this->systemTrayIconDelegate, SIGNAL(showFeedbackPageSelected()), this, SLOT(showFeedbackPage()));

        // parent: this - automatic release
        this->autoStartupDelegate = new AutoStartupDelegate(this);
        this->globalHotkeyDelegate = new GlobalHotkeyDelegate(this);

        // application database loading
        //            connect(this->applicationDatabaseLoadingDelegate, SIGNAL(iconModelFoundAndProcessed(RunnableApplicationIconModel)), this, SLOT(addIconModelToListAndShowIt(RunnableApplicationIconModel)));

    }

    //
    // check autostartup settings
    this->actualizeAutoStartupStateBySettings();

#if 0
    // setup hotkeys
    {
        //QAbstractEventDispatcher::instance

        if (Settings::sharedSettings()->getIsSetGlobalHotkeyToLauncher()) {
            this->globalHotkeyDelegate->activateGlobalHotkey(this->winId());
        } else {
            this->globalHotkeyDelegate->deactivateGlobalHotkey(this->winId());
        }
    }
#endif

    LocalAppDatabaseManager *localAppDatabaseManager = new LocalAppDatabaseManager;
    localAppDatabaseManager->initialize();
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->setLocalAppDatabaseManager(localAppDatabaseManager);
    connect(localAppDatabaseManager, SIGNAL(_result_getAppDescriptionModelByAppId(ApplicationDescriptionModel,QString,bool)), this, SLOT(_result_getAppDescriptionModelByAppId(ApplicationDescriptionModel,QString,bool)));

    CleverApplicationIconPresenterSortingDelegate *cleverSortingDelegate = new CleverApplicationIconPresenterSortingDelegate;
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->setCleverSortingDelegate(cleverSortingDelegate);

    // start a scan for local, installed applications
    // -> moved: start the scan when required - when BrowserWindow created
    //        connect(this->_localApplicationScanner, SIGNAL(applicationScanEndedAndNewItemsFound()), this, SLOT(localAppScannerFinished()));
    //        _localApplicationScanner->scanForLocalApplications();

    //
    // and present the browser window
    this->showBrowserWindow();

    // system tray icon
    this->systemTrayIconDelegate->createTheSystemTrayItem();

    this->__processGivenProgramArguments();

#ifdef Q_OS_MAC
    DLog("Remove AppWhirrU from quarantine");

    RemoveFromQuarantine::releaseFromQuarantine(AppSpecificPathHelper::getUpdaterExecutablePath().toAscii());
#endif

    ILog("Main thread id: ") << QThread::currentThreadId();
}

void AppwhirrApplication::userLoggedInInitializeRequiredObjects(QString userId)
{
    QString userLocalDirectoryPath = AppSpecificPathHelper::getLocalUserDirectoryPathForUserId(userId);

    // clever sorting database

    QString cleverSortingDatabasePath = PathHelper::combineAndCleanPathes(userLocalDirectoryPath, "Sorting.db");
    if(cleverSortingDatabasePath.isEmpty()) {
        WLog("Cannot start cleverSortingDatabasePath because the given path is empty!");
    }
    else
    {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCleverSortingDelegate()->_loadSortingDatabase(cleverSortingDatabasePath);
    }


    // local app database

    QString userLocalAppServerPath = PathHelper::combineAndCleanPathes(userLocalDirectoryPath, "LocalAppDatabase.db");
    if(userLocalAppServerPath.isEmpty()) {
        WLog("Cannot start local-app-database-server because the given path is empty!");
    }
    else
    {
        LocalAppDatabaseManager *localAppDatabaseServer = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager();
//        disconnect(localAppDatabaseServer, SIGNAL(serverStarted()), this, SLOT(_localAppDatabaseServerStarted()));
//        disconnect(localAppDatabaseServer, SIGNAL(serverStopped()), this, SLOT(_localAppDatabaseServerStopped()));
//        disconnect(localAppDatabaseServer, SIGNAL(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), this, SLOT(_localAppDatabaseWholeContentResult(QList<ApplicationDescriptionModel>,QString,bool)));
        connect(localAppDatabaseServer, SIGNAL(serverStarted()), this, SLOT(_localAppDatabaseServerStarted()), Qt::UniqueConnection);
        connect(localAppDatabaseServer, SIGNAL(serverStopped()), this, SLOT(_localAppDatabaseServerStopped()), Qt::UniqueConnection);
        connect(localAppDatabaseServer, SIGNAL(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), this, SLOT(_localAppDatabaseWholeContentResult(QList<ApplicationDescriptionModel>,QString,bool)), Qt::UniqueConnection);

        localAppDatabaseServer->startServer(userLocalAppServerPath);

        ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->setIsInInitialUserAppLoading(true);
    }
}

void AppwhirrApplication::_localAppDatabaseServerStarted()
{
    DLog("--server started--");
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->getAllAppDescriptions(SpecialLocalAppDatabaseOperationIds::getInitialWholeAppDatabaseQueryForUserQueryOperationId());
}

void AppwhirrApplication::_localAppDatabaseServerStopped()
{
    DLog("--server stopped--");
}

void AppwhirrApplication::_localAppDatabaseWholeContentResult(QList<ApplicationDescriptionModel> results, QString operationId, bool isQuerySuccess)
{
    DLog(" ---> result cnt, operation-id and success:") << results.size() << operationId << isQuerySuccess;

    if(operationId == SpecialLocalAppDatabaseOperationIds::getInitialWholeAppDatabaseQueryForUserQueryOperationId()) {
        // disconnect - not interested anymore
        disconnect(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager(), SIGNAL(_result_getAllAppDescriptions(QList<ApplicationDescriptionModel>,QString,bool)), this, SLOT(_localAppDatabaseWholeContentResult(QList<ApplicationDescriptionModel>,QString,bool)));



        //
        // check for autostart apps

        ApplicationRunner *appRunnerForAutostartApps = NULL;
        int appCnt = results.size();
        if(appCnt > 0) {
            appRunnerForAutostartApps = new ApplicationRunner(true);
            connect(appRunnerForAutostartApps, SIGNAL(failedWithApp(QString,QString,ApplicationRunner*)), this, SLOT(appStartFailed(QString,QString,ApplicationRunner*)));
            connect(appRunnerForAutostartApps, SIGNAL(successWithApp(QString,ApplicationRunner*)), this, SLOT(appStartedSuccessfully(QString,ApplicationRunner*)));
            connect(appRunnerForAutostartApps, SIGNAL(finishedWithQueueAndSafeToDelete(ApplicationRunner*)), this, SLOT(autostartAppRunnerFinished(ApplicationRunner*)));
        }
        for(int i = 0; i < appCnt; i++) {
            ApplicationDescriptionModel currAppDescriptionModel = results[i];

            AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum appState = currAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppState();

            //
            // app state normalizations -> will normalize apps which started to update or install but interrupted
            if((int)appState > (int)AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled && (int)appState < (int)AppWhirr::ApplicationGrabStates::Installed) {
                // not installed - downgrade to grabbed
                appState = AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled;
                GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->downgradeAppToGrabbedState(currAppDescriptionModel);
            }
            else if((int)appState > (int)AppWhirr::ApplicationGrabStates::Installed) {
                //  - downgrade to installed if it's installed correctly -
                // currently: downgrade to grabbed state, because if an update was interrupted it could delete files of the app before it was interrupted
                //  this should be improved
                appState = AppWhirr::ApplicationGrabStates::GrabbedButNotInstalled;
                GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->downgradeAppToGrabbedState(currAppDescriptionModel);
            }

            // test whether to auto-start it
            DLog("Autostart test: ") << appState << currAppDescriptionModel.getDynamicDescription_ReadOnly()->getIsStartWhenClientStarts();
            if( (int)appState >= (int)AppWhirr::ApplicationGrabStates::Installed && currAppDescriptionModel.getDynamicDescription_ReadOnly()->getIsStartWhenClientStarts())
            {
                QString executableAbsolutePath = PathHelper::combineAndCleanPathes(currAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), currAppDescriptionModel.getStaticDescription_ReadOnly()->getExecutablePath());
                QString absoluteWorkingDir = PathHelper::combineAndCleanPathes(currAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), currAppDescriptionModel.getStaticDescription_ReadOnly()->getExecutableWorkingDirectory());

                DLog("- autostart app found: ") << executableAbsolutePath;

                if(PathHelper::isFileExecutable(executableAbsolutePath)) {
                    DLog("Add as autorun app with path: ") << executableAbsolutePath;
                    appRunnerForAutostartApps->addApplicationToRunQueue(executableAbsolutePath, absoluteWorkingDir, currAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppName());
                }
            }
        }
        if(appCnt > 0) {
            // and also start the autostart-runner
            if(!appRunnerForAutostartApps->isRunQueueEmpty()) {
                // present a notification about the batch-application running, and only run them if the user accepted
//                int appCount = appRunnerForAutostartApps->getAppCountInQueue();
//                QString appString = appCount > 1 ? tr("apps") : tr("app");
//                QString approvalText = QString(tr("Do you want to run the installed %1 %2 you marked to run when client starts?")).arg(appCount).arg(appString);
                AutostartAppsNotificationWidget *userApprovalNotification = new AutostartAppsNotificationWidget;
                connect(userApprovalNotification, SIGNAL(runOptionSelected()), appRunnerForAutostartApps, SLOT(runApplicationsInQueueWithDelay()));
                connect(userApprovalNotification, SIGNAL(abortOptionSelected()), appRunnerForAutostartApps, SLOT(clearApplicationsFromQueue()));
                connect(userApprovalNotification, SIGNAL(detailsOptionSelected()), this, SLOT(showMyAppsAutorunApps()));
                GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addNotificationWithSelfTimeoutingCustomWidget(userApprovalNotification, QString("User approval accept/cancel notification about batch application run."), QString("autostart-app-notif"));
            } else {
                AW_QOBJECT_SAFE_DELETE(appRunnerForAutostartApps);
            }
        }


        //
        // and finally - emit we're done

        Q_EMIT _userAppsLoadedToLocalAppDatabase();
    }
}

void AppwhirrApplication::newBatchAppsAddedToAppDatabase(bool isFoundNewItems)
{
//    if(!isFoundNewItems) {
//        DLog("New new items scanned. Do nothing.");
//    }

//    else {
//        QList<ApplicationDescriptionModel> foundAppDescriptionModels = this->_localApplicationScanner->getFoundApplicationDescriptionModels();
//        int appCnt = foundAppDescriptionModels.size();
//        ApplicationRunner *appRunnerForAutostartApps = NULL;
//        if(appCnt > 0) {
////            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseServer()->startBatchInstalledApplicationAdd();
//            appRunnerForAutostartApps = new ApplicationRunner;
//            connect(appRunnerForAutostartApps, SIGNAL(finishedWithQueueAndSafeToDelete(ApplicationRunner*)), this, SLOT(autostartAppRunnerFinished(ApplicationRunner*)));
//        }
//        for(int i = 0; i < appCnt; i++) {
//            ApplicationDescriptionModel currAppDescriptionModel = foundAppDescriptionModels[i];
//            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseServer()->addInstalledOrGrabbedApplication(currAppDescriptionModel.getStaticDescription()->getAppId(), currAppDescriptionModel.getDynamicDescription()->getInstalledRootPath());


//            // test whether to auto-start it
//            if(currAppDescriptionModel.getDynamicDescription()->getIsStartWhenClientStarts()) {
//                QString executableAbsolutePath = PathHelper::combineAndCleanPathes(currAppDescriptionModel.getDynamicDescription()->getInstalledRootPath(), currAppDescriptionModel.getStaticDescription()->getExecutablePath());
//                QString absoluteWorkingDir = PathHelper::combineAndCleanPathes(currAppDescriptionModel.getDynamicDescription()->getInstalledRootPath(), currAppDescriptionModel.getStaticDescription()->getExecutableWorkingDirectory());

//                if(PathHelper::isFileExistsAndItsReallyAFile(executableAbsolutePath)) {
//                    //                DLog("Add as autorun app with path: ") << executableAbsolutePath;
//                    appRunnerForAutostartApps->addApplicationToRunQueue(executableAbsolutePath, absoluteWorkingDir, currAppDescriptionModel.getStaticDescription()->getAppName());
//                }
//            }
//        }
//        if(appCnt > 0) {
////            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseServer()->commitBatchInstalledApplicationAdd();

//            // and also start the autostart-runner
//            if(!appRunnerForAutostartApps->isRunQueueEmpty()) {
//                // present a notification about the batch-application running, and only run them if the user accepted
//                QString approvalText = QString(tr("Do you want to run the installed %1 app(s) you marked to run when client starts?")).arg(appRunnerForAutostartApps->getAppCountInQueue());
//                TextAndAcceptCancelDetailsButtonsNotificationWidget *userApprovalNotification = new TextAndAcceptCancelDetailsButtonsNotificationWidget(approvalText, tr("Run"), tr("Cancel"), tr("Show them"), false);
//                connect(userApprovalNotification, SIGNAL(acceptOptionSelected()), appRunnerForAutostartApps, SLOT(runApplicationsInQueueWithDelay()));
//                connect(userApprovalNotification, SIGNAL(cancelOptionSelected()), appRunnerForAutostartApps, SLOT(clearApplicationsFromQueue()));
//                connect(userApprovalNotification, SIGNAL(detailsOptionSelected()), this, SLOT(showMyAppsAutorunApps()));
//                GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addNotificationWithSelfTimeoutingCustomWidget(userApprovalNotification, QString("User approval accept/cancel notification about batch application run."));
//            } else {
//                AW_QOBJECT_SAFE_DELETE(appRunnerForAutostartApps);
//            }
//        }
//    }

    Q_EMIT _userAppsLoadedToLocalAppDatabase();

//    AW_QOBJECT_SAFE_DELETE(this->_localApplicationScanner);
}

void AppwhirrApplication::autostartAppRunnerFinished(ApplicationRunner *source)
{
    AW_QOBJECT_SAFE_DELETE(source);
}

void AppwhirrApplication::appStartedSuccessfully(QString appName, ApplicationRunner *source)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("Application started: ") + appName, 3000);

    if(!source->getIsQueueMode()) {
        AW_QOBJECT_SAFE_DELETE(source);
    }
}

void AppwhirrApplication::appStartFailed(QString appName, QString errorMessage, ApplicationRunner *source)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("Application (%1) cannot be started. Error: %2").arg(appName).arg(errorMessage), 10000, NotificationManager::ErrorPriority);

    if(!source->getIsQueueMode()) {
        AW_QOBJECT_SAFE_DELETE(source);
    }
}

QMainWindow *AppwhirrApplication::getTopmostMainWindow()
{
    return this->getBrowserWindow();
}

void AppwhirrApplication::PresentWarningMessageBoxOnTopOfCurrentActiveMainWindow(QString title, QString text)
{
    QMessageBox::warning(this->getTopmostMainWindow(), title, text);
}

void AppwhirrApplication::PresentInformationMessageBoxOnTopOfCurrentActiveMainWindow(QString title, QString text)
{
    QMessageBox::information(this->getTopmostMainWindow(), title, text);
}

void AppwhirrApplication::__restartAppWhirrClient()
{
    this->exit(ConfigManager::getRestartAppWhirrClient_returnCode());
}

void AppwhirrApplication::actualizeAutoStartupStateBySettings()
{
    // make it auto startup
    if (Settings::sharedSettings()->getIsAutoStartAtSystemStartup()) {
        if(!this->autoStartupDelegate->makeItAutoStartAtSystemStartup()) {
            this->PresentWarningMessageBoxOnTopOfCurrentActiveMainWindow(tr("Failed"), tr("Failed to add to startup. Probably you don't have permission to do it."));
        }
    } else {
        if(!this->autoStartupDelegate->removeItFromAutoStartList()) {
            this->PresentWarningMessageBoxOnTopOfCurrentActiveMainWindow(tr("Auto Startup"), tr("Failed to remove from startup. Probably you don't have permission to do it."));
        }
    }
}

//void MyApplication::grabThisApp(QString appId, QString appTitle)
//{
//    QString notifText = "Application grab started: ";
//    notifText += appTitle;
//    GlobalObjectManager::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(notifText, 5000);

//    _downloadingAppIdToTitleMap.insert(appId, appTitle); // store it temporarily
//    this->grabAppManager->grabThisApp(appId);
//}

BrowserWindow* AppwhirrApplication::getBrowserWindow() {
    if(this->browserWindow == NULL) {
        this->browserWindow = new BrowserWindow;

        if (Settings::sharedSettings()->getIsSetGlobalHotkeyToLauncher()) {
//            AppwhirrApplication::instance()->_registerGlobalHotkeyToWinId(this->browserWindow->winId());
            AppwhirrApplication::instance()->registerGlobalHotkeyForBrowserWindow();
        }

        // start a scan for local, installed applications
//        _localApplicationScanner->scanForLocalApplications();
    }

    return browserWindow;
}

#if 0
#ifdef Q_WS_WIN
bool MyApplication::winEvent(MSG *message, long *result)
{
    if(this->globalHotkeyDelegate != NULL) {
        if(this->globalHotkeyDelegate->processWinEvent(message, result)) {
            return true;
        }
    }

    return QApplication::winEvent(message, result);
}
#endif
#endif

#ifdef Q_WS_WIN
bool AppwhirrApplication::winEventFilter(MSG *message, long *result)
{
//    qDebug(" !! win evt");
    if(this->globalHotkeyDelegate != NULL) {
//        qDebug("hotkey delegate found");
        if(this->globalHotkeyDelegate->processWinEvent(message, result)) {
            return true;
        }
    }

    return QApplication::winEventFilter(message, result);
}
#endif

void AppwhirrApplication::_registerGlobalHotkeyToWinId(WId platformDependentWindowId) {
    if(this->globalHotkeyDelegate != NULL) {
        this->globalHotkeyDelegate->activateGlobalHotkey(platformDependentWindowId);
    }
}

void AppwhirrApplication::registerGlobalHotkeyForBrowserWindow()
{
    this->_registerGlobalHotkeyToWinId(this->getBrowserWindow()->winId());
}

void AppwhirrApplication::_unregisterGlobalHotkeyToWinId(WId platformDependentWindowId) {
    if(this->globalHotkeyDelegate != NULL) {
        this->globalHotkeyDelegate->deactivateGlobalHotkey(platformDependentWindowId);
    }
}

void AppwhirrApplication::unregisterGlobalHotkeyForBrowserWindow()
{
    this->_unregisterGlobalHotkeyToWinId(this->getBrowserWindow()->winId());
}

bool AppwhirrApplication::getIsAlreadyRunning() const
{
    return this->isAlreadyRunning;
}

void AppwhirrApplication::hideBrowserWindow()
{
    Q_FOREACH (QWidget *widget, this->topLevelWidgets()) {
        if (widget->isVisible()) {
            if(widget == this->browserWindow)
            {
                widget->hide();
            }
        }
    }
}

bool AppwhirrApplication::event(QEvent *event)
{
    if(event->type() == ForceHideOrMinimizeBrowserEvent::getCustomEventType())
    {
        this->hideBrowserWindow();

        event->accept();
        return true;
    }

    else if(event->type() == StartThisApplicationEvent::getCustomEventType())
    {
        this->processStartThisApplicationEvent(event);

        event->accept();
        return true;
    }

    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent* fileEvent = static_cast<QFileOpenEvent*>(event);
        if (!fileEvent->url().isEmpty()) {
            _urlSchemeHandlerDelegate->__schemeCalledWithURL(fileEvent->url().toString());
        }
    }

    return QApplication::event(event);
}


//void MyApplication::setBrowserWindow(BrowserWindow *browserWindow)
//{
//    this->browserWindow = browserWindow;
//}

void AppwhirrApplication::postEventToBrowserWindow(QEvent *event) {
    this->postEvent(this->getBrowserWindow(), event);
}

// slot
void AppwhirrApplication::showBrowserWindow()
{
//    this->postEvent(this, new ForceHideOrMinimizeEvent());

    // Shouldn't be initalized in main() to speed up startup time
    // Also the user may not need this window, so why should we use more resources
    BrowserWindow *theBrowserWindow = this->getBrowserWindow();

    theBrowserWindow->show();
    theBrowserWindow->activateWindow();

    theBrowserWindow->setFocus();
}

void AppwhirrApplication::showMyAppsPage()
{
    this->showBrowserWindow();
    this->getBrowserWindow()->showMyAppsPage();
}

void AppwhirrApplication::showStorePage()
{
    this->showBrowserWindow();
    this->getBrowserWindow()->showStorePage();
}

void AppwhirrApplication::showFeedbackPage()
{
    this->showBrowserWindow();
    this->getBrowserWindow()->showFeedbackPage();
}

void AppwhirrApplication::showMyAppsAutorunApps()
{
    this->showBrowserWindow();
    this->getBrowserWindow()->showMyAppsPageAutostartAppsOnly();
}


// -------------------------------------
// --- application start/run methods ---

void AppwhirrApplication::processStartThisApplicationEvent(QEvent *e) {

    StartThisApplicationEvent *castedEvent = dynamic_cast<StartThisApplicationEvent *>(e);
    if(castedEvent == NULL)
    {
        return;
    }

    QString startThisAppPath = castedEvent->getApplicationPath();

    if(startThisAppPath.isEmpty()) {
        qDebug("Cannot start this app, because the given path is empty");
        return;
    }

    if( castedEvent->getRelatedFilter().isEmpty() ) {
        qDebug("No filter provided with the starting application, so it won't be added");
    } else {
        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getCleverSortingDelegate()->addOrSetValueForKey(castedEvent->getRelatedFilter(), castedEvent->getApplicationTitle());
    }

    qDebug(startThisAppPath.toAscii());

//    // handle special (fake) url(s)
//    if(startThisAppPath == FAKE_BROWSER_ICON_URL) {
//        this->showBrowserWindow();
//        return;
//    }


    this->__runSingleApplication(castedEvent->getApplicationPath(), castedEvent->getApplicationWorkingDirectory(), castedEvent->getApplicationTitle());
}

void AppwhirrApplication::_runAppByAppId(QString appId)
{
    // get app details first
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getLocalAppDatabaseManager()->getAppDescriptionModelByAppId(appId, "AppwhirrApplication::_runAppByAppId");
}

void AppwhirrApplication::__runSingleApplication(QString executableAbsolutePath, QString executableAbsoluteWorkingDirectoryPath, QString appName)
{
    ApplicationRunner *runner = new ApplicationRunner(false);
    connect(runner, SIGNAL(failedWithApp(QString,QString,ApplicationRunner*)), this, SLOT(appStartFailed(QString,QString,ApplicationRunner*)));
    connect(runner, SIGNAL(successWithApp(QString,ApplicationRunner*)), this, SLOT(appStartedSuccessfully(QString,ApplicationRunner*)));
    runner->runSingleApplication(executableAbsolutePath, executableAbsoluteWorkingDirectoryPath, appName);
}




//SortingDatabaseManager *MyApplication::getSortingDatabaseManager()
//{
//    return this->sortingDatabaseManager;
//}

//LocalApplicationScanner *MyApplication::getLocalApplicationScanner()
//{
//    return this->_localApplicationScanner;
//}

AppwhirrApplication *AppwhirrApplication::instance()
{
    return dynamic_cast<AppwhirrApplication *>(QApplication::instance());
}

void AppwhirrApplication::showSystemTrayMessage(QString message, QString title)
{
    if(this->systemTrayIconDelegate != NULL) {
        this->systemTrayIconDelegate->showSystemTrayMessage(message, title);
    }
}

void AppwhirrApplication::applicationPathChanged(QString previousPath, QString currentPath) {
    // TODO: normal behaviour
//    QMessageBox::warning(NULL, tr("App moving warning"),
//                         tr(QString("AppWhirr moved from: " + previousPath + " to: " + currentPath + ".").toAscii())
//                         );

    this->PresentInformationMessageBoxOnTopOfCurrentActiveMainWindow(tr("AppWhirr program was moved."), tr("AppWhirr program was moved in your computer from: ") + previousPath + tr(" to: ") + currentPath);
}

void AppwhirrApplication::applicationStartedAtTheFirstTime() {
//    QMessageBox::warning(NULL,
//                             tr("Hello there"),
//                             tr("Application started at the first time!"));

    this->PresentInformationMessageBoxOnTopOfCurrentActiveMainWindow(tr("Hello there"), tr("Application started at the first time!"));
}


// ------------------------------------------------
// --- local app database handling result slots ---

void AppwhirrApplication::_result_getAppDescriptionModelByAppId(ApplicationDescriptionModel resultAppDescriptionModel, QString dbOperationId, bool isQuerySuccess)
{
    DLog("_result_getAppDescriptionModelByAppId");
    if(dbOperationId == "AppwhirrApplication::_runAppByAppId") {
        if(isQuerySuccess && ( (int)resultAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppState() >= (int)AppWhirr::ApplicationGrabStates::Installed )) {
            // run it
            QString executableAbsolutePath = PathHelper::combineAndCleanPathes(resultAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), resultAppDescriptionModel.getStaticDescription_ReadOnly()->getExecutablePath());
            QString absoluteWorkingDir = PathHelper::combineAndCleanPathes(resultAppDescriptionModel.getDynamicDescription_ReadOnly()->getInstalledRootPath(), resultAppDescriptionModel.getStaticDescription_ReadOnly()->getExecutableWorkingDirectory());
            this->__runSingleApplication(executableAbsolutePath, absoluteWorkingDir, resultAppDescriptionModel.getDynamicDescription_ReadOnly()->getAppName());
        }
        else {
            // application not found
            GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("The application cannot be run, because it's not installed."), 5000, NotificationManager::ErrorPriority);
        }
    }

    else {
        DLog("Unhandled dbOperationId: ") << dbOperationId;
    }
}

// ----------------------------------
// --- custom url scheme handling ---

void AppwhirrApplication::_openAppDetailPageUrlSchemeAction(QString appId)
{
    // have to test whether the user is logged in and AppWhirr is fully initialized for handling the url request
    //  this is important because AppWhirr can get url requests for registered custom schemes even if AppWhirr is not running
    //  in this case we have to postpone the action after client self update finishes and the user logged in

    if(ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->getIsUpdateChekingAndInitialLoginFinished()) {
        // update checking and user login finished - show the store page immediatelly
        this->getBrowserWindow()->_showStoreDetailPageForAppId(appId);
    }
    else {
        // update checking or user login is not finished, so store this action
        ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->addAfterLoginAction(new ShowAppDetailPageForAppIdAfterLoginAction(appId));
    }
}

void AppwhirrApplication::_runAppUrlSchemeAction(QString appId)
{
    // have to test whether the user is logged in and AppWhirr is fully initialized for handling the url request
    //  this is important because AppWhirr can get url requests for registered custom schemes even if AppWhirr is not running
    //  in this case we have to postpone the action after client self update finishes and the user logged in

    if(ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->getIsUpdateChekingAndInitialLoginFinished()) {
        // update checking and user login finished - show the store page immediatelly
        this->_runAppByAppId(appId);
    }
    else {
        // update checking or user login is not finished, so store this action
        ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->addAfterLoginAction(new RunAppAfterLoginAction(appId));
    }
}

void AppwhirrApplication::_searchInStoreByFilterText(QString searchFilterText)
{
    // have to test whether the user is logged in and AppWhirr is fully initialized for handling the url request
    //  this is important because AppWhirr can get url requests for registered custom schemes even if AppWhirr is not running
    //  in this case we have to postpone the action after client self update finishes and the user logged in

    if(ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->getIsUpdateChekingAndInitialLoginFinished()) {
        // update checking and user login finished - show the store page immediatelly
        this->getBrowserWindow()->_showSearchInStorePageAndSearchForFilterText(searchFilterText);
    }
    else {
        // update checking or user login is not finished, so store this action
        ApplicationGlobalStateRegistry::sharedApplicationGlobalStateRegistry()->addAfterLoginAction(new ShowSearchInStorePageAndSearchAfterLoginAction(searchFilterText));
    }
}


// -------------------------
// --- main local server ---

bool AppwhirrApplication::_createServer() {
    singleInstanceLocalServer = new QLocalServer(this);
    if (!singleInstanceLocalServer->listen(SINGLE_INSTANCE_LOCAL_SERVER_NAME)) {
        DLog("Failed to create server - try to remove it the previous one");
        QLocalServer::removeServer(SINGLE_INSTANCE_LOCAL_SERVER_NAME);

        // Try it again
        if(!singleInstanceLocalServer->listen(SINGLE_INSTANCE_LOCAL_SERVER_NAME)) {
            WLog("Failed for second time to listen to server-name: ") << SINGLE_INSTANCE_LOCAL_SERVER_NAME;
            return false;
        }
    }

    DLog("local server initialized");
    return true;
}

void AppwhirrApplication::_newLocalServerConnection()
{
    // send a ping message to the connected SLOT(s)
    // so they can quit because 1 instance of the application is already running
    QLocalSocket *clientConnection = singleInstanceLocalServer->nextPendingConnection();
    if(clientConnection != NULL) {
        connect(clientConnection, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(_localServerMessageFailedWithError(QLocalSocket::LocalSocketError)));
        connect(clientConnection, SIGNAL(readyRead()), this, SLOT(_localServerMessageReadyToRead()));
    }

//    clientConnection->write("(Already running)");
//    clientConnection->flush();
//    clientConnection->disconnectFromServer();

    // and also make this application reappear
    Q_FOREACH (QWidget *widget, this->topLevelWidgets()) {
        BrowserWindow *mainWindow = dynamic_cast<BrowserWindow *>(widget);
        if(mainWindow != NULL) {
            this->postEvent(mainWindow, new EnsureApplicationPresentedEvent());
            //mainWindow->ensureWindowPresented();
            this->getBrowserWindow()->ensureWindowPresented();
            //this->setActiveWindow(mainWindow);
        }
    }
}

void AppwhirrApplication::_localServerMessageFailedWithError(QLocalSocket::LocalSocketError err)
{
    QLocalSocket *senderLocalSocket = dynamic_cast<QLocalSocket *>(this->sender());
    if(senderLocalSocket == NULL) {
        WLog("Cannot cast the server to local socket.");
        return;
    }

    WLog("Local server - socket error: ") << err;

    connect(senderLocalSocket, SIGNAL(disconnected()), senderLocalSocket, SLOT(deleteLater()));
    senderLocalSocket->disconnectFromServer();
}

void AppwhirrApplication::_localServerMessageReadyToRead()
{
    QLocalSocket *senderLocalSocket = dynamic_cast<QLocalSocket *>(this->sender());
    if(senderLocalSocket == NULL) {
        WLog("Cannot cast the server to local socket.");
        return;
    }

    QByteArray messageBytes = senderLocalSocket->readAll();
    if(messageBytes.size() < 1) {
        WLog("Empty message.");
    }
    else {
        //
        // message found - parse it

        if(messageBytes[0] == (char)STRING_MESSAGE) {
            if(messageBytes.size() == 1) {
                WLog("Empty string message.");
            }
            else {
                QByteArray messageStringBytes = messageBytes;
                messageStringBytes.remove(0, 1);
                QString messageString(messageStringBytes);
                DLog("String message arrived: ") << messageString;

                if(messageString.startsWith(MESSAGE_TEXT_TYPE_ANOTHER_INSTANCE_PROGRAM_ARGS)) {
                    messageString.remove(0, QString(MESSAGE_TEXT_TYPE_ANOTHER_INSTANCE_PROGRAM_ARGS).length());
                    // another instance's args
                    QStringList parts = messageString.split("|", QString::SkipEmptyParts);
                    if(parts.isEmpty()) {
                        DLog("No arguments found.");
                    }
                    else {
                        this->_processProgramArguments(parts);
                    }
                }
                else {
                    WLog("Invalid string message type.");
                }
            }
        }
    }

    connect(senderLocalSocket, SIGNAL(disconnected()), senderLocalSocket, SLOT(deleteLater()));
    senderLocalSocket->disconnectFromServer();
}

// ----------------------------------
// --- program arguments handling ---

void AppwhirrApplication::__processGivenProgramArguments()
{
    this->_processProgramArguments(this->arguments());
}

void AppwhirrApplication::_processProgramArguments(QStringList givenArguments)
{
//    givenArguments
    DLog(" -- Given arg cnt: ") << givenArguments.size();
    for(int i = 0; i < givenArguments.size(); i++) {
        DLog(QString("Arg %1: %2").arg(i).arg(givenArguments[i]));

        if(givenArguments[i].startsWith(this->_urlSchemeHandlerDelegate->getSchemePrefix())) {
            DLog("Url scheme found");
            this->_urlSchemeHandlerDelegate->__schemeCalledWithURL(givenArguments[i]);
        }
    }
}

// -------------------------------------------------------
// --- communication with the already running instance ---

void AppwhirrApplication::_sendGivenProgramArgumentsToAlreadyRunningInstanceThenQuit()
{
    if(this->arguments().isEmpty()) {
        // nothing to send, quit
        this->quit();
        return;
    }

    DLog("Send given args then quit.");
    LocalMessageSender *localMessageSender = new LocalMessageSender(SINGLE_INSTANCE_LOCAL_SERVER_NAME);
    connect(localMessageSender, SIGNAL(finishedWithSuccess()), this, SLOT(_communicationWithAlreadyRunningInstanceSucceeded()));
    connect(localMessageSender, SIGNAL(failed(QString)), this, SLOT(_communicationWithAlreadyRunningInstanceFailedWithError(QString)));
    QByteArray messageBytes;
    messageBytes.append((char)STRING_MESSAGE);

    QString messageText(MESSAGE_TEXT_TYPE_ANOTHER_INSTANCE_PROGRAM_ARGS);
    for(int i = 0; i < this->arguments().size(); i++) {
        if(i>0) {
            messageText += "|" + this->arguments()[i];
        }
        else {
            // first arg
            messageText += this->arguments()[i];
        }
    }
    messageBytes.append(messageText);

    localMessageSender->sendSingleMessage(messageBytes);
}

void AppwhirrApplication::_communicationWithAlreadyRunningInstanceFailedWithError(QString errorMessage)
{
    WLog("Couldn't send message to the already running instance! Error message: ") << errorMessage;
//    this->sender()->deleteLater();
    this->exit(23);
}

void AppwhirrApplication::_communicationWithAlreadyRunningInstanceSucceeded()
{
    DLog("Message sent for the already running instance.");
//    this->sender()->deleteLater();
    this->exit(22);
}
