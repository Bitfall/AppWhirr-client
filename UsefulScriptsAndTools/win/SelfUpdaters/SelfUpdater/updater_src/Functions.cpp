/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	Functions.cpp
	file path:	Updater
	file base:	Functions
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Functions.h"

#include "FileVersion.h"		// Get version of file

#include <sys/types.h>			// For getting date of file
#include <sys/stat.h>			// For getting date of file

#include "md5.h"				// MD5 hash support

#include <tlhelp32.h>			// Close other app support
#include "RiskFunctions.h"		// Close other app support (also older versions of windows)

#include "CloseApplicationDlg.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CFunctions> CFunctions::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CFunctions::CFunctions()
{
	// Init singleton classes
	m_pPath = CPath::Instance();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CFunctions * CFunctions::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = auto_ptr<CFunctions>(new CFunctions);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CFunctions);
	return sm_inst.get();
	*/
}

//=====================================================================

CString CFunctions::GetFileVersion(CString sFilename)
{
	// Declare variables
	CString sVersion;
	CFileVersion fileversion;

	// Check if file exists
	if (!PathFileExists(sFilename))
		return _T("0.0.0.0");

	// Get version number
	if (!fileversion.Open(sFilename))
		return _T("0.0.0.0");
	sVersion = fileversion.GetFileVersion();
	fileversion.Close();

	// Return version
	return sVersion;
}

//=====================================================================

int CFunctions::GetFileSize(CString sFilename)
{
	// Declare variables
	int iFileSize = -1;
	CFile * pFile;

	// Check if file exists
	if (PathFileExists(sFilename))
	{
		try
		{
			// Create file
			pFile = new CFile(sFilename, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone);

			// Get file size
			iFileSize = static_cast<int>(pFile->SeekToEnd());
		
			// Close file
			pFile->Close();

			// Delete object
			delete pFile;
		}
		catch (CFileException * Ex) 
		{
#if defined _DEBUG || defined _BETA
			Ex->ReportError();
#endif
			// Delete object
			Ex->Delete();
		}

	}
	
	// Return the filesize
	return iFileSize;	
}

//=====================================================================

CString CFunctions::GetFileDate(CString sFilename)
{
	USES_CONVERSION;
	
	// Declare variables
	struct _stat fileInfo;
	CString sTemp;
	
	// Check if file exists
	if (!PathFileExists(sFilename))
		return _T("1970-01-01");

	// Get file info
	_stat(W2A(sFilename), &fileInfo);

	// Get time info
	CTime timeInfo(fileInfo.st_mtime);

	// Return modification date in yyyy-mm-dd format
	sTemp.Format(_T("%04d-%02d-%02d"), timeInfo.GetYear(),
				 timeInfo.GetMonth(), timeInfo.GetDay());

	// Return value
	return sTemp;
}

//=====================================================================

CString CFunctions::GetFileDateTime(CString sFilename)
{
	USES_CONVERSION;

	// Declare variables
	struct _stat fileInfo;
	CString sTemp;
	
	// Check if file exists
	if (!PathFileExists(sFilename))
		return _T("1970-01-01/00:00:00");

	// Get file info
	_stat(W2A(sFilename), &fileInfo);
	
	// Get time info
	CTime timeInfo(fileInfo.st_mtime);
	
	// Return modification time in yyyy-mm-dd/hh:mm:ss format
	sTemp.Format(_T("%04d-%02d-%02d/%02d:%02d:%02d"), timeInfo.GetYear(),
				 timeInfo.GetMonth(), timeInfo.GetDay(), timeInfo.GetHour(), 
				 timeInfo.GetMinute(), timeInfo.GetSecond());
	
	// Return value
	return sTemp;
}

//=====================================================================

CString CFunctions::GetFileMD5Hash(CString sFilename)
{
	USES_CONVERSION;

	// Declare variables
	CString sResult = _T(""), sTemp = _T("");
	FILE * fileToHash = NULL;

	// MD5 object to work with
	MD5_CTX md5;

	// Check if file exists
	if (!PathFileExists(sFilename))
		return _T("");

	// Hash the file
	if (md5file(W2A(sFilename.GetBuffer(MAX_PATH)), 0, &md5))
	{
		// Read digest
		for (int i = 0; i < 16; i++)
		{
			char tmp[3];
			_itoa(md5.digest[i], tmp, 16);
			if (strlen(tmp) == 1)
			{
				tmp[1] = tmp[0];
				tmp[0] = _T('0');
				tmp[2] = _T('\0');
			}
			sTemp += tmp;		
		}
	}

	// Set result and return it
	sResult = sTemp;
	return sResult;
}

//=====================================================================

CString CFunctions::ReplaceConnectionSettings(CString sText)
{
	// Declare variables
	int iIndex = -1;
	CSettings * pSettings = CSettings::Instance();

	// Replace all instances of ftpusername
	while (iIndex != 0)
	{
		iIndex = sText.Replace(CONNECTION_FTPUSERNAME, pSettings->GetFtpUsername());
	}

	iIndex = -1;

	// Replace all instances of ftpusername
	while (iIndex != 0)
	{
		iIndex = sText.Replace(CONNECTION_FTPPASSWORD, pSettings->GetFtpPassword());
	}

	iIndex = -1;

	// Replace all instances of httpusername
	while (iIndex != 0)
	{
		iIndex = sText.Replace(CONNECTION_HTTPUSERNAME, pSettings->GetHttpUsername());
	}

	iIndex = -1;

	// Replace all instances of httpusername
	while (iIndex != 0)
	{
		iIndex = sText.Replace(CONNECTION_HTTPPASSWORD, pSettings->GetHttpPassword());
	}

	// Return value
	return sText;
}

//=====================================================================

CString CFunctions::ExtractRegistryBase(CString sKey)
{
	// Declare variables
	int iPos;

	// Search for first backslash
	iPos = sKey.Find(_T('\\'));

	if (iPos != -1)
	{
		// Delete rest of key
		sKey.Delete(iPos, sKey.GetLength()-iPos);
		return sKey;
	}

	// If we not found, we are already dealing with registry base
	return sKey;
}

//=====================================================================

CString CFunctions::ExtractRegistryKey(CString sKey)
{
	// Declare variables
	int iPos;

	// Search for first backslash
	iPos = sKey.Find(_T('\\'));

	if (iPos != -1)
	{
		// Delete first part
		sKey.Delete(0, iPos+1);
	}

	// Delete last too!
	// Now search for the last backslash
	for (int i = sKey.GetLength(); i > 0; i--)
	{
		iPos = sKey.Find(_T('\\'), sKey.GetLength()-(sKey.GetLength()-i));
		// If we found them, delete last piece
		if (iPos != -1)
		{
			sKey.Delete(iPos, sKey.GetLength()-i);
			return sKey;
		}
	}

	// Return the key
	return sKey;
}

//=====================================================================

CString CFunctions::ExtractRegistryItem(CString sKey)
{
	// Find first
	int iPos = sKey.Find(_T('\\'), 0);

	// Search for backslash
	while (iPos != -1)
	{
		// Delete until backslash
		sKey.Delete(0, iPos+1);

		// Search again
		iPos = sKey.Find(_T('\\'), 0);
	}

	// Return the key
	return sKey;
}

//=====================================================================

CString CFunctions::StripFileStatus(CString sFilename)
{
	// Declare variables
	int iPos = sFilename.ReverseFind(_T('('));

	// If we found the character
	if (iPos != -1)
	{
		// Strip from last ( 
		sFilename.Delete(iPos, sFilename.GetLength() - iPos);

		// Strip spaces that might be located at the end of the string
		sFilename.TrimRight();
	}

	// Return the stripped version of the filename
	return sFilename;
}

//=====================================================================

void CFunctions::DeleteAllFiles(CString sFolder, bool bDeleteFolderToo)
{
	// Declare variables
	WIN32_FIND_DATA wfd;
	HANDLE hFile;
	DWORD dwFileAttr;
	CString sFile;
	CString sSpec = sFolder + _T("*.*");
	CString sPathFile;
	CString sMyVar;

	// Find the first file
	hFile = FindFirstFile(sSpec, &wfd);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			sFile = wfd.cFileName;
			sPathFile = sFolder + sFile;
			// Get the file attributes
			dwFileAttr = GetFileAttributes(sPathFile);

			// See if file is read-only : if so unset read-only
			if (dwFileAttr & FILE_ATTRIBUTE_READONLY)
			{
				dwFileAttr &= ~FILE_ATTRIBUTE_READONLY;
				SetFileAttributes(sPathFile, dwFileAttr);
			}

			// See if the file is a directory
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// Make sure it isn't current or parent directory
				if (sFile != _T(".") && sFile != _T(".."))
				{
					sPathFile += _T("\\");
					// Recursively delete all files in this folder
					DeleteAllFiles(sPathFile, true);

					// Remove the directory
					//RemoveDirectory(sPathFile);
				}
			}
			else
			{
				// Delete the file
				DeleteFile(sPathFile);
			}
		}
		while (FindNextFile(hFile, &wfd));
	}
	
	// Close handle to file
	FindClose(hFile);

	// Delete directory too if needed
	if (bDeleteFolderToo)
	{
		RemoveDirectory(sFolder);
	}
}

//=====================================================================

bool CFunctions::CreateFolder(CString sFolder)
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

int CFunctions::CountFiles(CString sFolder, bool bSubfolders)
{
	// Declare variables
	int iCount = 0;
	CString	sFilePath, sPattern;
	HANDLE hFile;
	WIN32_FIND_DATA FileInformation;

	// Get pattern
	sPattern = sFolder + _T("\\*.*");
	hFile = ::FindFirstFile(sPattern, &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Is it something else then current or top folder?
			if ((FileInformation.cFileName[0] != '.') && (FileInformation.cFileName[0] != '..'))
			{
				// Set up path
				sFilePath = sFolder + _T("\\") + FileInformation.cFileName;

				// Is it a folder?
				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// Should we search subdirectories too?
					if (bSubfolders)
					{
						// Search subdirectory
						int iRC = CountFiles(sFilePath, bSubfolders);

						// Check result of function
						iCount += iRC;
					}
				}
				else
				{
					// Increase counter
					++iCount;
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);
	}

	// Return count
	return iCount;
}

//=====================================================================

int	CFunctions::GetFolderSize(CString sFolder, bool bSubfolders)
{
	// Declare variables
	int iSize = 0;
	CString	sFilePath, sPattern;
	HANDLE hFile;
	WIN32_FIND_DATA FileInformation;

	// Get pattern
	sPattern = sFolder + _T("\\*.*");
	hFile = ::FindFirstFile(sPattern, &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Is it something else then current or top folder?
			if ((FileInformation.cFileName[0] != '.') && (FileInformation.cFileName[0] != '..'))
			{
				// Set up path
				sFilePath = sFolder + _T("\\") + FileInformation.cFileName;

				// Is it a folder?
				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// Should we search subdirectories too?
					if (bSubfolders)
					{
						// Search subdirectory
						int iRS = GetFolderSize(sFilePath, bSubfolders);

						// Check result of function
						iSize += iRS;
					}
				}
				else
				{
					// Increase size
					if (FileInformation.nFileSizeHigh > 0)
					{
						iSize += (FileInformation.nFileSizeHigh * MAXDWORD) + 
								 FileInformation.nFileSizeLow;
					}
					else
					{
						iSize += FileInformation.nFileSizeLow;
					}
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);
	}

	// Return count
	return iSize;
}

//=====================================================================

bool CFunctions::IsWebAddress(CString sString)
{
	// Check if it starts with http
	if (sString.Find(_T("http"), 0) == 0)
		// Valid web address
		return true;

	// Check if it starts with www
	if (sString.Find(_T("www"), 0) == 0)
		// Valid web address
		return true;

	// No valid web address
	return false;
}

//=====================================================================

int CFunctions::CompareVersions(CString sVersion1, CString sVersion2)
{
	USES_CONVERSION;

	// Declare variables
	CString arrVersion1[50], arrVersion2[50], sSeperator1, sSeperator2;
	int iPosition, iVersion1Count = 0, iVersion2Count = 0, i;

	// Find out what kind of separator the user is using
	sSeperator1 = FindVersionSeperator(sVersion1);
	sSeperator2 = FindVersionSeperator(sVersion2);

	// Parse versions
	iPosition = sVersion1.Find(sSeperator1, 0);
	while (iPosition != -1)
	{
		// Copy number of text into field
		if (sSeperator1.IsEmpty())
		{
			arrVersion1[iVersion1Count++] = sVersion1;
		}
		else
		{
			arrVersion1[iVersion1Count++] = sVersion1.Left(iPosition);
		}
		sVersion1.Delete(0, iPosition + 1);
		iPosition = sVersion1.Find(sSeperator1, 0);
	}
	arrVersion1[iVersion1Count++] = sVersion1;

	iPosition = sVersion2.Find(sSeperator2, 0);
	while (iPosition != -1)
	{
		// Copy number of text into field
		if (sSeperator2.IsEmpty())
		{
			arrVersion2[iVersion2Count++] = sVersion2;
		}
		else
		{
			arrVersion2[iVersion2Count++] = sVersion2.Left(iPosition);
		}
		sVersion2.Delete(0, iPosition + 1);
		iPosition = sVersion2.Find(sSeperator2, 0);
	}
	arrVersion2[iVersion2Count++] = sVersion2;

	// Strip all unneeded spaces
	for (i = 0; i < iVersion1Count; i++)
	{
		arrVersion1[i].Remove(_T(' '));
	}
	for (i = 0; i < iVersion2Count; i++)
	{
		arrVersion2[i].Remove(_T(' '));
	}

	// Compare the versions
	for (i = 0; i < 50; i++)
	{
		if (atol(W2A(arrVersion1[i])) < atol(W2A(arrVersion2[i])))
			return COMPAREVERSION_FIRSTSMALLER;

		if (atol(W2A(arrVersion1[i])) > atol(W2A(arrVersion2[i])))
			return COMPAREVERSION_FIRSTLARGER;
	}

	return COMPAREVERSION_EQUAL;
}

//=====================================================================

bool CFunctions::ApplicationRunning(CString sFilename)
{
	// Declare variables
	std::string strVersion;
	UINT uWinPlatform;
	UINT uVersion = getWinVersion(strVersion, uWinPlatform);
	CString sProcess = m_pPath->ExtractFileName(sFilename);

	// Make lowercase
	sProcess = StringLower(sProcess);

	// Check if the application is running
	if ((uVersion == WINDOWS_NT3) || (uVersion == WINDOWS_NT4))
	{
		// Check if application is running
		return NTApplicationRunning(sProcess.GetBuffer(sProcess.GetLength()));
	}
	else
	{
		// Check if application is running
		return AllWinApplicationRunning(sProcess.GetBuffer(sProcess.GetLength()));
	}
}

//=====================================================================

bool CFunctions::CloseApplication(CString sApplicationName, CString sFilename)
{
	// Declare variables
	std::string strVersion;
	UINT uWinPlatform;
	UINT uVersion = getWinVersion(strVersion, uWinPlatform);
	CString sProcess = m_pPath->ExtractFileName(sFilename);

	sProcess = StringLower(sProcess);

	// Check if application is running
	if ((uVersion == WINDOWS_NT3) || (uVersion == WINDOWS_NT4))
	{
		if (!NTApplicationRunning(sProcess.GetBuffer(sProcess.GetLength())))
			return true;
	}
	else
	{
		if (!AllWinApplicationRunning(sProcess.GetBuffer(sProcess.GetLength())))
			return true;
	}

	// Show close application dialog
	CCloseApplicationDlg dlgCloseApplication(sApplicationName, sFilename);
	if (dlgCloseApplication.DoModal() == IDCANCEL)
		return false;

	// If we get here, return true
	return true;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

CString CFunctions::FindVersionSeperator(CString sVersion)
{
	// Declare variables
	CString sSeperator = _T(""), sSeperatorToFind = _T("");
	int iSeperatorCount = 0, iPosition;

	// Find out what kind of separator the user is using
	while ((iSeperatorCount < 7) && (sSeperator.IsEmpty()))
	{
		// Search for our seperator
		switch (iSeperatorCount)
		{
		case 0:
			// Search for .
			sSeperatorToFind = _T(".");
			break;

		case 1:
			// Search for ,
			sSeperatorToFind = _T(",");
			break;

		case 2:
			// Search for _
			sSeperatorToFind = _T("_");
			break;

		case 3:
			// Search for |
			sSeperatorToFind = _T("|");
			break;

		case 4:
			// Search for -
			sSeperatorToFind = _T("-");
			break;

		case 5:
			// Search for space
			sSeperatorToFind = _T(" ");
			break;

		default:
			// We cannot found our seperator, treat versions as 1 block
			sSeperator = _T("");
			break;
		}
	
		// Do the search
		iPosition = sVersion.Find(sSeperatorToFind, 0);
		if (iPosition != -1)
		{
			// We found our seperator
			sSeperator = sSeperatorToFind;
		}

		// Search for next if we didn't found anything
		iSeperatorCount++;
	}

	// Return the seperator
	return sSeperator;
}