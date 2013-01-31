/********************************************************************
	created:	2005/02/21
	created:	21:2:2005   11:51
	filename: 	ShortcutData.h
	file path:	Updater
	file base:	ShortcutData
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef SHORTCUTDATA_H
#define SHORTCUTDATA_H

//*********************************************************************
// CShortcutData
//*********************************************************************

class CShortcutData  
{
public:
	// Constructor & destructor
	CShortcutData(CString sTargetFile, CString sParameters, CString sWorkingDirectory,
		CString sLinkFile, CString sIconFile, CString sDescription, int iShowMode, int iIconIndex);
	virtual ~CShortcutData();

	// Functions
	CString GetTargetFile();
	CString GetParameters();
	CString GetWorkingDirectory();
	CString GetLinkFile();
	CString GetIconFile();
	CString GetDescription();
	int		GetShowMode();
	int		GetIconIndex();

private:
	// Functions

	// Variables
	CString m_sTargetFile;
	CString m_sParameters;
	CString m_sWorkingDirectory;
	CString m_sLinkFile;
	CString m_sIconFile;
	CString m_sDescription;
	int		m_iShowMode;
	int		m_iIconIndex;
};

// End of redefinition protection
#endif // SHORTCUTDATA_H