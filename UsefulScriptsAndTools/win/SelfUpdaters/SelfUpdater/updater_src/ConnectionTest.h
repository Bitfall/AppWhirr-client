/********************************************************************
	created:	2005/02/22
	created:	22:2:2005   13:09
	filename: 	ConnectionTest.h
	file path:	Updater
	file base:	ConnectionTest
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef CONNECTIONTEST_H
#define CONNECTIONTEST_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <afxsock.h>

// Constants
static const TCHAR gszInternetSettingsSubKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings");
static const TCHAR gszEnableAutodialItem[] = _T("EnableAutodial");

//*********************************************************************
// CConnectionInfo
//*********************************************************************

class CConnectionTest  
{
public:
	// Constructor & destructor
	// No constructor & destructor used, because functions are static

	// Functions
	static bool Connected();

private:
	// Functions
	static void InitWinsock();
	static void UninitWinsock();
	static bool AutoDialSet(const BOOL bOn);
	static int  AutoDialGet();
	static bool TestConnection(CString sURL);

	// Variables
};

// End of redefinition protection
#endif // CONNECTIONTEST_H