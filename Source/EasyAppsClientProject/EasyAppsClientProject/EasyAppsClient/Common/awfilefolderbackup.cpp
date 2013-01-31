#include "awfilefolderbackup.h"

#include <QFile>
#include <QDir>

#include "./Helpers/pathhelper.h"

AWFileFolderBackup::AWFileFolderBackup(QFileInfo fileOrFolderInfo, QString backupRenamePrefix, QObject *parent) :
    QObject(parent),
    _originalFileOrFolderInfo(fileOrFolderInfo),
    _backupRenamePrefix(backupRenamePrefix)
{
}

bool AWFileFolderBackup::doBackup()
{
    if( _originalFileOrFolderInfo.isFile() )
    {
        QString backuppedFileName = QString("%1%2").arg(_backupRenamePrefix).arg(_originalFileOrFolderInfo.fileName());
        return QFile(_originalFileOrFolderInfo.absoluteFilePath()).rename(backuppedFileName);
    }
    else if( _originalFileOrFolderInfo.isDir() )
    {
        QDir parentDir(_originalFileOrFolderInfo.absoluteFilePath());
        if( !parentDir.cdUp() ) {
            WLog("Cannot switch to parent directory.");
            return false;
        }
        QString originalDirName = QDir(_originalFileOrFolderInfo.absoluteFilePath()).dirName();
        if(originalDirName.isEmpty()) {
            WLog("Invalid original directory name (empty)");
            return false;
        }
        QString backuppedDirName = QString("%1%2").arg(_backupRenamePrefix).arg(originalDirName);
        return parentDir.rename(originalDirName, backuppedDirName);
    }
    else {
        WLog("Unsupported file-system-entry type.");
        return false;
    }
}

bool AWFileFolderBackup::rollbackBackup()
{
    if( _originalFileOrFolderInfo.isFile() )
    {
        QString backuppedFileName = QString("%1%2").arg(_backupRenamePrefix).arg(_originalFileOrFolderInfo.fileName());
        QString backuppedFileFullPath = PathHelper::combineAndCleanPathes(_originalFileOrFolderInfo.absolutePath(), backuppedFileName);
        return QFile(backuppedFileFullPath).rename(_originalFileOrFolderInfo.fileName());
    }
    else if( _originalFileOrFolderInfo.isDir() )
    {
        QDir parentDir(_originalFileOrFolderInfo.absoluteFilePath());
        if( !parentDir.cdUp() ) {
            WLog("Cannot switch to parent directory.");
            return false;
        }
        QString originalDirName = QDir(_originalFileOrFolderInfo.absoluteFilePath()).dirName();
        if(originalDirName.isEmpty()) {
            WLog("Invalid original directory name (empty)");
            return false;
        }
        QString backuppedDirName = QString("%1%2").arg(_backupRenamePrefix).arg(originalDirName);
        return parentDir.rename(backuppedDirName, originalDirName);
    }
    else {
        WLog("Unsupported file-system-entry type.");
        return false;
    }
}

bool AWFileFolderBackup::deleteBackup()
{
    if( _originalFileOrFolderInfo.isFile() )
    {
        QString backuppedFileName = QString("%1%2").arg(_backupRenamePrefix).arg(_originalFileOrFolderInfo.fileName());
        QString backuppedFileFullPath = PathHelper::combineAndCleanPathes(_originalFileOrFolderInfo.absolutePath(), backuppedFileName);
        return QFile(backuppedFileFullPath).remove();
    }
    else if( _originalFileOrFolderInfo.isDir() )
    {
//        QDir parentDir(_originalFileOrFolderInfo.absoluteFilePath());
//        if( !parentDir.cdUp() ) {
//            WLog("Cannot switch to parent directory.");
//            return false;
//        }
        QString originalDirName = QDir(_originalFileOrFolderInfo.absoluteFilePath()).dirName();
        if(originalDirName.isEmpty()) {
            WLog("Invalid original directory name (empty)");
            return false;
        }
        QString backuppedDirName = QString("%1%2").arg(_backupRenamePrefix).arg(originalDirName);
        QString backuppedDirFullPath = PathHelper::combineAndCleanPathes(_originalFileOrFolderInfo.absolutePath(), backuppedDirName);
        return PathHelper::deleteThisDirectoryAndEverythingBelow(backuppedDirFullPath);
    }
    else {
        WLog("Unsupported file-system-entry type.");
        return false;
    }
}
