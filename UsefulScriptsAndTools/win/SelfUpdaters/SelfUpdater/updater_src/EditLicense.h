/********************************************************************
	created:	2005/12/23
	created:	23:12:2005   0:08
	filename: 	EditLicense.h
	file path:	Updater
	file base:	EditLicense
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef EDITLICENSE_H
#define EDITLICENSE_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CEditLicense
//*********************************************************************

class CEditLicense : public CEdit
{
public:
	// Constructor & destructor
	CEditLicense();
	virtual ~CEditLicense();

	// Functions

protected:
	// Functions
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

	// Variables
	CBrush	 m_oBrush;
	COLORREF m_clrBackground;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // EDITLICENSE_H
