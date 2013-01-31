/********************************************************************
	created:	2005/03/24
	created:	24:3:2005   13:27
	filename: 	CopyFile.cpp
	file path:	Updater
	file base:	CopyFile
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "CopyFile.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CCopyFile, CWinThread)
	ON_THREAD_MESSAGE(WMU_COPY_START, OnStartCopy)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CCopyFile, CWinThread)

//=====================================================================

CCopyFile::CCopyFile()
{
}

//=====================================================================

CCopyFile::~CCopyFile()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CCopyFile::InitInstance()
{
	// Log
	CLog::Instance()->Log("CCopyFile::InitInstance");
	return TRUE;
}

//=====================================================================

int CCopyFile::ExitInstance()
{
	// Log
	CLog::Instance()->Log("CCopyFile::ExitInstance");

	// Delete objects
	if (m_pOriginalFile)
		delete m_pOriginalFile;

	if (m_pNewFile)
		delete m_pNewFile;

	// Auto delete
	m_bAutoDelete = TRUE;

	// Call original function
	return CWinThread::ExitInstance();
}

//=====================================================================

void CCopyFile::SetParent(CWinThread * pParentThread)
{
	// Set parent
	m_pParent = pParentThread;
}

//=====================================================================

void CCopyFile::StartFileCopy(CFile * pOriginalFile, CString sNewFile,
							  bool bSkipIfExists)
{
	// Set default values
	m_pNewFile = NULL;
	m_pOriginalFile = NULL;
	CPath * pPath = CPath::Instance();

	// Check if file exists
	if (PathFileExists(sNewFile))
	{
		// Should we fail now?
		if (bSkipIfExists)
		{
			// Don't really fail, send ready message
			PostMessageToParent(WMU_COPY_COMPLETE, 0, 0);

			// Exit this thread
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
	}

	// Be sure the directory exists
	if (!PathFileExists(pPath->ExtractFilePath(sNewFile)))
	{
		// It does not exist, create it
		CFunctions::Instance()->CreateFolder(pPath->ExtractFilePath(sNewFile));
	}

	// Set options
	m_pOriginalFile = pOriginalFile;
	m_bSkipIfExists = bSkipIfExists;

	// Check if file already exists
	if (PathFileExists(sNewFile))
	{
		// Set file attributes to normal so we are sure we are not dealing with a read-only file
		SetFileAttributes(sNewFile, FILE_ATTRIBUTE_NORMAL);
	}

	try
	{
		// Create file
		m_pNewFile = new CFile(sNewFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
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
		return;
	}

	// Send yourself message to start
	PostThreadMessage(WMU_COPY_START, 0, 0);
}

//=====================================================================	

void CCopyFile::StartFileCopy(CString sOriginalFile, CString sNewFile,
							  bool bSkipIfExists)
{
	USES_CONVERSION;

	// Set default values
	m_pNewFile = NULL;
	m_pOriginalFile = NULL;
	CPath * pPath = CPath::Instance();

	// Check if file exists
	if (PathFileExists(sNewFile))
	{
		// Should we fail now?
		if (bSkipIfExists)
		{
			// Don't really fail, send ready message
			PostMessageToParent(WMU_COPY_COMPLETE, 0, 0);

			// Exit this thread
			PostThreadMessage(WM_QUIT, 0, 0);
			return;
		}
	}

	// Be sure the directory exists
	if (!PathFileExists(pPath->ExtractFilePath(sNewFile)))
	{
		// It does not exist, create it
		CFunctions::Instance()->CreateFolder(pPath->ExtractFilePath(sNewFile));
	}

	// Check if file exists
	if (!PathFileExists(sOriginalFile))
	{
		// Send error report
		PostMessageToParent(WMU_ERROR, ERROR_FILENOTFOUND, 0);
		// Exit
		return;
	}

	// Get original file
	try
	{
		m_pOriginalFile = new CFile(sOriginalFile, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary);
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
		return;
	}

	// Be sure the folder exists
	if (!CreateFolder(W2A(ExtractFilePath(sNewFile))))
	{
		// Send error report
		PostMessageToParent(WMU_ERROR, ERROR_WRITEFILE, 0);
		// Exit
		return;
	}

	// Check if file already exists
	if (PathFileExists(sNewFile))
	{
		// Set file attributes to normal so we are sure we are not dealing with a read-only file
		SetFileAttributes(sNewFile, FILE_ATTRIBUTE_NORMAL);
	}

	try
	{
		// Create file
		m_pNewFile = new CFile(sNewFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
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
		return;
	}

	// Set options
	m_bSkipIfExists = bSkipIfExists;

	// Send yourself message to start
	PostThreadMessage(WMU_COPY_START, 0, 0);
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CCopyFile::Copy(CFile * pOriginalFile, CFile * pNewFile,
					 bool bSkipIfExists)
{
	// Declare variables
	char szBuffer[BUFFER_COPYFILE];
	int iFileBytes, iFileBytesCopied, iBytesRead;
	int iPreviousFileProgress = -1, iPreviousTotalProgress = -1;
	CFileStatus fileStatusLocation, fileStatusDestination;
	CString sNewFilePath;

	// Can we start?
	if (pNewFile == NULL)
	{
		// Send error message
		PostMessageToParent(WMU_ERROR, ERROR_COPYFILE, 0);
		
		// Exit this thread
		PostThreadMessage(WM_QUIT, 0, 0);
	}

	// Use try to catch errors
	try
	{		
		// Get file date/time of original file
		pOriginalFile->GetStatus(fileStatusLocation);

		// Seek to end
		iFileBytes =  static_cast<int>(pOriginalFile->SeekToEnd());
		
		// Move cursor back to start byte
		pOriginalFile->Seek(0, CFile::begin);

		// Set bytes copied (is needed if resume is enabled)
		iFileBytesCopied = 0;

		// Move cursor of write file to end of file
		pNewFile->SeekToEnd();

		// Read from file
		while (iBytesRead = pOriginalFile->Read(szBuffer, BUFFER_COPYFILE))
		{
			// Write actual data into file
			pNewFile->Write(szBuffer, iBytesRead); 
			
			// Update copied bytes
			iFileBytesCopied += iBytesRead;

			// Send copy status to parent
			SendCopyStatus(iFileBytesCopied, iFileBytes);
		}
		
		// Get new file path
		sNewFilePath = pNewFile->GetFilePath();

		// Set file date/time of original file
		pNewFile->GetStatus(fileStatusDestination);
		pNewFile->Close();

		// Copy ended, close file
		pOriginalFile->Close();
	}
	// If there where any errors, catch them
	catch (CException * pEx)
	{
#ifdef _DEBUG
		pEx->ReportError();
#endif

		// Delete exception object to prevent leaks
		pEx->Delete();
		
		// Post error
		PostMessageToParent(WMU_ERROR, ERROR_COPYFILE, 0);
	}

	// Try to set file status
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

CString CCopyFile::ExtractFilePath(CString sFilename)
{
	// Declare variables
	int iPos;
	
	// Now search for the last backslash
	for (int i = sFilename.GetLength(); i > 0; i--)
	{
		iPos = sFilename.Find('\\', sFilename.GetLength()-(sFilename.GetLength()-i));
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

bool CCopyFile::CreateFolder(CString sFolder)
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

void CCopyFile::SendCopyStatus(int iBytesCopied, int iTotalBytes)
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
		PostMessageToParent(WMU_COPY_STATUS, m_iCurrentActionPoints, m_iActionPoints);	
	}
}

//=====================================================================

void CCopyFile::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

void CCopyFile::OnStartCopy(WPARAM wParam, LPARAM lParam)
{
	// Init action points
	m_iCurrentActionPoints = 0;

	// Dangerous code
	try
	{
		// Calculate action points
		m_iActionPoints = CActionPoints::CalculateActionPoints(static_cast<int>(m_pOriginalFile->SeekToEnd()));

		// Start the copy process
		Copy(m_pOriginalFile, m_pNewFile, m_bSkipIfExists);

		// Send message to parent that we are ready
		PostMessageToParent(WMU_COPY_COMPLETE, 0, 0);
	}
	catch (...)
	{
		// Send error message
		PostMessageToParent(WMU_ERROR, ERROR_COPYFILE, 0);
	}
}
