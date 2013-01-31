/********************************************************************
	created:	2005/03/25
	created:	25:3:2005   12:53
	filename: 	InstallRegistry.h
	file path:	Updater
	file base:	InstallRegistry
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef INSTALLREGISTRY_H
#define INSTALLREGISTRY_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"
#include "BackUp.h"

//*********************************************************************
// CInstallRegistry
//*********************************************************************

class CInstallRegistry : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	int GetActionCount();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CInstallRegistry)
	CInstallRegistry();           // protected constructor used by dynamic creation
	virtual ~CInstallRegistry();

	// Functions
	void InstallRegistryItem(int iRegistryItem);
	void InstallNextRegistryItem();
	void SendInstallStatus();
	int	 CalculateInstallActions();
	int  CalculateBackUpActions();
	void PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnInstallStart(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRegistryItemComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBackUpComplete(WPARAM wParam, LPARAM lParam);
	
	// Variables
	CWinThread   * m_pParent;
	InstallStatus  m_InstallStatus;
	int			   m_iCurrentRegistryItem;
	CUpdateInfo  * m_pUpdateInfo;
	CFunctions   * m_pFunctions;
	CLanguage    * m_pLanguage;
	CBackUp      * m_pBackupThread;
	CPath		 * m_pPath;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // INSTALLREGISTRY_H
