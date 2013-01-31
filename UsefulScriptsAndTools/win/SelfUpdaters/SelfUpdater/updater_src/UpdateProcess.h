/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   9:26
	filename: 	UpdateProcess.h
	file path:	Updater
	file base:	UpdateProcess
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef UPDATEPROCESS_H
#define UPDATEPROCESS_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"

// User interfaces
#include "ModeHidden.h"
#include "ModeSilent.h"
#include "ModeFullSheet.h"

//*********************************************************************
// CUpdateProcess
//*********************************************************************

class CUpdateProcess : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CUpdateProcess)
	CUpdateProcess(); 
	virtual ~CUpdateProcess();

	// Functions
	void StartTask(int iTask = TASK_NEXTTASK);
	bool CheckVersion();
	bool Protect();
	bool SelectUpdate();
	bool Download();
	bool Install();
	void Rollback();
	void FinishUpdate();
	void SetMode(CString sMode);
	void DestroyUserInterface();
	void SendMessageToUI(UINT message, WPARAM wParam, LPARAM lParam, bool bSaveMessage = true);
	void HandleEvent(CEventData * pEventData);
	bool CreateShortcut(CString sTargetFile, CString sParameters, CString sLinkFile,
						CString sDescription, int iShowMode, CString sWorkingDirectory,
						CString sIconFile, int iIconIndex);
	afx_msg void OnThreadMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTaskComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWarning(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWarningComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnQuestion(WPARAM wParam, LPARAM lParam);
	afx_msg void OnQuestionAnswer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateGUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateGUIFileProgress(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateGUITotalProgress(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateGUIProtection(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSwitchUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowAbout(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread  * m_pCurrentThread;
	int			  m_iCurrentTask;
	CWnd		* m_pCurrentUI;
	CSettings   * m_pSettings;
	CLanguage   * m_pLanguage;
	CUpdateInfo * m_pUpdateInfo;
	CFunctions  * m_pFunctions;
	CEventInfo  * m_pEventInfo;
	CPath		* m_pPath;
	CLog		* m_pLog;
	CString		  m_sMode;
	MessageStore  m_oLastUIMessage;
	bool		  m_bFinished;
	bool		  m_bRollback;
	bool		  m_bInstallComplete;
	bool		  m_bAllowUIChange;
	bool		  m_bUpdateCancelled;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // UPDATEPROCESS_H
