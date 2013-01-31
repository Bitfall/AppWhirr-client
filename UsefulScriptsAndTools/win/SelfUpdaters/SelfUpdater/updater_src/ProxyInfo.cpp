/********************************************************************
	created:	2005/04/25
	created:	25:4:2005   15:07
	filename: 	Proxy.cpp
	file path:	Updater
	file base:	ProxyInfo
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ProxyInfo.h"
#include <afxinet.h>
#include "ProxyWPAD.h"

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

bool CProxyInfo::GetProxyInfo(CString sURL, ProxyData * pProxyInfo)
{
	// Declare variables
	INTERNET_PER_CONN_OPTION_LIST List;
	INTERNET_PER_CONN_OPTION Option[5];
	unsigned long nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
	CString sLog;
	CFunctions * pFunctions = CFunctions::Instance();
	CSettings * pSettings = CSettings::Instance();
	CLog * pLog = CLog::Instance();

	// Set default values for pProxyInfo
	pProxyInfo->dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;
	//pProxyInfo->sProxyName = NULL;
	//pProxyInfo->sProxyBypass = NULL;
	
	// Initialize options
	Option[0].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
	Option[1].dwOption = INTERNET_PER_CONN_AUTODISCOVERY_FLAGS;
	Option[2].dwOption = INTERNET_PER_CONN_FLAGS;
	Option[3].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	Option[4].dwOption = INTERNET_PER_CONN_PROXY_SERVER;

	// Initialize list
	List.dwSize = nSize;
	List.pszConnection = NULL;
	List.dwOptionCount = 5;
	List.dwOptionError = 0;
	List.pOptions = Option;

	// Get proxy information
	if (InternetQueryOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize))
	{
		// Do we have a valid auto config URL?
		if (Option[0].Value.pszValue != NULL)
		{
			// Get special WPAD proxy info
			CProxyWPAD::GetWPADProxyInfo(sURL, Option[0].Value.pszValue, pProxyInfo);
		}
		else
		{
			// Set proxy info
			pProxyInfo->dwAccessType = Option[2].Value.dwValue;
			pProxyInfo->sProxyName = Option[4].Value.pszValue;
			pProxyInfo->sProxyBypass = Option[3].Value.pszValue;
		}

		// Make sure to pick the fastest connection (direct)
		//pProxyInfo->dwAccessType |= PROXY_TYPE_AUTO_DETECT;
	}

	// Free objects
	if (Option[0].Value.pszValue != NULL)
		GlobalFree(Option[0].Value.pszValue);
	if (Option[3].Value.pszValue != NULL)
		GlobalFree(Option[3].Value.pszValue);
	if (Option[4].Value.pszValue != NULL)
		GlobalFree(Option[4].Value.pszValue);

	// Return value is not used (yet)
	return true;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************
