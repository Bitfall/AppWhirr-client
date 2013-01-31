/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:55
	filename: 	Download.cpp
	file path:	Updater
	file base:	Download
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Download.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CDownload, CWinThread)
	ON_THREAD_MESSAGE(WMU_SETPARENT, OnSetParent)
	ON_THREAD_MESSAGE(WMU_THREADMESSAGE, OnThreadMessage)
	ON_THREAD_MESSAGE(WMU_DOWNLOAD_STATUS, OnDownloadStatus)
	ON_THREAD_MESSAGE(WMU_DOWNLOAD_COMPLETE, OnDownloadComplete)
	ON_THREAD_MESSAGE(WMU_ERROR, OnError)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNCREATE(CDownload, CWinThread)

//=====================================================================

CDownload::CDownload()
{
}

//=====================================================================

CDownload::~CDownload()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CDownload::InitInstance()
{
	// Log
	CLog::Instance()->Log(_T("CDownload::InitInstance"));

	// Set parent to NULL
	m_pParent = NULL;

	// We are not started yet
	m_bStarted = false;

	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pInternet = CInternet::Instance();
	m_pPath = CPath::Instance();

	// Set parent for internet
	m_pInternet->SetParent(this);

	// Reserve memory for buffer
	m_szFilename = (TCHAR *)malloc(sizeof(TCHAR) * 500);
	ZeroMemory(m_szFilename, sizeof(TCHAR) * 500);

	return TRUE;
}

//=====================================================================

int CDownload::ExitInstance()
{
	// Log
	CLog::Instance()->Log(_T("CDownload::ExitInstance"));

	// Stop all download processes that "might" be running
	m_pInternet->StopDownloading();

	// Auto clean-up thread
	m_bAutoDelete = TRUE;

	// Call original function
	return CWinThread::ExitInstance();
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CDownload::Main()
{
	// Initialise the downloads
	if (!InitDownloads())
	{
		// Send error message
		PostMessageToParent(WMU_ERROR, ERROR_FILENOTFOUND, (LPARAM)m_szFilename);
		PostThreadMessage(WM_QUIT, 0, 0);
		return;
	}

	// Check if we have enough diskspace
	if (!EnoughFreeDiskSpace())
	{	
		PostMessageToParent(WMU_ERROR, ERROR_NODISKSPACE, m_iTotalBytes);
		PostThreadMessage(WM_QUIT, 0, 0);
		return;
	}

	// If we have any downloads
	if (m_iDownloadCount > 0)
	{
		// Start first download
		StartDownload(0);
	}
	else
	{
		// Put empty string into char buffer
		lstrcpy(m_szFilename, _T(""));

		// Set progress to 100%
		PostMessageToParent(WMU_UPDATE_GUI_FILEPROGRESS, 100, (LPARAM)m_szFilename);
		PostMessageToParent(WMU_UPDATE_GUI_TOTALPROGRESS, 100, 0);

		// Send message to parent that we are ready
		PostMessageToParent(WMU_TASK_COMPLETE, TASK_DOWNLOAD, 0);

		// Exit the thread
		PostThreadMessage(WM_QUIT, 0, 0);
	}
}

//=====================================================================

bool CDownload::InitDownloads()
{
	// Declare variables
	m_iDownloadCount = 0;
	m_iTotalBytes = 0;
	m_iCurrentActionPoints = 0;
	m_iTotalActionPoints = 0;
	CFileData * pFileData;
	CActionDownload * pActionDownload;
	int iFileSize, k;
	CString sName;

	// For all files
	for (int i = 0; i < m_pUpdateInfo->GetFileCount(); i++)
	{
		// Get file object
		pFileData = m_pUpdateInfo->GetFileData(i);

		// Should we update file?
		if (pFileData->GetUpdateFile())
		{
			// For all actions
			for (int j = 0; j < pFileData->GetActionCount(); j++)
			{
				// Check if action is a download
				if (pFileData->GetAction(j)->GetType() == ACTION_DOWNLOAD)
				{
					// Get object
					pActionDownload = (CActionDownload *)pFileData->GetAction(j);
					
					// Put string into char buffer
					sName = pFileData->GetName();
					lstrcpy(m_szFilename, sName);
					
					// Init k
					k = 0;
					iFileSize = -1;
					
					// While we did not found any valid location and we have still locations
					while ((k < pActionDownload->GetDownloadLocationCount()) && (iFileSize == -1))
					{
						// Can we get filesize?
						iFileSize = m_pInternet->GetFileSize(pActionDownload->GetDownloadLocation(k));
						
						// Did we get filesize successfully?
						if (iFileSize != -1)
						{
							// Create struct
							DownloadInfo tmpDownloadInfo;
							tmpDownloadInfo.sTitle = pFileData->GetName();
							tmpDownloadInfo.sFilename = pFileData->GetFilename();
							tmpDownloadInfo.sDownloadLocation = pActionDownload->GetDownloadLocation(k);
							tmpDownloadInfo.sDestination = pActionDownload->GetDownloadDestination();
							
							// Add to array
							m_arrDownloads[m_iDownloadCount++] = tmpDownloadInfo;

							// Also add it to the garbage collector
							if (pActionDownload->SubscribeToGarbageCollector())
							{
								CGarbageCollector::Instance()->AddFileToRemove(tmpDownloadInfo.sDestination);
							}
							
							// Also add filesize to total filesize
							m_iTotalBytes += iFileSize;

							// Calculate action points for file
							m_iTotalActionPoints += CActionPoints::CalculateActionPoints(iFileSize);
						}
						else
						{
							// No, we failed
							k++;
						}
					}
					
					// If filesize is still -1
					if (iFileSize == -1)
					{
						// File could not be found!
						return false;
					}
				}
			}
		}
	}

	// If we get here, function is successful
	return true;
}

//=====================================================================

void CDownload::StartDownload(int iDownload)
{
	// Declare variables
	CString sDestination, sName;

	// Set current download
	m_iCurrentDownload = iDownload;
	m_iTempActionPoints = 0;

	// Set up filename on hard-disk
	sDestination = m_arrDownloads[m_iCurrentDownload].sDestination;

	// Put string into char buffer
	sName = m_pPath->ExtractFileName(m_arrDownloads[m_iCurrentDownload].sTitle);
	lstrcpy(m_szFilename, sName);

	// Start download
	if (!m_pInternet->DownloadFile(m_arrDownloads[m_iCurrentDownload].sDownloadLocation, 
		sDestination, m_pUpdateInfo->GetResumeDownloads()))
	{
		// Post error
		PostMessageToParent(WMU_ERROR, ERROR_DOWNLOADFILE, (LPARAM)m_szFilename);
	}
}

//=====================================================================

void CDownload::StartNextDownload()
{
	// Update current download
	m_iCurrentDownload++;

	// Do we still have any downloads?
	if (m_iCurrentDownload < m_iDownloadCount)
	{
		// Start next download
		StartDownload(m_iCurrentDownload);
	}
	else
	{
		// Always end up with 100%
		PostMessageToParent(WMU_UPDATE_GUI_FILEPROGRESS, 100, (LPARAM)m_szFilename);
		PostMessageToParent(WMU_UPDATE_GUI_TOTALPROGRESS, 100, 0);

		// Send message to parent that we are ready
		PostMessageToParent(WMU_TASK_COMPLETE, TASK_DOWNLOAD, 0);
		
		// Exit the thread
		PostThreadMessage(WM_QUIT, 0, 0);
	}
}

//=====================================================================

bool CDownload::EnoughFreeDiskSpace()
{
	// Declare variables
	ULARGE_INTEGER liBytesForCaller;
	ULARGE_INTEGER liBytesCount;
	ULARGE_INTEGER liFreeBytesCount;
	
	// Get data
	if (!GetDiskFreeSpaceEx(m_pPath->GetPathTemp(),
		&liBytesForCaller,
		&liBytesCount,
		&liFreeBytesCount))
	{
		// We can't check if we have enough disk space
		return false;
	}
	
	// Check if we have enough
	if (liFreeBytesCount.QuadPart > m_iTotalBytes)
		return true;
	
	// We do not have enough diskspace
	return false;
}

//=====================================================================

void CDownload::PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check if parent is still valid
	if (!IsBadReadPtr(m_pParent, sizeof(CWinThread *)))
	{
		// Send message
		m_pParent->PostThreadMessage(message, wParam, lParam);
	}
}

//=====================================================================

void CDownload::OnDownloadStatus(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iFileProgress, iTotalProgress;
	
	// Calculate percentage of file progress
	iFileProgress = (wParam * 100) / lParam;

	// Calculate percentage of total progress
	m_iCurrentActionPoints += (wParam - m_iTempActionPoints);
	m_iTempActionPoints = wParam;
	if (m_iTotalActionPoints != 0)
		iTotalProgress = (m_iCurrentActionPoints * 100) / m_iTotalActionPoints;
	else
		iTotalProgress = 100;

	// Update GUI only when needed
	if (m_iFileProgress != iFileProgress)
	{
		PostMessageToParent(WMU_UPDATE_GUI_FILEPROGRESS, iFileProgress,
			(LPARAM)m_szFilename);
	}
	if (m_iTotalProgress != iTotalProgress)
	{
		PostMessageToParent(WMU_UPDATE_GUI_TOTALPROGRESS, iTotalProgress, 0);
	}
	
	// Update data
	m_iTotalProgress = iTotalProgress;
	m_iFileProgress = iFileProgress;
}

//=====================================================================

void CDownload::OnDownloadComplete(WPARAM wParam, LPARAM lParam)
{
	// Yes, another download completed, start next download
	StartNextDownload();
}

//=====================================================================

void CDownload::OnError(WPARAM wParam, LPARAM lParam)
{
	// The download failed!
	PostMessageToParent(WMU_ERROR, ERROR_DOWNLOADFILE, (LPARAM)m_szFilename);
	// Exit thread
	PostThreadMessage(WM_QUIT, 0, 0);
}

//=====================================================================

void CDownload::OnSetParent(WPARAM wParam, LPARAM lParam)
{
	// Set parent
	m_pParent = (CWinThread *)wParam;
}

//=====================================================================

void CDownload::OnThreadMessage(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iThreadAction = (int)wParam;
	
	// Check what task to perform for thread
	switch (iThreadAction)
	{
	case THREAD_START:
		// Are we already started?
		if (!m_bStarted)
		{
			// Start main function
			Main();
		}
		break;
		
	case THREAD_PAUSE:
		// Pause thread
		SuspendThread();
		break;
		
	case THREAD_RESUME:
		// Resume thread
		ResumeThread();
		break;
		
	case THREAD_CANCEL:
		// Exit thread
		PostThreadMessage(WM_QUIT, 0, 0);
		break;
		
	case THREAD_FINISH:
		// Exit thread
		PostThreadMessage(WM_QUIT, 0, 0);
		break;
	}
}
