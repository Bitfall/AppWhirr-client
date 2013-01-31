/********************************************************************
	created:	2005/08/06
	created:	6:8:2005   10:12
	filename: 	WarningDlg.h
	file path:	Updater
	file base:	WarningDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef WARNINGDLG_H
#define WARNINGDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "TransparentStatic.h"
#include "Updater.h"

//*********************************************************************
// CWarningDlg
//*********************************************************************

class CWarningDlg : public CDialog
{
public:
	// Constructor & destructor
	CWarningDlg(CString sWindowCaption = "Warning",
		CString sTitle = "Warning", CString sExplanation = "",
		CString sOK = "OK");   // standard constructor
	~CWarningDlg();

	// Functions
	BOOL OnInitDialog();

	// Dialog data
	enum { IDD = IDD_WARNING };

protected:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnBtnOK();
	void AutoResizeControls();

	// Variables
	CTransparentStatic m_lblTitle;
	CStatic			   m_lblExplanation;
	CButton			   m_btnOK;
	CStatic			   m_bmpLogo;
	CFont			   m_fntTitle;
	CString			   m_sWindowCaption;
	CString			   m_sTitle;
	CString			   m_sExplanation;
	CString			   m_sOK;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // WARNINGDLG_H
