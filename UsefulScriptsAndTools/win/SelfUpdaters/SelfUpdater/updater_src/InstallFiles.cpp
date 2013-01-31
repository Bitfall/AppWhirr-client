/********************************************************************
	created:	2005/03/25
	created:	25:3:2005   12:51
	filename: 	InstallFiles.cpp
	file path:	Updater
	file base:	InstallFiles
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "InstallFiles.h"

#include "Unzipper.h"						// Unzip support
#include "BackUp.h"							// Backup support

typedef HRESULT (* DLLREGISTERSERVERPROC)(); 

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CInstallFiles, CWinThread)
	ON_THREAD_MESSAGE(WMU_INSTALL_START, OnInstallStart)
	ON_THREAD_MESSAGE(WMU_INSTALLFILES_COMPLETE, OnFileComplete)
	ON_THREAD_MESSAGE(WMU_INSTALLACTION_COMPLETE, OnActionComplete)
	ON_THREAD_MESSAGE(WMU_BACKUP_STATUS, OnBackUpStatus)
	ON_THREAD_MESSAGE(WMU_BACKUP_COMPLETE, OnBackUpComplete)
	ON_THREAD_MESSAGE(WMU_ERROR, OnError)
	ON_THREAD_MESSAGE(WMU_WARNING_COMPLETE, OnWarningComplete)
	ON_THREAD_MESSAGE(WMU_QUESTION_ANSWER, OnQuestionAnswer)
	ON_THREAD_MESSAGE(WMU_COPY_STATUS, OnCopyStatus)
	ON_THREAD_MESSAGE(WMU_COPY_COMPLETE, OnCopyComplete)
	ON_THREAD_MESSAGE(WMU_SETPARENT, OnSetParent)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CInstallFiles, CWinThread)

//=====================================================================

CInstallFiles::CInstallFiles()
{
}

//=====================================================================

CInstallFiles::~CInstallFiles()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CInstallFiles::InitInstance()
{
	// Log
	CLog::Instance()->Log("CInstallFiles::InitInstance()");
	
	// Init singleton classes
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pFunctions = CFunctions::Instance();
	m_pLanguage = CLanguage::Instance();
	m_pPath = CPath::Instance();

	// Set number of actions to zero
	m_InstallStatus.iTotalActions = 0;
	m_InstallStatus.iTotalActionsPerformed = 0;

	// Calculate actions
	for (int i = 0; i < m_pUpdateInfo->GetFileCount(); i++)
	{
		m_InstallStatus.iTotalActions += CalculateActionsOfFile(i);
	}
	
	// Set install type
	m_InstallStatus.iInstallPart = INSTALL_FILES;

	return TRUE;
}

//=====================================================================

int CInstallFiles::ExitInstance()
{
	// Log
	CLog::Instance()->Log("CInstallFiles::ExitInstance()");

	// Is there a copy thread runnig?
	if (!IsBadReadPtr(m_pCopyThread, sizeof(CCopyFile *)))
	{
		// End thread
		m_pCopyThread->PostThreadMessage(WM_QUIT, 0, 0);

		// Wait until thread has really ended
		WaitForSingleObject(m_pCopyThread->m_hThread, INFINITE);
	}

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

int CInstallFiles::GetActionCount()
{
	// Return value
	return m_InstallStatus.iTotalActions;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CInstallFiles::BackUpObject()
{
	// Declare variables
	bool bBackup = false;
	CString sType;

	// Get object
	CFileData * pFileData = m_pUpdateInfo->GetFileData(m_iCurrentFile);

	// Prepare info
	m_InstallStatus.sTitle.Format(m_pLanguage->GetString(IDS_ACTION_BACKUP), pFileData->GetName());
	m_InstallStatus.iFileActions = CalculateBackupActionsOfAction(m_iCurrentFile, m_iCurrentAction);
	SendInstallStatus();

	// Create backup thread
	m_pBackupThread = (CBackUp *)AfxBeginThread(RUNTIME_CLASS(CBackUp),
		THREAD_PRIORITY_NORMAL,
		0,
		0,
		NULL);

	// Set parent
	m_pBackupThread->SetParent(this);

	// Check how to backup
	sType = pFileData->GetAction(m_iCurrentAction)->GetType();
	
	// Unzip
	if (sType == ACTION_UNZIP)
	{
		// Get object
		CActionUnzip * pUnzip = (CActionUnzip *) pFileData->GetAction(m_iCurrentAction);

		// Backup
		m_pBackupThread->StartBackUpFolder(pUnzip->GetDestination());
	}

	// Delete
	if (sType == ACTION_DELETE)
	{
		// Get object
		CActionDelete * pDelete = (CActionDelete *) pFileData->GetAction(m_iCurrentAction);

		// Backup
		m_pBackupThread->StartBackUpFile(pDelete->GetDeleteLocation());
	}

	// Copy
	if (sType == ACTION_COPY)
	{
		// Get object
		CActionCopy * pCopy = (CActionCopy *) pFileData->GetAction(m_iCurrentAction);

		// Backup
		m_pBackupThread->StartBackUpFile(pCopy->GetNewLocation());
	}

	// Register
	if (sType == ACTION_REGISTER)
	{
		// Get object
		CActionRegister * pRegister = (CActionRegister *) pFileData->GetAction(m_iCurrentAction);

		// Backup
		m_pBackupThread->StartBackUpRegister(pRegister->GetLocation());
	}
}

//=====================================================================

void CInstallFiles::InstallFile(int iFile)
{
	// Set current file
	m_iCurrentFile = iFile;
	
	// Should we update the file?
	if (m_pUpdateInfo->GetFileData(iFile)->GetUpdateFile())
	{
		// Prepare info
		m_InstallStatus.iFileActionsPerformed = 0;
		m_InstallStatus.iFileActions = CalculateActionsOfFile(m_iCurrentFile);

		// Start first action of file
		m_iCurrentAction = -1;
		PerformNextAction();
	}
	else
	{
		// Don't update, go to next file
		InstallNextFile();
	}
}

//=====================================================================

void CInstallFiles::InstallNextFile()
{
	// Update current file
	m_iCurrentFile++;
	
	// Are there still files left in this part?
	if (m_iCurrentFile < m_pUpdateInfo->GetFileCount())
	{
		// Install next file
		InstallFile(m_iCurrentFile);
	}
	else
	{
		// Send message to parent that we are ready
		PostMessageToParent(WMU_INSTALL_COMPLETE, INSTALL_FILES, 0);
	}
}

//=====================================================================

void CInstallFiles::PerformAction(int iAction)
{
	// Declare variables
	CString sActionType;
	bool bActionRun = false;

	// Set current action
	m_iCurrentAction = iAction;

	// Check what action to perform
	sActionType = m_pUpdateInfo->GetFileData(m_iCurrentFile)->GetAction(iAction)->GetType();

	// Calculate action points
	m_InstallStatus.iFileActionsPerformed = 0;

	// Copy
	if (sActionType == ACTION_COPY)
	{
		// Should we backup?
		if (m_pUpdateInfo->GetRollbackEnabled())
		{
			// First backup
			BackUpObject();
		}
		else
		{
			// Perform action immediatly
			PerformActionCopy(m_pUpdateInfo->GetFileData(m_iCurrentFile), m_iCurrentAction);
		}
		bActionRun = true;
	}

	// Delete
	if (sActionType == ACTION_DELETE)
	{
		// Should we backup?
		if (m_pUpdateInfo->GetRollbackEnabled())
		{
			// First backup
			BackUpObject();
		}
		else
		{
			// Perform action immediatly
			PerformActionDelete(m_pUpdateInfo->GetFileData(m_iCurrentFile), m_iCurrentAction);
		}
		bActionRun = true;
	}

	// Run
	if (sActionType == ACTION_RUN)
	{
		// No backup is needed
		bActionRun = true;
		PerformActionRun(m_pUpdateInfo->GetFileData(m_iCurrentFile), m_iCurrentAction);
	}

	// Unzip
	if (sActionType == ACTION_UNZIP)
	{
		// Should we backup?
		if (m_pUpdateInfo->GetRollbackEnabled())
		{
			// First backup
			BackUpObject();
		}
		else
		{
			// Perform action immediatly
			PerformActionUnzip(m_pUpdateInfo->GetFileData(m_iCurrentFile), m_iCurrentAction);
		}
		bActionRun = true;
	}

	// Set file attributes
	if (sActionType == ACTION_SETFILEATTR)
	{
		// No backup is needed
		bActionRun = true;
		PerformActionSetFileAttr(m_pUpdateInfo->GetFileData(m_iCurrentFile), m_iCurrentAction);
	}

	// Register file
	if (sActionType == ACTION_REGISTER)
	{
		// Should we backup?
		if (m_pUpdateInfo->GetRollbackEnabled())
		{
			// First backup
			BackUpObject();
		}
		else
		{
			// Perform action immediatly
			PerformActionRegister(m_pUpdateInfo->GetFileData(m_iCurrentFile), m_iCurrentAction);
		}
		bActionRun = true;
	}

	// Were we able to run any action?
	if (!bActionRun)
	{
		// It was probably a download or something, go to next action
		PostThreadMessage(WMU_INSTALLACTION_COMPLETE, 0, 0);
	}
}

//=====================================================================

void CInstallFiles::PerformNextAction()
{
	// Update current action
	m_iCurrentAction++;
	
	// Are there still actions left in this file?
	if (m_iCurrentAction < m_pUpdateInfo->GetFileData(m_iCurrentFile)->GetActionCount())
	{
		// Start next action
		PerformAction(m_iCurrentAction);
	}
	else
	{
		// Install next file
		InstallNextFile();
	}
}

//=====================================================================

void CInstallFiles::PerformActionCopy(CFileData * pFileData, int iActionIndex)
{
	// Get info
	CActionCopy * pInfo = (CActionCopy *)pFileData->GetAction(iActionIndex);	

	// Update info
	m_InstallStatus.sTitle.Format(m_pLanguage->GetString(IDS_ACTION_COPY), pFileData->GetName());
	m_InstallStatus.iFileActions = CalculateActionsOfAction(m_iCurrentFile, m_iCurrentAction);
	m_InstallStatus.iFileActionsPerformed = 0;
	SendInstallStatus();

	// Start thread
	m_pCopyThread = (CCopyFile *)AfxBeginThread(RUNTIME_CLASS(CCopyFile),
		THREAD_PRIORITY_NORMAL,
		0,
		0,
		NULL);
	
	// Set parent
	m_pCopyThread->SetParent(this);
	
	// Init copy
	m_pCopyThread->StartFileCopy(pInfo->GetOldLocation(), pInfo->GetNewLocation(), !pInfo->GetOverwrite());
}

//=====================================================================

void CInstallFiles::PerformActionDelete(CFileData * pFileData, int iActionIndex)
{
	// Get info
	CActionDelete * pInfo = (CActionDelete *)pFileData->GetAction(iActionIndex);
	CSettings * pSettings = CSettings::Instance();

	// Update info
	m_InstallStatus.sTitle.Format(m_pLanguage->GetString(IDS_ACTION_DELETE), pFileData->GetName());
	m_InstallStatus.iFileActions = CalculateActionsOfAction(m_iCurrentFile, m_iCurrentAction);
	m_InstallStatus.iFileActionsPerformed = 0;
	SendInstallStatus();

	// Check if we should ask user
	if (pInfo->GetAskConfirmation())
	{
		// We should ask
		PostMessageToParent(WMU_QUESTION, QUESTION_DELETEFILE, (WPARAM)pInfo);
	}
	else
	{
		// Delete file
		ContinueActionDelete();

		// Finish it
		FinishActionDelete();
	}
}

//=====================================================================

void CInstallFiles::PerformActionRun(CFileData * pFileData, int iActionIndex)
{
	// Get info
	CActionRun * pInfo = (CActionRun *)pFileData->GetAction(iActionIndex);

	// Declare variables
	CString sDirectory;

	// Update info
	m_InstallStatus.sTitle.Format(m_pLanguage->GetString(IDS_ACTION_RUN), pFileData->GetName());
	m_InstallStatus.iFileActions = CalculateActionsOfAction(m_iCurrentFile, m_iCurrentAction);
	m_InstallStatus.iFileActionsPerformed = 0;
	SendInstallStatus();

	// Check if the user tries to open a website
	if (!m_pFunctions->IsWebAddress(pInfo->GetRunLocation()))
	{
		// Check if file exists
		if (!PathFileExists(pInfo->GetRunLocation()))
		{
			// Send error message
			PostMessageToParent(WMU_ERROR, ERROR_FILENOTFOUND, 0);
			// Maybe we should even stop executing
			return;
		}
	}
	
	// Prepare shellExecutInfo
	sDirectory = m_pPath->ExtractFilePath(pInfo->GetRunLocation());
	SHELLEXECUTEINFO ShRun = {0};
	ShRun.cbSize = sizeof(SHELLEXECUTEINFO);
	ShRun.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShRun.hwnd = NULL;
	ShRun.lpVerb = NULL;
	ShRun.lpFile = pInfo->GetRunLocation();
	ShRun.lpParameters = pInfo->GetParameters();
	ShRun.lpDirectory = sDirectory;
	ShRun.nShow = SW_SHOW;
	ShRun.hInstApp = NULL;

	// Execute the file with the parameters
	if (!ShellExecuteEx(&ShRun))
	{
		// Send error message
		PostMessageToParent(WMU_ERROR, ERROR_RUNFILE, 0);
		// Maybe we should even stop executing
		return;
	}	
	
	// Should we wait?
	if (pInfo->GetWaitUntilReady())
	{
		// Wait until process is ready
		WaitForSingleObject(ShRun.hProcess, INFINITE);
	}

	// Update information
	m_InstallStatus.iFileActionsPerformed = ACTIONPOINTS_RUN;
	m_InstallStatus.iTotalActionsPerformed += ACTIONPOINTS_RUN;
	SendInstallStatus();

	// We are ready!
	PostThreadMessage(WMU_INSTALLACTION_COMPLETE, 0, 0);
}

//=====================================================================

void CInstallFiles::PerformActionUnzip(CFileData * pFileData, int iActionIndex)
{
	// Get info
	CActionUnzip * pInfo = (CActionUnzip *)pFileData->GetAction(iActionIndex);

	// Declare variables
	CUnzipper zipUnzip;
	int iFileCount;

	// Update info
	m_InstallStatus.sTitle.Format(m_pLanguage->GetString(IDS_ACTION_UNZIP), pFileData->GetName());
	m_InstallStatus.iFileActions = CalculateActionsOfAction(m_iCurrentFile, m_iCurrentAction);
	m_InstallStatus.iFileActionsPerformed = 0;
	SendInstallStatus();

	// Open the zip file
	if (!zipUnzip.OpenZip(pInfo->GetLocation()))
	{
		// Send error message
		PostMessageToParent(WMU_ERROR, ERROR_UNZIPFILE, 0);
		// Maybe we should even stop executing
		return;
	}

	// Do we have any files?
	iFileCount = zipUnzip.GetFileCount();
	if (iFileCount == 0)
		return;
	
	// Go to first file
	if (!zipUnzip.GotoFirstFile())
		return;
	
	// Unzip all files
	do
	{
		if (!zipUnzip.UnzipFile(pInfo->GetDestination()))
		{
			// Send error message
			PostMessageToParent(WMU_ERROR, ERROR_UNZIPFILE, 0);
			// Close zip file
			zipUnzip.CloseZip();
			// Maybe we should even stop executing
			return;
		}
		
		// Update information
		m_InstallStatus.iFileActionsPerformed++;
		m_InstallStatus.iTotalActionsPerformed++;
		SendInstallStatus();

		// Should we continue unzipping?
		if (!m_pParent)
		{
			// Stop yourself
			PostThreadMessage(WM_QUIT, 0, 0);
		}
	}
	while (zipUnzip.GotoNextFile());
	
	// Close zip file
	zipUnzip.CloseZip();

	// Update information
	m_InstallStatus.iFileActionsPerformed = m_InstallStatus.iFileActions;
	m_InstallStatus.iTotalActionsPerformed++;
	SendInstallStatus();

	// We are ready!
	PostThreadMessage(WMU_INSTALLACTION_COMPLETE, 0, 0);
}

//=====================================================================

void CInstallFiles::PerformActionSetFileAttr(CFileData * pFileData, int iActionIndex)
{
	// Get info
	CActionSetFileAttr * pInfo = (CActionSetFileAttr *)pFileData->GetAction(iActionIndex);

	// Update info
	m_InstallStatus.sTitle.Format(m_pLanguage->GetString(IDS_ACTION_SETFILEATTR), pFileData->GetName());
	m_InstallStatus.iFileActions = CalculateActionsOfAction(m_iCurrentFile, m_iCurrentAction);
	m_InstallStatus.iFileActionsPerformed = 0;
	SendInstallStatus();

	// Set file attributes
	SetFileAttributes(pInfo->GetLocation(), pInfo->GetAttributes());
	
	// Update information
	m_InstallStatus.iFileActionsPerformed = m_InstallStatus.iFileActions;
	m_InstallStatus.iTotalActionsPerformed++;
	SendInstallStatus();

	// We are ready!
	PostThreadMessage(WMU_INSTALLACTION_COMPLETE, 0, 0);
}

//=====================================================================

void CInstallFiles::PerformActionRegister(CFileData * pFileData, int iActionIndex)
{
	// Get info
	CActionRegister * pInfo = (CActionRegister *)pFileData->GetAction(iActionIndex);

	// Update info
	m_InstallStatus.sTitle.Format(m_pLanguage->GetString(IDS_ACTION_REGISTER), pFileData->GetName());
	m_InstallStatus.iFileActions = CalculateActionsOfAction(m_iCurrentFile, m_iCurrentAction);
	m_InstallStatus.iFileActionsPerformed = 0;
	SendInstallStatus();

	// Check if we should ask user
	if (pInfo->GetAskUser())
	{
		// We should ask
		PostMessageToParent(WMU_QUESTION, QUESTION_REGISTERFILE, (WPARAM)pInfo);
	}
	else
	{
		// Register file
		ContinueActionRegister();

		// Finish it
		FinishActionRegister();
	}
}

//=====================================================================

void CInstallFiles::ContinueActionDelete()
{
	// Declare variables
	CActionDelete * pInfo = (CActionDelete *) m_pUpdateInfo->GetFileData(m_iCurrentFile)->GetAction(m_iCurrentAction);

	// Delete the file
	DeleteFile(pInfo->GetDeleteLocation());
}

//=====================================================================

void CInstallFiles::FinishActionDelete()
{
	// Update information
	m_InstallStatus.iFileActionsPerformed = ACTIONPOINTS_DELETE;
	m_InstallStatus.iTotalActionsPerformed += ACTIONPOINTS_DELETE;
	SendInstallStatus();

	// We are ready with the install action, continue
	PostThreadMessage(WMU_INSTALLACTION_COMPLETE, 0, 0);
}

//=====================================================================

bool CInstallFiles::ContinueActionRegister()
{
	// Declare variables
	bool bFailed = false;
	CSettings * pSettings = CSettings::Instance();
	HMODULE hMod;
	DLLREGISTERSERVERPROC DllRegisterServerFunction;

	// Get info
	CActionRegister * pInfo = (CActionRegister *) m_pUpdateInfo->GetFileData(m_iCurrentFile)->GetAction(m_iCurrentAction);

	// Enable COM
	::CoInitialize(NULL);

	// Register file
	hMod = LoadLibrary(pInfo->GetLocation());

	// Check if we can load library
	if (hMod != NULL)
	{
		// Get proc address
		DllRegisterServerFunction = (DLLREGISTERSERVERPROC) GetProcAddress(hMod, "DllRegisterServer");

		// Check if we can run the function
		if (DllRegisterServerFunction != NULL)
		{
			// Register
			if (FAILED(DllRegisterServerFunction()))
			{
				// We failed
				bFailed = true;
			}
		}
		else
		{
			// We failed
			bFailed = true;
		}
	}
	else
	{
		// We failed
		bFailed = true;
	}

	// Disable COM
	::CoUninitialize();

	// Return result
	return !bFailed;
}

//=====================================================================

void CInstallFiles::FinishActionRegister()
{
	// Update information
	m_InstallStatus.iFileActionsPerformed = m_InstallStatus.iFileActions;
	m_InstallStatus.iTotalActionsPerformed++;
	SendInstallStatus();

	// We are ready with the install action, continue
	PostThreadMessage(WMU_INSTALLACTION_COMPLETE, 0, 0);
}

//=====================================================================

void CInstallFiles::SendInstallStatus()
{
	// Send status to parent
	if (m_pParent)
		PostMessageToParent(WMU_INSTALL_STATUS, (WPARAM)&m_InstallStatus, 0);	
}

//=====================================================================

void CInstallFiles::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

int CInstallFiles::CalculateActionsOfFile(int iFile)
{
	// Declare variables
	int iActionCount = 0;

	// Get file info
	CFileData * pTempFile = m_pUpdateInfo->GetFileData(iFile);

	// Should we install file?
	if (pTempFile->GetUpdateFile())
	{
		// Calculate for all actions
		for (int i = 0; i < pTempFile->GetActionCount(); i++)
		{
			iActionCount += CalculateActionsOfAction(iFile, i);
			if (m_pUpdateInfo->GetRollbackEnabled())
			{
				iActionCount += CalculateBackupActionsOfAction(iFile, i);
			}	
		}
	}

	// Return value
	return iActionCount;
}

//=====================================================================

int CInstallFiles::CalculateActionsOfAction(int iFile, int iAction)
{
	// Declare variables
	int iActionCount;

	// Get action info
	CActionBase * pTempAction = m_pUpdateInfo->GetFileData(iFile)->GetAction(iAction);
			
	// Calculate actionpoints
	iActionCount = CActionPoints::CalculateActionPoints(pTempAction);

	// Return value
	return iActionCount; 
}

//=====================================================================

int CInstallFiles::CalculateBackupActionsOfAction(int iFile, int iAction)
{
	// Declare variables
	int iActionCount = 0;

	// Get action info
	CActionBase * pTempAction = m_pUpdateInfo->GetFileData(iFile)->GetAction(iAction);
			
	// Check if action is unzip
	if (pTempAction->GetType() == ACTION_UNZIP)
	{
		// Get object
		CActionUnzip * pTempUnzip = (CActionUnzip *)pTempAction;

		// Get actionpoints
		iActionCount += CActionPoints::CalculateActionPoints(pTempUnzip->GetDestination());
	}
	
	// Check if action is copy
	if (pTempAction->GetType() == ACTION_COPY)
	{
		// Get object
		CActionCopy * pTempCopy = (CActionCopy *)pTempAction;
		
		// Get actionpoints
		iActionCount += CActionPoints::CalculateActionPoints(pTempCopy->GetNewLocation());
	}

	// Check if action is delete
	if (pTempAction->GetType() == ACTION_DELETE)
	{
		// Run is 1 action
		iActionCount += ACTIONPOINTS_DELETE;
	}

	// Check if action is register
	if (pTempAction->GetType() == ACTION_REGISTER)
	{
		// Run is 1 action
		iActionCount += ACTIONPOINTS_REGISTER;
	}

	// Return value
	return iActionCount; 
}

//=====================================================================

void CInstallFiles::OnInstallStart(WPARAM wParam, LPARAM lParam)
{
	// Install first file
	m_iCurrentFile = -1;
	InstallNextFile();
}

//=====================================================================

void CInstallFiles::OnFileComplete(WPARAM wParam, LPARAM lParam)
{
	// Start next file
	InstallNextFile();
}

//=====================================================================

void CInstallFiles::OnActionComplete(WPARAM wParam, LPARAM lParam)
{
	// Start next action
	PerformNextAction();
}

//=====================================================================

void CInstallFiles::OnBackUpStatus(WPARAM wParam, LPARAM lParam)
{
	// Update data
	m_InstallStatus.iTotalActionsPerformed += wParam - m_InstallStatus.iFileActionsPerformed;
	m_InstallStatus.iFileActionsPerformed += wParam - m_InstallStatus.iFileActionsPerformed;

	SendInstallStatus();
}

//=====================================================================

void CInstallFiles::OnBackUpComplete(WPARAM wParam, LPARAM lParam)
{	
	// Declare variables
	CString sActionType;

	// Check what action to perform
	sActionType = m_pUpdateInfo->GetFileData(m_iCurrentFile)->GetAction(m_iCurrentAction)->GetType();

	// Calculate action points
	m_InstallStatus.iTotalActionsPerformed += wParam - m_InstallStatus.iFileActionsPerformed;
	m_InstallStatus.iFileActionsPerformed = m_InstallStatus.iFileActions;

	// Copy
	if (sActionType == ACTION_COPY)
	{
		// Perform action immediatly
		PerformActionCopy(m_pUpdateInfo->GetFileData(m_iCurrentFile), m_iCurrentAction);
	}

	// Delete
	if (sActionType == ACTION_DELETE)
	{
		// Perform action
		PerformActionDelete(m_pUpdateInfo->GetFileData(m_iCurrentFile), m_iCurrentAction);
	}	

	// Unzip
	if (sActionType == ACTION_UNZIP)
	{
		// Perform action
		PerformActionUnzip(m_pUpdateInfo->GetFileData(m_iCurrentFile), m_iCurrentAction);
	}

	// Register
	if (sActionType == ACTION_REGISTER)
	{
		// Perform action
		PerformActionRegister(m_pUpdateInfo->GetFileData(m_iCurrentFile), m_iCurrentAction);
	}
}

//=====================================================================

void CInstallFiles::OnError(WPARAM wParam, LPARAM lParam)
{
	// An error occurred!
	PostMessageToParent(WMU_ERROR, wParam, lParam);
}

//=====================================================================

void CInstallFiles::OnWarningComplete(WPARAM wParam, LPARAM lParam)
{
	// We are ready with the install action, continue
	PostThreadMessage(WMU_INSTALLACTION_COMPLETE, 0, 0);
}

//=====================================================================

void CInstallFiles::OnQuestionAnswer(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iQuestion = wParam;
	int iAnswer = lParam;

	// Check what question has an answer
	switch (iQuestion)
	{
	case QUESTION_DELETEFILE:
		// Check answer
		if (iAnswer == QUESTIONRESULT_YES)
		{
			// Continue deleting
			ContinueActionDelete();
		}
		
		// Finish deleting
		FinishActionDelete();
		break;

	case QUESTION_REGISTERFILE:
		// Check answer
		if (iAnswer == QUESTIONRESULT_YES)
		{
			// Continue registering file
			if (!ContinueActionRegister())
			{
				// Post warning
				PostMessageToParent(WMU_WARNING, WARNING_REGISTER, 0);
				return;
			}
		}

		// Finish registering
		FinishActionRegister();
		break;
	}
}

//=====================================================================

void CInstallFiles::OnCopyStatus(WPARAM wParam, LPARAM lParam)
{
	// Calculate actions performed
	m_InstallStatus.iTotalActionsPerformed += wParam - m_InstallStatus.iFileActionsPerformed;
	m_InstallStatus.iFileActionsPerformed += wParam - m_InstallStatus.iFileActionsPerformed;

	// Send install status
	SendInstallStatus();
}

//=====================================================================

void CInstallFiles::OnCopyComplete(WPARAM wParam, LPARAM lParam)
{	
	// Update information
	m_InstallStatus.iTotalActionsPerformed += m_InstallStatus.iFileActions - m_InstallStatus.iFileActionsPerformed;
	m_InstallStatus.iFileActionsPerformed = m_InstallStatus.iFileActions;

	// Send install status
	SendInstallStatus();

	// Start next action
	PerformNextAction();
}

//=====================================================================

void CInstallFiles::OnSetParent(WPARAM wParam, LPARAM lParam)
{
	// Set parent
	m_pParent = (CWinThread *)wParam;
}