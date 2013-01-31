// ExtractorDlg.h : header file
//

#if !defined(AFX_EXTRACTORDLG_H__3DE9A2F7_4A44_11D3_A8BC_0050043A01C0__INCLUDED_)
#define AFX_EXTRACTORDLG_H__3DE9A2F7_4A44_11D3_A8BC_0050043A01C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "SelfExtractor.h"
#include "DirDialog.h"
#include "TextProgressCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CExtractorDlg dialog

class CExtractorDlg : public CDialog
{
// Construction
public:
	CExtractorDlg(CWnd* pParent = NULL);	// standard constructor

	CSelfExtractor	m_Extractor;

	//Callback for extraction
	static void ExtractCallBack(int nMsgType, void *CallBackData, void* userData);

// Dialog Data
	//{{AFX_DATA(CExtractorDlg)
	enum { IDD = IDD_EXTRACTOR_DIALOG };
	CEdit	m_Path;
	CButton	m_Extract;
	CButton	m_Browse;
	CTextProgressCtrl	m_ZipProgress;
	CTextProgressCtrl	m_Progress;
	CString	m_strPath;
	CString	m_strTotalStatus;
	CString	m_strFileStatus;
	CString	m_strIntroText;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtractorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CExtractorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowse();
	afx_msg void OnExtract();
	afx_msg void OnAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTRACTORDLG_H__3DE9A2F7_4A44_11D3_A8BC_0050043A01C0__INCLUDED_)
