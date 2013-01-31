/********************************************************************
	created:	2005/04/25
	created:	25:4:2005   15:02
	filename: 	ProxyInfo.h
	file path:	Updater
	file base:	ProxyInfo
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef PROXYINFO_H
#define PROXYINFO_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CProxy
//*********************************************************************

class CProxyInfo 
{
public:
	// Constructor & destructor
	// None, because of static class

	// Functions
	static bool GetProxyInfo(CString sURL, ProxyData * pProxyInfo);

private:
	// Functions

	// Variables

};

// End of redefinition protection
#endif // PROXYINFO_H
