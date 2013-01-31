/********************************************************************
	created:	2005/10/31
	created:	31:10:2005   9:46
	filename: 	Ftp.cpp
	file path:	Updater
	file base:	FtpFileObject
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "FtpFileObject.h"
#include "Proxy.h"						// Proxy support
#include "AuthenticationDlg.h"			// Proxy authentication
#include <wininet.h>

const CString TIMECOMPARE = _T("%Y%m%d%H%M%S");
const CString TIMECOMPARE_DEFAULT = _T("19700101000000");

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CFtpFileObject, CWinThread)
	ON_THREAD_MESSAGE(WMU_DOWNLOAD_START, OnStartDownload)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CFtpFileObject, CWinThread)

//=====================================================================

CFtpFileObject::CFtpFileObject()
{
}

//=====================================================================

CFtpFileObject::~CFtpFileObject()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CFtpFileObject::InitInstance()
{	
	// Initialize singleton classes
	m_pLog = CLog::Instance();

	// Set the pointers to NULL
	m_pFtpFile = NULL;
	m_pFtpConnection = NULL;
	m_pNetSession = NULL;

	// Set some default values
	m_iTimeOut = 3000;

	// Set some default values
	m_iFileSize = -1;
	m_sFileDate = _T("1970-01-01");
	m_sFileDateTime = _T("1970-01-01/00:00:00");
	m_sFileVersion = _T("0.0.0.0");

	return TRUE;
}

//=====================================================================

int CFtpFileObject::ExitInstance()
{
	// Always close file
	Close();

	// Call original function
	return CWinThread::ExitInstance();
}

//=====================================================================

void CFtpFileObject::SetParent(CWinThread * pParentThread)
{
	// Set parent
	m_pParent = pParentThread;
}

//=====================================================================

void CFtpFileObject::SetTimeOut(int iTimeOut)
{
	// Set value
	m_iTimeOut = iTimeOut;
}

//=====================================================================
	
bool CFtpFileObject::Open(CString sURL, CString sProxyUsername /* = "" */,
					 CString sProxyPassword /* = "" */,
					 CString sFtpUsername /* = "" */,
					 CString sFtpPassword /* = "" */)
{
	// If we are opened, close yourself first
	if (m_pFtpFile)
	{
		Close();
	}

	// Save information
	m_sURL = sURL;
	m_sProxyUsername = sProxyUsername;
	m_sProxyPassword = sProxyPassword;
	m_sFtpUsername = sFtpUsername;
	m_sFtpPassword = sFtpPassword;

	// Establish connection
	return EstablishConnection();
}

//=====================================================================

int CFtpFileObject::GetSize()
{
	// Return size
	return m_iFileSize;
}

//=====================================================================

CString CFtpFileObject::GetVersion()
{
	// Return version
	return m_sFileVersion;
}

//=====================================================================

CString CFtpFileObject::GetDate()
{
	// Return date
	return m_sFileDate;
}

//=====================================================================

CString CFtpFileObject::GetDateTime()
{
	// Return date
	return m_sFileDateTime;
}

//=====================================================================

void CFtpFileObject::Download(CString sDestination, bool bResumeIfPossible, 
								  HANDLE hEventStopDownloading)
{
	// Check if the file is open
	if (!m_pFtpFile)
	{
		// We can't download file
		return;
	}

	// Set options
	m_bResumeIfPossible = bResumeIfPossible;
	m_hEventStopDownloading = hEventStopDownloading;

	// Be sure the directory exists
	if (!PathFileExists(m_pPath->ExtractFilePath(sDestination)))
	{
		// It does not exist, create it
		CFunctions::Instance()->CreateFolder(m_pPath->ExtractFilePath(sDestination));
	}

	// RESUME IS NOT POSSIBLE WITH FTP FILES
	m_bResumeIfPossible = false;

	// Check if file already exists
	if (PathFileExists(sDestination))
	{
		// Set file attributes to normal so we are sure we are not dealing with a read-only file
		SetFileAttributes(sDestination, FILE_ATTRIBUTE_NORMAL);
	}

	try
	{
		// Set start reading byte to zero
		m_lStartReadingByte = 0;

		// Check if resume should be supported
		if (bResumeIfPossible)
		{
			// Check if resume is possible
			if (IsResumePossible(sDestination))
			{
				// Yes, create file with append options
				m_pDestinationFile = new CFile(sDestination, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeBinary);

				// Set start reading byte to size of new file
				m_lStartReadingByte = (int)m_pDestinationFile->SeekToEnd();
			}
			else
			{
				// Create the resume file
				CreateResumeFile(sDestination);

				// Create file
				m_pDestinationFile = new CFile(sDestination, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
			}
		}
		else
		{
			// Create file
			m_pDestinationFile = new CFile(sDestination, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		}
	}
	catch (CFileException * pEx)
	{
#if defined _DEBUG || defined _BETA
		pEx->ReportError();
#endif
		// Delete exception object
		pEx->Delete();

		// Send error report
		PostMessageToParent(WMU_ERROR, ERROR_DOWNLOADFILE, 0);
		
		// Exit
		return;
	}

	// Send yourself message to start
	PostThreadMessage(WMU_DOWNLOAD_START, 0, 0);
}

//=====================================================================

void CFtpFileObject::Close()
{
	// Close file
	if (m_pFtpFile)
	{
		m_pFtpFile->Close();
		delete m_pFtpFile;
	}

	// Close ftp connection
	if (m_pFtpConnection)
	{
		m_pFtpConnection->Close();
		delete m_pFtpConnection;
	}

	// Close internet session
	if (m_pNetSession)
	{
		m_pNetSession->Close();
		delete m_pNetSession;
	}

	// Set some default values
	m_iFileSize = -1;
	m_sFileDate = _T("1970-01-01");
	m_sFileDateTime = _T("1970-01-01/00:00:00");
	m_sFileVersion = _T("0.0.0.0");

	// Always set pointers to NULL
	m_pFtpFile = NULL;
	m_pFtpConnection = NULL;
	m_pNetSession = NULL;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

bool CFtpFileObject::EstablishConnection()
{
	// Declare variables
	ProxyData proxyInfo;
	CString sLog, sTemp;
	CString sServer, sObject, sProxy, sAuthentication;
	INTERNET_PORT nPort;
	DWORD dwServiceType;

	// Clean up old connection object
	if (m_pNetSession)
	{
		// Close internet session
		m_pNetSession->Close();

		// Delete internet session
		delete m_pNetSession;
	}

	// Set retry count to zero
	m_iProxyRetryCount = 0;

	// Should we enable proxy support or not?
	/*if (m_bEnableProxy)
	{
		// Get proxy information
		CProxy::GetProxyInfo(m_sURL, &proxyInfo);

		// Log
		sLog.Format("PROXY_INFO: connection type => %d", proxyInfo.dwAccessType);
		m_pLog->Log(sLog);
		sLog.Format("PROXY_INFO: proxy server => %s", proxyInfo.sProxyName);
		m_pLog->Log(sLog);
		sLog.Format(_T("PROXY_INFO: bypass list => %s"), proxyInfo.sProxyBypass);
		m_pLog->Log(sLog);

		try
		{
			// Create connection with known info
			m_pNetSession = new CInternetSession(NULL, 1, proxyInfo.dwAccessType,
												 proxyInfo.sProxyName,
												 proxyInfo.sProxyBypass);
		}
		catch (CException * pEx)
		{
			// Delete error
			pEx->Delete();

			// Log
			m_pLog->Log(_T("PROXY_INFO: no proxy used"));

			// Connect without proxy support
			m_pNetSession = new CInternetSession();
		}
	}
	else
	{
		// Log
		m_pLog->Log(_T("PROXY_INFO: proxy is disabled by user"));

		// Connect without proxy support
		m_pNetSession = new CInternetSession();
	}*/

	// Connect without proxy support
	m_pNetSession = new CInternetSession(NULL, 1, PRE_CONFIG_INTERNET_ACCESS);

	// Set timeout
	m_pNetSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, m_iTimeOut);
	m_pNetSession->SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, m_iTimeOut);

	// Set retry count (3 retries)
	m_pNetSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 3);

	// Parse URL to get all data
	if (!AfxParseURL(m_sURL, dwServiceType, sServer, sObject, nPort))
		return false;

	// Try to get the file
	try
	{
		// Get connection
		m_pFtpConnection = m_pNetSession->GetFtpConnection(sServer, m_sFtpUsername, m_sFtpPassword,
			nPort);

		// Set current directory
		CString sPath = m_pPath->ExtractFilePath(sObject, _T("/"));
		if (!m_pFtpConnection->SetCurrentDirectory(sPath))
		{
			// Error, clear current connection
			m_pNetSession->Close();
			return false;
		}

		// Get all file information
		GetFileInformation(m_pPath->ExtractFileName(m_sURL, _T("/")));

		// Get file object
		m_pFtpFile = m_pFtpConnection->OpenFile(m_pPath->ExtractFileName(m_sURL, _T("/")), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY);

		/*
		TO-DO: Add proxy authentication

		// Should we check proxy authentication?
		if (m_bEnableProxy)
		{
			// Yes
			HandleProxyServer();
		}

		*/

		// We successfully opened the file
		return true;
	}
	catch (CException * pEx)
	{
		// Delete
		pEx->Delete();

		// Return NULL
		return false;
	}	

	return true;
}

//=====================================================================

void CFtpFileObject::HandleProxyServer()
{
	// Declare variables
	//DWORD dwStatusCode;
	CAuthenticationDlg dlgAuthentication;
	CSettings * pSettings = CSettings::Instance();
	//TCHAR szUsername[1024];
	//TCHAR szPassword[1024];

	// Get Ftp file object
	/*CFtpFile * pFile = (CFtpFile *)m_pFtpFile;

	// Send empty request
	pFile->SendRequest();

	// Check the status code
	pFile->QueryInfoStatusCode(dwStatusCode);

	// Check if we should use authentication
	while ((dwStatusCode == Ftp_STATUS_PROXY_AUTH_REQ) &&
		   (m_iProxyRetryCount <= PROXY_RETRYCOUNT))
	{
		// Show screen to user, if this is the first time, try last username and password
		if (m_iProxyRetryCount != 0)
			dlgAuthentication.DoModal();

		// We tried (again)
		m_iProxyRetryCount++;

		// Copy the username and password to buffers
		strcpy(szUsername, pSettings->GetProxyUsername());
		strcpy(szPassword, pSettings->GetProxyPassword());

		// Set username and password
		if (strlen(szUsername) > 0)
			pFile->SetOption(INTERNET_OPTION_PROXY_USERNAME, szUsername, strlen(szUsername));
		if (strlen(szPassword) > 0)
			pFile->SetOption(INTERNET_OPTION_PROXY_PASSWORD, szPassword, strlen(szPassword));

		// Send request again
		pFile->SendRequest();

		// Check the status code
		pFile->QueryInfoStatusCode(dwStatusCode);
	}*/	
}

//=====================================================================

void CFtpFileObject::GetFileInformation(CString sObject)
{
	// Declare variables
	CTime dtmFile;

	// Set default values
	m_iFileSize = -1;
	m_sFileDate = _T("1970-01-01");
	m_sFileDateTime = _T("1970-01-01/00:00:00");
	m_sFileVersion = _T("0.0.0.0");

	// Check if we have a valid ftp connection
	if (!m_pFtpConnection)
		return;

	// Get file find object
	CFtpFileFind oFileFind(m_pFtpConnection);

	// Try to search for the file
	if (oFileFind.FindFile(sObject))
	{
		// Find next
		oFileFind.FindNextFile();

		// Now get all the information
		m_iFileSize = static_cast<int>(oFileFind.GetLength());
		oFileFind.GetLastWriteTime(dtmFile);

		// Format date
		m_sFileDate.Format(_T("%04d-%02d-%02d"), dtmFile.GetYear(),
			dtmFile.GetMonth(), dtmFile.GetDay());

		// Format date & time
		m_sFileDateTime.Format(_T("%04d-%02d-%02d/%02d:%02d:%02d"), dtmFile.GetYear(),
			dtmFile.GetMonth(), dtmFile.GetDay(),
			dtmFile.GetHour(), dtmFile.GetMinute(),
			dtmFile.GetSecond());
	}

	// Close the filefinder
	oFileFind.Close();
}


//=====================================================================

void CFtpFileObject::DownloadFile()
{
	// Declare variables
	char szBuffer[BUFFER_DOWNLOADFILE];
	int iFileBytes, iFileBytesCopied, iBytesRead;
	int iPreviousFileProgress = -1, iPreviousTotalProgress = -1;
	CFileStatus fileStatusLocation, fileStatusDestination;
	CString sNewFilePath;

	// Can we start?
	if (m_pDestinationFile == NULL)
	{
		// Send error message
		PostMessageToParent(WMU_ERROR, ERROR_DOWNLOADFILE, 0);
		
		// Exit this thread
		PostThreadMessage(WM_QUIT, 0, 0);
	}

	// Use try to catch errors
	try
	{		
		// Get file date/time of original file
		m_pFtpFile->GetStatus(fileStatusLocation);

		// Seek to end
		iFileBytes = m_iFileSize;

		// Check if we can skip file
		if (iFileBytes != m_lStartReadingByte)
		{
			// Move cursor back to start byte
			//m_pFtpFile->Seek(m_lStartReadingByte, CFile::begin);

			// Set bytes copied (is needed if resume is enabled)
			iFileBytesCopied = m_lStartReadingByte;

			// Move cursor of write file to end of file
			m_pDestinationFile->SeekToEnd();

			// Read from file
			while (iBytesRead = m_pFtpFile->Read(szBuffer, BUFFER_DOWNLOADFILE))
			{
				// Write actual data into file
				m_pDestinationFile->Write(szBuffer, iBytesRead); 
				
				// Update copied bytes
				iFileBytesCopied += iBytesRead;

				// Send copy status to parent
				SendDownloadStatus(iFileBytesCopied, iFileBytes);

				// Check if we are allowed to read again
				if (WaitForSingleObject(m_hEventStopDownloading, 0) == WAIT_OBJECT_0)
				{
					// No, stop download
					return;
				}
			}
		}

		// Get new file path
		sNewFilePath = m_pDestinationFile->GetFilePath();
		
		// Set file date/time of original file
		m_pDestinationFile->GetStatus(fileStatusDestination);
		m_pDestinationFile->Close();
	}
	// If there where any errors, catch them
	catch (CException * pEx)
	{
#if defined _DEBUG || defined _BETA
		pEx->ReportError();
#endif

		// Delete exception object to prevent leaks
		pEx->Delete();
		
		// Post error
		PostMessageToParent(WMU_ERROR, ERROR_DOWNLOADFILE, 0);
	}

	// Try to set filestatus
	try
	{
		fileStatusDestination.m_atime = fileStatusLocation.m_atime;
		fileStatusDestination.m_ctime = fileStatusLocation.m_ctime;
		fileStatusDestination.m_mtime = fileStatusLocation.m_mtime;
		CFile::SetStatus(sNewFilePath, fileStatusDestination);
	}
	catch (CException * pEx)
	{
		// Not important
		pEx->Delete();
	}
}

//=====================================================================

bool CFtpFileObject::IsResumePossible(CString sDestination)
{
	// Declare variables
	CString sResumeFile, sResumeInfo, sTemp = _T("");
	CFile * pResumeFile;
	CFileStatus fsOriginalFile;
	int iFileSize;

	// Set up resume file path
	sResumeFile = sDestination + _T(".rsm");

	// Check if we can find any part of the file on harddisk
	if (!PathFileExists(sDestination))
		return false;

	// Check if we can find resume file
	if (!PathFileExists(sResumeFile))
		return false;

	try
	{
		// Open resume file
		pResumeFile = new CFile(sResumeFile, CFile::modeRead | CFile::typeBinary);

		// Get file size
		iFileSize = (int)pResumeFile->SeekToEnd();
		pResumeFile->SeekToBegin();

		// Read value from resume file
		pResumeFile->Read(sTemp.GetBuffer(iFileSize), iFileSize);
		sResumeInfo = sTemp.GetBuffer(iFileSize);
		//sTemp.ReleaseBuffer();

		// Close resume file
		pResumeFile->Close();

		// Delete memory for file
		delete pResumeFile;
	}
	catch (CException * pEx)
	{
		pEx->Delete();
		return false;
	}

	// Get file status of original file
	m_pFtpFile->GetStatus(fsOriginalFile);

	// First check if we are dealing with a default date
	if (sResumeInfo == TIMECOMPARE_DEFAULT)
		return false;

	// Compare current time with saved time
	if (fsOriginalFile.m_mtime.Format(TIMECOMPARE) != sResumeInfo)
		return false;

	// If we get here, resume is possible
	return true;
}

//=====================================================================

void CFtpFileObject::CreateResumeFile(CString sDestination)
{
	// Declare variables
	CString sResumeFile;
	CFile * pResumeFile;
	CFileStatus fsOriginalFile;

	// Set up resume file path
	sResumeFile = sDestination + _T(".rsm");

	// Get file status of original file
	m_pFtpFile->GetStatus(fsOriginalFile);

	try
	{
		// Open resume file
		pResumeFile = new CFile(sResumeFile, CFile::modeCreate | CFile::modeWrite);

		// Write value to resume file
		pResumeFile->Write(fsOriginalFile.m_mtime.Format(TIMECOMPARE),
						   fsOriginalFile.m_mtime.Format(TIMECOMPARE).GetLength());

		// Close resume file
		pResumeFile->Close();

		// Delete memory for file
		delete pResumeFile;
	}
	catch (CException * pEx)
	{
		pEx->Delete();
	}
}

//=====================================================================

void CFtpFileObject::SendDownloadStatus(int iBytesCopied, int iTotalBytes)
{
	// Declare variables
	int iPercentageCompleted, iCurrentActionPoints, iTemp;

	// Calculate current percentage completed
	if (iBytesCopied > INT_MAX / 100)
	{
		// Size is getting too large, go a few steps back
		iTemp = iBytesCopied / 10000;

		// Calculate it
		iPercentageCompleted = iTemp / (iTotalBytes / 1000000);
	}
	else
	{
		// Calculate percentage on a normal way
		iPercentageCompleted = (iBytesCopied * 100) / iTotalBytes;
	}
	
	// Because sometimes we are not very accurate
	if (iPercentageCompleted > 100)
	{
		iPercentageCompleted = 100;
	}

	// Calculate current actionpoints completed
	iCurrentActionPoints = (iPercentageCompleted * m_iActionPoints) / 100;

	// Send information to parent
	if (m_pParent && (m_iCurrentActionPoints != iCurrentActionPoints))
	{
		m_iCurrentActionPoints = iCurrentActionPoints;
		
		// Send status
		PostMessageToParent(WMU_DOWNLOAD_STATUS, m_iCurrentActionPoints, m_iActionPoints);	
	}
}

//=====================================================================

void CFtpFileObject::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

CString CFtpFileObject::ExtractFilePath(CString sFilename)
{
	// Declare variables
	int iPos;
	
	// Now search for the last backslash
	for (int i = sFilename.GetLength(); i > 0; i--)
	{
		iPos = sFilename.Find(_T('\\'), sFilename.GetLength()-(sFilename.GetLength()-i));
		// If we found them, delete last piece
		if (iPos != -1)
		{
			sFilename.Delete(iPos, sFilename.GetLength()-i);
			return sFilename;
		}
	}
	
	// If we not found, we are already dealing with path
	return sFilename;
}

//=====================================================================

bool CFtpFileObject::CreateFolder(CString sFolder)
{
	// Declare variables
	DWORD dwAttrib = GetFileAttributes(sFolder);
	CString sTemp;
	
	// Is folder already existing?
	if (PathFileExists(sFolder))
		return true;
	
	// Recursively create from the top down
	int iPos = sFolder.ReverseFind(_T('\\'));
	
	if (iPos != -1) 
	{
		// The parent is a dir, not a drive
		sTemp = sFolder;
		sTemp.Delete(iPos, sFolder.GetLength() - iPos);
		
		// if can't create parent
		if (!CreateFolder(sTemp))
		{
			return false;
		}
		
		// Check if last character is a backslash
		if (sTemp.GetAt(sTemp.GetLength() - 1) == _T('\\'))
			return true;
		
		// Check if path exists
		if (PathFileExists(sFolder))
			return true;

		// Try to create directory
		if (!::CreateDirectory(sFolder, NULL)) 
			return false;
	}
	
	// If we get here, function is successful
	return true;
}

//=====================================================================

void CFtpFileObject::OnStartDownload(WPARAM wParam, LPARAM lParam)
{
	// Init action points
	m_iCurrentActionPoints = 0;
	m_iActionPoints = CActionPoints::CalculateActionPoints(m_iFileSize);

	// Start the copy process
	DownloadFile();

	// Send message to parent that we are ready
	PostMessageToParent(WMU_DOWNLOAD_COMPLETE, 0, 0);
}
