/********************************************************************
	created:	2005/12/23
	created:	23:12:2005   0:04
	filename: 	EditLicense.cpp
	file path:	Updater
	file base:	EditLicense
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "EditLicense.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CEditLicense, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CEditLicense::CEditLicense()
{
	// Create brush & color
	m_clrBackground = RGB(255, 255, 255);
	m_oBrush.CreateSolidBrush(m_clrBackground);
}

//=====================================================================

CEditLicense::~CEditLicense()
{
	// Release brush
	m_oBrush.DeleteObject();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

HBRUSH CEditLicense::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// Set background color to white, even when read-only
	pDC->SetBkColor(m_clrBackground);

	// Call original function
	return m_oBrush;
}