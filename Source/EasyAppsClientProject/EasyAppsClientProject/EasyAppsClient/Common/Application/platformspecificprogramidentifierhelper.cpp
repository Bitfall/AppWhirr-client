#include "platformspecificprogramidentifierhelper.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include "../Helpers/pathhelper.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>

#include "win_applicationprocessbyexedata.h"
#endif

PlatformSpecificProgramIdentifierHelper::PlatformSpecificProgramIdentifierHelper()
{
}

QList<PlatformspecificRunningExecutableDataInterface *> PlatformSpecificProgramIdentifierHelper::getRunningExecutableDataForPathes(QStringList pathes)
{
    QList<PlatformspecificRunningExecutableDataInterface *> retList;

#if defined(Q_OS_WIN)

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        WLog("FAILED: CreateToolhelp32Snapshot (of processes)");
        return retList;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        WLog("FAILED: Process32First");
        CloseHandle( hProcessSnap );          // clean the snapshot object
        return retList;
    }

    QHash< QString, QList<DWORD> > fileProcHash;
    // Now walk the snapshot of processes
    do
    {
        QString processRelatedExe = QString::fromWCharArray(pe32.szExeFile);
//        DLogS << "Process related exe compare: " << processRelatedExe << relatedExeFileName;
        Q_FOREACH(QString currFilePath, pathes)
        {
            QFileInfo relatedExeFileInfo(currFilePath);
            QString relatedExeFullPath = relatedExeFileInfo.absoluteFilePath();
            QString relatedExeFileName = relatedExeFileInfo.fileName();

            if(processRelatedExe == relatedExeFileName)
            {
                DLogS << "Exe matches a running instance's exe-file: " << processRelatedExe << relatedExeFileName;
                if(PlatformSpecificProgramIdentifierHelper::_isExePathRelatedToProcess(pe32.th32ProcessID, relatedExeFullPath))
                {
                    QList<DWORD> relatedProcIds;
                    if( fileProcHash.contains(relatedExeFullPath) ) {
                        relatedProcIds = fileProcHash.value(relatedExeFullPath);
                    }
                    relatedProcIds.append(pe32.th32ProcessID);
                    fileProcHash[relatedExeFullPath] = relatedProcIds;
                }
            }
        }

    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );

    QHash< QString, QList<DWORD> >::const_iterator c_it = fileProcHash.constBegin();
    while (c_it != fileProcHash.constEnd()) {
        DLog("Running item: ") << c_it.key() << ":" << c_it.value();
        retList.append(new Win_RunningExecutableData(c_it.key(), c_it.value()));
        ++c_it;
    }

#elif defined(Q_OS_MAC)

    LOG_AS_NOT_IMPLEMENTED;

#endif

    return retList;
}

#ifdef Q_OS_WIN
//
// based on: http://stackoverflow.com/questions/4891495/is-it-possible-to-get-the-winapi-process-handle-by-the-process-name-without-itera
// and: http://msdn.microsoft.com/en-us/library/ms686701(v=vs.85).aspx
//
QList<DWORD> PlatformSpecificProgramIdentifierHelper::getAllProcessIdsByExePath(QString exePath)
{
//    DLogS << "Exe path" << exePath;

    QList<DWORD> foundIds;

    if(!PathHelper::isFileExistsAndItsReallyAFile(exePath)) {
        return foundIds;
    }

    QFileInfo relatedExeFileInfo(exePath);
    QString relatedExeFileName = relatedExeFileInfo.fileName();
//    DLogS << "File name: " << relatedExeFileName;

//    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
//    if (Process32First(snapshot, &entry) == TRUE){
//        while (Process32Next(snapshot, &entry) == TRUE)
//        {
//            if(QString::fromWCharArray(entry.szExeFile) == exePath) {
////            if (stricmp(entry.szExeFile, exePath.toAscii(). ) == 0){
////                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
////                // measure process memory usage
////                CloseHandle(hProcess);


//            }
//        }
//    }
//    CloseHandle(snapshot);



    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        DLog("FAILED: CreateToolhelp32Snapshot (of processes)");
        return foundIds;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        DLog("FAILED: Process32First");
        CloseHandle( hProcessSnap );          // clean the snapshot object
        return foundIds;
    }

    // Now walk the snapshot of processes
    do
    {
        QString processRelatedExe = QString::fromWCharArray(pe32.szExeFile);
//        DLogS << "Process related exe compare: " << processRelatedExe << relatedExeFileName;
        if(processRelatedExe == relatedExeFileName) {
            DLogS << "Exe matches a running instance";
            if(PlatformSpecificProgramIdentifierHelper::_isExePathRelatedToProcess(pe32.th32ProcessID, exePath)) {
                foundIds.append(pe32.th32ProcessID);
            }
        }

    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );


    return foundIds;
}

bool PlatformSpecificProgramIdentifierHelper::_isExePathRelatedToProcess(DWORD dwPID, QString exePath)
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    FLAG_FOR_REVIEW_WITH_HINT("https://bitbucket.org/shadevampire/easyapps/issue/238/bug-termination-fails-for-some-64-bit-apps");

    // Take a snapshot of all modules in the specified process.
#ifdef __MINGW32__
    hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE/* | TH32CS_SNAPMODULE32*/, dwPID );
#else // MSVC
    hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwPID );
#endif
    if( hModuleSnap == INVALID_HANDLE_VALUE )
    {
        DLog("Failed, retry if error-bad-length...");
        bool isSuccess = false;
        int lastErrorCode = GetLastError();
        while( (lastErrorCode = GetLastError()) == ERROR_BAD_LENGTH) {
            // retry while failes
            // it's hilarious, but this is what the docs says...
            //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms682489(v=vs.85).aspx
            DLog("Retry while it's error-bad-length, as the docs says...") << lastErrorCode;

#ifdef __MINGW32__
            hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE/* | TH32CS_SNAPMODULE32*/, dwPID );
#else // MSVC
            hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwPID );
#endif
            if( hModuleSnap == INVALID_HANDLE_VALUE )
            {
                // retry if error-bad-length
            }
            else {
                isSuccess = true;
            }
        }

        if(!isSuccess) {
            DLog("FAILED: CreateToolhelp32Snapshot (of modules). Last error code: ") << lastErrorCode;
            return( false );
        }
    }

    // Set the size of the structure before using it.
    me32.dwSize = sizeof( MODULEENTRY32 );

    // Retrieve information about the first module,
    // and exit if unsuccessful
    if( !Module32First( hModuleSnap, &me32 ) )
    {
        DLog("FAILED: Module32First");  // show cause of failure
        CloseHandle( hModuleSnap );           // clean the snapshot object
        return( false );
    }

    bool isFound = false;
    // Now walk the module list of the process,
    // and display information about each module
    do
    {
//        _tprintf( TEXT("\n\n     MODULE NAME:     %s"),   me32.szModule );
//        _tprintf( TEXT("\n     Executable     = %s"),     me32.szExePath );
//        _tprintf( TEXT("\n     Process ID     = 0x%08X"),         me32.th32ProcessID );
//        _tprintf( TEXT("\n     Ref count (g)  = 0x%04X"),     me32.GlblcntUsage );
//        _tprintf( TEXT("\n     Ref count (p)  = 0x%04X"),     me32.ProccntUsage );
//        _tprintf( TEXT("\n     Base address   = 0x%08X"), (DWORD) me32.modBaseAddr );
//        _tprintf( TEXT("\n     Base size      = %d"),             me32.modBaseSize );

        if(me32.th32ProcessID == dwPID) {
            QString processRelatedExePath = QString::fromWCharArray(me32.szExePath);
            processRelatedExePath = QDir().absoluteFilePath(processRelatedExePath);
            processRelatedExePath = PathHelper::cleanThisPath(processRelatedExePath);
            QString absoluteReferenceExePath = QDir().absoluteFilePath(exePath);
            absoluteReferenceExePath = PathHelper::cleanThisPath(absoluteReferenceExePath);
//            DLogS << "proc related exe-path: " << processRelatedExePath << " reference path: " << absoluteReferenceExePath;
            if(processRelatedExePath == absoluteReferenceExePath) {
                isFound = true;
                break;
            }
        }
    } while( Module32Next( hModuleSnap, &me32 ) );

    CloseHandle( hModuleSnap );
    return( isFound );
}

#endif
