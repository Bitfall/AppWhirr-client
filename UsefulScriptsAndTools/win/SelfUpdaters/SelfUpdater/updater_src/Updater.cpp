/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   9:10
	filename: 	Updater.cpp
	file path:	Updater
	file base:	Updater
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "UpdateProcess.h"
#include "CheckForUpdatesProcess.h"
#include "Updater.h"

#include "FileVersion.h"
#include "RestoreDlg.h"
#include "ProxySettingsDlg.h"
#include "SelectLanguageDlg.h"
#include "LogViewerDlg.h"

#include "ErrorDlg.h"
#include "WarningDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CUpdaterApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CUpdaterApp::CUpdaterApp()
{
}

//=====================================================================

CUpdaterApp theApp;

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CUpdaterApp::InitInstance()
{
	// Set default return value
	m_iReturnValue = RETURN_OK;

	// Try
	try
	{
		// Catch any errors
		return Main();
	}
	catch (exception & ex)
	{
		// Error
		m_iReturnValue = RETURN_ERROR;

#ifdef _DEBUG
		ex.what();
#endif
	}
	catch (CException * pEx)
	{
		// Error
		m_iReturnValue = RETURN_ERROR;

#ifdef _DEBUG
		pEx->ReportError();
#endif
		pEx->Delete();
	}

	// If we get here, return 0
	return FALSE;
}

//=====================================================================

int CUpdaterApp::ExitInstance()
{
	// Call original function
	CWinApp::ExitInstance();

	// Return result code
	return m_iReturnValue;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

BOOL CUpdaterApp::Main()
{
	// Initialise common control library (xp look)
	InitCommonControls();
	AfxEnableControlContainer();

	// Declare variables
	CString sTemp, sCommandLine, sSettingsFile;
	bool bLanguageChosen = false;
	CSettings * pSettings = CSettings::Instance();

	// Updater is not allowed to run when no user is logged in
	if (!IsUserLoggedIn())
	{
		// Quit updater
		return FALSE;
	}

	// Check what kind of release we are dealing with
#ifdef _BETA
	// We are using beta, show it to the user
	CWarningDlg dlgWarning(_T("Warning"), _T("Updater beta"), 
		_T("You are using a beta version of Updater. This version is for testing purposes only!\n\nUse an official release if you want to use Updater in your software."));
	dlgWarning.DoModal();
#endif

	// Initialize all paths
	CPath * pPath = CPath::Instance();
	if (!pPath->Init())
		return FALSE;

	// Parse command line to get settings file
	try
	{
		sSettingsFile = pSettings->ParseCommandLineForSettingsFile(m_lpCmdLine);
	}	
	catch (exception & ex)
	{
		// Declare variables
		CString sTitle, sExplanation, sMore;
		CLanguage * pLanguage = CLanguage::Instance();

		// Set up error message
		sTitle = _T("Parameter parsing error");
		sExplanation.Format(_T("The following exception occurred when parsing parameters:\n\n\"%s\".\nCommandline: %s"), ex.what(), m_lpCmdLine);
		sMore = _T("Check if the parameters being used are correct.\n\nIf the error keeps occurring, contact the developer at ") +
			UPDATER_WEBADDRESS + _T(".");

		// Show error dialog
		CErrorDlg dlgError(sTitle, pLanguage->GetString(IDS_ERROR_TITLE), sExplanation,
			sMore, pLanguage->GetString(IDS_GENERAL_OK), pLanguage->GetString(IDS_ERROR_MORE));
		dlgError.DoModal();

		// Quit Updater
		return FALSE;
	}

	// Read settings.ini
	if (!PathFileExists(sSettingsFile))
	{
		CInternalData::Instance()->SetSettingsFile(pPath->GetPathUpdater() + FILENAME_SETTINGS);
	}
	else
	{
		CInternalData::Instance()->SetSettingsFile(sSettingsFile);
	}
	pSettings->ReadFile(CInternalData::Instance()->GetSettingsFile());

	// Always delete UpdaterCopy.exe from temp dir
	DeleteFile(pPath->GetPathTemp() + _T("UpdaterCopy.exe"));

	// Parse command line, called after the settings, because this
	// should overrule the settings from the ini file
	try
	{
		pSettings->ParseCommandLine(m_lpCmdLine);
	}
	catch (exception & ex)
	{
		// Declare variables
		CString sTitle, sExplanation, sMore;
		CLanguage * pLanguage = CLanguage::Instance();

		// Set up error message
		sTitle = _T("Parameter parsing error");
		sExplanation.Format(_T("The following exception occurred when parsing parameters:\n\n\"%s\".\nCommandline: %s"), ex.what(), m_lpCmdLine);
		sMore = _T("Check if the parameters being used are correct.\n\nIf the error keeps occurring, contact the developer at ") +
			UPDATER_WEBADDRESS + _T(".");

		// Show error dialog
		CErrorDlg dlgError(sTitle, pLanguage->GetString(IDS_ERROR_TITLE), sExplanation,
			sMore, pLanguage->GetString(IDS_GENERAL_OK), pLanguage->GetString(IDS_ERROR_MORE));
		dlgError.DoModal();

		// Quit Updater
		return FALSE;
	}

	// Create mutex, because there cannot be 2 instances of Updater for same application
	HANDLE hMutex = CreateMutex(NULL, FALSE, _T("updater_") + pSettings->GetAppName()); 

	// Check if mutex is created succesfully
	switch(GetLastError())
	{
	case ERROR_SUCCESS:
		// Mutex created successfully. There is no instance running
		break;
		
	case ERROR_ALREADY_EXISTS:
		// Mutex already exists so there is a running instance of our app.
		return FALSE;
		
	default:
		// Failed to create mutex by unknown reason
		return FALSE;
	}

	// Create needed folders such as temp and rollback
	if (!pPath->CreateUpdaterFolders())
		return FALSE;

	// Load the right language - auto
	CLanguage * pLanguage = CLanguage::Instance();
	if ((pSettings->GetLanguage() == LANGUAGETYPE_AUTO) && !bLanguageChosen)
	{
		// We have chosen a language
		bLanguageChosen = true;

		// Automatically load
		pLanguage->LoadLanguage(GetSystemDefaultLangID());
	}

	// Load the right language - user
	if ((pSettings->GetLanguage() == LANGUAGETYPE_USER) && !bLanguageChosen)
	{
		// We have chosen a language
		bLanguageChosen = true;

		// Let user choose
		CSelectLanguageDlg dlgLanguage;
		dlgLanguage.DoModal();

		// Load chosen language
		sTemp.Format(_T("%s\\lng\\%s.xml"), pPath->GetPathUpdater(), pSettings->GetLanguage());
		pLanguage->LoadLanguage(sTemp, pSettings->GetLanguage());
	}

	// Load the right language - settings
	if (!bLanguageChosen)
	{
		// Load chosen language
		sTemp.Format(_T("%s\\lng\\%s.xml"), pPath->GetPathUpdater(), pSettings->GetLanguage());
		pLanguage->LoadLanguage(sTemp, pSettings->GetLanguage());
	}

	// Run right part of application
	sCommandLine.Format(_T("%s"), m_lpCmdLine);
	StartApplication(sCommandLine);

	// Release language
	pLanguage->ReleaseLanguage();

	// Exit Updater
	return TRUE;
}

//=====================================================================

void CUpdaterApp::StartApplication(CString sCommandLine)
{
	// Proxy settings
	if ((sCommandLine.Find(PARAMETER_PROXYSETTINGS) >= 0))
	{
		// Edit proxy settings
		StartProxySettings();
		return;		
	}

	// Restore
	if ((sCommandLine.Find(PARAMETER_RESTORE) >= 0))
	{
		// Start restoring
		StartRestore();
		return;
	}

	// Log viewer
	if ((sCommandLine.Find(PARAMETER_LOG) >= 0))
	{
		// Start log viewer
		StartViewLog();
		return;
	}

	// Check for updates
	if ((sCommandLine.Find(PARAMETER_CHECKFORUPDATES) >= 0))
	{
		// Start check for updates
		StartCheckForUpdates();
		return;
	}

	// Run Updater
	StartUpdater();
}

//=====================================================================

void CUpdaterApp::StartUpdater()
{
	// Declare variables
	CPath * pPath = CPath::Instance();
	CSettings * pSettings = CSettings::Instance();
	CString sTemp, sUpdaterVersion;;
	LARGE_INTEGER iFrequency, iTimeStart, iTimeEnd;

	// Calculate frequency
	QueryPerformanceFrequency(&iFrequency);

	// Record the time
	QueryPerformanceCounter(&iTimeStart);

	// Update to new version, create log file
	CFileVersion fileVersion;
	fileVersion.Open(pPath->GetPathUpdaterExecutable());
	sUpdaterVersion = fileVersion.GetFileVersion();
	CLog * pLog = CLog::Instance();
	pLog->CreateLog(pPath->GetPathTemp() + _T("Updater log for ") + pSettings->GetAppName() + _T(".txt"), false);
	pLog->Log(_T("UPDATER LOG FILE"));
	pLog->Log(_T("================"));
	pLog->Log(_T(""));
	pLog->Log(_T("Updater is developed by ") + UPDATER_DEVELOPER + _T(", ") + UPDATER_YEARS);
	pLog->Log(_T(""));
	pLog->Log(_T("Visit ") + UPDATER_WEBADDRESS + _T(" for more information."));
	pLog->Log(_T(""));
	pLog->Log(_T("*****************************************************"));
	fileVersion.Close();
	if (fileVersion.Open(pSettings->GetAppLocation()))
	{
		pLog->Log(_T(""));
		pLog->Log(_T("Application: ") + pSettings->GetAppName());
		pLog->Log(_T(""));
		pLog->Log(_T("Additional application data (begin)"));
		pLog->Log(_T("  Application data - product name:    ") + fileVersion.GetProductName());
		pLog->Log(_T("  Application data - current version: ") + fileVersion.GetFileVersion());
		pLog->Log(_T("  Application data - developer:       ") + fileVersion.GetCompanyName());
		pLog->Log(_T("  Application data - internal name:   ") + fileVersion.GetInternalName());
		pLog->Log(_T("  Application data - copyright:       ") + fileVersion.GetLegalCopyright());
		pLog->Log(_T("Additional application data (end)"));
		pLog->Log(_T(""));
		pLog->Log(_T("*****************************************************"));
		fileVersion.Close();
	}

	pLog->Log(_T(""));
	sTemp.Format(_T("Commandline: %s"), m_lpCmdLine);
	pLog->Log(sTemp);
	pLog->Log(_T(""));
	pLog->Log(_T("*****************************************************"));

	pLog->Log(_T(""));
	pLog->EnableTimestamp(true);

	// Start main update thread
	CUpdateProcess * pUpdateProcess;
	pUpdateProcess = (CUpdateProcess *)AfxBeginThread(RUNTIME_CLASS(CUpdateProcess),
													  THREAD_PRIORITY_NORMAL,
													  0,
													  0,
													  NULL);

	// Wait for thread to be ready
	WaitForSingleObject(pUpdateProcess->m_hThread, INFINITE);

	// Stop the time
	QueryPerformanceCounter(&iTimeEnd);

	// Save time to log file
	sTemp.Format(_T("Update took %s (hh:mm:ss:ms)"), ConvertTime((iTimeEnd.QuadPart - iTimeStart.QuadPart)
		/ (iFrequency.QuadPart / 1000)));
	pLog->EnableTimestamp(false);
	pLog->Log(_T(""));
	pLog->Log(_T("-------------------------------------"));
	pLog->Log(sTemp);
	pLog->Log(_T("-------------------------------------"));
	pLog->Log(_T(""));
	pLog->Log(_T("Close Updater"));

	// Update thread is ready
	pUpdateProcess = NULL;

	// Close log file
	pLog->CloseLog();
}

//=====================================================================

void CUpdaterApp::StartProxySettings()
{
	// Show proxy settings
	CProxySettingsDlg dlg;
	dlg.DoModal();
}

//=====================================================================

void CUpdaterApp::StartRestore()
{
	// Restore older version
	CRestoreDlg dlg;
	dlg.DoModal();
}

//=====================================================================

void CUpdaterApp::StartViewLog()
{
	// Show last log
	CLogViewerDlg dlg;
	dlg.DoModal();
}

//=====================================================================

void CUpdaterApp::StartCheckForUpdates()
{
	// Declare variables
	ULONG lResult = 0;

	// Start check for updates thread
	CCheckForUpdatesProcess * pCheckProcess;
	pCheckProcess = (CCheckForUpdatesProcess *)AfxBeginThread(RUNTIME_CLASS(CCheckForUpdatesProcess),
													 THREAD_PRIORITY_NORMAL,
													 0,
													 0,
													 NULL);

	// Don't auto delete the thread
	pCheckProcess->m_bAutoDelete = FALSE;

	// Wait until thread is complete
	WaitForSingleObject(pCheckProcess->m_hThread, INFINITE);

	// Get return code
	if (GetExitCodeThread(pCheckProcess->m_hThread, &lResult))
	{
		m_iReturnValue = lResult;
	}	
	else
	{
		// An error occurred
		m_iReturnValue = RETURN_ERROR;
	}

	// Delete thread object
	delete pCheckProcess;
}

//=====================================================================

bool CUpdaterApp::IsUserLoggedIn()
{
	// Declare variables
	bool bLoggedIn = false;

	// Get current desktop
	HDESK hDesk = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);

	// If we have a valid desktop
	if (hDesk)
	{
		// Get user object information
		SID sid;
		DWORD dwSidLength = 0;
		BOOL bSuccess = GetUserObjectInformation(hDesk, UOI_USER_SID, &sid, sizeof(SID), &dwSidLength);
		
		// Did we succeed or did we have a buffer error?
		if (!bSuccess && ERROR_INSUFFICIENT_BUFFER == GetLastError())
		{
			SID *psid = (SID *)malloc(dwSidLength);
			if (psid)
			{
				bSuccess = GetUserObjectInformation(hDesk, UOI_USER_SID, psid, dwSidLength, &dwSidLength);
				free(psid);
			}
		}

		// Did we succeed?
		if (bSuccess)
		{
			if (dwSidLength)
			{
				// A user has logged in
				bLoggedIn = true;
			}
			else
			{
				// No user has logged in
				bLoggedIn = false;
			}
		}

		// Close desktop
		CloseDesktop(hDesk);
	}

	// Return result
	return bLoggedIn;
}

//=====================================================================

CString CUpdaterApp::ConvertTime(int iTimeInMilliSeconds)
{
	// Declare variables
	int iHours, iMinutes, iSeconds;
	CString sResult;

	// Set default values
	iHours = 0;
	iMinutes = 0;
	iSeconds = 0;

	// Calculate hours
	while (iTimeInMilliSeconds > 3600000)
	{
		iHours++;
		iTimeInMilliSeconds -= 3600000;
	}

	// Calculate minutes
	while (iTimeInMilliSeconds > 60000)
	{
		iMinutes++;
		iTimeInMilliSeconds -= 60000;
	}

	// Calculate seconds
	while (iTimeInMilliSeconds > 1000)
	{
		iSeconds++;
		iTimeInMilliSeconds -= 1000;
	}

	// Setup string
	sResult.Format(_T("%d:%02d:%02d:%03d"), iHours, iMinutes, iSeconds, iTimeInMilliSeconds);

	// Return time
	return sResult;
}
