/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:19
	filename: 	ModeFullPropHtmlMessage.cpp
	file path:	Updater
	file base:	ModeFullPropHtmlMessage
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeFullPropHtmlMessage.h"
#include "ModeFullSheet.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeFullPropHtmlMessage, CPropertyPageEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeFullPropHtmlMessage, CPropertyPageEx)

//=====================================================================

CModeFullPropHtmlMessage::CModeFullPropHtmlMessage()
	: CPropertyPageEx(CModeFullPropHtmlMessage::IDD)
{
	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();

	// Set title
	m_strHeaderTitle = m_pLanguage->GetString(IDS_FULL_HTMLMESSAGE_TITLE);
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_HTMLMESSAGE_SUBTITLE),
				 m_pSettings->GetAppName());
	m_strHeaderSubTitle = sTemp;
}

//=====================================================================

CModeFullPropHtmlMessage::~CModeFullPropHtmlMessage()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CModeFullPropHtmlMessage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	DDX_Control(pDX, htmlMessage, m_htmlMessage);
}

//=====================================================================

BOOL CModeFullPropHtmlMessage::OnInitDialog()
{
	// Call original function
	CPropertyPageEx::OnInitDialog();

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropHtmlMessage::OnSetActive()
{
	// Call original function
	CPropertyPageEx::OnSetActive();

	// Navigate to the right page
	m_htmlMessage.Navigate(m_pUpdateInfo->GetHtmlMessageURL(), NULL, NULL, NULL, NULL);

	// Set countdown value
	m_iCountDown = m_pUpdateInfo->GetHtmlMessageTime();
	if (m_iCountDown > 0)
	{
		// Increase timer value
		m_iCountDown++;

		// Call timer function once yourself
		OnTimer(TIMER_COUNTDOWN);

		// Start timer
		SetTimer(TIMER_COUNTDOWN, 1000, NULL);
	}

	// Return value
	return TRUE;
}

//=====================================================================

void CModeFullPropHtmlMessage::OnTimer(UINT nIDEvent)
{
	// Declare variables
	CString sTemp;

	// Did we receive the right timer event?
	if (nIDEvent == TIMER_COUNTDOWN)
	{
		// Get parent
		CModeFullSheet * pParent = (CModeFullSheet *)GetParent();

		// Lower the timer
		m_iCountDown--;

		// Set up text
		if (m_iCountDown > 0)
		{
			// Show next text + remaining time
			sTemp.Format(_T("%s (%d)"), m_pLanguage->GetString(IDS_GENERAL_NEXT), m_iCountDown);
		}
		else
		{
			// Show normal next text
			sTemp = m_pLanguage->GetString(IDS_GENERAL_NEXT);

			// Enable the button
			pParent->SetWizardButtons(PSWIZB_NEXT);

			// Kill timer
			KillTimer(TIMER_COUNTDOWN);
		}

		// Set text
		CWnd * pNext = pParent->GetDlgItem(ID_WIZNEXT);
		if (pNext)
			pNext->SetWindowText(sTemp);
	}
}

//=====================================================================

LRESULT CModeFullPropHtmlMessage::OnWizardNext()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();

	// Start next task
	pParent->PressButton(PSBTN_NEXT);

	// Don't change page yourself
	return -1;
}

//=====================================================================