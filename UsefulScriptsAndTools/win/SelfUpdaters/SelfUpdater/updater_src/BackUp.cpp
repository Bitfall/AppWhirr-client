/********************************************************************
	created:	2005/04/01
	created:	1:4:2005   13:54
	filename: 	BackUp.cpp
	file path:	Updater
	file base:	BackUp
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "BackUp.h"
#include "Registry.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CBackUp, CWinThread)
	ON_THREAD_MESSAGE(WMU_COPY_STATUS, OnCopyStatus)
	ON_THREAD_MESSAGE(WMU_COPY_COMPLETE, OnCopyComplete)
	ON_THREAD_MESSAGE(WMU_ERROR, OnError)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CBackUp, CWinThread)

//=====================================================================

CBackUp::CBackUp()
{
	// Init singleton classes
	m_pLog = CLog::Instance();
}

//=====================================================================

CBackUp::~CBackUp()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CBackUp::InitInstance()
{
	// Log
	CLog::Instance()->Log(_T("CBackUp::InitInstance"));
	return TRUE;
}

//=====================================================================

int CBackUp::ExitInstance()
{
	// Log
	CLog::Instance()->Log(_T("CBackUp::ExitInstance"));

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

	// Auto delete
	m_bAutoDelete = TRUE;

	return CWinThread::ExitInstance();
}

//=====================================================================

void CBackUp::SetParent(CWinThread * pParentThread)
{
	// Set parent
	m_pParent = pParentThread;
}

//=====================================================================

void CBackUp::StartBackUpFile(CString sFilename)
{
	// Declare variables
	CString sBackupLocation, sDrive;
	CFunctions * pFunctions = CFunctions::Instance();
	CPath * pPath = CPath::Instance();

	// Log
	m_pLog->Log(_T("BACKUP: start backup of file ") + sFilename);

	// Check if file exists
	if (PathFileExists(sFilename))
	{
		// Get drive
		sDrive = pPath->ExtractDrive(sFilename);

		// Remove it from new location
		sBackupLocation = sFilename;
		sBackupLocation.Delete(0, sDrive.GetLength() + 1);

		// Insert rollback folder in front
		sBackupLocation.Insert(0, pPath->GetPathRollback());

		// Store values
		m_sCurrentLocation = sFilename;
		m_sBackupLocation = sBackupLocation;

		// Create folder
		pFunctions->CreateFolder(pPath->ExtractFilePath(sBackupLocation));

		// Copy the file
		m_pCopyFile = (CCopyFile *)AfxBeginThread(RUNTIME_CLASS(CCopyFile),
			THREAD_PRIORITY_NORMAL,
			0,
			0,
			NULL);

		// Set parent
		m_pCopyFile->SetParent(this);

		// Start copy
		m_pCopyFile->StartFileCopy(sFilename, sBackupLocation, false);

		// Add rollback action
		AddRollbackCopy(sBackupLocation, sFilename);
	}
	else
	{
		// Delete when rolling back
		AddRollbackDelete(sFilename);

		// We are ready!
		PostMessageToParent(WMU_BACKUP_COMPLETE, 0, 0);
	}
}

//=====================================================================

void CBackUp::StartBackUpFolder(CString sFolder)
{
	// Declare variables
	CString sBackupLocation, sDrive;
	CPath * pPath = CPath::Instance();

	// Log
	m_pLog->Log(_T("BACKUP: start backup of folder ") + sFolder);

	// Check if file exists
	if (PathFileExists(sFolder))
	{
		// Get drive
		sDrive = pPath->ExtractDrive(sFolder);

		// Remove it from new location
		sBackupLocation = sFolder;
		sBackupLocation.Delete(0, sDrive.GetLength());

		// Insert rollback folder in front
		sBackupLocation.Insert(0, pPath->GetPathRollback());

		// Store values
		m_sCurrentLocation = sFolder;
		m_sBackupLocation = sBackupLocation;
		
		// Copy the folder
		m_pCopyFolder = (CCopyFolder *)AfxBeginThread(RUNTIME_CLASS(CCopyFolder),
			THREAD_PRIORITY_NORMAL,
			0,
			0,
			NULL);

		// Set parent
		m_pCopyFolder->SetParent(this);

		// Start copy
		m_pCopyFolder->StartFolderCopy(sFolder, sBackupLocation, false);
	}
	else
	{
		// Delete when rolling back
		AddRollbackDelete(sFolder);

		// We are ready!
		PostMessageToParent(WMU_BACKUP_COMPLETE, 0, 0);
	}
}

//=====================================================================

void CBackUp::StartBackUpRegistry(CString sRegistrykey)
{
	// Declare variables
	CRegistry registry;
	CString sBase, sKey, sItem, sValue = _T("");
	HKEY hBase;
	CFunctions * pFunctions = CFunctions::Instance();

	// Get information
	sBase = pFunctions->ExtractRegistryBase(sRegistrykey);
	sKey = pFunctions->ExtractRegistryKey(sRegistrykey);
	sItem = pFunctions->ExtractRegistryItem(sRegistrykey);
	
	// Check what base to use
	sBase = StringUpper(sBase);

	if (sBase == _T("HKEY_LOCAL_MACHINE"))
		hBase = HKEY_LOCAL_MACHINE;
	
	if (sBase == _T("HKEY_CURRENT_USER"))
		hBase = HKEY_CURRENT_USER;
	
	if (sBase == _T("HKEY_CURRENT_CONFIG"))
		hBase = HKEY_CURRENT_CONFIG;
	
	if (sBase == _T("HKEY_CLASSES_ROOT"))
		hBase = HKEY_CLASSES_ROOT;
	
	if (sBase == _T("HKEY_USERS"))
		hBase = HKEY_USERS;
	
	// Is there any data?
	if (registry.VerifyKey(hBase, sKey))
	{
		// Open key
		if (registry.Open(hBase, sKey))
		{
			// Read value
			registry.Read(sItem, sValue);

			// Close registry
			registry.Close();
		}
	}

	// Add rollback action
	AddRollbackRegistry(sRegistrykey, sValue);

	// We are ready!
	PostMessageToParent(WMU_BACKUP_COMPLETE, 0, 0);
}

//=====================================================================

void CBackUp::StartBackUpRegister(CString sFilename)
{
	// Add rollback action
	AddRollbackRegister(sFilename);

	// We are ready!
	PostMessageToParent(WMU_BACKUP_COMPLETE, 0, 0);
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CBackUp::AddRollbackDelete(CString sLocation)
{
	// Declare variables
	CRollbackInfo * pRollbackInfo = CRollbackInfo::Instance();

	// Add action to rollback script
	pRollbackInfo->AddActionDelete(sLocation);
}

//=====================================================================

void CBackUp::AddRollbackCopy(CString sOldLocation, CString sNewLocation)
{
	// Declare variables
	CRollbackInfo * pRollbackInfo = CRollbackInfo::Instance();

	// Add action to rollback script
	pRollbackInfo->AddActionCopy(sOldLocation, sNewLocation);
}

//=====================================================================

void CBackUp::AddRollbackRegistry(CString sRegistrykey, CString sValue)
{
	// Declare variables
	CRollbackInfo * pRollbackInfo = CRollbackInfo::Instance();

	// Add action to rollback script
	pRollbackInfo->AddActionRegistry(sRegistrykey, sValue);
}

//=====================================================================

void CBackUp::AddRollbackRegister(CString sFile)
{
	// Declare variables
	CRollbackInfo * pRollbackInfo = CRollbackInfo::Instance();

	// Add action to rollback script
	pRollbackInfo->AddActionUnregister(sFile);
}

//=====================================================================

void CBackUp::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

void CBackUp::OnCopyStatus(WPARAM wParam, LPARAM lParam)
{
	// Just update data
	PostMessageToParent(WMU_BACKUP_STATUS, wParam, lParam);
}

//=====================================================================

void CBackUp::OnCopyComplete(WPARAM wParam, LPARAM lParam)
{
	// Add rollback action
	AddRollbackCopy(m_sBackupLocation, m_sCurrentLocation);

	// We are ready!
	PostMessageToParent(WMU_BACKUP_COMPLETE, 0, 0);
}

//=====================================================================

void CBackUp::OnError(WPARAM wParam, LPARAM lParam)
{
	// We are ready!
	PostMessageToParent(WMU_BACKUP_COMPLETE, 0, 0);
}