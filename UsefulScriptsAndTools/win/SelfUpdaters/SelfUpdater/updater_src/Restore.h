/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:58
	filename: 	Restore.h
	file path:	Updater
	file base:	Restore
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef RESTORE_H
#define RESTORE_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "CopyFile.h"						// Copy file support
#include "CopyFolder.h"						// Copy folder support

//*********************************************************************
// CRestore
//*********************************************************************

class CRestore : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CRestore)
	CRestore();
	virtual ~CRestore();

	// Functions
	void PerformAction(int iIndex);
	void PerformNextAction();
	void PerformActionCopy(CRestoreActionBase * pAction);
	void PerformActionDelete(CRestoreActionBase * pAction);
	void PerformActionRegistry(CRestoreActionBase * pAction);
	void PerformActionUnregister(CRestoreActionBase * pAction);
	int  CalculateActionCount();
	void SendRestoreStatus();
	void PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	bool IsFolder(CString sFileOrFolder);
	afx_msg void OnSetParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnThreadMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRestoreComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);

	// Variables
	CWnd		  * m_pParent;
	bool		    m_bStarted;
	CFunctions    * m_pFunctions;
	CSettings     * m_pSettings;
	CUpdateInfo   * m_pUpdateInfo;
	CRestoreInfo * m_pRestoreInfo;
	int			    m_iCurrentAction;
	int				m_iTotalProgress;
	int				m_iCurrentActionPoints;
	int				m_iTotalActionPoints;
	int				m_iTempActionPoints;
	CCopyFile     * m_pCopyFile;
	CCopyFolder   * m_pCopyFolder;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // RESTORE_H