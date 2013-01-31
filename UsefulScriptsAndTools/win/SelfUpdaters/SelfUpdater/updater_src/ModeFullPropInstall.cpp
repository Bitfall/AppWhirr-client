/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:16
	filename: 	ModeFullPropInstall.cpp
	file path:	Updater
	file base:	ModeFullPropInstall
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeFullPropInstall.h"
#include "ModeFullSheet.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeFullPropInstall, CPropertyPageEx)
	ON_MESSAGE(WMU_UPDATE_GUI_FILEPROGRESS, OnUpdateGUIFileProgress)
	ON_MESSAGE(WMU_UPDATE_GUI_TOTALPROGRESS, OnUpdateGUITotalProgress)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeFullPropInstall, CPropertyPageEx)

//=====================================================================

CModeFullPropInstall::CModeFullPropInstall()
	: CPropertyPageEx(CModeFullPropInstall::IDD)
{
	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();

	// Set title
	m_strHeaderTitle = m_pLanguage->GetString(IDS_FULL_INSTALL_TITLE);
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_INSTALL_SUBTITLE),
				 m_pSettings->GetAppName());
	m_strHeaderSubTitle = sTemp;
}

//=====================================================================

CModeFullPropInstall::~CModeFullPropInstall()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CModeFullPropInstall::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	DDX_Control(pDX, lblFileProgress, m_lblFileProgress);
	DDX_Control(pDX, prgFileProgress, m_prgFileProgress);
	DDX_Control(pDX, lblFileProgressProcent, m_lblFileProgressProcent);
	DDX_Control(pDX, lblTotalProgress, m_lblTotalProgress);
	DDX_Control(pDX, prgTotalProgress, m_prgTotalProgress);
	DDX_Control(pDX, lblTotalProgressProcent, m_lblTotalProgressProcent);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
}

//=====================================================================

BOOL CModeFullPropInstall::OnInitDialog()
{
	// Call original function
	CPropertyPageEx::OnInitDialog();

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropInstall::OnSetActive()
{
	// Call original function
	CPropertyPageEx::OnSetActive();
	
	// Load language strings
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_INSTALL_EXPLANATION), m_pSettings->GetAppName());
	m_lblExplanation.SetWindowText(sTemp);
	m_lblFileProgress.SetWindowText(m_pLanguage->GetString(IDS_STATUS_INITIALIZING));
	m_lblTotalProgress.SetWindowText(m_pLanguage->GetString(IDS_STATUS_INITIALIZING));
	
	// Load default values for progress
	m_prgFileProgress.SetRange32(0, 100);
	m_lblFileProgressProcent.SetWindowText(_T("0%"));
	m_prgTotalProgress.SetRange32(0, 100);
	m_lblTotalProgressProcent.SetWindowText(_T("0%"));

	// Return value
	return TRUE;
}

//=====================================================================

LRESULT CModeFullPropInstall::OnWizardNext()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();
	
	// Start next task
	pParent->PressButton(PSBTN_NEXT);
	
	// Don't change page yourself
	return -1;
}

//=====================================================================

LRESULT CModeFullPropInstall::OnUpdateGUIFileProgress(WPARAM wParam, LPARAM lParam)
{
	// Declare temp variable
	CString sTemp, sCompare;
	
	// Get file progress
	m_prgFileProgress.SetPos(static_cast<int>(wParam));
	sTemp.Format(_T("%d%%"), static_cast<int>(wParam));
	m_lblFileProgressProcent.SetWindowText(sTemp);
	
	// Get task name
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_FILEPROGRESS), lParam);
	m_lblFileProgress.GetWindowText(sCompare);
	if (sCompare != sTemp)
		m_lblFileProgress.SetWindowText(sTemp);

	return S_OK;
}

//=====================================================================

LRESULT CModeFullPropInstall::OnUpdateGUITotalProgress(WPARAM wParam, LPARAM lParam)
{
	// Declare temp variable
	CString sTemp, sCompare;
	
	// Get total progress
	m_prgTotalProgress.SetPos(static_cast<int>(wParam));
	sTemp.Format(_T("%d%%"), static_cast<int>(wParam));
	m_lblTotalProgressProcent.SetWindowText(sTemp);

	// Get task name
	m_lblTotalProgress.SetWindowText(m_pLanguage->GetString(IDS_FULL_TOTALPROGRESS));

	return S_OK;
}
