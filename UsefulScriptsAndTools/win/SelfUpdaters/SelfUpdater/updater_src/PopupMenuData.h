/********************************************************************
	created:	2005/03/18
	created:	18:3:2005   13:44
	filename: 	PopupMenuData.h
	file path:	Updater
	file base:	PopupMenuData
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef POPUPMENUDATA_H
#define POPUPMENUDATA_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CPopupMenuData
//*********************************************************************

class CPopupMenuData
{
public:
	// Constructor & destructor
	CPopupMenuData(CString sText, CString sLocation, CString sParameters,
				   CString sBitmap)
	{
		m_sText = sText;
		m_sLocation = sLocation;
		m_sParameters = sParameters;
		m_sBitmap = sBitmap;
	}
	~CPopupMenuData() { }

	// Functions
	CString GetText() { return m_sText; }
	CString GetLocation() { return m_sLocation; }
	CString GetParameters() { return m_sParameters; }
	CString GetBitmap() { return m_sBitmap; }

private:
	// Functions

	// Variables
	CString m_sText;
	CString m_sLocation;
	CString m_sParameters;
	CString m_sBitmap;
};

// End of redefinition protection
#endif // POPUPMENUDATA_H
