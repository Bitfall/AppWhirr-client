/********************************************************************
	created:	2005/07/06
	created:	6:7:2005   10:42
	filename: 	AuthenticationDlg.cpp
	file path:	Updater
	file base:	AuthenticationDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"
#include "AuthenticationDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CAuthenticationDlg, CDialog)
	ON_BN_CLICKED(btnOK, OnBtnOK)
	ON_BN_CLICKED(btnCancel, OnBtnCancel)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CAuthenticationDlg::CAuthenticationDlg(CWnd* pParent /* = NULL */)
	: CDialog(CAuthenticationDlg::IDD, pParent)
{

}

//=====================================================================

CAuthenticationDlg::~CAuthenticationDlg()
{

}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CAuthenticationDlg::OnInitDialog()
{
	// Call original function
	CDialog::OnInitDialog();

	// Declare variables;
	CString sTemp;
	CLanguage * pLanguage = CLanguage::Instance();
	CFunctions * pFunctions = CFunctions::Instance();
	CSettings * pSettings = CSettings::Instance();

	// Resize dialog so it fits nice to the image
	CRect rectClient;
	GetWindowRect(&rectClient);
	rectClient.right--;
	MoveWindow(rectClient);

	// Load icon
	if (PathFileExists(pSettings->GetIcon()))
	{
		// Custom user icon
		HICON tmpIcon = (HICON)LoadImage(NULL, pSettings->GetIcon(), IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);

		// Check if we loaded successfully
		if (tmpIcon)
		{
			SetIcon(tmpIcon, TRUE);
		}
		else
		{
			// Default Updater icon
			SetIcon(AfxGetApp()->LoadIcon(IDI_UPDATER), TRUE);
		}
	}
	else
	{
		// Default Updater icon
		SetIcon(AfxGetApp()->LoadIcon(IDI_UPDATER), TRUE);
	}

	// Set title
	SetWindowText(pLanguage->GetString(IDS_AUTHENTICATION_TITLE));

	// Set label text
	m_lblTitle.SetWindowText(pLanguage->GetString(IDS_AUTHENTICATION_TITLE));
	m_lblExplanation.SetWindowText(pLanguage->GetString(IDS_AUTHENTICATION_EXPLANATION));
	m_lblUsername.SetWindowText(pLanguage->GetString(IDS_AUTHENTICATION_USERNAME));
	m_lblPassword.SetWindowText(pLanguage->GetString(IDS_AUTHENTICATION_PASSWORD));
	m_chkSaveCredentials.SetWindowText(pLanguage->GetString(IDS_AUTHENTICATION_SAVECREDENTIALS));
	m_btnOK.SetWindowText(pLanguage->GetString(IDS_GENERAL_OK));
	m_btnCancel.SetWindowText(pLanguage->GetString(IDS_GENERAL_CANCEL));

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

	// Don't check the save credentials box by default
	m_chkSaveCredentials.SetCheck(0);

	// Move controls
	AutoResizeControls();

	// Suggest a username
	sTemp = GetActiveUser();
	if (!sTemp.IsEmpty())
	{
		// Set username
		m_txtUsername.SetWindowText(sTemp);

		// Set focus to password field
		m_txtPassword.SetFocus();

		// We should return FALSE
		return FALSE;
	}

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CAuthenticationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, bmpLogo, m_bmpLogo);
	DDX_Control(pDX, lblTitle, m_lblTitle);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
	DDX_Control(pDX, lblUsername, m_lblUsername);
	DDX_Control(pDX, lblPassword, m_lblPassword);
	DDX_Control(pDX, txtUsername, m_txtUsername);
	DDX_Control(pDX, txtPassword, m_txtPassword);
	DDX_Control(pDX, chkSaveCredentials, m_chkSaveCredentials);
	DDX_Control(pDX, btnOK, m_btnOK);
	DDX_Control(pDX, btnCancel, m_btnCancel);
}

//=====================================================================

void CAuthenticationDlg::OnBtnOK()
{
	// Declare variables
	CString sSettingsFile, sUsername, sPassword;
	CSettings * pSettings = CSettings::Instance();

	// Store username and password in settings
	m_txtUsername.GetWindowText(sUsername);
	pSettings->SetProxyUsername(sUsername);
	m_txtPassword.GetWindowText(sPassword);
	pSettings->SetProxyPassword(sPassword);

	// Should we also store the username and password in the settings.ini?
	if (m_chkSaveCredentials.GetCheck() != 0)
	{
		// Save to settings.ini
		sSettingsFile = CPath::Instance()->GetPathUpdater() + _T("\\") + FILENAME_SETTINGS;
		WritePrivateProfileString(_T("PROXY"), _T("username"), sUsername, sSettingsFile);
		WritePrivateProfileString(_T("PROXY"), _T("password"), sPassword, sSettingsFile);
	}

	// Close dialog
	CDialog::OnOK();
}

//=====================================================================

void CAuthenticationDlg::OnBtnCancel()
{
	// Close dialog
	CDialog::OnCancel();
}

//=====================================================================

CString CAuthenticationDlg::GetActiveUser()
{
	// Declare variables
	CString sUsername = _T(""), sDomainName = _T(""), sBuffer, sResult = _T("");
	PSID pSid = NULL;
	BYTE bySidBuffer[256];
	DWORD dwSidSize = sizeof(bySidBuffer), dwDomainNameSize = 1024, dwUsernameSize = 1024;
	SID_NAME_USE sidType;   

	// Initialize variables
	pSid = (PSID)bySidBuffer;
	dwSidSize = sizeof(bySidBuffer);

	// Get current username
	GetUserName(sBuffer.GetBuffer(dwUsernameSize), &dwUsernameSize);
	sUsername = sBuffer.GetBuffer(dwUsernameSize);
	sBuffer.ReleaseBuffer();

	// Get the domain name
	LookupAccountName(NULL, sUsername, (PSID) pSid, &dwSidSize,
		sBuffer.GetBuffer(dwDomainNameSize), &dwDomainNameSize,	(PSID_NAME_USE)&sidType);
	sDomainName = sBuffer.GetBuffer(dwDomainNameSize);
	sBuffer.ReleaseBuffer();

	// Return right result
	if (!sDomainName.IsEmpty())
		sResult = sDomainName + _T("\\");
	sResult += sUsername;

	return sResult;
}

//=====================================================================

void CAuthenticationDlg::AutoResizeControls()
{
	// Declare variables
	CString sWindowText;
	CRect rectExplanation, rectTemp;
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

	// Since the height of the description rectangle is 10 in design, remove it here
	rectExplanation.bottom -= 10;
	//ClientToScreen(&rectExplanation);

	// Resize dialog
	GetWindowRect(&rectTemp);
	rectTemp.bottom += rectExplanation.Height();
	MoveWindow(&rectTemp);

	// Move buttons
	m_btnOK.GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	rectTemp.top += rectExplanation.Height();
	rectTemp.bottom += rectExplanation.Height();
	m_btnOK.MoveWindow(&rectTemp);

	m_btnCancel.GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	rectTemp.top += rectExplanation.Height();
	rectTemp.bottom += rectExplanation.Height();
	m_btnCancel.MoveWindow(&rectTemp);

	// Move check box
	m_chkSaveCredentials.GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	rectTemp.top += rectExplanation.Height();
	rectTemp.bottom += rectExplanation.Height();
	m_chkSaveCredentials.MoveWindow(&rectTemp);

	// Move text boxes
	m_txtUsername.GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	rectTemp.top += rectExplanation.Height();
	rectTemp.bottom += rectExplanation.Height();
	m_txtUsername.MoveWindow(&rectTemp);

	m_txtPassword.GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	rectTemp.top += rectExplanation.Height();
	rectTemp.bottom += rectExplanation.Height();
	m_txtPassword.MoveWindow(&rectTemp);

	// Move labels
	m_lblUsername.GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	rectTemp.top += rectExplanation.Height();
	rectTemp.bottom += rectExplanation.Height();
	m_lblUsername.MoveWindow(&rectTemp);

	m_lblPassword.GetWindowRect(&rectTemp);
	ScreenToClient(&rectTemp);
	rectTemp.top += rectExplanation.Height();
	rectTemp.bottom += rectExplanation.Height();
	m_lblPassword.MoveWindow(&rectTemp);

	// Clear memory
	//delete pDC; <-- causes crash at end of application
}