#include "autostartupdelegate.h"

#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <QDir>

// utility
#include "Common/Helpers/pathhelper.h"
#include "Application/appspecificpathhelper.h"
#include "Common/DesktopShortcutCreator/desktopshortcutcreator.h"

AutoStartupDelegate::AutoStartupDelegate(QObject *parent) :
    QObject(parent)
{
}

#ifdef Q_WS_WIN
bool AutoStartupDelegate::willStartAtSystemStartup(QString& applicationPath) {
    LOG_AS_NOT_IMPLEMENTED;
    return false;
}
#endif

#ifdef Q_WS_WIN
bool AutoStartupDelegate::makeItAutoStartAtSystemStartup() {

    if(!DesktopShortcutCreator::createShortcut(AppSpecificPathHelper::getWinAndWinUserSpecAppWhirrClientAutoStartupFilePath(), AppSpecificPathHelper::getClientExecutablePath(), QString(), "Start AppWhirr client", true))
    {
        WLog("Cannot create the desktop-shortcut to path: ") << AppSpecificPathHelper::getWinAndWinUserSpecAppWhirrClientAutoStartupFilePath();
        return false;
    } else {
//        DLog("SUccess!!");
        return true;
    }

//    bool isSucceeded = true;

//    // Get the file path
//    QString qAppPath = QCoreApplication::applicationFilePath();

//    // Check if we have already in the autostarup list
//    if (willStartAtSystemStartup(qAppPath)) {
//        return true;
//    }

//    QString easyAppsWindowStartupFilePath = this->getEasyAppsStartupFile_WinAndUserSpecific();
//    DLogS << "startup file path: " << easyAppsWindowStartupFilePath;

//    if(!easyAppsWindowStartupFilePath.isEmpty())
//    {
//        qDebug(easyAppsWindowStartupFilePath.toAscii());

//        QFile f(easyAppsWindowStartupFilePath);
//        if (!f.open(QFile::WriteOnly | QIODevice::Text)) {
//            qDebug("FAILED to open the startup file");
//            return false;
//        }

//        QFileInfo fi(QApplication::applicationFilePath());

//        f.write("@ECHO OFF\n");
//        // the proper syntax to "start app without waiting to finish" with working dir:
//        //  example: start /d "E:\My Dropbox\EasyApps\Releases\V1\PreAlpha" EasyPortableApps_AppLauncher.exe
//        QString startCommand = QString("start /d %1 %2").arg(
//                PathManager::quotePath(QDir::toNativeSeparators(QApplication::applicationDirPath())),
//                fi.fileName());
//        f.write(startCommand.toAscii());
//        f.close();

//        qDebug(startCommand.toAscii());
//    } else {
//        qDebug("Cannot get the startup-folder. Sorry.");
//    }

//    return isSucceeded;
}
#endif

#ifdef Q_WS_WIN
bool AutoStartupDelegate::removeItFromAutoStartList() {
    bool isSucceeded = true;

    QString appwhirrWindowStartupFilePath = AppSpecificPathHelper::getWinAndWinUserSpecAppWhirrClientAutoStartupFilePath();
    if( QFile::exists(appwhirrWindowStartupFilePath) ) {
        if( QFile(appwhirrWindowStartupFilePath).remove() ) {
            DLog("Auto-startup file removed");
        }
        else {
            WLog("Auto-startup file found but cannot be removed");
            isSucceeded = false;
        }
    }

    return isSucceeded;
}
#endif
