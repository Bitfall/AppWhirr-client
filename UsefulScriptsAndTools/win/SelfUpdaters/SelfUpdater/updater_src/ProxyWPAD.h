/********************************************************************
	created:	2005/04/25
	created:	25:4:2005   15:18
	filename: 	ProxyWPAD.h
	file path:	Updater
	file base:	ProxyWPAD
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef PROXYWPAD_H
#define PROXYWPAD_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CProxyWPAD
//*********************************************************************

class CProxyWPAD  
{
public:
	// Constructor & destructor
	// None, because of static class

	// Functions
	static bool GetWPADProxyInfo(CString sURL, CString sWPADURL, ProxyData * pProxyInfo);

private:
	// Functions

	// Variables
};

// End of redefinition protection
#endif // PROXYWPAD_H
