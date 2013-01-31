/********************************************************************
	created:	2005/12/04
	created:	4:12:2005   23:00
	filename: 	Proxy.cpp
	file path:	Updater
	file base:	Proxy
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Proxy.h"
#include "ProxyInfo.h"
#include <wininet.h>
#include <afxinet.h>

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CProxy> CProxy::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CProxy::CProxy()
{
	// Set default values
	m_iProxyType = PROXY_AUTODETECT;
	m_sManualHttp = "";
	m_sManualFtp = "";
	m_sAutoConfigURL = "";
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CProxy * CProxy::Instance()
{
	if (sm_inst.get() == 0)
		sm_inst = auto_ptr<CProxy>(new CProxy);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
		sm_inst.reset(new CProxy);
	return sm_inst.get();
	*/
}

//=====================================================================

bool CProxy::GetProxyInfo(CString sURL, ProxyData * pProxyInfo)
{
	// Check which type to use
	switch (m_iProxyType)
	{
	case PROXY_DIRECTCONNECTION:
		return GetDirectConnection(sURL, pProxyInfo);

	case PROXY_AUTODETECT:
		return GetAutoDetect(sURL, pProxyInfo);

	case PROXY_MANUAL:
		return GetManual(sURL, pProxyInfo);

	case PROXY_AUTOCONFIGURL:
		return GetAutomaticConfigURL(sURL, pProxyInfo);

	default:
		// Auto detect
		return GetAutoDetect(sURL, pProxyInfo);
	}
}

//=====================================================================

void CProxy::SetType(int iProxyType)
{
	// Set value
	m_iProxyType = iProxyType;
}

//=====================================================================

void CProxy::SetManualServers(CString sHttp, CString sFtp)
{
	// Set value
	m_sManualHttp = sHttp;
	m_sManualFtp = sFtp;
}

//=====================================================================

void CProxy::SetAutoConfigURL(CString sURL)
{
	// Set value
	m_sAutoConfigURL = sURL;	
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

bool CProxy::GetDirectConnection(CString sURL, ProxyData * pProxyInfo)
{
	// Set direct connection
	pProxyInfo->dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
	pProxyInfo->sProxyName = "";
	pProxyInfo->sProxyBypass = "";

	// We always succeed
	return true;
}

//=====================================================================

bool CProxy::GetAutoDetect(CString sURL, ProxyData * pProxyInfo)
{
	// Auto detect settings
	return CProxyInfo::GetProxyInfo(sURL, pProxyInfo);
}

//=====================================================================

bool CProxy::GetManual(CString sURL, ProxyData * pProxyInfo)
{
	// Declare variables
	CString sServer, sObject;
	INTERNET_PORT nPort;
	DWORD dwServiceType;

	// Try to parse URL
	AfxParseURL(sURL, dwServiceType, sServer, sObject, nPort);

	// Always use proxy type
	pProxyInfo->dwAccessType = INTERNET_OPEN_TYPE_PROXY;

	// Use right manual proxy server
	switch (dwServiceType)
	{
	case AFX_INET_SERVICE_HTTP:
		pProxyInfo->sProxyName = m_sManualHttp;
		pProxyInfo->sProxyBypass = m_sManualHttp;
		break;

	case AFX_INET_SERVICE_FTP:
		pProxyInfo->sProxyName = m_sManualFtp;
		pProxyInfo->sProxyBypass = m_sManualFtp;
		break;

	default:
		// Use HTTP
		pProxyInfo->sProxyName = m_sManualHttp;
		pProxyInfo->sProxyBypass = m_sManualHttp;
		break;
	}

	// We have succeeded
	return true;
}

//=====================================================================

bool CProxy::GetAutomaticConfigURL(CString sURL, ProxyData * pProxyInfo)
{
	/* NOT IMPLEMENTED YET */
	return GetAutoDetect(sURL, pProxyInfo);
}