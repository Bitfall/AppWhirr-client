/********************************************************************
	created:	2005/03/03
	created:	3:3:2005   10:21
	filename: 	EventActions.h
	file path:	Updater
	file base:	EventActions
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef EVENTACTIONS_H
#define EVENTACTIONS_H

//*********************************************************************
// CActionBase
//*********************************************************************

class CEventActionBase
{
public:
	// Constructor & destructor
	CEventActionBase(CString sType){ m_sType = sType; }

	// Functions
	CString GetType(){ return m_sType; }

private:
	// Functions

	// Variables
	CString m_sType;
};

//*********************************************************************
// CEventActionClose
//*********************************************************************

class CEventActionClose : public CEventActionBase
{
public:
	// Constructor & destructor
	CEventActionClose() : CEventActionBase(EVENTACTION_CLOSE){}
	
	// Functions

private:
	// Functions

	// Variables
};

//*********************************************************************
// CEventActionSetMode
//*********************************************************************

class CEventActionSetMode : public CEventActionBase
{
public:
	// Constructor & destructor
	CEventActionSetMode(CString sMode) : CEventActionBase(EVENTACTION_SETMODE)
	{
		m_sMode = sMode;
	}
	
	// Functions
	CString GetMode(){ return m_sMode; }

private:
	// Functions

	// Variables
	CString m_sMode;
};

//*********************************************************************
// CActionRun
//*********************************************************************

class CEventActionRun : public CEventActionBase
{
public:
	// Constructor & destructor
	CEventActionRun(CString sRunLocation, CString sParameters) : CEventActionBase(EVENTACTION_RUN)
	{
		m_sRunLocation = sRunLocation;
		m_sParameters = sParameters;
	}

	// Functions
	CString GetRunLocation(){ return m_sRunLocation; }
	CString GetParameters(){ return m_sParameters; }

private:
	// Functions

	// Variables
	CString m_sRunLocation;
	CString m_sParameters;
};

//*********************************************************************
// CEventActionSkipNextTask
//*********************************************************************

class CEventActionSkipNextTask : public CEventActionBase
{
public:
	// Constructor & destructor
	CEventActionSkipNextTask() : CEventActionBase(EVENTACTION_SKIPNEXTTASK){}
	
	// Functions

private:
	// Functions

	// Variables
};

//*********************************************************************
// CEventActionStartNextTask
//*********************************************************************

class CEventActionStartNextTask : public CEventActionBase
{
public:
	// Constructor & destructor
	CEventActionStartNextTask() : CEventActionBase(EVENTACTION_STARTNEXTTASK){}
	
	// Functions

private:
	// Functions

	// Variables
};

//*********************************************************************
// CEventActionCloseApplication
//*********************************************************************

class CEventActionCloseApplication : public CEventActionBase
{
public:
	// Constructor & destructor
	CEventActionCloseApplication(CString sTitle, CString sFilename, bool bAskUser) : CEventActionBase(EVENTACTION_CLOSEAPPLICATION)
	{
		m_sTitle = sTitle;
		m_sFilename = sFilename;
		m_bAskUser = bAskUser;
	}
	
	// Functions
	CString GetTitle(){ return m_sTitle; }
	CString GetFilename(){ return m_sFilename; }
	bool	GetAskUser(){ return m_bAskUser; }

private:
	// Functions

	// Variables
	CString m_sTitle;
	CString m_sFilename;
	bool	m_bAskUser;
};

//*********************************************************************
// CEventActionShowNotifier
//*********************************************************************

class CEventActionShowNotifier : public CEventActionBase
{
public:
	// Constructor & destructor
	CEventActionShowNotifier(CString sTitle, CString sDescription) : CEventActionBase(EVENTACTION_SHOWNOTIFIER)
	{
		m_sTitle = sTitle;
		m_sDescription = sDescription;
	}
	
	// Functions
	CString GetTitle(){ return m_sTitle; }
	CString GetDescription(){ return m_sDescription; }

private:
	// Functions

	// Variables
	CString m_sTitle;
	CString m_sDescription;
};

// End of redefinition protection
#endif // EVENTACTIONS_H
