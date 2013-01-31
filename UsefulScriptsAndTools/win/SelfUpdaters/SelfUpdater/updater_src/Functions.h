/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	Functions.h
	file path:	Updater
	file base:	Functions
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

//*********************************************************************
// CFunctions
//*********************************************************************

class CFunctions  
{
public:
	// Functions
	static CFunctions * Instance();
	bool    Init();
	CString GetFileVersion(CString sFilename);
	int		GetFileSize(CString sFilename);
	CString GetFileDate(CString sFilename);
	CString GetFileDateTime(CString sFilename);
	CString GetFileMD5Hash(CString sFilename);
	CString ReplaceConnectionSettings(CString sText);
	CString ExtractRegistryBase(CString sKey);
	CString ExtractRegistryKey(CString sKey);
	CString ExtractRegistryItem(CString sKey);
	CString StripFileStatus(CString sFilename);
	void    DeleteAllFiles(CString sFolder, bool bDeleteFolderToo = false);
	bool	CreateFolder(CString sFolder);
	int		CountFiles(CString sFolder, bool bSubfolders = true);
	int		GetFolderSize(CString sFolder, bool bSubfolders = true);
	bool	IsWebAddress(CString sString);
	int		CompareVersions(CString sVersion1, CString sVersion2);
	bool	ApplicationRunning(CString sFilename);
	bool	CloseApplication(CString sApplicationName, CString sFilename);

private:
	// Constructor & destructor
	CFunctions();			// Private constructor because singleton!

	// Functions
	CString FindVersionSeperator(CString sVersion);

	// Variables
	static std::auto_ptr<CFunctions> sm_inst;
	CPath * m_pPath;
};

// End of redefinition protection
#endif // FUNCTIONS_H