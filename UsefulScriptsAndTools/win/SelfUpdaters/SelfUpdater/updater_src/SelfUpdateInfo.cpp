/********************************************************************
	created:	2005/05/02
	created:	2:5:2005   11:00
	filename: 	SelfUpdateInfo.cpp
	file path:	Updater
	file base:	SelfUpdateInfo
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "SelfUpdateInfo.h"
#include "FileVersion.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CSelfUpdateInfo> CSelfUpdateInfo::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CSelfUpdateInfo::CSelfUpdateInfo()
{
	// Declare variables
	CPath * pPath = CPath::Instance();

	// Set current version as newest version
	CFileVersion fileVersion;
	fileVersion.Open(pPath->GetPathUpdaterExecutable());
	m_sVersion = fileVersion.GetFileVersion();
	fileVersion.Close();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CSelfUpdateInfo * CSelfUpdateInfo::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = auto_ptr<CSelfUpdateInfo>(new CSelfUpdateInfo);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CSelfUpdateInfo);
	return sm_inst.get();
	*/
}

//=====================================================================

void CSelfUpdateInfo::ReadFile(CString sUpdateFile)
{
	// Declare variables
	CMarkup xml;
	CString sXML;

	// Get XML file
	sXML = XML_to_CString(sUpdateFile);

	// Open XML file
	xml.SetDoc(sXML);
	if (!xml.FindElem(_T("SELFUPDATE")))
		return;
	xml.IntoElem();

	// Find version part
	if (!xml.FindElem(_T("VERSION")))
		return;
	m_sVersion = xml.GetElemContent();
	
	// Find location part
	if (!xml.FindElem(_T("LOCATION")))
		return;
	m_sDownloadLocation = xml.GetElemContent();
}

//=====================================================================

CString CSelfUpdateInfo::GetDownloadLocation()
{
	// Return download location
	return m_sDownloadLocation;
}

//=====================================================================

CString CSelfUpdateInfo::GetVersion()
{
	// Return version
	return m_sVersion;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

CString CSelfUpdateInfo::XML_to_CString(CString sFilename)
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