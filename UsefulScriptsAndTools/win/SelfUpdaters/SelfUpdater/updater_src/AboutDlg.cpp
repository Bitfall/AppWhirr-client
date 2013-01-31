/********************************************************************
	created:	2005/04/13
	created:	13:4:2005   13:05
	filename: 	AboutDlg.cpp
	file path:	Updater
	file base:	AboutDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "AboutDlg.h"
#include "FileVersion.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_COMMAND(btnOK, OnBtnOK)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
}

//=====================================================================

CAboutDlg::~CAboutDlg()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CAboutDlg::OnInitDialog()
{
	// Call original function
	CDialog::OnInitDialog();

	// Declare variables;
	int iItem;
	CString sTemp;
	CFileVersion fileversion;
	CLanguage * pLanguage = CLanguage::Instance();
	CFunctions * pFunctions = CFunctions::Instance();
	CSettings * pSettings = CSettings::Instance();
	CPath * pPath = CPath::Instance();

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
	SetWindowText(pLanguage->GetString(IDS_ABOUT_TITLE));

	// Set versions
	fileversion.Open(pPath->GetPathUpdaterExecutable());
	sTemp = fileversion.GetFileVersion();

#ifdef _DEBUG
	sTemp += _T(" (DEBUG VERSION)");
#endif

#ifdef _BETA
	sTemp += _T(" (BETA VERSION)");
#endif

	m_txtUpdaterVersion.SetWindowText(sTemp);
	m_txtLanguageVersion.SetWindowText(VERSION_LANGUAGE);
	fileversion.Close();

	// Set label text
	sTemp.Format(_T("%s %s, %s."), pLanguage->GetString(IDS_ABOUT_DEVELOPEDBY), UPDATER_DEVELOPER, UPDATER_YEARS);
	m_lblDeveloper.SetWindowText(sTemp);
	m_lblHyperlink.SetWindowText(UPDATER_WEBADDRESS);
	m_lblHyperlink.SetHyperlink(UPDATER_WEBADDRESS);
	m_lblHyperlink.SetColorLink(pSettings->GetLinkColorLink());
	m_lblHyperlink.SetColorHover(pSettings->GetLinkColorHover());
	m_lblWebsite.SetWindowText(pLanguage->GetString(IDS_ABOUT_WEBSITE));
	m_lblUpdaterVersion.SetWindowText(pLanguage->GetString(IDS_ABOUT_UPDATERVERSION));
	m_lblLanguageVersion.SetWindowText(pLanguage->GetString(IDS_ABOUT_LANGUAGEVERSION));
	m_lblLanguageUsed.SetWindowText(pLanguage->GetString(IDS_ABOUT_LANGUAGEUSED));
	m_lblSpecialThanks.SetWindowText(pLanguage->GetString(IDS_ABOUT_SPECIALTHANKS));

	// Set up language name + translator
	sTemp.Format(_T("%s, %s %s"), pLanguage->GetLanguageName(), pLanguage->GetString(IDS_ABOUT_TRANSLATEDBY),
		pLanguage->GetLanguageTranslator());
	m_txtLanguageUsed.SetWindowText(sTemp);
	
	// Fill thanks list box
	m_lstThanks.InsertColumn(0, pLanguage->GetString(IDS_ABOUT_THANKS_NAME), LVCFMT_LEFT, 84);
	m_lstThanks.InsertColumn(1, pLanguage->GetString(IDS_ABOUT_THANKS_REASON), LVCFMT_LEFT, 180);

	// Add all names
	iItem = m_lstThanks.InsertItem(0, _T("Neville Franks"));
	m_lstThanks.SetItemText(iItem, 1, _T("Checking for internet connection"));
	iItem = m_lstThanks.InsertItem(0, _T("Chris Maunders"));
	m_lstThanks.SetItemText(iItem, 1, _T("CTrayicon class"));
	iItem = m_lstThanks.InsertItem(0, _T("Manuel Laflamme"));
	m_lstThanks.SetItemText(iItem, 1, _T("CFileVersion class"));
	iItem = m_lstThanks.InsertItem(0, _T("Shane Martin"));
	m_lstThanks.SetItemText(iItem, 1, _T("CRegistry class"));
	iItem = m_lstThanks.InsertItem(0, _T("Daniel Godson"));
	m_lstThanks.SetItemText(iItem, 1, _T("Unzip support"));
	iItem = m_lstThanks.InsertItem(0, _T("First Objective Software, Inc."));
	m_lstThanks.SetItemText(iItem, 1, _T("CMarkUp class"));
	iItem = m_lstThanks.InsertItem(0, _T("Sven Schaefer"));
	m_lstThanks.SetItemText(iItem, 1, _T("Testing & useful suggestions"));
	iItem = m_lstThanks.InsertItem(0, _T("Wout"));
	m_lstThanks.SetItemText(iItem, 1, _T("Testing & useful suggestions"));
	iItem = m_lstThanks.InsertItem(0, _T("Eric Dauth"));
	m_lstThanks.SetItemText(iItem, 1, _T("Testing & useful suggestions"));
	iItem = m_lstThanks.InsertItem(0, _T("Jeff Tomich"));
	m_lstThanks.SetItemText(iItem, 1, _T("Testing & useful suggestions"));
	iItem = m_lstThanks.InsertItem(0, _T("François Le Luhern"));
	m_lstThanks.SetItemText(iItem, 1, _T("Testing & useful suggestions"));
	iItem = m_lstThanks.InsertItem(0, _T("Hans Dietrich"));
	m_lstThanks.SetItemText(iItem, 1, _T("XHTMLStatic class"));
	iItem = m_lstThanks.InsertItem(0, _T("Vijay Balani"));
	m_lstThanks.SetItemText(iItem, 1, _T("Testing & code fixes / expansions"));
	iItem = m_lstThanks.InsertItem(0, _T("Andrzej Markowski"));
	m_lstThanks.SetItemText(iItem, 1, _T("CustomBitmapButton class"));
	iItem = m_lstThanks.InsertItem(0, _T("George Anescu"));
	m_lstThanks.SetItemText(iItem, 1, _T("Rijndael encryption class"));

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, lblDeveloper, m_lblDeveloper);
	DDX_Control(pDX, lblHyperlink, m_lblHyperlink);
	DDX_Control(pDX, lblWebsite, m_lblWebsite);
	DDX_Control(pDX, lblUpdaterVersion, m_lblUpdaterVersion);
	DDX_Control(pDX, lblLanguageVersion, m_lblLanguageVersion);
	DDX_Control(pDX, lblLanguageUsed, m_lblLanguageUsed);
	DDX_Control(pDX, lblSpecialThanks, m_lblSpecialThanks);
	DDX_Control(pDX, txtUpdaterVersion, m_txtUpdaterVersion);
	DDX_Control(pDX, txtLanguageVersion, m_txtLanguageVersion);
	DDX_Control(pDX, txtLanguageUsed, m_txtLanguageUsed);
	DDX_Control(pDX, lstThanks, m_lstThanks);
}

//=====================================================================

void CAboutDlg::OnBtnOK()
{
	// Close dialog
	CAboutDlg::OnOK();
}
