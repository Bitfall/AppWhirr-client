/********************************************************************
	created:	2005/03/04
	created:	4:3:2005   11:02
	filename: 	EventData.cpp
	file path:	Updater
	file base:	EventData
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "EventData.h"

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CEventData::CEventData()
{
	// Set default values
	m_iEventActionCount = 0;
}

//=====================================================================

CEventData::~CEventData()
{

}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

int CEventData::GetActionCount()
{
	// Return action count
	return m_iEventActionCount;
}

//=====================================================================

CEventActionBase * CEventData::GetAction(int iIndex)
{
	// Return event
	return m_arrEventActions[iIndex];
}

//=====================================================================

void CEventData::AddAction(CEventActionBase * pAction)
{
	// Add event to array
	m_arrEventActions[m_iEventActionCount++] = pAction;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************