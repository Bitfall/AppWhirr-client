/********************************************************************
	created:	2005/05/02
	created:	2:5:2005   10:57
	filename: 	SelfUpdateInfo.h
	file path:	Updater
	file base:	SelfUpdateInfo
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef SELFUPDATEINFO_H
#define SELFUPDATEINFO_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CSelfUpdateInfo
//*********************************************************************

class CSelfUpdateInfo
{
public:
	// Functions
	static CSelfUpdateInfo * Instance();

	// Variables
	void	ReadFile(CString sUpdateFile);	// Read XML selfupdate file
	CString GetDownloadLocation();
	CString GetVersion();

private:
	// Constructor & destructor
	CSelfUpdateInfo();			// Private constructor because singleton!
	
	// Functions
	CString XML_to_CString(CString sFilename);

	// Variables
	static std::auto_ptr<CSelfUpdateInfo> sm_inst;
	CString m_sDownloadLocation;
	CString m_sVersion;
};

// End of redefinition protection
#endif // SELFUPDATEINFO_H
