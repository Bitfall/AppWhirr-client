/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	InternalData.cpp
	file path:	Updater
	file base:	InternalData
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "InternalData.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CInternalData> CInternalData::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CInternalData::CInternalData()
{
	// Set default values
	m_bApplicationClosed = false;
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CInternalData * CInternalData::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = auto_ptr<CInternalData>(new CInternalData);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CInternalData);
	return sm_inst.get();
	*/
}

//=====================================================================

bool CInternalData::GetApplicationClosed()
{
	return m_bApplicationClosed;
}

void CInternalData::SetApplicationClosed(bool bClosed)
{
	m_bApplicationClosed = bClosed;
}

//=====================================================================

CString CInternalData::GetSettingsFile()
{
	return m_sSettingsFile;
}

void CInternalData::SetSettingsFile(CString sSettingsFile)
{
	m_sSettingsFile = sSettingsFile;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************
