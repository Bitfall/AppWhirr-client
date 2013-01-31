#include "uncompressor.h"

#include <QDir>
#include <QFile>

#include <quazip.h>
#include <quazipfile.h>
#include "gojobzip.h"

// debug and utility
#include "../Helpers/pathhelper.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

enum ZipCreatedBy {
    ZCB_MSDOS = 0,
    ZCB_Amiga,
    ZCB_OpenVMS,
    ZCB_UNIX,
    ZCB_VM,
    ZCB_Atari,
    ZCB_OS_2,
    ZCB_MAC,
    ZCB_ZSystem,
    ZCB_CPM,
    ZCB_WindowsNTFS,
    ZCB_MVS,
    ZCB_VSE,
    ZCB_Acorn,
    ZCB_VFAT,
    ZCB_AlternateMVS,
    ZCB_BEOS,
    ZCB_Tandem,
    ZCB_OS400,
    ZCB_OSX,
    ZCB_Thru
};

// DOS file info
#define DOS_RDONLY  0x01    /* readonly */
#define DOS_HIDDEN  0x02    /* hidden */
#define DOS_SYSTEM  0x04    /* */
#define DOS_SUBDIR  0x10    /* directory */
#define DOS_ARCH    0x20    /* file */

// Unix file info
#define UNIX_IFIFO  0x10000000  /* named pipe (fifo) */
#define UNIX_IFCHR  0x20000000  /* character special */
#define UNIX_IFDIR  0x40000000  /* directory */
#define UNIX_IFBLK  0x60000000  /* block special */
#define UNIX_IFREG  0x80000000  /* regular */
#define UNIX_IFLNK  0xA0000000 /* symbolic link */
#define UNIX_IFSOCK 0xC0000000 /* socket */

// Permissions
#define S_ISUID 0x08000000 /* set user id on execution */
#define S_ISGID 0x04000000 /* set group id on execution */
#define S_ISTXT 0x02000000 /* sticky bit */
#define S_IRWXU 0x01C00000 /* RWX mask for owner */
#define S_IRUSR 0x01000000 /* R for owner */
#define S_IWUSR 0x00800000 /* W for owner */
#define S_IXUSR 0x00400000 /* X for owner */
#define S_IRWXG 0x00380000 /* RWX mask for group */
#define S_IRGRP 0x00200000 /* R for group */
#define S_IWGRP 0x00100000 /* W for group */
#define S_IXGRP 0x00080000 /* X for group */
#define S_IRWXO 0x00070000 /* RWX mask for other */
#define S_IROTH 0x00040000 /* R for other */
#define S_IWOTH 0x00020000 /* W for other */
#define S_IXOTH 0x00010000 /* X for other */
#define S_ISVTX 0x02000000 /* save swapped text even after use */

Uncompressor::Uncompressor()
{
}

bool Uncompressor::uncompress(QString targetZipFilePath, QString targetOutputDirectoryPath, bool isCleanupTargetOutputDirectoryBeforeUncompress, QStringList ignoreTheseRootDirectories, bool isCopyOnlyContentsOfRootDir, bool isApplyFilePermissions)
{
    // Delete directories
    if(isCleanupTargetOutputDirectoryBeforeUncompress)
    {
        if (!PathHelper::deleteThisDirectoryAndEverythingBelow(targetOutputDirectoryPath)) {
            WLog("Cannot delete target output dir: ") << targetOutputDirectoryPath;
            return false;
        }
    }

    // ensure the target directory is created
    if(!PathHelper::ensureDirectoryCreated(targetOutputDirectoryPath)) {
        WLog("Cannot create target output dir: ") << targetOutputDirectoryPath;
        return false;
    }

    targetOutputDirectoryPath = PathHelper::cleanThisPath(targetOutputDirectoryPath);

    if(targetOutputDirectoryPath.length() <= 0) {
        WLogS << " ! ZIP: no output directory specified (empty path)";
        return false;
    }

    QuaZip zip(targetZipFilePath);

    DLogS << "try to unzip: " << targetZipFilePath;

    if(!zip.open(QuaZip::mdUnzip)) {
        WLog(QString("testRead(): zip.open(): %1").arg(zip.getZipError()));
        return false;
    }

    zip.setFileNameCodec("IBM866");

    bool isFailedToUncompress = false;

    QuaZipFileInfo zipFileInfo;
    QuaZipFile zipFile(&zip);

    quint32 flagDir;
    quint32 flagFile;
    quint32 flagSymlink;
    bool symlinkSupported;
    bool setupPermissions;
    for(bool more = zip.goToFirstFile(); more && !isFailedToUncompress; more = zip.goToNextFile()) {
        if(!zip.getCurrentFileInfo(&zipFileInfo)) {
            WLog(QString("cannnot get current-file-info - getCurrentFileInfo(): %1").arg(zip.getZipError()));
            return false;
        }

        // Get file version information
        switch (zipFileInfo.versionCreated >> 8) {
        case ZCB_MSDOS:
            DLog("Zip Type: MSDOS");

            flagDir = DOS_SUBDIR;
            flagFile = DOS_ARCH;
            flagSymlink = 0;
            symlinkSupported = false;
            setupPermissions = false;
            break;
        case ZCB_UNIX:
            DLog("Zip Type: UNIX");

            flagDir = UNIX_IFDIR;
            flagFile = UNIX_IFREG;
            flagSymlink = UNIX_IFLNK;
            symlinkSupported = true;
            setupPermissions = true;
            break;
        default:
            DLog("Unsupported version made with: ") << (zipFileInfo.versionCreated >> 8);
            zip.close();
            return false;
        }

        QString fileRelativePathInZip = zipFileInfo.name;
        if(!ignoreTheseRootDirectories.isEmpty())
        {
            QStringList filePathComponents = fileRelativePathInZip.split("/");
            bool isIgnoreThisItem = false;
            if(!filePathComponents.isEmpty()) {
                for(int i = 0; i < ignoreTheseRootDirectories.size() && !isIgnoreThisItem; ++i) {
                    if(filePathComponents[0] == ignoreTheseRootDirectories[i]) {
                        isIgnoreThisItem = true;
                    }
                }
            }

            if(isIgnoreThisItem) {
                DLog("This item will be ignored - specified in 'ignore these root dirs' list: ") << fileRelativePathInZip;
                continue;
            }
        }

        if(isCopyOnlyContentsOfRootDir)
        {
            QStringList filePathComponents = fileRelativePathInZip.split("/");
            if(filePathComponents.size() > 1) {
                QString newRelativePath;
                for(int i = 1; i < filePathComponents.size(); ++i) {
                    if(i == 1) {
                        // first item
                        newRelativePath += filePathComponents[i];
                    }
                    else {
                        // not first item
                        newRelativePath += "/" + filePathComponents[i];
                    }
                }
                DLog("newRelativePath: ") << newRelativePath;
                fileRelativePathInZip = newRelativePath;
            }
            else {
                WLog("isCopyOnlyContentsOfRootDir is true but this item is not in a root folder (will be skipped): ") << fileRelativePathInZip;
                continue;
            }
        }

        QString path = PathHelper::combineAndCleanPathes(targetOutputDirectoryPath, fileRelativePathInZip);

#ifdef ENABLE_COMPRESSION_LOGGING
        DLogS << "entry dest dir path " << path;
        DLogS << QString("entry infos; external: (%1); internal: (%2)").arg(QString::number(zipFileInfo.externalAttr)).arg(QString::number(zipFileInfo.internalAttr));
        DLog(" - File comment: ") << zipFileInfo.comment;
        DLog(" - Versions: ") << "upper: " << (zipFileInfo.versionCreated & 0xFF00) << "lower: " << (zipFileInfo.versionCreated & 0x00FF) << "zip version needed" << zipFileInfo.versionNeeded;
        DLog(" - Method: ") << zipFileInfo.method << "general flag: " << zipFileInfo.flags;
        DLog(" - Extra: ") << zipFileInfo.extra;
#endif

        // Directory
        if ((zipFileInfo.externalAttr & flagDir) == flagDir) {
            if (!PathHelper::ensureDirectoryCreated(path)) {
                WLogS << " ! ZIP: cannot create the dir." << path;
                isFailedToUncompress = true;
            } else {
#ifdef ENABLE_COMPRESSION_LOGGING
                DLogS << " ZIP: directory created: " << path;
                DLogS << " - Zip Dir Info: " << zipFileInfo.externalAttr << zipFileInfo.internalAttr;
#endif

#ifdef Q_OS_WIN
                DWORD win_dirAttributes = 0;
                if((zipFileInfo.externalAttr & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN) {
                    // hidden
                    win_dirAttributes |= FILE_ATTRIBUTE_HIDDEN;
                }
                // a win dir cannot be readonly, only files of it

                if(win_dirAttributes > 0) {
                    // set the required file attributes for the created file
                    DLog("Set required win dir attributes: ") << win_dirAttributes;
                    WCHAR *win_createdDirPath = (WCHAR *)malloc(sizeof(WCHAR) * (path.size() + 1));
                    path.toWCharArray(win_createdDirPath);
                    // null terminate
                    win_createdDirPath[path.size()] = 0;

                    if(!SUCCEEDED(SetFileAttributes(win_createdDirPath, win_dirAttributes))) {
                        WLog("Cannot set the required windows dir attributes.");
                    }
                    free(win_createdDirPath);
                }
#endif
            }
        }

        else {

            QFileInfo fpathtmpInfo(path);
            if(!PathHelper::ensureDirectoryCreated(fpathtmpInfo.absolutePath())) {
                WLog("Cannot crate dir: ") << fpathtmpInfo.absolutePath();
                isFailedToUncompress = true;
                break;
            }

            // Symlink
            if (symlinkSupported &&
               ((zipFileInfo.externalAttr & flagSymlink) == flagSymlink)) {
                // Unzip file contents
                if(!zipFile.open(QIODevice::ReadOnly)) {
                    WLog(QString("testRead(): file.open(): %1").arg(zipFile.getZipError()));
                    return false;
                }

                // Get file content
                QString linkPath = zipFile.readAll();

#ifdef Q_OS_MAC
                if (!QFile::link(linkPath, path)) {
                    WLogS << "! ZIP: failed to create symlink: " << path;
                    isFailedToUncompress = true;
                }
#endif

#ifdef Q_OS_WIN
                //#warning WINRE LECSEKKOLNI
                WLog("Windowson leellenorizni, hogy symlinkekre helyesen muxik");

                if (path.toLower().endsWith(".lnk")) {
                    if (!QFile::link(linkPath, path))  {
                        WLogS << "! ZIP: failed to create symlink: " << path;
                        isFailedToUncompress = true;
                    }
                } else {
                    if (!QFile::link(linkPath, path.append(".lnk"))) {
                        WLogS << "! ZIP: failed to create symlink: " << path;
                        isFailedToUncompress = true;
                    }
                }
#endif

                zipFile.close();
            }

            // File
            else if ((zipFileInfo.externalAttr & flagFile) == flagFile) {
                // Unzip file contents
                if(!zipFile.open(QIODevice::ReadOnly)) {
                    WLog(QString("testRead(): file.open(): %1").arg(zipFile.getZipError()));
                    return false;
                }

                // File
                QFile file;

                // Create file
                file.setFileName(path);
                if(!file.open(QIODevice::WriteOnly)) {
                    WLogS << " ! ZIP: cannot open the file for write: " << path;
                    isFailedToUncompress = true;
                } else {
#ifdef ENABLE_COMPRESSION_LOGGING
                    DLogS << " ZIP: file created: " << path;
#endif
                }

                // Write into
                char buf[4096];
                int len = 0;
                char c;
                while (zipFile.getChar(&c)) {
                    // we could just do this, but it's about 40% slower:
                    // out.putChar(c);
                    buf[len++] = c;
                    if (len >= 4096) {
                        file.write(buf, len);
                        len = 0;
                    }
                }
                if (len > 0) {
                    file.write(buf, len);
                }

                // Close the file
                file.close();
                zipFile.close();

#ifdef Q_OS_MAC
                // Set permissions
                if (setupPermissions) {
                    quint32 filePermission = 0;

                    // Owner permissions

                    if (!isApplyFilePermissions ||
                            ((zipFileInfo.externalAttr & S_IRUSR) == S_IRUSR))
                        filePermission |= QFile::ReadOwner | QFile::ReadUser;
                    if (!isApplyFilePermissions ||
                            ((zipFileInfo.externalAttr & S_IWUSR) == S_IWUSR))
                        filePermission |= QFile::WriteOwner | QFile::WriteUser;
                    if (!isApplyFilePermissions ||
                            ((zipFileInfo.externalAttr & S_IXUSR) == S_IXUSR))
                        filePermission |= QFile::ExeOwner | QFile::ExeUser;

                    // Group permissions
                    if (!isApplyFilePermissions ||
                            ((zipFileInfo.externalAttr & S_IRGRP) == S_IRGRP))
                        filePermission |= QFile::ReadGroup;
                    if (!isApplyFilePermissions ||
                            ((zipFileInfo.externalAttr & S_IWGRP) == S_IWGRP))
                        filePermission |= QFile::WriteGroup;
                    if (!isApplyFilePermissions ||
                            ((zipFileInfo.externalAttr & S_IXGRP) == S_IXGRP))
                        filePermission |= QFile::ExeGroup;

                    // Other permissions
                    if (!isApplyFilePermissions ||
                            ((zipFileInfo.externalAttr & S_IROTH) == S_IROTH))
                        filePermission |= QFile::ReadOther;
                    if (!isApplyFilePermissions ||
                            ((zipFileInfo.externalAttr & S_IWOTH) == S_IWOTH))
                        filePermission |= QFile::WriteOther;
                    if (!isApplyFilePermissions ||
                            ((zipFileInfo.externalAttr & S_IXOTH) == S_IXOTH))
                        filePermission |= QFile::ExeOther;

                    PathHelper::setPermissionForFile(path,
                                                     (QFile::Permission)filePermission,
                                                     false);
                }
#endif

#ifdef Q_OS_WIN
                DWORD win_fileAttributes = 0;
                if((zipFileInfo.externalAttr & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN) {
                    // hidden
                    win_fileAttributes |= FILE_ATTRIBUTE_HIDDEN;
                }
                // don't set read-only flag, because it will make the file 'un-removable' in lot of cases
//                if((zipFileInfo.externalAttr & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY) {
//                    // readonly
//                    win_fileAttributes |= FILE_ATTRIBUTE_READONLY;
//                }

                if(win_fileAttributes > 0) {
                    // set the required file attributes for the created file
                    DLog("Set required win file attributes: ") << win_fileAttributes;
                    WCHAR *win_createdFilePath = (WCHAR *)malloc(sizeof(WCHAR) * (path.size() + 1));
                    path.toWCharArray(win_createdFilePath);
                    // null terminate
                    win_createdFilePath[path.size()] = 0;

                    if(!SUCCEEDED(SetFileAttributes(win_createdFilePath, win_fileAttributes))) {
                        WLog("Cannot set the required windows file attributes.");
                    }
                    free(win_createdFilePath);
                }
#endif
            }
        }
    }

    zip.close();

    return !isFailedToUncompress;
}
