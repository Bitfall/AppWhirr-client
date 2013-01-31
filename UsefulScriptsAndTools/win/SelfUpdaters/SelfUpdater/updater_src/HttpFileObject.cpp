/********************************************************************
	created:	2005/10/31
	created:	31:10:2005   9:46
	filename: 	Http.cpp
	file path:	Updater
	file base:	HttpFileObject
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "HttpFileObject.h"
#include "AuthenticationDlg.h"			// Proxy authentication
#include <wininet.h>

const CString TIMECOMPARE = _T("%Y%m%d%H%M%S");
const CString TIMECOMPARE_DEFAULT = _T("19700101000000");

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CHttpFileObject, CWinThread)
	ON_THREAD_MESSAGE(WMU_DOWNLOAD_START, OnStartDownload)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CHttpFileObject, CWinThread)

//=====================================================================

CHttpFileObject::CHttpFileObject()
{
}

//=====================================================================

CHttpFileObject::~CHttpFileObject()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CHttpFileObject::InitInstance()
{	
	// Initialize singleton classes
	m_pLog = CLog::Instance();

	// Set the pointers to NULL
	m_pHttpFile = NULL;
	m_pNetSession = NULL;

	// Set some default values
	m_iTimeOut = 3000;

	// Get proxy information
	m_pProxy = CProxy::Instance();

	return TRUE;
}

//=====================================================================

int CHttpFileObject::ExitInstance()
{
	// Always close file
	Close();

	// Call original function
	return CWinThread::ExitInstance();
}

//=====================================================================

void CHttpFileObject::SetParent(CWinThread * pParentThread)
{
	// Set parent
	m_pParent = pParentThread;
}

//=====================================================================

void CHttpFileObject::SetTimeOut(int iTimeOut)
{
	// Set value
	m_iTimeOut = iTimeOut;
}

//=====================================================================
	
bool CHttpFileObject::Open(CString sURL, CString sProxyUsername /* = "" */,
					 CString sProxyPassword /* = "" */,
					 CString sHttpUsername /* = "" */,
					 CString sHttpPassword /* = "" */)
{
	// If we are opened, close yourself first
	if (m_pHttpFile)
	{
		Close();
	}

	// Save information
	m_sURL = sURL;
	m_sProxyUsername = sProxyUsername;
	m_sProxyPassword = sProxyPassword;
	m_sHttpUsername = sHttpUsername;
	m_sHttpPassword = sHttpPassword;

	// Establish connection
	return EstablishConnection();
}

//=====================================================================

int CHttpFileObject::GetSize()
{
	// Declare variables
	int iSize = -1;

	// Check if the file is open
	if (!m_pHttpFile)
	{
		// We can't get size
		return iSize;
	}

	// Try to get filesize
	try
	{
		// Seek to end
		iSize = static_cast<int>(m_pHttpFile->SeekToEnd());
	}
	catch (CException * pEx)
	{
#if defined _DEBUG || defined _BETA
		//pEx->ReportError();
#endif

		// Delete exception object to prevent leaks
		pEx->Delete();
	}

	// Return size
	return iSize;
}

//=====================================================================

CString CHttpFileObject::GetVersion()
{
	// Declare variables
	CString sVersion = _T("0.0.0.0");

	// Check if the file is open
	if (!m_pHttpFile)
	{
		// We can't get version
		return sVersion;
	}

	// Not possible for internet files (yet)
	return sVersion;
}

//=====================================================================

CString CHttpFileObject::GetDate()
{
	// Declare variables
	CString sDate = _T("1970-01-01");
	CFileStatus fileStatus;

	// Check if the file is open
	if (!m_pHttpFile)
	{
		// We can't get date
		return sDate;
	}

	// Try to get date
	try
	{
		// Get file status
		m_pHttpFile->GetStatus(fileStatus);

		// Get date
		sDate.Format(_T("%04d-%02d-%02d"), fileStatus.m_mtime.GetYear(),
			fileStatus.m_mtime.GetMonth(), fileStatus.m_mtime.GetDay());
	}
	catch (CException * pEx)
	{
#if defined _DEBUG || defined _BETA
		//pEx->ReportError();
#endif

		// Delete exception object to prevent leaks
		pEx->Delete();
	}

	// Return date
	return sDate;
}

//=====================================================================

CString CHttpFileObject::GetDateTime()
{
	// Declare variables
	CString sDateTime = _T("1970-01-01/00:00:00");
	CFileStatus fileStatus;

	// Check if the file is open
	if (!m_pHttpFile)
	{
		// We can't get date
		return sDateTime;
	}

	// Try to get date
	try
	{
		// Get file status
		m_pHttpFile->GetStatus(fileStatus);

		// Get date
		sDateTime.Format(_T("%04d-%02d-%02d/%02d:%02d:%02d"), fileStatus.m_mtime.GetYear(),
			fileStatus.m_mtime.GetMonth(), fileStatus.m_mtime.GetDay(),
			fileStatus.m_mtime.GetHour(), fileStatus.m_mtime.GetMinute(),
			fileStatus.m_mtime.GetSecond());
	}
	catch (CException * pEx)
	{
#if defined _DEBUG || defined _BETA
		//pEx->ReportError();
#endif

		// Delete exception object to prevent leaks
		pEx->Delete();
	}
	
	// Return date
	return sDateTime;
}

//=====================================================================

void CHttpFileObject::Download(CString sDestination, bool bResumeIfPossible, 
								  HANDLE hEventStopDownloading)
{
	// Check if the file is open
	if (!m_pHttpFile)
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
		//pEx->ReportError();
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

void CHttpFileObject::Close()
{
	// Close file
	if (m_pHttpFile)
	{
		m_pHttpFile->Close();
		delete m_pHttpFile;
	}

	// Close internet session
	if (m_pNetSession)
	{
		m_pNetSession->Close();
		delete m_pNetSession;
	}

	// Always set pointers to NULL
	m_pHttpFile = NULL;
	m_pNetSession = NULL;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

bool CHttpFileObject::EstablishConnection()
{
	// Declare variables
	CString sLog, sTemp;
	CHttpConnection * pConnection;
	ProxyData proxyInfo;
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

	// Get proxy information
	m_pProxy->GetProxyInfo(m_sURL, &proxyInfo);

	// Log
	sLog.Format(_T("PROXY_INFO: connection type => %d"), proxyInfo.dwAccessType);
	m_pLog->Log(sLog);
	sLog.Format(_T("PROXY_INFO: proxy server => %s"), proxyInfo.sProxyName);
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
		// Declare variables
		TCHAR szCause[255];
		CString strFormatted;

		// Get error message
		pEx->GetErrorMessage(szCause, 255);

		// Delete error
		pEx->Delete();

		// Log
		sLog.Format(_T("PROXY_INFO: no proxy used because of exception %s"), szCause);
		m_pLog->Log(sLog);

		// Connect without proxy support
		m_pNetSession = new CInternetSession();
	}

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
		pConnection = m_pNetSession->GetHttpConnection(sServer, nPort, m_sHttpUsername, m_sHttpPassword);

		// Set up flags
        unsigned long ulFlags = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_RELOAD;
        if (dwServiceType == AFX_INET_SERVICE_HTTPS)
        {
            ulFlags = ulFlags | INTERNET_FLAG_SECURE;
        }

		// Get the file
		m_pHttpFile = pConnection->OpenRequest(HTTP_VERB_GET, sObject, NULL, 1, NULL, NULL, ulFlags);
			
		// Handle request (and proxy settings)
        ProcessHttpRequest((CHttpFile*)m_pHttpFile);

		// Return the pointer to the file
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

void CHttpFileObject::ProcessHttpRequest(CHttpFile * pFile)
{
	// Declare variables
    bool bRetry = false;

	// Try to process http request
    do 
    {
        try
        {
	        // Send empty request
	        pFile->SendRequest();

            // Process the response.
            bRetry = ProcessHttpResponse(pFile);
		}
		catch (CException* pEx)
		{
            // Process the HTTP exception.
            bRetry = ProcessHttpException(pFile, pEx);

            // Delete the exception as not going to get propagated.
            pEx->Delete();
        }
    }
    while (bRetry && m_iProxyRetryCount++ <= PROXY_RETRYCOUNT);
}

//=====================================================================

bool CHttpFileObject::ProcessHttpResponse(CHttpFile * pFile)
{
	// Declare variables
    bool bReturn = false;
	DWORD dwStatusCode = 0;

	// Check the status code
	pFile->QueryInfoStatusCode(dwStatusCode);

    // Check if we should use authentication
	if (dwStatusCode == HTTP_STATUS_PROXY_AUTH_REQ)
    {
		// Get the interface to setttings
	    CSettings * pSettings = CSettings::Instance();

		// Copy the username and password to buffers
	    TCHAR szUsername[1024];
	    TCHAR szPassword[1024];
		lstrcpy(szUsername, pSettings->GetProxyUsername());
		lstrcpy(szPassword, pSettings->GetProxyPassword());

		// Set username and password
        if (pSettings->GetRunmode() != MODE_HIDDEN)
        {
		    if (lstrlen(szUsername) == 0 || lstrlen(szPassword) == 0)
            {
                // Prompt for proxy auth credentials from user.
			    CAuthenticationDlg dlgAuthentication;
                dlgAuthentication.DoModal();

		        // Copy the username and password to buffers
		        lstrcpy(szUsername, pSettings->GetProxyUsername());
		        lstrcpy(szPassword, pSettings->GetProxyPassword());
            }
        }

		// Set username and password
		if (lstrlen(szUsername) > 0 && lstrlen(szPassword) > 0)
        {
            // Set the proxy auth credentials.
			pFile->SetOption(INTERNET_OPTION_PROXY_USERNAME, szUsername, lstrlen(szUsername));
			pFile->SetOption(INTERNET_OPTION_PROXY_PASSWORD, szPassword, lstrlen(szPassword));

            // Retry sending the request with required piece of information.
            bReturn = true;
        }
    }

	// Return result
    return bReturn;
}

//=====================================================================

bool CHttpFileObject::ProcessHttpException(CHttpFile * pFile, CException * pEx)
{
	// Declare variables
    bool bReturn = false;
	DWORD dwFlags;

	// Get the error
    DWORD dwLastError = ((CInternetException*)pEx)->m_dwError;

	// Check if there is a valid exception
    if (pEx && pEx->IsKindOf(RUNTIME_CLASS(CInternetException)))
    {
        // TypeCast
        CInternetException * pException = (CInternetException*) pEx;

 	    // Get the error
        DWORD dwLastError = pException->m_dwError;

		switch (dwLastError)
		{
		case ERROR_INTERNET_INVALID_CA:
		    // Ignore security errors
            dwFlags = SECURITY_FLAG_IGNORE_CERT_CN_INVALID  | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_REVOCATION | SECURITY_FLAG_IGNORE_WRONG_USAGE;
	        pFile->SetOption(INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, sizeof(dwFlags));

			// Retry
            bReturn = true;
			break;

		case ERROR_INTERNET_INVALID_URL:
			// Don't retry
			bReturn = false;
			break;

		case ERROR_INTERNET_INCORRECT_USER_NAME:
			// Try again with new username
			bReturn = true;
			break;

		case ERROR_INTERNET_INCORRECT_PASSWORD:
			// Try again with new password
			bReturn = true;
			break;

		case ERROR_INTERNET_LOGIN_FAILURE:
			// Try again with new username & password
			bReturn = true;
			break;

		default:
			// Just try again
			bReturn = true;
			break;
		}
    }

	// Return result
    return bReturn;
}

//=====================================================================

void CHttpFileObject::DownloadFile()
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
		m_pHttpFile->GetStatus(fileStatusLocation);

		// Seek to end
		iFileBytes = static_cast<int>(m_pHttpFile->SeekToEnd());

		// Check if we can skip file
		if (iFileBytes != m_lStartReadingByte)
		{
			// Move cursor back to start byte
			m_pHttpFile->Seek(m_lStartReadingByte, CFile::begin);

			// Set bytes copied (is needed if resume is enabled)
			iFileBytesCopied = m_lStartReadingByte;

			// Move cursor of write file to end of file
			m_pDestinationFile->SeekToEnd();

			// Read from file
			while (iBytesRead = m_pHttpFile->Read(szBuffer, BUFFER_DOWNLOADFILE))
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
	//	pEx->ReportError();
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

bool CHttpFileObject::IsResumePossible(CString sDestination)
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
		pResumeFile = new CFile(sResumeFile, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone);

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
	m_pHttpFile->GetStatus(fsOriginalFile);

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

void CHttpFileObject::CreateResumeFile(CString sDestination)
{
	// Declare variables
	CString sResumeFile;
	CFile * pResumeFile;
	CFileStatus fsOriginalFile;

	// Set up resume file path
	sResumeFile = sDestination + _T(".rsm");

	// Get file status of original file
	m_pHttpFile->GetStatus(fsOriginalFile);

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

void CHttpFileObject::SendDownloadStatus(int iBytesCopied, int iTotalBytes)
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

void CHttpFileObject::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

CString CHttpFileObject::ExtractFilePath(CString sFilename)
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

bool CHttpFileObject::CreateFolder(CString sFolder)
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

void CHttpFileObject::OnStartDownload(WPARAM wParam, LPARAM lParam)
{
	// Init action points
	m_iCurrentActionPoints = 0;
	DWORD dwStatusCode = 0;

	// Dangerous code
	try
	{
	    // Get status code
	    ((CHttpFile*) m_pHttpFile)->QueryInfoStatusCode(dwStatusCode);

		// Check if status code is ok
        if (dwStatusCode == HTTP_STATUS_OK)
        {
		    // Calculate action points
		    m_iActionPoints = CActionPoints::CalculateActionPoints(static_cast<int>(m_pHttpFile->SeekToEnd()));

		    // Start the copy process
		    DownloadFile();

		    // Send message to parent that we are ready
		    PostMessageToParent(WMU_DOWNLOAD_COMPLETE, 0, 0);
        }
        else
        {
			// We can't download the file
		    PostMessageToParent(WMU_ERROR, ERROR_DOWNLOADFILE, 0);
        }
	}
	catch (...)
	{
		// Send error message
		PostMessageToParent(WMU_ERROR, ERROR_DOWNLOADFILE, 0);
	}
}
