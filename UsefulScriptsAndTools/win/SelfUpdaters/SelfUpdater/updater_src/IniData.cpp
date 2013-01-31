/********************************************************************
	created:	2005/02/21
	created:	21:2:2005   11:54
	filename: 	IniData.cpp
	file path:	Updater
	file base:	IniData
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "IniData.h"

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CIniData::CIniData(CString sIniLocation, CString sGroup,
				   CString sKey, CString sValue)
{
	// Set values
	m_sIniLocation = sIniLocation;
	m_sGroup = sGroup;
	m_sKey = sKey;
	m_sValue = sValue;
}

//=====================================================================

CIniData::~CIniData()
{

}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CString CIniData::GetIniLocation()
{
	// Return value
	return m_sIniLocation;
}

//=====================================================================

CString CIniData::GetGroup()
{
	// Return value
	return m_sGroup;
}

//=====================================================================

CString CIniData::GetKey()
{
	// Return value
	return m_sKey;
}

//=====================================================================

CString CIniData::GetValue()
{
	// Return value
	return m_sValue;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************