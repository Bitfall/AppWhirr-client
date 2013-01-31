/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:19
	filename: 	ModeFullPropProtection.cpp
	file path:	Updater
	file base:	ModeFullPropProtection
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeFullPropProtection.h"
#include "ModeFullSheet.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeFullPropProtection, CPropertyPageEx)
	ON_MESSAGE(WMU_UPDATE_GUI_PROTECTION, OnUpdateGUIProtection)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeFullPropProtection, CPropertyPageEx)

//=====================================================================

CModeFullPropProtection::CModeFullPropProtection()
	: CPropertyPageEx(CModeFullPropProtection::IDD)
{
	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();

	// Set title
	m_strHeaderTitle = m_pLanguage->GetString(IDS_FULL_PROTECTION_TITLE);
	m_strHeaderSubTitle = m_pLanguage->GetString(IDS_FULL_PROTECTION_SUBTITLE);
}

//=====================================================================

CModeFullPropProtection::~CModeFullPropProtection()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CModeFullPropProtection::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
	DDX_Control(pDX, lblCheckLicense, m_lblCheckLicense);
	DDX_Control(pDX, lblCheckFiles, m_lblCheckFiles);
	DDX_Control(pDX, lblGetLicense, m_lblGetLicense);
}

//=====================================================================

BOOL CModeFullPropProtection::OnInitDialog()
{
	// Call original function
	CPropertyPageEx::OnInitDialog();

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropProtection::OnSetActive()
{
	// Call original function
	CPropertyPageEx::OnSetActive();
	
	// Load language strings
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_PROTECTION_EXPLANATION), m_pSettings->GetAppName(),
		m_pSettings->GetAppName());
	m_lblExplanation.SetWindowText(sTemp);
	m_lblCheckLicense.SetWindowText(m_pLanguage->GetString(IDS_FULL_PROTECTION_CHECKLICENSE));
	m_lblCheckFiles.SetWindowText(m_pLanguage->GetString(IDS_FULL_PROTECTION_CHECKFILES));
	m_lblGetLicense.SetWindowText(m_pLanguage->GetString(IDS_FULL_PROTECTION_GETLICENSE));

	// Set some settings for the hyperlink
	m_lblGetLicense.SetAutoResize(true);
	m_lblGetLicense.SetHyperlink(m_pUpdateInfo->GetProtectionWebsite());
	m_lblGetLicense.SetColorLink(m_pSettings->GetLinkColorLink());
	m_lblGetLicense.SetColorHover(m_pSettings->GetLinkColorHover());	

	// Set items visible or invisible
	m_lblGetLicense.ShowWindow(SW_HIDE);

	if (m_pUpdateInfo->GetProtectionRegistrationDll().IsEmpty())
		m_lblCheckLicense.ShowWindow(SW_HIDE);
	else
		m_lblCheckLicense.ShowWindow(SW_SHOW);

	if (m_pUpdateInfo->GetProtectionHashCount() == 0)
		m_lblCheckFiles.ShowWindow(SW_HIDE);
	else
		m_lblCheckFiles.ShowWindow(SW_SHOW);

	// Set default states
	m_lblCheckLicense.SetState(LBLRESULT_TOCHECK);
	m_lblCheckFiles.SetState(LBLRESULT_TOCHECK);

	// Return value
	return TRUE;
}

//=====================================================================

LRESULT CModeFullPropProtection::OnWizardNext()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();
	
	// Start next task
	pParent->PressButton(PSBTN_NEXT);
	
	// Don't change page yourself
	return -1;
}

//=====================================================================

BOOL CModeFullPropProtection::OnWizardFinish()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();
	
	// Start next task
	pParent->PressButton(PSWIZB_FINISH);
	
	// Don't change page yourself
	return -1;
}

//=====================================================================

LRESULT CModeFullPropProtection::OnUpdateGUIProtection(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iTask = wParam;
	int iState = lParam;

	// Select right task
	switch (iTask)
	{
	case PROTECTION_TASK_LICENSE:
		// Set state
		m_lblCheckLicense.SetState(iState);
		break;

	case PROTECTION_TASK_HASH:
		// Set state
		m_lblCheckFiles.SetState(iState);
		break;
	}

	// If we had an error
	if (iState == PROTECTION_STATE_NOTOK)
	{
		// Show hyperlink
		if (!m_pUpdateInfo->GetProtectionWebsite().IsEmpty())
			m_lblGetLicense.ShowWindow(SW_SHOW);
	}

	return S_OK;
}