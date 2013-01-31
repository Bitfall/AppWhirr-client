/********************************************************************
	created:	2005/11/18
	created:	18:11:2005   19:24
	filename: 	RiskFunctions.cpp
	file path:	Updater
	file base:	RiskFunctions
	file ext:	cpp
	author:		François Le Luhern
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "RiskFunctions.h"

/*typedef struct tagPROCESSENTRY32
{
	DWORD   dwSize;
	DWORD   cntUsage;
	DWORD   th32ProcessID;          // this process
	ULONG_PTR th32DefaultHeapID;
	DWORD   th32ModuleID;           // associated exe
	DWORD   cntThreads;
	DWORD   th32ParentProcessID;    // this process's parent process
	LONG    pcPriClassBase;         // Base priority of process's threads
	DWORD   dwFlags;
	CHAR    szExeFile[MAX_PATH];    // Path
} PROCESSENTRY32;
typedef PROCESSENTRY32 *  PPROCESSENTRY32;
typedef PROCESSENTRY32 *  LPPROCESSENTRY32;

#ifdef UNICODE
#define Process32First Process32FirstW
#define Process32Next Process32NextW
#define PROCESSENTRY32 PROCESSENTRY32W
#define PPROCESSENTRY32 PPROCESSENTRY32W
#define LPPROCESSENTRY32 LPPROCESSENTRY32W
#endif  // !UNICODE

#define TH32CS_SNAPPROCESS  0x00000002*/

//=====================================================================

HMODULE hSHlib = NULL;

LPSHGETFOLDERPATH lpSHGetFolderPath = NULL;


HMODULE hK32lib = NULL;

typedef HANDLE (CALLBACK* LPCREATETOOLHELP32SNAPSHOT)(DWORD ,DWORD );
LPCREATETOOLHELP32SNAPSHOT lpCreateToolhelp32Snapshot = NULL;

typedef UINT (CALLBACK* LPPROCESS32FIRST)(HANDLE ,LPPROCESSENTRY32 );
LPPROCESS32FIRST lpProcess32First = NULL;

typedef UINT (CALLBACK* LPPROCESS32NEXT)(HANDLE ,LPPROCESSENTRY32 );
LPPROCESS32NEXT lpProcess32Next = NULL;


//=====================================================================
// PSAPI.DLL -> WINNT4

HMODULE hPSlib = NULL;

typedef UINT (CALLBACK* LPENUMPROCESSES)(DWORD* ,DWORD, DWORD*);
LPENUMPROCESSES lpEnumProcesses = NULL;

typedef UINT (CALLBACK* LPENUMPROCESSMODULES)(HANDLE ,HMODULE*, DWORD, LPDWORD );
LPENUMPROCESSMODULES lpEnumProcessModules = NULL;

typedef UINT (CALLBACK* LPGETMODULEBASENAME)(HANDLE ,HMODULE, LPTSTR, DWORD );
LPGETMODULEBASENAME lpGetModuleBaseName = NULL;


//=====================================================================

void exitDLL()
{
	if( hSHlib != NULL )
		FreeLibrary( hSHlib );
	if( hK32lib != NULL )
		FreeLibrary( hK32lib );
	if( hK32lib != NULL )
		FreeLibrary( hPSlib );
	
}

//=====================================================================

void initDLL()
{
	USES_CONVERSION;

	atexit( exitDLL );

	hSHlib = LoadLibrary( _T("shfolder.dll") );
	if( hSHlib != NULL )
		lpSHGetFolderPath = (LPSHGETFOLDERPATH)GetProcAddress( hSHlib, W2A(_T("SHGetFolderPathA")) );

	hK32lib = LoadLibrary( _T("kernel32.dll") );
	if (hK32lib != NULL)
	{
		lpCreateToolhelp32Snapshot	= (LPCREATETOOLHELP32SNAPSHOT)GetProcAddress(	hK32lib, W2A(_T("CreateToolhelp32Snapshot")));
		lpProcess32First			= (LPPROCESS32FIRST)GetProcAddress(				hK32lib, W2A(_T("Process32First")));
		lpProcess32Next				= (LPPROCESS32NEXT)GetProcAddress(				hK32lib, W2A(_T("Process32Next")));
	}

	std::string strVersion;
	UINT uWinPlatform;
	getWinVersion( strVersion, uWinPlatform );
	if( uWinPlatform == VER_PLATFORM_WIN32_NT )
	{
		hPSlib = LoadLibrary( _T("psapi.dll") );

		if (hPSlib != NULL)
		{
			lpEnumProcesses		 = (LPENUMPROCESSES)GetProcAddress(		hPSlib, W2A(_T("EnumProcesses")));
			lpEnumProcessModules = (LPENUMPROCESSMODULES)GetProcAddress(hPSlib, W2A(_T("EnumProcessModules")));
			lpGetModuleBaseName  = (LPGETMODULEBASENAME)GetProcAddress(	hPSlib, W2A(_T("GetModuleBaseNameA")));
		}
	}
}

//=====================================================================

UINT getWinVersion( std::string& strVersion, UINT& uWinPlatform )
{
	strVersion = "<unknown>";
	UINT uVersion = WINDOWS_UNKNOWN;

	OSVERSIONINFO osVersion = { 0 };
	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &osVersion );

	switch( uWinPlatform = osVersion.dwPlatformId )
	{
	case VER_PLATFORM_WIN32_NT:
		switch( osVersion.dwMajorVersion )
		{
		case 3 :
			strVersion = "Windows NT 3";
			uVersion = WINDOWS_NT3;
			break;
		case 4 :
			strVersion = "Windows NT 4";
			uVersion = WINDOWS_NT4;
			break;
		case 5 :
			switch( osVersion.dwMinorVersion ) 
			{
			case 0:
				strVersion = "Windows 2000";
				uVersion = WINDOWS_2000;
				break;
			case 1:
				strVersion = "Windows XP";
				uVersion = WINDOWS_XP;
				break;
			case 2:
				strVersion = "Windows 2003";
				uVersion = WINDOWS_2003;
				break;
			}
			break;
		}
		break;

	case VER_PLATFORM_WIN32_WINDOWS :
		switch( osVersion.dwMinorVersion ) 
		{
		case 0:
			strVersion = "Windows 95";
			uVersion = WINDOWS_95;
			if( (osVersion.szCSDVersion[1] == _T('C')) || (osVersion.szCSDVersion[1] == _T('B')) )
			{
				strVersion += " OSR2";
				uVersion = WINDOWS_95_OSR2;
			}
			break;
		case 10:
			strVersion = "Windows 98";
			uVersion = WINDOWS_98;
			if( (osVersion.szCSDVersion[1] == _T('A')) )
			{
				strVersion += " SE";
				uVersion = WINDOWS_98_SE;
			}
			break;
		case 90:
			strVersion = "Windows Me";
			uVersion = WINDOWS_ME;
			break;
		}
	}
	return uVersion;
}

//=====================================================================

BOOL CALLBACK EnumWinHandle(HWND hWnd, LPARAM lParam)
{
	// Declare variables
	DWORD dwProcessID;

	// Check if this is the right window
	GetWindowThreadProcessId(hWnd, &dwProcessID);

	// Is it the same process?
	if (dwProcessID == lParam)
	{
		// Try to close application by sending it a message
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
	}

	// Keep enumerating
	return TRUE;
}

//=====================================================================

std::string GetProcessNameAndID( DWORD processID )
{
	USES_CONVERSION;

	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	std::string strProcess;

	// Get a handle to the process.

	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID );

	// Get the process name.

	if (NULL != hProcess )
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if ( lpEnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
		{
			lpGetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
		}

		strProcess = _strlwr(W2A(szProcessName));
		// _tprintf( TEXT(_T("%s  (PID: %u)\n")), szProcessName, processID );

		CloseHandle( hProcess );
	}
	return strProcess;
}

//=====================================================================

bool NTApplicationRunning(TCHAR * strApplName)
{
	USES_CONVERSION;

	// Get the list of process identifiers.

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	std::string strAppName = _strlwr(W2A(strApplName));

	if ( !lpEnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		return false;

	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.

	for ( i = 0; i < cProcesses; i++ )
	{
		std::string strProcess = GetProcessNameAndID( aProcesses[i] );

		if( strstr( strProcess.c_str(), strAppName.c_str() ) != NULL )
		{
			// Application is running
			return true;
		}
	}

	// Application is not running
	return false;
}

//=====================================================================

bool AllWinApplicationRunning( TCHAR* strApplName )
{
	// Declare variables
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {NULL};
	TCHAR szCompare[_MAX_PATH];

	hProcessSnap = lpCreateToolhelp32Snapshot 
		(TH32CS_SNAPPROCESS, 0);

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			// Copy process name
			lstrcpy(szCompare, pe32.szExeFile);

			// Compare processes
			if (lstrcmpi(szCompare, strApplName) == 0)
			{
				// Application is running
				return true;
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}

	// Application is not running
	return false;
}

//=====================================================================

bool CloseNTApplication( TCHAR* strApplName, bool bForce /* = false */ )
{
	USES_CONVERSION;

	// Check if application is running
	if (!NTApplicationRunning(strApplName))
		return true;

	// Application is closed
	CInternalData::Instance()->SetApplicationClosed(true);

	// Get the list of process identifiers.
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	std::string strAppName = _strlwr(W2A(strApplName));
	if ( !lpEnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		return false;

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.
	for ( i = 0; i < cProcesses; i++ )
	{
		std::string strProcess = GetProcessNameAndID( aProcesses[i] );

		if( strstr( strProcess.c_str(), strAppName.c_str() ) != NULL )
		{
			// Should we force the application to close?
			if (bForce)
			{
				// Get handle to process
				HANDLE hProcess = OpenProcess
					(PROCESS_ALL_ACCESS, FALSE, aProcesses[i]);

				// Terminate process
				DWORD exCode;
				GetExitCodeProcess(hProcess, &exCode);
				TerminateProcess(hProcess, exCode);
			}
			else
			{
				// Send message to the right window
				EnumWindows(EnumWinHandle, aProcesses[i]);
			}
		}
	}
	return true;
}

//=====================================================================

bool CloseAllWinApplication( TCHAR* strApplName, bool bForce /* = false */ )
{
	// Declare variables
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {NULL};
	TCHAR szCompare[_MAX_PATH];

	// Is app running?
	if (!AllWinApplicationRunning(strApplName))
		return true;

	// Application is closed
	CInternalData::Instance()->SetApplicationClosed(true);

	hProcessSnap = lpCreateToolhelp32Snapshot 
		(TH32CS_SNAPPROCESS, 0);

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			// Copy process name
			lstrcpy(szCompare, pe32.szExeFile);

			// Compare processes
			if (lstrcmpi(szCompare, strApplName) == 0)
			{
				// Should we force the application to close?
				if (bForce)
				{
					// Get handle to process
					HANDLE hProcess = OpenProcess
						(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

					// Terminate process
					DWORD exCode;
					GetExitCodeProcess(hProcess, &exCode);
					TerminateProcess(hProcess, exCode);
				}
				else
				{
					// Send message to the right window
					EnumWindows(EnumWinHandle,  pe32.th32ProcessID);
				}
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}

	// Application is not running
	return false;
}
