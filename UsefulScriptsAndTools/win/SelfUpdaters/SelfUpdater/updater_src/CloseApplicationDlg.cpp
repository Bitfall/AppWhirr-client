/********************************************************************
	created:	2005/08/06
	created:	6:8:2005   10:24
	filename: 	CloseApplicationDlg.cpp
	file path:	Updater
	file base:	CloseApplicationDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "RiskFunctions.h"
#include "CloseApplicationDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CCloseApplicationDlg, CDialog)
	ON_WM_TIMER()
	ON_COMMAND(btnCancel, OnBtnCancel)
	ON_COMMAND(btnEndNow, OnBtnEndNow)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CCloseApplicationDlg::CCloseApplicationDlg(CString sApplicationName,
	CString sApplicationPath)
	: CDialog(CCloseApplicationDlg::IDD, NULL)
{
	// Set data
	m_sApplicationTitle = sApplicationName.IsEmpty() ? CLanguage::Instance()->GetString(IDS_CLOSEAPPLICATION_APPLICATION)
		: sApplicationName;
	m_sApplicationPath = sApplicationPath;
	m_iCountDown = CLOSEAPPLICATION_TIME;

	// Extract icon
	if (PathFileExists(sApplicationPath))
	{
		m_hIcon = ExtractIcon(NULL, sApplicationPath, 0);
	}
	else
	{
		m_hIcon = AfxGetApp()->LoadIcon(IDI_UPDATER);
	}
}

//=====================================================================

CCloseApplicationDlg::~CCloseApplicationDlg()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CCloseApplicationDlg::OnInitDialog()
{
	// Declare variables
	CString sTemp;
	CLanguage * pLanguage = CLanguage::Instance();

	// Call original function
	CDialog::OnInitDialog();

	// Set icon
	m_icoApplication.SetSize(40, 40);
	m_icoApplication.SetTransparentColor(RGB(0, 0, 0));
	m_icoApplication.LoadIcon(m_hIcon);

	// Set window text
	sTemp.Format(pLanguage->GetString(IDS_CLOSEAPPLICATION_TITLE), m_sApplicationTitle);
	SetWindowText(sTemp);
	m_lblTitle.SetWindowText(sTemp);

	// Set explanation text
	sTemp.Format(pLanguage->GetString(IDS_CLOSEAPPLICATION_EXPLANATION), m_sApplicationTitle,
		m_iCountDown, m_sApplicationTitle);
	m_lblExplanation.SetWindowText(sTemp);

	// Set button text
	m_btnCancel.SetWindowText(pLanguage->GetString(IDS_GENERAL_CANCEL));
	m_btnEndNow.SetWindowText(pLanguage->GetString(IDS_CLOSEAPPLICATION_ENDNOW));

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

	// Set control properties
	m_prgEnd.SetRange32(0, CLOSEAPPLICATION_TIME);
	m_prgEnd.SetPos(0);

	// Start timer
	SetTimer(TIMER_COUNTDOWN, 1000, NULL);

	// Try to close application in a nice way
	std::string strVersion;
	UINT uWinPlatform;
	UINT uVersion = getWinVersion(strVersion, uWinPlatform);
	CString sProcess = CPath::Instance()->ExtractFileName(m_sApplicationPath);
	if ((uVersion == WINDOWS_NT3) || (uVersion == WINDOWS_NT4))
	{
		CloseNTApplication(sProcess.GetBuffer(sProcess.GetLength()));
	}
	else
	{
		CloseAllWinApplication(sProcess.GetBuffer(sProcess.GetLength()));
	}

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CCloseApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, lblTitle, m_lblTitle);
	DDX_Control(pDX, icoApplication, m_icoApplication);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
	DDX_Control(pDX, prgEnd, m_prgEnd);
	DDX_Control(pDX, btnCancel, m_btnCancel);
	DDX_Control(pDX, btnEndNow, m_btnEndNow);
}

//=====================================================================

void CCloseApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// If user is closing the dialog
	if (nID == 61536)
	{
		// Press cancel
		OnBtnCancel();
	}

	// Call original function
	CDialog::OnSysCommand(nID, lParam);
}

//=====================================================================

void CCloseApplicationDlg::OnTimer(UINT nIDEvent)
{
	// Declare variables
	CString sTemp;
	bool bRunning = false;

	// Did we receive the right timer event?
	if (nIDEvent == TIMER_COUNTDOWN)
	{
		// Check if the application is still running
		std::string strVersion;
		UINT uWinPlatform;
		UINT uVersion = getWinVersion(strVersion, uWinPlatform);
		CString sProcess = CPath::Instance()->ExtractFileName(m_sApplicationPath);
		if ((uVersion == WINDOWS_NT3) || (uVersion == WINDOWS_NT4))
		{
			bRunning = NTApplicationRunning(sProcess.GetBuffer(sProcess.GetLength()));
		}
		else
		{
			bRunning = AllWinApplicationRunning(sProcess.GetBuffer(sProcess.GetLength()));
		}

		// Is the application running?
		if (bRunning == false)
		{
			// No, we can close the dialog
			EndDialog(IDOK);
		}

		// Lower the timer
		m_iCountDown--;

		// Set up text
		if (m_iCountDown > 0)
		{
			// Set explanation text
			sTemp.Format(CLanguage::Instance()->GetString(IDS_CLOSEAPPLICATION_EXPLANATION), m_sApplicationTitle,
				m_iCountDown, m_sApplicationTitle);
			m_lblExplanation.SetWindowText(sTemp);

			// Update progressbar
			m_prgEnd.SetPos(m_prgEnd.GetPos() + 1);
		}
		else
		{
			// Kill timer
			KillTimer(TIMER_COUNTDOWN);

			// Force application to close
			OnBtnEndNow();
		}
	}
}

//=====================================================================

void CCloseApplicationDlg::OnBtnCancel()
{
	// Set return code
	EndDialog(IDCANCEL);
}

//=====================================================================

void CCloseApplicationDlg::OnBtnEndNow()
{
	// OK, kill the process
	std::string strVersion;
	UINT uWinPlatform;
	UINT uVersion = getWinVersion(strVersion, uWinPlatform);
	CString sProcess = CPath::Instance()->ExtractFileName(m_sApplicationPath);
	if ((uVersion == WINDOWS_NT3) || (uVersion == WINDOWS_NT4))
	{
		CloseNTApplication(sProcess.GetBuffer(sProcess.GetLength()), true);
	}
	else
	{
		CloseAllWinApplication(sProcess.GetBuffer(sProcess.GetLength()), true);
	}

	// Set return code
	EndDialog(IDOK);
}

//=====================================================================

void CCloseApplicationDlg::AutoResizeControls()
{
	// Declare variables
	CString sWindowText;
	CRect rectIcon, rectExplanation, rectCancel, rectEndNow, rectProgress, rectDialog;
	int iButtonHeight;
	CDC * pExplanationDC = m_lblExplanation.GetDC();
	
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
	m_btnCancel.GetWindowRect(&rectCancel);
	m_btnEndNow.GetWindowRect(&rectEndNow);
	m_prgEnd.GetWindowRect(&rectProgress);
	GetWindowRect(&rectDialog);

	// Get height of buttons
	iButtonHeight = rectCancel.bottom - rectCancel.top;

	// Calculate new positions
	rectProgress.top = rectExplanation.bottom + 13;
	rectProgress.bottom = rectProgress.top + iButtonHeight;
	rectCancel.top = rectProgress.bottom + 13;
	rectCancel.bottom = rectCancel.top + iButtonHeight;
	rectEndNow.top = rectProgress.bottom + 13;
	rectEndNow.bottom = rectEndNow.top + iButtonHeight;

	// Calculate new size of dialog
	rectDialog.bottom = rectEndNow.bottom + 13;

	// Set new position of controls
	ScreenToClient(&rectProgress);
	m_prgEnd.MoveWindow(&rectProgress);
	ScreenToClient(&rectCancel);
	m_btnCancel.MoveWindow(&rectCancel);
	ScreenToClient(&rectEndNow);
	m_btnEndNow.MoveWindow(&rectEndNow);

	// Set new position of dialog
	MoveWindow(&rectDialog);

	// Set icon position
	rectIcon.right = rectDialog.right - 15;
	rectIcon.top = rectDialog.top + 9;
	rectIcon.left = rectIcon.right - 40;
	rectIcon.bottom = rectIcon.top + 40;
	m_icoApplication.MoveWindow(&rectIcon);

	// Clear memory
	//delete pDC; <-- causes crash at end of application
}
