#ifndef WIN_APPLICATIONPROCESSBYEXEDATA_H
#define WIN_APPLICATIONPROCESSBYEXEDATA_H

#include <qglobal.h> // required by platform spec ifdef(s)

#ifdef Q_OS_WIN
#include <QList>
#include <QString>
#include <windows.h>

#include "platformspecificrunningexecutabledatainterface.h"

//
// you can use the PlatformSpecificProgramIdentifierHelper class to get the exe related processIds
class Win_RunningExecutableData : public PlatformspecificRunningExecutableDataInterface
{
public:
    explicit Win_RunningExecutableData(QString relatedExecutablePath, QList<DWORD> relatedProcessIds);

    QList<DWORD> getRelatedProcessIds();

private:
    QList<DWORD> relatedProcessIds;
};
#endif

#endif // WIN_APPLICATIONPROCESSBYEXEDATA_H
