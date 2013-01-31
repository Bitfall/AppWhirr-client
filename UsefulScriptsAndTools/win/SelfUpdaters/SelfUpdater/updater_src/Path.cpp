/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	Path.cpp
	file path:	Updater
	file base:	Path
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Path.h"

#include <direct.h>				// For getting path to current folder
#include <shlobj.h>				// For using SHGetSpecialFolderPath
#include "RiskFunctions.h"		// For older versions of windows, some function calls are different
#include <Shlwapi.h>			// For PathRelativePathTo function

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CPath> CPath::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CPath::CPath()
{

}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CPath * CPath::Instance()
{
	if(sm_inst.get() == 0)
	sm_inst = auto_ptr<CPath>(new CPath);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CPath);
	return sm_inst.get();
	*/
}

//=====================================================================

bool CPath::Init()
{
	USES_CONVERSION;

	// Declare variables
	CString sDLLfile;
	char map[_MAX_PATH];
	TCHAR szTemp[_MAX_PATH] = {0};

	// Init windows NT dll
	initDLL();

	// Find several paths
    _getcwd(map, _MAX_PATH);

	// Updater executable
	GetModuleFileName(NULL, szTemp, _MAX_PATH);
	m_sPathUpdaterExecutable = szTemp;

	// Updater (working) path
	m_sPathUpdater = map;
	m_sPathUpdater += _T("\\");
	
	// Temp path
	GetTempPath(_MAX_PATH, szTemp);
	m_sPathTemp = szTemp;

	// System path
	GetSystemDirectory(szTemp, _MAX_PATH);
	m_sPathSystem = szTemp;
	m_sPathSystem += _T("\\");

	// Program files path
    SHGetSpecialFolderPath(NULL, szTemp, CSIDL_PROGRAM_FILES, FALSE);
	m_sPathProgramfiles = szTemp;
	m_sPathProgramfiles += _T("\\");

	// Application data path
    SHGetSpecialFolderPath(NULL, szTemp, CSIDL_APPDATA, FALSE);
	m_sPathAppData = szTemp;
	m_sPathAppData += _T("\\");
 
	// Common application data path
    SHGetSpecialFolderPath(NULL, szTemp, CSIDL_COMMON_APPDATA, FALSE);
	m_sPathCommonAppData = szTemp;
	m_sPathCommonAppData += _T("\\");

	// User home directory path
    SHGetSpecialFolderPath(NULL, szTemp, CSIDL_PROFILE, FALSE);
	m_sPathUserProfile = szTemp;
	m_sPathUserProfile += _T("\\");

	// Quick launch - current user path
	m_sPathQuickLaunchCurrentUser = m_sPathAppData + _T("\\Microsoft\\Internet Explorer\\Quick Launch\\");

	// Menu start - current user path
    SHGetSpecialFolderPath(NULL, szTemp, CSIDL_STARTMENU, FALSE);
	m_sPathMenuStartCurrentUser = szTemp;
	m_sPathMenuStartCurrentUser += _T("\\");

	// Desktop - current user path
    SHGetSpecialFolderPath(NULL, szTemp, CSIDL_DESKTOPDIRECTORY, FALSE);
	m_sPathDesktopCurrentUser = szTemp;
	m_sPathDesktopCurrentUser += _T("\\");

	// Quick launch - all users path
	m_sPathQuickLaunchAllUsers = m_sPathCommonAppData + _T("\\Microsoft\\Internet Explorer\\Quick Launch\\");

	// Menu start - all users path
    SHGetSpecialFolderPath(NULL, szTemp, CSIDL_COMMON_STARTMENU, FALSE);
	m_sPathMenuStartAllUsers = szTemp;
	m_sPathMenuStartAllUsers += _T("\\");

	// Desktop - all users path
    SHGetSpecialFolderPath(NULL, szTemp, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
	m_sPathDesktopAllUsers = szTemp;
	m_sPathDesktopAllUsers += _T("\\");

	// Windows Directory path
	GetWindowsDirectory(szTemp, _MAX_PATH);
	m_sPathWindowsDirectory = szTemp;
	m_sPathWindowsDirectory += _T("\\");

	return true;
}

//=====================================================================

bool CPath::CreateUpdaterFolders()
{
	// Get settings
	CSettings * pSettings = CSettings::Instance();
	CFunctions * pFunctions = CFunctions::Instance();

	// Updater temp path
	m_sPathUpdaterTemp = m_sPathTemp + _T("updater_temp_") + pSettings->GetAppName() + _T("\\") + pSettings->GetAppVersion() + _T("\\");

	// Rollback path
	m_sPathRollback = m_sPathUpdaterTemp + _T("rollback\\");

	// Application path
	m_sPathApp = ExtractFilePath(pSettings->GetAppLocation());
	m_sPathApp += _T("\\");

	// Update server locations
	m_sPathUpdateServerPath = pSettings->GetURL();
	m_sPathUpdateServerFile = ExtractFileName(pSettings->GetURL());

	// Create temp directory
	if (!PathFileExists(m_sPathUpdaterTemp))
	{
		if (!pFunctions->CreateFolder(m_sPathUpdaterTemp))
			return false;
	}
	
	// Create rollback directory
	if (!PathFileExists(m_sPathRollback))
	{
		if (!pFunctions->CreateFolder(m_sPathRollback))
			return false;
	}

	// If we get here, we are successful
	return true;
}

//=====================================================================

CString CPath::GetPathUpdater()
{
	// Return value
	return m_sPathUpdater;
}

//=====================================================================

CString CPath::GetPathUpdaterExecutable()
{
	// Return value
	return m_sPathUpdaterExecutable;
}

//=====================================================================

CString CPath::GetPathUpdaterTemp()
{
	// Return value
	return m_sPathUpdaterTemp;
}

//=====================================================================

CString CPath::GetPathRollback()
{
	// Return value
	return m_sPathRollback;
}

//=====================================================================

CString CPath::GetPathTemp()
{
	// Return value
	return m_sPathTemp;
}

//=====================================================================

CString CPath::GetPathSystem()
{
	// Return value
	return m_sPathSystem;
}

//=====================================================================

CString CPath::GetPathProgramfiles()
{
	// Return value
	return m_sPathProgramfiles;
}

//=====================================================================

CString CPath::GetPathApp()
{
	// Return value
	return m_sPathApp;
}

//=====================================================================

CString CPath::GetPathUserProfile()
{
	// Return value
	return m_sPathUserProfile;
}

//=====================================================================

CString CPath::GetPathAppData()
{
	// Return value
	return m_sPathAppData;
}

//=====================================================================

CString CPath::GetPathCommonAppData()
{
	// Return value
	return m_sPathCommonAppData;
}

//=====================================================================

CString CPath::GetPathQuickLaunchCurrentUser()
{
	// Return value
	return m_sPathQuickLaunchCurrentUser;
}

//=====================================================================

CString CPath::GetPathMenuStartCurrentUser()
{
	// Return value
	return m_sPathMenuStartCurrentUser;
}

//=====================================================================

CString CPath::GetPathDesktopCurrentUser()
{
	// Return value
	return m_sPathDesktopCurrentUser;
}

//=====================================================================

CString CPath::GetPathQuickLaunchAllUsers()
{
	// Return value
	return m_sPathQuickLaunchAllUsers;
}

//=====================================================================

CString CPath::GetPathMenuStartAllUsers()
{
	// Return value
	return m_sPathMenuStartAllUsers;
}

//=====================================================================

CString CPath::GetPathDesktopAllUsers()
{
	// Return value
	return m_sPathDesktopAllUsers;
}

//=====================================================================

CString CPath::ReplacePathConstants(CString sText)
{
	// Declare variables
	int iBegin;

	// Replace all constants
	sText.Replace(PATH_APP, m_sPathApp);
	sText.Replace(PATH_APPROOT, CSettings::Instance()->GetAppRoot());
	sText.Replace(PATH_UPDATER, m_sPathUpdater);
	sText.Replace(PATH_UPDATERTEMP, m_sPathUpdaterTemp);
	sText.Replace(PATH_TEMP, m_sPathTemp);
	sText.Replace(PATH_PROGRAMFILES, m_sPathProgramfiles);
	sText.Replace(PATH_SYSTEM, m_sPathSystem);
	sText.Replace(PATH_APPDATA, m_sPathAppData);
	sText.Replace(PATH_COMMON_APPDATA, m_sPathCommonAppData);
	sText.Replace(PATH_USERPROFILE, m_sPathUserProfile);
	sText.Replace(PATH_QUICKLAUNCHCURRENTUSER, m_sPathQuickLaunchCurrentUser);
	sText.Replace(PATH_MENUSTARTCURRENTUSER, m_sPathMenuStartCurrentUser);
	sText.Replace(PATH_DESKTOPCURRENTUSER, m_sPathDesktopCurrentUser);
	sText.Replace(PATH_QUICKLAUNCHALLUSERS, m_sPathQuickLaunchAllUsers);
	sText.Replace(PATH_MENUSTARTALLUSERS, m_sPathMenuStartAllUsers);
	sText.Replace(PATH_DESKTOPALLUSERS, m_sPathDesktopAllUsers);
	sText.Replace(PATH_WINDOWSDIRECTORY, m_sPathWindowsDirectory);
	sText.Replace(PATH_UPDATESERVERPATH, m_sPathUpdateServerPath);
	sText.Replace(PATH_UPDATESERVERFILE, m_sPathUpdateServerFile);

	// It is possible we have double \\ now, so replace them
	// But we can't replace the first 2 because it may lead to a server
	iBegin = sText.Find(_T("\\\\"), 1);
	while (iBegin > 1)
	{
		// Replace double slashes
		sText.Delete(iBegin);

		// Find another one
		iBegin = sText.Find(_T("\\\\"), 1);
	}

	// Return converted text
	return sText;
}

//=====================================================================

CString CPath::ExtractFileName(CString sPath, CString sSplitter /* = _T("\\") */)
{
	// Find first
	int iPos = sPath.Find(_T('\\'), 0);

	// Search for backslash
	while (iPos != -1)
	{
		// Delete until backslash
		sPath.Delete(0, iPos+1);

		// Search again
		iPos = sPath.Find(_T('\\'), 0);
	}

	// Find first /
	iPos = sPath.Find(_T('/'), 0);

	// Search for slash
	while (iPos != -1)
	{
		// Delete until slash
		sPath.Delete(0, iPos+1);

		// Search again
		iPos = sPath.Find(_T('/'), 0);
	}

	// Return the path
	return sPath;
}

//=====================================================================

CString CPath::ExtractFilePath(CString sPath, CString sSplitter /* = "\\" */)
{
	// Declare variables
	int iPos;

	// Now search for the last backslash
	for (int i = sPath.GetLength(); i > 0; i--)
	{
		iPos = sPath.Find(sSplitter, sPath.GetLength()-(sPath.GetLength()-i));
		// If we found them, delete last piece
		if (iPos != -1)
		{
			sPath.Delete(iPos, sPath.GetLength() - i);
			return sPath;
		}
	}

	// If we not found, we are already dealing with path
	return sPath;
}

//=====================================================================

CString CPath::ExtractDrive(CString sPath, CString sSplitter /* = "\\" */)
{
	// Declare variables
	int iPos;
	DWORD dwDriveType;

	// Search for first backslash
	iPos = sPath.Find(sSplitter);

	if (iPos != -1)
	{
		// Delete rest of path
		sPath.Delete(iPos, sPath.GetLength() - iPos);
	}

	// Check if the path is a valid drive
	sPath += _T("\\");
	dwDriveType = GetDriveType(sPath);
	if ((dwDriveType == DRIVE_UNKNOWN) || (dwDriveType == DRIVE_NO_ROOT_DIR))
	{
		return _T("");
	}

	// Return the drive
	return sPath;
}

//=====================================================================

CString CPath::ExtractFileExtension(CString sPath)
{
	// Declare variables
	int iPos;

	// Now search for the last dot
	for (int i = sPath.GetLength(); i > 0; i--)
	{
		iPos = sPath.Find(_T("."), sPath.GetLength()-(sPath.GetLength()-i));
		// If we found them, delete last piece
		if (iPos != -1)
		{
			sPath.Delete(0, iPos);
			return sPath;
		}

		// If it is a slash, we should not use this extension anymore
		if (sPath.Find(_T("\\"), sPath.GetLength()-(sPath.GetLength()-i)) != -1)
			return _T("");
		if (sPath.Find(_T("\\"), sPath.GetLength()-(sPath.GetLength()-i)) != -1)
			return _T("");
	}

	// If we not found, no extension is available
	return _T("");
}

//=====================================================================

CString CPath::StripFileExtension(CString sPath)
{
	// Declare variables
	int iPos;

	// Now search for the last dot
	for (int i = sPath.GetLength(); i > 0; i--)
	{
		iPos = sPath.Find(_T("."), sPath.GetLength()-(sPath.GetLength()-i));
		// If we found them, delete last piece
		if (iPos != -1)
		{
			sPath.Delete(iPos, sPath.GetLength() - i);
			return sPath;
		}

		// If it is a slash, we should not strip this extension anymore
		if (sPath.Find(_T("\\"), sPath.GetLength()-(sPath.GetLength()-i)) != -1)
			return sPath;
		if (sPath.Find(_T("\\"), sPath.GetLength()-(sPath.GetLength()-i)) != -1)
			return sPath;
	}

	// If we not found, we are already dealing with path
	return sPath;
}

//=====================================================================

CString CPath::ConvertRelativeToFullPath(CString sRelativePath)
{
	// Declare variables
	CString sFullPath, sBuffer;

	// Convert
	GetFullPathName(sRelativePath, _MAX_PATH, sBuffer.GetBuffer(_MAX_PATH), NULL);

	// Get value of buffer
	sFullPath = sBuffer.GetBuffer(sBuffer.GetLength());
	sBuffer.ReleaseBuffer();

	// Return result
	return sFullPath;
}

//=====================================================================

CString CPath::ConvertFullToRelativePath(CString sFullPath)
{
	// Declare variables
	CString sRelativePath, sBuffer;
	DWORD dwPathType;

	// Check if path is pointing to file or folder
	if (PathIsDirectory(sFullPath))
	{
		dwPathType = FILE_ATTRIBUTE_DIRECTORY;
	}
	else
	{
		dwPathType = FILE_ATTRIBUTE_NORMAL;
	}

	// Convert
	PathRelativePathTo(sBuffer.GetBuffer(_MAX_PATH), GetPathUpdater(), FILE_ATTRIBUTE_DIRECTORY, 
		sFullPath, dwPathType);

	// Get value of buffer
	sRelativePath = sBuffer.GetBuffer(sBuffer.GetLength());
	sBuffer.ReleaseBuffer();

	// Return result
	return sRelativePath;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************