/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	EventInfo.h
	file path:	Updater
	file base:	EventInfo
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef EVENTINFO_H
#define EVENTINFO_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

#include "EventData.h"
#include "Markup.h"				// XML support

//*********************************************************************
// CEventInfo
//*********************************************************************

class CEventInfo  
{
public:
	// Functions
	static CEventInfo * Instance();

	// Variables
	void		 ParseEvents(CMarkup & xml);
	CEventData * GetEventInfo(int iEvent);
	void		 CleanUp();

private:
	// Constructor & destructor
	CEventInfo();			// Private constructor because singleton!

	// Functions
	void ParseEvent(CMarkup & xml, CEventData * pEventData);
	CEventActionBase * ParseActionRun(CMarkup & xml);
	CEventActionBase * ParseActionSetMode(CMarkup & xml);
	CEventActionBase * ParseActionCloseApplication(CMarkup & xml);
	CEventActionBase * ParseActionShowNotifier(CMarkup & xml);

	// Variables
	static std::auto_ptr<CEventInfo> sm_inst;
	CEventData * m_arrEvents[EVENT_COUNT];
	CFunctions * m_pFunctions;
	CPath	   * m_pPath;
};

// End of redefinition protection
#endif // EVENTINFO_H