/********************************************************************
	created:	2005/04/01
	created:	1:4:2005   13:55
	filename: 	BackUp.h
	file path:	Updater
	file base:	BackUp
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef BACKUP_H
#define BACKUP_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"
#include "CopyFile.h"
#include "CopyFolder.h"

//*********************************************************************
// CBackUp
//*********************************************************************

class CBackUp : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void SetParent(CWinThread * pParentThread);
	void StartBackUpFile(CString sFilename);
	void StartBackUpFolder(CString sFolder);
	void StartBackUpRegistry(CString sRegistrykey);
	void StartBackUpRegister(CString sFilename);

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CBackUp)
	CBackUp();           // protected constructor used by dynamic creation
	virtual ~CBackUp();

	// Functions
	void AddRollbackDelete(CString sLocation);
	void AddRollbackCopy(CString sOldLocation, CString sNewLocation);
	void AddRollbackRegistry(CString sRegistrykey, CString sValue);
	void AddRollbackRegister(CString sFile);
	void PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread  * m_pParent;
	CCopyFile   * m_pCopyFile;
	CCopyFolder * m_pCopyFolder;
	CLog		* m_pLog;
	CString		  m_sBackupLocation;
	CString		  m_sCurrentLocation;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // BACKUP_H
