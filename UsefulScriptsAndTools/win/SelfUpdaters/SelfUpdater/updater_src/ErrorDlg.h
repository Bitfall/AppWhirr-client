/********************************************************************
	created:	2005/08/06
	created:	6:8:2005   10:12
	filename: 	ErrorDlg.h
	file path:	Updater
	file base:	ErrorDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef ERRORDLG_H
#define ERRORDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "TransparentStatic.h"
#include "Updater.h"

//*********************************************************************
// CErrorDlg
//*********************************************************************

class CErrorDlg : public CDialog
{
public:
	// Constructor & destructor
	CErrorDlg(CString sWindowCaption = "Error",
		CString sTitle = "Error", CString sExplanation = "",
		CString sMore = "", CString sOK = "OK", CString sMoreButton = "More");   // standard constructor
	~CErrorDlg();

	// Functions
	BOOL OnInitDialog();

	// Dialog data
	enum { IDD = IDD_ERROR };

protected:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnBtnOK();
	void OnBtnMore();
	void AutoResizeControls();

	// Variables
	CTransparentStatic m_lblTitle;
	CStatic			   m_lblExplanation;
	CStatic			   m_lblMore;
	CStatic			   m_lnMoreSplitter;
	CButton			   m_btnOK;
	CButton			   m_btnMore;
	CStatic			   m_bmpLogo;
	CFont			   m_fntTitle;
	CString			   m_sWindowCaption;
	CString			   m_sTitle;
	CString			   m_sExplanation;
	CString			   m_sMore;
	CString			   m_sOK;
	CString			   m_sMoreButton;
	bool			   m_bExpanded;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // ERRORDLG_H
