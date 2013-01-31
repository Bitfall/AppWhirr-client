/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	Sections.cpp
	file path:	Updater
	file base:	Sections
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Sections.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CSections> CSections::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CSections::CSections()
{
	// Set default values
	m_iSectionCount = 0;
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CSections * CSections::Instance()
{
	if(sm_inst.get() == 0)
	sm_inst = auto_ptr<CSections>(new CSections);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CSections);
	return sm_inst.get();
	*/
}

//=====================================================================

void CSections::ReadFile(CString sFilename)
{
	// Declare variables
	CMarkup xml;
	CString sTemp, sXML, sType;

	// Copy file into cstring
	sXML = XML_to_CString(sFilename);

	// Open XML file
	xml.SetDoc(sXML);
	if (!xml.FindElem(_T("SECTIONS")))
		return;
	xml.IntoElem();

	// Parse the elements part
	while (xml.FindElem(_T("SECTION")))
	{
		// Read attributes
		m_arrSections[m_iSectionCount].sName = xml.GetAttrib(_T("name"));
		m_arrSections[m_iSectionCount++].sVersion = xml.GetAttrib(_T("version"));
	}
}

//=====================================================================

void CSections::WriteFile(CString sFilename)
{
	// Declare variables
	CMarkup xml;

	// Should we even write the section file?
	if (m_iSectionCount <= 0)
		return;

	// Add sections element
	xml.SetDoc(_T("<?xml version=\"1.0\" ?>"));
	xml.AddElem(_T("SECTIONS"));
	xml.IntoElem();

	// Add all elements
	for (int i = 0; i < m_iSectionCount; i++)
	{
		xml.AddElem(_T("SECTION"));
		xml.AddAttrib(_T("name"), m_arrSections[i].sName);
		xml.AddAttrib(_T("version"), m_arrSections[i].sVersion);
	}

	// Write to file
	xml.Save(sFilename);
}

//=====================================================================

SectionInfo CSections::GetSectionInfo(int iIndex)
{
	// Return value
	return m_arrSections[iIndex];
}

//=====================================================================

int	CSections::GetSectionCount()
{
	// Return value
	return m_iSectionCount;
}

//=====================================================================

CString CSections::GetSectionVersion(CString sSectionName)
{
	// Convert to lowercase
	sSectionName = StringLower(sSectionName);

	// Search for the section
	for (int i = 0; i < m_iSectionCount; i++)
	{
		// Have we found the right section?
		if (sSectionName == m_arrSections[i].sName)
			return m_arrSections[i].sVersion;
	}

	// If we get here, we need to return 0.0.0.0
	return _T("0.0.0.0");
}

//=====================================================================

void CSections::SetSectionVersion(CString sSectionName, CString sVersion)
{
	// Convert to lowercase
	sSectionName = StringLower(sSectionName);

	// Search for the section
	for (int i = 0; i < m_iSectionCount; i++)
	{
		// Have we found the right section?
		if (sSectionName == m_arrSections[i].sName)
		{
			// Set version and return
			m_arrSections[i].sVersion = sVersion;
			return;
		}
	}

	// If we get here, our item does not exists, create it
	m_arrSections[m_iSectionCount].sName = sSectionName;
	m_arrSections[m_iSectionCount++].sVersion = sVersion;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

CString CSections::XML_to_CString(CString sFilename)
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