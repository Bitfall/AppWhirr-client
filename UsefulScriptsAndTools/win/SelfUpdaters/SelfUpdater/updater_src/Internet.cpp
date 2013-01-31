/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	Internet.cpp
	file path:	Updater
	file base:	Internet
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Internet.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CInternet> CInternet::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CInternet::CInternet()
{
	// Set some values
	m_sProxyUsername = "";
	m_sProxyPassword = "";
	m_sHttpUsername = "";
	m_sHttpPassword = "";
	m_sFtpUsername = "";
	m_sFtpPassword = "";

	// Init singleton classes
	m_pLog = CLog::Instance();

	// Create the file classes
	m_pHttpFile = (CHttpFileObject *) AfxBeginThread(RUNTIME_CLASS(CHttpFileObject),
											THREAD_PRIORITY_NORMAL,
											0,
											0,
											NULL);

	m_pNetworkFile = (CNetworkFileObject *) AfxBeginThread(RUNTIME_CLASS(CNetworkFileObject),
												THREAD_PRIORITY_NORMAL,
												0,
												0,
												NULL);

	m_pFtpFile = (CFtpFileObject *) AfxBeginThread(RUNTIME_CLASS(CFtpFileObject),
												THREAD_PRIORITY_NORMAL,
												0,
												0,
												NULL);

	// Create stop downloading event
	m_hEventStopDownloading = CreateEvent(NULL, NULL, TRUE, NULL);
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CInternet * CInternet::Instance()
{
	if(sm_inst.get() == 0)
	sm_inst = auto_ptr<CInternet>(new CInternet);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CInternet);
	return sm_inst.get();
	*/
}

//=====================================================================

void CInternet::SetParent(CWinThread * pParentThread)
{
	// Set parent for all connection types
	m_pNetworkFile->SetParent(pParentThread);
	m_pHttpFile->SetParent(pParentThread);
	m_pFtpFile->SetParent(pParentThread);
}

//=====================================================================

void CInternet::SetTimeOut(int iTimeOut)
{
	// Set timeout for all connection types
	m_pNetworkFile->SetTimeOut(iTimeOut);
	m_pHttpFile->SetTimeOut(iTimeOut);
	m_pFtpFile->SetTimeOut(iTimeOut);
}

//=====================================================================

void CInternet::SetProxyAuthentication(CString sUsername, CString sPassword)
{
	// Set proxy authentication
	m_sProxyUsername = sUsername;
	m_sProxyPassword = sPassword;

}

//=====================================================================

void CInternet::SetHttpAuthentication(CString sUsername, CString sPassword)
{
	// Store information
	m_sHttpUsername = sUsername;
	m_sHttpPassword = sPassword;
}

//=====================================================================

void CInternet::SetFtpAuthentication(CString sUsername, CString sPassword)
{
	// Store information
	m_sFtpUsername = sUsername;
	m_sFtpPassword = sPassword;
}

//=====================================================================

bool CInternet::DownloadFile(CString sURL, CString sDestination,
							 bool bResumeIfPossible /* = false */)
{
	// Declare variables
	bool bDownloadStarted = false;
	CPath * pPath = CPath::Instance();

	// Allow threads to download
	ResetEvent(m_hEventStopDownloading);

	// Log
	m_pLog->Log("CInternet::DownloadFile -> " + pPath->ExtractFileName(sURL));

	// Download file
	switch (GetFileType(sURL))
	{
	case AFX_INET_SERVICE_FILE:
		// Open file
		if (m_pNetworkFile->Open(sURL))
		{
			// Start downloading
			bDownloadStarted = true;
			m_pNetworkFile->Download(sDestination, bResumeIfPossible, m_hEventStopDownloading);
		}
		break;

	case AFX_INET_SERVICE_FTP:
		// Open file
		if (m_pFtpFile->Open(sURL, m_sProxyUsername, m_sProxyPassword,
				m_sFtpUsername, m_sFtpPassword))
		{
			// Start downloading
			bDownloadStarted = true;
			m_pFtpFile->Download(sDestination, bResumeIfPossible, m_hEventStopDownloading);
		}
		break;

	case AFX_INET_SERVICE_HTTP:
	case AFX_INET_SERVICE_HTTPS:
		// Open file
		if (m_pHttpFile->Open(sURL, m_sProxyUsername, m_sProxyPassword,
				m_sHttpUsername, m_sHttpPassword))
		{
			// Start downloading
			bDownloadStarted = true;
			m_pHttpFile->Download(sDestination, bResumeIfPossible, m_hEventStopDownloading);
		}
		break;
	}

	// Return result
	return bDownloadStarted;
}

//=====================================================================

bool CInternet::FileExists(CString sURL)
{
	// Check if the size is -1
	if (GetFileSize(sURL) == -1)
		return false;

	return true;
}

//=====================================================================

CString CInternet::GetFileVersion(CString sURL)
{
	// Declare variables
	CString sVersion = "0.0.0.0";

	// Get date & time of file
	switch (GetFileType(sURL))
	{
	case AFX_INET_SERVICE_FILE:
		// Open file
		if (m_pNetworkFile->Open(sURL))
		{
			// Get date & time
			sVersion = m_pNetworkFile->GetVersion();

			// Close file
			m_pNetworkFile->Close();
		}
		break;

	case AFX_INET_SERVICE_FTP:
		// Open file
		if (m_pFtpFile->Open(sURL, m_sProxyUsername, m_sProxyPassword,
			m_sFtpUsername, m_sFtpPassword))
		{
			// Get version
			sVersion = m_pFtpFile->GetVersion();

			// Close file
			m_pFtpFile->Close();
		}
		break;

	case AFX_INET_SERVICE_HTTP:
	case AFX_INET_SERVICE_HTTPS:
		// Open file
		if (m_pHttpFile->Open(sURL, m_sProxyUsername, m_sProxyPassword,
			m_sHttpUsername, m_sHttpPassword))
		{
			// Get version
			sVersion = m_pHttpFile->GetVersion();

			// Close file
			m_pHttpFile->Close();
		}
		break;
	}

	// Return version
	return sVersion;
}

//=====================================================================

CString CInternet::GetFileDate(CString sURL)
{
	// Declare variables
	CString sDate = "1970-01-01";

	// Get date & time of file
	switch (GetFileType(sURL))
	{
	case AFX_INET_SERVICE_FILE:
		// Open file
		if (m_pNetworkFile->Open(sURL))
		{
			// Get date & time
			sDate = m_pNetworkFile->GetDate();

			// Close file
			m_pNetworkFile->Close();
		}
		break;

	case AFX_INET_SERVICE_FTP:
		// Open file
		if (m_pFtpFile->Open(sURL, m_sProxyUsername, m_sProxyPassword,
			m_sFtpUsername, m_sFtpPassword))
		{
			// Get size
			sDate = m_pFtpFile->GetDate();

			// Close file
			m_pFtpFile->Close();
		}
		break;

	case AFX_INET_SERVICE_HTTP:
	case AFX_INET_SERVICE_HTTPS:
		// Open file
		if (m_pHttpFile->Open(sURL, m_sProxyUsername, m_sProxyPassword,
			m_sHttpUsername, m_sHttpPassword))
		{
			// Get size
			sDate = m_pHttpFile->GetDate();

			// Close file
			m_pHttpFile->Close();
		}
		break;
	}

	// Return date
	return sDate;
}

//=====================================================================

CString CInternet::GetFileDateTime(CString sURL)
{
	// Declare variables
	CString sDateTime = "1970-01-01/00:00:00";

	// Get date & time of file
	switch (GetFileType(sURL))
	{
	case AFX_INET_SERVICE_FILE:
		// Open file
		if (m_pNetworkFile->Open(sURL))
		{
			// Get date & time
			sDateTime = m_pNetworkFile->GetDateTime();

			// Close file
			m_pNetworkFile->Close();
		}
		break;

	case AFX_INET_SERVICE_FTP:
		// Open file
		if (m_pFtpFile->Open(sURL, m_sProxyUsername, m_sProxyPassword,
			m_sFtpUsername, m_sFtpPassword))
		{
			// Get date & time
			sDateTime = m_pFtpFile->GetDateTime();

			// Close file
			m_pFtpFile->Close();
		}
		break;

	case AFX_INET_SERVICE_HTTP:
	case AFX_INET_SERVICE_HTTPS:
		// Open file
		if (m_pHttpFile->Open(sURL, m_sProxyUsername, m_sProxyPassword,
			m_sHttpUsername, m_sHttpPassword))
		{
			// Get date & time
			sDateTime = m_pHttpFile->GetDateTime();

			// Close file
			m_pHttpFile->Close();
		}
		break;
	}

	// Return date & time
	return sDateTime;
}

//=====================================================================

int CInternet::GetFileSize(CString sURL)
{
	// Declare variables
	int iSize = -1;

	// Get size of file
	switch (GetFileType(sURL))
	{
	case AFX_INET_SERVICE_FILE:
		// Open file
		if (m_pNetworkFile->Open(sURL))
		{
			// Get size
			iSize = m_pNetworkFile->GetSize();

			// Close file
			m_pNetworkFile->Close();
		}
		break;

	case AFX_INET_SERVICE_FTP:
		// Open file
		if (m_pFtpFile->Open(sURL, m_sProxyUsername, m_sProxyPassword,
			m_sFtpUsername, m_sFtpPassword))
		{
			// Get size
			iSize = m_pFtpFile->GetSize();

			// Close file
			m_pFtpFile->Close();
		}
		break;

	case AFX_INET_SERVICE_HTTP:
	case AFX_INET_SERVICE_HTTPS:
		// Open file
		if (m_pHttpFile->Open(sURL, m_sProxyUsername, m_sProxyPassword,
			m_sHttpUsername, m_sHttpPassword))
		{
			// Get size
			iSize = m_pHttpFile->GetSize();

			// Close file
			m_pHttpFile->Close();
		}
		break;
	}
	
	// Return the size
	return iSize;
}

//=====================================================================

void CInternet::StopDownloading()
{
	// Stop downloading for all file objects
	SetEvent(m_hEventStopDownloading);
}

//=====================================================================

void CInternet::CleanUp()
{
	// Always stop downloading
	StopDownloading();
	
	// Delete file objects
	delete m_pHttpFile;
	delete m_pNetworkFile;
	delete m_pFtpFile;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

DWORD CInternet::GetFileType(CString sURL)
{
	// Declare variables
	CString sServer, sObject, sProxy, sAuthentication;
	INTERNET_PORT nPort;
	DWORD dwServiceType;

	// Check if path exists (on hard-drive)
	if (PathFileExists(sURL))
	{
		// Get file from network
		dwServiceType = AFX_INET_SERVICE_FILE;
	}
	else
	{
		// Parse URL to check whether it is an URL or network path
		if (!AfxParseURL(sURL, dwServiceType, sServer, sObject, nPort))
			return AFX_INET_SERVICE_FILE;
	}

	// Return type we are dealing with
	return dwServiceType;
}
