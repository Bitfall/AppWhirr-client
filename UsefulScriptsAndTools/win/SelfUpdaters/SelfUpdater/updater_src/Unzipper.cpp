// Unzipper.cpp: implementation of the CUnzipper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include "Unzipper.h"

#include "zlib\unzip.h"
#include "zlib\iowin32.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const UINT BUFFERSIZE = 2048;

CUnzipper::CUnzipper(CString szFilePath) : m_uzFile(0)
{
	OpenZip(szFilePath);
}

CUnzipper::~CUnzipper()
{
	CloseZip();
}

bool CUnzipper::CloseZip()
{
	unzCloseCurrentFile(m_uzFile);

	int nRet = unzClose(m_uzFile);
	m_uzFile = NULL;
	m_szOutputFolder[0] = 0;

	return (nRet == UNZ_OK);
}

// simple interface
bool CUnzipper::Unzip(bool bIgnoreFilePath)
{
	if (!m_uzFile)
		return FALSE;

	return UnzipTo(m_szOutputFolder, bIgnoreFilePath);
}

bool CUnzipper::UnzipTo(CString szFolder, bool bIgnoreFilePath)
{
	if (!m_uzFile)
		return FALSE;

	if (!szFolder || !CreateFolder(szFolder))
		return FALSE;

	if (GetFileCount() == 0)
		return FALSE;

	if (!GotoFirstFile())
		return FALSE;

	// else
	do
	{
		if (!UnzipFile(szFolder, bIgnoreFilePath))
			return FALSE;
	}
	while (GotoNextFile());
	
	return TRUE;
}

// static version
bool CUnzipper::Unzip(CString szFileName, CString szFolder, bool bIgnoreFilePath)
{
	CUnzipper unz;

	if (!unz.OpenZip(szFileName))
		return FALSE;

	return unz.UnzipTo(szFolder, bIgnoreFilePath);
}

// extended interface
bool CUnzipper::OpenZip(CString szFilePath)
{
	USES_CONVERSION;

	CloseZip();

	if (!szFilePath || !lstrlen(szFilePath))
		return false;

	// convert szFilePath to fully qualified path 
	TCHAR szFullPath[MAX_PATH];
	
	if (!GetFullPathName(szFilePath, MAX_PATH, szFullPath, NULL))
		return false;
	
	m_uzFile = unzOpen(W2A(szFullPath));
	
	if (m_uzFile)
	{
		TCHAR szDrive[_MAX_DRIVE], szFolder[MAX_PATH], szFName[_MAX_FNAME];
		
		_tsplitpath(szFullPath, szDrive, szFolder, szFName, NULL);
		_tmakepath(m_szOutputFolder, szDrive, szFolder, szFName, NULL);
	}

	return (m_uzFile != NULL);
}

bool CUnzipper::SetOutputFolder(CString szFolder)
{
	DWORD dwAttrib = GetFileAttributes(szFolder);

	if (dwAttrib != 0xffffffff && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
		return FALSE;

	lstrcpy(m_szOutputFolder, szFolder);

	return CreateFolder(szFolder);
}

int CUnzipper::GetFileCount()
{
	if (!m_uzFile)
		return 0;

	unz_global_info info;

	if (unzGetGlobalInfo(m_uzFile, &info) == UNZ_OK)
	{
		return (int)info.number_entry;
	}

	return 0;
}

bool CUnzipper::GetFileInfo(int nFile, UZ_FileInfo& info)
{
	if (!m_uzFile)
		return FALSE;

	if (!GotoFile(nFile))
		return FALSE;

	return GetFileInfo(info);
}

bool CUnzipper::UnzipFile(int nFile, CString szFolder, bool bIgnoreFilePath)
{
	if (!m_uzFile)
		return FALSE;

	if (!szFolder)
		szFolder = m_szOutputFolder;

	if (!GotoFile(nFile))
		return FALSE;

	return UnzipFile(szFolder, bIgnoreFilePath);
}

bool CUnzipper::GotoFirstFile(CString szExt)
{
	if (!m_uzFile)
		return FALSE;

	if (!szExt || !lstrlen(szExt))
		return (unzGoToFirstFile(m_uzFile) == UNZ_OK);

	// else
	if (unzGoToFirstFile(m_uzFile) == UNZ_OK)
	{
		UZ_FileInfo info;

		if (!GetFileInfo(info))
			return FALSE;

		// test extension
		TCHAR szFExt[_MAX_EXT];
		_tsplitpath(info.szFileName, NULL, NULL, NULL, szFExt);

		if (szFExt)
		{
			if (lstrcmpi(szExt, szFExt + 1) == 0)
				return TRUE;
		}

		return GotoNextFile(szExt);
	}

	return FALSE;
}

bool CUnzipper::GotoNextFile(CString szExt)
{
	if (!m_uzFile)
		return FALSE;

	if (!szExt || !lstrlen(szExt))
		return (unzGoToNextFile(m_uzFile) == UNZ_OK);

	// else
	UZ_FileInfo info;

	while (unzGoToNextFile(m_uzFile) == UNZ_OK)
	{
		if (!GetFileInfo(info))
			return FALSE;

		// test extension
		TCHAR szFExt[_MAX_EXT];
		_tsplitpath(info.szFileName, NULL, NULL, NULL, szFExt);

		if (szFExt)
		{
			if (lstrcmpi(szExt, szFExt + 1) == 0)
				return TRUE;
		}
	}

	return FALSE;

}

bool CUnzipper::GetFileInfo(UZ_FileInfo& info)
{
	USES_CONVERSION;

	if (!m_uzFile)
		return FALSE;

	unz_file_info uzfi;

	ZeroMemory(&info, sizeof(info));
	ZeroMemory(&uzfi, sizeof(uzfi));

	char szFileName[MAX_PATH+1] = "";
	char szComment[MAX_COMMENT+1] = "";
	
	if (UNZ_OK != unzGetCurrentFileInfo(m_uzFile, &uzfi, szFileName, MAX_PATH, NULL, 0, szComment, MAX_COMMENT))
		return FALSE;

	_tcsncpy (info.szFileName, A2CT(szFileName),MAX_PATH);
	_tcsncpy (info.szComment, A2CT(szComment),MAX_COMMENT);

	// copy across
	info.dwVersion = uzfi.version;	
	info.dwVersionNeeded = uzfi.version_needed;
	info.dwFlags = uzfi.flag;	
	info.dwCompressionMethod = uzfi.compression_method; 
	info.dwDosDate = uzfi.dosDate;  
	info.dwCRC = uzfi.crc;	 
	info.dwCompressedSize = uzfi.compressed_size; 
	info.dwUncompressedSize = uzfi.uncompressed_size;
	info.dwInternalAttrib = uzfi.internal_fa; 
	info.dwExternalAttrib = uzfi.external_fa; 

	// replace filename forward slashes with backslashes
	int nLen = lstrlen(info.szFileName);

	while (nLen--)
	{
		if (info.szFileName[nLen] == '/')
			info.szFileName[nLen] = '\\';
	}

	// is it a folder?
	info.bFolder = ((info.dwExternalAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);

	return TRUE;
}

bool CUnzipper::UnzipFile(CString szFolder, bool bIgnoreFilePath)
{
	if (!m_uzFile)
		return FALSE;

	if (!szFolder)
		szFolder = m_szOutputFolder;

	if (!CreateFolder(szFolder))
		return FALSE;

	UZ_FileInfo info;
	GetFileInfo(info);

	// if the item is a folder then create it and return 'TRUE'
	if (info.bFolder)
	{
		TCHAR szFolderPath[MAX_PATH];
		_tmakepath(szFolderPath, NULL, szFolder, info.szFileName, NULL);

		return CreateFolder(szFolderPath);
	}

	// build the output filename
	TCHAR szFilePath[MAX_PATH];

	if (bIgnoreFilePath)
	{
		TCHAR szFName[_MAX_FNAME], szExt[_MAX_EXT];

		_tsplitpath(info.szFileName, NULL, NULL, szFName, szExt);
		_tmakepath(info.szFileName, NULL, NULL, szFName, szExt);
	}

	_tmakepath(szFilePath, NULL, szFolder, info.szFileName, NULL);

	// open the input and output files
	if (!CreateFilePath(szFilePath))
		return FALSE;

	HANDLE hOutputFile = ::CreateFile(szFilePath, 
										GENERIC_WRITE,
										0,
										NULL,
										CREATE_ALWAYS,
										FILE_ATTRIBUTE_NORMAL,
										NULL);

	if (INVALID_HANDLE_VALUE == hOutputFile)
		return FALSE;

	if (unzOpenCurrentFile(m_uzFile) != UNZ_OK)
		return FALSE;

	// read the file and output
	int nRet = UNZ_OK;
	TCHAR pBuffer[BUFFERSIZE];

	do
	{
		nRet = unzReadCurrentFile(m_uzFile, pBuffer, BUFFERSIZE);

		if (nRet > 0)
		{
			// output
			DWORD dwBytesWritten = 0;

			if (!::WriteFile(hOutputFile, pBuffer, nRet, &dwBytesWritten, NULL) ||
				dwBytesWritten != (DWORD)nRet)
			{
				nRet = UNZ_ERRNO;
				break;
			}
		}
	}
	while (nRet > 0);

	CloseHandle(hOutputFile);
	unzCloseCurrentFile(m_uzFile);

	if (nRet == UNZ_OK)
	{
		SetFileModTime(szFilePath, info.dwDosDate);
		SetFileAttributes(szFilePath, info.dwExternalAttrib);
	}

	return (nRet == UNZ_OK);
}

bool CUnzipper::GotoFile(int nFile)
{
	if (!m_uzFile)
		return FALSE;

	if (nFile < 0 || nFile >= GetFileCount())
		return FALSE;

	GotoFirstFile();

	while (nFile--)
	{
		if (!GotoNextFile())
			return FALSE;
	}

	return TRUE;
}

bool CUnzipper::GotoFile(CString szFileName, bool bIgnoreFilePath)
{
	USES_CONVERSION;

	if (!m_uzFile)
		return FALSE;

	// try the simple approach
	if (unzLocateFile(m_uzFile, W2A(szFileName), 2) == UNZ_OK)
		return TRUE;

	else if (bIgnoreFilePath)
	{ 
		// brute force way
		if (unzGoToFirstFile(m_uzFile) != UNZ_OK)
			return FALSE;

		UZ_FileInfo info;

		do
		{
			if (!GetFileInfo(info))
				return FALSE;

			// test name
			TCHAR szFName[_MAX_FNAME], szName[_MAX_FNAME], szExt[_MAX_EXT];

			_tsplitpath(info.szFileName, NULL, NULL, szName, szExt);
			_tmakepath(szFName, NULL, NULL, szName, szExt);

			if (lstrcmpi(szFileName, szFName) == 0)
				return TRUE;
		}
		while (unzGoToNextFile(m_uzFile) == UNZ_OK);
	}

	// else
	return FALSE;
}

bool CUnzipper::CreateFolder(CString sFolder)
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

bool CUnzipper::CreateFilePath(CString szFilePath)
{
	// Declare variables
	bool bRes = FALSE;

	// Find last backslash
	int iPos = szFilePath.ReverseFind(_T('\\'));
	
	if (iPos != -1) 
	{
		// Remove part of string
		szFilePath.Delete(iPos, szFilePath.GetLength() - iPos);
		
		bRes = CreateFolder(szFilePath);
	}

	return bRes;
}

bool CUnzipper::SetFileModTime(CString szFilePath, DWORD dwDosDate)
{
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (!hFile)
		return FALSE;
	
	FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

	bool bRes = (GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite) == TRUE);

	if (bRes)
		bRes = (TRUE == DosDateTimeToFileTime((WORD)(dwDosDate >> 16), (WORD)dwDosDate, &ftLocal));

	if (bRes)
		bRes = (TRUE == LocalFileTimeToFileTime(&ftLocal, &ftm));

	if (bRes)
		bRes = (TRUE == SetFileTime(hFile, &ftm, &ftLastAcc, &ftm));

	CloseHandle(hFile);

	return bRes;
}

