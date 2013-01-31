/********************************************************************
	created:	2005/03/04
	created:	4:3:2005   11:03
	filename: 	EventData.h
	file path:	Updater
	file base:	EventData
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef EVENTDATA_H
#define EVENTDATA_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"
#include "EventActions.h"

//*********************************************************************
// CEventData
//*********************************************************************

class CEventData  
{
public:
	// Constructor & destructor
	CEventData();
	virtual ~CEventData();

	// Functions
	int				   GetActionCount();
	CEventActionBase * GetAction(int iIndex);
	void			   AddAction(CEventActionBase * pAction);

private:
	// Functions

	// Variables
	int				   m_iEventActionCount;
	CEventActionBase * m_arrEventActions[MAX_ACTIONS];
};

// End of redefinition protection
#endif // EVENTDATA_H