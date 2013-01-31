/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	UpdateInfo.h
	file path:	Updater
	file base:	UpdateInfo
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef UPDATEINFO_H
#define UPDATEINFO_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

#include "SectionData.h"		// Section data
#include "FileData.h"			// File data
#include "IniData.h"			// Ini data
#include "RegistryData.h"		// Registry data
#include "PopupMenuData.h"		// Popup menu data
#include "ShortcutData.h"		// Shortcut data

#include "Markup.h"				// XML support

//*********************************************************************
// CUSTOM TYPES
//*********************************************************************

struct Constant
{
	CString sName;			// Name of the constant
	CString sValue;			// Value of the constant
};

//*********************************************************************
// CUpdateInfo
//*********************************************************************

class CUpdateInfo  
{
public:
	// Functions
	static CUpdateInfo * Instance();
	CString			 GetPreviousUpdate(CString sUpdateFile);	// If return "", use this file, other download previous one
	bool             ReadFile(CString sUpdateFile);				// Read XML update file
	void			 ReadSelfUpdateFile(CString sUpdateFile);	// Read self update file
	CString          GetNewVersion();							// Get update version
	CString			 GetLicense();								// Get license
	int              GetCloseApplication();						// Get close app
	bool             GetCheckSeparately();
	bool             GetRollbackEnabled();
	bool			 GetShowErrors();
	bool			 GetSelfUpdate();
	bool			 GetResumeDownloads();
	bool			 GetForceUpdate();
	bool			 GetShortcutsAskUser();
	bool			 GetShortcutsCreateShortcuts();
	void			 SetShortcutsCreateShortcuts(bool bCreate = true);
	CString			 GetHtmlMessageURL();
	int				 GetHtmlMessageTime();
	CSectionData   * GetSectionData(int index);
	int              GetSectionCount();
	CFileData      * GetFileData(int index);
	int              GetFileCount();
	CIniData       * GetIniData(int index);
	int              GetIniCount();
	CRegistryData  * GetRegistryData(int index);
	int              GetRegistryCount();
	FeatureData      GetFeature(int index);
	int              GetFeatureCount();
	CPopupMenuData * GetPopupMenuData(int index);
	int				 GetPopupMenuCount();
	CShortcutData  * GetShortcutData(int index);
	int				 GetShortcutCount();
	ProtectionHashData GetProtectionHashData(int index);
	int				 GetProtectionHashCount();
	CString			 GetProtectionRegistrationDll();
	CString			 GetProtectionWebsite();
	void			 CleanUp();

	// Variables

private:
	// Constructor & destructor
	CUpdateInfo();			// Private constructor because singleton!

	// Functions
	void	LoadXML(CString sFilename, CMarkup & xml);
	bool	ParseXML(CMarkup & xml);
	bool	ParseXMLConstants(CMarkup & xml);
	bool	ParseXMLHtmlMessage(CMarkup & xml);
	bool	ParseXMLGeneralInformation(CMarkup & xml);
	bool	ParseXMLProtection(CMarkup & xml);
	bool	ParseXMLSections(CMarkup & xml, CString sParent = "");
	bool	ParseXMLFeatures(CMarkup & xml);
	bool	ParseXMLFiles(CMarkup & xml);
	bool	ParseXMLIni(CMarkup & xml);
	bool	ParseXMLRegistry(CMarkup & xml);
	bool	ParseXMLPopupMenu(CMarkup & xml);
	bool	ParseXMLShortcuts(CMarkup & xml);
	bool	ParseXMLFileDownload(CMarkup & xml, CFileData * pFileData);
	bool	ParseXMLFileCopy(CMarkup & xml, CFileData * pFileData);
	bool	ParseXMLFileDelete(CMarkup & xml, CFileData * pFileData);
	bool	ParseXMLFileRun(CMarkup & xml, CFileData * pFileData);
	bool	ParseXMLFileUnzip(CMarkup & xml, CFileData * pFileData);
	bool	ParseXMLFileSetFileAttr(CMarkup & xml, CFileData * pFileData);
	bool	ParseXMLFileRegister(CMarkup & xml, CFileData * pFileData);
	CString	ReplaceConstants(CString sText);
	CString	ReplaceCustomConstants(CString sText);
	CString XML_to_CString(CString sFilename);

	// Variables
	static std::auto_ptr<CUpdateInfo> sm_inst;
	CFunctions  * m_pFunctions;					// General functions
	CPath		* m_pPath;
	CLog		* m_pLog;
	CSectionData * m_arrSectionData[MAX_SECTIONS];	// Array with sections
	int		    m_iSectionCount;				// Number of sections
	CFileData   * m_arrFileData[MAX_ACTIONS];	// Array with files
	int         m_iFileCount;					// Number of files
	CIniData    * m_arrIniData[MAX_ACTIONS];	// Array with ini settings
	int         m_iIniCount;					// Number of ini settings
	CRegistryData * m_arrRegistryData[MAX_ACTIONS];	// Array with registry settings
	int         m_iRegistryCount;				// Number of registry settings
	FeatureData m_arrFeatures[MAX_FEATURES];	// Array with features
	int         m_iFeatureCount;				// Number of features
	CPopupMenuData * m_arrPopupMenuData[MAX_POPUPMENUITEMS]; // Array with popup menu items
	int		    m_iPopupMenuCount;				// Number of popup menu items
	CShortcutData  * m_arrShortcutData[MAX_SHORTCUTS]; // Array with shortcut items
	int			m_iShortcutCount;				// Number of shortcut items
	Constant    m_arrCustomConstants[MAX_CUSTOMCONSTANTS]; // Array with custom constants
	int		    m_iCustomConstantsCount;		// Number of custom constants
	ProtectionHashData m_arrProtectionHashData[MAX_PROTECTIONHASHES];
	int			m_iProtectionHashCount;
	CString		m_sProtectionRegistrationDll;
	CString		m_sProtectionWebsite;
	CString     m_sNewVersion;					// Newest version
	CString	    m_sLicense;						// License
	CString	    m_sHtmlMessageURL;				// URL to html message
	int		    m_iHtmlMessageTime;				// Time to show html at minimum
	int         m_iCloseApplication;			// Can be yes, no or restart (see structs.h)
	bool        m_bCheckSeparately;				// True if all files should be version checked separately
	bool        m_bRollbackEnabled;				// True if rollback should be enabled
	bool	    m_bShowErrors;					// True if errors should be shown to user
	bool	    m_bSelfUpdate;					// True if Updater should selfupdate when possible
	bool	    m_bResumeDownloads;			    // True if Updater should try to resume broken downloads
	bool		m_bForceUpdate;					// True if user should be forced to install updates
	bool		m_bShortcutsAskUser;			// True is user should be asked to install the shortcuts
	bool		m_bShortcutsCreateShortcuts;	// True if shortcuts should be created
};

// End of redefinition protection
#endif // UPDATEINFO_H