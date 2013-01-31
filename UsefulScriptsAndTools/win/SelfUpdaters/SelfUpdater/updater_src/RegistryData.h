/********************************************************************
	created:	2005/02/21
	created:	21:2:2005   11:49
	filename: 	RegistryData.h
	file path:	Updater
	file base:	RegistryData
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef REGISTRYDATA_H
#define REGISTRYDATA_H

//*********************************************************************
// CRegistryData
//*********************************************************************

class CRegistryData  
{
public:
	// Constructor & destructor
	CRegistryData(CString sType, CString sKey, CString sValue);
	virtual ~CRegistryData();

	// Functions
	CString GetType();
	CString GetKey();
	CString GetValue();

private:
	// Functions

	// Variables
	CString m_sType;
	CString m_sKey;
	CString m_sValue;
};

// End of redefinition protection
#endif // REGISTRYDATA_H

