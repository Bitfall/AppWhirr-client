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

    // global (for all (sub)projects) and for all users
    static QString _getClientRelativeRootDirectory();
//    static QString getRootInstalledAppDirectory();
    static QString getLocalUsersDirectoryPath();
    static QString getLocalUserDirectoryPathForUserId(QString userId);
    static QString getTmpDirectory();
    static QString getSettingsDirectory();
    static QString getLocalRootDirectory();
    static QString getCacheDirectory();
    static QString getLocalLogContainerDirectoryPath();

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


};

#endif // APPSPECIFICPATHHELPER_H
