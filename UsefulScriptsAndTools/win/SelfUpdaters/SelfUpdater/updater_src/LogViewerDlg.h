/********************************************************************
	created:	2005/08/06
	created:	6:8:2005   10:12
	filename: 	LogViewerDlg.h
	file path:	Updater
	file base:	LogViewerDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef LOGVIEWERDLG_H
#define LOGVIEWERDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "Updater.h"
#include "Resource.h"
#include "EditLicense.h"

//*********************************************************************
// CLogViewerDlg
//*********************************************************************

class CLogViewerDlg : public CDialog
{
public:
	// Constructor & destructor
	CLogViewerDlg(CWnd* pParent = NULL);   // standard constructor

	// Functions
	BOOL OnInitDialog();

	// Dialog Data
	enum { IDD = IDD_LOGVIEWER };

protected:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO * lpMMI);
	void OnBtnOK();

	// Variables
	CEditLicense m_txtLog;
	CButton		 m_btnOK;
	CFont		 m_fntLog;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // LOGVIEWERDLG_H
