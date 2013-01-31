/********************************************************************
	created:	2005/12/04
	created:	4:12:2005   23:00
	filename: 	Proxy.h
	file path:	Updater
	file base:	Proxy
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef PROXY_H
#define PROXY_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

//*********************************************************************
// CONSTANTS
//*********************************************************************

const int PROXY_DIRECTCONNECTION = 0;
const int PROXY_AUTODETECT = 1;
const int PROXY_MANUAL = 2;
const int PROXY_AUTOCONFIGURL = 3;

//*********************************************************************
// CProxy
//*********************************************************************

class CProxy  
{
public:
	// Functions
	static  CProxy * Instance();
	bool	GetProxyInfo(CString sURL, ProxyData * pProxyInfo);
	void	SetType(int iProxyType);
	void	SetManualServers(CString sHttp, CString sFtp);
	void	SetAutoConfigURL(CString sURL);

private:
	// Constructor & destructor
	CProxy();			// Private constructor because singleton!

	// Functions
	bool GetDirectConnection(CString sURL, ProxyData * pProxyInfo);
	bool GetAutoDetect(CString sURL, ProxyData * pProxyInfo);
	bool GetManual(CString sURL, ProxyData * pProxyInfo);
	bool GetAutomaticConfigURL(CString sURL, ProxyData * pProxyInfo);

	// Variables
	static std::auto_ptr<CProxy> sm_inst;
	int		m_iProxyType;
	CString m_sManualHttp;
	CString m_sManualFtp;
	CString m_sAutoConfigURL;
};

// End of redefinition protection
#endif // PROXY_H