/********************************************************************
	created:	2006/03/02
	created:	2:3:2006   9:26
	filename: 	CheckForUpdatesProcess.h
	file path:	Updater
	file base:	CheckForUpdatesProcess
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef CHECKFORUPDATESPROCESS_H
#define CHECKFORUPDATESPROCESS_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"

//*********************************************************************
// CUpdateProcess
//*********************************************************************

class CCheckForUpdatesProcess : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CCheckForUpdatesProcess)
	CCheckForUpdatesProcess(); 
	virtual ~CCheckForUpdatesProcess();

	// Functions
	void StartTask(int iTask = TASK_NEXTTASK);
	bool CheckVersion();
	afx_msg void OnThreadMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTaskComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);
	afx_msg void OnQuestion(WPARAM wParam, LPARAM lParam);
	afx_msg void OnQuestionAnswer(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread  * m_pCurrentThread;
	CSettings   * m_pSettings;
	CLanguage   * m_pLanguage;
	CUpdateInfo * m_pUpdateInfo;
	CFunctions  * m_pFunctions;
	CPath		* m_pPath;
	int			  m_iCurrentTask;
	int			  m_iResult;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // CHECKFORUPDATESPROCESS_H
