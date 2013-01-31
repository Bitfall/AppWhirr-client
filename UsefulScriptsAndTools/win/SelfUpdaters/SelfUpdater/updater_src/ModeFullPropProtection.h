/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:20
	filename: 	ModeFullPropProtection.h
	file path:	Updater
	file base:	ModeFullPropProtection
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLPROPPROTECTION_H
#define MODEFULLPROPPROTECTION_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "afxwin.h"
#include "ResultLabel.h"
#include "HyperlinkLabel.h"

//*********************************************************************
// CModeFullPropProtection dialog
//*********************************************************************

class CModeFullPropProtection : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CModeFullPropProtection)

public:
	// Constructor & destructor
	CModeFullPropProtection();
	virtual ~CModeFullPropProtection();

	// Functions

	// Dialog Data
	enum { IDD = IDD_FULL_PROPPAGE_PROTECTION };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL	OnInitDialog();
	BOOL	OnSetActive();
	LRESULT OnWizardNext();
	BOOL	OnWizardFinish();
	afx_msg LRESULT OnUpdateGUIProtection(WPARAM wParam, LPARAM lParam);

	// Variables
	CStatic			 m_lblExplanation;
	CResultLabel	 m_lblCheckLicense;
	CResultLabel	 m_lblCheckFiles;
	CHyperlinkLabel  m_lblGetLicense;
	CFunctions	   * m_pFunctions;
	CSettings	   * m_pSettings;
	CUpdateInfo    * m_pUpdateInfo;
	CLanguage	   * m_pLanguage;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODEFULLPROPPROTECTION_H
