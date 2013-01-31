/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:59
	filename: 	Install.h
	file path:	Updater
	file base:	Install
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef INSTALL_H
#define INSTALL_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"

#include "InstallFiles.h"
#include "InstallIni.h"
#include "InstallRegistry.h"

//*********************************************************************
// CInstall
//*********************************************************************

class CInstall : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CInstall)
	CInstall();
	virtual ~CInstall();

	// Functions
	void InstallPart(int iPart);
	void InstallNextPart();
	void ExtractUpdaterCopy();
	void PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnInstallStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInstallComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWarning(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWarningComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnQuestion(WPARAM wParam, LPARAM lParam);
	afx_msg void OnQuestionAnswer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnThreadMessage(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread       * m_pParent;
	CInstallFiles    * m_pInstallFiles;
	CInstallIni      * m_pInstallIni;
	CInstallRegistry * m_pInstallRegistry;
	bool		       m_bStarted;
	int				   m_iCurrentPart;
	int				   m_iActionCount;
	int				   m_iPreviousActionsPerformed;
	int				   m_iTotalActionsPerformed;
	int				   m_iFileProgress;
	int				   m_iTotalProgress;
	CFunctions       * m_pFunctions;
	CSettings        * m_pSettings;
	CUpdateInfo      * m_pUpdateInfo;
	CLanguage	     * m_pLanguage;
	CRollbackInfo    * m_pRollbackInfo;
	CPath			 * m_pPath;
	TCHAR	 	     * m_szFilename;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // INSTALL_H