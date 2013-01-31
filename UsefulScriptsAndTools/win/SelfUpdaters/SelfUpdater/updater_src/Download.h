/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:58
	filename: 	Download.h
	file path:	Updater
	file base:	Download
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef DOWNLOAD_H
#define DOWNLOAD_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "Internet.h"				// Internet support

//*********************************************************************
// CUSTOM STRUCTS
//*********************************************************************
 
struct DownloadInfo
{
	CString sTitle;
	CString sFilename;
	CString sDownloadLocation;
	CString sDestination;
};

//*********************************************************************
// CDownload
//*********************************************************************

class CDownload : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CDownload)
	CDownload();
	virtual ~CDownload();

	// Functions
	void Main();
	
	bool InitDownloads();
	void StartDownload(int iDownload);
	void StartNextDownload();
	bool EnoughFreeDiskSpace();
	void PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnDownloadStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDownloadComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnError(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnThreadMessage(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread  * m_pParent;
	bool		  m_bStarted;
	int			  m_iTotalBytes;
	int			  m_iFileProgress;
	int			  m_iTotalProgress;
	int			  m_iDownloadCount;
	int			  m_iCurrentDownload;
	int			  m_iTotalActionPoints;
	int			  m_iCurrentActionPoints;
	int			  m_iTempActionPoints;
	DownloadInfo  m_arrDownloads[MAX_DOWNLOADS];
	CFunctions  * m_pFunctions;
	CSettings   * m_pSettings;
	CUpdateInfo * m_pUpdateInfo;
	CInternet   * m_pInternet;
	CPath		* m_pPath;
	TCHAR		* m_szFilename;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // DOWNLOAD_H