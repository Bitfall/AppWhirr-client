/********************************************************************
	created:	2005/02/21
	created:	21:2:2005   11:50
	filename: 	IniData.h
	file path:	Updater
	file base:	IniData
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef INIDATA_H
#define INIDATA_H

//*********************************************************************
// CIniData
//*********************************************************************

class CIniData  
{
public:
	// Constructor & destructor
	CIniData(CString sIniLocation, CString sGroup,
			 CString sKey, CString sValue);
	virtual ~CIniData();

	// Functions
	CString GetIniLocation();
	CString GetGroup();
	CString GetKey();
	CString GetValue();

private:
	// Functions

	// Variables
	CString m_sIniLocation;
	CString m_sGroup;
	CString m_sKey;
	CString m_sValue;
};

// End of redefinition protection
#endif // INIDATA_H

