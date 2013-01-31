#ifndef MACPROCESS_H
#define MACPROCESS_H

#include <qglobal.h> // required by platform spec ifdef(s)

#ifdef Q_OS_MAC
#include <QList>
#include <QString>
#include <Carbon/Carbon.h>

#include "platformspecificrunningexecutabledatainterface.h"

class MacProcess : public PlatformspecificRunningExecutableDataInterface
{
public:
    explicit MacProcess(QString relatedExecutablePath, ProcessSerialNumber psn);

    ProcessSerialNumber getRelatedProcess();

private:
    ProcessSerialNumber relatedProcess;
};
#endif

#endif // MACPROCESS_H
