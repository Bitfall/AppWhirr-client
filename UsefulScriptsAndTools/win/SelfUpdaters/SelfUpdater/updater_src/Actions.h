/********************************************************************
	created:	2005/03/03
	created:	3:3:2005   10:21
	filename: 	Actions.h
	file path:	Updater
	file base:	Actions
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef ACTIONS_H
#define ACTIONS_H

//*********************************************************************
// CActionBase
//*********************************************************************

class CActionBase
{
public:
	// Constructor & destructor
	CActionBase(CString sType){ m_sType = sType; }

	// Functions
	CString GetType(){ return m_sType; }

private:
	// Functions

	// Variables
	CString m_sType;
};

//*********************************************************************
// CActionDownload
//*********************************************************************

class CActionDownload : public CActionBase
{
public:
	// Constructor & destructor
	CActionDownload(CString sDestination, bool bSubscribeToGarbageCollector = true) : CActionBase(ACTION_DOWNLOAD) 
	{ 
		m_iDownloadLocationCount = 0; 
		m_sDestination = sDestination;
		m_bSubscribeToGarbageCollector = bSubscribeToGarbageCollector;
	}
	
	// Functions
	void	AddDownloadLocation(CString sDownloadLocation)
	{
		m_arrDownloadLocation[m_iDownloadLocationCount++] = sDownloadLocation;
	}
	int		GetDownloadLocationCount() { return m_iDownloadLocationCount; }
	CString GetDownloadLocation(int iIndex){ return m_arrDownloadLocation[iIndex]; }
	CString GetDownloadDestination() { return m_sDestination; }
	bool	SubscribeToGarbageCollector() { return m_bSubscribeToGarbageCollector; }

private:
	// Functions

	// Variables
	int		m_iDownloadLocationCount;
	CString m_arrDownloadLocation[MAX_DOWNLOADLOCATIONS];
	CString m_sDestination;
	bool	m_bSubscribeToGarbageCollector;
};

//*********************************************************************
// CActionCopy
//*********************************************************************

class CActionCopy :	public CActionBase
{
public:
	// Constructor & destructor
	CActionCopy(CString sOldLocation, CString sNewLocation,
				bool bOverwrite) : CActionBase(ACTION_COPY)
	{
		m_sOldLocation = sOldLocation;
		m_sNewLocation = sNewLocation;
		m_bOverwrite = bOverwrite;
	}

	// Functions
	CString GetOldLocation(){ return m_sOldLocation; }
	CString GetNewLocation(){ return m_sNewLocation; }
	bool	GetOverwrite(){ return m_bOverwrite; }

private:
	// Functions

	// Variables
	CString m_sOldLocation;
	CString m_sNewLocation;
	bool	m_bOverwrite;
};

//*********************************************************************
// CActionRun
//*********************************************************************

class CActionRun : public CActionBase
{
public:
	// Constructor & destructor
	CActionRun(CString sRunLocation, CString sParameters,
			   bool bWaitUntilReady) : CActionBase(ACTION_RUN)
	{
		m_sRunLocation = sRunLocation;
		m_sParameters = sParameters;
		m_bWaitUntilReady = bWaitUntilReady;
	}

	// Functions
	CString GetRunLocation(){ return m_sRunLocation; }
	CString GetParameters(){ return m_sParameters; }
	bool	GetWaitUntilReady(){ return m_bWaitUntilReady; }

private:
	// Functions

	// Variables
	CString m_sRunLocation;
	CString m_sParameters;
	bool	m_bWaitUntilReady;
};

//*********************************************************************
// CActionDelete
//*********************************************************************

class CActionDelete : public CActionBase
{
public:
	// Constructor & destructor
	CActionDelete(CString sDeleteLocation, bool bAskConfirmation) : CActionBase(ACTION_DELETE)
	{
		m_sDeleteLocation = sDeleteLocation;
		m_bAskConfirmation = bAskConfirmation;
	}
	
	// Functions
	CString GetDeleteLocation(){ return m_sDeleteLocation; }
	bool	GetAskConfirmation(){ return m_bAskConfirmation; }

private:
	// Functions

	// Variables
	CString m_sDeleteLocation;
	bool	m_bAskConfirmation;
};

//*********************************************************************
// CActionUnzip
//*********************************************************************

class CActionUnzip : public CActionBase
{
public:
	// Constructor & destructor
	CActionUnzip(CString sLocation, CString sDestination) : CActionBase(ACTION_UNZIP)
	{
		m_sLocation = sLocation;
		m_sDestination = sDestination;
	}
	
	// Functions
	CString GetLocation(){ return m_sLocation; }
	CString GetDestination(){ return m_sDestination; }

private:
	// Functions

	// Variables
	CString m_sLocation;
	CString m_sDestination;
};

//*********************************************************************
// CActionSetFileAttr
//*********************************************************************

class CActionSetFileAttr : public CActionBase
{
public:
	// Constructor & destructor
	CActionSetFileAttr(CString sLocation, int iAttributes) : CActionBase(ACTION_SETFILEATTR)
	{
		m_sLocation = sLocation;
		m_iAttributes = iAttributes;
	}
	
	// Functions
	CString GetLocation(){ return m_sLocation; }
	int     GetAttributes(){ return m_iAttributes; }

private:
	// Functions

	// Variables
	CString m_sLocation;
	int     m_iAttributes;
};

//*********************************************************************
// CActionRegister
//*********************************************************************

class CActionRegister : public CActionBase
{
public:
	// Constructor & destructor
	CActionRegister(CString sLocation, bool bAskUser) : CActionBase(ACTION_REGISTER)
	{
		m_sLocation = sLocation;
		m_bAskUser = bAskUser;
	}
	
	// Functions
	CString GetLocation(){ return m_sLocation; }
	bool    GetAskUser(){ return m_bAskUser; }

private:
	// Functions

	// Variables
	CString m_sLocation;
	bool    m_bAskUser;
};

// End of redefinition protection
#endif // ACTIONS_H
