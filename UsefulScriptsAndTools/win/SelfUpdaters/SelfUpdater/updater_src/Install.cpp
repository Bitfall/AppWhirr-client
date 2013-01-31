/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:55
	filename: 	Install.cpp
	file path:	Updater
	file base:	Install
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Install.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CInstall, CWinThread)
	ON_THREAD_MESSAGE(WMU_INSTALL_STATUS, OnInstallStatus)
	ON_THREAD_MESSAGE(WMU_ERROR, OnError)
	ON_THREAD_MESSAGE(WMU_WARNING, OnWarning)
	ON_THREAD_MESSAGE(WMU_WARNING_COMPLETE, OnWarningComplete)
	ON_THREAD_MESSAGE(WMU_QUESTION, OnQuestion)
	ON_THREAD_MESSAGE(WMU_QUESTION_ANSWER, OnQuestionAnswer)
	ON_THREAD_MESSAGE(WMU_INSTALL_COMPLETE, OnInstallComplete)
	ON_THREAD_MESSAGE(WMU_SETPARENT, OnSetParent)
	ON_THREAD_MESSAGE(WMU_THREADMESSAGE, OnThreadMessage)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CInstall, CWinThread)

//=====================================================================

CInstall::CInstall()
{
}

//=====================================================================

CInstall::~CInstall()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CInstall::InitInstance()
{
	// Log
	CLog::Instance()->Log(_T("CInstall::InitInstance()"));

	// Set parent to NULL
	m_pParent = NULL;

	// We are not started yet
	m_bStarted = false;
	
	// Reserve memory for buffer
	m_szFilename = (TCHAR *)malloc(sizeof(TCHAR) * 500);
	ZeroMemory(m_szFilename, sizeof(TCHAR) * 500);

	// Start install files thread
	m_pInstallFiles = (CInstallFiles *)AfxBeginThread(RUNTIME_CLASS(CInstallFiles),
		THREAD_PRIORITY_NORMAL,
		0,
		0,
		NULL);

	// Start install ini thread
	m_pInstallIni = (CInstallIni *)AfxBeginThread(RUNTIME_CLASS(CInstallIni),
		THREAD_PRIORITY_NORMAL,
		0,
		0,
		NULL);
	
	// Start install registry thread
	m_pInstallRegistry = (CInstallRegistry *)AfxBeginThread(RUNTIME_CLASS(CInstallRegistry),
		THREAD_PRIORITY_NORMAL,
		0,
		0,
		NULL);

	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();
	m_pPath = CPath::Instance();

	// Extract UpdaterCopy.exe from this executable
	if (m_pUpdateInfo->GetSelfUpdate())
		ExtractUpdaterCopy();

	// Set parent
	m_pInstallFiles->PostThreadMessage(WMU_SETPARENT, (WPARAM)this, 0);
	m_pInstallIni->PostThreadMessage(WMU_SETPARENT, (WPARAM)this, 0);
	m_pInstallRegistry->PostThreadMessage(WMU_SETPARENT, (WPARAM)this, 0);

	// Suspend all threads
	/*m_pInstallFiles->SuspendThread();
	m_pInstallIni->SuspendThread();
	m_pInstallRegistry->SuspendThread();*/

	// Set some default values
	m_iPreviousActionsPerformed = 0;
	m_iTotalActionsPerformed = 0;

	return TRUE;
}

//=====================================================================

int CInstall::ExitInstance()
{
	// Log
	CLog::Instance()->Log(_T("CInstall::ExitInstance()"));

	// Destroy threads
	m_pInstallFiles->PostThreadMessage(WM_QUIT, 0, 0);
	m_pInstallIni->PostThreadMessage(WM_QUIT, 0, 0);
	m_pInstallRegistry->PostThreadMessage(WM_QUIT, 0, 0);

	// Auto clean-up thread
	m_bAutoDelete = TRUE;

	// Call original function
	return CWinThread::ExitInstance();
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CInstall::InstallPart(int iPart)
{
	// Set this part as active part
	m_iCurrentPart = iPart;

	// Check what part we are
	switch (m_iCurrentPart)
	{
	case INSTALL_FILES:
		//m_pInstallFiles->ResumeThread();
		m_pInstallFiles->PostThreadMessage(WMU_INSTALL_START, 0, 0);
		break;

	case INSTALL_INI:
		//m_pInstallIni->ResumeThread();
		m_pInstallIni->PostThreadMessage(WMU_INSTALL_START, 0, 0);
		break;

	case INSTALL_REGISTRY:
		//m_pInstallRegistry->ResumeThread();
		m_pInstallRegistry->PostThreadMessage(WMU_INSTALL_START, 0, 0);
		break;
	}
}

//=====================================================================

void CInstall::InstallNextPart()
{
	// Suspend current thread
	switch (m_iCurrentPart)
	{
	case INSTALL_FILES:
		m_pInstallFiles->PostThreadMessage(WM_QUIT, 0, 0);
		//m_pInstallFiles->SuspendThread();
		break;
		
	case INSTALL_INI:
		m_pInstallIni->PostThreadMessage(WM_QUIT, 0, 0);
		//m_pInstallIni->SuspendThread();
		break;
		
	case INSTALL_REGISTRY:
		m_pInstallRegistry->PostThreadMessage(WM_QUIT, 0, 0);
		//m_pInstallRegistry->SuspendThread();
		break;
	}

	// Update the part
	m_iCurrentPart++;

	// Are there still parts left?
	if (m_iCurrentPart < INSTALL_MAX)
	{
		// Install next part
		InstallPart(m_iCurrentPart);
	}
	else
	{
		// We are ready, progress MUST be 100%
		PostMessageToParent(WMU_UPDATE_GUI_FILEPROGRESS, 100, (LPARAM)m_szFilename);
		PostMessageToParent(WMU_UPDATE_GUI_TOTALPROGRESS, 100, 0);

		// Send message to parent that we are ready
		PostMessageToParent(WMU_TASK_COMPLETE, TASK_INSTALL, 0);
		
		// Exit the thread
		PostThreadMessage(WM_QUIT, 0, 0);
	}
}

//=====================================================================

void CInstall::ExtractUpdaterCopy()
{
	// Declare variables
	HRSRC resource;
	HGLOBAL resourceData;
	long * lpResLock;
	DWORD dwResourceSize;
	CString sUpdaterLocation, sResourceName, sUpdaterCopyLocation;

	// Get updater location
	sUpdaterLocation.Format(m_pPath->GetPathUpdaterExecutable());

	// Find the right resource
	sResourceName.Format(_T("#%d"), MAKEINTRESOURCE(IDR_UPDATERCOPY));
	resource = FindResource(NULL, sResourceName, _T("Bin"));

	// Load the executable resource
	resourceData = LoadResource(NULL, resource);

	// Get start address of resource
	lpResLock = (long *) LockResource(resourceData);

	// Get size of resource
	dwResourceSize = SizeofResource(NULL, resource);

	try
	{
		// Set up filename
		sUpdaterCopyLocation.Format(_T("%s\\%s"), m_pPath->GetPathTemp(), _T("UpdaterCopy.exe"));

		// Create file
		CFile exeFile(sUpdaterCopyLocation, CFile::modeWrite | CFile::modeCreate);

		// Wait until class is created
		Sleep(100);

		// Write data to executable
		exeFile.Write(lpResLock, dwResourceSize);

		// Close executable
		exeFile.Close();
	}

	// If there were any errors, catch them
	catch (CException * pEx)
	{
#if defined _DEBUG || defined _BETA
		pEx->ReportError();
#endif
		// Delete exception object to prevent leaks
		pEx->Delete();
	}
}

//=====================================================================

void CInstall::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

void CInstall::OnInstallStatus(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sTemp;
	int iFileProgress, iTotalProgress;
	
	// Calculate action count every time to be as precise as possible
	m_iActionCount = m_pInstallFiles->GetActionCount();
	m_iActionCount += m_pInstallIni->GetActionCount();
	m_iActionCount += m_pInstallRegistry->GetActionCount();

	// Get status information
	InstallStatus * pInstallStatus = (InstallStatus *)wParam;

	// Get name
	lstrcpy(m_szFilename, pInstallStatus->sTitle);

	// Check if there is a positive difference between previous and current action count
	if (pInstallStatus->iTotalActionsPerformed - m_iPreviousActionsPerformed > 0)
	{
		// Update the difference
		m_iTotalActionsPerformed += pInstallStatus->iTotalActionsPerformed - m_iPreviousActionsPerformed;
	}

	// Save current action count so we can compare
	m_iPreviousActionsPerformed = pInstallStatus->iTotalActionsPerformed;

	// Calculate % of file
	if (pInstallStatus->iFileActions != 0)
	{
		iFileProgress = (pInstallStatus->iFileActionsPerformed * 100) / pInstallStatus->iFileActions;
	}
	else
	{
		iFileProgress = 0;
	}
	
	// Calculate % of total progress
	if (m_iActionCount != 0)
	{
		iTotalProgress = (m_iTotalActionsPerformed * 100) / m_iActionCount;
	}
	else
	{
		iTotalProgress = 0;
	}

	// Should we send information to parent?
	if (m_iFileProgress != iFileProgress)
		PostMessageToParent(WMU_UPDATE_GUI_FILEPROGRESS, iFileProgress, (LPARAM)m_szFilename);
	if (m_iTotalProgress != iTotalProgress)
		PostMessageToParent(WMU_UPDATE_GUI_TOTALPROGRESS, iTotalProgress, 0);

	// Update information
	m_iFileProgress = iFileProgress;
	m_iTotalProgress = iTotalProgress;
}

//=====================================================================

void CInstall::OnInstallComplete(WPARAM wParam, LPARAM lParam)
{
	// Install next part
	InstallNextPart();
}

//=====================================================================

void CInstall::OnError(WPARAM wParam, LPARAM lParam)
{
	// Send this error immediatly to parent
	PostMessageToParent(WMU_ERROR, wParam, (LPARAM)m_szFilename);
}

//=====================================================================

void CInstall::OnWarning(WPARAM wParam, LPARAM lParam)
{
	// Send this warning immediatly to parent
	PostMessageToParent(WMU_WARNING, wParam, (LPARAM)m_szFilename);
}

//=====================================================================

void CInstall::OnWarningComplete(WPARAM wParam, LPARAM lParam)
{
	// Send this complete message immediatly to childs
	m_pInstallFiles->PostThreadMessage(WMU_WARNING_COMPLETE, wParam, lParam);
	m_pInstallIni->PostThreadMessage(WMU_WARNING_COMPLETE, wParam, lParam);
	m_pInstallRegistry->PostThreadMessage(WMU_WARNING_COMPLETE, wParam, lParam);
}

//=====================================================================

void CInstall::OnQuestion(WPARAM wParam, LPARAM lParam)
{
	// Send this question immediatly to parent
	PostMessageToParent(WMU_QUESTION, wParam, lParam);
}

//=====================================================================

void CInstall::OnQuestionAnswer(WPARAM wParam, LPARAM lParam)
{
	// Check which question we are dealing with
	if (wParam == QUESTION_CLOSEAPPLICATION)
	{
		// Handle ourself
		if (lParam == QUESTIONRESULT_YES)
		{
			if (m_pUpdateInfo->GetCloseApplication() != CLOSE_USER)
			{
				// Close application
				if (!m_pFunctions->CloseApplication(m_pSettings->GetAppName(), m_pSettings->GetAppLocation()))
				{
					// Send error message
					PostMessageToParent(WMU_ERROR, ERROR_CLOSEAPP, (LPARAM)(LPCTSTR)m_pSettings->GetAppName());
					PostThreadMessage(WM_QUIT, 0, 0);
					return;
				}

				// Start installing first part
				InstallPart(INSTALL_FILES);
			}
			else
			{
				PostThreadMessage(WMU_THREADMESSAGE, THREAD_START, 0);
			}
		}
		else
		{
			// Send error message
			PostMessageToParent(WMU_ERROR, ERROR_CLOSEAPP, (LPARAM)(LPCTSTR)m_pSettings->GetAppName());
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
	}
	else
	{
		// Send this result message immediatly to childs
		m_pInstallFiles->PostThreadMessage(WMU_QUESTION_ANSWER, wParam, lParam);
		m_pInstallIni->PostThreadMessage(WMU_QUESTION_ANSWER, wParam, lParam);
		m_pInstallRegistry->PostThreadMessage(WMU_QUESTION_ANSWER, wParam, lParam);
	}
}

//=====================================================================

void CInstall::OnSetParent(WPARAM wParam, LPARAM lParam)
{
	// Set parent
	m_pParent = (CWinThread *)wParam;
}

//=====================================================================

void CInstall::OnThreadMessage(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iThreadAction = (int)wParam;
	
	// Check what task to perform for thread
	switch (iThreadAction)
	{
	case THREAD_START:
		// Are we already started?
		if (!m_bStarted)
		{
			// Check if app should be closed
			if (m_pUpdateInfo->GetCloseApplication() != CLOSE_FALSE)
			{
				// Is application running?
				if (m_pFunctions->ApplicationRunning(m_pSettings->GetAppLocation()))
				{
					// Ask user
					PostMessageToParent(WMU_QUESTION, QUESTION_CLOSEAPPLICATION, 
						(LPARAM)(LPCTSTR)m_pSettings->GetAppName());

					// Now wait for an answer
					return;
				}
			}

			// Application should be closed now, start installing first part
			InstallPart(INSTALL_FILES);
		}
		break;
		
	case THREAD_PAUSE:
		// Pause thread
		SuspendThread();
		break;
		
	case THREAD_RESUME:
		// Resume thread
		ResumeThread();
		break;
		
	case THREAD_CANCEL:
		// Exit thread
		PostThreadMessage(WM_QUIT, 0, 0);
		break;
		
	case THREAD_FINISH:
		// Exit thread
		PostThreadMessage(WM_QUIT, 0, 0);
		break;
	}
}

//=====================================================================
