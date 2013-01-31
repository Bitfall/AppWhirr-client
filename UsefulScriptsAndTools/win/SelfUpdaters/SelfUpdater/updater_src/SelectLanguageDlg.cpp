/********************************************************************
	created:	2005/09/10
	created:	10:9:2005   20:28
	filename: 	SelectLanguageDlg.cpp
	file path:	Updater
	file base:	SelectLanguageDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "SelectLanguageDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CSelectLanguageDlg, CDialog)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CSelectLanguageDlg::CSelectLanguageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectLanguageDlg::IDD, pParent)
{

}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CSelectLanguageDlg::OnInitDialog()
{
	// Declare variables
	CPath * pPath = CPath::Instance();

	// Call original function
	CDialog::OnInitDialog();

	// Load right bitmap
	m_bmpImage.LoadBitmap(IDB_SELECTLANGUAGE);

	// We have no languages yet
	m_iLanguageCount = 0;

	// Add all languages that are available in the lng folder
	ListLanguages(pPath->GetPathUpdater() + _T("\\lng\\"));

	// Now add all the languages to the combobox
	for (int i = 0; i < m_iLanguageCount; i++)
	{
		// Add language to combobox
		m_cboLanguage.AddString(m_arrLanguages[i].sDescription);
	}

	// Select english
	m_cboLanguage.SelectString(0, _T("English (original)"));

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

void CSelectLanguageDlg::OnOK()
{
	// Declare variables
	CSettings * pSettings = CSettings::Instance();
	CPath * pPath = CPath::Instance();
	CString sLanguage, sLanguageFile = _T("default");

	// Get data
	m_cboLanguage.GetWindowText(sLanguage);

	// Now, get the right file
	for (int i = 0; i < m_iLanguageCount; i++)
	{
		// Is this the right language
		if (m_arrLanguages[i].sDescription == sLanguage)
		{
			// Yes, it is
			sLanguageFile = m_arrLanguages[i].sFilename;
		}
	}

	// Set language in settings
	pSettings->SetLanguage(sLanguageFile);

	// Should we even save it to settings.ini?
	if (m_chkRemember.GetCheck() != 0)
	{
		// Save to settings.ini
		WritePrivateProfileString(_T("UPDATER"), _T("language"), sLanguageFile,
			CInternalData::Instance()->GetSettingsFile());
	}

	// Call original function
	CDialog::OnOK();
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CSelectLanguageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, cboLanguage, m_cboLanguage);
	DDX_Control(pDX, chkRemember, m_chkRemember);
	DDX_Control(pDX, bmpImage, m_bmpImage);
}

//=====================================================================

void CSelectLanguageDlg::ListLanguages(CString sLanguageFolder)
{
	// Declare variables
	WIN32_FIND_DATA wfd;
	HANDLE hFile;
	CString sFile;
	CString sSpec = sLanguageFolder + _T("*.xml");
	CString sPathFile;
	CLanguage * pLanguage = CLanguage::Instance();
	CFunctions * pFunctions = CFunctions::Instance();
	CPath * pPath = CPath::Instance();
	LanguageType oLangType;

	// Find the first file
	hFile = FindFirstFile(sSpec, &wfd);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Get file info
			sFile = wfd.cFileName;
			sPathFile = sLanguageFolder + sFile;

			// Get language info
			if (pLanguage->LoadLanguage(sPathFile, _T(""), false))
			{
				// Check if it is the right version
				if (pFunctions->CompareVersions(pLanguage->GetLanguageVersion(), VERSION_LANGUAGE) != COMPAREVERSION_FIRSTSMALLER)
				{
					// Add to list of languages
					oLangType.sFilename = pPath->StripFileExtension(pPath->ExtractFileName(sPathFile));
					oLangType.sDescription = pLanguage->GetLanguageName();
					m_arrLanguages[m_iLanguageCount++] = oLangType;
				}
			}
		}
		while (FindNextFile(hFile, &wfd));

		// Close handle to file
		FindClose(hFile);
	}

	// Release last language
	pLanguage->ReleaseLanguage();
}
