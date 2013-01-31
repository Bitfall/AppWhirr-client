#ifndef BACKGROUND_COPYDIRECTORY_H
#define BACKGROUND_COPYDIRECTORY_H

#include <QObject>
#include <QRunnable>

#include "../Helpers/pathhelper.h"

class Runnable_CopyDirectory : public QObject, public QRunnable
{
    Q_OBJECT
public:
    //
    // if target-dir-path does not exists then it will try to create it
    // if isCopyOnlyContentOfSourceDirectory is true then it won't create a directory inside the targetDir called sourceDir, but it will try to copy the content of the sourceDir only
    //      if it's false then it will create the given sourceDir as well inside the targetDir
    explicit Runnable_CopyDirectory(QString sourceDirPath, QString targetDirPath, bool isCopyOnlyContentOfSourceDirectory, PathHelper::SymlinkHandlingModeEnum symlinkHandlingMode, bool isFailWhenContentCopyFails, QObject *parent = 0);

    void run();

Q_SIGNALS:
    void finishedWithSuccess();
    void failedWithError(QString errorMessage);

private:
    QString _sourceDirPath;
    QString _targetDirPath;
    bool _isCopyOnlyContentOfSourceDirectory;
    bool _isFailWhenContentCopyFails;
    PathHelper::SymlinkHandlingModeEnum _symlinkHandlingMode;
};

#endif // BACKGROUND_COPYDIRECTORY_H
