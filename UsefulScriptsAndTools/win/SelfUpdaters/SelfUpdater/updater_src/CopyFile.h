/********************************************************************
	created:	2005/03/24
	created:	24:3:2005   14:21
	filename: 	CopyFile.h
	file path:	Updater
	file base:	CopyFile
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef COPYFILE_H
#define COPYFILE_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"

//*********************************************************************
// CCopyFile
//*********************************************************************

class CCopyFile : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void SetParent(CWinThread * pParentThread);
	void StartFileCopy(CFile * pOriginalFile, CString sNewFile, bool bSkipIfExists);
	void StartFileCopy(CString sOriginalFile, CString sNewFile, bool bSkipIfExists);

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CCopyFile)
	CCopyFile();           // protected constructor used by dynamic creation
	virtual ~CCopyFile();

	// Functions
	void    Copy(CFile * pOriginalFile, CFile * pNewFile, bool bSkipIfExists);
	CString ExtractFilePath(CString sFilename);
	bool    CreateFolder(CString sFolder);
	void	SendCopyStatus(int iBytesCopied, int iTotalBytes);
	void	PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnStartCopy(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread * m_pParent;
	CFile	   * m_pOriginalFile;
	CFile	   * m_pNewFile;
	bool		 m_bSkipIfExists;
	int			 m_iCurrentActionPoints;
	int			 m_iActionPoints;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // COPYFILE_H

