#include "systemprofiler.h"

#include <QObject>

#include "Common/Helpers/platformflagshelper.h"

#ifdef Q_WS_MAC
#include <sys/sysctl.h>
#include <Carbon/Carbon.h>

#if __BIG_ENDIAN__
#define k64BitSupport       "hw.optional.64bitops"
#else
#define k64BitSupport       "hw.optional.x86_64"
#endif
#endif

#ifdef Q_WS_WIN
#include <windows.h>
//#include "stdafx.h"
#include <iostream>
#include "comutil.h"

typedef BOOL (WINAPI *IW64PFP)(HANDLE, BOOL *);
#endif

SystemProfiler* SystemProfiler::sharedSystemProfiler() {
    static SystemProfiler* systemProfiler = 0;

    if (!systemProfiler) {
        systemProfiler = new SystemProfiler;
    }
    return systemProfiler;
}

SystemProfiler::SystemProfiler()
{    
#ifdef Q_WS_MAC
    size_t length;
    uint32_t v32;
    uint64_t v64;

    length = sizeof(v32);
    if(sysctlbyname("hw.activecpu", &v32, &length, NULL, 0) == 0)
        cpuCount = v32;
    else
        DLog("Failed to get cpu count");

    length = sizeof(v64);
    if(sysctlbyname("hw.memsize", &v64, &length, NULL, 0) == 0)
        memorySize = v64;
    else
        DLog("Failed to get memory size");

    length = sizeof(v64);
    if(sysctlbyname("hw.cpufrequency", &v64, &length, NULL, 0) == 0)
        cpuFrequency = v64;
    else
        DLog("Failed to get cpu frequency");

    length = sizeof(v32);
    if((sysctlbyname(k64BitSupport, &v32, &length, NULL, 0) == 0) && v32)
        has64BitSupport = true;
    else
        has64BitSupport = false;
#endif

#ifdef Q_WS_WIN
    BOOL res = FALSE;

    // When this application is compiled as a 32-bit app,
    // and run on a native 64-bit system, Windows will run
    // this application under WOW64.  WOW64 is the Windows-
    // on-Windows subsystem that lets native 32-bit applications
    // run in 64-bit land.  This calls the kernel32.dll
    // API to see if this process is running under WOW64.
    // If it is running under WOW64, then that clearly means
    // this 32-bit application is running on a 64-bit OS,
    // and IsWow64Process will return true.
    IW64PFP IW64P = (IW64PFP)GetProcAddress(
                GetModuleHandle(L"kernel32"), "IsWow64Process");

    if(IW64P != NULL) {
        IW64P(GetCurrentProcess(), &res);
    }

    //      cout << ((res) ? RESPONSE_64_BIT : RESPONSE_32_BIT) << endl;

    has64BitSupport = (res);
#endif

#ifdef ENABLE_SYSTEM_PROFILER_LOGGING
    DLog("Is this a 64 bit OS? ") << has64BitSupport;
#endif
}

quint64 SystemProfiler::_getCurrentSystemSubFlagsAsUInt() {
    quint64 flags = 0;

//#ifdef Q_WS_MAC
//    flags = PlatformFlagsHelper::_buildSummedPlatformFlagsByComponentsAsInt(PlatformFlagsHelper::Mac, (has64BitSupport ? PlatformFlagsHelper::Bit64 : PlatformFlagsHelper::Bit32));

//    flags |= PlatformFlagsHelper::__getAllMacFlagsSummed();
//#endif

//#ifdef Q_WS_WIN
//    // bitness
//    flags = (quint64) ((has64BitSupport ? PlatformFlagsHelper::Bit64 : PlatformFlagsHelper::Bit32));

//    // current OS's sub-flag
//    flags |= PlatformFlagsHelper::__getAllWinFlagsSummed();
//#endif

    PlatformFlagsHelper::SummedSinglePlatformInformations *platformInfo = this->_getCurrentSystemPlatformInformations();
    if(platformInfo != NULL) {
        flags = (quint64)platformInfo->_getSummedSubFlags();
        delete platformInfo;
    }

#ifdef ENABLE_SYSTEM_PROFILER_LOGGING
    DLog("Supported platform sub-flags: ") << flags;
#endif

    return flags;
}

bool SystemProfiler::is64BitSupported()
{
    return has64BitSupport;
}

bool SystemProfiler::isThisInfoIsCompatibleWithThisPlatform(PlatformFlagsHelper::SummedSinglePlatformInformations *info)
{
    PlatformFlagsHelper::SummedSinglePlatformInformations *currPlatformInfos = this->_getCurrentSystemPlatformInformations();
    bool isCompatible = PlatformFlagsHelper::isTheseInfosAreCompatible(currPlatformInfos, info);
    delete currPlatformInfos;

    return isCompatible;
}

#ifdef Q_WS_WIN
PlatformFlagsHelper::SummedWinPlatformInformations *__win_CurrentSystemPlatformInformations() {

    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    GetVersionEx((OSVERSIONINFO *)&osvi);

    //
    // version numbers from: http://msdn.microsoft.com/en-us/library/ms724833(VS.85).aspx
    //
    // * XP: 5.1
    // * XP Pro x64 or Server 2003: 5.2
    // ** so in general: 5.x is XP
    // * Vista and Win Server 2008: 6.0
    // * Win7 and Win Server 2008 RC2: 6.1
    // * Win8 is 6.2 (by experimenting with the 'Preview' versions of the not-yet released Win8)
    //

    // OS version - debug
#if 0
    DLog("Major version: ") << osvi.dwMajorVersion;
    DLog("Minor version: ") << osvi.dwMinorVersion;
    // SP version
    DLog("SP major version: ") << osvi.wServicePackMajor;
    DLog("SP minor version: ") << osvi.wServicePackMinor;
//    DLog("Service pack name: ") << osvi.szCSDVersion;
    // other
    DLog("Build: ") << osvi.dwBuildNumber;
    DLog("Platform ID: ") << osvi.dwPlatformId;
    DLog("Suite mask: ") << osvi.wSuiteMask;
#endif

    PlatformFlagsHelper::PlatformFlagsBitness bitness = SystemProfiler::sharedSystemProfiler()->is64BitSupported() ? PlatformFlagsHelper::Bit64 : PlatformFlagsHelper::Bit32;

    PlatformFlagsHelper::WinSubFlags winSubFlag = PlatformFlagsHelper::Win_INVALID;

    if(osvi.dwMajorVersion == 5) {
        // XP
        if(osvi.wServicePackMajor == 0) {
            winSubFlag = PlatformFlagsHelper::Win_XP_NoSP;
        }
        else if(osvi.wServicePackMajor == 1) {
            winSubFlag = PlatformFlagsHelper::Win_XP_SP1;
        }
        else if(osvi.wServicePackMajor == 2) {
            winSubFlag = PlatformFlagsHelper::Win_XP_SP2;
        }
        else if(osvi.wServicePackMajor == 3) {
            winSubFlag = PlatformFlagsHelper::Win_XP_SP3;
        }
    }
    if(osvi.dwMajorVersion == 6) {
        // Vista or Win7
        if(osvi.dwMinorVersion == 0) {
            // Vista
            if(osvi.wServicePackMajor == 0) {
                winSubFlag = PlatformFlagsHelper::Win_Vista_NoSP;
            }
            else if(osvi.wServicePackMajor == 1) {
                winSubFlag = PlatformFlagsHelper::Win_Vista_SP1;
            }
            else if(osvi.wServicePackMajor == 2) {
                winSubFlag = PlatformFlagsHelper::Win_Vista_SP2;
            }
        }
        else if(osvi.dwMinorVersion == 1) {
            // Win7
            if(osvi.wServicePackMajor == 0) {
                winSubFlag = PlatformFlagsHelper::Win_7_NoSP;
            }
            else if(osvi.wServicePackMajor == 1) {
                winSubFlag = PlatformFlagsHelper::Win_7_SP1;
            }
        }
        else if(osvi.dwMinorVersion == 2) {
            // Win8
            if(osvi.wServicePackMajor == 0) {
                winSubFlag = PlatformFlagsHelper::Win_8_NoSP;
            }
        }
    }

#ifdef ENABLE_SYSTEM_PROFILER_LOGGING
    DLog("Win Sub Flag: ") << winSubFlag;
    DLog("Bitness flag: ") << bitness;
#endif

    PlatformFlagsHelper::SummedWinPlatformInformations *_ret = new PlatformFlagsHelper::SummedWinPlatformInformations(winSubFlag, bitness);

    return _ret;
}
#endif

#ifdef Q_WS_MAC
PlatformFlagsHelper::SummedMacPlatformInformations *__mac_CurrentSystemPlatformInformations() {
    int versionMajor = 0;
    int versionMinor = 0;
    int versionBugFix = 0;
    Gestalt( gestaltSystemVersionMajor, &versionMajor );
    Gestalt( gestaltSystemVersionMinor, &versionMinor );
    Gestalt( gestaltSystemVersionBugFix, &versionBugFix );

    quint64 flag = PlatformFlagsHelper::Mac_INVALID;

    switch(versionMajor)
    {
    case 10:
        switch(versionMinor)
        {
        case 6:
            switch(versionBugFix)
            {
            case 0:
                flag = PlatformFlagsHelper::MacOS_10_6_0;
                break;
            case 1:
                flag = PlatformFlagsHelper::MacOS_10_6_1;
                break;
            case 2:
                flag = PlatformFlagsHelper::MacOS_10_6_2;
                break;
            case 3:
                flag = PlatformFlagsHelper::MacOS_10_6_3;
                break;
            case 4:
                flag = PlatformFlagsHelper::MacOS_10_6_4;
                break;
            case 5:
                flag = PlatformFlagsHelper::MacOS_10_6_5;
                break;
            case 6:
                flag = PlatformFlagsHelper::MacOS_10_6_6;
                break;
            case 7:
                flag = PlatformFlagsHelper::MacOS_10_6_7;
                break;
            case 8:
                flag = PlatformFlagsHelper::MacOS_10_6_8;
                break;
            default:
                break;
            }

            break;
        case 7:
            switch(versionBugFix)
            {
            case 0:
                flag = PlatformFlagsHelper::MacOS_10_7_0;
                break;
            case 1:
                flag = PlatformFlagsHelper::MacOS_10_7_1;
                break;
            case 2:
                flag = PlatformFlagsHelper::MacOS_10_7_2;
                break;
            case 3:
                flag = PlatformFlagsHelper::MacOS_10_7_3;
                break;
            case 4:
                flag = PlatformFlagsHelper::MacOS_10_7_4;
            default:
                break;
            }

            break;
        }

        break;
    }
    PlatformFlagsHelper::SummedMacPlatformInformations *_ret = new PlatformFlagsHelper::SummedMacPlatformInformations(flag,
                                                                                                                      SystemProfiler::sharedSystemProfiler()->is64BitSupported() ? PlatformFlagsHelper::Bit64 : PlatformFlagsHelper::Bit32);

    return _ret;
}
#endif


PlatformFlagsHelper::SummedSinglePlatformInformations *SystemProfiler::_getCurrentSystemPlatformInformations() {
    PlatformFlagsHelper::SummedSinglePlatformInformations *_ret = NULL;

#ifdef Q_WS_WIN
    _ret = __win_CurrentSystemPlatformInformations();
#endif

#ifdef Q_WS_MAC
    _ret = __mac_CurrentSystemPlatformInformations();
#endif

    return _ret;
}

