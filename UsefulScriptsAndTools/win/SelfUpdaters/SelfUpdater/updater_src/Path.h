/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	Path.h
	file path:	Updater
	file base:	Path
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef PATH_H
#define PATH_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

//*********************************************************************
// CPath
//*********************************************************************

class CPath  
{
public:
	// Functions
	static CPath * Instance();
	bool    Init();
	bool	CreateUpdaterFolders();
	CString GetPathUpdater();
	CString GetPathUpdaterExecutable();
	CString GetPathUpdaterTemp();
	CString GetPathRollback();
	CString GetPathTemp();
	CString GetPathSystem();
	CString GetPathProgramfiles();
	CString GetPathApp();
	CString GetPathUserProfile();
	CString GetPathAppData();
	CString GetPathCommonAppData();
	CString GetPathQuickLaunchCurrentUser();
	CString GetPathMenuStartCurrentUser();
	CString GetPathDesktopCurrentUser();
	CString GetPathQuickLaunchAllUsers();
	CString GetPathMenuStartAllUsers();
	CString GetPathDesktopAllUsers();
	CString ReplacePathConstants(CString sText);
	CString ExtractFileName(CString sPath, CString sSplitter = _T("\\"));
	CString ExtractFilePath(CString sPath, CString sSplitter = _T("\\"));
	CString ExtractDrive(CString sPath, CString sSplitter = _T("\\"));
	CString ExtractFileExtension(CString sPath);
	CString StripFileExtension(CString sPath);
	CString ConvertRelativeToFullPath(CString sRelativePath);
	CString ConvertFullToRelativePath(CString sFullPath);

private:
	// Constructor & destructor
	CPath();			// Private constructor because singleton!
	
	// Functions

	// Variables
	static std::auto_ptr<CPath> sm_inst;
	CString m_sPathUpdater;						// Path to updater
	CString m_sPathUpdaterExecutable;			// Path to updater executable
	CString m_sPathApp;							// Path to application
	CString m_sPathUpdaterTemp;					// Path to updater temp files
	CString m_sPathRollback;					// Path to rollback files
	CString m_sPathTemp;						// Path to system temp files
	CString m_sPathProgramfiles;				// Path to program files
	CString m_sPathSystem;						// Path to system folder
	CString m_sPathUserProfile;					// Path to user home directory
	CString m_sPathAppData;						// Path to application data
	CString m_sPathCommonAppData;				// Path to common application data
	CString m_sPathQuickLaunchCurrentUser;		// Path to quick launch menu - current user
	CString m_sPathMenuStartCurrentUser;		// Path to menu start - current user
	CString m_sPathDesktopCurrentUser;			// Path to desktop - current user
	CString m_sPathQuickLaunchAllUsers;			// Path to quick launch menu - all users
	CString m_sPathMenuStartAllUsers;			// Path to menu start - all users
	CString m_sPathDesktopAllUsers;				// Path to desktop - all users
	CString m_sPathWindowsDirectory;			// Path to windows directory
	CString m_sPathUpdateServerPath;			// Server + path where updateinfo is located
	CString m_sPathUpdateServerFile;			// Server + path + file where updateinfo is located
};

// End of redefinition protection
#endif // PATH_H