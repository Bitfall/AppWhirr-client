/********************************************************************
	created:	2006/03/02
	created:	2:3:2006   9:24
	filename: 	CheckForUpdatesProcess.cpp
	file path:	Updater
	file base:	CheckForUpdatesProcess
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "CheckForUpdatesProcess.h"

// Registry support
#include "Registry.h"

// Actions
#include "CheckVersion.h"

// Sections support
#include "Sections.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CCheckForUpdatesProcess, CWinThread)
	ON_THREAD_MESSAGE(WMU_THREADMESSAGE, OnThreadMessage)
	ON_THREAD_MESSAGE(WMU_TASK_COMPLETE, OnTaskComplete)
	ON_THREAD_MESSAGE(WMU_ERROR, OnError)
	ON_THREAD_MESSAGE(WMU_QUESTION, OnQuestion)
	ON_THREAD_MESSAGE(WMU_QUESTION_ANSWER, OnQuestionAnswer)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CCheckForUpdatesProcess, CWinThread)

//=====================================================================

CCheckForUpdatesProcess::CCheckForUpdatesProcess()
{
}

//=====================================================================

CCheckForUpdatesProcess::~CCheckForUpdatesProcess()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CCheckForUpdatesProcess::InitInstance()
{
	// Initialise data
	m_iCurrentTask = TASK_INIT;
	m_pCurrentThread = NULL;

	// Initialise singleton classes
	m_pSettings = CSettings::Instance();
	m_pLanguage = CLanguage::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pFunctions = CFunctions::Instance();
	m_pPath = CPath::Instance();

	// Read section file
	CSections * pSections = CSections::Instance();
	pSections->ReadFile(m_pPath->GetPathUpdater() + FILENAME_SECTIONS);

	// Initialize internet class
	CInternet * pInternet = CInternet::Instance();
	pInternet->SetProxyAuthentication(m_pSettings->GetProxyUsername(), m_pSettings->GetProxyPassword());
	pInternet->SetHttpAuthentication(m_pSettings->GetHttpUsername(), m_pSettings->GetHttpPassword());
	pInternet->SetFtpAuthentication(m_pSettings->GetFtpUsername(), m_pSettings->GetFtpPassword());
	pInternet->SetTimeOut(m_pSettings->GetTimeout());

	// Start downloading custom language file
	pInternet->SetParent(this);

	// Set default return code
	m_iResult = RETURN_ERROR;

	// Init task is ready
	PostThreadMessage(WMU_TASK_COMPLETE, TASK_INIT, 0);

	// Start message pump
	return TRUE;
}

//=====================================================================

int CCheckForUpdatesProcess::ExitInstance()
{
	// Declare variables
	CInternet * pInternet = CInternet::Instance();
	CRollbackInfo * pRollback = CRollbackInfo::Instance();

	// Is the current thread closed?
	if (!IsBadReadPtr(m_pCurrentThread, sizeof(CWinThread *)))
		m_pCurrentThread->PostThreadMessage(WM_QUIT, 0, 0);

	// Clear some memory (singleton classes won't be cleared)
	m_pUpdateInfo->CleanUp();
	pInternet->CleanUp();
	pRollback->CleanUp();
	CEventInfo::Instance()->CleanUp();

	// Exit this thread, which will end application
	CWinThread::ExitInstance();

	// Return the right code
	return m_iResult;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CCheckForUpdatesProcess::StartTask(int iTask)
{
	// What task should we start?
	switch (iTask)
	{
	case TASK_NEXTTASK:
		// Start next task
		m_iCurrentTask++;
		break;

	default:
		// Set task
		m_iCurrentTask = iTask;	
	}

	// Check what task to start now
	switch (m_iCurrentTask)
	{
	case TASK_CHECKVERSION:
		// Check version
		if (!CheckVersion())
		{
			// Stop executing this thread
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
		break;
	}
}

//=====================================================================

bool CCheckForUpdatesProcess::CheckVersion()
{
	// Start thread
	CCheckVersion * pCheckVersion;
	pCheckVersion = (CCheckVersion *)AfxBeginThread(RUNTIME_CLASS(CCheckVersion),
													THREAD_PRIORITY_NORMAL,
													0,
													0,
													NULL);

	// Check if thread is successfully started
	if (pCheckVersion != NULL)
	{
		// Inform who the parent is
		pCheckVersion->PostThreadMessage(WMU_SETPARENT, (WPARAM)this, 0);

		// Now really start thread
		pCheckVersion->PostThreadMessage(WMU_THREADMESSAGE, THREAD_START, 0);

		// Set current thread
		m_pCurrentThread = pCheckVersion;

		// We are successful
		return true;
	}
	else
	{
		// Thread did not start successful
		return false;
	}
}

//=====================================================================

void CCheckForUpdatesProcess::OnThreadMessage(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iThreadAction = wParam;
	int iTask = lParam;

	// Check what task to perform for thread
	switch (iThreadAction)
	{
	case THREAD_PAUSE:
		// Pause thread
		SuspendThread();
		break;

	case THREAD_RESUME:
		// Resume thread
		ResumeThread();
		break;

	case THREAD_STARTNEXTTASK:
		// Start next task
		StartTask();
		break;

	case THREAD_STARTTASK:
		// Check if task isn't running already
		if (iTask < (TASK_CHECKVERSION + 1))
		{
			// Start a specific task
			StartTask(iTask);
		}
		break;
	}
}

//=====================================================================

void CCheckForUpdatesProcess::OnTaskComplete(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iTask = (int)wParam;
	unsigned long ulCounter = 0;

	if (!IsBadReadPtr(m_pCurrentThread, sizeof(CWinThread *)))
	{
		// Wait for the thread to be really closed, not forever,
        // be smart to come out of any stale state.
        while ((++ulCounter < 60) && !IsBadReadPtr(m_pCurrentThread, sizeof(CWinThread *)))
        {
			// Now wait
    		if (WaitForSingleObject(m_pCurrentThread->m_hThread, 1000) == WAIT_OBJECT_0)
            {
                break;
            }
        }

		// Thread is closed, set to NULL
		m_pCurrentThread = NULL;
	}
	
	// Check what task is completed
	switch (iTask)
	{
	case TASK_INIT:
		// Start check version task
		PostThreadMessage(WMU_THREADMESSAGE, THREAD_STARTTASK, TASK_CHECKVERSION);
		break;

	case TASK_CHECKVERSION:
		// New version available
		m_iResult = RETURN_NEWVERSION;

		// Quit thread
		PostThreadMessage(WM_QUIT, 0, 0);
		break;
	}
}

//=====================================================================

void CCheckForUpdatesProcess::OnError(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iError = (int)wParam;
	int iTask = (int)lParam;
	CString sTemp;

	// Check what error occurred
	switch (iError)
	{
	case ERROR_NONEWVERSION:
		// No new version is found
		m_iResult = RETURN_NONEWVERSION;
		break;

	default:
		// An error occurred
		m_iResult = RETURN_ERROR;
		break;
	}

	// Quit thread
	PostThreadMessage(WM_QUIT, 0, 0);
}

//=====================================================================

void CCheckForUpdatesProcess::OnQuestion(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iQuestion = (int)wParam;
	CString sTemp;
}

//=====================================================================

void CCheckForUpdatesProcess::OnQuestionAnswer(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iQuestion = (int)wParam;
	int iAnswer = (int)lParam;
	CString sTemp;

	// Are we handling an event?
	/*if (m_bEventHandling)
	{
		// Handle ourself
		if (lParam == QUESTIONRESULT_YES)
		{
			// Close application
			if (!m_pFunctions->CloseApplication(m_pSettings->GetAppLocation()))
			{
				// Send error message
				PostThreadMessage(WMU_ERROR, ERROR_CLOSEAPP, (LPARAM)(LPCTSTR)m_pSettings->GetAppName());
				return;
			}

			// Continue
			
		}
		else
		{
			// Send error message
			PostThreadMessage(WMU_ERROR, ERROR_CLOSEAPP, (LPARAM)(LPCTSTR)m_pSettings->GetAppName());
			return;
		}

		// Unset boolean
		m_bEventHandling = false;
	}
	else
	{*/
	// Send message to thread so it can continue
	m_pCurrentThread->PostThreadMessage(WMU_QUESTION_ANSWER, wParam, lParam);
}

