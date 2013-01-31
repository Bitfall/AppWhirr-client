/********************************************************************
	created:	2005/03/03
	created:	3:3:2005   10:21
	filename: 	RestoreActions.h
	file path:	Updater
	file base:	RestoreActions
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef RESTOREACTIONS_H
#define RESTOREACTIONS_H

//*********************************************************************
// CRestoreActionBase
//*********************************************************************

class CRestoreActionBase
{
public:
	// Constructor & destructor
	CRestoreActionBase(int iType){ m_iType = iType; }

	// Functions
	int GetType(){ return m_iType; }
	virtual CString GetXmlOutput() = 0;

private:
	// Functions

	// Variables
	int m_iType;
};

//*********************************************************************
// CRestoreActionCopy
//*********************************************************************

class CRestoreActionCopy : public CRestoreActionBase
{
public:
	// Constructor & destructor
	CRestoreActionCopy(CString sOldLocation, CString sNewLocation) : CRestoreActionBase(RESTOREACTION_COPY)
	{
		m_sOldLocation = sOldLocation;
		m_sNewLocation = sNewLocation;
	}

	// Functions
	CString GetOldLocation(){ return m_sOldLocation; }
	CString GetNewLocation(){ return m_sNewLocation; }
	CString GetXmlOutput()
	{
		// Declare variables
		CString sXML = _T("");

		// Add data
		sXML.Format(_T("    <ACTION type=\"copy\" old=\"%s\" new=\"%s\" />\r\n"),
			m_sOldLocation, m_sNewLocation);

		// Return value
		return sXML;
	}

private:
	// Functions

	// Variables
	CString m_sOldLocation;
	CString m_sNewLocation;
};

//*********************************************************************
// CRestoreActionDelete
//*********************************************************************

class CRestoreActionDelete : public CRestoreActionBase
{
public:
	// Constructor & destructor
	CRestoreActionDelete(CString sLocation) : CRestoreActionBase(RESTOREACTION_DELETE)
	{
		m_sLocation = sLocation;
	}

	// Functions
	CString GetLocation(){ return m_sLocation; }
	CString GetXmlOutput()
	{
		// Declare variables
		CString sXML = _T("");

		// Add data
		sXML.Format(_T("    <ACTION type=\"delete\" location=\"%s\" />\r\n"), m_sLocation);

		// Return value
		return sXML;
	}

private:
	// Functions
	
	// Variables
	CString m_sLocation;
};

//*********************************************************************
// CRestoreActionRegistry
//*********************************************************************

class CRestoreActionRegistry : public CRestoreActionBase
{
public:
	// Constructor & destructor
	CRestoreActionRegistry(CString sKey, CString sValue) : CRestoreActionBase(RESTOREACTION_REGISTRY)
	{
		m_sKey = sKey;
		m_sValue = sValue;
	}

	// Functions
	CString GetKey(){ return m_sKey; }
	CString GetValue(){ return m_sValue; }
	CString GetXmlOutput()
	{
		// Declare variables
		CString sXML = _T("");

		// Add data
		sXML.Format(_T("    <ACTION type=\"registry\" key=\"%s\" value=\"%s\" />\r\n"),
			m_sKey, m_sValue);

		// Return value
		return sXML;
	}

private:
	// Functions

	// Variables
	CString m_sKey;
	CString m_sValue;
};

//*********************************************************************
// CRestoreActionUnregister
//*********************************************************************

class CRestoreActionUnregister : public CRestoreActionBase
{
public:
	// Constructor & destructor
	CRestoreActionUnregister(CString sFile) : CRestoreActionBase(RESTOREACTION_UNREGISTER)
	{
		m_sFile = sFile;
	}

	// Functions
	CString GetFile(){ return m_sFile; }
	CString GetXmlOutput()
	{
		// Declare variables
		CString sXML = _T("");

		// Add data
		sXML.Format(_T("    <ACTION type=\"unregister\" file=\"%s\" />\r\n"), m_sFile);

		// Return value
		return sXML;
	}

private:
	// Functions

	// Variables
	CString m_sFile;
};


// End of redefinition protection
#endif // RESTOREACTIONS_H
