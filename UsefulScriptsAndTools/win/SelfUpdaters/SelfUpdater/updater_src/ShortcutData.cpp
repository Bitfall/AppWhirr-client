/********************************************************************
	created:	2005/02/21
	created:	21:2:2005   11:55
	filename: 	ShortcutData.cpp
	file path:	Updater
	file base:	ShortcutData
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "ShortcutData.h"

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CShortcutData::CShortcutData(CString sTargetFile, CString sParameters,
							 CString sWorkingDirectory, CString sLinkFile,
							 CString sIconFile, CString sDescription,
							 int iShowMode, int iIconIndex)
{
	// Set default values
	m_sTargetFile = sTargetFile;
	m_sParameters = sParameters;
	m_sWorkingDirectory = sWorkingDirectory;
	m_sLinkFile = sLinkFile;
	m_sIconFile = sIconFile;
	m_sDescription = sDescription;
	m_iShowMode = iShowMode;
	m_iIconIndex = iIconIndex;
}

//=====================================================================

CShortcutData::~CShortcutData()
{

}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CString CShortcutData::GetTargetFile()
{
	// Return value
	return m_sTargetFile;
}

//=====================================================================

CString CShortcutData::GetParameters()
{
	// Return value
	return m_sParameters;
}

//=====================================================================

CString CShortcutData::GetWorkingDirectory()
{
	// Return value
	return m_sWorkingDirectory;
}

//=====================================================================

CString CShortcutData::GetLinkFile()
{
	// Return value
	return m_sLinkFile;
}

//=====================================================================

CString CShortcutData::GetIconFile()
{
	// Return value
	return m_sIconFile;
}

//=====================================================================

CString CShortcutData::GetDescription()
{
	// Return value
	return m_sDescription;
}

//=====================================================================

int	CShortcutData::GetShowMode()
{
	// Return value
	return m_iShowMode;
}

//=====================================================================

int	CShortcutData::GetIconIndex()
{
	// Return value
	return m_iIconIndex;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************