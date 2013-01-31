/********************************************************************
	created:	2005/08/06
	created:	6:8:2005   10:12
	filename: 	CloseApplicationDlg.h
	file path:	Updater
	file base:	CloseApplicationDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef CLOSEAPPLICATIONDLG_H
#define CLOSEAPPLICATIONDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "TransparentStatic.h"
#include "TransparentIcon.h"
#include "Updater.h"
#include "Resource.h"

const int CLOSEAPPLICATION_TIME = 60;

//*********************************************************************
// CCloseApplicationDlg
//*********************************************************************

class CCloseApplicationDlg : public CDialog
{
public:
	// Constructor & destructor
	CCloseApplicationDlg(CString sApplicationName = _T(""), CString sApplicationPath = _T(""));   // standard constructor
	~CCloseApplicationDlg();

	// Functions
	BOOL OnInitDialog();

	// Dialog data
	enum { IDD = IDD_CLOSEAPPLICATION };

protected:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnSysCommand(UINT nID, LPARAM lParam);
	void OnTimer(UINT nIDEvent);
	void OnBtnCancel();
	void OnBtnEndNow();
	void AutoResizeControls();

	// Variables
	CTransparentStatic	m_lblTitle;
	CTransparentIcon	m_icoApplication;
	CStatic				m_lblExplanation;
	CProgressCtrl		m_prgEnd;
	CButton				m_btnCancel;
	CButton				m_btnEndNow;
	CFont				m_fntTitle;
	CString				m_sApplicationTitle;
	CString				m_sApplicationPath;
	int					m_iCountDown;
	HICON				m_hIcon;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // CLOSEAPPLICATIONDLG_H
