/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:55
	filename: 	SelectUpdate.cpp
	file path:	Updater
	file base:	SelectUpdate
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "SelectUpdate.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CSelectUpdate, CWinThread)
	ON_THREAD_MESSAGE(WMU_SETPARENT, OnSetParent)
	ON_THREAD_MESSAGE(WMU_THREADMESSAGE, OnThreadMessage)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CSelectUpdate, CWinThread)

//=====================================================================

CSelectUpdate::CSelectUpdate()
{
}

//=====================================================================

CSelectUpdate::~CSelectUpdate()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CSelectUpdate::InitInstance()
{
	// Log
	CLog::Instance()->Log("CSelectUpdate::InitInstance");

	// Set parent to NULL
	m_pParent = NULL;

	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();

	// We are not started yet
	m_bStarted = false;
	
	return TRUE;
}

//=====================================================================

int CSelectUpdate::ExitInstance()
{
	// Log
	CLog::Instance()->Log("CSelectUpdate::ExitInstance");

	// Auto clean-up thread
	m_bAutoDelete = TRUE;

	// Call original function
	return CWinThread::ExitInstance();
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CSelectUpdate::Main()
{
	// Declare variables
	CString arrSectionsToUpdate[MAX_SECTIONS];
	int i, j, iSectionsToUpdate = 0;

	// We are started
	m_bStarted = true;

	// Check which sections should be updated
	for (i = 0; i < m_pUpdateInfo->GetSectionCount(); i++)
	{
		if (m_pUpdateInfo->GetSectionData(i)->GetSelectedByUser())
		{
			// This section should be updated, add name to array
			arrSectionsToUpdate[iSectionsToUpdate++] = m_pUpdateInfo->GetSectionData(i)->GetName();
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
			// Should be update this file?
			if (m_pFunctions->CompareVersions(m_pUpdateInfo->GetNewVersion(), m_pSettings->GetAppVersion()) == COMPAREVERSION_FIRSTLARGER)
			{
				// We should update this file
				m_pUpdateInfo->GetFileData(i)->SetUpdateFile(true);
			}
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

	// We are ready, no need to notify the parent, because this is done after the
	// real thread. Just stop yourself and the parent will go on
	PostThreadMessage(WM_QUIT, 0, 0);
}

//=====================================================================

void CSelectUpdate::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

void CSelectUpdate::OnSetParent(WPARAM wParam, LPARAM lParam)
{
	// Set parent
	m_pParent = (CWinThread *)wParam;
}

//=====================================================================

void CSelectUpdate::OnThreadMessage(WPARAM wParam, LPARAM lParam)
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