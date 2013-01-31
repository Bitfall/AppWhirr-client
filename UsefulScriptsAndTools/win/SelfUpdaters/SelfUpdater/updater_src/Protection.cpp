/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:55
	filename: 	Protection.cpp
	file path:	Updater
	file base:	Protection
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Protection.h"
#include "md5.h"

typedef bool (* REGISTRATIONPROC)(); 

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CProtection, CWinThread)
	ON_THREAD_MESSAGE(WMU_SETPARENT, OnSetParent)
	ON_THREAD_MESSAGE(WMU_THREADMESSAGE, OnThreadMessage)
	ON_THREAD_MESSAGE(WMU_DOWNLOAD_COMPLETE, OnDownloadComplete)
	ON_THREAD_MESSAGE(WMU_ERROR, OnError)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CProtection, CWinThread)

//=====================================================================

CProtection::CProtection()
{
}

//=====================================================================

CProtection::~CProtection()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CProtection::InitInstance()
{
	// Set parent to NULL
	m_pParent = NULL;

	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pInternet = CInternet::Instance();
	m_pPath = CPath::Instance();

	// Set parent for internet
	m_pInternet->SetParent(this);

	// We are not started yet
	m_bStarted = false;
	
	return TRUE;
}

//=====================================================================

int CProtection::ExitInstance()
{
	// Stop all download processes that "might" be running
	m_pInternet->StopDownloading();

	// Auto clean-up thread
	m_bAutoDelete = TRUE;

	// Call original function
	return CWinThread::ExitInstance();
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CProtection::Main()
{
	// We are started
	m_bStarted = true;

	// Should we check for license?
	if (m_pUpdateInfo->GetProtectionRegistrationDll().IsEmpty())
	{
		// No
		// Check hashes
		if (CheckHashes())
		{
			// Succeeded
			PostMessageToParent(WMU_UPDATE_GUI_PROTECTION, PROTECTION_TASK_HASH,
				PROTECTION_STATE_OK);
		}
		else
		{
			// Not succeeded
			PostMessageToParent(WMU_UPDATE_GUI_PROTECTION, PROTECTION_TASK_HASH,
				PROTECTION_STATE_NOTOK);

			PostMessageToParent(WMU_ERROR, ERROR_PROTECTION, 0);

			// Quit thread
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
	}
	else
	{
		// Yes
		// Download file
		if (m_pInternet->DownloadFile(m_pUpdateInfo->GetProtectionRegistrationDll(),
			m_pPath->GetPathUpdaterTemp() + "\\upd_reg_prot.dll"))
		{
			// Update state
			PostMessageToParent(WMU_UPDATE_GUI_PROTECTION, PROTECTION_TASK_LICENSE,
				PROTECTION_STATE_CHECKING);
		}
		else
		{
			// Show error
			PostMessageToParent(WMU_ERROR, ERROR_DOWNLOADFILE, (LPARAM) _T("Protection DLL"));
		}
	}
}

//=====================================================================

bool CProtection::CheckRegistration()
{
	// Declare variables
	bool bResult = false;
	HMODULE hModule;
	REGISTRATIONPROC CheckLicense; 

	// Load dll
	hModule = LoadLibrary(m_pPath->GetPathUpdaterTemp() + "\\upd_reg_prot.dll");

	// Check if dll is loaded successfully
	if (hModule)
	{
		// Get proc address
		CheckLicense = (REGISTRATIONPROC) GetProcAddress(hModule, "CheckLicense");

		// Execute function in dll
		if (CheckLicense != NULL)
		{
			bResult = CheckLicense();
		}

		// Release dll
		FreeLibrary(hModule);
	}

	// Delete dll again
	DeleteFile(m_pPath->GetPathUpdaterTemp() + "\\upd_reg_prot.dll");

	// Return result
	return bResult;
}

//=====================================================================

bool CProtection::CheckHashes()
{
	// Declare variables
	CString sHash;
	bool bResult = true;

	// Update state
	PostMessageToParent(WMU_UPDATE_GUI_PROTECTION, PROTECTION_TASK_HASH,
		PROTECTION_STATE_CHECKING);

	// Hash all files
	for (int i = 0; i < m_pUpdateInfo->GetProtectionHashCount(); i++)
	{
		// Hash file
		sHash = m_pFunctions->GetFileMD5Hash(m_pUpdateInfo->GetProtectionHashData(i).sFilename);

		// Compare hashes
		if (sHash != m_pUpdateInfo->GetProtectionHashData(i).sHash)
			bResult = false;
	}

	// Return result
	return bResult;
}

//=====================================================================

void CProtection::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

void CProtection::OnDownloadComplete(WPARAM wParam, LPARAM lParam)
{
	// Check registration
	if (CheckRegistration())
	{
		// Succeeded
		PostMessageToParent(WMU_UPDATE_GUI_PROTECTION, PROTECTION_TASK_LICENSE,
			PROTECTION_STATE_OK);
	}
	else
	{
		// Not succeeded
		PostMessageToParent(WMU_UPDATE_GUI_PROTECTION, PROTECTION_TASK_LICENSE,
			PROTECTION_STATE_NOTOK);

		PostMessageToParent(WMU_ERROR, ERROR_PROTECTION, 0);

		// Quit thread
		PostThreadMessage(WM_QUIT, 0, 0);
		return;
	}

	// Check hashes
	if (CheckHashes())
	{
		// Succeeded
		PostMessageToParent(WMU_UPDATE_GUI_PROTECTION, PROTECTION_TASK_HASH,
			PROTECTION_STATE_OK);
	}
	else
	{
		// Not succeeded
		PostMessageToParent(WMU_UPDATE_GUI_PROTECTION, PROTECTION_TASK_HASH,
			PROTECTION_STATE_NOTOK);

		PostMessageToParent(WMU_ERROR, ERROR_PROTECTION, 0);

		// Quit thread
		PostThreadMessage(WM_QUIT, 0, 0);
		return;
	}

	// Send message to parent that we are ready
	PostMessageToParent(WMU_TASK_COMPLETE, TASK_PROTECTION, 0);

	// We are ready, no need to notify the parent, because this is done after the
	// real thread. Just stop yourself and the parent will go on
	PostThreadMessage(WM_QUIT, 0, 0);
}

//=====================================================================

void CProtection::OnError(WPARAM wParam, LPARAM lParam)
{
	// Protection check failed
	PostMessageToParent(WMU_UPDATE_GUI_PROTECTION, PROTECTION_TASK_LICENSE,
		PROTECTION_STATE_NOTOK);

	PostMessageToParent(WMU_ERROR, ERROR_PROTECTION, 0);
}

//=====================================================================

void CProtection::OnSetParent(WPARAM wParam, LPARAM lParam)
{
	// Set parent
	m_pParent = (CWinThread *)wParam;
}

//=====================================================================

void CProtection::OnThreadMessage(WPARAM wParam, LPARAM lParam)
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
			// Start
			Main();
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