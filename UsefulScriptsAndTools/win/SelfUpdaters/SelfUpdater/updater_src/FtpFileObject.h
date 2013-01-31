/********************************************************************
	created:	2005/10/31
	created:	31:10:2005   9:19
	filename: 	FtpFileObject.h
	file path:	Updater
	file base:	FttpFileObject
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef FTPFILEOBJECT_H
#define FTPFILEOBJECT_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"
#include <afxinet.h>
#include "Path.h"

//*********************************************************************
// CFtp
//*********************************************************************

class CFtpFileObject : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void	SetParent(CWinThread * pParentThread);
	void	SetTimeOut(int iTimeOut);
	bool	Open(CString sURL, CString sProxyUsername = "", CString sProxyPassword = "",
				 CString sFtpUsername = "", CString sFtpPassword = "");
	int		GetSize();
	CString GetVersion();
	CString	GetDate();
	CString GetDateTime();
	void	Download(CString sDestination, bool bResumeIfPossible, HANDLE hEventStopDownloading);
	void	Close();

protected:
	// Constructor & destructor
	DECLARE_DYNCREATE(CFtpFileObject)
	CFtpFileObject();           // protected constructor used by dynamic creation
	virtual ~CFtpFileObject();

	// Functions
	bool			EstablishConnection();
	void			HandleProxyServer();
	void			GetFileInformation(CString sObject);
	void			DownloadFile();
	bool			IsResumePossible(CString sDestination);
	void			CreateResumeFile(CString sDestination);	
	void			SendDownloadStatus(int iBytesCopied, int iTotalBytes);
	void			PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	CString			ExtractFilePath(CString sFilename);
	bool			CreateFolder(CString sFolder);
	afx_msg void	OnStartDownload(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread	  * m_pParent;
	CInternetSession * m_pNetSession;
	CFtpConnection * m_pFtpConnection;
	CString			m_sURL;
	CString			m_sDestination;
	CInternetFile * m_pFtpFile;
	CFile		  * m_pDestinationFile;
	CPath		  *	m_pPath;
	CLog		  * m_pLog;
	CString			m_sProxyUsername;
	CString			m_sProxyPassword;
	CString			m_sFtpUsername;
	CString			m_sFtpPassword;
	bool			m_bResumeIfPossible;
	long			m_lStartReadingByte;
	int				m_iTimeOut;
	int				m_iProxyRetryCount;
	int				m_iCurrentActionPoints;
	int				m_iActionPoints;
	HANDLE			m_hEventStopDownloading;

	int				m_iFileSize;
	CString			m_sFileDate;
	CString			m_sFileDateTime;
	CString			m_sFileVersion;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // FTPFILEOBJECT_H
