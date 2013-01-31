/********************************************************************
	created:	2005/08/06
	created:	6:8:2005   10:24
	filename: 	ErrorDlg.cpp
	file path:	Updater
	file base:	ErrorDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "ErrorDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CErrorDlg, CDialog)
	ON_COMMAND(btnOK, OnBtnOK)
	ON_COMMAND(btnMore, OnBtnMore)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CErrorDlg::CErrorDlg(CString sWindowCaption /* = "Error" */, CString sTitle /* = "Error" */,
					 CString sExplanation /* = "" */, CString sMore /* = "" */,
					 CString sOK /* = "OK" */, CString sMoreButton /* = "More" */)
	: CDialog(CErrorDlg::IDD, NULL)
{
	// Set data
	m_sWindowCaption = sWindowCaption;
	m_sTitle = sTitle;
	m_sExplanation = sExplanation;
	m_sMore = sMore;
	m_sOK = sOK;
	m_sMoreButton = sMoreButton;
	m_bExpanded = false;
}

//=====================================================================

CErrorDlg::~CErrorDlg()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CErrorDlg::OnInitDialog()
{
	// Declare variables
	CString sTemp;

	// Call original function
	CDialog::OnInitDialog();

	// Set window text
	sTemp.Format(_T("%s >>"), m_sMoreButton);
	SetWindowText(m_sWindowCaption);
	m_lblTitle.SetWindowText(m_sTitle);
	m_lblExplanation.SetWindowText(m_sExplanation);

	m_lblMore.SetWindowText(m_sMore);
	m_btnOK.SetWindowText(m_sOK);
	m_btnMore.SetWindowText(sTemp);

	// Create and set font
	m_fntTitle.DeleteObject();
	m_fntTitle.CreateFont(
		20,								// nHeight
		0,								// nWidth
		0,								// nEscapement
		0,								// nOrientation
		FW_BOLD,						// nWeight
		FALSE,							// bItalic
		FALSE,							// bUnderline
		0,								// cStrikeOut
		DEFAULT_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,				// nOutPrecision
		CLIP_DEFAULT_PRECIS,			// nClipPrecision
		PROOF_QUALITY,					// nQuality
		DEFAULT_PITCH | FF_DONTCARE,	// nPitchAndFamily
		NULL);
	m_lblTitle.SetFont(&m_fntTitle);

	// Should we disable the more button?
	if (m_sMore.IsEmpty())
		m_btnMore.EnableWindow(FALSE);

	// Auto resize controls
	AutoResizeControls();

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, bmpLogo, m_bmpLogo);
	DDX_Control(pDX, lblTitle, m_lblTitle);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
	DDX_Control(pDX, lblMore, m_lblMore);
	DDX_Control(pDX, lnMoreSplitter, m_lnMoreSplitter);
	DDX_Control(pDX, btnOK, m_btnOK);
	DDX_Control(pDX, btnMore, m_btnMore);
}

//=====================================================================

void CErrorDlg::OnBtnOK()
{
	// Set return code
	EndDialog(IDOK);
}

//=====================================================================

void CErrorDlg::OnBtnMore()
{
	// Declare variables
	CString sTemp;

	// Set expanded
	m_bExpanded = !m_bExpanded;

	// Change text
	if (m_bExpanded)
	{
		// Set new text
		sTemp.Format(_T("<< %s"), m_sMoreButton);
		m_btnMore.SetWindowText(sTemp);

		// Show splitter
		m_lnMoreSplitter.ShowWindow(TRUE);
	}
	else
	{
		// Set new text
		sTemp.Format(_T("%s >>"), m_sMoreButton);
		m_btnMore.SetWindowText(sTemp);

		// Hide splitter
		m_lnMoreSplitter.ShowWindow(FALSE);
	}

	// Auto resize controls
	AutoResizeControls();
}

//=====================================================================

void CErrorDlg::AutoResizeControls()
{
	// Declare variables
	CString sWindowText;
	CRect rectExplanation, rectMore, rectOK, rectMoreButton, rectDialog, rectSplitter;
	int iButtonHeight;
	CDC * pExplanationDC = m_lblExplanation.GetDC();
	CDC * pMoreDC = m_lblMore.GetDC();
	
	// Get window text & size
	m_lblExplanation.GetWindowText(sWindowText);
	m_lblExplanation.GetWindowRect(&rectExplanation);

	// Get size of explanation
	pExplanationDC->SelectObject(GetFont());
	pExplanationDC->DrawText(sWindowText, &rectExplanation, DT_CALCRECT | DT_WORDBREAK);

	// Resize our label
	ScreenToClient(&rectExplanation);
	m_lblExplanation.MoveWindow(&rectExplanation);	

	// Now get the positions of our controls
	ClientToScreen(&rectExplanation);
	m_btnOK.GetWindowRect(&rectOK);
	m_btnMore.GetWindowRect(&rectMoreButton);
	GetWindowRect(&rectDialog);

	// Get height of buttons
	iButtonHeight = rectMoreButton.bottom - rectMoreButton.top;

	// Calculate new positions
	rectOK.top = rectExplanation.bottom + 13;
	rectOK.bottom = rectOK.top + iButtonHeight;
	rectMoreButton.top = rectExplanation.bottom + 13;
	rectMoreButton.bottom = rectMoreButton.top + iButtonHeight;

	// Maybe we should also calculate the height of our more label
	if (m_bExpanded)
	{
		// Get window text & size
		m_lblMore.GetWindowText(sWindowText);
		m_lblMore.GetWindowRect(&rectMore);

		// Set new top of our splitter
		m_lnMoreSplitter.GetWindowRect(&rectSplitter);
		rectSplitter.top = rectOK.bottom + 13;
		rectSplitter.bottom = rectSplitter.top + 1;

		// Set new top of our label
		rectMore.top = rectSplitter.bottom + 13;
		rectMore.bottom = rectMore.top + 10;

		// Get size of explanation
		pMoreDC->SelectObject(GetFont());
		pMoreDC->DrawText(sWindowText, &rectMore, DT_CALCRECT | DT_WORDBREAK);

		// Calculate new size of dialog
		rectDialog.bottom = rectMore.bottom + 13;

		// Resize our label & splitter
		ScreenToClient(&rectMore);
		ScreenToClient(&rectSplitter);
		m_lblMore.MoveWindow(&rectMore);
		m_lnMoreSplitter.MoveWindow(&rectSplitter);
	}
	else
	{
		// Calculate new size of dialog
		rectDialog.bottom = rectOK.bottom + 13;
	}

	// Set new position of buttons
	ScreenToClient(&rectOK);
	m_btnOK.MoveWindow(&rectOK);
	ScreenToClient(&rectMoreButton);
	m_btnMore.MoveWindow(&rectMoreButton);

	// Set new position of dialog
	MoveWindow(&rectDialog);

	// Clear memory
	//delete pDC; <-- causes crash at end of application
}