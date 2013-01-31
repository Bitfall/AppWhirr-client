/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:14
	filename: 	ModeFullPropWelcome.h
	file path:	Updater
	file base:	ModeFullPropWelcome
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLPROPWELCOME_H
#define MODEFULLPROPWELCOME_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "afxwin.h"

//*********************************************************************
// CModeFullPropWelcome dialog
//*********************************************************************

class CModeFullPropWelcome : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CModeFullPropWelcome)

public:
	// Constructor & destructor
	CModeFullPropWelcome();
	virtual ~CModeFullPropWelcome();

	// Functions

	// Dialog Data
	enum { IDD = IDD_FULL_PROPPAGE_WELCOME };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL	OnInitDialog();
	BOOL	OnSetActive();
	LRESULT OnWizardNext();	
	afx_msg HBRUSH  OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	afx_msg BOOL	OnEraseBkgnd(CDC * pDC);

	// Variables
	CStatic			 m_lblTitle;
	CStatic			 m_lblExplanation;
	CFunctions	   * m_pFunctions;
	CSettings	   * m_pSettings;
	CUpdateInfo    * m_pUpdateInfo;
	CLanguage	   * m_pLanguage;
	CFont			 m_fntTitle;
	CBitmap			 m_bmpWatermark;
	CBrush			 m_oHollowBrush;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODEFULLPROPWELCOME_H
