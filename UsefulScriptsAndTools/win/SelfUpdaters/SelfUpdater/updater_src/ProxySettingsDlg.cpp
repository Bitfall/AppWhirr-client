/********************************************************************
	created:	2005/12/05
	created:	5:12:2005   11:35
	filename: 	ProxySettingsDlg.cpp
	file path:	Updater
	file base:	ProxySettingsDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ProxySettingsDlg.h"
#include "AboutDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CProxySettingsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_COMMAND(btnOK, OnBtnOK)
	ON_COMMAND(btnCancel,OnBtnCancel)
	ON_COMMAND(rdDirectConnection, OnSelectDirectConnection)
	ON_COMMAND(rdAutoDetect, OnSelectAutoDetect)
	ON_COMMAND(rdManual, OnSelectManual)
	ON_COMMAND(rdAutoConfigURL, OnSelectAutoConfigURL)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CProxySettingsDlg::CProxySettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProxySettingsDlg::IDD, pParent)
{

}

//=====================================================================

CProxySettingsDlg::~CProxySettingsDlg()
{
	// Release bitmap objects
	m_bmpMenuLogo.DeleteObject();
	m_bmpMenuLogoNegative.DeleteObject();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

void CProxySettingsDlg::OnBtnOK()
{
	// Declare variables
	CString sProxyType = PROXY_TYPE_AUTODETECT;
	CString sHttp, sFtp, sAutoConfigURL;
	CSettings * pSettings = CSettings::Instance();

	// Get selected type
	if (m_rdDirectConnection.GetCheck() != 0)
		sProxyType = PROXY_TYPE_DIRECTCONNECTION;

	if (m_rdAutoDetect.GetCheck() != 0)
		sProxyType = PROXY_TYPE_AUTODETECT;

	if (m_rdManual.GetCheck() != 0)
		sProxyType = PROXY_TYPE_MANUAL;

	if (m_rdAutoConfigURL.GetCheck() != 0)
		sProxyType = PROXY_TYPE_AUTOCONFIGURL;

	// Get manual settings
	m_txtHttpProxy.GetWindowText(sHttp);
	m_txtFtpProxy.GetWindowText(sFtp);
	
	// Get auto config url
	m_txtAutoConfigURL.GetWindowText(sAutoConfigURL);

	// Store all settings
	pSettings->SetProxyType(sProxyType);
	pSettings->SetProxyManualHttp(sHttp);
	pSettings->SetProxyManualFtp(sFtp);
	pSettings->SetProxyAutoConfigURL(sAutoConfigURL);

	// Save settings
	pSettings->SaveFile();

	// Close dialog
	EndDialog(IDOK);
}

//=====================================================================

void CProxySettingsDlg::OnBtnCancel()
{
	// Close dialog
	EndDialog(IDCANCEL);
}

//=====================================================================

void CProxySettingsDlg::OnSelectDirectConnection()
{
	// Disable all other controls
	EnableManualProxy(FALSE);
	EnableAutoConfigURL(FALSE);

	// Select the right control
	m_rdDirectConnection.SetCheck(1);
	m_rdAutoDetect.SetCheck(0);
	m_rdManual.SetCheck(0);
	m_rdAutoConfigURL.SetCheck(0);
}

//=====================================================================

void CProxySettingsDlg::OnSelectAutoDetect()
{
	// Disable all other controls
	EnableManualProxy(FALSE);
	EnableAutoConfigURL(FALSE);

	// Select the right control
	m_rdDirectConnection.SetCheck(0);
	m_rdAutoDetect.SetCheck(1);
	m_rdManual.SetCheck(0);
	m_rdAutoConfigURL.SetCheck(0);
}

//=====================================================================

void CProxySettingsDlg::OnSelectManual()
{
	// Disable all other controls
	EnableAutoConfigURL(FALSE);

	// Enable manual proxy
	EnableManualProxy(TRUE);

	// Select the right control
	m_rdDirectConnection.SetCheck(0);
	m_rdAutoDetect.SetCheck(0);
	m_rdManual.SetCheck(1);
	m_rdAutoConfigURL.SetCheck(0);
}

//=====================================================================

void CProxySettingsDlg::OnSelectAutoConfigURL()
{
	// Disable all other controls
	EnableManualProxy(FALSE);

	// Enable auto config url proxy
	EnableAutoConfigURL(TRUE);	

	// Select the right control
	m_rdDirectConnection.SetCheck(0);
	m_rdAutoDetect.SetCheck(0);
	m_rdManual.SetCheck(0);
	m_rdAutoConfigURL.SetCheck(1);
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CProxySettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, rdDirectConnection, m_rdDirectConnection);
	DDX_Control(pDX, rdAutoDetect, m_rdAutoDetect);
	DDX_Control(pDX, rdManual, m_rdManual);
	DDX_Control(pDX, rdAutoConfigURL, m_rdAutoConfigURL);
	DDX_Control(pDX, btnOK, m_btnOK);
	DDX_Control(pDX, btnCancel, m_btnCancel);
	DDX_Control(pDX, lblHttpProxy, m_lblHttpProxy);
	DDX_Control(pDX, lblFtpProxy, m_lblFtpProxy);
	DDX_Control(pDX, txtFtpProxy, m_txtFtpProxy);
	DDX_Control(pDX, txtHttpProxy, m_txtHttpProxy);
	DDX_Control(pDX, txtAutoConfigURL, m_txtAutoConfigURL);
}

//=====================================================================

BOOL CProxySettingsDlg::OnInitDialog()
{
	// Call original function
	CDialog::OnInitDialog();

	// Declare variables
	CLanguage * pLanguage = CLanguage::Instance();
	CSettings * pSettings = CSettings::Instance();
	bool bSelected = false;

	// Edit system menu
	CMenu * pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		// Add about
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, pLanguage->GetString(IDS_ABOUT_TITLE) + "...");

		// Add bitmap to menu
		m_bmpMenuLogo.LoadBitmap(IDB_UPDATER);
		m_bmpMenuLogoNegative.LoadBitmap(IDB_UPDATER_NEG);
		pSysMenu->SetMenuItemBitmaps(IDM_ABOUTBOX, MF_BYCOMMAND, &m_bmpMenuLogo, &m_bmpMenuLogoNegative);

		// Delete maximize
		pSysMenu->DeleteMenu(SC_MAXIMIZE, MF_BYCOMMAND);

		// Delete minimize
		pSysMenu->DeleteMenu(SC_MINIMIZE, MF_BYCOMMAND);

		// Delete change size
		pSysMenu->DeleteMenu(SC_SIZE, MF_BYCOMMAND);
	}

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

	// Load language specific strings
	SetWindowText(pLanguage->GetString(IDS_PROXY_TITLE));
	m_rdDirectConnection.SetWindowText(pLanguage->GetString(IDS_PROXY_DIRECT));
	m_rdAutoDetect.SetWindowText(pLanguage->GetString(IDS_PROXY_AUTODETECT));
	m_rdManual.SetWindowText(pLanguage->GetString(IDS_PROXY_MANUAL));
	m_rdAutoConfigURL.SetWindowText(pLanguage->GetString(IDS_PROXY_AUTOCONFIGURL));
	m_btnOK.SetWindowText(pLanguage->GetString(IDS_GENERAL_OK));
	m_btnCancel.SetWindowText(pLanguage->GetString(IDS_GENERAL_CANCEL));
	m_lblHttpProxy.SetWindowText(pLanguage->GetString(IDS_PROXY_HTTP));
	m_lblFtpProxy.SetWindowText(pLanguage->GetString(IDS_PROXY_FTP));

	// Select right settings
	if (pSettings->GetProxyType() == PROXY_TYPE_DIRECTCONNECTION)
	{
		OnSelectDirectConnection();
		bSelected = true;
	}

	if (pSettings->GetProxyType() == PROXY_TYPE_AUTODETECT)
	{
		OnSelectAutoDetect();
		bSelected = true;
	}

	if (pSettings->GetProxyType() == PROXY_TYPE_MANUAL)
	{
		OnSelectManual();
		bSelected = true;
	}

	if (pSettings->GetProxyType() == PROXY_TYPE_AUTOCONFIGURL)
	{
		OnSelectAutoConfigURL();
		bSelected = true;
	}

	// If we did not select anything, select auto-detect
	if (!bSelected)
	{
		OnSelectAutoDetect();
	}

	// Manual settings
	m_txtHttpProxy.SetWindowText(pSettings->GetProxyManualHttp());
	m_txtFtpProxy.SetWindowText(pSettings->GetProxyManualFtp());

	// Auto config URL
	m_txtAutoConfigURL.SetWindowText(pSettings->GetProxyAutoConfigURL());
	
	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

void CProxySettingsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// If user wants to see about box
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		// Show about dialog
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}

	// Call original function
	CDialog::OnSysCommand(nID, lParam);
}

//=====================================================================

void CProxySettingsDlg::EnableManualProxy(BOOL bEnable)
{
	// Enable/disable right controls
	m_txtHttpProxy.EnableWindow(bEnable);
	m_txtFtpProxy.EnableWindow(bEnable);
}

//=====================================================================

void CProxySettingsDlg::EnableAutoConfigURL(BOOL bEnable)
{
	// Enable/disable right controls
	m_txtAutoConfigURL.EnableWindow(bEnable);
}