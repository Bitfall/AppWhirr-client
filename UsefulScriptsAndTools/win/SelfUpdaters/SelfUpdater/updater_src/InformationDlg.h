/********************************************************************
	created:	2005/08/06
	created:	6:8:2005   10:12
	filename: 	InformationDlg.h
	file path:	Updater
	file base:	InformationDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef INFORMATIONDLG_H
#define INFORMATIONDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "TransparentStatic.h"
#include "Updater.h"

//*********************************************************************
// CInformationDlg
//*********************************************************************

class CInformationDlg : public CDialog
{
public:
	// Constructor & destructor
	CInformationDlg(CString sWindowCaption = "Information",
		CString sTitle = "Information", CString sExplanation = "",
		CString sOK = "OK");   // standard constructor
	~CInformationDlg();

	// Functions
	BOOL OnInitDialog();

	// Dialog data
	enum { IDD = IDD_INFORMATION };

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
#endif // INFORMATIONDLG_H
