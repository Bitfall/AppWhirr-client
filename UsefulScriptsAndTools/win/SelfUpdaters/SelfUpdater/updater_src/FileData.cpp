/********************************************************************
	created:	2005/02/21
	created:	21:2:2005   11:55
	filename: 	FileData.cpp
	file path:	Updater
	file base:	FileData
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "FileData.h"

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CFileData::CFileData(CString sName, CString sFilename, CString sVersion,
					 CString sDate, CString sHash, CString sCheckType,
					 CString sSection)
{
	// Set default values
	m_sName = sName;
	m_sFilename = sFilename;
	m_sVersion = sVersion;
	m_sDate = sDate;
	m_sHash = sHash;
	m_sCheckType = sCheckType;
	m_sSection = sSection;
	m_iActionCount = 0;
	m_bErrorFree = true;
	m_bUpdateFile = true;
}

//=====================================================================

CFileData::~CFileData()
{

}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CString CFileData::GetName()
{
	// Return value
	return m_sName;
}

//=====================================================================

CString CFileData::GetFilename()
{
	// Return value
	return m_sFilename;
}

//=====================================================================

CString CFileData::GetVersion()
{
	// Return value
	return m_sVersion;
}

//=====================================================================

CString CFileData::GetDate()
{
	// Return value
	return m_sDate;
}

//=====================================================================

CString CFileData::GetHash()
{
	// Return value
	return m_sHash;
}

//=====================================================================

CString CFileData::GetCheckType()
{
	// Return value
	return m_sCheckType;
}

//=====================================================================

CString CFileData::GetSection()
{
	// Return value
	return m_sSection;
}

//=====================================================================

int	CFileData::GetActionCount()
{
	// Return value
	return m_iActionCount;
}

//=====================================================================

CActionBase * CFileData::GetAction(int index)
{
	// Return value
	return m_arrActions[index];
}

//=====================================================================

bool CFileData::GetErrorFree()
{
	// Return value
	return m_bErrorFree;
}

//=====================================================================

void CFileData::SetErrorFree(bool bErrorFree)
{
	// Set value
	m_bErrorFree = bErrorFree;
}

//=====================================================================

bool CFileData::GetUpdateFile()
{
	// Return value
	return m_bUpdateFile;
}

//=====================================================================

void CFileData::SetUpdateFile(bool bUpdateFile)
{
	// Set value
	m_bUpdateFile = bUpdateFile;
}

//=====================================================================

void CFileData::AddAction(CActionBase * pAction)
{
	// Add object to array
	m_arrActions[m_iActionCount++] = pAction;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************