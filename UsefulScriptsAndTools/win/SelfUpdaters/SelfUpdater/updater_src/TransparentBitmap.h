/********************************************************************
	created:	2005/09/10
	created:	10:9:2005   19:46
	filename: 	TransparentBitmap.h
	file path:	Updater
	file base:	TransparentBitmap
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef TRANSPARENTBITMAP_H
#define TRANSPARENTBITMAP_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CTransparentBitmap
//*********************************************************************

class CTransparentBitmap : public CStatic  
{
public:
	// Constructor & destructor
	CTransparentBitmap();
	virtual ~CTransparentBitmap();

	// Functions
	bool LoadBitmap(CString sFilename);
	bool LoadBitmap(UINT nResourceID);
	void SetTransparentColor(COLORREF clrTransparent);

protected:
	// Functions
	void PrepareMask(CBitmap * pBitmapMask);
	void DrawTransparentBitmap(CDC * pDC, int xStart,  int yStart,
                               int wWidth,  int wHeight, CDC * pTmpDC,
                               int xSource, int ySource);
	afx_msg void OnPaint();

	// Variables
	CBitmap  m_bmpBitmap;
	CBitmap  m_bmpMask;
	BITMAP	 m_bmInfo;
	COLORREF m_clrTransparent;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // TRANSPARENTBITMAP_H
