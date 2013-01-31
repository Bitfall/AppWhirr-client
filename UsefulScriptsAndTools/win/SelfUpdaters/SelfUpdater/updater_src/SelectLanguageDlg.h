/********************************************************************
	created:	2005/09/10
	created:	10:9:2005   19:46
	filename: 	SelectLanguageDlg.h
	file path:	Updater
	file base:	SelectLanguageDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef SELECTLANGUAGEDLG_H
#define SELECTLANGUAGEDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "TransparentBitmap.h"

struct LanguageType
{
	CString sFilename;
	CString sDescription;
};

//*********************************************************************
// CSelectLanguageDlg
//*********************************************************************

class CSelectLanguageDlg : public CDialog
{
public:
	// Constructor & destructor
	CSelectLanguageDlg(CWnd* pParent = NULL);   // standard constructor

	// Functions
	BOOL OnInitDialog();
	void OnOK();

	// Dialog data
	enum { IDD = IDD_SELECTLANGUAGE };

protected:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void ListLanguages(CString sLanguageFolder);

	// Variables
	CTransparentBitmap	m_bmpImage;
	CButton				m_chkRemember;
	CComboBox			m_cboLanguage;
	LanguageType		m_arrLanguages[128];
	int					m_iLanguageCount;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // SELECTLANGUAGEDLG_H
