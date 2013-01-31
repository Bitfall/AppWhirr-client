/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	UpdateInfo.cpp
	file path:	Updater
	file base:	UpdateInfo
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "UpdateInfo.h"
#include "FileVersion.h"
#include "SelfUpdateInfo.h"
#include "Rijndael.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CUpdateInfo> CUpdateInfo::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CUpdateInfo::CUpdateInfo()
{
	// Set some default values
	m_iSectionCount = 0;
	m_iFileCount = 0;
	m_iIniCount = 0;
	m_iRegistryCount = 0;
	m_iFeatureCount = 0;
	m_iPopupMenuCount = 0;
	m_iShortcutCount = 0;
	m_iCustomConstantsCount = 0;
	m_iHtmlMessageTime = 0;
	m_iProtectionHashCount = 0;
	m_iCloseApplication = CLOSE_FALSE;
	m_bCheckSeparately = false;
	m_bRollbackEnabled = false;
	m_bShowErrors = true;
	m_bSelfUpdate = true;
	m_bForceUpdate = false;
	m_sHtmlMessageURL = _T("");
	m_sProtectionRegistrationDll = _T("");
	m_sProtectionWebsite = _T("");
	m_bShortcutsCreateShortcuts = true;

	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pPath = CPath::Instance();
	m_pLog = CLog::Instance();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CUpdateInfo * CUpdateInfo::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = auto_ptr<CUpdateInfo>(new CUpdateInfo);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CUpdateInfo);
	return sm_inst.get();
	*/
}

//=====================================================================

CString CUpdateInfo::GetPreviousUpdate(CString sUpdateFile)
{
	// Declare variables
	CString sPreviousUpdate = _T(""), sVersion = _T("");
	CMarkup xml;
	CSettings * pSettings = CSettings::Instance();

	// Load the XML file
	LoadXML(sUpdateFile, xml);

	// Get into the updateinfo
	if (!xml.FindElem(_T("UPDATEINFO")))
		return sPreviousUpdate;
	xml.IntoElem();

	// Check if we have a sequence part
	if (xml.FindElem(_T("SEQUENCE")))
	{
		// Jump into element
		xml.IntoElem();
		
		// Read version
		if (!xml.FindElem(_T("MINIMUMVERSION")))
			return sPreviousUpdate;
		sVersion = xml.GetElemContent();

		// Reset pos
		xml.ResetMainPos();

		// Read location
		if (!xml.FindElem(_T("LOCATION")))
			return sPreviousUpdate;
		sPreviousUpdate = xml.GetElemContent();

		// Check if the version is right or not
		if (m_pFunctions->CompareVersions(pSettings->GetAppVersion(), sVersion) != COMPAREVERSION_FIRSTSMALLER)
		{
			// We should note update
			return _T("");
		}
	}

	// Return the value
	return sPreviousUpdate;
}

//=====================================================================

bool CUpdateInfo::ReadFile(CString sUpdateFile)
{
	// Declare variables
	CMarkup xml;
	CString sXmlFileName = CPath::Instance()->ExtractFileName(sUpdateFile);

	// Log
	m_pLog->Log(_T("Start parsing ") + sXmlFileName);

	// Load the XML file
	LoadXML(sUpdateFile, xml);

	// Parse XML file
	if (!ParseXML(xml))
	{
		// Log
		m_pLog->Log(sXmlFileName + _T(" NOT succesfully parsed"));
		return false;
	}
	
	// Log
	m_pLog->Log(sXmlFileName + _T(" succesfully parsed"));
	return true;
}

//=====================================================================

void CUpdateInfo::ReadSelfUpdateFile(CString sUpdateFile)
{
	// Declare variables;
	CFileVersion fileVersion;
	CString sVersion, sParameters, sOldLocation, sNewLocation;
	CFileData * pFileData;
	CActionBase * pAction;
	CActionDownload * pActionDownload;
	CEventActionRun * pEventRun;

	// Should we update Updater too?
	if (m_bSelfUpdate)
	{
		// Read file
		CSelfUpdateInfo * pSelfUpdateInfo = CSelfUpdateInfo::Instance();
		pSelfUpdateInfo->ReadFile(sUpdateFile);
		
		// Get current Updater versio
		fileVersion.Open(m_pPath->GetPathUpdaterExecutable());
		sVersion = fileVersion.GetFileVersion();
		fileVersion.Close();
		
		// Is there a new version?
		if (pSelfUpdateInfo->GetVersion() != sVersion)
		{
			// Define paths
			sOldLocation.Format(_T("%s\\%s"), m_pPath->GetPathUpdaterTemp(), m_pPath->ExtractFileName(pSelfUpdateInfo->GetDownloadLocation()));
			sNewLocation.Format(_T("%s\\%s"), m_pPath->GetPathTemp(), _T("updater.exe"));

			// Add download action
			pFileData = new CFileData(_T("Updater"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""));
			pAction = new CActionDownload(sOldLocation);
			pActionDownload = (CActionDownload *)pAction;
			pActionDownload->AddDownloadLocation(pSelfUpdateInfo->GetDownloadLocation());
			pFileData->AddAction(pAction);
			
			// Add copy action
			pAction = new CActionCopy(sOldLocation, sNewLocation, true);
			pFileData->AddAction(pAction);

			// Add object to array
			m_arrFileData[m_iFileCount++] = pFileData;

			// Add OnClose event (which will always be the last)
			CEventInfo * pEventInfo = CEventInfo::Instance();
			sParameters.Format(_T("\"%s\" \"%s\""),
				m_pPath->GetPathTemp() + _T("Updater.exe"), 
				m_pPath->GetPathUpdaterExecutable());
			pEventRun = new CEventActionRun(m_pPath->GetPathTemp() + _T("UpdaterCopy.exe"),
				sParameters);
			pEventInfo->GetEventInfo(EVENT_ONCLOSE)->AddAction(pEventRun);
		}
	}
}

//=====================================================================

CString CUpdateInfo::GetNewVersion()
{
	// Return value
	return m_sNewVersion;
}

//=====================================================================

CString CUpdateInfo::GetLicense()
{
	// Return value
	return m_sLicense;
}

//=====================================================================

int CUpdateInfo::GetCloseApplication()
{
	// Return value
	return m_iCloseApplication;
}

//=====================================================================

bool CUpdateInfo::GetCheckSeparately()
{
	// Return value
	return m_bCheckSeparately;
}

//=====================================================================

bool CUpdateInfo::GetRollbackEnabled()
{
	// Return value
	return m_bRollbackEnabled;
}

//=====================================================================

bool CUpdateInfo::GetShowErrors()
{
	// Return value
	return m_bShowErrors;
}

//=====================================================================

bool CUpdateInfo::GetSelfUpdate()
{
	// Return value
	return m_bSelfUpdate;
}

//=====================================================================

bool CUpdateInfo::GetResumeDownloads()
{
	// Return value
	return m_bResumeDownloads;
}

//=====================================================================

bool CUpdateInfo::GetForceUpdate()
{
	// Return value
	return m_bForceUpdate;
}

//=====================================================================

bool CUpdateInfo::GetShortcutsAskUser()
{
	// Return value
	return m_bShortcutsAskUser;
}

//=====================================================================

bool CUpdateInfo::GetShortcutsCreateShortcuts()
{
	// Return value
	return m_bShortcutsCreateShortcuts;
}

//=====================================================================

void CUpdateInfo::SetShortcutsCreateShortcuts(bool bCreate /* = true */)
{
	// Set value
	m_bShortcutsCreateShortcuts = bCreate;
}

//=====================================================================

CString CUpdateInfo::GetHtmlMessageURL()
{
	// Return value
	return m_sHtmlMessageURL;
}

//=====================================================================

int	CUpdateInfo::GetHtmlMessageTime()
{
	// Return value
	return m_iHtmlMessageTime;
}

//=====================================================================

CSectionData * CUpdateInfo::GetSectionData(int index)
{
	// Return data
	return m_arrSectionData[index];
}

//=====================================================================
		
int CUpdateInfo::GetSectionCount()
{
	// Return count
	return m_iSectionCount;
}


//=====================================================================

CFileData * CUpdateInfo::GetFileData(int index)
{
	// Return data
	return m_arrFileData[index];
}

//=====================================================================
		
int CUpdateInfo::GetFileCount()
{
	// Return count
	return m_iFileCount;
}

//=====================================================================

CIniData * CUpdateInfo::GetIniData(int index)
{
	// Return data
	return m_arrIniData[index];
}

//=====================================================================

int CUpdateInfo::GetIniCount()
{
	// Return count
	return m_iIniCount;
}

//=====================================================================

CRegistryData * CUpdateInfo::GetRegistryData(int index)
{
	// Return data
	return m_arrRegistryData[index];
}

//=====================================================================

int CUpdateInfo::GetRegistryCount()
{
	// Return count
	return m_iRegistryCount;
}

//=====================================================================

FeatureData CUpdateInfo::GetFeature(int index)
{
	// Return data
	return m_arrFeatures[index];
}

//=====================================================================

int CUpdateInfo::GetFeatureCount()
{
	// Return count
	return m_iFeatureCount;
}

//=====================================================================

CPopupMenuData * CUpdateInfo::GetPopupMenuData(int index)
{
	// Return data
	return m_arrPopupMenuData[index];
}

//=====================================================================

int CUpdateInfo::GetPopupMenuCount()
{
	// Return count
	return m_iPopupMenuCount;
}

//=====================================================================

CShortcutData * CUpdateInfo::GetShortcutData(int index)
{
	// Return data
	return m_arrShortcutData[index];
}

//=====================================================================

int CUpdateInfo::GetShortcutCount()
{
	// Return count
	return m_iShortcutCount;
}

//=====================================================================

ProtectionHashData CUpdateInfo::GetProtectionHashData(int index)
{
	// Return data
	return m_arrProtectionHashData[index];
}

//=====================================================================

int	CUpdateInfo::GetProtectionHashCount()
{
	// Return count
	return m_iProtectionHashCount;
}

//=====================================================================

CString	CUpdateInfo::GetProtectionRegistrationDll()
{
	// Return data
	return m_sProtectionRegistrationDll;
}

//=====================================================================

CString	CUpdateInfo::GetProtectionWebsite()
{
	// Return data
	return m_sProtectionWebsite;
}

//=====================================================================

void CUpdateInfo::CleanUp()
{
	// Declare variables
	int i, j;

	// Delete all sections
	for (i = 0; i < m_iSectionCount; i++)
	{
		// Delete section object itself
		delete m_arrSectionData[i];
	}

	// Delete all file actions
	for (i = 0; i < m_iFileCount; i++)
	{
		// Delete all actions
		for (j = 0; j < m_arrFileData[i]->GetActionCount(); j++)
		{
			// Delete object
			delete m_arrFileData[i]->GetAction(j);
		}

		// Delete file object itself
		delete m_arrFileData[i];
	}

	// Delete all ini actions
	for (i = 0; i < m_iIniCount; i++)
	{
		// Delete ini object itself
		delete m_arrIniData[i];
	}

	// Delete all registry actions
	for (i = 0; i < m_iRegistryCount; i++)
	{
		// Delete ini object itself
		delete m_arrRegistryData[i];
	}

	// Delete all popup menu items
	for (i = 0; i < m_iPopupMenuCount; i++)
	{
		// Delete popup menu item itself
		delete m_arrPopupMenuData[i];
	}

	// Delete all shortcut items
	for (i = 0; i < m_iShortcutCount; i++)
	{
		// Delete shortcut item itself
		delete m_arrShortcutData[i];
	}
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CUpdateInfo::LoadXML(CString sFilename, CMarkup & xml)
{
	// Declare variables
	CRijndael oRijndael;
	CString sXML;
	CFile file;
	char * pBuffer = NULL, * pResult = NULL;
	int iSize;
	
	// Open XML file
	if (!file.Open(sFilename, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary))
	{
		// Set empty doc
		xml.SetDoc(_T(""));

		// Exit function
		return;
	}

	// Get file length
	iSize = (int)file.GetLength();

	// Allocate buffer for binary file data
	pBuffer = new char[iSize];

	// Read file
	iSize = file.Read(pBuffer, iSize);

	// Close file
	file.Close();	

	// Check if encryption is used
	char * pPrefix = strstr(pBuffer, CRYPT_PREFIX);
	if (pPrefix == pBuffer)
	{
		// Size is smaller now
		iSize -= strlen(CRYPT_PREFIX);

		// Allocate 2nd buffer
		pResult = new char[iSize];

		// Copy buffer without prefix
		memcpy(pResult, pBuffer + strlen(CRYPT_PREFIX), iSize);

		// Delete the buffer
		delete pBuffer;

		// Allocate buffer with smaller size
		pBuffer = new char[iSize];

		// Copy buffer
		memcpy(pBuffer, pResult, iSize);

		// Clear result buffer
		ZeroMemory(pResult, iSize);

		// Initialize key
		oRijndael.MakeKey(CRYPT_KEY, CRijndael::sm_chain0);

		// Decrypt data
		oRijndael.Decrypt(pBuffer, pResult, iSize, CRijndael::ECB);

		// Convert to normal CString
		sXML = ConvertHexToString(pResult);
	}
	else
	{
		// Load file directly into CString
		sXML = XML_to_CString(sFilename);
	}

	// Delete buffers
	delete pBuffer;
	delete pResult;

	// Open XML file
	xml.SetDoc(sXML);
}

//=====================================================================

bool CUpdateInfo::ParseXML(CMarkup & xml)
{
	// Declare variables
	CString sTemp, sXML, sType, sLowerTemp;

	// Find first part of xml file
	if (!xml.FindElem(_T("UPDATEINFO")))
	{
		// Log
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: updateinfo node not found"));
		return false;
	}
	xml.IntoElem();

	// Parse constants part
	if (xml.FindElem(_T("CONSTANTS")))
	{
		if (!ParseXMLConstants(xml))
			return false;
	}

	// Reset pos
	xml.ResetMainPos();

	// Parse protection part
	if (xml.FindElem(_T("PROTECTION")))
	{
		if (!ParseXMLProtection(xml))
			return false;
	}

	// Reset pos
	xml.ResetMainPos();

	// Parse htmlmessage part
	if (xml.FindElem(_T("HTMLMESSAGE")))
	{
		if (!ParseXMLHtmlMessage(xml))
			return false;
	}

	// Reset pos
	xml.ResetMainPos();

	// Parse general part
	if (!xml.FindElem(_T("GENERAL")))
	{
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: general information node not found"));
		return false;
	}
	else
	{
		if (!ParseXMLGeneralInformation(xml))
			return false;
	}

	// Reset pos
	xml.ResetMainPos();

	// Parse features part
	if (xml.FindElem(_T("FEATURES")))
	{
		if (!ParseXMLFeatures(xml))
			return false;
	}

	// Reset pos
	xml.ResetMainPos();

	// Parse Files part
	if (xml.FindElem(_T("FILES")))
	{
		if (!ParseXMLFiles(xml))
			return false;
	}

	// Reset pos
	xml.ResetMainPos();

	// Parse Ini part
	if (xml.FindElem(_T("INI")))
	{
		if (!ParseXMLIni(xml))
			return false;
	}

	// Reset pos
	xml.ResetMainPos();

	// Parse Registry part
	if (xml.FindElem(_T("REGISTRY")))
	{
		if (!ParseXMLRegistry(xml))
			return false;
	}

	// Reset pos
	xml.ResetMainPos();

	// Parse events
	if (xml.FindElem(_T("EVENTS")))
	{
		CEventInfo * pEventInfo = CEventInfo::Instance();
		pEventInfo->ParseEvents(xml);
	}

	// Reset pos
	xml.ResetMainPos();
	
	// Parse popup menu
	if (xml.FindElem(_T("POPUPMENU")))
	{
		if (!ParseXMLPopupMenu(xml))
			return false;
	}

	// Reset pos
	xml.ResetMainPos();

	// Parse shortcuts
	if (xml.FindElem(_T("SHORTCUTS")))
	{
		// Get askuser
		sTemp = xml.GetAttrib(_T("askuser"));
		sTemp = StringLower(sTemp);
		m_bShortcutsAskUser = (sTemp == _T("false")) ? false : true;

		// Parse all shortcuts
		if (!ParseXMLShortcuts(xml))
			return false;
	}

	// Reset pos
	xml.ResetMainPos();

	// Parse features part as last, because it is parsing recursive, the
	// xml class seems not to work well anymore...
	if (xml.FindElem(_T("SECTIONS")))
	{
		if (!ParseXMLSections(xml))
			return false;
	}

	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLConstants(CMarkup & xml)
{
	// Declare variables
	CString sLowerTemp, sName, sValue;
	Constant tmpConstant;

	// Jump into element
	xml.IntoElem();

	// Read constants
	while (xml.FindElem(_T("CONSTANT")))
	{
		// Get attributes
		sName = xml.GetAttrib(_T("name"));
		sValue = ReplaceConstants(xml.GetAttrib(_T("value")));

		// Check if we have a valid name
		if (!sName.IsEmpty())
		{
			// Check if a % is added before constant
			if (sName.Find(_T("%"), 0) != 0)
			{
				// Add % before variable
				sName.Insert(0, _T("%"));
			}

			// Check if a % is added after constant
			if (sName.Find(_T("%"), 2) != sName.GetLength() - 1)
			{
				// Add % before variable
				sName.Insert(sName.GetLength(), _T("%"));
			}
		}

		// Check if we have a valid value
		if (!sValue.IsEmpty() && !sName.IsEmpty())
		{
			// Add constant to array of constants
			tmpConstant.sName = sName;
			tmpConstant.sValue = sValue;

			m_arrCustomConstants[m_iCustomConstantsCount++] = tmpConstant;
		}
	}

	// Jump out of element
	xml.OutOfElem();

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLHtmlMessage(CMarkup & xml)
{
	USES_CONVERSION;

	// Declare variables
	CString sTemp = _T("0");

	// Jump into element
	xml.IntoElem();

	// Find url part
	if (xml.FindElem(_T("URL")))
		m_sHtmlMessageURL = ReplaceConstants(xml.GetElemContent());

	// Reset pos
	xml.ResetMainPos();

	// Find time part
	if (xml.FindElem(_T("TIME")))
		sTemp = ReplaceConstants(xml.GetElemContent());

	// Convert time to integer
	m_iHtmlMessageTime = atoi(W2A(sTemp));

	// Reset pos
	xml.ResetMainPos();	

	// Jump out of element
	xml.OutOfElem();

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLGeneralInformation(CMarkup & xml)
{
	// Declare variables
	CString sTemp;
	CString sLowerTemp;

	// Jump into element
	xml.IntoElem();

	// Find version part
	if (!xml.FindElem(_T("VERSION")))
	{
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: general information -> version not found"));
		return false;
	}
	m_sNewVersion = ReplaceConstants(xml.GetElemContent());

	// Reset pos
	xml.ResetMainPos();

	// Find license part
	if (xml.FindElem(_T("LICENSE")))
		m_sLicense = ReplaceConstants(xml.GetElemContent());
	else
		m_sLicense = _T("");

	// Reset pos
	xml.ResetMainPos();

	// Find close application part
	if (!xml.FindElem(_T("CLOSEAPPLICATION")))
		sTemp = _T("false");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);
	
	// Set default value
	m_iCloseApplication = CLOSE_FALSE;

	// Check what kind of closure should be used
	if (sTemp == _T("true"))
		m_iCloseApplication = CLOSE_TRUE;

	if (sTemp == _T("restart"))
		m_iCloseApplication = CLOSE_RESTART;

	if (sTemp == _T("restartsystem"))
		m_iCloseApplication = CLOSE_RESTARTSYSTEM;

	if (sTemp == _T("restartsystemapp"))
		m_iCloseApplication = CLOSE_RESTARTSYSTEMAPP;

	if (sTemp == _T("user"))
		m_iCloseApplication = CLOSE_USER;

	// Reset pos
	xml.ResetMainPos();

	// Find check separately part
	if (!xml.FindElem(_T("CHECKSEPARATELY")))
		sTemp = _T("false");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);
	
	m_bCheckSeparately = (sTemp == _T("true")) ? true : false;

	// Reset pos
	xml.ResetMainPos();

	// Find rollback part
	if (!xml.FindElem(_T("ENABLEROLLBACK")))
		sTemp = _T("false");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);	
	
	m_bRollbackEnabled = (sTemp == _T("true")) ? true : false;

	// Reset pos
	xml.ResetMainPos();

	// Find show errors part
	if (!xml.FindElem(_T("SHOWERRORS")))
		sTemp = _T("true");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);	

	m_bShowErrors = (sTemp == _T("false")) ? false : true;

	// Reset pos
	xml.ResetMainPos();

	// Find self update part
	if (!xml.FindElem(_T("SELFUPDATE")))
		sTemp = _T("true");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);	
	
	m_bSelfUpdate = (sTemp == _T("false")) ? false : true;

	// Reset pos
	xml.ResetMainPos();

	// Find resume downloads part
	if (!xml.FindElem(_T("RESUMEDOWNLOADS")))
		sTemp = _T("true");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);	
	
	m_bResumeDownloads = (sTemp == _T("false")) ? false : true;

	// Reset pos
	xml.ResetMainPos();

	// Find force update part
	if (!xml.FindElem(_T("FORCEUPDATE")))
		sTemp = _T("false");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);		
	
	m_bForceUpdate = (sTemp == _T("true")) ? true : false;

	// Jump out of element
	xml.OutOfElem();

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLProtection(CMarkup & xml)
{
	// Declare variables
	CString sTemp = _T("");
	ProtectionHashData hashData;

	// Get website
	m_sProtectionWebsite = ReplaceConstants(xml.GetAttrib(_T("website")));

	// Jump into element
	xml.IntoElem();

	// Find registration part
	if (xml.FindElem(_T("REGISTRATION")))
	{
		// Jump into element
		xml.IntoElem();

		// Get location
		if (!xml.FindElem(_T("LOCATION")))
		{
			m_pLog->Log(_T("UPDATEFILE PARSE ERROR: protection location not found"));
			return false;
		}
		else
			m_sProtectionRegistrationDll = ReplaceConstants(xml.GetElemContent());

		// Jump out of element
		xml.OutOfElem();
	}

	// Reset pos
	xml.ResetMainPos();

	// Find hashing part
	if (xml.FindElem(_T("HASHING")))
	{
		// Jump into element
		xml.IntoElem();

		// Find all hash operation
		while (xml.FindElem(_T("HASH")))
		{
			// Get filename
			hashData.sFilename = ReplaceConstants(xml.GetAttrib(_T("file")));

			// Get hash
			hashData.sHash = ReplaceConstants(xml.GetAttrib(_T("hash")));

			// Add data to array
			m_arrProtectionHashData[m_iProtectionHashCount++] = hashData;
		}

		// Jump out of element
		xml.OutOfElem();
	}

	// Jump out of element
	xml.OutOfElem();

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLSections(CMarkup & xml, CString sParent /* = "" */)
{
	// Declare variables
	CString sName = _T(""), sTitle = _T(""), sVersion = _T(""), sDescription = _T(""), sURL = _T(""), sTemp, sLowerTemp;
	bool bCheck = true, bEnable = true;
	CSectionData * pSectionData;

	// Jump into element
	xml.IntoElem();

	// While we can find section elements
	while (xml.FindElem(_T("SECTION")))
	{
		// Clear values
		sName = _T("");
		sTitle = _T("");
		sVersion = _T("");
		sDescription = _T("");
		sURL = _T("");
		bCheck = true;
		bEnable = true;

		// Read name, location, version and date
		sName = ReplaceConstants(xml.GetAttrib(_T("name")));
		sTitle = ReplaceConstants(xml.GetAttrib(_T("title")));
		sVersion = ReplaceConstants(xml.GetAttrib(_T("version")));
		sDescription = ReplaceConstants(xml.GetAttrib(_T("description")));
		sURL = ReplaceConstants(xml.GetAttrib(_T("url")));

		sTemp = ReplaceConstants(xml.GetAttrib(_T("check")));
		sTemp = StringLower(sTemp);	
		bCheck = (sTemp == _T("false")) ? false : true;

		sTemp = ReplaceConstants(xml.GetAttrib(_T("enabled")));
		sTemp = StringLower(sTemp);		
		bEnable = (sTemp == _T("false")) ? false : true;

		// Create section object
		pSectionData = new CSectionData(sName, sTitle, sVersion, sDescription, sParent, sURL, bCheck, bEnable);

		// Add object to array
		m_arrSectionData[m_iSectionCount++] = pSectionData;

		// Search for child elements
		if (!ParseXMLSections(xml, sName))
			return false;
	}

	// Jump out of element
	xml.OutOfElem();

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLFeatures(CMarkup & xml)
{
	// Declare variables
	FeatureData feature;

	// Jump into element
	xml.IntoElem();

	// While we can find file elements
	while (xml.FindElem(_T("FEATURE")) && (m_iFeatureCount < MAX_FEATURES))
	{
		// Jump into element
		xml.IntoElem();

		// Reset feature data
		feature.sText = _T("");
		feature.sHyperlink = _T("");

		// Search for text
		if (xml.FindElem(_T("TEXT")))
		{
			// Get data
			feature.sText = ReplaceConstants(xml.GetElemContent());

			// Reset cursor
			xml.ResetMainPos();

			// Check if we have a hyperlink part
			if (xml.FindElem(_T("LINK")))
			{
				// Get data
				feature.sHyperlink = ReplaceConstants(xml.GetElemContent());
			}

			// Add feature to list
			m_arrFeatures[m_iFeatureCount++] = feature;
		}

		// Jump out of element
		xml.OutOfElem();
	}

	// Jump out of element
	xml.OutOfElem();

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLFiles(CMarkup & xml)
{
	// Declare variables
	CString sName = _T(""), sLocation = _T(""), sVersion = _T("");
	CString sType = _T(""), sDate = _T(""), sHash = _T(""), sCheckType = _T(""), sSection = _T("");
	CFileData * pFileData;

	// Jump into element
	xml.IntoElem();

	// While we can find file elements
	while (xml.FindElem(_T("FILE")))
	{
		// Read name, location, version and date
		sName = ReplaceConstants(xml.GetAttrib(_T("name")));
		sLocation = ReplaceConstants(xml.GetAttrib(_T("location")));
		sVersion = ReplaceConstants(xml.GetAttrib(_T("version")));
		sDate = ReplaceConstants(xml.GetAttrib(_T("date")));
		sHash = ReplaceConstants(xml.GetAttrib(_T("hash")));
		sCheckType = ReplaceConstants(xml.GetAttrib(_T("check")));
		sSection = ReplaceConstants(xml.GetAttrib(_T("section")));

		// Create file object
		pFileData = new CFileData(sName, sLocation, sVersion, sDate, sHash, sCheckType, sSection);

		// Jump into element
		xml.IntoElem();

		// While we can find action elements
		while (xml.FindElem(_T("ACTION")))
		{
			// Get type
			sType = ReplaceConstants(xml.GetAttrib(_T("type")));

			sType = StringLower(sType);			

			// Check what action to parse
			if (sType == ACTION_DOWNLOAD)
			{
				if (!ParseXMLFileDownload(xml, pFileData))
					return false;
			}

			if (sType == ACTION_COPY)
			{
				if (!ParseXMLFileCopy(xml, pFileData))
					return false;
			}

			if (sType == ACTION_DELETE)
			{
				if (!ParseXMLFileDelete(xml, pFileData))
					return false;
			}

			if (sType == ACTION_RUN)
			{
				if (!ParseXMLFileRun(xml, pFileData))
					return false;
			}

			if (sType == ACTION_UNZIP)
			{
				if (!ParseXMLFileUnzip(xml, pFileData))
					return false;
			}

			if (sType == ACTION_SETFILEATTR)
			{
				if (!ParseXMLFileSetFileAttr(xml, pFileData))
					return false;
			}

			if (sType == ACTION_REGISTER)
			{
				if (!ParseXMLFileRegister(xml, pFileData))
					return false;
			}
		}

		// Jump out of element
		xml.OutOfElem();

		// Add object to array
		m_arrFileData[m_iFileCount++] = pFileData;
	}

	// Jump out of element
	xml.OutOfElem();

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLIni(CMarkup & xml)
{
	// Declare variables
	CString sLocation = _T(""), sGroup = _T(""), sKey = _T(""), sValue = _T("");
	CIniData * pIniData;

	// Jump into element
	xml.IntoElem();

	// While we can find file elements
	while (xml.FindElem(_T("ITEM")))
	{
		// Jump into element
		xml.IntoElem();

		// Get data
		if (!xml.FindElem(_T("LOCATION")))
		{
			m_pLog->Log(_T("UPDATEFILE PARSE ERROR: ini location not found"));
			return false;
		}
		sLocation = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		if (!xml.FindElem(_T("GROUP")))
		{
			m_pLog->Log(_T("UPDATEFILE PARSE ERROR: ini group not found"));
			return false;
		}
		sGroup = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		if (!xml.FindElem(_T("KEY")))
		{
			m_pLog->Log(_T("UPDATEFILE PARSE ERROR: ini key not found"));
			return false;
		}
		sKey = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		if (!xml.FindElem(_T("VALUE")))
		{
			m_pLog->Log(_T("UPDATEFILE PARSE ERROR: ini value not found"));
			return false;
		}
		sValue = ReplaceConstants(xml.GetElemContent());

		// Create object
		pIniData = new CIniData(sLocation, sGroup, sKey, sValue);

		// Add object to array
		m_arrIniData[m_iIniCount++] = pIniData;

		// Jump out of element
		xml.OutOfElem();
	}

	// Jump out of element
	xml.OutOfElem();

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLRegistry(CMarkup & xml)
{
	// Declare variables
	CString sKey = _T(""), sValue = _T(""), sType = _T(""), sTemp, sLowerTemp;
	CRegistryData * pRegistryData;

	// Jump into element
	xml.IntoElem();

	// While we can find file elements
	while (xml.FindElem(_T("ITEM")))
	{
		// Get type
		sTemp = xml.GetAttrib(_T("type"));
		sType = _T("");

		sTemp = StringLower(sTemp);

		// Check what type we are dealing with
		if (sTemp == REGTYPE_STRING)
			sType = sTemp;

		if (sTemp == REGTYPE_BINARY)
			sType = sTemp;

		if (sTemp == REGTYPE_DWORD)
			sType = sTemp;

		if (sType.IsEmpty())
			sType = REGTYPE_STRING;

		// Jump into element
		xml.IntoElem();

		// Get data
		if (!xml.FindElem(_T("KEY")))
		{
			m_pLog->Log(_T("UPDATEFILE PARSE ERROR: registry key not found"));
			return false;
		}
		sKey = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		if (!xml.FindElem(_T("VALUE")))
		{
			m_pLog->Log(_T("UPDATEFILE PARSE ERROR: registry value not found"));
			return false;
		}
		sValue = ReplaceConstants(xml.GetElemContent());

		// Create object
		pRegistryData = new CRegistryData(sType, sKey, sValue);

		// Add object to array
		m_arrRegistryData[m_iRegistryCount++] = pRegistryData;

		// Jump out of element
		xml.OutOfElem();
	}

	// Jump out of element
	xml.OutOfElem();

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLPopupMenu(CMarkup & xml)
{
	// Declare variables
	CString sText = _T(""), sLocation = _T(""), sParameters = _T(""), sBitmap = _T("");
	CLanguage * pLanguage = CLanguage::Instance();
	CFileData * pFile;
	CActionBase * pAction;
	CActionDownload * pTemp;

	// Jump into element
	xml.IntoElem();

	// Read download location(s)
	while (xml.FindElem(_T("ITEM")))
	{
		// Jump into element
		xml.IntoElem();

		// Find text element
		if (!xml.FindElem(_T("TEXT")))
		{
			m_pLog->Log(_T("UPDATEFILE PARSE ERROR: popup text not found"));
			return false;
		}

		// Get value
		sText = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		// Find location element
		if (!xml.FindElem(_T("LOCATION")))
			// Set default value
			sLocation = _T("");
		else
			// Get value
			sLocation = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		// Find parameters element
		if (!xml.FindElem(_T("PARAMETERS")))
			// Set default value
			sParameters = _T("");
		else
			// Get value
			sParameters = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		// Find bitmap element
		if (!xml.FindElem(_T("BITMAP")))
			// Set default value
			sBitmap = _T("");
		else
			// Get value
			sBitmap = m_pFunctions->ReplaceConnectionSettings(ReplaceConstants(xml.GetElemContent()));

		// Create popup item
		CPopupMenuData * pTemp = new CPopupMenuData(sText, sLocation, sParameters, sBitmap);

		// Add object to array
		m_arrPopupMenuData[m_iPopupMenuCount++] = pTemp;

		// Jump out of element
		xml.OutOfElem();
	}	

	// Jump out of element
	xml.OutOfElem();
	
	// Create one file and several download actions for all the bitmaps
	pFile = new CFileData(pLanguage->GetString(IDS_POPUP_BITMAPS), _T(""), _T(""), _T(""), _T(""), CHECK_HASH, _T(""));

	for (int i = 0; i < m_iPopupMenuCount; i++)
	{
		// Should we add this object?
		if (!m_arrPopupMenuData[i]->GetBitmap().IsEmpty())
		{
			// Create action
			pAction = new CActionDownload(m_pPath->GetPathUpdaterTemp() + m_pPath->ExtractFileName(m_arrPopupMenuData[i]->GetBitmap()));

			// Get action as download object
			pTemp = (CActionDownload *)pAction;

			// Add download action
			pTemp->AddDownloadLocation(m_arrPopupMenuData[i]->GetBitmap());

			// Add file to action
			pFile->AddAction(pAction);
		}
	}

	// Add file to file array
	m_arrFileData[m_iFileCount++] = pFile;

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLShortcuts(CMarkup & xml)
{
	USES_CONVERSION;

	// Declare variables
	CString sTemp, sLowerTemp;
	CString sTargetFile, sWorkingDirectory, sParameters, sDescription, sIconFile, sLinkFile;
	int iIconIndex, iShowMode;

	// Jump into element
	xml.IntoElem();

	// Read download location(s)
	while (xml.FindElem(_T("SHORTCUT")))
	{
		// Jump into element
		xml.IntoElem();

		// Reset all variables
		sTargetFile = _T("");
		sWorkingDirectory = _T("");
		sParameters = _T("");
		sDescription = _T("");
		sIconFile = _T("");
		sLinkFile = _T("");
		iIconIndex = -1;
		iShowMode = -1;

		// Find targetfile element
		if (!xml.FindElem(_T("TARGETFILE")))
		{
			m_pLog->Log(_T("UPDATEFILE PARSE ERROR: shortcut targetfile not found"));
			return false;
		}

		// Get value
		sTargetFile = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		// Find location element
		if (!xml.FindElem(_T("WORKINGDIRECTORY")))
			// Set default value
			sWorkingDirectory = _T("");
		else
			// Get value
			sWorkingDirectory = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		// Find parameters element
		if (!xml.FindElem(_T("PARAMETERS")))
			// Set default value
			sParameters = _T("");
		else
			// Get value
			sParameters = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		// Find parameters element
		if (!xml.FindElem(_T("DESCRIPTION")))
			// Set default value
			sDescription = _T("");
		else
			// Get value
			sDescription = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		// Find iconfile element
		if (!xml.FindElem(_T("ICONFILE")))
			// Use target file
			sIconFile = sTargetFile;
		else
			// Get value
			sIconFile = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		// Find linkfile element
		if (!xml.FindElem(_T("LINKFILE")))
		{
			m_pLog->Log(_T("UPDATEFILE PARSE ERROR: shortcut linkfile not found"));
			return false;
		}

		// Get value
		sLinkFile = ReplaceConstants(xml.GetElemContent());

		// Reset pos
		xml.ResetMainPos();

		// Find iconindex element
		if (!xml.FindElem(_T("ICONINDEX")))
			iIconIndex = 0;
		else
			// Get value
			iIconIndex = atoi(W2A(ReplaceConstants(xml.GetElemContent())));

		// Reset pos
		xml.ResetMainPos();

		// Find showmode element
		if (!xml.FindElem(_T("SHOWMODE")))
			iShowMode = SW_SHOWNORMAL;
		else
		{
			// Get value
			sTemp = ReplaceConstants(xml.GetElemContent());

			// Make uppercase
			sTemp = StringUpper(sTemp);

			// Check what type should be used
			if (sTemp == _T("SW_SHOWNORMAL"))
				iShowMode = SW_SHOWNORMAL;

			if (sTemp == _T("SW_SHOWMAXIMIZED"))
				iShowMode = SW_SHOWMAXIMIZED;
		
			if (sTemp == _T("SW_SHOWMINNOACTIVE"))
				iShowMode = SW_SHOWMINNOACTIVE;

			// If nothing is selected, use SW_SHOWNORMAL
			if (iShowMode == -1)
				iShowMode = SW_SHOWNORMAL;
		}

		// Create shortcut item
		CShortcutData * pTemp = new CShortcutData(sTargetFile, sParameters, sWorkingDirectory,
			sLinkFile, sIconFile, sDescription, iShowMode, iIconIndex);

		// Add object to array
		m_arrShortcutData[m_iShortcutCount++] = pTemp;

		// Jump out of element
		xml.OutOfElem();
	}	

	// Jump out of element
	xml.OutOfElem();
	
	// If we get here, function is successful
	return true;	
}

//=====================================================================

bool CUpdateInfo::ParseXMLFileDownload(CMarkup & xml, CFileData * pFileData)
{
	// Declare variables
	CActionBase * pAction;
	CActionDownload * pTemp;
	CString arrDownloadLocation[MAX_DOWNLOADLOCATIONS], sDestination = _T("");
	int iCount = 0;

	// Jump into element
	xml.IntoElem();

	// Read download location(s)
	while (xml.FindElem(_T("LOCATION")))
	{
		// Get value and replace all instances of %username% and %password% in the update file url
		arrDownloadLocation[iCount++] = m_pFunctions->ReplaceConnectionSettings(ReplaceConstants(xml.GetElemContent()));
	}

	// Check if we have any locations
	if (iCount > 0)
	{
		// Reset pos
		xml.ResetMainPos();

		// Get download destination
		if (xml.FindElem(_T("DESTINATION")))
		{
			// Get value
			sDestination = ReplaceConstants(xml.GetElemContent());

			// If it is an empty path, use default name
			if (sDestination == _T(""))
			{
				sDestination.Format(_T("%s\\%s"), m_pPath->GetPathUpdaterTemp(),
					m_pPath->ExtractFileName(arrDownloadLocation[0]));
			}

			// Is this a relative path?
			if (m_pPath->ExtractDrive(sDestination) == _T(""))
			{
				// Add %updatertemp% to destination
				sDestination = _T("%updatertemp%\\") + sDestination;
				sDestination = ReplaceConstants(sDestination);
			}

			// Do we have a filename?
			if (m_pPath->ExtractFileExtension(sDestination) == _T(""))
			{
				// Add existing filename
				sDestination += _T("\\") + m_pPath->ExtractFileName(arrDownloadLocation[0]);
			}
		}
	}

	// If it is an empty path, use default name
	if (sDestination == _T(""))
	{
		sDestination.Format(_T("%s\\%s"), m_pPath->GetPathUpdaterTemp(),
			m_pPath->ExtractFileName(arrDownloadLocation[0]));
	}

	// Create action
	pAction = new CActionDownload(sDestination);

	// Get action as download object
	pTemp = (CActionDownload *)pAction;

	// Add all download locations
	for (int i = 0; i < iCount; i++)
	{
		pTemp->AddDownloadLocation(arrDownloadLocation[i]);
	}

	// Jump out of element
	xml.OutOfElem();

	// Add action
	pFileData->AddAction(pAction);

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLFileCopy(CMarkup & xml, CFileData * pFileData)
{
	// Declare variables
	CActionBase * pAction;
	CString sOldLocation = _T(""), sNewLocation = _T(""), sTemp = _T("");
	bool bOverwrite;

	// Jump into element
	xml.IntoElem();

	// Read old location
	if (!xml.FindElem(_T("OLDLOCATION")))
	{
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: file copy old location not found"));
		return false;
	}
	sOldLocation = ReplaceConstants(xml.GetElemContent());

	// Reset pos
	xml.ResetMainPos();

	// Read new location
	if (!xml.FindElem(_T("NEWLOCATION")))
	{
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: file copy new location not found"));
		return false;
	}
	sNewLocation = ReplaceConstants(xml.GetElemContent());

	// Reset pos
	xml.ResetMainPos();

	// Read overwrite
	if (!xml.FindElem(_T("OVERWRITE")))
		sTemp = _T("false");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);	

	bOverwrite = (sTemp == _T("true")) ? true : false;

	// Jump out of element
	xml.OutOfElem();

	// Create action
	pAction = new CActionCopy(sOldLocation, sNewLocation, bOverwrite);

	// Add action
	pFileData->AddAction(pAction);

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLFileDelete(CMarkup & xml, CFileData * pFileData)
{
	// Declare variables
	CActionBase * pAction;
	CString sLocation = _T(""), sTemp = _T("");
	bool bAskUser;

	// Jump into element
	xml.IntoElem();

	// Read location
	if (!xml.FindElem(_T("LOCATION")))
	{
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: file delete location not found"));
		return false;
	}
	sLocation = ReplaceConstants(xml.GetElemContent());

	// Reset pos
	xml.ResetMainPos();

	// Read overwrite
	if (!xml.FindElem(_T("ASKUSER")))
		sTemp = _T("false");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);
	
	bAskUser = (sTemp == _T("true")) ? true : false;

	// Jump out of element
	xml.OutOfElem();

	// Create action
	pAction = new CActionDelete(sLocation, bAskUser);

	// Add action
	pFileData->AddAction(pAction);

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLFileRun(CMarkup & xml, CFileData * pFileData)
{
	// Declare variables
	CActionBase * pAction;
	CString sLocation = _T(""), sParameters = _T(""), sTemp;
	bool bWaitUntilReady;

	// Jump into element
	xml.IntoElem();

	// Read location
	if (!xml.FindElem(_T("LOCATION")))
	{
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: file run (execute) location not found"));
		return false;
	}
	sLocation = ReplaceConstants(xml.GetElemContent());

	// Reset pos
	xml.ResetMainPos();

	// Read parameters
	if (!xml.FindElem(_T("PARAMETERS")))
		sParameters = _T("");
	else
		sParameters = ReplaceConstants(xml.GetElemContent());

	// Reset pos
	xml.ResetMainPos();

	// Read parameters
	if (!xml.FindElem(_T("WAIT")))
		sTemp = _T("false");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);		

	bWaitUntilReady = (sTemp == _T("false")) ? false : true;

	// Jump out of element
	xml.OutOfElem();

	// Create action
	pAction = new CActionRun(sLocation, sParameters, bWaitUntilReady);

	// Add action
	pFileData->AddAction(pAction);

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLFileUnzip(CMarkup & xml, CFileData * pFileData)
{
	// Declare variables
	CActionBase * pAction;
	CString sLocation = _T(""), sDestination = _T("");

	// Jump into element
	xml.IntoElem();

	// Read location
	if (!xml.FindElem(_T("LOCATION")))
	{
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: file unzip location not found"));
		return false;
	}
	sLocation = ReplaceConstants(xml.GetElemContent());

	// Reset pos
	xml.ResetMainPos();

	// Read destination
	if (!xml.FindElem(_T("DESTINATION")))
	{
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: file unzip destination not found"));
		return false;
	}
	sDestination = ReplaceConstants(xml.GetElemContent());

	// Jump out of element
	xml.OutOfElem();
	
	// Create action
	pAction = new CActionUnzip(sLocation, sDestination);
	
	// Add action
	pFileData->AddAction(pAction);
	
	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLFileSetFileAttr(CMarkup & xml, CFileData * pFileData)
{
	// Declare variables
	CActionBase * pAction;
	CString sLocation = _T(""), sAttributes = _T("");
	int iAttributes = 0;

	// Jump into element
	xml.IntoElem();

	// Read location
	if (!xml.FindElem(_T("LOCATION")))
	{
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: file set attributes location not found"));
		return false;
	}
	sLocation = ReplaceConstants(xml.GetElemContent());

	// Reset pos
	xml.ResetMainPos();

	// Read attributes
	if (!xml.FindElem(_T("ATTRIBUTES")))
		iAttributes = FILE_ATTRIBUTE_NORMAL;
	else
		sAttributes = ReplaceConstants(xml.GetElemContent());

	sAttributes = StringLower(sAttributes);

	// Parse the attributes
	if (sAttributes.Find(FILEATTR_SYSTEM) != -1)
		iAttributes |= FILE_ATTRIBUTE_SYSTEM;

	if (sAttributes.Find(FILEATTR_READONLY) != -1)
		iAttributes |= FILE_ATTRIBUTE_READONLY;

	if (sAttributes.Find(FILEATTR_HIDDEN) != -1)
		iAttributes |= FILE_ATTRIBUTE_HIDDEN;

	if (sAttributes.Find(FILEATTR_ARCHIVE) != -1)
		iAttributes |= FILE_ATTRIBUTE_ARCHIVE;

	if (sAttributes.Find(FILEATTR_COMPRESSED) != -1)
		iAttributes |= FILE_ATTRIBUTE_COMPRESSED;

	// Jump out of element
	xml.OutOfElem();
	
	// Create action
	pAction = new CActionSetFileAttr(sLocation, iAttributes);
	
	// Add action
	pFileData->AddAction(pAction);
	
	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CUpdateInfo::ParseXMLFileRegister(CMarkup & xml, CFileData * pFileData)
{
	// Declare variables
	CActionBase * pAction;
	CString sLocation = _T(""), sTemp = _T("");
	bool bAskUser;

	// Jump into element
	xml.IntoElem();

	// Read location
	if (!xml.FindElem(_T("LOCATION")))
	{
		m_pLog->Log(_T("UPDATEFILE PARSE ERROR: file register location not found"));
		return false;
	}
	sLocation = ReplaceConstants(xml.GetElemContent());

	// Read attributes
	if (!xml.FindElem(_T("ASKUSER")))
		sTemp = _T("true");
	else
		sTemp = ReplaceConstants(xml.GetElemContent());

	sTemp = StringLower(sTemp);

	bAskUser = (sTemp == _T("false")) ? false : true;

	// Jump out of element
	xml.OutOfElem();

	// Create action
	pAction = new CActionRegister(sLocation, bAskUser);

	// Add action
	pFileData->AddAction(pAction);

	// If we get here, function is successful
	return true;
}

//=====================================================================

CString	CUpdateInfo::ReplaceConstants(CString sText)
{
	// Declare variables
	CString sResult;

	// Replace custom constants
	sResult = ReplaceCustomConstants(sText);	

	// Replace Updater defined constants
	sResult = m_pPath->ReplacePathConstants(sResult);

	// Return result
	return sResult;
}

//=====================================================================

CString	CUpdateInfo::ReplaceCustomConstants(CString sText)
{
	// Loop all items
	for (int i = 0; i < m_iCustomConstantsCount; i++)
	{
		// Replace the constant
		sText.Replace(m_arrCustomConstants[i].sName, m_arrCustomConstants[i].sValue);
	}

	// Return result
	return sText;
}

//=====================================================================

CString CUpdateInfo::XML_to_CString(CString sFilename)
{
	// Declare variables
	CString sText, sNotes;
	CFile file;
	int iFileLength;
	unsigned char * pBuffer;

	// Open XML file
	if (!file.Open(sFilename, CFile::modeRead | CFile::shareDenyNone))
		return _T("");

	// Get file length
	iFileLength = (int)file.GetLength();

	// Allocate buffer for binary file data
	pBuffer = new unsigned char[iFileLength + 2];

	// Read file
	iFileLength = file.Read(pBuffer, iFileLength);

	// Close file
	file.Close();

	// Add \0 to string
	pBuffer[iFileLength] = _T('\0');
	pBuffer[iFileLength + 1] = _T('\0'); // in case 2-byte encoded

	// Windows Unicode file is detected if starts with FEFF
	if ((pBuffer[0] == 0xff) && (pBuffer[1] == 0xfe))
	{
		// Contains byte order mark, so assume wide char content
		// non _UNICODE builds should perform UCS-2 (wide char) to UTF-8 conversion here
		sText = (LPCWSTR)(&pBuffer[2]);
	}
	else
	{
		// _UNICODE builds should perform UTF-8 to UCS-2 (wide char) conversion here
		sText = (LPCTSTR)pBuffer;
	}

	// Release memory
	delete [] pBuffer;

	// Return string
	return sText;
}