#include "appterminationcontroller.h"

#include <QMetaType>
#include <QTimer>
#include <QThreadPool>

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#include <Common/Application/macprocess.h>
#endif

#ifdef Q_WS_WIN
#include <windows.h>
#include "Common/Application/win_applicationprocessbyexedata.h"

#define TA_FAILED 0
#define TA_SUCCESS_CLEAN 1
#define TA_SUCCESS_KILL 2
#define TA_SUCCESS_16 3

DWORD WINAPI TerminateApp( DWORD dwPID, DWORD dwTimeout ) ;
//DWORD WINAPI Terminate16App( DWORD dwPID, DWORD dwThread,
//                            WORD w16Task, DWORD dwTimeout );

#define WIN_TERMINATION_TIMEOUT_MILLISECONDS 10000

#endif


// utility
#include "Common/Helpers/pathhelper.h"
#include "Common/Application/platformspecificprogramidentifierhelper.h"
#include "Common/Runnables/runnable_checkforrunningexecutable.h"
#include "Common/Application/platformspecificrunningexecutabledatainterface.h"


AppTerminationController::AppTerminationController(QObject *parent) :
    QObject(parent),
    _isInTerminationProcess(false)
{
    qRegisterMetaType<AppTerminationQueueItem>("AppTerminationQueueItem");
    qRegisterMetaType< QList<PlatformspecificRunningExecutableDataInterface*> >("QList<PlatformspecificRunningExecutableDataInterface*>");

    //
    // internal connections
    connect(this, SIGNAL(__addThisRootDirectoryToQueueSignal(AppTerminationQueueItem)), this, SLOT(_addThisRootDirectoryToQueueAndTryToStart(AppTerminationQueueItem)));

    // other initializations
    _checkForRunningInstanceTimer = new QTimer(this);
    connect(_checkForRunningInstanceTimer, SIGNAL(timeout()), this, SLOT(_checkIfCurrentApplicationIsStillRunning()));

    _checkForRunningInstanceTimer->setInterval(500);
    _checkForRunningInstanceTimer->setSingleShot(false);
    _checkForRunningInstanceTimer->stop();
}


// -----------------
// --- interface ---

void AppTerminationController::terminateAppUnderDirectory(QString appId, QString appName, QString rootDirectoryToScanForRunningApps, AppTerminationController::TerminationReasonCodeEnum terminationReasonCode)
{
    Q_EMIT __addThisRootDirectoryToQueueSignal(AppTerminationQueueItem(appId, appName, rootDirectoryToScanForRunningApps, terminationReasonCode));
}


// -----------------
// --- presenter ---

void AppTerminationController::_presenter_StartTermination()
{
    this->_terminateCurrentApplication();
}

void AppTerminationController::_presenter_AbortTermination()
{
    this->_currentApplicationTerminationFailed();
}


// ----------------
// --- internal ---

void AppTerminationController::_addThisRootDirectoryToQueueAndTryToStart(AppTerminationQueueItem queueItem)
{
    this->_queueItems.append(queueItem);
    this->_tryNextScanAndTerminationFromQueue();
}

void AppTerminationController::_removeCurrentItemFromQueueAndTryStartTheNextOne()
{
    if(this->_queueItems.isEmpty()) {
        WLog("Cannot remove - queue is empty.");
        return;
    }

    //
    // remove

    this->_queueItems.dequeue();
    // release
    for(int i = 0; i < this->_currRunningExecutableDataList.size(); i++) {
#ifdef Q_WS_WIN
        Win_RunningExecutableData *casted = dynamic_cast<Win_RunningExecutableData *>(this->_currRunningExecutableDataList[i]);
#endif

#ifdef Q_WS_MAC
        MacProcess *casted = dynamic_cast<MacProcess *>(this->_currRunningExecutableDataList[i]);
#endif
        if(casted == NULL) {
            WLog("Cannot cast!!!");
        }
        else {
            delete casted;
        }
    }

    this->_currRunningExecutableDataList.clear();

    this->_isInTerminationProcess = false;

    //
    // start the next one
    this->_tryNextScanAndTerminationFromQueue();
}

void AppTerminationController::_tryNextScanAndTerminationFromQueue()
{
    if(_isInTerminationProcess) {
        return;
    }

    if(this->_queueItems.isEmpty()) {
        return;
    }

    // termination process started
    _isInTerminationProcess = true;

    // start the scan
    DLog("--Start scan");
    Runnable_CheckForRunningExecutable *worker = new Runnable_CheckForRunningExecutable(this->_queueItems.head().getRootDirectoryToScanForRunningApps());
    connect(worker, SIGNAL(finishedWithSuccess(QList<PlatformspecificRunningExecutableDataInterface*>)), this, SLOT(_currentItemRunningExecutableCheckFinished(QList<PlatformspecificRunningExecutableDataInterface*>)));
    QThreadPool::globalInstance()->start(worker);
}

void AppTerminationController::_currentItemRunningExecutableCheckFinished(QList<PlatformspecificRunningExecutableDataInterface *> result)
{
    DLog("--Found item cnt:") << result.size();
    // store the results
    QList<PlatformspecificRunningExecutableDataInterface *> runningExeDataList = result;

    if(!runningExeDataList.isEmpty()) {
        // found running app
        _currRunningExecutableDataList = runningExeDataList;
    }
    else {
        Q_EMIT nothingToTerminate(this->_queueItems.head().getAppId(), this->_queueItems.head().getAppName());
        this->_removeCurrentItemFromQueueAndTryStartTheNextOne();
        return;
    }

    // and show with presenter and check regularly
    this->_checkForRunningInstanceTimer->start();
    Q_EMIT _presenter_ShowWithInfo(this->_queueItems.head().getAppId(), this->_queueItems.head().getAppName(), this->_queueItems.head().getTerminationReasonCode());
}


void AppTerminationController::_currentApplicationTerminated()
{
    this->_checkForRunningInstanceTimer->stop();
    Q_EMIT _presenter_Hide();
    if(this->_queueItems.isEmpty()) {
        WLog("Queue is empty! Crucial!!!");
        return;
    }
    Q_EMIT applicationTerminatedSuccesfully(this->_queueItems.head().getAppId(), this->_queueItems.head().getAppName());
    this->_removeCurrentItemFromQueueAndTryStartTheNextOne();
}

void AppTerminationController::_currentApplicationTerminationFailed()
{
    this->_checkForRunningInstanceTimer->stop();
    Q_EMIT _presenter_Hide();
    if(this->_queueItems.isEmpty()) {
        WLog("Queue is empty! Crucial!!!");
        return;
    }
    Q_EMIT cannotTerminateApplication(this->_queueItems.head().getAppId(), this->_queueItems.head().getAppName());
    this->_removeCurrentItemFromQueueAndTryStartTheNextOne();
}



void AppTerminationController::_terminateCurrentApplication() {
    bool applicationTerminated = false;

#ifdef Q_OS_MAC
    bool isTerminationFailed = false;
    for (int i=0; i < _currRunningExecutableDataList.size() && !isTerminationFailed; i++) {
        MacProcess* casted = dynamic_cast<MacProcess*>(this->_currRunningExecutableDataList[i]);
        if (casted == NULL) {
            WLog("Cannot cast - serious issue!!!");
            isTerminationFailed = true;
            continue;
        }

        pid_t pid;
        ProcessSerialNumber psn = casted->getRelatedProcess();
        if (KillProcess(&psn) != noErr) {
            WLog("Cannot terminate!!!");
            isTerminationFailed = true;
            continue;
        } else {
            WLog("Cannot get process id for PSN!!!");
            isTerminationFailed = true;
            continue;
        }
    }

    if(isTerminationFailed) {
        applicationTerminated = false;
    } else {
        applicationTerminated = true;
    }
#endif

#ifdef Q_WS_WIN
    bool isTerminationFailed = false;
    for(int i = 0; i < _currRunningExecutableDataList.size() && !isTerminationFailed; i++)
    {
        Win_RunningExecutableData *casted = dynamic_cast<Win_RunningExecutableData *>(this->_currRunningExecutableDataList[i]);
        if(casted == NULL) {
            WLog("Cannot cast - serious issue!!!");
            isTerminationFailed = true;
            continue;
        }

        // get the current item's related process-ids
        QList<DWORD> currRelatedProcIds = casted->getRelatedProcessIds();
        int numberOfProcessesToTerminate = currRelatedProcIds.size();
        //    QList<DWORD> remainingProcessIdsToTerminate;
        if(numberOfProcessesToTerminate > 0) {
            for(int i = 0; i < numberOfProcessesToTerminate && !isTerminationFailed; i++)
            {
                if(TerminateApp(currRelatedProcIds[i], WIN_TERMINATION_TIMEOUT_MILLISECONDS) == TA_FAILED) {
                    // this item will remain in the list
                    // have to retry it later
                    //                remainingProcessIdsToTerminate.append(this->theProcessIdsToTerminate[i]);
                    isTerminationFailed = true;
                }
            }

            //        if(remainingProcessIdsToTerminate.size() == 0) {
            //            applicationTerminated = true;
            //        }

            //        this->theProcessIdsToTerminate = remainingProcessIdsToTerminate;
        }
        else {
            // no ids were specified for termination, so we're done
//            applicationTerminated = true;
            WLog("The related process-id list is empty. Won't crash the app but it should never happen!");
            isTerminationFailed = true;
        }
    }

    if(isTerminationFailed) {
        applicationTerminated = false;
    } else {
        applicationTerminated = true;
    }
#endif

    if (applicationTerminated) {
        this->_currentApplicationTerminated();
    } else {
        WLog("Failed to terminate application");
        this->_currentApplicationTerminationFailed();
    }
}

void AppTerminationController::_checkIfCurrentApplicationIsStillRunning() {
    bool applicationDidTerminatedByUser = false;

#ifdef Q_OS_MAC
    bool isFailed = false;
    bool isFoundRunningProcess = false;
    for(int i = 0; i < _currRunningExecutableDataList.size() && !isFailed && !isFoundRunningProcess; i++)
    {
        MacProcess *casted = dynamic_cast<MacProcess *>(this->_currRunningExecutableDataList[i]);
        if(casted == NULL) {
            WLog("Cannot cast - serious issue!!!");
            isFailed = true;
            continue;
        }

        pid_t pid;
        ProcessSerialNumber psn = casted->getRelatedProcess();
        OSStatus status;
        status = GetProcessPID(&psn, &pid);
        if (status == noErr) {
            isFoundRunningProcess = true;
        }
    }

    if(!isFoundRunningProcess && !isFailed) {
        applicationDidTerminatedByUser = true;
    }
#endif

#ifdef Q_WS_WIN
    bool isFailed = false;
    bool isFoundRunningProcess = false;
    for(int i = 0; i < _currRunningExecutableDataList.size() && !isFailed && !isFoundRunningProcess; i++)
    {
        Win_RunningExecutableData *casted = dynamic_cast<Win_RunningExecutableData *>(this->_currRunningExecutableDataList[i]);
        if(casted == NULL) {
            WLog("Cannot cast - serious issue!!!");
            isFailed = true;
            continue;
        }

        if(PlatformSpecificProgramIdentifierHelper::getAllProcessIdsByExePath(casted->getRelatedExecutablePath()).size() != 0)
        {
            isFoundRunningProcess = true;
        }
    }

    if(!isFoundRunningProcess && !isFailed) {
        applicationDidTerminatedByUser = true;
    }
#endif

    if (applicationDidTerminatedByUser) {
        this->_currentApplicationTerminated();
    }
}


#ifdef Q_WS_WIN
// ---------------------------- WINDOWS HELPER METHODS ----------------------------

//
// from: http://support.microsoft.com/kb/178893
//

//   #include "TermApp.h"
//   #include <vdmdbg.h>

typedef struct
{
    DWORD   dwID ;
    DWORD   dwThread ;
} TERMINFO ;

// Declare Callback Enum Functions.
BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam ) ;

//BOOL CALLBACK Terminate16AppEnum( HWND hwnd, LPARAM lParam ) ;

/*----------------------------------------------------------------
   DWORD WINAPI TerminateApp( DWORD dwPID, DWORD dwTimeout )

   Purpose:
      Shut down a 32-Bit Process (or 16-bit process under Windows 95)

   Parameters:
      dwPID
         Process ID of the process to shut down.

      dwTimeout
         Wait time in milliseconds before shutting down the process.

   Return Value:
      TA_FAILED - If the shutdown failed.
      TA_SUCCESS_CLEAN - If the process was shutdown using WM_CLOSE.
      TA_SUCCESS_KILL - if the process was shut down with
         TerminateProcess().
      NOTE:  See header for these defines.
   ----------------------------------------------------------------*/
DWORD WINAPI TerminateApp( DWORD dwPID, DWORD dwTimeout )
{
    HANDLE   hProc ;
    DWORD   dwRet ;

    // If we can't open the process with PROCESS_TERMINATE rights,
    // then we give up immediately.
    hProc = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE,
                        dwPID);

    if(hProc == NULL)
    {
        return TA_FAILED ;
    }

    // TerminateAppEnum() posts WM_CLOSE to all windows whose PID
    // matches your process's.
    EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM) dwPID) ;

    // Wait on the handle. If it Q_SIGNALS, great. If it times out,
    // then you kill it.
    if(WaitForSingleObject(hProc, dwTimeout)!=WAIT_OBJECT_0)
        dwRet=(TerminateProcess(hProc,0)?TA_SUCCESS_KILL:TA_FAILED);
    else
        dwRet = TA_SUCCESS_CLEAN ;

    CloseHandle(hProc) ;

    return dwRet ;
}

BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam )
{
    DWORD dwID ;

    GetWindowThreadProcessId(hwnd, &dwID) ;

    if(dwID == (DWORD)lParam)
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0) ;
    }

    return TRUE ;
}

/*----------------------------------------------------------------
   DWORD WINAPI Terminate16App( DWORD dwPID, DWORD dwThread,
                        WORD w16Task, DWORD dwTimeout )

   Purpose:
      Shut down a Win16 APP.

   Parameters:
      dwPID
         Process ID of the NTVDM in which the 16-bit application is
         running.

      dwThread
         Thread ID of the thread of execution for the 16-bit
         application.

      w16Task
         16-bit task handle for the application.

      dwTimeout
         Wait time in milliseconds before shutting down the task.

   Return Value:
      If successful, returns TA_SUCCESS_16
      If unsuccessful, returns TA_FAILED.
      NOTE:  These values are defined in the header for this
      function.

   NOTE:
      You can get the Win16 task and thread ID through the
      VDMEnumTaskWOW() or the VDMEnumTaskWOWEx() functions.
   ----------------------------------------------------------------*/
//DWORD WINAPI Terminate16App( DWORD dwPID, DWORD dwThread,
//                            WORD w16Task, DWORD dwTimeout )
//{
//    HINSTANCE      hInstLib ;
//    TERMINFO      info ;

//    // You will be calling the functions through explicit linking
//    // so that this code will be binary compatible across
//    // Win32 platforms.
//    BOOL (WINAPI *lpfVDMTerminateTaskWOW)(DWORD dwProcessId, WORD htask);

//    hInstLib = LoadLibraryA( "VDMDBG.DLL" ) ;
//    if( hInstLib == NULL )
//        return TA_FAILED ;

//    // Get procedure addresses.
//    lpfVDMTerminateTaskWOW = (BOOL (WINAPI *)(DWORD, WORD ))
//            GetProcAddress( hInstLib, "VDMTerminateTaskWOW" ) ;

//    if( lpfVDMTerminateTaskWOW == NULL )
//    {
//        FreeLibrary( hInstLib ) ;
//        return TA_FAILED ;
//    }

//    // Post a WM_CLOSE to all windows that match the ID and the
//    // thread.
//    info.dwID = dwPID ;
//    info.dwThread = dwThread ;
//    EnumWindows((WNDENUMPROC)Terminate16AppEnum, (LPARAM) &info) ;

//    // Wait.
//    Sleep( dwTimeout ) ;

//    // Then terminate.
//    lpfVDMTerminateTaskWOW(dwPID, w16Task) ;

//    FreeLibrary( hInstLib ) ;
//    return TA_SUCCESS_16 ;
//}


//BOOL CALLBACK Terminate16AppEnum( HWND hwnd, LPARAM lParam )
//{
//    DWORD      dwID ;
//    DWORD      dwThread ;
//    TERMINFO   *termInfo ;

//    termInfo = (TERMINFO *)lParam ;

//    dwThread = GetWindowThreadProcessId(hwnd, &dwID) ;

//    if(dwID == termInfo->dwID && termInfo->dwThread == dwThread )
//    {
//        PostMessage(hwnd, WM_CLOSE, 0, 0) ;
//    }

//    return TRUE ;
//}

#endif
