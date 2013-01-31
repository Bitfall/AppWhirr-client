/********************************************************************
	created:	2004/11/08
	created:	8:11:2004   19:46
	filename: 	ModeFullPropRollback.h
	file path:	Updater
	file base:	ModeFullPropRollback
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLPROPROLLBACK_H
#define MODEFULLPROPROLLBACK_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CModeFullPropRollback dialog
//*********************************************************************

class CModeFullPropRollback : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CModeFullPropRollback)

public:
	// Constructor & destructor
	CModeFullPropRollback();
	virtual ~CModeFullPropRollback();

	// Functions

	// Dialog Data
	enum { IDD = IDD_FULL_PROPPAGE_ROLLBACK };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL	OnInitDialog();
	BOOL	OnSetActive();
	BOOL	OnWizardFinish();
	afx_msg LRESULT OnUpdateGUITotalProgress(WPARAM wParam, LPARAM lParam);

	// Variables
	CStatic			 m_lblExplanation;
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
#endif // MODEFULLPROPROLLBACK_H
