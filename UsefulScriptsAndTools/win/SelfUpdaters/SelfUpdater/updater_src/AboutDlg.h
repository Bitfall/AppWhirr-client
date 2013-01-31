/********************************************************************
	created:	2005/04/13
	created:	13:4:2005   12:52
	filename: 	AboutDlg.h
	file path:	Updater
	file base:	AboutDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef ABOUTDLG_H
#define ABOUTDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

// Include hyperlink label
#include "HyperlinkLabel.h"

//*********************************************************************
// CAboutDlg
//*********************************************************************

class CAboutDlg : public CDialog
{
public:
	// Constructor & destructor
	CAboutDlg(CWnd* pParent = NULL);   // standard constructor
	~CAboutDlg();

	// Functions
	BOOL OnInitDialog();

	// Dialog data
	enum { IDD = IDD_ABOUT };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnBtnOK();

	// Variables
	CStatic			m_lblDeveloper;
	CHyperlinkLabel m_lblHyperlink;
	CStatic			m_lblWebsite;
	CStatic			m_lblUpdaterVersion;
	CStatic			m_lblLanguageVersion;
	CStatic			m_lblLanguageUsed;
	CStatic			m_lblSpecialThanks;
	CEdit			m_txtUpdaterVersion;
	CEdit			m_txtLanguageVersion;
	CEdit			m_txtLanguageUsed;
	CListCtrl		m_lstThanks;
	COLORREF		m_clrHyperlinkColor;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // ABOUTDLG_H
