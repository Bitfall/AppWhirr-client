/********************************************************************
	created:	2005/12/05
	created:	5:12:2005   11:26
	filename: 	ProxySettingsDlg.h
	file path:	Updater
	file base:	ProxySettingsDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef PROXYSETTINGSDLG_H
#define PROXYSETTINGSDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "Resource.h"

//*********************************************************************
// CProxySettingsDlg
//*********************************************************************

class CProxySettingsDlg : public CDialog
{
public:
	// Constructor & destructor
	CProxySettingsDlg(CWnd* pParent = NULL);   // standard constructor
	~CProxySettingsDlg();

	// Functions
	void OnBtnOK();
	void OnBtnCancel();
	void OnSelectDirectConnection();
	void OnSelectAutoDetect();
	void OnSelectManual();
	void OnSelectAutoConfigURL();

	// Dialog data
	enum { IDD = IDD_PROXYSETTINGS };

protected:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	void OnSysCommand(UINT nID, LPARAM lParam);
	void EnableManualProxy(BOOL bEnable = TRUE);
	void EnableAutoConfigURL(BOOL bEnable = TRUE);

	// Variables
	CButton	m_rdDirectConnection;
	CButton m_rdAutoDetect;
	CButton m_rdManual;
	CButton m_rdAutoConfigURL;
	CButton m_btnOK;
	CButton m_btnCancel;
	CStatic m_lblHttpProxy;
	CStatic m_lblFtpProxy;
	CEdit	m_txtFtpProxy;
	CEdit	m_txtHttpProxy;
	CEdit	m_txtAutoConfigURL;
	CBitmap	m_bmpMenuLogo;
	CBitmap	m_bmpMenuLogoNegative;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // PROXYSETTINGSDLG_H
