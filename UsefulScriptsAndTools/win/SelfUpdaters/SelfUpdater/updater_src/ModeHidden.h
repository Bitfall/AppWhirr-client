/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   15:15
	filename: 	ModeHidden.h
	file path:	Updater
	file base:	ModeHidden
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEHIDDEN_H
#define MODEHIDDEN_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "resource.h"

//*********************************************************************
// CModeHidden
//*********************************************************************

class CModeHidden : public CDialog
{
public:
	// Constructor & destructor
	DECLARE_DYNAMIC(CModeHidden)
	CModeHidden(CWinThread * pUpdateProcess, int iCurrentTask = TASK_INIT);
	virtual ~CModeHidden();

	// Functions

	// Dialog Data
	enum { IDD = IDD_HIDDEN };

private:
	// Functions
	BOOL OnInitDialog();
	afx_msg LRESULT OnTaskStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskComplete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnError(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWarning(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnQuestion(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread  * m_pUpdateProcess;
	int			  m_iCurrentTask;
	CFunctions  * m_pFunctions;
	CSettings   * m_pSettings;
	CUpdateInfo * m_pUpdateInfo;
	CLanguage	* m_pLanguage;
	CPath		* m_pPath;
	bool		  m_bInit;
	bool		  m_bFinished;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODEHIDDEN_H