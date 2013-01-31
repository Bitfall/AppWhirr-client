/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	Internet.h
	file path:	Updater
	file base:	Internet
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef INTERNET_H
#define INTERNET_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"

#include "HttpFileObject.h"		// Support for http files
#include "NetworkFileObject.h"	// Support for network files
#include "FtpFileObject.h"		// Support for ftp files
#include <afxinet.h>			// CInternetSession support
#include <memory>				// Needed for singleton class
using namespace std;

//*********************************************************************
// CInternet
//*********************************************************************

class CInternet
{
public:
	// Functions
	static CInternet * Instance();
	void    SetParent(CWinThread * pParentThread);
	void	SetTimeOut(int iTimeOut);
	void	SetProxyAuthentication(CString sUsername, CString sPassword);
	void	SetHttpAuthentication(CString sUsername, CString sPassword);
	void	SetFtpAuthentication(CString sUsername, CString sPassword);
	bool    DownloadFile(CString sURL, CString sDestination, bool bResumeIfPossible = false);
	bool    FileExists(CString sURL);
	CString GetFileVersion(CString sURL);
	CString GetFileDate(CString sURL);
	CString GetFileDateTime(CString sURL);
	int		GetFileSize(CString sURL);
	void	StopDownloading();
	void	CleanUp();

private:
	// Constructor & destructor
	CInternet();			// Private constructor because singleton!
	
	// Functions
	DWORD	GetFileType(CString sURL);

	// Variables
	static std::auto_ptr<CInternet> sm_inst;
	CHttpFileObject	   * m_pHttpFile;
	CFtpFileObject	   * m_pFtpFile;
	CNetworkFileObject * m_pNetworkFile;
	CLog			   * m_pLog;
	CString				 m_sProxyUsername;
	CString				 m_sProxyPassword;
	CString				 m_sHttpUsername;
	CString				 m_sHttpPassword;
	CString				 m_sFtpUsername;
	CString				 m_sFtpPassword;
	HANDLE				 m_hEventStopDownloading;
};

// End of redefinition protection
#endif // INTERNET_H