/********************************************************************
	created:	2005/08/06
	created:	6:8:2005   10:24
	filename: 	InformationDlg.cpp
	file path:	Updater
	file base:	InformationDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "InformationDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CInformationDlg, CDialog)
	ON_COMMAND(btnOK, OnBtnOK)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CInformationDlg::CInformationDlg(CString sWindowCaption /* = "Information" */,
								   CString sTitle /* = "Information" */,
								   CString sExplanation /* = "" */, CString sOK /* = "OK" */)
	: CDialog(CInformationDlg::IDD, NULL)
{
	// Set data
	m_sWindowCaption = sWindowCaption;
	m_sTitle = sTitle;
	m_sExplanation = sExplanation;
	m_sOK = sOK;
}

//=====================================================================

CInformationDlg::~CInformationDlg()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CInformationDlg::OnInitDialog()
{
	// Call original function
	CDialog::OnInitDialog();

	// Set window text
	SetWindowText(m_sWindowCaption);
	m_lblTitle.SetWindowText(m_sTitle);
	m_lblExplanation.SetWindowText(m_sExplanation);
	m_btnOK.SetWindowText(m_sOK);

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

void CInformationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, bmpLogo, m_bmpLogo);
	DDX_Control(pDX, lblTitle, m_lblTitle);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
	DDX_Control(pDX, btnOK, m_btnOK);
}

//=====================================================================

void CInformationDlg::OnBtnOK()
{
	// Set return code
	EndDialog(IDOK);
}

//=====================================================================

void CInformationDlg::AutoResizeControls()
{
	// Declare variables
	CString sWindowText;
	CRect rectLabel, rectOK, rectDialog;
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
	m_btnOK.GetWindowRect(&rectOK);
	GetWindowRect(&rectDialog);

	// Get height of buttons
	iButtonHeight = rectOK.bottom - rectOK.top;

	// Calculate new positions
	rectOK.top = rectLabel.bottom + 13;
	rectOK.bottom = rectOK.top + iButtonHeight;
	rectDialog.bottom = rectOK.bottom + 13;

	// Set new position of buttons
	ScreenToClient(&rectOK);
	m_btnOK.MoveWindow(&rectOK);

	// Set new position of dialog
	MoveWindow(&rectDialog);
}