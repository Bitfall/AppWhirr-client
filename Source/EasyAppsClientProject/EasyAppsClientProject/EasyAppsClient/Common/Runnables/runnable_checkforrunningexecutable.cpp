#include "runnable_checkforrunningexecutable.h"

#include "../Helpers/pathhelper.h"

Runnable_CheckForRunningExecutable::Runnable_CheckForRunningExecutable(QString rootDirPath, QObject *parent) :
    QObject(parent),
    _rootDirPath(rootDirPath)
{
}

void Runnable_CheckForRunningExecutable::run()
{
    DLog("source path: ") << _rootDirPath;

    QList<PlatformspecificRunningExecutableDataInterface *> resultPlatformSpecificRunningAppInfos;
    PathHelper::checkForRunningExecutableUnderThisDirector(_rootDirPath, resultPlatformSpecificRunningAppInfos);
    Q_EMIT finishedWithSuccess(resultPlatformSpecificRunningAppInfos);
}
