/********************************************************************
	created:	2004/11/08
	created:	8:11:2004   19:48
	filename: 	ModeFullPropDownload.h
	file path:	Updater
	file base:	ModeFullPropDownload
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLPROPDOWNLOAD_H
#define MODEFULLPROPDOWNLOAD_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "afxwin.h"
#include "afxcmn.h"

//*********************************************************************
// CPropCheck dialog
//*********************************************************************

class CModeFullPropDownload : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CModeFullPropDownload)

public:
	// Constructor & destructor
	CModeFullPropDownload();
	virtual ~CModeFullPropDownload();

	// Functions

	// Dialog Data
	enum { IDD = IDD_FULL_PROPPAGE_DOWNLOAD };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL	OnInitDialog();
	BOOL	OnSetActive();
	LRESULT OnWizardNext();
	afx_msg LRESULT OnUpdateGUIFileProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateGUITotalProgress(WPARAM wParam, LPARAM lParam);

	// Variables
	CStatic			 m_lblExplanation;
	CStatic			 m_lblFileProgressProcent;
	CStatic			 m_lblFileProgress;
	CProgressCtrl	 m_prgFileProgress;
	CStatic			 m_lblTotalProgressProcent;
	CStatic			 m_lblTotalProgress;
	CProgressCtrl	 m_prgTotalProgress;
	CFunctions	   * m_pFunctions;
	CSettings	   * m_pSettings;
	CUpdateInfo    * m_pUpdateInfo;
	CLanguage	   * m_pLanguage;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODEFULLPROPDOWNLOAD_H