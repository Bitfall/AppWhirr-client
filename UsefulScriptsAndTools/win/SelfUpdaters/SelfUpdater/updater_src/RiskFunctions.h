/********************************************************************
	created:	2005/11/18
	created:	18:11:2005   19:22
	filename: 	RiskFunctions.h
	file path:	Updater
	file base:	RiskFunctions
	file ext:	h
	author:		François Le Luhern
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef RISKFUNCTIONS_H
#define RISKFUNCTIONS_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <string>
#include <Tlhelp32.h>

//*********************************************************************
// ENUMS
//*********************************************************************

enum
{
	//
	WINDOWS_UNKNOWN = -1,
	WINDOWS_95 = 1,
	WINDOWS_98,
	WINDOWS_ME,
	WINDOWS_NT3,
	WINDOWS_NT4,
	WINDOWS_2000,
	WINDOWS_XP,
	WINDOWS_2003,
};

enum
{
	//
	WINDOWS_95_OSR2 = WINDOWS_95 + 256,
	WINDOWS_98_SE   = WINDOWS_98 + 256,
};

typedef  HRESULT (CALLBACK* LPSHGETFOLDERPATH)( HWND, int, HANDLE, DWORD, LPTSTR );
extern LPSHGETFOLDERPATH lpSHGetFolderPath;

//*********************************************************************
// FUNCTIONS
//*********************************************************************

void exitDLL();
void initDLL();
UINT getWinVersion( std::string& strVersion, UINT& uWinPlatform );
bool NTApplicationRunning( TCHAR * strApplName );
bool AllWinApplicationRunning( TCHAR * strApplName );
bool CloseNTApplication( TCHAR * strApplName, bool bForce = false );
bool CloseAllWinApplication( TCHAR * strApplName, bool bForce = false );

// End of redefinition protection
#endif // RISKFUNCTIONS_H
