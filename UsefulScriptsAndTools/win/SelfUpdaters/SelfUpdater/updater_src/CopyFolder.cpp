/********************************************************************
	created:	2005/04/01
	created:	1:4:2005   9:47
	filename: 	CopyFolder.cpp
	file path:	Updater
	file base:	CopyFolder
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "CopyFolder.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CCopyFolder, CWinThread)
	ON_THREAD_MESSAGE(WMU_COPY_START, OnCopyStart)
	ON_THREAD_MESSAGE(WMU_COPY_STATUS, OnCopyFileStatus)
	ON_THREAD_MESSAGE(WMU_COPY_COMPLETE, OnCopyFileComplete)
	ON_THREAD_MESSAGE(WMU_ERROR, OnError)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CCopyFolder, CWinThread)

//=====================================================================

CCopyFolder::CCopyFolder()
{
}

//=====================================================================

CCopyFolder::~CCopyFolder()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CCopyFolder::InitInstance()
{
	// Log
	CLog::Instance()->Log("CCopyFolder::InitInstance");
	return TRUE;
}

//=====================================================================

int CCopyFolder::ExitInstance()
{
	// Log
	CLog::Instance()->Log("CCopyFolder::ExitInstance");

	// If thread is running
	if (!IsBadReadPtr(m_pCopyThread, sizeof(CCopyFile *)))
	{
		// Stop it
		m_pCopyThread->PostThreadMessage(WM_QUIT, 0, 0);

		// Wait until thread has really ended
		WaitForSingleObject(m_pCopyThread->m_hThread, INFINITE);
	}	

	// Auto delete
	m_bAutoDelete = TRUE;

	// Call original function
	return CWinThread::ExitInstance();
}

//=====================================================================

void CCopyFolder::SetParent(CWinThread * pParentThread)
{
	// Set parent
	m_pParent = pParentThread;
}

//=====================================================================

void CCopyFolder::StartFolderCopy(CString sSource, CString sDestination, 
								  bool bFailIfExists)
{
	// Declare variables
	CFunctions * pFunctions = CFunctions::Instance();

	// Calculate action points
	m_iCurrentActionPoints = 0;
	m_iActionPoints = CActionPoints::CalculateActionPoints(sSource);

	// Set options
	m_bFailIfExists = bFailIfExists;

	// Set some default values
	m_iCopyInstructionCount = 0;
	m_iCurrentCopyInstruction = 0;
	
	// Calculate number of files
	m_iFilesCopied = 0;
	m_iFilesToCopy = pFunctions->CountFiles(sSource);

	// Set up folder script
	SetupFolderScript(sSource, sDestination);

	// Start thread
	PostThreadMessage(WMU_COPY_START, 0, 0);
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CCopyFolder::SetupFolderScript(CString sSource, CString sDestination)
{
	// Declare variables
	CString sTempSource, sTempDestination, sPattern;
	HANDLE hFile;
	WIN32_FIND_DATA FileInformation;

	// Get patern
	sPattern = sSource + "\\*.*";

	// Find first file
	hFile = ::FindFirstFile(sPattern, &FileInformation);

	// Find next files
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Check if it is not the current folder (.) or topfolder (..)
			if ((FileInformation.cFileName[0] != '.') && (FileInformation.cFileName[0] != '..'))
			{
				// Set up new filename
				sTempSource      = sSource + "\\" + FileInformation.cFileName;
				sTempDestination = sDestination + "\\" + FileInformation.cFileName;

				// If it is a folder
				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// Set up subdirectory
					SetupFolderScript(sTempSource, sTempDestination);
				}
				else
				{
					// Add file to script
					CopyInstruction cpyInstr;
					cpyInstr.sNewLocation = sTempDestination;
					cpyInstr.sOriginalLocation = sTempSource;
					m_arrCopyInstructions[m_iCopyInstructionCount++] = cpyInstr;
				}
			}
		} while (::FindNextFile(hFile, &FileInformation));

		// Close handle
		::FindClose(hFile);
	}
}

//=====================================================================

void CCopyFolder::CopyFile(int iFile)
{
	// Declare variables
	CopyInstruction cpyInstr = m_arrCopyInstructions[iFile];

	// Copy the file
	m_pCopyThread = (CCopyFile *)AfxBeginThread(RUNTIME_CLASS(CCopyFile),
		THREAD_PRIORITY_NORMAL,
		0,
		0,
		NULL);

	// Set parent
	m_pCopyThread->SetParent(this);

	// Start copy
	m_pCopyThread->StartFileCopy(cpyInstr.sOriginalLocation, cpyInstr.sNewLocation,
							   m_bFailIfExists);
}

//=====================================================================

void CCopyFolder::CopyNextFile()
{
	// Increase current file
	m_iCurrentCopyInstruction++;

	// Are there still any instructions left?
	if (m_iCurrentCopyInstruction < m_iCopyInstructionCount)
	{
		// Copy the file
		CopyFile(m_iCurrentCopyInstruction);
	}
	else
	{
		// We are ready!
		PostMessageToParent(WMU_COPY_COMPLETE, 0, 0);

		// Exit this thread
		PostThreadMessage(WM_QUIT, 0, 0);
	}
}

//=====================================================================

void CCopyFolder::SendCopyStatus(int iBytesCopied, int iTotalBytes)
{
	// Declare variables
	int iPercentageCompleted, iCurrentActionPoints;

	// Calculate percentage
	iPercentageCompleted = (m_iFilesCopied * 100) / m_iFilesToCopy;

	// Calculate action points performed
	iCurrentActionPoints = (iPercentageCompleted * m_iActionPoints) / 100;

	// Send information to parent
	if (m_pParent && (iCurrentActionPoints != m_iCurrentActionPoints))
	{
		m_iCurrentActionPoints = iCurrentActionPoints;

		// Send status
		PostMessageToParent(WMU_COPY_STATUS, m_iCurrentActionPoints, m_iActionPoints);	
	}
}

//=====================================================================

void CCopyFolder::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

void CCopyFolder::OnCopyStart(WPARAM wParam, LPARAM lParam)
{
	// Check which file to copy
	m_iCurrentCopyInstruction = -1;

	// Start copy process
	CopyNextFile();
}

//=====================================================================

void CCopyFolder::OnCopyFileStatus(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iCurrentActionPoints = 0;

	// Calculate difference between previous and now
	if (m_iTempActionPoints < static_cast<int>(wParam))
	{
		// It's still the same file
		iCurrentActionPoints = wParam - m_iTempActionPoints;
	}
	
	// Set new value of files copied
	m_iTempActionPoints = wParam;

	// Update total bytes copied value
	m_iCurrentActionPoints += iCurrentActionPoints;

	SendCopyStatus(m_iCurrentActionPoints, m_iActionPoints);
}

//=====================================================================

void CCopyFolder::OnCopyFileComplete(WPARAM wParam, LPARAM lParam)
{
	// Another file ready
	m_iFilesCopied++;

	// Copy next file
	CopyNextFile();
}

//=====================================================================

void CCopyFolder::OnError(WPARAM wParam, LPARAM lParam)
{	
	// Send error to parent
	PostMessageToParent(WMU_ERROR, wParam, lParam);
}