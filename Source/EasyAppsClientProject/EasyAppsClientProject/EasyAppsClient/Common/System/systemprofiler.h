#ifndef SYSTEMPROFILER_H
#define SYSTEMPROFILER_H

#include <QString>

#include "Common/Helpers/platformflagshelper.h"

//
// for some OS related informations (OS version) check the QSysInfo class
//  http://doc.qt.nokia.com/4.7/qsysinfo.html
class SystemProfiler
{
public:
    static SystemProfiler* sharedSystemProfiler();

    //
    // it's a combination of OsSubFlag and Bitness flags in 1 uint.
    //  this DOES NOT contain the main OS, only the current OS's sub-flag - e.g.: if this system is a "Win Vista SP1 32 bit" then this uint containt: Vista SP1, 32 bit, but it does not contain that it's a Windows machine
    // will return 0 if it's an unsupported platform
    quint64 _getCurrentSystemSubFlagsAsUInt();
    //
    // this contains every information about the current system
    PlatformFlagsHelper::SummedSinglePlatformInformations *_getCurrentSystemPlatformInformations();

//    bool isThisSummedFlagCompatibleWithThisPlatform(QString summedFlags);
    bool isThisInfoIsCompatibleWithThisPlatform(PlatformFlagsHelper::SummedSinglePlatformInformations *info);

    bool is64BitSupported();

private:
    SystemProfiler();

    quint64 cpuCount;
    quint64 memorySize;
    quint64 cpuFrequency;
    bool    has64BitSupport;
};

#endif // SYSTEMPROFILER_H
