/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:16
	filename: 	ModeFullPropCheck.h
	file path:	Updater
	file base:	ModeFullPropCheck
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLPROPCHECK_H
#define MODEFULLPROPCHECK_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "afxwin.h"
#include "HyperlinkLabel.h"

//*********************************************************************
// CModeFullPropCheck dialog
//*********************************************************************

class CModeFullPropCheck : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CModeFullPropCheck)

public:
	// Constructor & destructor
	CModeFullPropCheck();
	virtual ~CModeFullPropCheck();

	// Functions
	void SetVersionResult(bool bNewVersion = true);

	// Dialog Data
	enum { IDD = IDD_FULL_PROPPAGE_CHECK };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL	OnInitDialog();
	BOOL	OnSetActive();
	LRESULT OnWizardNext();
	BOOL	OnWizardFinish();

	// Variables
	CStatic			 m_lblExplanation;
	CStatic			 m_lblStatus;
	CStatic			 m_lblFeatures;
	CHyperlinkLabel  m_arrFeatures[MAX_FEATURES];
	CFunctions	   * m_pFunctions;
	CSettings	   * m_pSettings;
	CUpdateInfo    * m_pUpdateInfo;
	CLanguage	   * m_pLanguage;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODEFULLPROPCHECK_H
