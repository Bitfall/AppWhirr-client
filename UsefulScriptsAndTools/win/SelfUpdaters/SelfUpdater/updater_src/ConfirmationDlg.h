/********************************************************************
	created:	2005/08/06
	created:	6:8:2005   10:12
	filename: 	ConfirmationDlg.h
	file path:	Updater
	file base:	ConfirmationDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef CONFIRMATIONDLG_H
#define CONFIRMATIONDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "TransparentStatic.h"
#include "Updater.h"

//*********************************************************************
// CConfirmationDlg
//*********************************************************************

class CConfirmationDlg : public CDialog
{
public:
	// Constructor & destructor
	CConfirmationDlg(CString sWindowCaption = "Confirmation required",
		CString sTitle = "Confirmation required", CString sExplanation = "",
		CString sYes = "Yes", CString sNo = "No");   // standard constructor
	~CConfirmationDlg();

	// Functions
	BOOL OnInitDialog();

	// Dialog data
	enum { IDD = IDD_CONFIRMATION };

protected:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnBtnYes();
	void OnBtnNo();
	void AutoResizeControls();

	// Variables
	CTransparentStatic m_lblTitle;
	CStatic			   m_lblExplanation;
	CButton			   m_btnYes;
	CButton			   m_btnNo;
	CStatic			   m_bmpLogo;
	CFont			   m_fntTitle;
	CString			   m_sWindowCaption;
	CString			   m_sTitle;
	CString			   m_sExplanation;
	CString			   m_sYes;
	CString			   m_sNo;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // CONFIRMATIONDLG_H
