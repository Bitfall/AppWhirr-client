#include <QtGui/QApplication>
#include "mainwindow.h"

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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef TRANSFER_LOG_TO_FILE
    // --- log to file

    QString logFilePath = QString("%1/AppWhirrUninst_log.txt").arg(QApplication::applicationDirPath());
    logfile.open(logFilePath.toUtf8(), ios::out);
    qInstallMsgHandler(SimpleLoggingHandler);
    DLogS << "------------------------------------------------------------";

    DLog("AppWhirrUninstaller started: ") << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz");

    // -----------------
#endif


    MainWindow w;
    w.show();
    
    a.setQuitOnLastWindowClosed(true);
    return a.exec();
}
