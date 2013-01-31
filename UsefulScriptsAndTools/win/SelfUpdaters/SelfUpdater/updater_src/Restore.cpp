/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:55
	filename: 	Restore.cpp
	file path:	Updater
	file base:	Restore
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Restore.h"

#include "Registry.h"					// Registry support
#include "CopyFolder.h"					// Copy folder support

typedef HRESULT (* DLLUNREGISTERSERVERPROC)(); 

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CRestore, CWinThread)
	ON_THREAD_MESSAGE(WMU_SETPARENT, OnSetParent)
	ON_THREAD_MESSAGE(WMU_THREADMESSAGE, OnThreadMessage)
	ON_THREAD_MESSAGE(WMU_COPY_STATUS, OnCopyStatus)
	ON_THREAD_MESSAGE(WMU_COPY_COMPLETE, OnCopyComplete)
	ON_THREAD_MESSAGE(WMU_RESTORE_COMPLETE, OnRestoreComplete)
	ON_THREAD_MESSAGE(WMU_ERROR, OnError)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CRestore, CWinThread)

//=====================================================================

CRestore::CRestore()
{
}

//=====================================================================

CRestore::~CRestore()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CRestore::InitInstance()
{
	// Set parent to NULL
	m_pParent = NULL;

	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pRestoreInfo = CRestoreInfo::Instance();

	// We are not started yet
	m_bStarted = false;
	m_iCurrentActionPoints = 0;
	m_iTotalActionPoints = 0;
	for (int i = 0; i < m_pRestoreInfo->GetActionCount(); i++)
	{
		m_iTotalActionPoints += CActionPoints::CalculateActionPoints(m_pRestoreInfo->GetAction(i));
	}
	
	return TRUE;
}

//=====================================================================

int CRestore::ExitInstance()
{
	// If thread is running
	if (!IsBadReadPtr(m_pCopyFile, sizeof(CCopyFile *)))
	{
		// Stop it
		m_pCopyFile->PostThreadMessage(WM_QUIT, 0, 0);

		// Wait until thread has really ended
		WaitForSingleObject(m_pCopyFile->m_hThread, INFINITE);
	}

	// If thread is running
	if (!IsBadReadPtr(m_pCopyFolder, sizeof(CCopyFolder *)))
	{
		// Stop it
		m_pCopyFolder->PostThreadMessage(WM_QUIT, 0, 0);

		// Wait until thread has really ended
		WaitForSingleObject(m_pCopyFolder->m_hThread, INFINITE);
	}	

	// Auto clean-up thread
	m_bAutoDelete = TRUE;

	// Call original function
	return CWinThread::ExitInstance();
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CRestore::PerformAction(int iIndex)
{
	// Send rollback status
	SendRestoreStatus();

	// Check what action to perform
	switch (m_pRestoreInfo->GetAction(iIndex)->GetType())
	{
	case RESTOREACTION_COPY:
		PerformActionCopy(m_pRestoreInfo->GetAction(iIndex));
		break;

	case RESTOREACTION_DELETE:
		PerformActionDelete(m_pRestoreInfo->GetAction(iIndex));
		break;

	case RESTOREACTION_REGISTRY:
		PerformActionRegistry(m_pRestoreInfo->GetAction(iIndex));
		break;

	case RESTOREACTION_UNREGISTER:
		PerformActionUnregister(m_pRestoreInfo->GetAction(iIndex));
		break;
	}
}

//=====================================================================

void CRestore::PerformNextAction()
{
	// Increase current action
	m_iCurrentAction++;

	// Are there any actions left?
	if (m_iCurrentAction < m_pRestoreInfo->GetActionCount())
	{
		// Yes, start next action
		PerformAction(m_iCurrentAction);
	}
	else
	{
		// Always set everything to 100 % at the end
		PostMessageToParent(WMU_UPDATE_GUI_TOTALPROGRESS, 100, 0);

		// Send message to parent that we are ready
		PostMessageToParent(WMU_TASK_COMPLETE, TASK_RESTORE, 0);

		// Exit the thread
		PostThreadMessage(WM_QUIT, 0, 0);
	}
}

//=====================================================================	

void CRestore::PerformActionCopy(CRestoreActionBase * pAction)
{
	// Declare variables
	CRestoreActionCopy * pCopy = (CRestoreActionCopy *)pAction;

	// We didn't copy anything yet
	m_iTempActionPoints = 0;

	// Check if we should copy file or folder
	if (!IsFolder(pCopy->GetOldLocation()))
	{
		// Copy the file
		m_pCopyFile = (CCopyFile *)AfxBeginThread(RUNTIME_CLASS(CCopyFile),
			THREAD_PRIORITY_NORMAL,
			0,
			0,
			NULL);

		// Set parent
		m_pCopyFile->SetParent(this);

		// Start copy
		m_pCopyFile->StartFileCopy(pCopy->GetOldLocation(), pCopy->GetNewLocation(),
								   false);
	}
	else
	{
		// Copy the folder
		m_pCopyFolder = (CCopyFolder *)AfxBeginThread(RUNTIME_CLASS(CCopyFolder),
			THREAD_PRIORITY_NORMAL,
			0,
			0,
			NULL);

		// Set parent
		m_pCopyFolder->SetParent(this);

		// Start copy
		m_pCopyFolder->StartFolderCopy(pCopy->GetOldLocation(), pCopy->GetNewLocation(),
								       false);
	}
}

//=====================================================================

void CRestore::PerformActionDelete(CRestoreActionBase * pAction)
{
	// Declare variables
	CRestoreActionDelete * pDelete = (CRestoreActionDelete *)pAction;

	// Check if we should copy a complete folder or just one file
	if (!IsFolder(pDelete->GetLocation()))
	{
		// Delete the file without asking & error checking
		DeleteFile(pDelete->GetLocation());
	}
	else
	{
		// Delete the folder without asking & error checking
		m_pFunctions->DeleteAllFiles(pDelete->GetLocation(), true);
	}

	// Update data
	m_iCurrentActionPoints++;
	SendRestoreStatus();

	// Action is ready
	PostThreadMessage(WMU_RESTORE_COMPLETE, 0, 0);
}

//=====================================================================

void CRestore::PerformActionRegistry(CRestoreActionBase * pAction)
{
	// Declare variables
	CRegistry registry;
	CRestoreActionRegistry * pRegistry = (CRestoreActionRegistry *)pAction;
	CString sBase, sKey, sItem;
	HKEY hBase; 

	// Get information
	sBase = m_pFunctions->ExtractRegistryBase(pRegistry->GetKey());
	sKey = m_pFunctions->ExtractRegistryKey(pRegistry->GetKey());
	sItem = m_pFunctions->ExtractRegistryItem(pRegistry->GetKey());
	
	// Check what base to use
	sBase = StringUpper(sBase);

	if (sBase == "HKEY_LOCAL_MACHINE")
		hBase = HKEY_LOCAL_MACHINE;
	
	if (sBase == "HKEY_CURRENT_USER")
		hBase = HKEY_CURRENT_USER;
	
	if (sBase == "HKEY_CURRENT_CONFIG")
		hBase = HKEY_CURRENT_CONFIG;
	
	if (sBase == "HKEY_CLASSES_ROOT")
		hBase = HKEY_CLASSES_ROOT;
	
	if (sBase == "HKEY_USERS")
		hBase = HKEY_USERS;
	
	// Set new data
	if (!registry.VerifyKey(hBase, sKey))
	{
		// Create key
		registry.CreateKey(hBase, sKey);
	}
	registry.Open(hBase, sKey);
	
	// Write data to registry key
	registry.Write(sItem, pRegistry->GetValue());
	
	// Close registry
	registry.Close();

	// Update data
	m_iCurrentActionPoints++;
	SendRestoreStatus();

	// Action is ready
	PostThreadMessage(WMU_RESTORE_COMPLETE, 0, 0);
}

//=====================================================================

void CRestore::PerformActionUnregister(CRestoreActionBase * pAction)
{
	// Declare variables
	CRestoreActionUnregister * pUnregister = (CRestoreActionUnregister *)pAction;
	CString sFile;
	HMODULE hMod;
	DLLUNREGISTERSERVERPROC DllUnRegisterServerFunction;

	// Get information
	sFile = pUnregister->GetFile();

	// Unregister file
	hMod = LoadLibrary(sFile);

	// Check if we can load library
	if (hMod != NULL)
	{
		// Get proc address
		DllUnRegisterServerFunction = (DLLUNREGISTERSERVERPROC) GetProcAddress(hMod, "DllUnregisterServer");

		// Check if we can run the function
		if (DllUnRegisterServerFunction != NULL)
		{
			// Unregister
			DllUnRegisterServerFunction();
		}
	}

	// Update data
	m_iCurrentActionPoints++;
	SendRestoreStatus();

	// Action is ready
	PostThreadMessage(WMU_RESTORE_COMPLETE, 0, 0); 
}

//=====================================================================

int CRestore::CalculateActionCount()
{
	// Declare variables
	int iActionCount = 0;
	int iSize;

	// Loop all actions
	for (int i = 0; i < m_pRestoreInfo->GetActionCount(); i++)
	{
		// Check what type of action
		if (m_pRestoreInfo->GetAction(i)->GetType() == RESTOREACTION_COPY)
		{
			// Get object
			CRestoreActionCopy * pCopy = (CRestoreActionCopy *) m_pRestoreInfo->GetAction(i);

			// Check if it is a folder of file
			if (!IsFolder(pCopy->GetOldLocation()))
			{
				iSize = m_pFunctions->GetFileSize(pCopy->GetOldLocation());
			}
			else
			{
				iSize = m_pFunctions->GetFolderSize(pCopy->GetOldLocation());
			}

			// Calculate actions
			div_t divInfo = div(iSize, BUFFER_COPYFILE);
			iActionCount += divInfo.quot;
		}

		if (m_pRestoreInfo->GetAction(i)->GetType() == RESTOREACTION_DELETE)
		{
			// Just one step
			iActionCount++;
		}

		if (m_pRestoreInfo->GetAction(i)->GetType() == RESTOREACTION_REGISTRY)
		{
			// Just one step
			iActionCount++;
		}

		if (m_pRestoreInfo->GetAction(i)->GetType() == RESTOREACTION_UNREGISTER)
		{
			// Just one step
			iActionCount++;
		}
	}

	// Return action count
	return iActionCount;
}

//=====================================================================

void CRestore::SendRestoreStatus()
{
	// Declare variables
	int iTotalProgress;

	// Calculate progress
	if (m_iTotalActionPoints > 0)
	{
		iTotalProgress = (m_iCurrentActionPoints * 100) / m_iTotalActionPoints;
	}
	else
	{
		iTotalProgress = 100;
	}

	// Send to parent
	if (m_iTotalProgress != iTotalProgress)
	{
		PostMessageToParent(WMU_UPDATE_GUI_TOTALPROGRESS, iTotalProgress, 0);
		m_iTotalProgress = iTotalProgress;
	}
}

//=====================================================================

void CRestore::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWnd *)))
	{
		// Send message
		m_pParent->PostMessage(message, wParam, lParam);
	}
}

//=====================================================================

bool CRestore::IsFolder(CString sFileOrFolder)
{
	// Declare variables
	WIN32_FIND_DATA FileInfo = {0};
	HANDLE hFile = 0;

	// Check if object is file or folder
	if ((hFile = FindFirstFile(sFileOrFolder, &FileInfo)) != INVALID_HANDLE_VALUE)
	{
        ::FindClose(hFile);
		if ((FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			// It is a Directory
			return true;
		}
		else
		{
			// It is a file
			return false;
		}
	}
	else
	{
		// It is a folder
		return true;
	}
}

//=====================================================================
	
void CRestore::OnSetParent(WPARAM wParam, LPARAM lParam)
{
	// Set parent
	m_pParent = (CWnd *)wParam;
}

//=====================================================================

void CRestore::OnThreadMessage(WPARAM wParam, LPARAM lParam)
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
			// Check if app is running
			if (m_pFunctions->ApplicationRunning(m_pSettings->GetAppLocation()))
			{
				// Close application
				if (!m_pFunctions->CloseApplication(m_pSettings->GetAppName(), m_pSettings->GetAppLocation()))
				{
					// Return error
					PostThreadMessage(WM_QUIT, 0, 0);
					return;
				}
			}

			// Start first action
			m_iCurrentAction = -1;
			PerformNextAction();
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

void CRestore::OnCopyStatus(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iCurrentActionPoints = 0;

	// Calculate difference between previous and now
	if (m_iTempActionPoints < static_cast<int>(wParam))
	{
		// It's still the same file
		iCurrentActionPoints = wParam - m_iTempActionPoints;
	}
	
	m_iCurrentActionPoints += iCurrentActionPoints;

	// Set new value of files copied
	m_iTempActionPoints = wParam;

	// Send rollback status
	SendRestoreStatus();
}

//=====================================================================

void CRestore::OnCopyComplete(WPARAM wParam, LPARAM lParam)
{
	// Set objects to NULL
	m_pCopyFile = NULL;
	m_pCopyFolder = NULL;

	// Ok, rollback action is finished, start next one
	PerformNextAction();
}

//=====================================================================

void CRestore::OnRestoreComplete(WPARAM wParam, LPARAM lParam)
{
	// Start next item
	PerformNextAction();
}

//=====================================================================

void CRestore::OnError(WPARAM wParam, LPARAM lParam)
{
	// Do do anything with the error, start next action
	PerformNextAction();
}