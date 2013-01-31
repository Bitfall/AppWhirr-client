#include "win_applicationprocessbyexedata.h"

#ifdef Q_OS_WIN

Win_RunningExecutableData::Win_RunningExecutableData(QString relatedExecutablePath, QList<DWORD> relatedProcessIds) :
    PlatformspecificRunningExecutableDataInterface(relatedExecutablePath),
    relatedProcessIds(relatedProcessIds)
{
}

QList<DWORD> Win_RunningExecutableData::getRelatedProcessIds()
{
    return this->relatedProcessIds;
}

#endif
