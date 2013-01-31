/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:58
	filename: 	Rollback.h
	file path:	Updater
	file base:	Rollback
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef ROLLBACK_H
#define ROLLBACK_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "CopyFile.h"						// Copy file support
#include "CopyFolder.h"						// Copy folder support

//*********************************************************************
// CRollback
//*********************************************************************

class CRollback : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CRollback)
	CRollback();
	virtual ~CRollback();

	// Functions
	void PerformAction(int iIndex);
	void PerformNextAction();
	void PerformActionCopy(CRollbackActionBase * pAction);
	void PerformActionDelete(CRollbackActionBase * pAction);
	void PerformActionRegistry(CRollbackActionBase * pAction);
	void PerformActionUnregister(CRollbackActionBase * pAction);
	int  CalculateActionCount();
	void SendRollbackStatus();
	void PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	bool IsFolder(CString sFileOrFolder);
	afx_msg void OnSetParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnThreadMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRollbackComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread    * m_pParent;
	bool		    m_bStarted;
	CFunctions    * m_pFunctions;
	CSettings     * m_pSettings;
	CUpdateInfo   * m_pUpdateInfo;
	CRollbackInfo * m_pRollbackInfo;
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
#endif // ROLLBACK_H