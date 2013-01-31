/********************************************************************
	created:	2005/04/25
	created:	25:4:2005   15:20
	filename: 	ProxyWPAD.cpp
	file path:	Updater
	file base:	ProxyWPAD
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ProxyWPAD.h"
#include <winhttp.h>

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

bool CProxyWPAD::GetWPADProxyInfo(CString sURL, CString sWPADURL, ProxyData * pProxyInfo)
{
	USES_CONVERSION;

	// Declare variables
	WINHTTP_AUTOPROXY_OPTIONS proxyOptions;
	WINHTTP_PROXY_INFO proxyInfo;

	// Zero memory
	ZeroMemory(&proxyOptions, sizeof(proxyOptions));
	ZeroMemory(&proxyInfo, sizeof(proxyInfo));

	// Open temp internet session
	HINTERNET hInternet = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_NO_PROXY,
									  WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
 
	// Set proxy options
	proxyOptions.lpszAutoConfigUrl = T2CW(LPCTSTR(sWPADURL));
	proxyOptions.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
	proxyOptions.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
	proxyOptions.dwReserved = NULL;
	proxyOptions.lpvReserved = 0;
	proxyOptions.fAutoLogonIfChallenged = TRUE;

	// Get proxy info
	if (!WinHttpGetProxyForUrl(hInternet, sURL,
							   &proxyOptions, &proxyInfo))
	{
		// Close temp internet session
		WinHttpCloseHandle(hInternet);

		// Failed
		return false;
	}

	// Copy data
	pProxyInfo->dwAccessType = proxyInfo.dwAccessType;
	pProxyInfo->sProxyName = proxyInfo.lpszProxy;
	pProxyInfo->sProxyBypass = proxyInfo.lpszProxyBypass;

	// Close temp internet session
	WinHttpCloseHandle(hInternet);

	// Free objects
	if (proxyInfo.lpszProxy != NULL)
		GlobalFree(proxyInfo.lpszProxy);
	if (proxyInfo.lpszProxyBypass != NULL)
		GlobalFree(proxyInfo.lpszProxyBypass);

	// Return value is not used 
	return true;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************