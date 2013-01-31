/********************************************************************
	created:	2005/10/31
	created:	31:10:2005   9:46
	filename: 	Network.cpp
	file path:	Updater
	file base:	NetworkFileObject
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "NetworkFileObject.h"
#include <wininet.h>

const CString TIMECOMPARE = _T("%Y%m%d%H%M%S");
const CString TIMECOMPARE_DEFAULT = _T("19700101000000");

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CNetworkFileObject, CWinThread)
	ON_THREAD_MESSAGE(WMU_DOWNLOAD_START, OnStartDownload)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CNetworkFileObject, CWinThread)

//=====================================================================

CNetworkFileObject::CNetworkFileObject()
{
}

//=====================================================================

CNetworkFileObject::~CNetworkFileObject()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CNetworkFileObject::InitInstance()
{
	// Initialize singleton classes
	m_pLog = CLog::Instance();

	// Set our filepointer to NULL
	m_pNetworkFile = NULL;

	return TRUE;
}

//=====================================================================

int CNetworkFileObject::ExitInstance()
{
	// Always close file
	Close();

	// Call original function
	return CWinThread::ExitInstance();
}

//=====================================================================

void CNetworkFileObject::SetParent(CWinThread * pParentThread)
{
	// Set parent
	m_pParent = pParentThread;
}

//=====================================================================

void CNetworkFileObject::SetTimeOut(int iTimeOut)
{
	// Set value
	m_iTimeOut = iTimeOut;
}

//=====================================================================
	
bool CNetworkFileObject::Open(CString sURL)
{
	// If we are opened, close yourself first
	if (m_pNetworkFile)
	{
		Close();
	}

	// Save information
	m_sURL = sURL;

	// Establish the connection
	return EstablishConnection();
}

//=====================================================================

int CNetworkFileObject::GetSize()
{
	// Declare variables
	int iSize = -1;

	// Check if the file is open
	if (!m_pNetworkFile)
	{
		// We can't get size
		return iSize;
	}

	// Try to get filesize
	try
	{
		// Seek to end
		iSize = static_cast<int>(m_pNetworkFile->SeekToEnd());
	}
	catch (CException * pEx)
	{
#if defined _DEBUG || defined _BETA
		pEx->ReportError();
#endif

		// Delete exception object to prevent leaks
		pEx->Delete();
	}

	// Return size
	return iSize;
}

//=====================================================================

CString CNetworkFileObject::GetVersion()
{
	// Declare variables
	CString sVersion = _T("0.0.0.0");

	// Check if the file is open
	if (!m_pNetworkFile)
	{
		// We can't get version
		return sVersion;
	}

	// Not possible for internet files (yet)
	return sVersion;
}

//=====================================================================

CString CNetworkFileObject::GetDate()
{
	// Declare variables
	CString sDate = _T("1970-01-01");
	CFileStatus fileStatus;

	// Check if the file is open
	if (!m_pNetworkFile)
	{
		// We can't get date
		return sDate;
	}

	// Try to get date
	try
	{
		// Get file status
		m_pNetworkFile->GetStatus(fileStatus);

		// Get date
		sDate.Format(_T("%04d-%02d-%02d"), fileStatus.m_mtime.GetYear(),
			fileStatus.m_mtime.GetMonth(), fileStatus.m_mtime.GetDay());
	}
	catch (CException * pEx)
	{
#if defined _DEBUG || defined _BETA
		pEx->ReportError();
#endif

		// Delete exception object to prevent leaks
		pEx->Delete();
	}

	// Return date
	return sDate;
}

//=====================================================================

CString CNetworkFileObject::GetDateTime()
{
	// Declare variables
	CString sDateTime = _T("1970-01-01/00:00:00");
	CFileStatus fileStatus;

	// Check if the file is open
	if (!m_pNetworkFile)
	{
		// We can't get date
		return sDateTime;
	}

	// Try to get date
	try
	{
		// Get file status
		m_pNetworkFile->GetStatus(fileStatus);

		// Get date
		sDateTime.Format(_T("%04d-%02d-%02d/%02d:%02d:%02d"), fileStatus.m_mtime.GetYear(),
			fileStatus.m_mtime.GetMonth(), fileStatus.m_mtime.GetDay(),
			fileStatus.m_mtime.GetHour(), fileStatus.m_mtime.GetMinute(),
			fileStatus.m_mtime.GetSecond());
	}
	catch (CException * pEx)
	{
#if defined _DEBUG || defined _BETA
		pEx->ReportError();
#endif

		// Delete exception object to prevent leaks
		pEx->Delete();
	}
	
	// Return date
	return sDateTime;
}

//=====================================================================

void CNetworkFileObject::Download(CString sDestination, bool bResumeIfPossible, 
								  HANDLE hEventStopDownloading)
{
	// Check if the file is open
	if (!m_pNetworkFile)
	{
		// Send error report
		PostMessageToParent(WMU_ERROR, ERROR_FILENOTFOUND, 0);

		// Exit function
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

void CNetworkFileObject::Close()
{
	// Close file
	if (m_pNetworkFile)
	{
		m_pNetworkFile->Close();
		delete m_pNetworkFile;
	}

	// Always set file pointer to NULL
	m_pNetworkFile = NULL;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

bool CNetworkFileObject::EstablishConnection()
{
	// Check if file exists
	if (!PathFileExists(m_sURL))
	{
		// Exit
		return false;
	}

	// Get original file
	try
	{
		m_pNetworkFile = new CFile(m_sURL, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone);
	}
	catch (CFileException * pEx)
	{
#if defined _DEBUG || defined _BETA
		pEx->ReportError();
#endif
		// Delete exception object
		pEx->Delete();

		// Send error report
		PostMessageToParent(WMU_ERROR, ERROR_COPYFILE, 0);

		// Exit
		return false;
	}

	// If we get here, connection is valid
	return true;
}

//=====================================================================

void CNetworkFileObject::DownloadFile()
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
		m_pNetworkFile->GetStatus(fileStatusLocation);

		// Seek to end
		iFileBytes =  static_cast<int>(m_pNetworkFile->SeekToEnd());
		
		// Check if we can skip file
		if (iFileBytes != m_lStartReadingByte)
		{
			// Move cursor back to start byte
			m_pNetworkFile->Seek(m_lStartReadingByte, CFile::begin);

			// Set bytes copied (is needed if resume is enabled)
			iFileBytesCopied = m_lStartReadingByte;

			// Move cursor of write file to end of file
			m_pDestinationFile->SeekToEnd();

			// Read from file
			while (iBytesRead = m_pNetworkFile->Read(szBuffer, BUFFER_DOWNLOADFILE))
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

bool CNetworkFileObject::IsResumePossible(CString sDestination)
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
	m_pNetworkFile->GetStatus(fsOriginalFile);

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

void CNetworkFileObject::CreateResumeFile(CString sDestination)
{
	// Declare variables
	CString sResumeFile;
	CFile * pResumeFile;
	CFileStatus fsOriginalFile;

	// Set up resume file path
	sResumeFile = sDestination + _T(".rsm");

	// Get file status of original file
	m_pNetworkFile->GetStatus(fsOriginalFile);

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

void CNetworkFileObject::SendDownloadStatus(int iBytesCopied, int iTotalBytes)
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

void CNetworkFileObject::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

CString CNetworkFileObject::ExtractFilePath(CString sFilename)
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

bool CNetworkFileObject::CreateFolder(CString sFolder)
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

void CNetworkFileObject::OnStartDownload(WPARAM wParam, LPARAM lParam)
{
	// Init action points
	m_iCurrentActionPoints = 0;
	m_iActionPoints = CActionPoints::CalculateActionPoints(static_cast<int>(m_pNetworkFile->GetLength()));

	// Start the copy process
	DownloadFile();

	// Send message to parent that we are ready
	PostMessageToParent(WMU_DOWNLOAD_COMPLETE, 0, 0);
}