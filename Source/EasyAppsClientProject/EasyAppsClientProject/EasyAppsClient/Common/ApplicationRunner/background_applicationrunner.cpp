#include "background_applicationrunner.h"

#include <QFileInfo>
#include <QDir>
#include <QProcess>

#include "Common/Helpers/pathhelper.h"
#include "Common/Helpers/awqprocessextension.h"

Background_ApplicationRunner::Background_ApplicationRunner(QString absoluteExecutablePath, QString absoluteWorkingDirectory, QString appName, QStringList appArgs, QObject *parent) :
    QObject(parent),
    _absoluteExecutablePath(absoluteExecutablePath),
    _absoluteWorkingDirectory(absoluteWorkingDirectory),
    _appName(appName),
    _appArgs(appArgs)
{
}

void Background_ApplicationRunner::run()
{
    QFileInfo fileInfo(_absoluteExecutablePath);
    QString fileName = fileInfo.fileName();
    QString filePath = fileInfo.filePath();

    if( !fileInfo.exists() ) {
        Q_EMIT failedWithError(_appName, tr("Application not found."));
        return;
    }

    QString theWorkingDirectory = _absoluteWorkingDirectory;
    if(theWorkingDirectory.isEmpty()) {
        // if working dir is empty then fallback to the executable's directory
        theWorkingDirectory = fileInfo.dir().path();
    }


    //
    // app process args - these will be used to start the app
    QString appPath;
    QString appWorkingDir;
    QStringList appArgs;

    if(fileInfo.suffix().toLower() == "jar")
    {
        // it's a Java application
        appPath = "java";
        appWorkingDir = theWorkingDirectory;
        appArgs << "-jar" << _absoluteExecutablePath;
    }
    else
    {
        // normal app
#ifdef Q_WS_WIN
        appPath = PathHelper::quotePath(_absoluteExecutablePath);
        appWorkingDir = PathHelper::quotePath(theWorkingDirectory);
#endif

#ifdef Q_WS_MAC
        appPath = "open";
        appArgs << filePath;
#endif
    }


    // append the given app args
    if(!_appArgs.isEmpty()) {
#ifdef Q_OS_MAC
        // mac's 'open' command requires an additional '--args' parameter before the args for the actual app
        appArgs.append("--args");
#endif
        appArgs.append(_appArgs);
    }

#if 0
    //
    // start the app
    if(!QProcess::startDetached(appPath,
                                appArgs,
                                appWorkingDir)) // Workin Dir will be here
    {
        WLog("Application cannot be started: ") << _appName << appPath << appWorkingDir << appArgs;
//        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("Application start failed: ") + absoluteExecutablePath, 10000, NotificationManager::ErrorPriority);
//        return false;
        Q_EMIT failedWithError(_appName, tr("Application found but cannot run it."));
    }

    else
    {
//    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("Application started: ") + appName, 3000);
        Q_EMIT successWithApp(_appName);
    }
#endif

    DLog("final args (before execute): ") << appPath << appWorkingDir << appArgs;

    //
    // start the app
    if(!AWQProcessExtension::startDetachedAndAllowElevation(appPath,
                                                            appArgs,
                                                            appWorkingDir)) // Workin Dir will be here
    {
        WLog("Application cannot be started: ") << _appName << appPath << appWorkingDir << appArgs;
//        GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("Application start failed: ") + absoluteExecutablePath, 10000, NotificationManager::ErrorPriority);
//        return false;
        Q_EMIT failedWithError(_appName, tr("Application found but cannot run it."));
    }

    else
    {
//    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getNotificationManager()->addSimpleTextNotificationWithTimeout(tr("Application started: ") + appName, 3000);
        DLog("App started successfully: ") << appPath;
        Q_EMIT successWithApp(_appName);
    }
}
