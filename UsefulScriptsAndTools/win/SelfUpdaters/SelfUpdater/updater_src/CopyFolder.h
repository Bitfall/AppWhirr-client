/********************************************************************
	created:	2005/04/01
	created:	1:4:2005   9:48
	filename: 	CopyFolder.h
	file path:	Updater
	file base:	CopyFolder
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef COPYFOLDER_H
#define COPYFOLDER_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"
#include "CopyFile.h"					// Copy file support

//*********************************************************************
// CUSTOM STRUCTS
//*********************************************************************

const int MAX_COPYINSTRUCTIONS = 65535;

struct CopyInstruction
{
	CString sOriginalLocation;
	CString sNewLocation;
};

//*********************************************************************
// CCopyFolder thread
//*********************************************************************

class CCopyFolder : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void SetParent(CWinThread * pParentThread);
	void StartFolderCopy(CString sSource, CString sDestination,
					     bool bFailIfExists);

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CCopyFolder)
	CCopyFolder();           // protected constructor used by dynamic creation
	virtual ~CCopyFolder();

	// Functions
	void SetupFolderScript(CString sSource, CString sDestination);
	void CopyFile(int iFile);
	void CopyNextFile();
	void SendCopyStatus(int iBytesCopied, int iTotalBytes);
	void PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyStart(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyFileStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCopyFileComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread    * m_pParent;
	CCopyFile     * m_pCopyThread;
	bool		    m_bFailIfExists;
	int			    m_iCopyInstructionCount;
	int				m_iCurrentCopyInstruction;
	int			    m_iActionPoints;
	int			    m_iCurrentActionPoints;
	int			    m_iTempActionPoints;
	int				m_iFilesToCopy;
	int				m_iFilesCopied;
	CopyInstruction m_arrCopyInstructions[MAX_COPYINSTRUCTIONS];

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // COPYFOLDER_H