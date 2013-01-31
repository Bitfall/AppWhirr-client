//*********************************************************************
// INCLUDES
//*********************************************************************

#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include <winsvc.h>
#include <process.h>

//*********************************************************************
// VARIABLES
//*********************************************************************

const int	nBufferSize = 500;
TCHAR		pServiceName[nBufferSize + 1];
TCHAR		pExeFile[nBufferSize + 1];
TCHAR		pInitFile[nBufferSize + 1];
TCHAR		pLogFile[nBufferSize + 1];
int			nProcCount = 0;

PROCESS_INFORMATION		* pProcInfo = 0;

SERVICE_STATUS          serviceStatus; 
SERVICE_STATUS_HANDLE   hServiceStatusHandle; 

//*********************************************************************
// FUNCTIONS
//*********************************************************************

