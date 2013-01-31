/********************************************************************
	created:	2005/10/31
	created:	31:10:2005   9:19
	filename: 	Network.h
	file path:	Updater
	file base:	NetworkFileObject
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef NETWORKFILEOBJECT_H
#define NETWORKFILEOBJECT_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"
#include <afxinet.h>
#include "Path.h"

//*********************************************************************
// CNetwork
//*********************************************************************

class CNetworkFileObject : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void	SetParent(CWinThread * pParentThread);
	void	SetTimeOut(int iTimeOut);
	bool	Open(CString sURL);
	int		GetSize();
	CString GetVersion();
	CString	GetDate();
	CString GetDateTime();
	void	Download(CString sDestination, bool bResumeIfPossible, HANDLE hEventStopDownloading);
	void	Close();

protected:
	// Constructor & destructor
	DECLARE_DYNCREATE(CNetworkFileObject)
	CNetworkFileObject();           // protected constructor used by dynamic creation
	virtual ~CNetworkFileObject();

	// Functions
	bool			EstablishConnection();
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
	CString			m_sURL;
	CString			m_sDestination;
	CFile		  * m_pNetworkFile;
	CFile		  * m_pDestinationFile;
	CPath		  *	m_pPath;
	CLog		  * m_pLog;
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
#endif // NETWORKFILEOBJECT_H

