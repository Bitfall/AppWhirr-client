/********************************************************************
	created:	2005/08/06
	created:	6:8:2005   10:24
	filename: 	ConfirmationDlg.cpp
	file path:	Updater
	file base:	ConfirmationDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "ConfirmationDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CConfirmationDlg, CDialog)
	ON_COMMAND(btnYes, OnBtnYes)
	ON_COMMAND(btnNo, OnBtnNo)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CConfirmationDlg::CConfirmationDlg(CString sWindowCaption /* = "Confirmation required" */,
								   CString sTitle /* = "Confirmation required" */,
								   CString sExplanation /* = "" */, CString sYes /* = "Yes" */,
								   CString sNo /* = "No" */)
	: CDialog(CConfirmationDlg::IDD, NULL)
{
	// Set data
	m_sWindowCaption = sWindowCaption;
	m_sTitle = sTitle;
	m_sExplanation = sExplanation;
	m_sYes = sYes;
	m_sNo = sNo;
}

//=====================================================================

CConfirmationDlg::~CConfirmationDlg()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CConfirmationDlg::OnInitDialog()
{
	// Call original function
	CDialog::OnInitDialog();

	// Set window text
	SetWindowText(m_sWindowCaption);
	m_lblTitle.SetWindowText(m_sTitle);
	m_lblExplanation.SetWindowText(m_sExplanation);
	m_btnYes.SetWindowText(m_sYes);
	m_btnNo.SetWindowText(m_sNo);

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

	// Auto resize controls
	AutoResizeControls();

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CConfirmationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, bmpLogo, m_bmpLogo);
	DDX_Control(pDX, lblTitle, m_lblTitle);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
	DDX_Control(pDX, btnYes, m_btnYes);
	DDX_Control(pDX, btnNo, m_btnNo);
}

//=====================================================================

void CConfirmationDlg::OnBtnYes()
{
	// Set return code
	EndDialog(IDYES);
}

//=====================================================================

void CConfirmationDlg::OnBtnNo()
{
	// Set return code
	EndDialog(IDNO);
}

//=====================================================================

void CConfirmationDlg::AutoResizeControls()
{
	// Declare variables
	CString sWindowText;
	CRect rectLabel, rectYes, rectNo, rectDialog;
	int iButtonHeight;
	CDC * pDC = m_lblExplanation.GetDC();

	// Get window text & size
	m_lblExplanation.GetWindowText(sWindowText);
	m_lblExplanation.GetWindowRect(&rectLabel);

	// Get size of explanation
	pDC->SelectObject(GetFont());
	pDC->DrawText(sWindowText, &rectLabel, DT_CALCRECT | DT_WORDBREAK);

	// Clear memory
	//delete pDC; <-- causes crash at end of application

	// Resize our label
	ScreenToClient(&rectLabel);
	m_lblExplanation.MoveWindow(&rectLabel);	

	// Now get the positions of our controls
	ClientToScreen(&rectLabel);
	m_btnYes.GetWindowRect(&rectYes);
	m_btnNo.GetWindowRect(&rectNo);
	GetWindowRect(&rectDialog);

	// Get height of buttons
	iButtonHeight = rectNo.bottom - rectNo.top;

	// Calculate new positions
	rectYes.top = rectLabel.bottom + 13;
	rectYes.bottom = rectYes.top + iButtonHeight;
	rectNo.top = rectLabel.bottom + 13;
	rectNo.bottom = rectNo.top + iButtonHeight;
	rectDialog.bottom = rectYes.bottom + 13;

	// Set new position of buttons
	ScreenToClient(&rectYes);
	m_btnYes.MoveWindow(&rectYes);
	ScreenToClient(&rectNo);
	m_btnNo.MoveWindow(&rectNo);

	// Set new position of dialog
	MoveWindow(&rectDialog);
}