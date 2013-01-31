/********************************************************************
	created:	2005/02/21
	created:	21:2:2005   11:51
	filename: 	SectionData.h
	file path:	Updater
	file base:	SectionData
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef SECTIONDATA_H
#define SECTIONDATA_H

//*********************************************************************
// CSectionData
//*********************************************************************

class CSectionData  
{
public:
	// Constructor & destructor
	CSectionData(CString sName, CString sTitle, CString sVersion,
				 CString sDescription, CString sParent, CString sURL,
				 bool bCheck, bool bEnable);
	virtual ~CSectionData();

	// Functions
	CString GetName();
	CString GetTitle();
	CString GetVersion();
	CString	GetDescription();
	CString GetParent();
	CString GetURL();
	bool 	GetCheck();
	bool	GetEnable();
	void	SetSelectedByUser(bool bSelected = true);
	bool	GetSelectedByUser();

private:
	// Functions

	// Variables
	CString	m_sName;
	CString	m_sTitle;
	CString	m_sVersion;
	CString	m_sDescription;
	CString m_sParent;
	CString m_sURL;
	bool	m_bCheck;
	bool	m_bEnable;
	bool	m_bSelectedByUser;
};

// End of redefinition protection
#endif // SECTIONDATA_H