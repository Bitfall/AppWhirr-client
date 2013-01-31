/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:58
	filename: 	Protection.h
	file path:	Updater
	file base:	Protection
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef PROTECTION_H
#define PROTECTION_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "Internet.h"

//*********************************************************************
// CProtection
//*********************************************************************

class CProtection : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CProtection)
	CProtection();
	virtual ~CProtection();

	// Functions
	void Main();
	bool CheckRegistration();
	bool CheckHashes();
	void PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnDownloadComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnThreadMessage(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread    * m_pParent;
	bool		    m_bStarted;
	CFunctions    * m_pFunctions;
	CSettings     * m_pSettings;
	CUpdateInfo   * m_pUpdateInfo;
	CInternet	  * m_pInternet;
	CPath		  * m_pPath;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // PROTECTION_H