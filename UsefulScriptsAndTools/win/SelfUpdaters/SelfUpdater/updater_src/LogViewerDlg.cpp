/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:16
	filename: 	LogViewerDlg.cpp
	file path:	Updater
	file base:	LogViewerDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "LogViewerDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CLogViewerDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(btnOK, OnBtnOK)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CLogViewerDlg::CLogViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogViewerDlg::IDD, pParent)
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CLogViewerDlg::OnInitDialog()
{
	// Declare variables
	CPath * pPath = CPath::Instance();
	CSettings * pSettings = CSettings::Instance();
	CLanguage * pLanguage = CLanguage::Instance();
	CString sLogPath;
	CString sContent;
	CRect rectClient;
	CFile oLog;

	// Call original function
	CDialog::OnInitDialog();
	
	// Load icon
	SetIcon(::AfxGetApp()->LoadIcon(IDI_LOGVIEWER), FALSE);
	SetIcon(::AfxGetApp()->LoadIcon(IDI_LOGVIEWER), TRUE);

	// Get log path
	sLogPath = pPath->GetPathTemp() + _T("Updater log for ") + pSettings->GetAppName() + _T(".txt");

	// Set title
	SetWindowText(pLanguage->GetString(IDS_LOGVIEWER_TITLE) + _T(" - ") + sLogPath);

	// Translate some controls
	m_btnOK.SetWindowText(pLanguage->GetString(IDS_GENERAL_OK));

	// Open file and put content in the edit control
	try
	{
		// Open file
		oLog.Open(sLogPath, CFile::modeRead | CFile::shareDenyNone);

		// Get filesize
		DWORD dwSize = static_cast<DWORD>(oLog.GetLength());

		// Read data into string
		oLog.Read(sContent.GetBuffer(dwSize / sizeof(TCHAR)), dwSize);
		sContent.ReleaseBuffer();

		// Close file
		oLog.Close();
	}
	catch (...)
	{
		// Show error
		sContent.Format(pLanguage->GetString(IDS_LOGVIEWER_NOLOG), pSettings->GetAppName());
	}

	// Set content
	m_txtLog.SetWindowText(sContent);

	// Set font
	m_fntLog.DeleteObject();
	m_fntLog.CreateFont(
		15,								// nHeight
		0,								// nWidth
		0,								// nEscapement
		0,								// nOrientation
		0,								// nWeight
		FALSE,							// bItalic
		FALSE,							// bUnderline
		0,								// cStrikeOut
		DEFAULT_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,				// nOutPrecision
		CLIP_DEFAULT_PRECIS,			// nClipPrecision
		PROOF_QUALITY,					// nQuality
		DEFAULT_PITCH | FF_DONTCARE,	// nPitchAndFamily
		_T("Courier New"));
	m_txtLog.SetFont(&m_fntLog);

	// Resize once to be sure user won't see any size changes
	GetWindowRect(&rectClient);
	PostMessage(WM_SIZE, rectClient.Width(), rectClient.Height());

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CLogViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, txtLog, m_txtLog);
	DDX_Control(pDX, btnOK, m_btnOK);
}

//=====================================================================

void CLogViewerDlg::OnSize(UINT nType, int cx, int cy)
{
	// Declare variables
	CRect rectWindow;

	// Call original function
	CDialog::OnSize(nType, cx, cy);

	// Get new window rect
	GetClientRect(&rectWindow);

	// Move button to right bottom
	if (m_btnOK.GetSafeHwnd() != NULL)
	{
		m_btnOK.MoveWindow(rectWindow.right - 82, rectWindow.bottom - 33, 72, 23);
	}

	// Fill dialog with edit control
	if (m_txtLog.GetSafeHwnd() != NULL)
	{
		m_txtLog.MoveWindow(10, 10, rectWindow.right - 20, rectWindow.bottom - 53);
	}
}

//=====================================================================

void CLogViewerDlg::OnGetMinMaxInfo(MINMAXINFO * lpMMI)
{
	// Set minimum size
	lpMMI->ptMinTrackSize.x = 400;
	lpMMI->ptMinTrackSize.y = 250;
}

//=====================================================================

void CLogViewerDlg::OnBtnOK()
{
	// Close dialog
	CDialog::EndDialog(IDOK);
}
