/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	InternalData.h
	file path:	Updater
	file base:	InternalData
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef INTERNALDATA_H
#define INTERNALDATA_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

//*********************************************************************
// CInternalData
//*********************************************************************

class CInternalData  
{
public:
	// Functions
	static  CInternalData * Instance();
	bool	GetApplicationClosed();
	void	SetApplicationClosed(bool bClosed);
	CString	GetSettingsFile();
	void	SetSettingsFile(CString sSettingsFile);

private:
	// Constructor & destructor
	CInternalData();			// Private constructor because singleton!

	// Functions
	bool	m_bApplicationClosed;
	CString m_sSettingsFile;

	// Variables
	static std::auto_ptr<CInternalData> sm_inst;

};

// End of redefinition protection
#endif // INTERNALDATA_H