/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:56
	filename: 	CheckVersion.h
	file path:	Updater
	file base:	CheckVersion
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef CHECKVERSION_H
#define CHECKVERSION_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "Internet.h"						// Internet support
#include "Sections.h"

//*********************************************************************
// CCheckVersion
//*********************************************************************

class CCheckVersion : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CCheckVersion)
	CCheckVersion();
	virtual ~CCheckVersion();

	// Functions
	void    Main();
	bool    CheckVersionSeparately();
	bool	CheckSections();
	void	PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	CString GetInternetFileDate(int iFileIndex, bool bTimeToo);
	afx_msg void OnDownloadComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnThreadMessage(WPARAM wParam, LPARAM lParam);

	// Variables
	bool		  m_bStarted;
	bool		  m_bDownloadsReady;
	bool		  m_bNoFailPossible;
	bool		  m_bUpdateSelector;
	CWinThread  * m_pParent;
	CFunctions  * m_pFunctions;
	CSettings   * m_pSettings;
	CUpdateInfo * m_pUpdateInfo;
	CInternet   * m_pInternet;
	CSections	* m_pSections;
	CPath		* m_pPath;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // CHECKVERSION_H