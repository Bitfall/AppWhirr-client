/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:20
	filename: 	ModeFullPropFinish.h
	file path:	Updater
	file base:	ModeFullPropFinish
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLPROPFINISH_H
#define MODEFULLPROPFINISH_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "afxwin.h"

//*********************************************************************
// CModeFullPropFinish dialog
//*********************************************************************

class CModeFullPropFinish : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CModeFullPropFinish)

public:
	// Constructor & destructor
	CModeFullPropFinish();
	virtual ~CModeFullPropFinish();

	// Functions

	// Dialog Data
	enum { IDD = IDD_FULL_PROPPAGE_FINISH };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL	OnInitDialog();
	BOOL	OnSetActive();
	BOOL	OnWizardFinish();
	afx_msg HBRUSH  OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	afx_msg BOOL	OnEraseBkgnd(CDC * pDC);

	// Variables
	CStatic			 m_lblExplanation;
	CButton		     m_chkCreateShortcuts;
	CFunctions	   * m_pFunctions;
	CSettings	   * m_pSettings;
	CUpdateInfo    * m_pUpdateInfo;
	CLanguage	   * m_pLanguage;
	CBitmap			 m_bmpWatermark;
	CBrush			 m_oHollowBrush;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODEFULLPROPFINISH_H
