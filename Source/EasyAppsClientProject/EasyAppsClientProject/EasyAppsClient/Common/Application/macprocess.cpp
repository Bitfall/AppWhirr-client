#include "macprocess.h"

#ifdef Q_OS_MAC
MacProcess::MacProcess(QString relatedExecutablePath, ProcessSerialNumber psn) :
    PlatformspecificRunningExecutableDataInterface(relatedExecutablePath),
    relatedProcess(psn)
{
}


ProcessSerialNumber MacProcess::getRelatedProcess()
{
    return this->relatedProcess;
}
#endif
