#include "appspecificpathhelper.h"

#include <QDir>
#include <QCoreApplication>
#include <QDesktopServices>

// utility
#include "../Common/Helpers/pathhelper.h"
#ifdef ENABLE_DYNAMIC_USER_RELATED_METHODS_IN_PATH_HELPER
#   include "./User/usermanager.h"
#endif


AppSpecificPathHelper::AppSpecificPathHelper()
{
}

// -------------------------------
// --- project specific pathes ---

QString AppSpecificPathHelper::getClientExecutableDirectory()
{
    QDir path(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MAC
    path.cdUp();
    path.cdUp();
    path.cdUp();
#endif

    return path.absolutePath();
}

QString AppSpecificPathHelper::getClientExecutablePath()
{
#ifdef Q_OS_WIN
    return QCoreApplication::applicationFilePath();
#endif

#ifdef Q_OS_MAC
    QDir executablePath(QCoreApplication::applicationDirPath());
    executablePath.cdUp();
    executablePath.cdUp();

    return executablePath.absolutePath();
#endif
}

QString AppSpecificPathHelper::getUpdaterExecutablePath()
{
    QDir updater(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MAC
    // Free from the bundle...
    updater.cdUp();
    updater.cdUp();
    updater.cdUp();
#endif
    return updater.absolutePath() + "/" + AppSpecificPathHelper::getUpdaterExecutableFileName();
}

QString AppSpecificPathHelper::getUpdaterExecutableFileName()
{
#ifdef Q_OS_WIN
    static QString updaterExeFileName("AppWhirrU");
#endif

#ifdef Q_OS_MAC
    static QString updaterExeFileName("AppWhirrU.app");
#endif

    return updaterExeFileName;
}

QString AppSpecificPathHelper::getClientPluginsDirectory()
{
    return PathHelper::combineAndCleanPathes(getClientExecutableDirectory(), "plugins");
}

QString AppSpecificPathHelper::getUninstallerExecutablePath()
{
#ifdef Q_OS_WIN
    QString s = PathHelper::combineAndCleanPathes(getUninstallerDirPath(), getUninstallerExecutableFileName());
#endif

#ifdef Q_OS_MAC
    QString s = getUninstallerDirPath();
#endif

    return s;
}

QString AppSpecificPathHelper::getUninstallerExecutableFileName()
{
#ifdef Q_OS_WIN
    return QString("AppWhirrUninstaller.exe");
#endif

#ifdef Q_OS_MAC
    return QString("AppWhirrUninstaller.app");
#endif
}

QString AppSpecificPathHelper::getUninstallerDirPath()
{
#ifdef Q_OS_WIN
    QString s = getClientExecutableDirectory();
#endif

#ifdef Q_OS_MAC
    QString s = PathHelper::combineAndCleanPathes(getClientExecutableDirectory(), getUninstallerExecutableFileName());
#endif

    return s;
}

QString AppSpecificPathHelper::_getClientRelativeRootDirectory()
{
#ifdef Q_OS_WIN
    static QString s = ".";
#endif

#ifdef Q_OS_MAC
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
    static QString s = PathHelper::combineAndCleanPathes(getLocalRootDirectory(), getLocalUsersDirectoryDirName());
    PathHelper::ensureDirectoryCreated(s);
    return s;
}

QString AppSpecificPathHelper::getLocalUsersDirectoryDirName()
{
    return QString("user");
}

#ifdef ENABLE_DYNAMIC_USER_RELATED_METHODS_IN_PATH_HELPER
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
#endif

QString AppSpecificPathHelper::getLocalAppWhirrTmpDirectory()
{
    static QString s = PathHelper::combineAndCleanPathes(getLocalRootDirectory(), "tmp");
    PathHelper::ensureDirectoryCreated(s);
    return s;
}

QString AppSpecificPathHelper::getSettingsDirectory()
{
    static QString s = PathHelper::combineAndCleanPathes(getLocalRootDirectory(), "settings");
    PathHelper::ensureDirectoryCreated(s);
    return s;
}

QString AppSpecificPathHelper::getLocalRootDirectory() {

#ifdef Q_OS_WIN
    QDir programFiles(PathHelper::getWinSpecProgramFilesPath());
    programFiles.cdUp();
#ifdef USE_DEBUG_LOCAL_FOLDER
    static QString s = PathHelper::combineAndCleanPathes(programFiles.canonicalPath(), "awdat-debug");
#else
    static QString s = PathHelper::combineAndCleanPathes(programFiles.canonicalPath(), "awdat");
#endif
#endif

#ifdef Q_OS_MAC
#ifdef USE_DEBUG_LOCAL_FOLDER
    static QString s = QDir::homePath() + "/Library/Application Support/AppWhirr-debug";
#else
    static QString s = QDir::homePath() + "/Library/Application Support/AppWhirr";
#endif
#endif

    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getCacheDirectory() {
    static QString s = PathHelper::combineAndCleanPathes(getLocalRootDirectory(), "cache");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getLocalLogContainerDirectoryPath() {
    static QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalRootDirectory(), "logs");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getAppWhirrTmpDirectoryInSystemTemp()
{
    QString s = PathHelper::combineAndCleanPathes(QDesktopServices::storageLocation(QDesktopServices::TempLocation), "AppWhirrTmp");
    PathHelper::ensureDirectoryCreated(s);
    return s;
}

QString AppSpecificPathHelper::getAppWhirrTmpDirectoryInSystemTempForUninstaller()
{
    QString s = PathHelper::combineAndCleanPathes(getAppWhirrTmpDirectoryInSystemTemp(), "uninst");
    PathHelper::ensureDirectoryCreated(s);
    return s;
}

QString AppSpecificPathHelper::getLocalFrameworkRootDirectory()
{
    static QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalRootDirectory(), "frameworks");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getLocalFrameworkCheckerDirectory()
{
    static QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalFrameworkRootDirectory(), "checkers");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getLocalFrameworkCheckResultDirectory()
{
    static QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalFrameworkRootDirectory(), "checkresults");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getLocalFrameworkInstallerDirectory()
{
    static QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalFrameworkRootDirectory(), "installers");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}


// ----------------------
// --- utility pathes ---

QString AppSpecificPathHelper::getTmp_Install_Uncompressed_DirectoryPath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalAppWhirrTmpDirectory(), "uncomp_app");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getTmp_Install_InstalledBackup_DirectoryPath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalAppWhirrTmpDirectory(), "inst_backup");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getTmp_UninstallBackupDirectoryPath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalAppWhirrTmpDirectory(), "uninst_backup");
    PathHelper::ensureDirectoryCreated(s);

    return s;
}

QString AppSpecificPathHelper::getTmp_ClientSelfUpdate_RootPath()
{
    QString s = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalAppWhirrTmpDirectory(), "ClientSelfUpdate");
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


// ---------------------------
// --- OS spec. pathes

#ifdef Q_OS_MAC
QString AppSpecificPathHelper::getMacSpecAppWhirrClientAliasFilePath()
{
    return PathHelper::combineAndCleanPathes( PathHelper::getMacSpecApplicationsPath(), "AppWhirr" );
}
#endif

#ifdef Q_OS_WIN
QString AppSpecificPathHelper::getWinAndWinUserSpecAppWhirrClientAutoStartupFilePath()
{
    return PathHelper::combineAndCleanPathes( PathHelper::getWinSpecStartupFolderPath(), "AppWhirr.lnk" );
}

QString AppSpecificPathHelper::getWinAndWinUserSpecAppWhirrClientDesktopShortcutFilePath()
{
    return PathHelper::combineAndCleanPathes( PathHelper::getLocalDesktopPath(), "AppWhirr.lnk" );
}

QString AppSpecificPathHelper::getWinAndWinUserSpecAppWhirrClientStartMenuShortcutFilePath()
{
    return PathHelper::combineAndCleanPathes( PathHelper::getWinSpecStartMenuProgramsPath(), "AppWhirr.lnk" );
}

#endif


// ----------------------------
// --- user specific pathes ---

#ifdef ENABLE_DYNAMIC_USER_RELATED_METHODS_IN_PATH_HELPER

QString AppSpecificPathHelper::getCurrentUserLocalDirectoryPath()
{
    UserManager *userManager = new UserManager;
    QString currUserId = userManager->getCurrentUserId();
    delete userManager;

    if(currUserId.isEmpty()) {
        return QString(); // empty string
    }

    return getLocalUserDirectoryPathForUserId(currUserId);
}

QString AppSpecificPathHelper::getCurrentUserLocalAppFolderPath()
{
    QString currUserLocalDirPath = getCurrentUserLocalDirectoryPath();
    if(currUserLocalDirPath.isEmpty()) {
        return QString(); // empty string
    }

    QString path = PathHelper::combineAndCleanPathes(currUserLocalDirPath, "app");

    if(PathHelper::ensureDirectoryCreated(path)) {
        return path;
    }

    return QString();
}

QString AppSpecificPathHelper::getCurrentUserLocalInstalledAppPathForAppId(QString appId)
{
    QString userLocalAppFolderPath = getCurrentUserLocalAppFolderPath();
    if(userLocalAppFolderPath.isEmpty()) {
        return QString();
    }

    QString path = PathHelper::combineAndCleanPathes(userLocalAppFolderPath, appId);
    PathHelper::ensureDirectoryCreated(path);

    return path;
}

//QString AppSpecificPathHelper::getCurrentUserLocalAppSettingsDirectoryPath()
//{
//    QString currUserLocalDirPath = getCurrentUserLocalDirectoryPath();
//    if(currUserLocalDirPath.isEmpty()) {
//        return QString(); // empty string
//    }

//    QString path = PathHelper::combineAndCleanPathes(currUserLocalDirPath, "LocalAppSettings");

//    if(PathHelper::ensureDirectoryCreated(path)) {
//        return path;
//    }

//    return QString();
//}

//QString AppSpecificPathHelper::getCurrentUserAppLocalSettingsFilePathByAppId(QString appId)
//{
//    QString localAppSettingsContainerDirectory = getCurrentUserLocalAppSettingsDirectoryPath();
//    if(localAppSettingsContainerDirectory.isEmpty()) {
//        return QString();
//    }

//    return PathHelper::combineAndCleanPathes(localAppSettingsContainerDirectory, QString("%1.sqlite").arg(appId));
//}
#endif
