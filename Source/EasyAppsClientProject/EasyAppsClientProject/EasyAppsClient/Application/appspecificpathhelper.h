#ifndef APPSPECIFICPATHHELPER_H
#define APPSPECIFICPATHHELPER_H

#include <QString>

class AppSpecificPathHelper
{
private:
    AppSpecificPathHelper();

public:
    // -------------------------------
    // --- project specific pathes ---

    // main executable pather
    static QString getClientExecutableDirectory();
    static QString getClientExecutablePath();
    static QString getUpdaterExecutablePath();
    static QString getUpdaterExecutableFileName();
    static QString getClientPluginsDirectory();
    static QString getUninstallerExecutablePath();
    static QString getUninstallerExecutableFileName();
    static QString getUninstallerDirPath();

    // global (for all (sub)projects) and for all users
    static QString _getClientRelativeRootDirectory();
//    static QString getRootInstalledAppDirectory();
    static QString getLocalUsersDirectoryPath();
    static QString getLocalUsersDirectoryDirName();
#ifdef ENABLE_DYNAMIC_USER_RELATED_METHODS_IN_PATH_HELPER
    static QString getLocalUserDirectoryPathForUserId(QString userId);
#endif
    static QString getLocalAppWhirrTmpDirectory();
    static QString getSettingsDirectory();
    static QString getLocalRootDirectory();
    static QString getCacheDirectory();
    static QString getLocalLogContainerDirectoryPath();

    static QString getAppWhirrTmpDirectoryInSystemTemp();
    static QString getAppWhirrTmpDirectoryInSystemTempForUninstaller();

    static QString getLocalFrameworkRootDirectory();
    static QString getLocalFrameworkCheckerDirectory();
    static QString getLocalFrameworkCheckResultDirectory();
    static QString getLocalFrameworkInstallerDirectory();

    // ------------------------
    // --- temporary pathes ---

    static QString getTmp_Install_Uncompressed_DirectoryPath();
    static QString getTmp_Install_InstalledBackup_DirectoryPath();
    static QString getTmp_UninstallBackupDirectoryPath();

    static QString getTmp_ClientSelfUpdate_RootPath();
    static QString getTmp_ClientSelfUpdate_ZipPath();
    static QString getTmp_ClientSelfUpdate_UncompressedNewVersionPath();
    static QString getTmp_ClientSelfUpdate_UncompressedTmpCopyWillPerformSelfUpdatePath();
    static QString getTmp_ClientSelfUpdate_OldVersionBackupFolder();

    // ---------------------------
    // --- OS spec. pathes
#ifdef Q_OS_MAC
    static QString getMacSpecAppWhirrClientAliasFilePath();
#endif
#ifdef Q_OS_WIN
    // get's the user's Startup folder (in StartMenu) under Windows
    // see the platform specific notes in the method
    // currently it supports XP+/2000+ versions of Windows
    static QString getWinAndWinUserSpecAppWhirrClientAutoStartupFilePath();
    static QString getWinAndWinUserSpecAppWhirrClientDesktopShortcutFilePath();
    static QString getWinAndWinUserSpecAppWhirrClientStartMenuShortcutFilePath();
#endif

    // ----------------------------
    // --- user specific pathes ---
#ifdef ENABLE_DYNAMIC_USER_RELATED_METHODS_IN_PATH_HELPER

    //
    // returns the path, or an empty string if the user is not selected or the directory does not exists and cannot be created
    static QString getCurrentUserLocalDirectoryPath();
    //
    // returns the path, or an empty string if the user is not selected or the directory does not exists and cannot be created
    static QString getCurrentUserLocalAppFolderPath();
    //
    // will return an empty string if no current user available
    static QString getCurrentUserLocalInstalledAppPathForAppId(QString appId);
    //
    // return the dir path contains the current user's apps' related settings files, or an empty string if the user is not selected or the directory does not exists and cannot be created
//    static QString getCurrentUserLocalAppSettingsDirectoryPath();
//    static QString getCurrentUserAppLocalSettingsFilePathByAppId(QString appId);
#endif
};

#endif // APPSPECIFICPATHHELPER_H
