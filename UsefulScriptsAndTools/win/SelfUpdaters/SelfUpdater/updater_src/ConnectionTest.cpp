/********************************************************************
	created:	2005/02/22
	created:	22:2:2005   13:09
	filename: 	ConnectionTest.cpp
	file path:	Updater
	file base:	ConnectionTest
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	THANKS TO NEVILLE FRANKS FOR SHARING THIS CODE!
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "ConnectionTest.h"
#include <afxinet.h>

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

bool CConnectionTest::Connected()
{
	// Declare variables
	int iDial;
	bool bResult = false;

	iDial = AutoDialGet();
    if (iDial == TRUE)   // If AutoDial is on then turn it off
        AutoDialSet(FALSE);

    // This can be a Socket connect or a higher level HTTP etc. call,
    // Note that a Socket will succeed even if we are offline, whereas HTTP doesn't.
    // See notes above.
	InitWinsock();

	// Test connection
	if (TestConnection(_T("http://www.microsoft.com")))
		bResult = true;

	if (TestConnection(_T("http://www.google.com")))
		bResult = true;

	// Clean up windsock
	UninitWinsock();

    if (iDial == TRUE)   // If AutoDial was on then turn it back on
        AutoDialSet(TRUE);

    return bResult;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CConnectionTest::InitWinsock()
{
	// Declare variables
	WORD sockVersion;
	WSADATA wsaData;

	// Get winsock version
	sockVersion = MAKEWORD(1, 1);

	// Call first function needed for winsock
	WSAStartup(sockVersion, &wsaData);
}

//=====================================================================

void CConnectionTest::UninitWinsock()
{	
	// Cleanup winsock
	WSACleanup();
}

//=====================================================================

bool CConnectionTest::AutoDialSet(const BOOL bOn)
{
	// Declare variables
	HKEY hKey;
	DWORD dwDial = -1;
	bool bStat = false;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, gszInternetSettingsSubKey,
                     0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
		const int iDialSize = 4;
		DWORD dwDialType = REG_BINARY, dwDialSize = iDialSize;
		BYTE byDial[iDialSize];
    
        // We only want to change byte 0 so get the current 4 bytes
        if (RegQueryValueEx(hKey, gszEnableAutodialItem, NULL, &dwDialType, (LPBYTE)&byDial, &dwDialSize) == ERROR_SUCCESS)
        {
            // Users may already have dodgy Reg Entries so don't do this in public
            // ASSERT( dwDialType == REG_BINARY && dwDialSize == 4 );    // Let us know if MS ever change how/what's stored
            
            // REG_BINARY is what windows uses, however some apps can
             // incorrectly change it to REG_DWORD.
            if ((dwDialType != REG_BINARY) || (dwDialSize != 4))
			{
				// If it has been changed to REG_DWORD we may as well leave it that way as our code works eitehr way.
				if (dwDialType == REG_DWORD)
					ZeroMemory(byDial, iDialSize);
			}

            // This works correctly for both REG_BINARY and REG_DWORD. For REG_DWORD we get 0x00000001
            byDial[0] = bOn ? 1 : 0;    // turn it on or off

            if (RegSetValueEx(hKey, gszEnableAutodialItem, NULL, dwDialType, (LPBYTE)&byDial, dwDialSize) == ERROR_SUCCESS)
                bStat = true;
        }
        RegCloseKey(hKey);
    }

    return bStat;
}

//=====================================================================

int CConnectionTest::AutoDialGet()
{
	// Declare variables
	HKEY hKey;
	int nDial = -1;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, gszInternetSettingsSubKey,
                     0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
		const int iDialSize = 4;
		DWORD dwDialType = REG_BINARY, dwDialSize = iDialSize;
		BYTE byDial[iDialSize];

        if (RegQueryValueEx(hKey, gszEnableAutodialItem, NULL, &dwDialType, (LPBYTE)&byDial, &dwDialSize) == ERROR_SUCCESS)
        {
            // Users may already have dodgy Reg Entries so don't do this in public
            // ASSERT( dwDialType == REG_BINARY && dwDialSize == 4 );  // Let us know if MS ever change how/what's stored

            // See comments in AutoDialSet()
            if ((dwDialType == REG_BINARY) && (dwDialSize == 4))
                nDial = byDial[0] == 0 ? 0 : 1;
            else
			{
				if (dwDialType == REG_DWORD )
					nDial = byDial[0] == 0 ? 0 : 1;   // for REG_DWORD Low Byte is also what we want.
			}
        }
        RegCloseKey( hKey );
    }

    return nDial;
}

//=====================================================================

bool CConnectionTest::TestConnection(CString sURL)
{
	USES_CONVERSION;

	// Declare variables
	LPHOSTENT hostEntry;
	SOCKADDR_IN serverInfo;
	int iResult;
	SOCKET testSocket;

	// Get information about server
	hostEntry = gethostbyname(W2A(sURL));

	// Check if server information is retrieved successfully
	if (!hostEntry)
		return false;

	// Setup socket	
	testSocket = socket(AF_INET,		// Use TCP/IP
				   		SOCK_STREAM,	// Stream-oriented
						IPPROTO_TCP);	// Use TCP protocol


	// Check if socket is created
	if (testSocket == INVALID_SOCKET)
	{
		// Close socket
		closesocket(testSocket);
		return false;
	}

	// Fill in address information
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
	serverInfo.sin_port = htons(INTERNET_DEFAULT_HTTP_PORT);

	// Now connect to server
	iResult = connect(testSocket, (LPSOCKADDR)&serverInfo, sizeof(struct sockaddr));

	// Close socket
	closesocket(testSocket);

	// Check result
	if (iResult == SOCKET_ERROR)
		return false;

	// We did it!
	return true;
}
