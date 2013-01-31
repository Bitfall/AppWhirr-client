#ifndef PLATFORMSPECIFICPROGRAMIDENTIFIERHELPER_H
#define PLATFORMSPECIFICPROGRAMIDENTIFIERHELPER_H

#include <QObject>
#include <QString>
#include <QList>

#include "platformspecificrunningexecutabledatainterface.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class PlatformSpecificProgramIdentifierHelper
{
private:
    PlatformSpecificProgramIdentifierHelper();

    /*! Checks the given \a pathes for running executables and returns related PlatformspecificRunningExecutableDataInterface for every running executable.

        @note Important! It's the caller's responsibility to free-up the returned list's items!
    */
    static QList<PlatformspecificRunningExecutableDataInterface *> getRunningExecutableDataForPathes(QStringList pathes);

#ifdef Q_OS_WIN
public:
    /*! Retrieves Windows specific process-ids of running instances of the given exe (\a exePath).

        This method is designed to get these process-ids for a single file. If you want to check multiple files you should use a batch-checker method instead (ex: \a getRunningExecutableDataForPathes() ) for performance reason.
    */
    static QList<DWORD> getAllProcessIdsByExePath(QString exePath);

private:
    static bool _isExePathRelatedToProcess(DWORD dwPID, QString exePath);
#endif
};

#endif // PLATFORMSPECIFICPROGRAMIDENTIFIERHELPER_H
