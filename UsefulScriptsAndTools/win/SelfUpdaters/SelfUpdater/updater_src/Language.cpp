/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	Language.cpp
	file path:	Updater
	file base:	Language
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Language.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CLanguage> CLanguage::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CLanguage::CLanguage()
{
	// Set default language name & translator
	m_sLanguageName = _T("");
	m_sLanguageTranslator = _T("");

	// Init available languages
	InitLanguagesAvailable();

	// We didn't parse the file (yet)
	m_bParsed = false;
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CLanguage * CLanguage::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = auto_ptr<CLanguage>(new CLanguage);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CLanguage);
	return sm_inst.get();
	*/
}

//=====================================================================

bool CLanguage::LoadLanguage(LANGID language)
{
	// Declare variables
	CPath * pPath = CPath::Instance();
	CString sTemp;

	// Automatic load right language
	for (int i = 0; i < m_iLanguageCount; i++)
	{
		// Check all sublanguages
		for (int j = 0; j < m_arrLanguages[i].iLanguageCount; j++)
		{
			// Check if we found the right language
			if (m_arrLanguages[i].arrLanguages[j] == language)
			{
				// We found it!
				sTemp.Format(_T("%s\\lng\\%s.xml"), pPath->GetPathUpdater(), m_arrLanguages[i].sXML);
				LoadLanguage(sTemp, m_arrLanguages[i].sXML);
				return true;
			}
		}
	}

	// Nothing found, set default values
	m_sLanguageName = _T("English (default)");
	m_sLanguageTranslator = UPDATER_DEVELOPER;

	// If we get here, not local language is found
	return false;
}

//=====================================================================

bool CLanguage::LoadLanguage(CString sXmlLocation, CString sLanguageName,
							 bool bShowErrors /* = true */)
{
	// Declare variables
	CString sError, sTemp;

	// If we already loaded a language, release it first
	ReleaseLanguage();

	// Check if the xml we want to load exists
    if (!PathFileExists(sXmlLocation))
	{
		m_sLanguageName = _T("English (default)");
		return false;
	}

	// Did we parse the XML file yet?
	if (!m_bParsed)
	{
		// Now we parsed
		m_bParsed = true;

		// Load XML File
		m_oLanguageXml.SetDoc(XML_to_CString(sXmlLocation));
	}

	// Retrieve language information
	if (!ReadLanguageInfo())
	{
		// Error
		return false;
	}

	// Set language
	if (m_sLanguageName == _T(""))
	{
		m_sLanguageName = sLanguageName;
	}

	// Check version
	CFunctions * pFunctions = CFunctions::Instance();

	// In debug, the EXACT version must be found, in release, a future version is also allowed
#ifdef _DEBUG
	if (pFunctions->CompareVersions(m_sLanguageVersion, VERSION_LANGUAGE) != COMPAREVERSION_EQUAL)
#else
	if (pFunctions->CompareVersions(m_sLanguageVersion, VERSION_LANGUAGE) == COMPAREVERSION_FIRSTSMALLER)
#endif
	{
		// Set up error message
		sTemp = _T("You are using a wrong version (%s) of the language file!\n");
		sTemp += _T("You should use version %s! Updater will now use default language (English)");
		sError.Format(sTemp, m_sLanguageVersion, VERSION_LANGUAGE);

		if (bShowErrors)
		{
			// Show error message (in english)
			AfxMessageBox(sError, MB_ICONERROR | MB_OK);
			m_sLanguageName = _T("english");
		}
		return false;
	}

	// We succeeded
	return true;
}

//=====================================================================

void CLanguage::ReleaseLanguage()
{
	// Clear all language data
	m_sLanguageName = _T("");
	m_sLanguageTranslator = _T("");
	m_sLanguageVersion = _T("");

	// We have no language parsed
	m_bParsed = false;

	// Empty the xml doc
	m_oLanguageXml.SetDoc(_T(""));
}

//=====================================================================

CString CLanguage::GetString(UINT ResourceNumber)
{
	// Declare variables
	CString sResult, sBuffer;

	// First try to read from XML
	if (sResult.IsEmpty())
		sResult = ReadFromXml(ResourceNumber);

	// If nothing works, read from default string table
	if (sResult.IsEmpty())
		sResult.LoadString(ResourceNumber);
	
	return sResult;
}

//=====================================================================

CString CLanguage::GetLanguageName()
{
	// Return value
	return m_sLanguageName;
}

//=====================================================================

CString CLanguage::GetLanguageTranslator()
{
	// Return value
	return m_sLanguageTranslator;
}

//=====================================================================

CString CLanguage::GetLanguageVersion()
{
	// Return value
	return m_sLanguageVersion;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

bool CLanguage::ReadLanguageInfo()
{
	// Try to find main element
	if (!m_oLanguageXml.FindElem(_T("UPDATERLANGUAGE")))
	{
		// We were not able to load the language
		return false;
	}

	// Jump into element
	m_oLanguageXml.IntoElem();

	// Try to find info part
	if (!m_oLanguageXml.FindElem(_T("INFO")))
	{
		// We were not able to load the language
		return false;
	}

	// Jump into element
	m_oLanguageXml.IntoElem();

	// Try to find language name
	if (!m_oLanguageXml.FindElem(_T("LANGUAGE")))
	{
		// We were not able to load the language
		return false;
	}
	m_sLanguageName = m_oLanguageXml.GetData();

	// Reset pos
	m_oLanguageXml.ResetMainPos();	

	// Try to find language version
	if (!m_oLanguageXml.FindElem(_T("VERSION")))
	{
		// We were not able to load the language
		return false;
	}
	m_sLanguageVersion = m_oLanguageXml.GetData();

	// Try to find translator name
	if (!m_oLanguageXml.FindElem(_T("TRANSLATOR")))
	{
		// We were not able to load the language
		return false;
	}
	m_sLanguageTranslator = m_oLanguageXml.GetData();

	// Step out of this element
	m_oLanguageXml.OutOfElem();

	// Reset pos
	m_oLanguageXml.ResetMainPos();	

	// Find real data
	if (m_oLanguageXml.FindElem(_T("DATA")))
	{
		// Jump into element
		m_oLanguageXml.IntoElem();

		// We succeeded
		return true;
	}
	
	// If we get here, function is not successfull
	return false;
}

//=====================================================================

CString CLanguage::ReadFromXml(UINT ResourceNumber)
{
	// Declare variables
	CString sResult = _T(""), sLangNameLower, sIdUpper, sIdToFind;

	// Check out which id to find
	sIdToFind = GetStringConstant(ResourceNumber);

	// Check if we have a valid id
	if (sIdToFind == _T(""))
	{
		// Return empty string so default string table is used
		return _T("");
	}

	// Try to find right string id
	if (m_oLanguageXml.FindElem(sIdToFind))
	{
		// Get result
		sResult = m_oLanguageXml.GetData();
	}

	// Reset pointer
	m_oLanguageXml.ResetMainPos();

	// Replace special characters
	sResult.Replace(_T("\\n"), _T("\n"));
	
	// Return result
	return sResult;
}

//=====================================================================

CString CLanguage::XML_to_CString(CString sFilename)
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
		sText = (LPCSTR)pBuffer;
	}

	// Release memory
	delete [] pBuffer;

	// If it is too short, assume it got truncated due to non-text content
	if (sText.GetLength() < iFileLength / 2 - 20)
	{
		return _T("");
	}

	// Return string
	return sText;
}

//=====================================================================

void CLanguage::InitLanguagesAvailable()
{
	// Set language count to zero
	m_iLanguageCount = 0;

	// Add czech
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0405; // Czech
	m_arrLanguages[m_iLanguageCount++].sXML = _T("czech");
	
	// Add danish
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0406; // Danish
	m_arrLanguages[m_iLanguageCount++].sXML = _T("danish");

	// Add dutch
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0413; // Dutch (standard)
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0813; // Belgian (flemish)
	m_arrLanguages[m_iLanguageCount++].sXML = _T("dutch");

	// Add finnish
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x040b; // Finnish
	m_arrLanguages[m_iLanguageCount++].sXML = _T("finnish");

	// Add french
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x040c; // French (standard)
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x080c; // Belgian
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0c0c; // Canadian
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x100c; // Swiss
	m_arrLanguages[m_iLanguageCount++].sXML = _T("french");

	// Add german
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0407; // German (standard)
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0807; // Swiss
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0c07; // Austrian
	m_arrLanguages[m_iLanguageCount++].sXML = _T("german");

	// Add greek
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0408; // Greek
	m_arrLanguages[m_iLanguageCount++].sXML = _T("greek");

	// Add hungarian
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x040e; // Hungarian
	m_arrLanguages[m_iLanguageCount++].sXML = _T("hungarian");

	// Add icelandic
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x040f; // Icelandic
	m_arrLanguages[m_iLanguageCount++].sXML = _T("icelandic");

	// Add italian
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0410; // Italian (standard)
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0810; // Swiss
	m_arrLanguages[m_iLanguageCount++].sXML = _T("italian");

	// Add norwegian
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0414; // Norwegian (bokmal)
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0814; // Norwegian (nynorsk)
	m_arrLanguages[m_iLanguageCount++].sXML = _T("norwegian");

	// Add polish
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0415; // Polish
	m_arrLanguages[m_iLanguageCount++].sXML = _T("polish");

	// Add portuguese brazil
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0416; // Portuguese (Brazilian)
	m_arrLanguages[m_iLanguageCount++].sXML = _T("portuguese");

	// Add russian
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0419; // Russian
	m_arrLanguages[m_iLanguageCount++].sXML = _T("russian");

	// Add slovak
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x041b; // Slovak
	m_arrLanguages[m_iLanguageCount++].sXML = _T("slovak");

	// Add spanish
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x040a; // Spanish (Standard/Traditional)
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x080a; // Mexican
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x0c0a; // Spanish (Modern)
	m_arrLanguages[m_iLanguageCount++].sXML = _T("spanish");

	// Add swedish
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x041D; // Swedish
	m_arrLanguages[m_iLanguageCount++].sXML = _T("swedish");

	// Add turkish
	m_arrLanguages[m_iLanguageCount].iLanguageCount = 0;
	m_arrLanguages[m_iLanguageCount].arrLanguages[m_arrLanguages[m_iLanguageCount].iLanguageCount++] = 0x041f; // Turkish
	m_arrLanguages[m_iLanguageCount++].sXML = _T("turkish");
}

//=====================================================================

CString CLanguage::GetStringConstant(UINT nResourceNumber)
{
	// Check what string should be returned
	switch (nResourceNumber)
	{
	case IDS_GENERAL_OK:
		return _T("IDS_GENERAL_OK");

	case IDS_GENERAL_CANCEL:
		return _T("IDS_GENERAL_CANCEL");

	case IDS_GENERAL_YES:
		return _T("IDS_GENERAL_YES");

	case IDS_GENERAL_NO:
		return _T("IDS_GENERAL_NO");

	case IDS_GENERAL_NEXT:
		return _T("IDS_GENERAL_NEXT");

	case IDS_GENERAL_FINISH:
		return _T("IDS_GENERAL_FINISH");

	case IDS_GENERAL_TITLE:
		return _T("IDS_GENERAL_TITLE");

	case IDS_QUESTION_CLOSE:
		return _T("IDS_QUESTION_CLOSE");

	case IDS_QUESTION_CLOSEAPPLICATION:
		return _T("IDS_QUESTION_CLOSEAPPLICATION");

	case IDS_QUESTION_DELETEFILE:
		return _T("IDS_QUESTION_DELETEFILE");

	case IDS_QUESTIONTITLE_SURE:
		return _T("IDS_QUESTIONTITLE_SURE");

	case IDS_SILENT_CHECKVERSION:
		return _T("IDS_SILENT_CHECKVERSION");

	case IDS_SILENT_DOWNLOAD:
		return _T("IDS_SILENT_DOWNLOAD");

	case IDS_SILENT_INSTALL:
		return _T("IDS_SILENT_INSTALL");

	case IDS_SILENT_WELCOME:
		return _T("IDS_SILENT_WELCOME");

	case IDS_SILENT_NOTIFICATION:
		return _T("IDS_SILENT_NOTIFICATION");

	case IDS_SILENT_FINISH:
		return _T("IDS_SILENT_FINISH");

	case IDS_SILENT_ERROR:
		return _T("IDS_SILENT_ERROR");

	case IDS_SILENT_ROLLBACK:
		return _T("IDS_SILENT_ROLLBACK");

	case IDS_ERROR_UNKNOWN:
		return _T("IDS_ERROR_UNKNOWN");

	case IDS_ERROR_NOCONNECTION:
		return _T("IDS_ERROR_NOCONNECTION");

	case IDS_ERROR_SERVER:
		return _T("IDS_ERROR_SERVER");

	case IDS_ERROR_NODISKSPACE:
		return _T("IDS_ERROR_NODISKSPACE");

	case IDS_ERROR_FILENOTFOUND:
		return _T("IDS_ERROR_FILENOTFOUND");

	case IDS_ERROR_COPYFILE:
		return _T("IDS_ERROR_COPYFILE");

	case IDS_ERROR_DELETEFILE:
		return _T("IDS_ERROR_DELETEFILE");

	case IDS_ERROR_RUNFILE:
		return _T("IDS_ERROR_RUNFILE");

	case IDS_ERROR_CLOSEAPP:
		return _T("IDS_ERROR_CLOSEAPP");

	case IDS_ERROR_FILECORRUPT:
		return _T("IDS_ERROR_FILECORRUPT");

	case IDS_ERROR_NONEWVERSION:
		return _T("IDS_ERROR_NONEWVERSION");

	case IDS_ERROR_WRITEFILE:
		return _T("IDS_ERROR_WRITEFILE");

	case IDS_FULL_WELCOME_EXPLANATION:
		return _T("IDS_FULL_WELCOME_EXPLANATION");

	case IDS_FULL_WELCOME_TITLE:
		return _T("IDS_FULL_WELCOME_TITLE");

	case IDS_FULL_CHECKVERSION_TITLE:
		return _T("IDS_FULL_CHECKVERSION_TITLE");

	case IDS_FULL_DOWNLOAD_TITLE:
		return _T("IDS_FULL_DOWNLOAD_TITLE");

	case IDS_FULL_INSTALL_TITLE:
		return _T("IDS_FULL_INSTALL_TITLE");

	case IDS_FULL_FINISH_TITLE:
		return _T("IDS_FULL_FINISH_TITLE");

	case IDS_FULL_ROLLBACK_TITLE:
		return _T("IDS_FULL_ROLLBACK_TITLE");

	case IDS_FULL_CHECKVERSION_EXPLANATION:
		return _T("IDS_FULL_CHECKVERSION_EXPLANATION");

	case IDS_FULL_FILEPROGRESS:
		return _T("IDS_FULL_FILEPROGRESS");

	case IDS_FULL_TOTALPROGRESS:
		return _T("IDS_FULL_TOTALPROGRESS");

	case IDS_FULL_FINISH_EXPLANATION:
		return _T("IDS_FULL_FINISH_EXPLANATION");

	case IDS_FULL_DOWNLOAD_EXPLANATION:
		return _T("IDS_FULL_DOWNLOAD_EXPLANATION");

	case IDS_FULL_INSTALL_EXPLANATION:
		return _T("IDS_FULL_INSTALL_EXPLANATION");

	case IDS_FULL_CHECKVERSION_FEATURES:
		return _T("IDS_FULL_CHECKVERSION_FEATURES");

	case IDS_FULL_ROLLBACK_EXPLANATION:
		return _T("IDS_FULL_ROLLBACK_EXPLANATION");

	case IDS_ACTION_COPY:
		return _T("IDS_ACTION_COPY");

	case IDS_ACTION_DELETE:
		return _T("IDS_ACTION_DELETE");

	case IDS_ACTION_RUN:
		return _T("IDS_ACTION_RUN");

	case IDS_ACTION_REGISTRY:
		return _T("IDS_ACTION_REGISTRY");

	case IDS_ACTION_INI:
		return _T("IDS_ACTION_INI");

	case IDS_STATUS_CONNECTINGTOSERVER:
		return _T("IDS_STATUS_CONNECTINGTOSERVER");

	case IDS_STATUS_NONEWVERSION:
		return _T("IDS_STATUS_NONEWVERSION");

	case IDS_STATUS_NEWVERSION:
		return _T("IDS_STATUS_NEWVERSION");

	case IDS_STATUS_INITIALIZING:
		return _T("IDS_STATUS_INITIALIZING");

	case IDS_ERROR_UNZIPFILE:
		return _T("IDS_ERROR_UNZIPFILE");

	case IDS_ACTION_UNZIP:
		return _T("IDS_ACTION_UNZIP");

	case IDS_ACTION_BACKUP:
		return _T("IDS_ACTION_BACKUP");

	case IDS_ABOUT_TITLE:
		return _T("IDS_ABOUT_TITLE");

	case IDS_ABOUT_UPDATERVERSION:
		return _T("IDS_ABOUT_UPDATERVERSION");

	case IDS_ABOUT_DEVELOPEDBY:
		return _T("IDS_ABOUT_DEVELOPEDBY");

	case IDS_ABOUT_WEBSITE:
		return _T("IDS_ABOUT_WEBSITE");

	case IDS_ABOUT_LANGUAGEVERSION:
		return _T("IDS_ABOUT_LANGUAGEVERSION");

	case IDS_ABOUT_TRANSLATEDBY:
		return _T("IDS_ABOUT_TRANSLATEDBY");

	case IDS_ABOUT_LANGUAGEUSED:
		return _T("IDS_ABOUT_LANGUAGEUSED");

	case IDS_ABOUT_SPECIALTHANKS:
		return _T("IDS_ABOUT_SPECIALTHANKS");

	case IDS_ABOUT_THANKS_NAME:
		return _T("IDS_ABOUT_THANKS_NAME");

	case IDS_ABOUT_THANKS_REASON:
		return _T("IDS_ABOUT_THANKS_REASON");

	case IDS_FULL_CHECKVERSION_SUBTITLE:
		return _T("IDS_FULL_CHECKVERSION_SUBTITLE");

	case IDS_FULL_DOWNLOAD_SUBTITLE:
		return _T("IDS_FULL_DOWNLOAD_SUBTITLE");

	case IDS_FULL_INSTALL_SUBTITLE:
		return _T("IDS_FULL_INSTALL_SUBTITLE");

	case IDS_FULL_FINISH_SUBTITLE:
		return _T("IDS_FULL_FINISH_SUBTITLE");

	case IDS_FULL_ROLLBACK_SUBTITLE:
		return _T("IDS_FULL_ROLLBACK_SUBTITLE");

	case IDS_FULL_SELECTUPDATE_EXPLANATION:
		return _T("IDS_FULL_SELECTUPDATE_EXPLANATION");

	case IDS_FULL_SELECTUPDATE_TITLE:
		return _T("IDS_FULL_SELECTUPDATE_TITLE");

	case IDS_FULL_SELECTUPDATE_SUBTITLE:
		return _T("IDS_FULL_SELECTUPDATE_SUBTITLE");

	case IDS_FULL_LICENSE_TITLE:
		return _T("IDS_FULL_LICENSE_TITLE");

	case IDS_FULL_LICENSE_SUBTITLE:
		return _T("IDS_FULL_LICENSE_SUBTITLE");

	case IDS_FULL_LICENSE_AGREE:
		return _T("IDS_FULL_LICENSE_AGREE");

	case IDS_FULL_LICENSE_NOTAGREE:
		return _T("IDS_FULL_LICENSE_NOTAGREE");

	case IDS_AUTHENTICATION_EXPLANATION:
		return _T("IDS_AUTHENTICATION_EXPLANATION");

	case IDS_AUTHENTICATION_USERNAME:
		return _T("IDS_AUTHENTICATION_USERNAME");

	case IDS_AUTHENTICATION_PASSWORD:
		return _T("IDS_AUTHENTICATION_PASSWORD");

	case IDS_AUTHENTICATION_TITLE:
		return _T("IDS_AUTHENTICATION_TITLE");

	case IDS_FULL_HTMLMESSAGE_TITLE:
		return _T("IDS_FULL_HTMLMESSAGE_TITLE");

	case IDS_FULL_HTMLMESSAGE_SUBTITLE:
		return _T("IDS_FULL_HTMLMESSAGE_SUBTITLE");

	case IDS_FULL_SELECTUPDATE_MOREINFO:
		return _T("IDS_FULL_SELECTUPDATE_MOREINFO");

	case IDS_CONFIRMATION_STOPUPDATING_TITLE:
		return _T("IDS_CONFIRMATION_STOPUPDATING_TITLE");

	case IDS_CONFIRMATION_STOPUPDATING_EXPLANATION:
		return _T("IDS_CONFIRMATION_STOPUPDATING_EXPLANATION");

	case IDS_CONFIRMATION_DELETEFILE_TITLE:
		return _T("IDS_CONFIRMATION_DELETEFILE_TITLE");

	case IDS_CONFIRMATION_DELETEFILE_EXPLANATION:
		return _T("IDS_CONFIRMATION_DELETEFILE_EXPLANATION");

	case IDS_CONFIRMATION_CLOSEAPPLICATION_TITLE:
		return _T("IDS_CONFIRMATION_CLOSEAPPLICATION_TITLE");

	case IDS_CONFIRMATION_CLOSEAPPLICATION_EXPLANATION:
		return _T("IDS_CONFIRMATION_CLOSEAPPLICATION_EXPLANATION");

	case IDS_CONFIRMATION_TITLE:
		return _T("IDS_CONFIRMATION_TITLE");

	case IDS_ERROR_TITLE:
		return _T("IDS_ERROR_TITLE");

	case IDS_ERROR_MORE:
		return _T("IDS_ERROR_MORE");

	case IDS_ERROR_UNKNOWN_TITLE:
		return _T("IDS_ERROR_UNKNOWN_TITLE");

	case IDS_ERROR_UNKNOWN_EXPLANATION:
		return _T("IDS_ERROR_UNKNOWN_EXPLANATION");

	case IDS_ERROR_NOCONNECTION_TITLE:
		return _T("IDS_ERROR_NOCONNECTION_TITLE");

	case IDS_ERROR_NOCONNECTION_EXPLANATION:
		return _T("IDS_ERROR_NOCONNECTION_EXPLANATION");

	case IDS_ERROR_NOCONNECTION_MORE:
		return _T("IDS_ERROR_NOCONNECTION_MORE");

	case IDS_ERROR_SERVER_TITLE:
		return _T("IDS_ERROR_SERVER_TITLE");

	case IDS_ERROR_SERVER_EXPLANATION:
		return _T("IDS_ERROR_SERVER_EXPLANATION");

	case IDS_ERROR_SERVER_MORE:
		return _T("IDS_ERROR_SERVER_MORE");

	case IDS_ERROR_NODISKSPACE_TITLE:
		return _T("IDS_ERROR_NODISKSPACE_TITLE");

	case IDS_ERROR_NODISKSPACE_EXPLANATION:
		return _T("IDS_ERROR_NODISKSPACE_EXPLANATION");

	case IDS_ERROR_NODISKSPACE_MORE:
		return _T("IDS_ERROR_NODISKSPACE_MORE");

	case IDS_ERROR_FILENOTFOUND_TITLE:
		return _T("IDS_ERROR_FILENOTFOUND_TITLE");

	case IDS_ERROR_FILENOTFOUND_EXPLANATION:
		return _T("IDS_ERROR_FILENOTFOUND_EXPLANATION");

	case IDS_ERROR_FILENOTFOUND_MORE:
		return _T("IDS_ERROR_FILENOTFOUND_MORE");

	case IDS_ERROR_COPYFILE_TITLE:
		return _T("IDS_ERROR_COPYFILE_TITLE");

	case IDS_ERROR_COPYFILE_EXPLANATION:
		return _T("IDS_ERROR_COPYFILE_EXPLANATION");

	case IDS_ERROR_COPYFILE_MORE:
		return _T("IDS_ERROR_COPYFILE_MORE");

	case IDS_ERROR_DELETEFILE_TITLE:
		return _T("IDS_ERROR_DELETEFILE_TITLE");

	case IDS_ERROR_DELETEFILE_EXPLANATION:
		return _T("IDS_ERROR_DELETEFILE_EXPLANATION");

	case IDS_ERROR_DELETEFILE_MORE:
		return _T("IDS_ERROR_DELETEFILE_MORE");

	case IDS_ERROR_RUNFILE_TITLE:
		return _T("IDS_ERROR_RUNFILE_TITLE");

	case IDS_ERROR_RUNFILE_EXPLANATION:
		return _T("IDS_ERROR_RUNFILE_EXPLANATION");

	case IDS_ERROR_RUNFILE_MORE:
		return _T("IDS_ERROR_RUNFILE_MORE");

	case IDS_ERROR_CLOSEAPP_TITLE:
		return _T("IDS_ERROR_CLOSEAPP_TITLE");

	case IDS_ERROR_CLOSEAPP_EXPLANATION:
		return _T("IDS_ERROR_CLOSEAPP_EXPLANATION");

	case IDS_ERROR_CLOSEAPP_MORE:
		return _T("IDS_ERROR_CLOSEAPP_MORE");

	case IDS_ERROR_FILECORRUPT_TITLE:
		return _T("IDS_ERROR_FILECORRUPT_TITLE");

	case IDS_ERROR_FILECORRUPT_EXPLANATION:
		return _T("IDS_ERROR_FILECORRUPT_EXPLANATION");

	case IDS_ERROR_FILECORRUPT_MORE:
		return _T("IDS_ERROR_FILECORRUPT_MORE");

	case IDS_ERROR_WRITEFILE_TITLE:
		return _T("IDS_ERROR_WRITEFILE_TITLE");

	case IDS_ERROR_WRITEFILE_EXPLANATION:
		return _T("IDS_ERROR_WRITEFILE_EXPLANATION");

	case IDS_ERROR_WRITEFILE_MORE:
		return _T("IDS_ERROR_WRITEFILE_MORE");

	case IDS_ERROR_UNZIPFILE_TITLE:
		return _T("IDS_ERROR_UNZIPFILE_TITLE");

	case IDS_ERROR_UNZIPFILE_EXPLANATION:
		return _T("IDS_ERROR_UNZIPFILE_EXPLANATION");

	case IDS_ERROR_UNZIPFILE_MORE:
		return _T("IDS_ERROR_UNZIPFILE_MORE");

	case IDS_INFORMATION_TITLE:
		return _T("IDS_INFORMATION_TITLE");

	case IDS_INFORMATION_RESTARTSYSTEM_TITLE:
		return _T("IDS_INFORMATION_RESTARTSYSTEM_TITLE");

	case IDS_INFORMATION_RESTARTSYSTEM_EXPLANATION:
		return _T("IDS_INFORMATION_RESTARTSYSTEM_EXPLANATION");

	case IDS_FULL_FINISH_CREATESHORTCUTS:
		return _T("IDS_FULL_FINISH_CREATESHORTCUTS");

	case IDS_FULL_PROTECTION_TITLE:
		return _T("IDS_FULL_PROTECTION_TITLE");

	case IDS_FULL_PROTECTION_SUBTITLE:
		return _T("IDS_FULL_PROTECTION_SUBTITLE");

	case IDS_FULL_PROTECTION_EXPLANATION:
		return _T("IDS_FULL_PROTECTION_EXPLANATION");

	case IDS_FULL_PROTECTION_CHECKLICENSE:
		return _T("IDS_FULL_PROTECTION_CHECKLICENSE");

	case IDS_FULL_PROTECTION_CHECKFILES:
		return _T("IDS_FULL_PROTECTION_CHECKFILES");

	case IDS_ERROR_PROTECTION_TITLE:
		return _T("IDS_ERROR_PROTECTION_TITLE");

	case IDS_ERROR_PROTECTION_EXPLANATION:
		return _T("IDS_ERROR_PROTECTION_EXPLANATION");

	case IDS_ERROR_PROTECTION_MORE:
		return _T("IDS_ERROR_PROTECTION_MORE");

	case IDS_FULL_PROTECTION_GETLICENSE:
		return _T("IDS_FULL_PROTECTION_GETLICENSE");

	case IDS_ERROR_PROTECTION:
		return _T("IDS_ERROR_PROTECTION");

	case IDS_ACTION_REGISTER:
		return _T("IDS_ACTION_REGISTER");

	case IDS_ACTION_SETFILEATTR:
		return _T("IDS_ACTION_SETFILEATTR");

	case IDS_CONFIRMATION_REGISTER_TITLE:
		return _T("IDS_CONFIRMATION_REGISTER_TITLE");

	case IDS_CONFIRMATION_REGISTER_EXPLANATION:
		return _T("IDS_CONFIRMATION_REGISTER_EXPLANATION");

	case IDS_WARNING_REGISTER:
		return _T("IDS_WARNING_REGISTER");

	case IDS_WARNING_TITLE:
		return _T("IDS_WARNING_TITLE");

	case IDS_WARNING_REGISTER_TITLE:
		return _T("IDS_WARNING_REGISTER_TITLE");

	case IDS_WARNING_REGISTER_EXPLANATION:
		return _T("IDS_WARNING_REGISTER_EXPLANATION");

	case IDS_POPUP_BITMAPS:
		return _T("IDS_POPUP_BITMAPS");

	case IDS_RESTORE_TITLE:
		return _T("IDS_RESTORE_TITLE");

	case IDS_RESTORE_EXPLANATION:
		return _T("IDS_RESTORE_EXPLANATION");

	case IDS_RESTORE_PROGRESS:
		return _T("IDS_RESTORE_PROGRESS");

	case IDS_RESTORE_RESTORE:
		return _T("IDS_RESTORE_RESTORE");

	case IDS_RESTORE_CLOSE:
		return _T("IDS_RESTORE_CLOSE");

	case IDS_PROXY_TITLE:
		return _T("IDS_PROXY_TITLE");

	case IDS_PROXY_DIRECT:
		return _T("IDS_PROXY_DIRECT");

	case IDS_PROXY_AUTODETECT:
		return _T("IDS_PROXY_AUTODETECT");

	case IDS_PROXY_MANUAL:
		return _T("IDS_PROXY_MANUAL");

	case IDS_PROXY_HTTP:
		return _T("IDS_PROXY_HTTP");

	case IDS_PROXY_FTP:
		return _T("IDS_PROXY_FTP");

	case IDS_PROXY_AUTOCONFIGURL:
		return _T("IDS_PROXY_AUTOCONFIGURL");

	case IDS_LOGVIEWER_TITLE:
		return _T("IDS_LOGVIEWER_TITLE");

	case IDS_LOGVIEWER_NOLOG:
		return _T("IDS_LOGVIEWER_NOLOG");

	case IDS_AUTHENTICATION_SAVECREDENTIALS:
		return _T("IDS_AUTHENTICATION_SAVECREDENTIALS");

	case IDS_CLOSEAPPLICATION_TITLE:
		return _T("IDS_CLOSEAPPLICATION_TITLE");

	case IDS_CLOSEAPPLICATION_EXPLANATION:
		return _T("IDS_CLOSEAPPLICATION_EXPLANATION");

	case IDS_CLOSEAPPLICATION_ENDNOW:
		return _T("IDS_CLOSEAPPLICATION_ENDNOW");

	case IDS_CLOSEAPPLICATION_APPLICATION:
		return _T("IDS_CLOSEAPPLICATION_APPLICATION");

	case IDS_CONFIRMATION_CLOSEAPPLICATION_USER_EXPLANATION:
		return _T("IDS_CONFIRMATION_CLOSEAPPLICATION_USER_EXPLANATION");

	default:
		return _T("");
	}

	return _T("");
}