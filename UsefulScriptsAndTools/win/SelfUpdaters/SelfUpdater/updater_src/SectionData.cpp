/********************************************************************
	created:	2005/02/21
	created:	21:2:2005   11:55
	filename: 	SectionData.cpp
	file path:	Updater
	file base:	SectionData
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "SectionData.h"

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CSectionData::CSectionData(CString sName, CString sTitle, CString sVersion,
						   CString sDescription, CString sParent, CString sURL,
						   bool bCheck, bool bEnable)
{
	// Set default values
	m_sName = sName;
	m_sTitle = sTitle;
	m_sVersion = sVersion;
	m_sDescription = sDescription;
	m_sParent = sParent;
	m_sURL = sURL;
	m_bCheck = bCheck;
	m_bEnable = bEnable;
	m_bSelectedByUser = bCheck;
}

//=====================================================================

CSectionData::~CSectionData()
{

}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CString CSectionData::GetName()
{
	// Return value
	return m_sName;
}

//=====================================================================

CString CSectionData::GetTitle()
{
	// Return value
	return m_sTitle;
}

//=====================================================================

CString CSectionData::GetVersion()
{
	// Return value
	return m_sVersion;
}

//=====================================================================

CString CSectionData::GetDescription()
{
	// Return value
	return m_sDescription;
}

//=====================================================================

CString CSectionData::GetParent()
{
	// Return value
	return m_sParent;
}

//=====================================================================

CString CSectionData::GetURL()
{
	// Return value
	return m_sURL;
}

//=====================================================================

bool CSectionData::GetCheck()
{
	// Return value
	return m_bCheck;
}

//=====================================================================

bool CSectionData::GetEnable()
{
	// Return value
	return m_bEnable;
}

//=====================================================================

void CSectionData::SetSelectedByUser(bool bSelected /* = true */)
{
	// Set value
	m_bSelectedByUser = bSelected;
}

//=====================================================================

bool CSectionData::GetSelectedByUser()
{
	// Return value
	return m_bSelectedByUser;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************