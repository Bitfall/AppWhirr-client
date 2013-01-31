/********************************************************************
	created:	2005/06/20
	created:	20:06:2005   12:16
	filename: 	ModeFullPropLicense.cpp
	file path:	Updater
	file base:	ModeFullPropLicense
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeFullPropLicense.h"
#include "ModeFullSheet.h"
#include "Sections.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeFullPropLicense, CPropertyPageEx)
	ON_BN_CLICKED(rdLicenceAgree, OnAgree)
	ON_BN_CLICKED(rdLicenceNotAgree, OnNotAgree)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeFullPropLicense, CPropertyPageEx)

//=====================================================================

CModeFullPropLicense::CModeFullPropLicense()
	: CPropertyPageEx(CModeFullPropLicense::IDD)
{
	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();

	// Set title
	m_strHeaderTitle = m_pLanguage->GetString(IDS_FULL_LICENSE_TITLE);
	m_strHeaderSubTitle = m_pLanguage->GetString(IDS_FULL_LICENSE_SUBTITLE);
}

//=====================================================================

CModeFullPropLicense::~CModeFullPropLicense()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CModeFullPropLicense::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	DDX_Control(pDX, rdLicenceAgree, m_rdLicenseAgree);
	DDX_Control(pDX, rdLicenceNotAgree, m_rdLicenseNotAgree);
	DDX_Control(pDX, txtLicense, m_txtLicense);
}

//=====================================================================

BOOL CModeFullPropLicense::OnInitDialog()
{
	// Call original function
	CPropertyPageEx::OnInitDialog();

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropLicense::OnSetActive()
{
	// Declare variables

	// Call original function
	CPropertyPageEx::OnSetActive();
	
	// Set text for radio buttons
	m_rdLicenseAgree.SetWindowText(m_pLanguage->GetString(IDS_FULL_LICENSE_AGREE));
	m_rdLicenseNotAgree.SetWindowText(m_pLanguage->GetString(IDS_FULL_LICENSE_NOTAGREE));

	// Load license agreement
	m_txtLicense.SetWindowText(m_pUpdateInfo->GetLicense());

	// Select the not agree radio button
	CheckRadioButton(rdLicenceAgree, rdLicenceNotAgree, rdLicenceNotAgree);

	// Set focus to radio button
	m_rdLicenseNotAgree.SetFocus();

	// Return value
	return TRUE;
}

//=====================================================================

LRESULT CModeFullPropLicense::OnWizardNext()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();
	
	// Start next task
	pParent->PressButton(PSBTN_NEXT);
	
	// Don't change page yourself
	return -1;
}

//=====================================================================

void CModeFullPropLicense::OnAgree()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();

	// Enable next button
	pParent->SetWizardButtons(PSWIZB_NEXT);
}

//=====================================================================

void CModeFullPropLicense::OnNotAgree()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();

	// Disable next button
	pParent->SetWizardButtons(NULL);
}