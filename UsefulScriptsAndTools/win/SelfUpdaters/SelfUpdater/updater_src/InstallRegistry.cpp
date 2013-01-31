/********************************************************************
	created:	2005/03/25
	created:	25:3:2005   12:51
	filename: 	InstallRegistry.cpp
	file path:	Updater
	file base:	InstallRegistry
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "InstallRegistry.h"

#include "Registry.h"						// Registry support
#include "BackUp.h"							// Backup support

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CInstallRegistry, CWinThread)
	ON_THREAD_MESSAGE(WMU_INSTALL_START, OnInstallStart)
	ON_THREAD_MESSAGE(WMU_INSTALLREGISTRY_COMPLETE, OnRegistryItemComplete)
	ON_THREAD_MESSAGE(WMU_SETPARENT, OnSetParent)
	ON_THREAD_MESSAGE(WMU_BACKUP_COMPLETE, OnBackUpComplete)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CInstallRegistry, CWinThread)

//=====================================================================

CInstallRegistry::CInstallRegistry()
{
}

//=====================================================================

CInstallRegistry::~CInstallRegistry()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CInstallRegistry::InitInstance()
{
	// Log
	CLog::Instance()->Log(_T("CInstallRegistry::InitInstance()"));

	// Init singleton classes
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pFunctions = CFunctions::Instance();
	m_pLanguage = CLanguage::Instance();
	m_pPath = CPath::Instance();
	
	// Set default values
	m_InstallStatus.iTotalActions = 0;
	m_InstallStatus.iTotalActionsPerformed = 0;
	m_InstallStatus.iFileActions = 0;
	m_InstallStatus.iFileActionsPerformed = 0;

	// Calculate actions
	m_InstallStatus.iTotalActions = CalculateInstallActions();

	// Set install type
	m_InstallStatus.iInstallPart = INSTALL_REGISTRY;
	
	return TRUE;
}

//=====================================================================

int CInstallRegistry::ExitInstance()
{
	// Log
	CLog::Instance()->Log(_T("CInstallRegistry::ExitInstance()"));

	// Is there a backup thread runnig?
	if (!IsBadReadPtr(m_pBackupThread, sizeof(CBackUp *)))
	{
		// End thread
		m_pBackupThread->PostThreadMessage(WM_QUIT, 0, 0);

		// Wait until thread has really ended
		WaitForSingleObject(m_pBackupThread->m_hThread, INFINITE);
	}

	return CWinThread::ExitInstance();
}

//=====================================================================

int CInstallRegistry::GetActionCount()
{
	// Return value
	return m_InstallStatus.iTotalActions;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CInstallRegistry::InstallRegistryItem(int iRegistryItem)
{
	USES_CONVERSION;

	// Set current file
	m_iCurrentRegistryItem = iRegistryItem;
	
	// Declare variables
	CRegistryData * pRegistryData;
	CRegistry registry;
	CString sBase, sKey, sItem;
	HKEY hBase;
	DWORD dwValue;
	CByteArray arrBytes;
	
	// Get registry data object
	pRegistryData = m_pUpdateInfo->GetRegistryData(m_iCurrentRegistryItem);
	
	// Get name
	m_InstallStatus.sTitle = m_pLanguage->GetString(IDS_ACTION_REGISTRY);
	
	// Get number of actions for this item
	m_InstallStatus.iFileActions = ACTIONPOINTS_REGISTRY;
	m_InstallStatus.iFileActionsPerformed = 0;
	
	// Send status to parent
	SendInstallStatus();

	// Get information
	sBase = m_pFunctions->ExtractRegistryBase(pRegistryData->GetKey());
	sKey = m_pFunctions->ExtractRegistryKey(pRegistryData->GetKey());
	sItem = m_pFunctions->ExtractRegistryItem(pRegistryData->GetKey());

	sBase = StringUpper(sBase);
	
	// Open registry key
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
	
	// Set new data
	if (!registry.VerifyKey(hBase, sKey))
	{
		// Create key
		registry.CreateKey(hBase, sKey);
	}
	registry.Open(hBase, sKey);
	
	// Write data to registry key
	if (pRegistryData->GetType() == REGTYPE_STRING)
	{
		// Write as string
		registry.Write(sItem, pRegistryData->GetValue());
	}

	if (pRegistryData->GetType() == REGTYPE_DWORD)
	{
		// Write as dword
		dwValue = atoi(W2A(pRegistryData->GetValue()));
		registry.Write(sItem, dwValue);
	}

	if (pRegistryData->GetType() == REGTYPE_BINARY)
	{
		// Write as binary
		arrBytes.Add((UCHAR)pRegistryData->GetValue().GetBuffer(pRegistryData->GetValue().GetLength()));
		registry.Write(sItem, arrBytes);
	}
	
	// Close registry
	registry.Close();
	
	// We installed an action
	m_InstallStatus.iFileActionsPerformed = ACTIONPOINTS_REGISTRY;
	m_InstallStatus.iTotalActionsPerformed += ACTIONPOINTS_REGISTRY;
	
	// Send status to parent
	SendInstallStatus();
	
	// We are ready
	PostThreadMessage(WMU_INSTALLREGISTRY_COMPLETE, 0, 0);
}

//=====================================================================

void CInstallRegistry::InstallNextRegistryItem()
{
	// Update current file
	m_iCurrentRegistryItem++;
	
	// Are there still Registry items left in this part?
	if (m_iCurrentRegistryItem < m_pUpdateInfo->GetRegistryCount())
	{
		// Should we backup?
		if (m_pUpdateInfo->GetRollbackEnabled())
		{
			// Create backup thread
			m_pBackupThread = (CBackUp *)AfxBeginThread(RUNTIME_CLASS(CBackUp),
				THREAD_PRIORITY_NORMAL,
				0,
				0,
				NULL);

			// Set parent
			m_pBackupThread->SetParent(this);

			// Backup
			m_pBackupThread->StartBackUpRegistry(m_pUpdateInfo->GetRegistryData(m_iCurrentRegistryItem)->GetKey());
		}
		else
		{
			// Install next ini item
			InstallRegistryItem(m_iCurrentRegistryItem);
		}
	}
	else
	{
		// Send message to parent that we are ready
		PostMessageToParent(WMU_INSTALL_COMPLETE, INSTALL_REGISTRY, 0);
	}
}

//=====================================================================

int	CInstallRegistry::CalculateInstallActions()
{
	// Declare variables
	int iActionCount = 0;

	// Calculate install actions to perform
	iActionCount = m_pUpdateInfo->GetRegistryCount();

	// Return action count
	return iActionCount;
}

//=====================================================================

int CInstallRegistry::CalculateBackUpActions()
{
	// Declare variables
	int iActionCount = 0;

	// Calculate backup actions to perform
	iActionCount = m_pUpdateInfo->GetRegistryCount();

	// Return action count
	return iActionCount;
}

//=====================================================================

void CInstallRegistry::SendInstallStatus()
{
	// Send status to parent
	if (m_pParent)	
		PostMessageToParent(WMU_INSTALL_STATUS, (WPARAM)&m_InstallStatus, 0);		
}

//=====================================================================

void CInstallRegistry::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

void CInstallRegistry::OnInstallStart(WPARAM wParam, LPARAM lParam)
{
	// Install first Registry item
	m_iCurrentRegistryItem = -1;
	InstallNextRegistryItem();
}

//=====================================================================

void CInstallRegistry::OnRegistryItemComplete(WPARAM wParam, LPARAM lParam)
{
	// Start next one
	InstallNextRegistryItem();
}

//=====================================================================

void CInstallRegistry::OnSetParent(WPARAM wParam, LPARAM lParam)
{
	// Set parent
	m_pParent = (CWinThread *)wParam;
}

//=====================================================================

void CInstallRegistry::OnBackUpComplete(WPARAM wParam, LPARAM lParam)
{
	// Are there still Registry items left in this part?
	if (m_iCurrentRegistryItem < m_pUpdateInfo->GetRegistryCount())
	{
		// Install next ini item
		InstallRegistryItem(m_iCurrentRegistryItem);
	}
	else
	{
		// Send message to parent that we are ready
		PostMessageToParent(WMU_INSTALL_COMPLETE, INSTALL_REGISTRY, 0);
	}
}