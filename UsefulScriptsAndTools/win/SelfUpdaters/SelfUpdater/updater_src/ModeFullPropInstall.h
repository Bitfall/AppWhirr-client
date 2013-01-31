/********************************************************************
	created:	2004/11/08
	created:	8:11:2004   19:46
	filename: 	ModeFullPropInstall.h
	file path:	Updater
	file base:	ModeFullPropInstall
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLPROPINSTALL_H
#define MODEFULLPROPINSTALL_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CModeFullPropInstall dialog
//*********************************************************************

class CModeFullPropInstall : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CModeFullPropInstall)

public:
	// Constructor & destructor
	CModeFullPropInstall();
	virtual ~CModeFullPropInstall();

	// Functions

	// Dialog Data
	enum { IDD = IDD_FULL_PROPPAGE_INSTALL };

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
#endif // MODEFULLPROPINSTALL_H
