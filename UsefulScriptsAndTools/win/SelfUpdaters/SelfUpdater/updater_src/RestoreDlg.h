/********************************************************************
	created:	2005/10/02
	created:	2:10:2005   12:28
	filename: 	RestoreDlg.h
	file path:	Updater
	file base:	RestoreDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef RESTOREDLG_H
#define RESTOREDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************


//*********************************************************************
// CRestoreDlg
//*********************************************************************

class CRestoreDlg : public CDialog
{
public:
	// Constructor & destructor
	CRestoreDlg(CWnd* pParent = NULL);   // standard constructor

	// Functions
	void OnBtnRestore();
	void OnBtnClose();

	// Dialog data
	enum { IDD = IDD_RESTORE };

protected:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL	OnInitDialog();
	BOOL	OnCommand(WPARAM wParam, LPARAM lParam);
	void	OnSysCommand(UINT nID, LPARAM lParam);
	bool	CloseDialog();
	CString GetPreviousVersion();
	CString GetPreviousVersionLocation();
	afx_msg LRESULT OnUpdateGUIProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRestoreComplete(WPARAM wParam, LPARAM lParam);

	// Variables
	CProgressCtrl m_prgRestore;
	CButton		  m_btnRestore;
	CButton		  m_btnClose;
	CStatic		  m_lblExplanation;
	CStatic		  m_lblProgress;
	bool		  m_bRestoreRunning;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // RESTOREDLG_H
