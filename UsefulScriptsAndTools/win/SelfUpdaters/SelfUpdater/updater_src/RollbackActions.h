/********************************************************************
	created:	2005/03/03
	created:	3:3:2005   10:21
	filename: 	RollbackActions.h
	file path:	Updater
	file base:	RollbackActions
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef ROLLBACKACTIONS_H
#define ROLLBACKACTIONS_H

//*********************************************************************
// CRollbackActionBase
//*********************************************************************

class CRollbackActionBase
{
public:
	// Constructor & destructor
	CRollbackActionBase(int iType){ m_iType = iType; }

	// Functions
	int GetType(){ return m_iType; }

private:
	// Functions

	// Variables
	int m_iType;
};

//*********************************************************************
// CRollbackActionCopy
//*********************************************************************

class CRollbackActionCopy : public CRollbackActionBase
{
public:
	// Constructor & destructor
	CRollbackActionCopy(CString sOldLocation, CString sNewLocation) : CRollbackActionBase(ROLLBACKACTION_COPY)
	{
		m_sOldLocation = sOldLocation;
		m_sNewLocation = sNewLocation;
	}

	// Functions
	CString GetOldLocation(){ return m_sOldLocation; }
	CString GetNewLocation(){ return m_sNewLocation; }

private:
	// Functions

	// Variables
	CString m_sOldLocation;
	CString m_sNewLocation;
};

//*********************************************************************
// CRollbackActionDelete
//*********************************************************************

class CRollbackActionDelete : public CRollbackActionBase
{
public:
	// Constructor & destructor
	CRollbackActionDelete(CString sLocation) : CRollbackActionBase(ROLLBACKACTION_DELETE)
	{
		m_sLocation = sLocation;
	}

	// Functions
	CString GetLocation(){ return m_sLocation; }

private:
	// Functions
	
	// Variables
	CString m_sLocation;
};

//*********************************************************************
// CRollbackActionRegistry
//*********************************************************************

class CRollbackActionRegistry : public CRollbackActionBase
{
public:
	// Constructor & destructor
	CRollbackActionRegistry(CString sKey, CString sValue) : CRollbackActionBase(ROLLBACKACTION_REGISTRY)
	{
		m_sKey = sKey;
		m_sValue = sValue;
	}

	// Functions
	CString GetKey(){ return m_sKey; }
	CString GetValue(){ return m_sValue; }

private:
	// Functions

	// Variables
	CString m_sKey;
	CString m_sValue;
};

//*********************************************************************
// CRollbackActionUnregister
//*********************************************************************

class CRollbackActionUnregister : public CRollbackActionBase
{
public:
	// Constructor & destructor
	CRollbackActionUnregister(CString sFile) : CRollbackActionBase(ROLLBACKACTION_UNREGISTER)
	{
		m_sFile = sFile;
	}

	// Functions
	CString GetFile(){ return m_sFile; }

private:
	// Functions

	// Variables
	CString m_sFile;
};

// End of redefinition protection
#endif // ROLLBACKACTIONS_H
