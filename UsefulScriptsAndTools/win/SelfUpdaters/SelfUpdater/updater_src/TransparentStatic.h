/********************************************************************
	created:	2005/09/10
	created:	10:9:2005   19:46
	filename: 	TransparentStatic.h
	file path:	Updater
	file base:	TransparentStatic
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef TRANSPARENTSTATIC_H
#define TRANSPARENTSTATIC_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CTransparentStatic
//*********************************************************************

class CTransparentStatic : public CStatic
{
	DECLARE_DYNAMIC(CTransparentStatic)

public:
	// Constructor & destructor
	CTransparentStatic();
	virtual ~CTransparentStatic();

	// Functions

protected:
	// Functions
	afx_msg void OnPaint();

	// Variables

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // TRANSPARENTSTATIC_H
