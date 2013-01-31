/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   15:13
	filename: 	ModeHidden.cpp
	file path:	Updater
	file base:	ModeHidden
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeHidden.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeHidden, CWnd)
	ON_MESSAGE(WMU_TASK_START, OnTaskStart)
	ON_MESSAGE(WMU_TASK_COMPLETE, OnTaskComplete)
	ON_MESSAGE(WMU_ERROR, OnError)
	ON_MESSAGE(WMU_WARNING, OnWarning)
	ON_MESSAGE(WMU_QUESTION, OnQuestion)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeHidden, CDialog)

//=====================================================================

CModeHidden::CModeHidden(CWinThread * pUpdateProcess, int iCurrentTask)
					: CDialog(CModeHidden::IDD, (CWnd *)pUpdateProcess)
{
	// Set values
	m_pUpdateProcess = pUpdateProcess;
	m_iCurrentTask = iCurrentTask;
}

//=====================================================================

CModeHidden::~CModeHidden()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************


//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

BOOL CModeHidden::OnInitDialog()
{
	// Call original function
	CDialog::OnInitDialog();

	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pLanguage = CLanguage::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pPath = CPath::Instance();

	// We are not initialized yet
	m_bInit = false;
	m_bFinished = false;

	// Set right action
	PostMessage(WMU_TASK_START, m_iCurrentTask);

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

LRESULT CModeHidden::OnTaskStart(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sTemp;
	
	// Get data
	int iTask = wParam;
	m_iCurrentTask = iTask;

	// Check what task is started
	switch (iTask)
	{
	case TASK_HTMLMESSAGE:
		// Just go to next step, user is not interested in html message
		m_pUpdateProcess->PostThreadMessage(WMU_TASK_COMPLETE, iTask, 0);
		//m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_STARTNEXTTASK, 0);
		break;

	case TASK_LICENSE:
		// Send task complete message
		m_pUpdateProcess->PostThreadMessage(WMU_TASK_COMPLETE, iTask, 0);
		break;

	case TASK_SELECTUPDATE:
		// Send task complete message
		m_pUpdateProcess->PostThreadMessage(WMU_TASK_COMPLETE, iTask, 0);
		break;
	}

	return S_OK;
}

//=====================================================================

LRESULT CModeHidden::OnTaskComplete(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sTemp;

	// Get data
	int iTask = wParam;

	// Take it easy, we are hidden anyway, this is needed otherwhise the threads
	// can't make it, Updater will be too fast!
	Sleep(250);

	// Check what task is completed
	switch (iTask)
	{
	case TASK_HTMLMESSAGE:
		// Do not send any message
		return S_OK;		

	case TASK_LICENSE:
		// Do not send any message
		return S_OK;

	case TASK_PROTECTION:
		// Do not send any message
		return S_OK;

	case TASK_SELECTUPDATE:
		// Do not send any message
		return S_OK;

	case TASK_DOWNLOAD:
		// Ok, calm down, we have downloaded all files
		Sleep(1000);	
		break;

	case TASK_ROLLBACK:
		// We are finished
		m_bFinished = true;
		// We are ready rolling back changes, now finish thread
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);
		break;

	case TASK_INSTALL:
		// We are finished
		m_bFinished = true;
		// We are ready, now finish thread
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);
		break;
	}

	// If it is the last step, let user close application
	if ((iTask != TASK_INSTALL) && (iTask != TASK_ROLLBACK))
	{
		// Task is complete, don't ask user for confirmation, go to next step
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_STARTNEXTTASK, 0);
	}

	return S_OK;
}

//=====================================================================

LRESULT CModeHidden::OnError(WPARAM wParam, LPARAM lParam)
{
	// Get data
	int iError = wParam;
	bool bRollback = true;

	// Check what error has occured
	switch (iError)
	{
	case ERROR_NONEWVERSION:
		// This is not a real error
		bRollback = false;
		break;

	case ERROR_PROTECTION:
		// Don't roll back
		bRollback = false;
		break;
	}

	// Don't show error to user
	//AfxMessageBox(sError, MB_OK | MB_ICONERROR);

	// Should we rollback?
	if (bRollback)
	{
		// Start next task, which will be rollback
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_STARTTASK, TASK_ROLLBACK);
	}
	else
	{
		// We are finished
		m_bFinished = true;

		// Finish updating
		if (m_bFinished)
			m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);
	}

	return S_OK;
}

//=====================================================================

LRESULT CModeHidden::OnWarning(WPARAM wParam, LPARAM lParam)
{
	// Don't show warning, just continue
	m_pUpdateProcess->PostThreadMessage(WMU_WARNING_COMPLETE, 0, 0);

	return S_OK;
}

//=====================================================================

LRESULT CModeHidden::OnQuestion(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iQuestion = wParam;
	int iResult;

	// Check what type of question
	switch (iQuestion)
	{
	case QUESTION_STOPUPDATING:
		// Ask user
		break;

	case QUESTION_DELETEFILE:
		// Answer yes
		iResult = QUESTIONRESULT_YES;
		break;

	case QUESTION_CLOSEAPPLICATION:
		// Answer yes
		iResult = QUESTIONRESULT_YES;
		break;

	case QUESTION_REGISTERFILE:
		// Answer yes
		iResult = QUESTIONRESULT_YES;
		break;
	}

	// Return answer
	m_pUpdateProcess->PostThreadMessage(WMU_QUESTION_ANSWER, iQuestion, iResult);

	return S_OK;
}