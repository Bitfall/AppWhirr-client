/********************************************************************
	created:	2005/02/21
	created:	21:2:2005   11:53
	filename: 	RegistryData.cpp
	file path:	Updater
	file base:	RegistryData
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "RegistryData.h"

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CRegistryData::CRegistryData(CString sType, CString sKey, CString sValue)
{
	// Set values
	m_sType = sType;
	m_sKey = sKey;
	m_sValue = sValue;
}

//=====================================================================

CRegistryData::~CRegistryData()
{

}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CString CRegistryData::GetType()
{
	// Return value
	return m_sType;
}

//=====================================================================

CString CRegistryData::GetKey()
{
	// Return value
	return m_sKey;
}

//=====================================================================

CString CRegistryData::GetValue()
{
	// Return value
	return m_sValue;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************