/********************************************************************
	created:	2005/03/25
	created:	25:3:2005   12:53
	filename: 	InstallFiles.h
	file path:	Updater
	file base:	InstallFiles
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef INSTALLFILES_H
#define INSTALLFILES_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"

#include "CopyFile.h"				// Support for copying files
#include "BackUp.h"					// Backup support

//*********************************************************************
// CONSTANTS
//*********************************************************************

//*********************************************************************
// CInstallFiles
//*********************************************************************

class CInstallFiles : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	int GetActionCount();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CInstallFiles)
	CInstallFiles();           // protected constructor used by dynamic creation
	virtual ~CInstallFiles();

	// Functions
	void BackUpObject();
	void InstallFile(int iFile);
	void InstallNextFile();
	void PerformAction(int iAction);
	void PerformNextAction();
	void PerformActionCopy(CFileData * pFileData, int iActionIndex);
	void PerformActionDelete(CFileData * pFileData, int iActionIndex);
	void PerformActionRun(CFileData * pFileData, int iActionIndex);
	void PerformActionUnzip(CFileData * pFileData, int iActionIndex);
	void PerformActionSetFileAttr(CFileData * pFileData, int iActionIndex);
	void PerformActionRegister(CFileData * pFileData, int iActionIndex);
	void ContinueActionDelete();
	void FinishActionDelete();
	bool ContinueActionRegister();
	void FinishActionRegister();
	void SendInstallStatus();
	void PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	int  CalculateActionsOfFile(int iFile);
	int  CalculateActionsOfAction(int iFile, int iAction);
	int  CalculateBackupActionsOfAction(int iFile, int iAction);
	afx_msg void OnInstallStart(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFileComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnActionComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBackUpStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBackUpComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWarningComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnQuestionAnswer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetParent(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread   * m_pParent;
	InstallStatus  m_InstallStatus;
	int			   m_iCurrentFile;
	int		   	   m_iCurrentAction;
	CCopyFile    * m_pCopyThread;
	CBackUp      * m_pBackupThread;
	CUpdateInfo  * m_pUpdateInfo;
	CFunctions   * m_pFunctions;
	CLanguage    * m_pLanguage;
	CPath		 * m_pPath;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // INSTALLFILES_H
