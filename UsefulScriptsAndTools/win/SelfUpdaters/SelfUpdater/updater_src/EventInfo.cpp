/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	EventInfo.cpp
	file path:	Updater
	file base:	EventInfo
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "EventInfo.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CEventInfo> CEventInfo::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CEventInfo::CEventInfo()
{
	// Set some default values
	for (int i = 0; i < EVENT_COUNT; i++)
		m_arrEvents[i] = new CEventData();
	
	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pPath = CPath::Instance();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CEventInfo * CEventInfo::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = auto_ptr<CEventInfo>(new CEventInfo);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CEventInfo);
	return sm_inst.get();
	*/
}
//=====================================================================

void CEventInfo::ParseEvents(CMarkup & xml)
{
	// Jump into elem
	xml.IntoElem();

	// Search for OnNewVersion event
	if (xml.FindElem(_T("ONNEWVERSION")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_ONNEWVERSION]);
	}

	// Reset pos
	xml.ResetMainPos();

	// Search for OnNoNewVersion event
	if (xml.FindElem(_T("ONNONEWVERSION")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_ONNONEWVERSION]);
	}

	// Reset pos
	xml.ResetMainPos();

	// Search for BeforeHtmlMessage event
	if (xml.FindElem(_T("BEFOREHTMLMESSAGE")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_BEFOREHTMLMESSAGE]);
	}
	
	// Reset pos
	xml.ResetMainPos();

	// Search for AfterHtmlMessage event
	if (xml.FindElem(_T("AFTERHTMLMESSAGE")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_AFTERHTMLMESSAGE]);
	}

	// Reset pos
	xml.ResetMainPos();

	// Search for BeforeLicense event
	if (xml.FindElem(_T("BEFORELICENSE")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_BEFORELICENSE]);
	}
	
	// Reset pos
	xml.ResetMainPos();

	// Search for AfterLicense event
	if (xml.FindElem(_T("AFTERLICENSE")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_AFTERLICENSE]);
	}

	// Reset pos
	xml.ResetMainPos();

	// Search for BeforeSelectUpdate event
	if (xml.FindElem(_T("BEFORESELECTUPDATE")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_BEFORESELECTUPDATE]);
	}
	
	// Reset pos
	xml.ResetMainPos();

	// Search for AfterSelectUpdate event
	if (xml.FindElem(_T("AFTERSELECTUPDATE")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_AFTERSELECTUPDATE]);
	}

	// Reset pos
	xml.ResetMainPos();

	// Search for BeforeDownload event
	if (xml.FindElem(_T("BEFOREDOWNLOAD")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_BEFOREDOWNLOAD]);
	}
	
	// Reset pos
	xml.ResetMainPos();

	// Search for AfterDownload event
	if (xml.FindElem(_T("AFTERDOWNLOAD")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_AFTERDOWNLOAD]);
	}

	// Reset pos
	xml.ResetMainPos();

	// Search for BeforeInstall event
	if (xml.FindElem(_T("BEFOREINSTALL")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_BEFOREINSTALL]);
	}

	// Reset pos
	xml.ResetMainPos();

	// Search for AfterInstall event
	if (xml.FindElem(_T("AFTERINSTALL")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_AFTERINSTALL]);
	}
	
	// Reset pos
	xml.ResetMainPos();
	
	// Search for BeforeInstall event
	if (xml.FindElem(_T("BEFOREROLLBACK")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_BEFOREROLLBACK]);
	}
	
	// Reset pos
	xml.ResetMainPos();
	
	// Search for AfterInstall event
	if (xml.FindElem(_T("AFTERROLLBACK")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_AFTERROLLBACK]);
	}

	// Reset pos
	xml.ResetMainPos();

	// Search for OnClose event
	if (xml.FindElem(_T("ONCLOSE")))
	{
		// Parse event
		ParseEvent(xml, m_arrEvents[EVENT_ONCLOSE]);
	}

	// Jump out of elem
	xml.OutOfElem();
}

//=====================================================================

CEventData * CEventInfo::GetEventInfo(int iEvent)
{
	// Check what event info to return
	return m_arrEvents[iEvent];
}

//=====================================================================

void CEventInfo::CleanUp()
{
	// Clean up
	for (int i = 0; i < EVENT_COUNT; i++)
	{
		// Remove all even actions
		for (int j = 0; j < m_arrEvents[i]->GetActionCount(); j++)
		{
			// Delete action
			delete m_arrEvents[i]->GetAction(j);
		}

		// Delete event
		delete m_arrEvents[i];
	}
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CEventInfo::ParseEvent(CMarkup & xml, CEventData * pEventData)
{
	// Declare variables
	CString sTemp;
	CEventActionBase * pAction;

	// Jump into element
	xml.IntoElem();

	// While we find actions
	while (xml.FindElem(_T("ACTION")))
	{
		// Check what action we need to parse
		sTemp = xml.GetAttrib(_T("type"));		

		// Run action
		if (sTemp == EVENTACTION_RUN)
		{
			pAction = ParseActionRun(xml);
		}

		// Close action
		if (sTemp == EVENTACTION_CLOSE)
		{
			// We know we should close
			pAction = new CEventActionClose();
		}

		// Set mode action
		if (sTemp == EVENTACTION_SETMODE)
		{
			pAction = ParseActionSetMode(xml);
		}

		// Skip next task action
		if (sTemp == EVENTACTION_SKIPNEXTTASK)
		{
			// We know we should skip next task
			pAction = new CEventActionSkipNextTask();
		}

		// Start next task action
		if (sTemp == EVENTACTION_STARTNEXTTASK)
		{
			// We know we should start next task
			pAction = new CEventActionStartNextTask();
		}

		// Close application action
		if (sTemp == EVENTACTION_CLOSEAPPLICATION)
		{
			pAction = ParseActionCloseApplication(xml);
		}

		// Show notifier action
		if (sTemp == EVENTACTION_SHOWNOTIFIER)
		{
			pAction = ParseActionShowNotifier(xml);
		}

		// Add the action to the event
		if (pEventData != NULL)
			pEventData->AddAction(pAction);
	}

	// Jump out of element
	xml.OutOfElem();
}

//=====================================================================

CEventActionBase * CEventInfo::ParseActionRun(CMarkup & xml)
{
	// Declare variables
	CString sLocation, sParameters;
	CEventActionBase * pAction;
	
	// Jump into element
	xml.IntoElem();

	xml.FindElem(_T("LOCATION"));
	sLocation = m_pPath->ReplacePathConstants(xml.GetElemContent());

	if (!xml.FindElem(_T("PARAMETERS")))
		sParameters = _T("");
	else
		sParameters = m_pPath->ReplacePathConstants(xml.GetElemContent());

	// Create object
	pAction = new CEventActionRun(sLocation, sParameters);

	// Jump out of element
	xml.OutOfElem();

	// Return the object
	return pAction;
}

//=====================================================================

CEventActionBase * CEventInfo::ParseActionSetMode(CMarkup & xml)
{
	// Declare variables
	CString sMode;
	CEventActionBase * pAction;

	// Jump into element
	xml.IntoElem();
	
	xml.FindElem(_T("MODE"));
	sMode = xml.GetElemContent();
	
	// Create object
	pAction = new CEventActionSetMode(sMode);

	// Jump out of element
	xml.OutOfElem();

	// Return the object
	return pAction;
}

//=====================================================================

CEventActionBase * CEventInfo::ParseActionCloseApplication(CMarkup & xml)
{
	// Declare variables
	CString sTitle, sFilename, sTemp, sLowerTemp;;
	CEventActionBase * pAction;
	bool bAskUser;

	// Jump into element
	xml.IntoElem();

	// Get title
	xml.FindElem(_T("TITLE"));
	sTitle = xml.GetElemContent();
	
	// Reset pointer
	xml.ResetMainPos();

	// Get filename
	xml.FindElem(_T("FILENAME"));
	sFilename = xml.GetElemContent();
	
	// Reset pointer
	xml.ResetMainPos();

	// Get ask user
	if (xml.FindElem(_T("ASKUSER")))
	{
		// Get data
		sTemp = xml.GetElemContent();

		// Make lowercase
		sTemp = StringLower(sTemp);

		bAskUser = (sTemp == _T("false")) ? false : true;
	}
	else
	{
		// Use default value (true)
		bAskUser = true;
	}

	// Create object
	pAction = new CEventActionCloseApplication(sTitle, sFilename, bAskUser);

	// Jump out of element
	xml.OutOfElem();

	// Return the object
	return pAction;
}

//=====================================================================

CEventActionBase * CEventInfo::ParseActionShowNotifier(CMarkup & xml)
{
	// Declare variables
	CString sTitle, sDescription;
	CEventActionBase * pAction;

	// Jump into element
	xml.IntoElem();
	
	xml.FindElem(_T("TITLE"));
	sTitle = xml.GetElemContent();

	// Reset pointer
	xml.ResetMainPos();

	xml.FindElem(_T("DESCRIPTION"));
	sDescription = xml.GetElemContent();
	
	// Create object
	pAction = new CEventActionShowNotifier(sTitle, sDescription);

	// Jump out of element
	xml.OutOfElem();

	// Return the object
	return pAction;
}