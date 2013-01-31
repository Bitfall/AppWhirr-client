/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:16
	filename: 	ModeFullPropRollback.cpp
	file path:	Updater
	file base:	ModeFullPropRollback
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeFullPropRollback.h"
#include "ModeFullSheet.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeFullPropRollback, CPropertyPageEx)
	ON_MESSAGE(WMU_UPDATE_GUI_TOTALPROGRESS, OnUpdateGUITotalProgress)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeFullPropRollback, CPropertyPageEx)

//=====================================================================

CModeFullPropRollback::CModeFullPropRollback()
	: CPropertyPageEx(CModeFullPropRollback::IDD)
{
	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();

	// Set title
	m_strHeaderTitle = m_pLanguage->GetString(IDS_FULL_ROLLBACK_TITLE);
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_ROLLBACK_SUBTITLE),
				 m_pSettings->GetAppName());
	m_strHeaderSubTitle = sTemp;
}

//=====================================================================

CModeFullPropRollback::~CModeFullPropRollback()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CModeFullPropRollback::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	DDX_Control(pDX, lblTotalProgress, m_lblTotalProgress);
	DDX_Control(pDX, prgTotalProgress, m_prgTotalProgress);
	DDX_Control(pDX, lblTotalProgressProcent, m_lblTotalProgressProcent);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
}

//=====================================================================

BOOL CModeFullPropRollback::OnInitDialog()
{
	// Call original function
	CPropertyPageEx::OnInitDialog();

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropRollback::OnSetActive()
{
	// Call original function
	CPropertyPageEx::OnSetActive();
	
	// Load language strings
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_ROLLBACK_EXPLANATION), m_pSettings->GetAppName());
	m_lblExplanation.SetWindowText(sTemp);
	m_lblTotalProgress.SetWindowText(m_pLanguage->GetString(IDS_STATUS_INITIALIZING));
	
	// Load default values for progress
	m_prgTotalProgress.SetRange32(0, 100);
	m_lblTotalProgressProcent.SetWindowText(_T("0%"));

	// Return value
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropRollback::OnWizardFinish()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();
	
	// Start next task
	pParent->PressButton(PSWIZB_FINISH);
	
	// Don't change page yourself
	return -1;
}

//=====================================================================

LRESULT CModeFullPropRollback::OnUpdateGUITotalProgress(WPARAM wParam, LPARAM lParam)
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
