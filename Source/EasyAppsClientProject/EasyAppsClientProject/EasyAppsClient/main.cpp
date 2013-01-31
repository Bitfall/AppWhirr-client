#include <QObject>

#include "Application/appwhirrapplication.h"
#include "Settings/settings.h"

#include <QList>
#include <QStringList>
#include "Common/Communication/basenetworkentity.h"

// utility
#include "Common/Helpers/pathhelper.h"
#include "Application/appspecificpathhelper.h"
#include "Settings/configmanager.h"
#include <QTime>


#ifdef TRANSFER_LOG_TO_FILE
// -- log to file

#include <fstream>

using namespace std;
ofstream logfile;

void SimpleLoggingHandler(QtMsgType type, const char *msg) {
    switch (type) {
        case QtDebugMsg:
            logfile << msg << "\n";
            break;
        case QtCriticalMsg:
            logfile << QTime::currentTime().toString().toAscii().data() << " Critical: " << msg << "\n";
            break;
        case QtWarningMsg:
            logfile << QTime::currentTime().toString().toAscii().data() << " Warning: " << msg << "\n";
            break;
        case QtFatalMsg:
            logfile << QTime::currentTime().toString().toAscii().data() <<  " Fatal: " << msg << "\n";
            abort();
        }
    }

// ------------------
#endif


int main(int argc, char *argv[]) {  


    int appExecReturnCode = 0;

//    do
//    {
        AppwhirrApplication app(argc, argv);


        //
        // install a default log handler to log initialization
        //  it will be replaced later by the AppwhirrApplication with a more sophisticated one this will only handle logging during initialization
#ifdef TRANSFER_LOG_TO_FILE
        // --- log to file

        QString logFilePath = QString("%1/AppWhirr_log-init.txt").arg(AppSpecificPathHelper::getLocalLogContainerDirectoryPath());
        logfile.open(logFilePath.toUtf8(), ios::out);
        qInstallMsgHandler(SimpleLoggingHandler);

        DLog("--------------------------------------------------------------");


        // -----------------
#endif

        DLog("AppWhirr started: ") << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz");

        // add plugins directory to qt search path
        QCoreApplication::addLibraryPath(AppSpecificPathHelper::getClientPluginsDirectory());
        DLog("Plugin dir: ") << AppSpecificPathHelper::getClientPluginsDirectory();
        // and log every "library" paths
        QStringList paths = QCoreApplication::libraryPaths();
        for (QStringList::iterator it = paths.begin(); it!=paths.end(); it++)
        {
            DLogS << "Looking for plugins at path: " << (*it);
        }

        DLog("AppWhirr client version: ") << ConfigManager::getClientVersion();



        //    app.checkForRunningInstanceAndInitialize();

        //    if(app.getIsAlreadyRunning()) {
        //        DLog("It's just another instance, a previous one is already running.");
        //        // the app will send some infos to the running instance then this instance will shut down
        ////        app.__sendProgramArgumentsToAlreadyRunningInstanceThenQuit(argc, argv);
        ////        return 22;
        //    }
        //    else {

        //        // We don't want to quit when window closes
        //        QApplication::setQuitOnLastWindowClosed(false);

        //        // Register complex meta types
        //        qRegisterMetaType<AW::Common::Communication::BaseNetworkEntity*>("AW::Common::Communication::BaseNetworkEntity*");
        //        qRegisterMetaType< QList<AW::Common::Communication::BaseNetworkEntity*> >("QList<AW::Common::Communication::BaseNetworkEntity*>");

        //        // Show application
        //        app.showBrowserWindow();

        //#if 0
        //        CasualLauncherWindow launcherWindow;
        //        //    CompactLauncherWindow launcherWindow;
        //        launcherWindow.getAndProcessLocalApplicationsFromDatabase();

        //        app.setLauncherWindow(&launcherWindow);

        //        if (Settings::sharedSettings()->getIsShowLauncherAtStartup()) {
        //            launcherWindow.show();
        //        } else {
        //            app.showSystemTrayMessage(QObject::tr("EasyApps is started."));
        //        }
        //#endif
        //        // process incoming arguments as well
        //        app.__processGivenProgramArguments();
        //    }

        app.setQuitOnLastWindowClosed(false);

        appExecReturnCode = app.exec();
//    }
//    while( appExecReturnCode == ConfigManager::getRestartAppWhirrClient_returnCode() );

//        appExecReturnCode = ConfigManager::getRestartAppWhirrClient_returnCode();

    return appExecReturnCode;
}
