/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   9:24
	filename: 	UpdateProcess.cpp
	file path:	Updater
	file base:	UpdateProcess
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "UpdateProcess.h"

// Registry support
#include "Registry.h"

// Actions
#include "CheckVersion.h"
#include "Protection.h"
#include "SelectUpdate.h"
#include "Download.h"
#include "Install.h"
#include "Rollback.h"

// Sections support
#include "Sections.h"

// About dlg
#include "AboutDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CUpdateProcess, CWinThread)
	ON_THREAD_MESSAGE(WMU_THREADMESSAGE, OnThreadMessage)
	ON_THREAD_MESSAGE(WMU_TASK_COMPLETE, OnTaskComplete)
	ON_THREAD_MESSAGE(WMU_ERROR, OnError)
	ON_THREAD_MESSAGE(WMU_WARNING, OnWarning)
	ON_THREAD_MESSAGE(WMU_WARNING_COMPLETE, OnWarningComplete)
	ON_THREAD_MESSAGE(WMU_QUESTION, OnQuestion)
	ON_THREAD_MESSAGE(WMU_QUESTION_ANSWER, OnQuestionAnswer)
	ON_THREAD_MESSAGE(WMU_UPDATE_GUI, OnUpdateGUI)
	ON_THREAD_MESSAGE(WMU_UPDATE_GUI_FILEPROGRESS, OnUpdateGUIFileProgress)
	ON_THREAD_MESSAGE(WMU_UPDATE_GUI_TOTALPROGRESS, OnUpdateGUITotalProgress)
	ON_THREAD_MESSAGE(WMU_UPDATE_GUI_PROTECTION, OnUpdateGUIProtection)
	ON_THREAD_MESSAGE(WMU_SWITCHUI, OnSwitchUI)
	ON_THREAD_MESSAGE(WMU_SHOWABOUT, OnShowAbout)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CUpdateProcess, CWinThread)

//=====================================================================

CUpdateProcess::CUpdateProcess()
{
}

//=====================================================================

CUpdateProcess::~CUpdateProcess()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CUpdateProcess::InitInstance()
{
	// Initialise data
	m_iCurrentTask = TASK_INIT;
	m_pCurrentUI = NULL;
	m_pCurrentThread = NULL;
	m_bFinished = false;
	m_bRollback = false;
	m_bInstallComplete = false;
	m_bAllowUIChange = true;
	m_bUpdateCancelled = false;

	// Initialise singleton classes
	m_pSettings = CSettings::Instance();
	m_pLanguage = CLanguage::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pFunctions = CFunctions::Instance();
	m_pEventInfo = CEventInfo::Instance();
	m_pLog = CLog::Instance();
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

	// Init task is ready
	PostThreadMessage(WMU_TASK_COMPLETE, TASK_INIT, 0);

	// Start message pump
	return TRUE;
}

//=====================================================================

int CUpdateProcess::ExitInstance()
{
	// Declare variables
	CInternet * pInternet = CInternet::Instance();
	CRollbackInfo * pRollback = CRollbackInfo::Instance();
	CString sDirectory;

	// Destroy the User Interface
	DestroyUserInterface();

	// Is the current thread closed?
	if (!IsBadReadPtr(m_pCurrentThread, sizeof(CWinThread *)))
		m_pCurrentThread->PostThreadMessage(WM_QUIT, 0, 0);

	// Handle OnClose event only if we are successful
	if ((m_iCurrentTask != TASK_ROLLBACK) && m_bFinished)
	{
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_ONCLOSE));
	}

	// Check if the update is completly ready
	if (m_bInstallComplete)
	{
		FinishUpdate();
	}

	// Clear some memory (singleton classes won't be cleared)
	m_pEventInfo->CleanUp();
	m_pUpdateInfo->CleanUp();
	pInternet->CleanUp();
	pRollback->CleanUp();

	// Auto clean-up thread
	m_bAutoDelete = TRUE;

	// Exit this thread, which will end application
	return CWinThread::ExitInstance();
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CUpdateProcess::StartTask(int iTask)
{
	// If user cancelled update
	if (m_bUpdateCancelled)
	{
		// Only allow rollback
		if (iTask != TASK_ROLLBACK)
			return;
	}

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

	// Check if process is cancelled
	if (m_bUpdateCancelled)
	{
		// We are only allowed to handle TASK_ROLLBACK
		if (m_iCurrentTask != TASK_ROLLBACK)
			return;
	}

	// Check what task to start now
	switch (m_iCurrentTask)
	{
	case TASK_CHECKVERSION:
		// Log
		m_pLog->Log(_T("starting task: TASK_CHECKVERSION"));

		// Check version
		if (!CheckVersion())
		{
			// Stop executing this thread
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
		break;

	case TASK_HTMLMESSAGE:
		// Log
		m_pLog->Log(_T("starting task: TASK_HTMLMESSAGE"));

		// Check if we should show dialog
		if (m_pUpdateInfo->GetHtmlMessageURL().IsEmpty())
		{
			// Post message to start next task
			PostThreadMessage(WMU_TASK_COMPLETE, TASK_HTMLMESSAGE, 0);
			return;
		}

		// Handle BeforeHtmlMessage event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_BEFOREHTMLMESSAGE));

		// Always send this message to user interface
		SendMessageToUI(WMU_TASK_START, TASK_HTMLMESSAGE, 0);

		// Now wait until user has watched the html message
		break;

	case TASK_LICENSE:
		// Log
		m_pLog->Log(_T("starting task: TASK_LICENSE"));

		// Check if we should show dialog
		if (m_pUpdateInfo->GetLicense().IsEmpty())
		{
			// Post message to start next task
			PostThreadMessage(WMU_TASK_COMPLETE, TASK_LICENSE, 0);
			return;
		}

		// Handle BeforeLicense event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_BEFORELICENSE));

		// Always send this message to user interface
		SendMessageToUI(WMU_TASK_START, TASK_LICENSE, 0);

		// Now wait until user has agreed to the license
		break;

	case TASK_PROTECTION:
		// Log
		m_pLog->Log(_T("starting task: TASK_PROTECTION"));

		// Check if we should show dialog
		if (m_pUpdateInfo->GetProtectionRegistrationDll().IsEmpty() && (m_pUpdateInfo->GetProtectionHashCount() == 0))
		{
			// Post message to start next task
			PostThreadMessage(WMU_THREADMESSAGE, THREAD_STARTNEXTTASK, 0);
			return;
		}

		// Check protection
		if (!Protect())
		{
			// Stop executing this thread
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
		break;

	case TASK_SELECTUPDATE:
		// Log
		m_pLog->Log(_T("starting task: TASK_SELECTUPDATE"));

		// Check if we should show dialog
		if (m_pUpdateInfo->GetSectionCount() <= 0)
		{
			// Post message to start next task
			PostThreadMessage(WMU_TASK_COMPLETE, TASK_SELECTUPDATE, 0);
			return;
		}

		// Handle BeforeSelectUpdate event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_BEFORESELECTUPDATE));

		// Always send this message to user interface
		SendMessageToUI(WMU_TASK_START, TASK_SELECTUPDATE, 0);

		// Now wait until user has selected the updates
		break;

	case TASK_DOWNLOAD:
		// Log
		m_pLog->Log(_T("starting task: TASK_DOWNLOAD"));

		// Download files
		if (!Download())
		{
			// Stop executing this thread
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
		break;
		
	case TASK_INSTALL:
		// Log
		m_pLog->Log(_T("starting task: TASK_INSTALL"));

		// Install files
		if (!Install())
		{
			// Stop executing this thread
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
		break;

	case TASK_ROLLBACK:
		// Log
		m_pLog->Log(_T("starting task: TASK_ROLLBACK"));

		// Install is not ready anymore!
		m_bInstallComplete = false;

		// Rollback actions
		Rollback();
		break;
	}
}

//=====================================================================

bool CUpdateProcess::CheckVersion()
{
	// Always send this message to user interface
	SendMessageToUI(WMU_TASK_START, TASK_CHECKVERSION, 0);

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

bool CUpdateProcess::Protect()
{
	// Always send this message to user interface
	SendMessageToUI(WMU_TASK_START, TASK_PROTECTION, 0);

	// Start thread
	CProtection * pProtection;
	pProtection = (CProtection *)AfxBeginThread(RUNTIME_CLASS(CProtection),
													THREAD_PRIORITY_NORMAL,
													0,
													0,
													NULL);

	// Check if thread is successfully started
	if (pProtection != NULL)
	{
		// Inform who the parent is
		pProtection->PostThreadMessage(WMU_SETPARENT, (WPARAM)this, 0);

		// Now really start thread
		pProtection->PostThreadMessage(WMU_THREADMESSAGE, THREAD_START, 0);

		// Set current thread
		m_pCurrentThread = pProtection;

		// We are successful
		return true;
	}
	else
	{
		// Thread did not start successful
		return false;
	}

	return true;
}

//=====================================================================

bool CUpdateProcess::SelectUpdate()
{
	// Declare variables
	CSections * pSections = CSections::Instance();
	CString arrSectionsToUpdate[MAX_SECTIONS];
	int i, j, iSectionsToUpdate = 0;

	// When check separately is used, no sections can be checked!
	if (m_pUpdateInfo->GetCheckSeparately())
	{
		return true;
	}

	// Log
	m_pLog->Log(_T("CHECKING FOR SELECTED SECTIONS BY USER"));

	// Check which sections should be updated
	for (i = 0; i < m_pUpdateInfo->GetSectionCount(); i++)
	{
		if (m_pUpdateInfo->GetSectionData(i)->GetSelectedByUser())
		{
			// This section should be updated, add name to array
			arrSectionsToUpdate[iSectionsToUpdate++] = m_pUpdateInfo->GetSectionData(i)->GetName();

			// Log
			m_pLog->Log(_T("  * ") + m_pUpdateInfo->GetSectionData(i)->GetName());
		}
	}

	// Calculate the files that should be updated
	for (i = 0; i < m_pUpdateInfo->GetFileCount(); i++)
	{
		// Set default to no update
		m_pUpdateInfo->GetFileData(i)->SetUpdateFile(false);

		// Is the section empty?
		if (m_pUpdateInfo->GetFileData(i)->GetSection().IsEmpty())
		{
			// We should update this file
			m_pUpdateInfo->GetFileData(i)->SetUpdateFile(true);
		}
		else
		{
			// Check to which section this file belongs
			for (j = 0; j < iSectionsToUpdate; j++)
			{
				// Should we update?
				if (arrSectionsToUpdate[j] == m_pUpdateInfo->GetFileData(i)->GetSection())
					m_pUpdateInfo->GetFileData(i)->SetUpdateFile(true);
			}
		}
	}

	// Log
	m_pLog->Log(_T("NEW VERSIONS FOR SELECTED SECTIONS"));

	// Set new version for the sections that will be updated
	for (i = 0; i < iSectionsToUpdate; i++)
	{
		// Get the newest version
		for (j = 0; j < m_pUpdateInfo->GetSectionCount(); j++)
		{
			// Do we have the right one?
			if (m_pUpdateInfo->GetSectionData(j)->GetName() == arrSectionsToUpdate[i])
			{
				// Set version
				pSections->SetSectionVersion(arrSectionsToUpdate[i], m_pUpdateInfo->GetSectionData(j)->GetVersion());

				// Log new section version
				m_pLog->Log(_T("  * ") + m_pUpdateInfo->GetSectionData(j)->GetName() + _T(" -> ") +
					m_pUpdateInfo->GetSectionData(j)->GetVersion());
			}
		}
	}

	// No thread needs to be started
	return true;
}

//=====================================================================

bool CUpdateProcess::Download()
{
	// Handle BeforeDownload task
	HandleEvent(m_pEventInfo->GetEventInfo(EVENT_BEFOREDOWNLOAD));

	// Always send this message to user interface
	SendMessageToUI(WMU_TASK_START, TASK_DOWNLOAD, 0);

	// Start thread
	CDownload * pDownload;
	pDownload = (CDownload *)AfxBeginThread(RUNTIME_CLASS(CDownload),
											THREAD_PRIORITY_NORMAL,
											0,
											0,
											NULL);
	
	// Check if thread is successfully started
	if (pDownload != NULL)
	{
		// Inform who the parent is
		pDownload->PostThreadMessage(WMU_SETPARENT, (WPARAM)this, 0);

		// Now really start thread
		pDownload->PostThreadMessage(WMU_THREADMESSAGE, THREAD_START, 0);

		// Set current thread
		m_pCurrentThread = pDownload;

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

bool CUpdateProcess::Install()
{
	// Handle BeforeInstall event
	HandleEvent(m_pEventInfo->GetEventInfo(EVENT_BEFOREINSTALL));

	// Always send this message to user interface
	SendMessageToUI(WMU_TASK_START, TASK_INSTALL, 0);

	// Start thread
	CInstall * pInstall;
	pInstall = (CInstall *)AfxBeginThread(RUNTIME_CLASS(CInstall),
										  THREAD_PRIORITY_NORMAL,
										  0,
										  0,
										  NULL);
	
	// Check if thread is successfully started
	if (pInstall != NULL)
	{
		// Inform who the parent is
		pInstall->PostThreadMessage(WMU_SETPARENT, (WPARAM)this, 0);

		// Now really start thread
		pInstall->PostThreadMessage(WMU_THREADMESSAGE, THREAD_START, 0);

		// Set current thread
		m_pCurrentThread = pInstall;

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

void CUpdateProcess::Rollback()
{
	// Rollback is used
	m_bRollback = true;

	// Handle BeforeRollback event
	HandleEvent(m_pEventInfo->GetEventInfo(EVENT_BEFOREROLLBACK));

	// Always send this message to user interface
	SendMessageToUI(WMU_TASK_START, TASK_ROLLBACK, 0);

	// Should we rollback?
	if (m_pUpdateInfo->GetRollbackEnabled() && !m_bFinished)
	{
		// Rollback all actions
		CRollback * pRollback;
		pRollback = (CRollback *)AfxBeginThread(RUNTIME_CLASS(CRollback),
												THREAD_PRIORITY_NORMAL,
												0,
												0,
												NULL);

		// Check if thread is successfully started
		if (pRollback != NULL)
		{
			// Inform who the parent is
			pRollback->PostThreadMessage(WMU_SETPARENT, (WPARAM)this, 0);
			
			// Now really start thread
			pRollback->PostThreadMessage(WMU_THREADMESSAGE, THREAD_START, 0);
			
			// Set current thread
			m_pCurrentThread = pRollback;
		}
	}
	else
	{
		// We shouldn't rollback, but in some cases, we don't know IF we
		// should rollback, i.e. when retrieving the update file. So, 
		// just send message that rollback is finished

		// Always set everything to 100 % at the end
		PostThreadMessage(WMU_UPDATE_GUI_TOTALPROGRESS, 100, 0);
		
		// Send message to parent that we are ready
		PostThreadMessage(WMU_TASK_COMPLETE, TASK_ROLLBACK, 0);
	}
}

//=====================================================================

void CUpdateProcess::FinishUpdate()
{
	// Declare variables
	CString sDirectory, sTemp, sSectionFile;
	bool bReboot = false;
	CShortcutData * pData;
	CRestoreInfo * pRestoreInfo = CRestoreInfo::Instance();

	// Initialize COM
	HRESULT hRes = CoInitialize(NULL);

	if (SUCCEEDED(hRes))
	{
		// Should we create shortcuts?
		if (m_pUpdateInfo->GetShortcutsCreateShortcuts())
		{
			// Create the shortcuts
			for (int i = 0; i < m_pUpdateInfo->GetShortcutCount(); i++)
			{
				// Get data
				pData = m_pUpdateInfo->GetShortcutData(i);

				// Log
				m_pLog->Log(_T("create shortcut ") + pData->GetDescription());

				// Create shortcut
				CreateShortcut(pData->GetTargetFile(), pData->GetParameters(), pData->GetLinkFile(),
					pData->GetDescription(), pData->GetShowMode(), pData->GetWorkingDirectory(),
					pData->GetIconFile(), pData->GetIconIndex());
			}
		}
	}

	// De-initialize COM
	CoUninitialize();

	// Write new version to settings.ini
#ifndef _DEBUG
	// Change version in settings.ini
	if (!PathFileExists(m_pSettings->GetAppVersion()))
	{
		// Only change new version when there really is a new version
		if (m_pFunctions->CompareVersions(m_pUpdateInfo->GetNewVersion(), m_pSettings->GetAppVersion()) == COMPAREVERSION_FIRSTLARGER)
		{
			// It is not a file, we can overwrite the settings
			WritePrivateProfileString(_T("APPLICATION"), _T("version"), m_pUpdateInfo->GetNewVersion(),
				CInternalData::Instance()->GetSettingsFile());

			// Log
			m_pLog->Log(_T("new version (") + m_pUpdateInfo->GetNewVersion() + _T(") is updated in settings file"));
		}
	}

	// Write all data to section file
	sSectionFile = m_pPath->GetPathUpdater() + FILENAME_SECTIONS;
	sSectionFile.Replace(_T("%name%"), m_pSettings->GetAppName());
	CSections * pSections = CSections::Instance();
	pSections->WriteFile(sSectionFile);
	
	// Log
	m_pLog->Log(_T("written section file to ") + FILENAME_SECTIONS);
#endif

	// If rollback is enabled, enable restore
	if (m_pUpdateInfo->GetRollbackEnabled())
	{
		// Log
		m_pLog->Log(_T("rollback is enabled, save restore information to ") + FILENAME_RESTORE);

		// Copy all information from rollback to restore
		pRestoreInfo->RetrieveInformationFromRollback();
		
		// Write information to file
		pRestoreInfo->WriteToFile(m_pPath->GetPathUpdaterTemp() + FILENAME_RESTORE);
	}

	// Delete all temporary files (don't do this anymore since restore is possible)
	CGarbageCollector::Instance()->CleanGarbage();

	// Log
	m_pLog->Log(_T("temporary update files deleted"));

	// Should we restart anything?
	switch (m_pUpdateInfo->GetCloseApplication())
	{
	case CLOSE_RESTART:
		// Log
		m_pLog->Log(_T("restart application (") + m_pSettings->GetAppLocation() + _T(")"));
		
		// Restart application
		if (PathFileExists(m_pSettings->GetAppLocation()))
		{
			// Is the application closed?
			if (CInternalData::Instance()->GetApplicationClosed())
			{
				// Prepare shellExecutInfo
				sDirectory = m_pPath->ExtractFilePath(m_pSettings->GetAppLocation());
				SHELLEXECUTEINFO ShRun = {0};
				ShRun.cbSize = sizeof(SHELLEXECUTEINFO);
				ShRun.fMask = SEE_MASK_NOCLOSEPROCESS;
				ShRun.hwnd = NULL;
				ShRun.lpVerb = NULL;
				ShRun.lpFile = m_pSettings->GetAppLocation();
				ShRun.lpDirectory = sDirectory;
				ShRun.nShow = SW_SHOW;
				ShRun.hInstApp = NULL;

				// Is app already running?
				if (!m_pFunctions->ApplicationRunning(m_pSettings->GetAppLocation()))
				{
					// Execute the file with the parameters
					ShellExecuteEx(&ShRun);
				}
			}
		}
		break;

	case CLOSE_RESTARTSYSTEM:
		// Log
		m_pLog->Log(_T("restart system"));

		// Reboot system
		bReboot = true;
		break;

	case CLOSE_RESTARTSYSTEMAPP:
		// Log
		m_pLog->Log(_T("restart system and run application (") + m_pSettings->GetAppLocation() + _T(") at next startup"));

		// Set application into startup path
		CRegistry reg;
		reg.CreateKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"));
		reg.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"));
		reg.Write(_T("Updater application restart"), m_pSettings->GetAppLocation());
		reg.Close();

		// Reboot system
		bReboot = true;
		break;
	}

	// Should we reboot?
	if (bReboot)
	{
		// Restart system
		if ((m_pSettings->GetRunmode() == MODE_FULL) || (m_pSettings->GetRunmode() == MODE_SILENT))
		{
			sTemp.Format(m_pLanguage->GetString(IDS_INFORMATION_RESTARTSYSTEM_EXPLANATION), m_pSettings->GetAppName());
			CInformationDlg dlgInformation(m_pLanguage->GetString(IDS_INFORMATION_TITLE), m_pLanguage->GetString(IDS_INFORMATION_RESTARTSYSTEM_TITLE),
				sTemp, m_pLanguage->GetString(IDS_GENERAL_OK));
			dlgInformation.DoModal();
		}

		// Restart system
		ExitWindowsEx(EWX_REBOOT, 0);
	}
}

//=====================================================================

void CUpdateProcess::SetMode(CString sMode)
{
	// Check if we are allowed to change UI
	if (!m_bAllowUIChange)
	{
		// Log
		m_pLog->Log(_T("CHANGING OF USER INTERFACE NOT ALLOWED"));

		// Exit functon
		return;
	}

	// Log
	m_pLog->Log(_T("START CHANGING USER INTERFACE"));
	m_pLog->Log(_T("  destroy existing user interface"));

	if (sMode == MODE_FULL)
	{
		// Destroy current user interface
		DestroyUserInterface();

		// Log
		m_pLog->Log(_T("  create new user interface: FULL MODE"));

		// Start full mode
		m_pCurrentUI = new CModeFullSheet(this, m_iCurrentTask);
		// Create window
		CModeFullSheet * pTemp = (CModeFullSheet *)m_pCurrentUI;
		pTemp->Create();

		// Set new mode
		m_sMode = sMode;
	}

	// Check what user interface should be loaded
	if (sMode == MODE_SILENT || sMode == MODE_VERYSILENT)
	{
		// Destroy current user interface
		DestroyUserInterface();

		// Log
		m_pLog->Log(_T("  create new user interface: SILENT MODE"));

		// Start silent mode
		m_pCurrentUI = new CModeSilent(this, m_iCurrentTask, sMode);

		// Create window
		CModeSilent * pTemp = (CModeSilent *)m_pCurrentUI;
		pTemp->Create(IDD_SILENT, (CWnd *)this);

		// Set new mode
		m_sMode = sMode;
	}

	if (sMode == MODE_HIDDEN)
	{
		// Destroy current user interface
		DestroyUserInterface();

		// Log
		m_pLog->Log(_T("  create new user interface: HIDDEN MODE"));

		// Start hidden mode
		m_pCurrentUI = new CModeHidden(this, m_iCurrentTask);

		// Create window
		CModeHidden * pTemp = (CModeHidden *)m_pCurrentUI;
		pTemp->Create(IDD_HIDDEN, (CWnd *)this);

		// Set new mode
		m_sMode = sMode;
	}

	// Log
	m_pLog->Log(_T("END CHANGING USER INTERFACE"));
}

//=====================================================================

void CUpdateProcess::DestroyUserInterface()
{
	// If we have a user interface
	if (m_pCurrentUI != NULL)
	{
		// Destroy window
		m_pCurrentUI->DestroyWindow();

		// Delete object
		delete m_pCurrentUI;

		// Set object to NULL
		m_pCurrentUI = NULL;
	}
}

//=====================================================================

void CUpdateProcess::SendMessageToUI(UINT message, WPARAM wParam, LPARAM lParam,
									 bool bSaveMessage /* = true */)
{
	// Do we have a valid UI?
	if (m_pCurrentUI != NULL)
	{
		// Save message
		if (bSaveMessage)
		{
			m_oLastUIMessage.message = message;
			m_oLastUIMessage.wParam = wParam;
			m_oLastUIMessage.lParam = lParam;
		}

		// Send message
		m_pCurrentUI->PostMessage(message, wParam, lParam);
	}
}

//=====================================================================

void CUpdateProcess::HandleEvent(CEventData * pEventData)
{
	// Declare variables
	CString sType, sDirectory;

	// Check if we have any event data
	if (pEventData == NULL)
		return;

	// Event is valid, check what actions to perform
	for (int i = 0; i < pEventData->GetActionCount(); i++)
	{
		// Get action type
		sType = pEventData->GetAction(i)->GetType();

		// Check what action to perform
		if (sType == EVENTACTION_CLOSE)
		{
			// Log
			m_pLog->Log(_T("handle eventaction CLOSE"));

			// Switch to hidden mode
			SetMode(MODE_HIDDEN);

			// Don't allow to switch to other UI again
			m_bAllowUIChange = false;

			// Check if the update is completly ready
			if (m_bInstallComplete || m_bFinished)
			{
				// Finish nicely
				PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);
			}
			else
			{
				// Rollback, then automatically close
				PostThreadMessage(WMU_THREADMESSAGE, THREAD_CANCEL, 0);
			}
		}

		if (sType == EVENTACTION_RUN)
		{
			// Get right action
			CEventActionRun * pAction = (CEventActionRun *)pEventData->GetAction(i);

			// Check if the user tries to open a website
			if (!m_pFunctions->IsWebAddress(pAction->GetRunLocation()))
			{
				// Check if file exists
				if (!PathFileExists(pAction->GetRunLocation()))
				{
					return;
				}
			}
			
			// Prepare shellExecutInfo
			sDirectory = m_pPath->ExtractFilePath(pAction->GetRunLocation());
			SHELLEXECUTEINFO ShRun = {0};
			ShRun.cbSize = sizeof(SHELLEXECUTEINFO);
			ShRun.fMask = SEE_MASK_NOCLOSEPROCESS;
			ShRun.hwnd = NULL;
			ShRun.lpVerb = NULL;
			ShRun.lpFile = pAction->GetRunLocation();
			ShRun.lpParameters = pAction->GetParameters();
			ShRun.lpDirectory = sDirectory;
			ShRun.nShow = SW_SHOW;
			ShRun.hInstApp = NULL;

			// Log
			m_pLog->Log(_T("handle eventaction RUN (") + pAction->GetRunLocation() + _T(")"));

			// Execute file
			ShellExecuteEx(&ShRun);
		}

		if (sType == EVENTACTION_SETMODE)
		{
			// Get right action
			CEventActionSetMode * pAction = (CEventActionSetMode *)pEventData->GetAction(i);

			// Log
			m_pLog->Log(_T("handle eventaction SETMODE"));

			// Set mode
			SetMode(pAction->GetMode());
		}

		if (sType == EVENTACTION_SKIPNEXTTASK)
		{
			// Log
			m_pLog->Log(_T("handle eventaction SKIPNEXTTASK"));

			// Skip next task
			PostThreadMessage(WMU_THREADMESSAGE, THREAD_SKIPNEXTTASK, 0);
		}

		if (sType == EVENTACTION_STARTNEXTTASK)
		{
			// Log
			m_pLog->Log(_T("handle eventaction STARTNEXTTASK"));

			// Skip next task
			PostThreadMessage(WMU_THREADMESSAGE, THREAD_STARTNEXTTASK, 0);
		}

		if (sType == EVENTACTION_CLOSEAPPLICATION)
		{
			// Get right action
			CEventActionCloseApplication * pAction = (CEventActionCloseApplication *)pEventData->GetAction(i);

			// Log
			m_pLog->Log(_T("handle eventaction CLOSEAPPLICATION"));

			// Check if application is running
			if (m_pFunctions->ApplicationRunning(pAction->GetFilename()))
			{
				// Should we ask user?
				if (pAction->GetAskUser())
				{
					// TEMPORARY SOLUTION - BEGIN
					/* 

					This should be asked to the user interface, but there is no solution for this one
					yet

					 */
					// Set up confirmation text
					CString sTemp;
					sTemp.Format(m_pLanguage->GetString(IDS_CONFIRMATION_CLOSEAPPLICATION_EXPLANATION),
						pAction->GetTitle(), m_pSettings->GetAppName(), pAction->GetTitle());					

					// Ask confirmation
					CConfirmationDlg dlgConfirm(m_pLanguage->GetString(IDS_CONFIRMATION_CLOSEAPPLICATION_TITLE),
						m_pLanguage->GetString(IDS_CONFIRMATION_TITLE), sTemp,
						m_pLanguage->GetString(IDS_GENERAL_YES), m_pLanguage->GetString(IDS_GENERAL_NO));
					if (dlgConfirm.DoModal() == IDNO)
					{
						// Send error message
						PostThreadMessage(WMU_ERROR, ERROR_CLOSEAPP, (LPARAM)(LPCTSTR)pAction->GetTitle());
						return;
					}
					else
					{
						// Close application imediately
						if (!m_pFunctions->CloseApplication(pAction->GetTitle(), pAction->GetFilename()))
						{
							// Send error message
							PostThreadMessage(WMU_ERROR, ERROR_CLOSEAPP, (LPARAM)(LPCTSTR)pAction->GetTitle());
							return;
						}
					}

					// TEMPORARY SOLUTION - END
				}
				else
				{
					// Close application imediately
					if (!m_pFunctions->CloseApplication(pAction->GetTitle(), pAction->GetFilename()))
					{
						// Send error message
						PostThreadMessage(WMU_ERROR, ERROR_CLOSEAPP, (LPARAM)(LPCTSTR)pAction->GetTitle());
						return;
					}
				}
			}
		}

		if (sType == EVENTACTION_SHOWNOTIFIER)
		{
			// Only valid in silent mode
			if (m_sMode != MODE_SILENT)
			{
				// Quit
				return;
			}

			// Get right action
			CEventActionShowNotifier * pAction = (CEventActionShowNotifier *)pEventData->GetAction(i);

			// Log
			m_pLog->Log(_T("handle eventaction SHOWNOTIFIER"));

			// Show notifier
			SendMessageToUI(WMU_SHOWNOTIFIER, (LPARAM)(LPCTSTR)pAction->GetTitle(),
				(LPARAM)(LPCTSTR)pAction->GetDescription());
		}
	}
}

//=====================================================================

bool CUpdateProcess::CreateShortcut(CString sTargetFile, CString sParameters,
								CString sLinkFile, CString sDescription, 
								int iShowMode, CString sWorkingDirectory,
								CString sIconFile, int iIconIndex)
{
	// Declare variables
	HRESULT        hRes;                  /* Returned COM result code */
	IShellLink *   pShellLink;            /* IShellLink object pointer */
	IPersistFile * pPersistFile;          /* IPersistFile object pointer */
	WCHAR          wszLinkfile[MAX_PATH]; /* pszLinkfile as Unicode string */
	bool	       bResult = true;

	// Create instance of IShellLink object
	hRes = CoCreateInstance(CLSID_ShellLink,      /* pre-defined CLSID of the IShellLink object */
							NULL,                 /* pointer to parent interface if part of aggregate */
							CLSCTX_INPROC_SERVER, /* caller and called code are in same process */
							IID_IShellLink,      /* pre-defined interface of the IShellLink object */
							(LPVOID*) &pShellLink);         /* Returns a pointer to the IShellLink object */

	// Check if we succeeded
	if (SUCCEEDED(hRes))
	{
		/* Set the fields in the IShellLink object */
		pShellLink->SetPath(sTargetFile);
		pShellLink->SetArguments(sParameters);

		if (sDescription.GetLength() > 0)
		{
			pShellLink->SetDescription(sDescription);
		}

		if (iShowMode > 0)
		{
			pShellLink->SetShowCmd(iShowMode);
		}

		if (lstrlen(sWorkingDirectory) > 0)
		{
			pShellLink->SetWorkingDirectory(sWorkingDirectory);
		}

		if (lstrlen(sIconFile) > 0 && iIconIndex >= 0)
		{
			pShellLink->SetIconLocation(sIconFile, iIconIndex);
		}

		/* Use the IPersistFile object to save the shell link */
		hRes = pShellLink->QueryInterface(IID_IPersistFile, /* pre-defined interface of the IPersistFile object */
										  (LPVOID *) &pPersistFile);    /* returns a pointer to the IPersistFile object */
		if (SUCCEEDED(hRes))
		{
			lstrcpy(wszLinkfile, sLinkFile);
			//iWideCharsWritten = MultiByteToWideChar(CP_ACP, 0, sLinkFile, -1, wszLinkfile, MAX_PATH);
			hRes = pPersistFile->Save(wszLinkfile, TRUE);
			pPersistFile->Release();
		}
		else
		{
			// We failed
			bResult = false;
		}
		pShellLink->Release();
	}
	else
	{
		// We failed
		bResult = false;
	}

	// Return result
	return bResult;
}

//=====================================================================

void CUpdateProcess::OnThreadMessage(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iThreadAction = wParam;
	int iTask = lParam;
	CString sTemp;

	// Log
	sTemp.Format(_T("CUpdateProcess::OnThreadMessage -> wParam = %d; lParam = %d"), iThreadAction, iTask);
	m_pLog->Log(sTemp);

	// Check what task to perform for thread
	switch (iThreadAction)
	{
	case THREAD_PAUSE:
		// Log
		m_pLog->Log(_T("CUpdateProcess::OnThreadMessage -> pause update process"));

		// Pause thread
		SuspendThread();
		break;

	case THREAD_RESUME:
		// Log
		m_pLog->Log(_T("CUpdateProcess::OnThreadMessage -> resume update process"));

		// Resume thread
		ResumeThread();
		break;

	case THREAD_STARTNEXTTASK:
		// Log
		m_pLog->Log(_T("CUpdateProcess::OnThreadMessage -> start next task"));

		// Start next task
		StartTask();
		break;

	case THREAD_SKIPNEXTTASK:
		// Log
		m_pLog->Log(_T("CUpdateProcess::OnThreadMessage -> skip next task"));

		// Skip next task
		m_iCurrentTask++;
		break;

	case THREAD_CANCEL:
		// Log
		m_pLog->Log(_T("CUpdateProcess::OnThreadMessage -> cancel update process"));

		// User cancelled update process
		m_bUpdateCancelled = true;

		// Stop current thread
		if (m_pCurrentThread)
		{
			// Stop thread
			m_pCurrentThread->PostThreadMessage(WMU_THREADMESSAGE, THREAD_CANCEL, 0);

			// Wait until it is really finished
			WaitForSingleObject(m_pCurrentThread->m_hThread, INFINITE);
		}

		// Only start if user is installing or further
		if (m_iCurrentTask >= TASK_INSTALL)
		{
			// Rollback changes
			StartTask(TASK_ROLLBACK);
		}
		else
		{
			// Just close Updater
			PostThreadMessage(WM_QUIT, 0, 0);
		}
		break;
		
	case THREAD_FINISH:
		// Log
		m_pLog->Log(_T("CUpdateProcess::OnThreadMessage -> finish update process"));

		// We are finished
		m_bFinished = true;
		
		// Exit thread
		PostThreadMessage(WM_QUIT, 0, 0);
		break;

	case THREAD_STARTTASK:
		// Log
		m_pLog->Log(_T("CUpdateProcess::OnThreadMessage -> start specific task"));

		// Check if task isn't running already
		if (iTask > m_iCurrentTask)
		{
			// Start a specific task
			StartTask(iTask);
		}
		break;
	}
}

//=====================================================================

void CUpdateProcess::OnTaskComplete(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iTask = (int)wParam;
	unsigned long ulCounter = 0;

	// Check if process is cancelled
	if (m_bUpdateCancelled)
	{
		// We are only allowed to handle TASK_ROLLBACK
		if (iTask != TASK_ROLLBACK)
			return;
	}

	if (!IsBadReadPtr(m_pCurrentThread, sizeof(CWinThread *)))
	{
		// Wait for the thread to be really closed, not forever,
        // be smart to come out of any stale state.
        while ((++ulCounter < 60) && !IsBadReadPtr(m_pCurrentThread, sizeof(CWinThread *)))
        {
			// Log
		    m_pLog->Log(_T("CUpdateProcess::OnTaskComplete - Waiting..."));

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
		// Log
		m_pLog->Log(_T("task ready: TASK_INIT"));
		// Check what user interface we should run in
		SetMode(m_pSettings->GetRunmode());
		break;

	case TASK_CHECKVERSION:
		// Log
		m_pLog->Log(_T("task ready: TASK_CHECKVERSION"));
		// Handle OnNewVersion event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_ONNEWVERSION));
		break;

	case TASK_HTMLMESSAGE:
		// Log
		m_pLog->Log(_T("task ready: TASK_HTMLMESSAGE"));
		// Handle AfterHtmlMessage event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_AFTERHTMLMESSAGE));
		// Start next task
		StartTask();
		break;

	case TASK_LICENSE:
		// Log
		m_pLog->Log(_T("task ready: TASK_LICENSE"));
		// Handle AfterLicense event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_AFTERLICENSE));
		// Start next task
		StartTask();
		break;

	case TASK_PROTECTION:
		// Log
		m_pLog->Log(_T("task ready: TASK_PROTECTION"));
		// Handle AfterProtection event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_AFTERPROTECTION));
		break;

	case TASK_SELECTUPDATE:
		// Select the updates (this is a little bit different because
		// this is needed after the user selected the updates)
		if (!SelectUpdate())
		{
			// Stop executing this thread
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}

		// Wait until thread is ready
		if (!IsBadReadPtr(m_pCurrentThread, sizeof(CWinThread *)))
		{
			// Wait for the thread to be really closed
			WaitForSingleObject(m_pCurrentThread->m_hThread, INFINITE);

			// Thread is closed, set to NULL
			m_pCurrentThread = NULL;
		}

		// Log
		m_pLog->Log(_T("task ready: TASK_SELECTUPDATE"));
		// Handle AfterSelectUpdate event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_AFTERSELECTUPDATE));
		// Start next task
		StartTask();
		break;

	case TASK_DOWNLOAD:
		// Log
		m_pLog->Log(_T("task ready: TASK_DOWNLOAD"));
		// Handle AfterDownload event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_AFTERDOWNLOAD));
		break;

	case TASK_INSTALL:
		// Log
		m_pLog->Log(_T("task ready: TASK_INSTALL"));
		// Install is complete
		m_bInstallComplete = true;
		// Handle AfterInstall event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_AFTERINSTALL));
		break;

	case TASK_ROLLBACK:
		// Log
		m_pLog->Log(_T("task ready: TASK_ROLLBACK"));
		// Install is not complete
		m_bInstallComplete = false;
		// Handle AfterRollback event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_AFTERROLLBACK));
		break;

#if defined _DEBUG || defined _BETA
	default:
		// We have a problem, stop thread!
		AfxMessageBox(_T("OOPS! Unknown task!"));
		PostThreadMessage(WM_QUIT, 0, 0);
		break;
#endif
	}

	// Always send this message to user interface
	SendMessageToUI(WMU_TASK_COMPLETE, wParam, lParam);
}

//=====================================================================

void CUpdateProcess::OnError(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iError = (int)wParam;
	int iTask = (int)lParam;
	CString sTemp;

	// Check what error occurred
	switch (iError)
	{
	case ERROR_UNKNOWN:
		// Log
		m_pLog->Log(_T("ERROR: unknown error"));
		break;

	case ERROR_NOCONNECTION:
		// Log
		m_pLog->Log(_T("ERROR: no connection"));
		break;

	case ERROR_SERVER:
		// Log
		m_pLog->Log(_T("ERROR: server"));
		break;

	case ERROR_NODISKSPACE:
		// Log
		m_pLog->Log(_T("ERROR: no disk space"));
		break;

	case ERROR_FILENOTFOUND:
		// Log
		sTemp.Format(_T("ERROR: file %s not found"), lParam);
		m_pLog->Log(sTemp);
		break;

	case ERROR_COPYFILE:
		// Log
		sTemp.Format(_T("ERROR: copy file %s"), lParam);
		m_pLog->Log(sTemp);
		break;

	case ERROR_DELETEFILE:
		// Log
		sTemp.Format(_T("ERROR: delete file %s"), lParam);
		m_pLog->Log(sTemp);
		break;

	case ERROR_RUNFILE:
		// Log
		sTemp.Format(_T("ERROR: run file %s"), lParam);
		m_pLog->Log(sTemp);
		break;

	case ERROR_CLOSEAPP:
		// Log
		m_pLog->Log(_T("ERROR: close app"));
		break;

	case ERROR_FILECORRUPT:
		// Log
		sTemp.Format(_T("ERROR: file %s corrupt"), lParam);
		m_pLog->Log(sTemp);
		break;

	case ERROR_NONEWVERSION:
		// Log
		m_pLog->Log(_T("No new version is found"));
		// Handle OnNoNewVersion event
		HandleEvent(m_pEventInfo->GetEventInfo(EVENT_ONNONEWVERSION));
		break;

	case ERROR_WRITEFILE:
		// Log
		sTemp.Format(_T("ERROR: write file %s"), lParam);
		m_pLog->Log(sTemp);
		break;

	case ERROR_UNZIPFILE:
		// Log
		sTemp.Format(_T("ERROR: unzip file %s"), lParam);
		m_pLog->Log(sTemp);
		break;

	case ERROR_PROTECTION:
		// Log
		m_pLog->Log(_T("ERROR: protection test failed"));
		break;
	}

	// First send message to user interface
	SendMessageToUI(WMU_ERROR, wParam, lParam);
}

//=====================================================================

void CUpdateProcess::OnWarning(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iWarning = (int)wParam;
	int iTask = (int)lParam;
	CString sTemp;

	// Check what warning is given
	switch (iWarning)
	{
	case WARNING_REGISTER:
		// Log
		sTemp.Format(_T("WARNING: file %s not registered"), lParam);
		m_pLog->Log(sTemp);
		break;

#if defined _DEBUG || defined _BETA
	default:
		// We have a problem, no unknown warning should be sent!
		AfxMessageBox(_T("WARNING: unknown warning, this is bad!"));
		PostThreadMessage(WM_QUIT, 0, 0);
		break;
#endif
	}

	// Send message to user interface
	SendMessageToUI(WMU_WARNING, wParam, lParam);
}

//=====================================================================

void CUpdateProcess::OnWarningComplete(WPARAM wParam, LPARAM lParam)
{
	// Send to thread so it can continue
	m_pCurrentThread->PostThreadMessage(WMU_WARNING_COMPLETE, wParam, lParam);
}

//=====================================================================

void CUpdateProcess::OnQuestion(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iQuestion = (int)wParam;
	CString sTemp;

	// Log question
	sTemp.Format(_T("QUESTION_ID: %d"), iQuestion);
	m_pLog->Log(sTemp);

	// Send message to user interface
	SendMessageToUI(WMU_QUESTION, wParam, lParam);
}

//=====================================================================

void CUpdateProcess::OnQuestionAnswer(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iQuestion = (int)wParam;
	int iAnswer = (int)lParam;
	CString sTemp;

	// Log question answer
	sTemp.Format(_T("QUESTIONANSWER_ID: question: %d -> answer: %d"), iQuestion, iAnswer);
	m_pLog->Log(sTemp);

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

//=====================================================================

void CUpdateProcess::OnUpdateGUI(WPARAM wParam, LPARAM lParam)
{
	// Send message to user interface
	SendMessageToUI(WMU_UPDATE_GUI, wParam, lParam);
}

//=====================================================================

void CUpdateProcess::OnUpdateGUIFileProgress(WPARAM wParam, LPARAM lParam)
{
	// Send message to user interface
	SendMessageToUI(WMU_UPDATE_GUI_FILEPROGRESS, wParam, lParam);
}

//=====================================================================

void CUpdateProcess::OnUpdateGUITotalProgress(WPARAM wParam, LPARAM lParam)
{
	// Send message to user interface
	SendMessageToUI(WMU_UPDATE_GUI_TOTALPROGRESS, wParam, lParam);
}

//=====================================================================

void CUpdateProcess::OnUpdateGUIProtection(WPARAM wParam, LPARAM lParam)
{
	// Are we stopped yet?
	if (!m_bUpdateCancelled)
	{
		// Send message to user interface
		SendMessageToUI(WMU_UPDATE_GUI_PROTECTION, wParam, lParam);
	}
}

//=====================================================================

void CUpdateProcess::OnSwitchUI(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sUI;

	// Get user interface
	sUI.Format(_T("%s"), wParam);

	// Switch to right user interface
	SetMode(sUI);

	// Send message to UI
	SendMessageToUI(m_oLastUIMessage.message, m_oLastUIMessage.lParam, m_oLastUIMessage.wParam, false); 
}

//=====================================================================

void CUpdateProcess::OnShowAbout(WPARAM wParam, LPARAM lParam)
{
	// If current thread is valid, pause it
	if (m_pCurrentThread)
		m_pCurrentThread->SuspendThread();

	// Log
	m_pLog->Log(_T("ABOUT: user wants to see about dialog"));

	// Show about box
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();

	// Log
	m_pLog->Log(_T("ABOUT: user closed about dialog"));

	// Resume thread
	if (m_pCurrentThread)
		m_pCurrentThread->ResumeThread();
}

