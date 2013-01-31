/********************************************************************
	created:	2005/03/25
	created:	25:3:2005   12:51
	filename: 	InstallIni.cpp
	file path:	Updater
	file base:	InstallIni
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "InstallIni.h"

#include "BackUp.h"							// Backup support

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CInstallIni, CWinThread)
	ON_THREAD_MESSAGE(WMU_INSTALL_START, OnInstallStart)
	ON_THREAD_MESSAGE(WMU_INSTALLINI_COMPLETE, OnIniItemComplete)
	ON_THREAD_MESSAGE(WMU_SETPARENT, OnSetParent)
	ON_THREAD_MESSAGE(WMU_BACKUP_COMPLETE, OnBackUpComplete)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CInstallIni, CWinThread)

//=====================================================================

CInstallIni::CInstallIni()
{
}

//=====================================================================

CInstallIni::~CInstallIni()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CInstallIni::InitInstance()
{
	// Log
	CLog::Instance()->Log(_T("CInstallIni::InitInstance()"));

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
	m_InstallStatus.iInstallPart = INSTALL_INI;

	return TRUE;
}

//=====================================================================

int CInstallIni::ExitInstance()
{
	// Log
	CLog::Instance()->Log(_T("CInstallIni::ExitInstance()"));

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

int CInstallIni::GetActionCount()
{
	// Return value
	return m_InstallStatus.iTotalActions;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CInstallIni::InstallIniItem(int iIniItem)
{
	// Set current file
	m_iCurrentIniItem = iIniItem;
	
	// Declared
	CIniData * pIniData;
	CString sTemp;
	
	// Get ini data object
	pIniData = m_pUpdateInfo->GetIniData(m_iCurrentIniItem);
	
	// Get name
	m_InstallStatus.sTitle = m_pLanguage->GetString(IDS_ACTION_INI);
	
	// Get number of actions for this item
	m_InstallStatus.iFileActions = ACTIONPOINTS_INI;
	m_InstallStatus.iFileActionsPerformed = 0;
	SendInstallStatus();

	// If the destination directory does not exist, create it
	sTemp = m_pPath->ExtractFilePath(pIniData->GetIniLocation());
	if (!m_pFunctions->CreateFolder(sTemp))
	{
		// Send error message
		PostMessageToParent(WMU_ERROR, ERROR_WRITEFILE, (LPARAM)&m_InstallStatus);
	}
	
	// Set settings into ini-file
	if (!WritePrivateProfileString(pIniData->GetGroup(), pIniData->GetKey(),
		pIniData->GetValue(), pIniData->GetIniLocation()))
	{
		// Get name
		m_InstallStatus.sTitle = m_pPath->ExtractFileName(pIniData->GetIniLocation());
		// Send error message
		PostMessageToParent(WMU_ERROR, ERROR_WRITEFILE, (LPARAM)&m_InstallStatus);
	}
	
	// We installed an action
	m_InstallStatus.iFileActionsPerformed = ACTIONPOINTS_INI;
	m_InstallStatus.iTotalActionsPerformed += ACTIONPOINTS_INI;
	
	// Send status to parent
	PostMessageToParent(WMU_INSTALL_STATUS, (WPARAM)&m_InstallStatus, 0);
	
	// Install next ini item
	InstallNextIniItem();
}

//=====================================================================

void CInstallIni::InstallNextIniItem()
{
	// Update current file
	m_iCurrentIniItem++;

	// Are there still ini items left in this part?
	if (m_iCurrentIniItem < m_pUpdateInfo->GetIniCount())
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
			m_pBackupThread->StartBackUpFile(m_pUpdateInfo->GetIniData(m_iCurrentIniItem)->GetIniLocation());
		}
		else
		{
			// Install next ini item
			InstallIniItem(m_iCurrentIniItem);
		}
	}
	else
	{
		// Send message to parent that we are ready
		PostMessageToParent(WMU_INSTALL_COMPLETE, INSTALL_INI, 0);
	}
}

//=====================================================================

int	CInstallIni::CalculateInstallActions()
{
	// Declare variables
	int iActionCount = 0;

	// Calculate install actions to perform
	iActionCount = m_pUpdateInfo->GetIniCount();

	// Return action count
	return iActionCount;
}

//=====================================================================

void CInstallIni::SendInstallStatus()
{
	// Send status to parent
	if (m_pParent)
		PostMessageToParent(WMU_INSTALL_STATUS, (WPARAM)&m_InstallStatus, 0);	
}

//=====================================================================

void CInstallIni::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}	
}

//=====================================================================

void CInstallIni::OnInstallStart(WPARAM wParam, LPARAM lParam)
{
	// Install first ini item
	m_iCurrentIniItem = -1;
	InstallNextIniItem();
}

//=====================================================================

void CInstallIni::OnIniItemComplete(WPARAM wParam, LPARAM lParam)
{
	// Start next one
	InstallNextIniItem();
}

//=====================================================================

void CInstallIni::OnSetParent(WPARAM wParam, LPARAM lParam)
{
	// Set parent
	m_pParent = (CWinThread *)wParam;
}

//=====================================================================

void CInstallIni::OnBackUpComplete(WPARAM wParam, LPARAM lParam)
{
	// Are there still ini items left in this part?
	if (m_iCurrentIniItem < m_pUpdateInfo->GetIniCount())
	{
		// Install next ini item
		InstallIniItem(m_iCurrentIniItem);
	}
	else
	{
		// Send message to parent that we are ready
		PostMessageToParent(WMU_INSTALL_COMPLETE, INSTALL_INI, 0);
	}
}