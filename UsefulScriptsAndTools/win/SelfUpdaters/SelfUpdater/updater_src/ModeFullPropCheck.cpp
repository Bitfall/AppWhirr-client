/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:16
	filename: 	ModeFullPropCheck.cpp
	file path:	Updater
	file base:	ModeFullPropCheck
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeFullPropCheck.h"
#include "ModeFullSheet.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeFullPropCheck, CPropertyPageEx)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeFullPropCheck, CPropertyPageEx)

//=====================================================================

CModeFullPropCheck::CModeFullPropCheck()
	: CPropertyPageEx(CModeFullPropCheck::IDD)
{
	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();

	// Set title
	m_strHeaderTitle = m_pLanguage->GetString(IDS_FULL_CHECKVERSION_TITLE);
	m_strHeaderSubTitle = m_pLanguage->GetString(IDS_FULL_CHECKVERSION_SUBTITLE);
}

//=====================================================================

CModeFullPropCheck::~CModeFullPropCheck()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

void CModeFullPropCheck::SetVersionResult(bool bNewVersion)
{
	// Check what result we have
	if (!bNewVersion)
	{
		// Now new version available
		if (m_lblStatus.GetSafeHwnd() != NULL)
		{
			m_lblStatus.SetWindowText(m_pLanguage->GetString(IDS_STATUS_NONEWVERSION));
		}
	}
	else
	{
		// Set new version text
		CString sTemp;
		sTemp.Format(m_pLanguage->GetString(IDS_STATUS_NEWVERSION), m_pUpdateInfo->GetNewVersion());
		if (m_lblStatus.GetSafeHwnd() != NULL)
		{
			m_lblStatus.SetWindowText(sTemp);
		}

		// Set main feature text
		sTemp.Format(_T("%s:\n"), m_pLanguage->GetString(IDS_FULL_CHECKVERSION_FEATURES));
		if (m_lblFeatures.GetSafeHwnd() != NULL)
		{
			m_lblFeatures.SetWindowText(sTemp);
		}

		// Loop all features
		for (int i = 0; i < m_pUpdateInfo->GetFeatureCount(); i++)
		{
			// Set up text
			sTemp.Format(_T("  %c %s"), 187, m_pUpdateInfo->GetFeature(i).sText);

			if (m_arrFeatures[i].GetSafeHwnd() != NULL)
			{
				m_arrFeatures[i].SetWindowText(sTemp);

				// Set hyperlink
				m_arrFeatures[i].SetHyperlink(m_pUpdateInfo->GetFeature(i).sHyperlink);

				// Auto resize
				m_arrFeatures[i].SetAutoResize(true);

				// Set link colors
				m_arrFeatures[i].SetColorLink(m_pSettings->GetLinkColorLink());
				m_arrFeatures[i].SetColorHover(m_pSettings->GetLinkColorHover());

				// Show label
				m_arrFeatures[i].ShowWindow(SW_SHOW);
			}
		}
		
		// Check if we should show label
		if (m_pUpdateInfo->GetFeatureCount() > 0)
		{
			if (m_lblFeatures.GetSafeHwnd() != NULL)
			{
				m_lblFeatures.ShowWindow(SW_SHOW);	
			}
		}
	}
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CModeFullPropCheck::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	DDX_Control(pDX, lblStatus, m_lblStatus);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
	DDX_Control(pDX, lblFeatures, m_lblFeatures);
	DDX_Control(pDX, lblFeature1, m_arrFeatures[0]);
	DDX_Control(pDX, lblFeature2, m_arrFeatures[1]);
	DDX_Control(pDX, lblFeature3, m_arrFeatures[2]);
	DDX_Control(pDX, lblFeature4, m_arrFeatures[3]);
	DDX_Control(pDX, lblFeature5, m_arrFeatures[4]);
}

//=====================================================================

BOOL CModeFullPropCheck::OnInitDialog()
{
	// Call original function
	CPropertyPageEx::OnInitDialog();

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropCheck::OnSetActive()
{
	// Call original function
	CPropertyPageEx::OnSetActive();

	// Load language strings
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_CHECKVERSION_EXPLANATION), m_pSettings->GetAppName());
	m_lblExplanation.SetWindowText(sTemp);
	m_lblStatus.SetWindowText(m_pLanguage->GetString(IDS_STATUS_CONNECTINGTOSERVER));
	
	// Hide features
	m_lblFeatures.ShowWindow(SW_HIDE);

	// Return value
	return TRUE;
}

//=====================================================================

LRESULT CModeFullPropCheck::OnWizardNext()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();

	// Start next task
	pParent->PressButton(PSBTN_NEXT);

	// Don't change page yourself
	return -1;
}

//=====================================================================

BOOL CModeFullPropCheck::OnWizardFinish()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();
	
	// Start next task
	pParent->PressButton(PSWIZB_FINISH);
	
	// Don't change page yourself
	return -1;
}
