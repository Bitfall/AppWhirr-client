/********************************************************************
	created:	2005/10/02
	created:	2:10:2005   12:31
	filename: 	RestoreDlg.cpp
	file path:	Updater
	file base:	RestoreDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "RestoreDlg.h"

#include "Sections.h"
#include "Restore.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CRestoreDlg, CDialog)
	ON_COMMAND(btnRestore, OnBtnRestore)
	ON_COMMAND(btnClose, OnBtnClose)
	ON_MESSAGE(WMU_UPDATE_GUI_TOTALPROGRESS, OnUpdateGUIProgress)
	ON_MESSAGE(WMU_TASK_COMPLETE, OnRestoreComplete)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CRestoreDlg::CRestoreDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRestoreDlg::IDD, pParent)
{
	// Restore is not running
	m_bRestoreRunning = false;
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

void CRestoreDlg::OnBtnRestore()
{
	// Declare variables
	CRestore * pRestore;
	CRestoreInfo * pRestoreInfo = CRestoreInfo::Instance();

	// Disable close button
	m_bRestoreRunning = true;
	m_btnClose.EnableWindow(FALSE);

	// Read information from file
	pRestoreInfo->ReadFromFile(GetPreviousVersionLocation() + FILENAME_RESTORE);

	// Rollback all actions
	pRestore = (CRestore *)AfxBeginThread(RUNTIME_CLASS(CRestore),
										  THREAD_PRIORITY_NORMAL,
										  0,
										  0,
										  NULL);

	// Check if thread is successfully started
	if (pRestore != NULL)
	{
		// Inform who the parent is
		pRestore->PostThreadMessage(WMU_SETPARENT, (WPARAM)this, 0);
		
		// Now really start thread
		pRestore->PostThreadMessage(WMU_THREADMESSAGE, THREAD_START, 0);
	}
}

//=====================================================================

void CRestoreDlg::OnBtnClose()
{
	// Close dialog
	EndDialog(IDOK);
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CRestoreDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, prgRestore, m_prgRestore);
	DDX_Control(pDX, btnRestore, m_btnRestore);
	DDX_Control(pDX, btnClose, m_btnClose);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
	DDX_Control(pDX, lblProgress, m_lblProgress);
}

//=====================================================================

BOOL CRestoreDlg::OnInitDialog()
{
	// Declare variables
	CLanguage * pLanguage = CLanguage::Instance();
	CSettings * pSettings = CSettings::Instance();
	CString sTemp, sPreviousVersion;

	// Call original function
	CDialog::OnInitDialog();

	// Set icon
	SetIcon(AfxGetApp()->LoadIcon(IDI_RESTORE), TRUE);

	// Load strings
	SetWindowText(pLanguage->GetString(IDS_RESTORE_TITLE));
	sPreviousVersion = GetPreviousVersion();
	sTemp.Format(pLanguage->GetString(IDS_RESTORE_EXPLANATION), pSettings->GetAppName(),
		pSettings->GetAppName(), sPreviousVersion);
	m_lblExplanation.SetWindowText(sTemp);
	m_lblProgress.SetWindowText(pLanguage->GetString(IDS_RESTORE_PROGRESS));
	m_btnRestore.SetWindowText(pLanguage->GetString(IDS_RESTORE_RESTORE));
	m_btnClose.SetWindowText(pLanguage->GetString(IDS_RESTORE_CLOSE));

	// Check if previous version folder contains a restoreinfo.xml
	if (!PathFileExists(GetPreviousVersionLocation() + FILENAME_RESTORE))
		EndDialog(IDCANCEL);
	
	// Set progress
	m_prgRestore.SetRange32(0, 100);

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

BOOL CRestoreDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// If user presses cancel
	if(LOWORD(wParam) == IDCANCEL)
	{
		// Handle close updater
		if (!CloseDialog())
			return TRUE;
	}

	// Always call the original function
	return CDialog::OnCommand(wParam, lParam);
}

//=====================================================================

void CRestoreDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// If someone is closing the app
	if (nID == 61536)
	{
		// Handle close updater		
		if (!CloseDialog())
			return;
	}

	// Call original function
	CDialog::OnSysCommand(nID, lParam);
}

//=====================================================================

bool CRestoreDlg::CloseDialog()
{
	// Check if we can close dialog
	if (m_bRestoreRunning)
		return false;

	return true;
}

//=====================================================================

CString CRestoreDlg::GetPreviousVersion()
{
	// Declare variables
	CString sVersion = _T("0.0.0.0"), sTemp, sRoot, sPattern;
	CPath * pPath = CPath::Instance();
	CSettings * pSettings = CSettings::Instance();
	HANDLE hFile;
	WIN32_FIND_DATA FileInformation;

	// Get the Updater Temp folder
	sRoot = pPath->ExtractFilePath(pPath->ExtractFilePath(pPath->GetPathUpdaterTemp()));

	// Now search for directories in the temp folder
	sPattern = sRoot + _T("\\*.*");
	hFile = ::FindFirstFile(sPattern, &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Is it something else then current or top folder?
			if ((FileInformation.cFileName[0] != '.') && (FileInformation.cFileName[0] != '..'))
			{
				// Is it a folder?
				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// Treat folder name as version
					sTemp = FileInformation.cFileName;

					// Check if it is not current version
					if (pSettings->GetAppVersion() != sTemp)
						sVersion = sTemp;
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);
	}

	// Return value
	return sVersion;
}

//=====================================================================

CString CRestoreDlg::GetPreviousVersionLocation()
{
	// Declare variables
	CPath * pPath = CPath::Instance();
	CString sLocation;

	// Get the Updater Temp folder
	sLocation = pPath->ExtractFilePath(pPath->ExtractFilePath(pPath->GetPathUpdaterTemp()));

	// Add version number
	sLocation += _T("\\") + GetPreviousVersion() + _T("\\");

	// Return value
	return sLocation;
}

//=====================================================================

LRESULT CRestoreDlg::OnUpdateGUIProgress(WPARAM wParam, LPARAM lParam)
{
	// Declare temp variable
	CLanguage * pLanguage = CLanguage::Instance();

	// Get total progress
	m_prgRestore.SetPos(static_cast<int>(wParam));

	return S_OK;
}

//=====================================================================

LRESULT CRestoreDlg::OnRestoreComplete(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CSettings * pSettings = CSettings::Instance();
	CSections * pSections = CSections::Instance();
	CPath * pPath = CPath::Instance();
	CRestoreInfo * pRestoreInfo = CRestoreInfo::Instance();

	// Write old version to settings.ini
#ifndef _DEBUG
	// Change version in settings.ini
	if (!PathFileExists(pSettings->GetAppVersion()))
	{
		// It is not a file, we can overwrite the settings
		WritePrivateProfileString(_T("APPLICATION"), _T("version"), GetPreviousVersion(), CInternalData::Instance()->GetSettingsFile());
	}

	// Write all data to section file
	for (int i = 0; i < pRestoreInfo->GetSectionCount(); i++)
	{
		pSections->SetSectionVersion(pRestoreInfo->GetSection(i).sName,
			pRestoreInfo->GetSection(i).sVersion);
	}
	pSections->WriteFile(pPath->GetPathUpdater() + FILENAME_SECTIONS);	
#endif

	// Set progress to 100 %
	m_prgRestore.SetPos(100);

	// Enable close button
	m_btnClose.EnableWindow();
	m_btnRestore.EnableWindow(FALSE);
	m_bRestoreRunning = false;

	return S_OK;
}