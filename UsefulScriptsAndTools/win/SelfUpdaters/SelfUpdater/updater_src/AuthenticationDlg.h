/********************************************************************
	created:	2005/07/06
	created:	6:7:2005   10:36
	filename: 	AuthenticationDlg.h
	file path:	Updater
	file base:	AuthenticationDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef AUTHENTICATIONDLG_H
#define AUTHENTICATIONDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "Updater.h"

//*********************************************************************
// CAuthenticationDlg
//*********************************************************************

class CAuthenticationDlg : public CDialog
{
public:
	// Constructor & destructor
	CAuthenticationDlg(CWnd* pParent = NULL);   // standard constructor
	~CAuthenticationDlg();

	// Functions
	BOOL OnInitDialog();

	// Dialog data
	enum { IDD = IDD_AUTHENTICATION };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void    OnBtnOK();
	void    OnBtnCancel();
	CString GetActiveUser();
	void	AutoResizeControls();

	// Variables
	CTransparentStatic	m_lblTitle;
	CStatic			    m_bmpLogo;
	CStatic				m_lblExplanation;
	CStatic				m_lblUsername;
	CStatic				m_lblPassword;
	CEdit				m_txtUsername;
	CEdit				m_txtPassword;
	CFont			    m_fntTitle;
	CButton				m_chkSaveCredentials;
	CButton				m_btnOK;
	CButton				m_btnCancel;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // AUTHENTICATIONDLG_H
