#include "appspecificpathhelper.h"

#include <QDir>
#include <QCoreApplication>
#include <QDesktopServices>

// utility
#include "../Common/Helpers/pathhelper.h"


AppSpecificPathHelper::AppSpecificPathHelper()
{
}

// -------------------------------
// --- project specific pathes ---

QString AppSpecificPathHelper::getClientExecutableDirectory()
{
    QDir path(QCoreApplication::applicationDirPath());
#ifdef Q_WS_MAC
    path.cdUp();
    path.cdUp();
#endif

    return path.absolutePath();
}

QString AppSpecificPathHelper::getClientExecutablePath()
{
    return QCoreApplication::applicationFilePath();
}

QString AppSpecificPathHelper::getUpdaterExecutablePath()
{
    QDir updater(QCoreApplication::applicationDirPath());
#ifdef Q_WS_MAC
    // Free from the bundle...
    updater.cdUp();
    updater.cdUp();
    updater.cdUp();
#endif
    return updater.absolutePath() + "/" + AppSpecificPathHelper::getUpdaterExecutableFileName();
}

QString AppSpecificPathHelper::getUpdaterExecutableFileName()
{
#ifdef Q_WS_WIN
    static QString updaterExeFileName("AppWhirrU");
#endif

#ifdef Q_WS_MAC
    static QString updaterExeFileName("AppWhirrU.app");
#endif

    return updaterExeFileName;
}

QString AppSpecificPathHelper::getClientPluginsDirectory()
{
    return PathHelper::combineAndCleanPathes(getClientExecutableDirectory(), "plugins");
}

QString AppSpecificPathHelper::_getClientRelativeRootDirectory()
{
#ifdef Q_WS_WIN
    static QString s = ".";
#endif

#ifdef Q_WS_MAC
    // Under development
    // static QString s = "./../../..";

    // Release version
    static QString s = ".";
#endif

#ifdef Q_OS_LINUX
    static QString s = ".";
#endif

    return s;
}

//QString AppSpecificPathHelper::getRootInstalledAppDirectory()
//{
//    static QString s = getLocalRootDirectory() + "/apps";
//    ensureDirectoryCreated(s);
//    return s;
//}

QString AppSpecificPathHelper::getLocalUsersDirectoryPath()
{
    static QString s = getLocalRootDirectory() + "/users";
    PathHelper::ensureDirectoryCreated(s);
    return s;
}

QString AppSpecificPathHelper::getLocalUserDirectoryPathForUserId(QString userId)
{
    if(userId.isEmpty()) {
        return QString(); // empty string
    }

    QString path = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalUsersDirectoryPath(), userId);

    if(PathHelper::ensureDirectoryCreated(path)) {
        return path;
    }

    return QString();
}

QString AppSpecificPathHelper::getTmpDirectory()
{
    static QString s = getLocalRootDirectory() + "/tmp";
    PathHelper::ensureDirectoryCreated(s);
    return s;
}

QString AppSpecificPathHelper::getSettingsDirectory()
{
    static QString s = getLocalRootDirectory() + "/Settings";
    PathHelper::ensureDirectoryCreated(s);
    return s;
}

QString AppSpecificPathHelper::getLocalRootDirectory() {

    QString s = "./";

//    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getCacheDirectory() {
#ifdef Q_WS_WIN
    static QString s = getLocalRootDirectory() + "/Cache";
#endif

#ifdef Q_WS_MAC
    static QString s = getLocalRootDirectory() + "/Cache";
#endif

    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getLocalLogContainerDirectoryPath() {
    static QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalRootDirectory(), "Logs");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}


// ----------------------
// --- utility pathes ---

QString AppSpecificPathHelper::getTmp_Install_Uncompressed_DirectoryPath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmpDirectory(), "UncompressedApplication");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getTmp_Install_InstalledBackup_DirectoryPath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmpDirectory(), "InstalledTmpBackup");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getTmp_UninstallBackupDirectoryPath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmpDirectory(), "UninstallTmpBackup");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getTmp_ClientSelfUpdate_RootPath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmpDirectory(), "ClientSelfUpdate");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getTmp_ClientSelfUpdate_ZipPath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmp_ClientSelfUpdate_RootPath(), "zip");
    PathHelper::ensureDirectoryCreated(s);

    return PathHelper::combineAndCleanPathes(s, "AppWhirr.zip");
}

QString AppSpecificPathHelper::getTmp_ClientSelfUpdate_UncompressedNewVersionPath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmp_ClientSelfUpdate_RootPath(), "uncompressed");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getTmp_ClientSelfUpdate_UncompressedTmpCopyWillPerformSelfUpdatePath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmp_ClientSelfUpdate_RootPath(), "tmp_installperformer_copy");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getTmp_ClientSelfUpdate_OldVersionBackupFolder()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmp_ClientSelfUpdate_RootPath(), "oldversion_backup");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}
