#include "runnable_copydirectory.h"

Runnable_CopyDirectory::Runnable_CopyDirectory(QString sourceDirPath, QString targetDirPath, bool isCopyOnlyContentOfSourceDirectory, PathHelper::SymlinkHandlingModeEnum symlinkHandlingMode, bool isFailWhenContentCopyFails, QObject *parent) :
    QObject(parent),
    _sourceDirPath(sourceDirPath),
    _targetDirPath(targetDirPath),
    _isCopyOnlyContentOfSourceDirectory(isCopyOnlyContentOfSourceDirectory),
    _isFailWhenContentCopyFails(isFailWhenContentCopyFails),
    _symlinkHandlingMode(symlinkHandlingMode)
{
}

void Runnable_CopyDirectory::run()
{
    DLog("source and dest path: ") << _sourceDirPath << _targetDirPath;

    if(PathHelper::copyWholeDirectory(_sourceDirPath, _targetDirPath, _isCopyOnlyContentOfSourceDirectory, _symlinkHandlingMode, _isFailWhenContentCopyFails)) {
        Q_EMIT finishedWithSuccess();
    }
    else {
        Q_EMIT failedWithError("Cannot copy.");
    }
}
