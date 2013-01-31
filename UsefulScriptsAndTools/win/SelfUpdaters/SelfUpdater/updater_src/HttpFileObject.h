/********************************************************************
	created:	2005/10/31
	created:	31:10:2005   9:19
	filename: 	Http.h
	file path:	Updater
	file base:	HttpFileObject
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef HTTPFILEOBJECT_H
#define HTTPFILEOBJECT_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"
#include <afxinet.h>
#include "Path.h"
#include "Proxy.h"

//*********************************************************************
// CHttp
//*********************************************************************

class CHttpFileObject : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void	SetParent(CWinThread * pParentThread);
	void	SetTimeOut(int iTimeOut);
	bool	Open(CString sURL, CString sProxyUsername = "", CString sProxyPassword = "",
				 CString sHttpUsername = "", CString sHttpPassword = "");
	int		GetSize();
	CString GetVersion();
	CString	GetDate();
	CString GetDateTime();
	void	Download(CString sDestination, bool bResumeIfPossible, HANDLE hEventStopDownloading);
	void	Close();

protected:
	// Constructor & destructor
	DECLARE_DYNCREATE(CHttpFileObject)
	CHttpFileObject();           // protected constructor used by dynamic creation
	virtual ~CHttpFileObject();

	// Functions
	bool			EstablishConnection();
    void            ProcessHttpRequest(CHttpFile* pFile);
    bool            ProcessHttpResponse(CHttpFile* pFile);
    bool            ProcessHttpException(CHttpFile* pFile,CException* pEx);
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
	CProxy		  * m_pProxy;
	CString			m_sURL;
	CString			m_sDestination;
	CInternetFile * m_pHttpFile;
	CFile		  * m_pDestinationFile;
	CPath		  *	m_pPath;
	CLog		  * m_pLog;
	CString			m_sProxyUsername;
	CString			m_sProxyPassword;
	CString			m_sHttpUsername;
	CString			m_sHttpPassword;
	bool			m_bResumeIfPossible;
	long			m_lStartReadingByte;
	int				m_iTimeOut;
	int				m_iProxyRetryCount;
	int				m_iCurrentActionPoints;
	int				m_iActionPoints;
	HANDLE			m_hEventStopDownloading;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // HTTPFILEOBJECT_H

