#include "appwhirrupdaterapplication.h"

#include <QString>
#include <QMessageBox>
#include <QDir>

#include "../EasyAppsClient/Common/Helpers/pathhelper.h"
#include "../EasyAppsClient/Application/appspecificpathhelper.h"


#define TRANSFER_LOG_TO_FILE 1

#ifdef TRANSFER_LOG_TO_FILE
// -- log to file

#include <fstream>
#include <QTime>

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

enum AppWhirrUpdaterArgumentErrorCodes {
    NoModeParameter = 3,
    InvalidModeParameter = 4,

    AppInstallUpdateModeNotEnoughParameter = 11,

    ClientSelfUpdateModeNotEnoughParameter = 21,

    AppWhirrRestartModeNotEnoughParameter = 31
};

int main(int argc, char *argv[])
{
    if (argc < 2) return NoModeParameter;

    AppWhirrUpdaterApplication application(argc, argv);
    QApplication::setQuitOnLastWindowClosed(true);

#ifdef TRANSFER_LOG_TO_FILE
    // --- log to file

    QString logFilePath = QString("%1/AppWhirrU_log.txt").arg(AppSpecificPathHelper::getLocalLogContainerDirectoryPath());
    logfile.open(logFilePath.toUtf8(), ios::out);
    qInstallMsgHandler(SimpleLoggingHandler);
    DLogS << "------------------------------------------------------------";


    // -----------------
#endif

    DLog("AppWhirrU started: ") << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz");

    // add plugins directory to qt search path
    QCoreApplication::addLibraryPath(AppSpecificPathHelper::getClientPluginsDirectory());
    DLog("Plugin dir: ") << AppSpecificPathHelper::getClientPluginsDirectory();
    // and log every "library" paths
    QStringList paths = QCoreApplication::libraryPaths();
    for (QStringList::iterator it = paths.begin(); it!=paths.end(); it++)
    {
       DLogS << "Looking for plugins at path: " << (*it);
    }

    QString updaterMode = QString(argv[1]);
    if(updaterMode == "1") {
        // app install or update mode
        if(argc != 4) {
            // required params: updater-start-path, mode, zip-path, appId
            return AppInstallUpdateModeNotEnoughParameter;
        }

        QString zipPath = QString(argv[2]);
        QString appId = QString(argv[3]);

        application.startAppInstallOrUpdate(zipPath, appId);
    }
    else if(updaterMode == "2") {
        // client self-update mode
        if(argc < 3) { // later modify this to ' != 4', but to maintain compatibility the 4th parameter will be only stored now, and won't be used
            // required params: updater-start-path, mode, client's root path
            return ClientSelfUpdateModeNotEnoughParameter;
        }

        QString clientRootPath = QString(argv[2]);
        QString prevLocalDirPath; // the 'appwhirr local directory path' used by the previous AppWhirr version
        if(argc >= 4) {
            prevLocalDirPath = QString(argv[3]); // to maintain compatibility the parameter will be only stored now, and won't be used
        }
        DLog("Given 'prev local dir path': ") << prevLocalDirPath;

        application.startClientSelfUpdate(clientRootPath);
    }
    else if(updaterMode == "3") {
        if(argc != 4) {
            WLog("No enough parameter to restart appwhirr!");
            return AppWhirrRestartModeNotEnoughParameter;
        }
        QString appWhirrExecutablePath = QString(argv[2]);
        QString appWhirrExecutableWorkingDirPath = QString(argv[3]);
        application.startAppWhirrRestart(appWhirrExecutablePath, appWhirrExecutableWorkingDirPath);
    }
    else {
        return InvalidModeParameter;
    }

    // debug info "printing"
//    QMessageBox::information(NULL, "info", QDir::currentPath());


//    QString zipPath = QString("IcoFX.zip");

    return application.exec();
}
