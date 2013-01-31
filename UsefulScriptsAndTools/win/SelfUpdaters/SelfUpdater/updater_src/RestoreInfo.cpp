/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	RestoreInfo.cpp
	file path:	Updater
	file base:	RestoreInfo
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "RestoreInfo.h"
#include "Sections.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CRestoreInfo> CRestoreInfo::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CRestoreInfo::CRestoreInfo()
{
	// Set some values
	m_iSectionCount = 0;
	m_iRestoreActionCount = 0;
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CRestoreInfo * CRestoreInfo::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = auto_ptr<CRestoreInfo>(new CRestoreInfo);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
		sm_inst.reset(new CRestoreInfo);
	return sm_inst.get();
	*/
}

//=====================================================================

CRestoreActionBase * CRestoreInfo::GetAction(int iIndex)
{
	// Return action
	return m_arrRestoreActions[iIndex];
}

//=====================================================================

void CRestoreInfo::AddAction(CRestoreActionBase * pAction)
{
	// Declare variables
	bool bValid = false;

	// Check what type of action to add
	switch (pAction->GetType())
	{
	case RESTOREACTION_COPY:
		if (IsValidActionCopy(pAction))
			bValid = true;
		break;

	case RESTOREACTION_DELETE:
		if (IsValidActionDelete(pAction))
			bValid = true;
		break;

	case RESTOREACTION_REGISTRY:
		// Always perform registry actions
		bValid = true;
		break;

	case RESTOREACTION_UNREGISTER:
		// Always perform unregister actions
		bValid = true;
		break;
	}	

	// Add action to array
	if (bValid)
		m_arrRestoreActions[m_iRestoreActionCount++] = pAction;
}

//=====================================================================

void CRestoreInfo::AddActionCopy(CString sOldLocation, CString sNewLocation)
{
	// Set up information
	CRestoreActionBase * pAction = new CRestoreActionCopy(sOldLocation, sNewLocation);

	// Add to array
	AddAction(pAction);
}

//=====================================================================

void CRestoreInfo::AddActionDelete(CString sLocation)
{
	// Set up information
	CRestoreActionBase * pAction = new CRestoreActionDelete(sLocation);
	
	// Add to array
	AddAction(pAction);
}

//=====================================================================

void CRestoreInfo::AddActionRegistry(CString sKey, CString sValue)
{
	// Set up information
	CRestoreActionBase * pAction = new CRestoreActionRegistry(sKey, sValue);
	
	// Add to array
	AddAction(pAction);
}

//=====================================================================

void CRestoreInfo::AddActionUnregister(CString sFile)
{
	// Set up information
	CRestoreActionBase * pAction = new CRestoreActionUnregister(sFile);
	
	// Add to array
	AddAction(pAction);
}

//=====================================================================

int CRestoreInfo::GetActionCount()
{
	// Return value
	return m_iRestoreActionCount;
}

//=====================================================================

void CRestoreInfo::SetVersion(CString sVersion)
{
	// Set value
	m_sVersion = sVersion;
}

//=====================================================================

CString CRestoreInfo::GetVersion()
{
	// Return value
	return m_sVersion;
}

//=====================================================================

void CRestoreInfo::AddSection(CString sName, CString sVersion)
{
	// Declare variables
	bool bFound = false;
	SectionInfo secInfo;
	
	// Check if the section isn't already added to the list
	for (int i = 0; i < m_iSectionCount; i++)
	{
		if (m_arrSections[i].sName == sName)
			bFound = true;
	}

	// If section is not found in the list
	if (!bFound)
	{
		// Prepare data
		secInfo.sName = sName;
		secInfo.sVersion = sVersion;

		// Add to list
		m_arrSections[m_iSectionCount++] = secInfo;
	}
}

//=====================================================================

int CRestoreInfo::GetSectionCount()
{
	// Return value
	return m_iSectionCount;
}

//=====================================================================

SectionInfo CRestoreInfo::GetSection(int iIndex)
{
	// Return value
	return m_arrSections[iIndex];
}

//=====================================================================

bool CRestoreInfo::RetrieveInformationFromRollback()
{
	// Declare variables
	CRollbackInfo * pRollbackInfo = CRollbackInfo::Instance();
	CSections * pSections = CSections::Instance();
	CSettings * pSettings = CSettings::Instance();
	SectionInfo sectionInfo;
	CRestoreActionBase * pAction;
	int i;

	// Clean up old information
	CleanUp();

	// Get general version
	m_sVersion = pSettings->GetAppVersion();

	// Get sections
	for (i = 0; i < pSections->GetSectionCount(); i++)
	{
		// Get data
		sectionInfo.sName = pSections->GetSectionInfo(i).sName;
		sectionInfo.sVersion = pSections->GetSectionInfo(i).sVersion;

		// Add to array
		m_arrSections[m_iSectionCount++] = sectionInfo;
	}

	// Get rollback actions
	for (i = 0; i < pRollbackInfo->GetActionCount(); i++)
	{
		// Set action to NULL
		pAction = NULL;

		// Get data
		if (pRollbackInfo->GetAction(i)->GetType() == ROLLBACKACTION_COPY)
		{
			// Copy action
			CRollbackActionCopy * pCopy = (CRollbackActionCopy *) pRollbackInfo->GetAction(i);
			CString sOldLocation, sNewLocation;
			
			// Get data
			sOldLocation = pCopy->GetOldLocation();
			sNewLocation = pCopy->GetNewLocation();

			// Create action
			pAction = new CRestoreActionCopy(sOldLocation, sNewLocation);
		}

		if (pRollbackInfo->GetAction(i)->GetType() == ROLLBACKACTION_DELETE)
		{
			// Delete action
			CRollbackActionDelete * pDelete = (CRollbackActionDelete *) pRollbackInfo->GetAction(i);
			CString sLocation;
			
			// Get data
			sLocation = pDelete->GetLocation();

			// Create action
			pAction = new CRestoreActionDelete(sLocation);
		}

		if (pRollbackInfo->GetAction(i)->GetType() == ROLLBACKACTION_REGISTRY)
		{
			// Registry action
			CRollbackActionRegistry * pRegistry = (CRollbackActionRegistry *) pRollbackInfo->GetAction(i);
			CString sKey, sValue;
			
			// Get data
			sKey = pRegistry->GetKey();
			sValue = pRegistry->GetValue();

			// Create action
			pAction = new CRestoreActionRegistry(sKey, sValue);
		}

		if (pRollbackInfo->GetAction(i)->GetType() == ROLLBACKACTION_UNREGISTER)
		{
			// Unregister action
			CRollbackActionUnregister * pUnregister = (CRollbackActionUnregister *) pRollbackInfo->GetAction(i);
			CString sFile;
			
			// Get data
			sFile = pUnregister->GetFile();

			// Create action
			pAction = new CRestoreActionUnregister(sFile);
		}

		// If action is valid, add it
		if (pAction != NULL)
		{
			// Add to array
			m_arrRestoreActions[m_iRestoreActionCount++] = pAction;
		}
	}

	// Return value
	return true;
}

//=====================================================================

bool CRestoreInfo::WriteToFile(CString sLocation)
{
	// Declare variables
	CString sXML = _T(""), sTemp;
	CStdioFile * pFile;
	int i;

	// Add opening
	sTemp.Format(_T("<RESTOREINFO version=\"%s\">\r\n"), m_sVersion);
	sXML += sTemp;

	// Write sections
	sXML += _T("  <SECTIONS>\r\n");
    for (i = 0; i < m_iSectionCount; i++)
	{
		sTemp.Format(_T("    <SECTION name=\"%s\" version=\"%s\" />\r\n"),
			m_arrSections[i].sName, m_arrSections[i].sVersion);
		sXML += sTemp;
	}
	sXML += _T("  </SECTIONS>\r\n");

	// Write actions
	sXML += _T("  <ACTIONS>\r\n");
	for (i = 0; i < m_iRestoreActionCount; i++)
	{
		sXML += m_arrRestoreActions[i]->GetXmlOutput();
	}
	sXML += _T("  </ACTIONS>\r\n");

	// Add ending
	sXML += _T("</RESTOREINFO>");

	try
	{
		// Create file
		pFile = new CStdioFile(sLocation, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

		// Write first part so the file is unicode
		TCHAR bom = (TCHAR)0xFEFF;
		pFile->Write(&bom, sizeof(TCHAR));

		// Write data to file
		pFile->Write(sXML.GetBuffer(sXML.GetLength()), sXML.GetLength() * sizeof(TCHAR));

		// Close & clean up file
		pFile->Close();
		delete pFile;
	}
	catch (CException * pEx)
	{	
		// Delete exception
		pEx->Delete();

		// Return false
		return false;
	}

	// Return value
	return true;
}

//=====================================================================

bool CRestoreInfo::ReadFromFile(CString sLocation)
{
	// Declare variables
	CMarkup xml;
	CRestoreActionBase * pAction;
	SectionInfo sectionInfo;

	// Clean up old information
	CleanUp();

	// Read xml file into memory
	xml.SetDoc(XML_to_CString(sLocation));

	// Get version
	if (!xml.FindElem(_T("RESTOREINFO")))
		return false;
	m_sVersion = xml.GetAttrib(_T("version"));

	// Jump into element
	xml.IntoElem();

	// Read sections
	if (xml.FindElem(_T("SECTIONS")))
	{
		// Jump into element
		xml.IntoElem();

		// As long as we can find sections
		while (xml.FindElem(_T("SECTION")))
		{
			// Clean up object
			sectionInfo.sName = _T("");
			sectionInfo.sVersion = _T("");

			// Get all attributes
			sectionInfo.sName = xml.GetAttrib(_T("name"));
			sectionInfo.sVersion = xml.GetAttrib(_T("version"));

			// Add to array
			m_arrSections[m_iSectionCount++] = sectionInfo;
		}
			
		// Jump out of element
		xml.OutOfElem();
	}

	// Reset read pointer
	xml.ResetMainPos();

	// Read actions
	if (xml.FindElem(_T("ACTIONS")))
	{
		// Jump into element
		xml.IntoElem();

		// While we can find actions
		while (xml.FindElem(_T("ACTION")))
		{
			// Reset pointer to	NULL
			pAction = NULL;

			// Check type
			if (xml.GetAttrib(_T("type")) == _T("copy"))
			{
				// Copy action
				CString sOldLocation, sNewLocation;

				// Get all attributes
				sOldLocation = xml.GetAttrib(_T("old"));
				sNewLocation = xml.GetAttrib(_T("new"));

				// Create action
				pAction = new CRestoreActionCopy(sOldLocation, sNewLocation);
			}

			if (xml.GetAttrib(_T("type")) == _T("delete"))
			{
				// Delete action
				CString sLocation;

				// Get all attributes
				sLocation = xml.GetAttrib(_T("location"));

				// Create action
				pAction = new CRestoreActionDelete(sLocation);
			}

			if (xml.GetAttrib(_T("type")) == _T("registry"))
			{
				// Registry action
				CString sKey, sValue;

				// Get all attributes
				sKey = xml.GetAttrib(_T("key"));
				sValue = xml.GetAttrib(_T("value"));

				// Create action
				pAction = new CRestoreActionRegistry(sKey, sValue);
			}

			if (xml.GetAttrib(_T("type")) == _T("unregister"))
			{
				// Unregister action
				CString sFile;

				// Get all attributes
				sFile = xml.GetAttrib(_T("file"));

				// Create action
				pAction = new CRestoreActionUnregister(sFile);
			}

			// Should we add action?
			if (pAction != NULL)
			{
				// Yes, add
				m_arrRestoreActions[m_iRestoreActionCount++] = pAction;
			}
		}

		// Jump out of element
		xml.OutOfElem();
	}

	// Return value
	return true;
}

//=====================================================================

void CRestoreInfo::CleanUp()
{
	// Delete all Restore actions
	for (int i = 0; i < m_iRestoreActionCount; i++)
	{
		// Delete object
		delete m_arrRestoreActions[i];
	}

	// Reset counters
	m_iRestoreActionCount = 0;
	m_iSectionCount = 0;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

bool CRestoreInfo::IsValidActionCopy(CRestoreActionBase * pAction)
{
	// Declare variables
	CRestoreActionCopy * pTemp;
	bool bValid = true;

	// Get object
	CRestoreActionCopy * pCopy = (CRestoreActionCopy *)pAction;

	// Check if file is't already in to be copied
	for (int i = 0; i < GetActionCount(); i++)
	{
		if (GetAction(i)->GetType() == RESTOREACTION_COPY)
		{
			// Get temp object
			pTemp = (CRestoreActionCopy *)GetAction(i);

			// Check if file isn't already begin copied
			if (pCopy->GetNewLocation() == pTemp->GetNewLocation())
				bValid = false;
		}
	}

	// Return value
	return bValid;
}

//=====================================================================

bool CRestoreInfo::IsValidActionDelete(CRestoreActionBase * pAction)
{
	// Declare variables
	CRestoreActionDelete * pTemp;
	bool bValid = true;
	
	// Get object
	CRestoreActionDelete * pDelete = (CRestoreActionDelete *)pAction;
	
	// Check if file isn't already in to be deleted
	for (int i = 0; i < GetActionCount(); i++)
	{
		if (GetAction(i)->GetType() == RESTOREACTION_DELETE)
		{
			// Get temp object
			pTemp = (CRestoreActionDelete *)GetAction(i);
			
			// Check if file isn't already being deleted
			if (pDelete->GetLocation() == pTemp->GetLocation())
				bValid = false;
		}
	}
	
	// Return value
	return bValid;
}

//=====================================================================

CString CRestoreInfo::XML_to_CString(CString sFilename)
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