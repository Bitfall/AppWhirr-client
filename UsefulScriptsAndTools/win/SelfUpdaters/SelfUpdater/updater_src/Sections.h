/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	Sections.h
	file path:	Updater
	file base:	Sections
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef SECTIONS_H
#define SECTIONS_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

//*********************************************************************
// CCLASSNAME
//*********************************************************************

class CSections  
{
public:
	// Functions
	static CSections * Instance();
	void		ReadFile(CString sFilename);
	void		WriteFile(CString sFilename);
	SectionInfo GetSectionInfo(int iIndex);
	int			GetSectionCount();
	CString		GetSectionVersion(CString sSectionName);
	void		SetSectionVersion(CString sSectionName, CString sVersion);

private:
	// Constructor & destructor
	CSections();			// Private constructor because singleton!

	// Functions
	CString XML_to_CString(CString sFilename);

	// Variables
	static std::auto_ptr<CSections> sm_inst;
	SectionInfo m_arrSections[MAX_SECTIONS];
	int			m_iSectionCount;
};

// End of redefinition protection
#endif // SECTIONS_H