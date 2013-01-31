/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	USERDATACOLLECTION: 	UserDataCollection.h
	file path:	Updater
	file base:	UserDataCollection
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef USERDATACOLLECTION_H
#define USERDATACOLLECTION_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

//*********************************************************************
// CUserDataCollection
//*********************************************************************

class CUserDataCollection  
{
public:
	// Functions
	static CUserDataCollection * Instance();

private:
	// Constructor & destructor
	CUserDataCollection();			// Private constructor because singleton!

	// Functions

	// Variables
	static std::auto_ptr<CUserDataCollection> sm_inst;
	CString m_sOSLanguage;
	CString m_sCountry;
	CString m_sProductName;
	CString m_sDefaultMode;
	CString m_sOriginalVersion;
	CString m_sNewestVersion;
	CString m_sUpdateResult;
};

// End of redefinition protection
#endif // USERDATACOLLECTION_H