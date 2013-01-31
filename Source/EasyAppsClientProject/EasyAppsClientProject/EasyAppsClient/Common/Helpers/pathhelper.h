#ifndef PATHHELPER_H
#define PATHHELPER_H

#include <QString>
#include <QFile>
#include <QList>
#include <QFileInfoList>
#include <QDir>

class PlatformspecificRunningExecutableDataInterface;

//
// the get..DirectoryPath() methods have to create the directory if it does not exists - call ensureDirectoryCreated(). If it cannot create the directory it has to return "failed" (usually empty string)
class PathHelper
{
private:
    PathHelper();

public:
    enum SymlinkHandlingModeEnum {
        SHM_Ignore = 0,
        SHM_UseAsFile = 1/*,
        SHM_UseAsTarget = 2*/
    };

public:    

    // -----------------------------------------------
    // --- system specific directories and pathers ---

    static QString getLocalDesktopPath();

#ifdef Q_OS_MAC
    static QString getMacSpecApplicationsPath();
#endif

#ifdef Q_OS_WIN
    static QString getWinSpecStartupFolderPath();
    static QString getWinSpecStartMenuPath();
    static QString getWinSpecStartMenuProgramsPath();
    static QString getWinSpecProgramFilesPath();
#endif


    // -----------------------------------------------------------------
    // --- utility functions to easy the work with Qt dirs and files ---
    //
    //  [!] these will skip SymLink files. The only exception is delete - this will delete the symlink file as well, but won't delete the files at the symlink's target (if that's a directory) - required by security


    /*! Removes / deletes the given \a topDirPath directory and every content of it

        Qt doesn't provide a simple method to remove directories which are not empty. This method makes it happen.
        @note It's a recursive function
        @note Symlinks will be handled as files - only the symlink files will be deleted, not the targets

        @param isFailWhenContentDeleteFails : if true (default) it will fail right after it cannot remove a subdirectory or a file. If false it will try to remove every file and subdirectory but won't fail only if at the end the given \a topDirPath directory cannot be removed.
    */
    static bool deleteThisDirectoryAndEverythingBelow(QString topDirPath, bool isFailWhenContentDeleteFails = true);

    /*! Copy the given \a sourceDirPath directory and it's content to \a destDirPath directory.

        Qt doesn't provide a simple method to copy whole directories. This method makes it happen.
        @note It's a recursive function
        @note Currently it doesn't handle SHM_UseAsTarget and will simply ignore shortcuts with this mode. SHM_UseAsFile is supported and implemented.
        @note The given \a destDirPath directory will be created if it doesn't exists

        @param isFailWhenContentCopyFails : if true (default) then it will return false at the first content copy error. If false then it will move forward and skip the problematic file. In both cases it will return false if the sourceDir does not exists.
    */
    static bool copyWholeDirectory(QString sourceDirPath, QString destDirPath, bool isCopyOnlyContentOfSource, SymlinkHandlingModeEnum symlinkHandlingMode, bool isFailWhenContentCopyFails = true);

    /*! It's a recursive extension of QDir's entryInfoList. It walks through the \a topDirPath directory and it's sub-directories and collects the QDir's entryInfoList.

        @note Because it's recursive nature and because of performance reasons the specified \a sortFlags is only applied to directory levels and not to the whole list.

        @param symlinkHandlingMode : if mode is 'ignore' then symlinks won't be added to the returned list and won't be processed at all. If mode is 'use as file' symlinks will be added to the returned list but won't be processed and the algorithm won't search in it if it's a symlink for a directory.
        @param isAddDirectoriesAsWell : if true directories will be added to the list as well. If false directories will be used only for walk-through. If true a directory will be added to the list \b before it's content.
    */
    static QFileInfoList getEntryInfoListRecursive(QString topDirPath, SymlinkHandlingModeEnum symlinkHandlingMode, bool isAddDirectoriesAsWell, const QStringList &nameFilters, QDir::Filters filters = QDir::NoFilter, QDir::SortFlags sortFlags = QDir::NoSort);

    //
    // this will convert the given path to the current platform's format
    //  on Windows it will replace / with \ and on Mac it will replace \ with / character
    static QString makePathPlatformDependent(QString path);

    //
    // combines the 2 path and returns the normalized combination of them
    static QString combineAndCleanPathes(QString partOne, QString partTwo);

    //
    // normalizes the path (replaces '\'s with '/'s, and cleans the path - based on QDir::cleanPath - resolves system-links, ., .., etc.
    static QString cleanThisPath(QString s);

    static bool renameFileOrFolder(QFileInfo fileOrFolderToRenameFileInfo, QString newNameOfTheFileOrFolder);

    //
    // QDir .exists returns true even if the specified path is not a directory but a file
    //  this function will return true only if the given path is really a file
    static bool isFileExistsAndItsReallyAFile(QString filePath);

    /*! Enhanced/alternative version of Qt's QFileInfo .isExecutable

        It also checks whether the file at the given \a filePath exists and checks for '.app' extension on Mac. With the Qt default methods the '.app' is a simple folder. This method will return true for a '.app' folder if it exists.

        @note On Mac currently this method will return true only for existing '.app' bundle folders and won't return true for unix executable files.
    */
    static bool isFileExecutable(QString filePath);

    //
    // Converts the input path to a safe path. E.g. it adds " -s around the path to allow spaces in it
    //  for programs can't do it automatically (e.g. command line - QProcess)
    // [!] But be aware that Qt may can't handle this safe-path.
    //  And if you use QProcess then it will add the quotes to arguments automatically,
    //      but not to the program's name!
    static QString quotePath(QString originalPath);

    //
    // Sets the file permission of the given basePath
    // Can be recursively - to this basePath have to be a directory and set recursively to true
    // Also it can add or modify (set) the permission
    static bool setPermissionForFilesOfDirectory(const QString& baseDirPath, QFile::Permissions permission, bool addPermission, bool recursively);
    static bool setPermissionForFile(QString targetFilePath, QFile::Permissions permission, bool addPermission);

    /*! Actually this is a tricky method: will scan the given baseDirPath recursively and will compare the file-matches in the destDirPath (and recursively in it's sub directories)

        If it finds a match then it will test whether it's a running application executable which cannot be overwritten -> it will return false if it finds at least 1 executable which runs somewhere under the baseDirPath and also found under the destDirPath (at the same relative location)
        @note It will return platform-specific application infos in it's outPlatformSpecificRunningAppInfos parameter which will contain the found running executables and the related platform specific data

        @note IMPORTANT NOTE: the result outPlatformSpecificRunningAppInfos will contain dynamically allocated items -> it's your responsibility to free them up!!!

        @note: REMOVED: it's not safe to check and perform an action separately, you should rather try to perform the action and in case it fails you handle it.
    */
//    static bool checkDirectoryCanBeSaflyCopiedToDirectory(const QString& baseDirPath, const QString& destDirPath, QList<PlatformspecificRunningExecutableDataInterface *> &outPlatformSpecificRunningAppInfos);

    /*! It's a recursive search function. It's optimized for batch-run-check so you should always use this method instead of using the single-file-check method \a isApplicationRunningAtPath() and call it recursively - that will be much slower than the check with this method!

        @note [!!!] IMPORTANT NOTE: the result outPlatformSpecificRunningAppInfos will contain dynamically allocated items -> it's your responsibility to free them up!!!
    */
    static bool checkForRunningExecutableUnderThisDirector(QString rootDirPath, QList<PlatformspecificRunningExecutableDataInterface *> &outPlatformSpecificRunningAppInfos);


    /*! Creates the given path \a s and every directory of it.

        @return True if the path already exists or if it could create the path. False if it cannot create the path and it doesn't exists as well.
    */
    static bool ensureDirectoryCreated(QString s);


    //
    // tries to copy a file from resource to the specified path
    //  if isSetCopiedFilePermissionToReadWrite is true, then it will try to set the new, copied file's permission to read-write
    //      if the permission-setting is fails, then it will delete the intermediate (read-only) file and returns false
    //  if isSetCopiedFilePermissionToReadWrite is false, then it just tries to copy the resource to the specified path
    //  in both cases it will return false if the copying cannot be performed
    //      (if isSetCopiedFilePermissionToReadWrite is true than it will set read-write to User and Owner)
    static bool copyFileFromResource(QString fileIdentifierInResource, QString targetFilePath, bool isSetCopiedFilePermissionToReadWrite = true);

#ifdef Q_OS_MAC
    static QString getBundleIdentifier(QString path);
    static QString getSymlinkTarget(QString symlink, int buffSize = 1024);
#endif

    static bool isSymlink(QString path);

    /*! Logs the last OS specific file-system handling error.

        @note It's only a helper/debugger method and it's log-entry can be used only for investigation. The last error code can be generated by anything.
        @note On Windows it uses GetLastError() ( http://msdn.microsoft.com/en-us/library/windows/desktop/ms679360(v=vs.85).aspx ) which is thread-dependent: "The return value is the calling thread's last-error code."
        @note On Mac it's not yet implemented.
    */
    static void logLastOSSpecificFileSystemHandlingError();

private:
    /*! Tests whether the given applicationPath is a running executable.

        It will append the related platform-specific running app-info to the given outPlatformSpecificRunningAppInfos if it's a running executable

        @note It's a slow procedure. If you want to test only 1 file you can call this method but if you have to check a whole directory then you should always call the \a checkForRunningExecutableUnderThisDirector() method which is optimized for batch-check. It's especially important on Windows.

        @note IMPORTANT NOTE: the result outPlatformSpecificRunningAppInfos will contain dynamically allocated items -> it's your responsibility to free up the items in the list \a outPlatformSpecificRunningAppInfos!

        @return True if it's a running executable and false if it's not.\n
    */
    static bool isApplicationRunningAtPath(const QString& applicationPath, QList<PlatformspecificRunningExecutableDataInterface *> &outPlatformSpecificRunningAppInfos);

    //
    // If isFailWhenContentCopyFails is true then it will return false at the first content copy error
    //  if it's false then it will move forward and skip the problematic file
    //  in both cases it will return false if the sourceDir does not exists
    static bool copyWholeDirectory_recursive(QString sourceDirPath, QString destDirPath, SymlinkHandlingModeEnum symlinkHandlingMode, bool isFailWhenContentCopyFails);

    /*! Copies every file of the given directory - copies symlink files as well if \a symlinkHandlinghMode does not prevent it.

        If symlink handling is not set to ignore you have to specify \a isIgnoreDirectorySymlinks param. If you set \a isIgnoreDirectorySymlinks to true then it will skip symlinks pointing to directories. Use it if you handle symlinks-pointing-to-dirs with other methods.

        @note This method will copy files of the directory + every kind of symlink (pointing to System, Hidden, Drive, ... targets). It won't resolve the found symlinks, it will only copy the symlink file, not it's target.
    */
    static bool copyOnlyFilesOfDirectory(QString sourceDirPath, QString destDirPath, SymlinkHandlingModeEnum symlinkHandlingMode, bool isIgnoreDirectorySymlinks);

//    static bool _copySymlink()
};

#endif // PATHHELPER_H
