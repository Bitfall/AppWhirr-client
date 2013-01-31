/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:53
	filename: 	CheckVersion.cpp
	file path:	Updater
	file base:	CheckVersion
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "CheckVersion.h"
#include "ConnectionTest.h"			// Test internet connection

typedef LPTSTR (* UPDATESELECTORPROC)(); 

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CCheckVersion, CWinThread)
	ON_THREAD_MESSAGE(WMU_DOWNLOAD_COMPLETE, OnDownloadComplete)
	ON_THREAD_MESSAGE(WMU_ERROR, OnError)
	ON_THREAD_MESSAGE(WMU_SETPARENT, OnSetParent)
	ON_THREAD_MESSAGE(WMU_THREADMESSAGE, OnThreadMessage)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CCheckVersion, CWinThread)

//=====================================================================

CCheckVersion::CCheckVersion()
{
}

//=====================================================================

CCheckVersion::~CCheckVersion()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CCheckVersion::InitInstance()
{
	// Set parent to NULL
	m_pParent = NULL;

	// We are not started yet
	m_bStarted = false;
	m_bNoFailPossible = false;
	m_bUpdateSelector = false;

	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pInternet = CInternet::Instance();
	m_pSections = CSections::Instance();
	m_pPath = CPath::Instance();

	// Set parent for internet
	m_pInternet->SetParent(this);

	return TRUE;
}

//=====================================================================

int CCheckVersion::ExitInstance()
{
	// Always stop the downloads
	m_pInternet->StopDownloading();

	// Auto clean-up thread
	m_bAutoDelete = TRUE;

	// Call original function
	return CWinThread::ExitInstance();
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CCheckVersion::Main()
{
	// We are not ready yet
	m_bDownloadsReady = false;

	// Should we check if we are connected?
	if (m_pSettings->GetCheckConnection())
	{
		// Check if we are connected to the internet
		if (!CConnectionTest::Connected())
		{
			// Send error message
			PostMessageToParent(WMU_ERROR, ERROR_NOCONNECTION, 0);
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
	}

	// Should we use update file or update selector?
	if (m_pSettings->GetUpdateSelector().IsEmpty())
	{
		// No update selector
		m_bUpdateSelector = false;

		// Start downloading of update list
		if (!m_pInternet->DownloadFile(m_pSettings->GetURL(),
			m_pPath->GetPathUpdaterTemp() + "updatefile.xml"))
		{
			// Post error
			PostMessageToParent(WMU_ERROR, ERROR_SERVER, 0);
		}
	}
	else
	{
		// Update selector
		m_bUpdateSelector = true;

		// Start downloading of update list
		if (!m_pInternet->DownloadFile(m_pSettings->GetUpdateSelector(),
			m_pPath->GetPathUpdaterTemp() + "updateselector.dll"))
		{
			// Post error
			PostMessageToParent(WMU_ERROR, ERROR_SERVER, 0);
		}
	}
}

//=====================================================================

bool CCheckVersion::CheckSections()
{
	// Search for at least 1 section that needs to be updated
	for (int i = 0; i < m_pUpdateInfo->GetSectionCount(); i++)
	{
		// Check version
		if (m_pFunctions->CompareVersions(m_pUpdateInfo->GetSectionData(i)->GetVersion(), 
			m_pSections->GetSectionVersion(m_pUpdateInfo->GetSectionData(i)->GetName())) == COMPAREVERSION_FIRSTLARGER)
			return true;
	}

	// If we get here, no new version is available
	return false;
}

//=====================================================================

bool CCheckVersion::CheckVersionSeparately()
{
	// Declare variables
	bool bUpdate = false, bUpdateFile;
	bool bFileProcessed;
	CString sFiledate = "";

	// For all files, check difference
	for (int i = 0; i < m_pUpdateInfo->GetFileCount(); i++)
	{
		// Default, don't update the file
		m_pUpdateInfo->GetFileData(i)->SetUpdateFile(false);

		// We haven't processed the file
		bFileProcessed = false;
		bUpdateFile = false;

		// Check by version
		if (m_pUpdateInfo->GetFileData(i)->GetCheckType() == CHECK_VERSION)
		{
			// We processed the file
			bFileProcessed = true;

			// Check if there is a new version available
			if (m_pFunctions->CompareVersions(m_pUpdateInfo->GetFileData(i)->GetVersion(), 
				m_pFunctions->GetFileVersion(m_pUpdateInfo->GetFileData(i)->GetFilename())) == COMPAREVERSION_FIRSTLARGER)
			{
				// There is a new version
				bUpdateFile = true;
			}
		}

		// Check by date
		if (m_pUpdateInfo->GetFileData(i)->GetCheckType() == CHECK_DATE)
		{
			// We processed the file
			bFileProcessed = true;

			// If empty, check automatically
			if (m_pUpdateInfo->GetFileData(i)->GetDate().IsEmpty())
			{
				// Get filedate
				sFiledate = GetInternetFileDate(i, false);
			}
			else
			{
				// Get filedate
				sFiledate = m_pUpdateInfo->GetFileData(i)->GetDate();
			}

			// Compare the date
			if (sFiledate >
				m_pFunctions->GetFileDate(m_pUpdateInfo->GetFileData(i)->GetFilename()))
			{
				// Different version
				bUpdateFile = true;
			}
		}

		// Check by date/time
		if (m_pUpdateInfo->GetFileData(i)->GetCheckType() == CHECK_DATETIME)
		{
			// We processed the file
			bFileProcessed = true;

			// If empty, check automatically
			if (m_pUpdateInfo->GetFileData(i)->GetDate().IsEmpty())
			{
				// Get filedate
				sFiledate = GetInternetFileDate(i, true);
			}
			else
			{
				// Get filedate
				sFiledate = m_pUpdateInfo->GetFileData(i)->GetDate();
			}
			
			// Compare the date
			if (sFiledate >
				m_pFunctions->GetFileDateTime(m_pUpdateInfo->GetFileData(i)->GetFilename()))
			{
				// Different version
				bUpdateFile = true;
			}
		}

		// Check by hash
		if (m_pUpdateInfo->GetFileData(i)->GetCheckType() == CHECK_HASH)
		{
			// We processed the file
			bFileProcessed = true;

			// Compare the hashes
			if (m_pUpdateInfo->GetFileData(i)->GetHash() !=
				m_pFunctions->GetFileMD5Hash(m_pUpdateInfo->GetFileData(i)->GetFilename()))
			{
				// Different hash
				bUpdateFile = true;
			}
		}

		// If we didn't process the file, it should ALWAYS be updated
		if (!bFileProcessed)
		{
			bUpdateFile = true;
		}

		// Store information if we should update the file or not
		m_pUpdateInfo->GetFileData(i)->SetUpdateFile(bUpdateFile);
		if (bUpdateFile)
		{
			// We have at least one file to update
			bUpdate = true;
		}
	}

	// Return the result
	return bUpdate;
}

//=====================================================================

CString CCheckVersion::GetInternetFileDate(int iFileIndex, bool bTimeToo)
{
	// Declare variables
	CString sDate = "1970-01-01";

	// For all download locations, check
	for (int j = 0; j < m_pUpdateInfo->GetFileData(iFileIndex)->GetActionCount(); j++)
	{
		// Get action
		CActionBase * pAction = m_pUpdateInfo->GetFileData(iFileIndex)->GetAction(j);
		
		// Are we dealing with download action?
		if (pAction->GetType() == ACTION_DOWNLOAD)
		{
			// Get download action
			CActionDownload * pDownload = (CActionDownload *)pAction;
			
			// Init k
			int k = 0;

			// Loop all locations
			while (k < pDownload->GetDownloadLocationCount() && 
						((sDate == "1970-01-01") || (sDate == "1970-01-01/00:00:00")))
			{
				// Check what we should get
				if (bTimeToo)
				{
					sDate = m_pInternet->GetFileDateTime(pDownload->GetDownloadLocation(k));
				}
				else
				{
					sDate = m_pInternet->GetFileDate(pDownload->GetDownloadLocation(k));
				}
			}
		}
	}

	// Return value
	return sDate;
}

//=====================================================================

void CCheckVersion::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

void CCheckVersion::OnDownloadComplete(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sUpdateLocation;
	HMODULE hModule;
	UPDATESELECTORPROC SelectUpdate; 

	// Did we download update selector?
	if (m_bUpdateSelector)
	{
		// Don't handle this code again
		m_bUpdateSelector = false;

		// Load dll
		hModule = LoadLibrary(m_pPath->GetPathUpdaterTemp() + "\\updateselector.dll");

		// Check if dll is loaded successfully
		if (hModule)
		{
			// Get proc address
			SelectUpdate = (UPDATESELECTORPROC) GetProcAddress(hModule, "SelectUpdate");

			// Execute function in dll
			if (SelectUpdate != NULL)
			{
				sUpdateLocation = SelectUpdate();
			}

			// Release dll
			FreeLibrary(hModule);
		}

		// Delete dll again
		DeleteFile(m_pPath->GetPathUpdaterTemp() + "\\updateselector.dll");

		// Start downloading of update list
		if (!m_pInternet->DownloadFile(sUpdateLocation,
			m_pPath->GetPathUpdaterTemp() + "updatefile.xml"))
		{
			// Post error
			PostMessageToParent(WMU_ERROR, ERROR_SERVER, 0);
		}
		return;
	}

	// Are we ready downloading yet?
	if (!m_bDownloadsReady)
	{
		// Check if this is the right update file (sequenced updating)
		sUpdateLocation = m_pUpdateInfo->GetPreviousUpdate(m_pPath->GetPathUpdaterTemp() + "updatefile.xml");
		if (!sUpdateLocation.IsEmpty())
		{
			// Download next one
			if (!m_pInternet->DownloadFile(sUpdateLocation,
				m_pPath->GetPathUpdaterTemp() + "updatefile.xml"))
			{
				// Post error
				PostMessageToParent(WMU_ERROR, ERROR_SERVER, 0);
			}
			return;
		}
		else
		{
			// Read update list
			if (!m_pUpdateInfo->ReadFile(m_pPath->GetPathUpdaterTemp() + "updatefile.xml"))
			{
				// Send error message
				PostMessageToParent(WMU_ERROR, ERROR_FILECORRUPT, (LPARAM)_T("updatefile.xml"));
				PostThreadMessage(WM_QUIT, 0, 0);
				return;
			}

			// We should delete the file for security reasons
			DeleteFile(m_pPath->GetPathUpdaterTemp() + "updatefile.xml");

			// Should we download other file?
			if (m_pUpdateInfo->GetSelfUpdate())
			{
				// Should we download custom or default update script?
				if (m_pSettings->GetSelfUpdateLocation().IsEmpty())
				{
					// Use default
					if (!m_pInternet->DownloadFile(UPDATER_UPDATELOCATION,
						m_pPath->GetPathUpdaterTemp() + "selfupdate.xml"))
					{
						// Just fake we are ready
						PostThreadMessage(WMU_DOWNLOAD_COMPLETE, 0, 0);
					}
				}
				else
				{
					// Use custom
					if (!m_pInternet->DownloadFile(m_pSettings->GetSelfUpdateLocation(),
						m_pPath->GetPathUpdaterTemp() + "selfupdate.xml"))
					{
						// Just fake we are ready
						PostThreadMessage(WMU_DOWNLOAD_COMPLETE, 0, 0);
					}
				}

				// Now we are ready downloading & updater cannot fail
				m_bDownloadsReady = true;
				m_bNoFailPossible = true;

				return;
			}
		}
	}

	// Read self update info
	if (PathFileExists(m_pPath->GetPathUpdaterTemp() + "selfupdate.xml"))
	{
		m_pUpdateInfo->ReadSelfUpdateFile(m_pPath->GetPathUpdaterTemp() + "selfupdate.xml");
	}

	// Should we check all files separately?
	if (m_pUpdateInfo->GetCheckSeparately())
	{
		// Check for all files, returns true if at least one file should be updated
		if (!CheckVersionSeparately())
		{
			// Send error message
			PostMessageToParent(WMU_ERROR, ERROR_NONEWVERSION, 0);
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
	}
	else
	{
		// First check for sections
		if (!CheckSections())
		{
			// Check if there is a new version available for the product
			if (m_pFunctions->CompareVersions(m_pUpdateInfo->GetNewVersion(), m_pSettings->GetAppVersion()) != COMPAREVERSION_FIRSTLARGER)
			{
				// Send error message
				PostMessageToParent(WMU_ERROR, ERROR_NONEWVERSION, 0);
				PostThreadMessage(WM_QUIT, 0, 0);
				return;
			}
		}
	}
	
	// Send message to parent that we are ready
	PostMessageToParent(WMU_TASK_COMPLETE, TASK_CHECKVERSION, 0);
	
	// Exit the thread
	PostThreadMessage(WM_QUIT, 0, 0);
}

//=====================================================================

void CCheckVersion::OnError(WPARAM wParam, LPARAM lParam)
{
	// The download failed!
	if (m_bNoFailPossible)
	{
		// Updater can't fail, so just ignore it
		PostThreadMessage(WMU_DOWNLOAD_COMPLETE, 0, 0);
	}
	else
	{
		// Report error
		PostMessageToParent(WMU_ERROR, ERROR_SERVER, (LPARAM)"updatefile.xml");

		// Exit thread
		PostThreadMessage(WM_QUIT, 0, 0);
	}
}

//=====================================================================

void CCheckVersion::OnSetParent(WPARAM wParam, LPARAM lParam)
{
	// Set parent
	m_pParent = (CWinThread *)wParam;
}

//=====================================================================

void CCheckVersion::OnThreadMessage(WPARAM wParam, LPARAM lParam)
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
			// Start main functions
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
