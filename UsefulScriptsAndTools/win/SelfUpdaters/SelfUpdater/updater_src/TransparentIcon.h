/********************************************************************
	created:	2005/09/10
	created:	10:9:2005   19:46
	filename: 	TransparentIcon.h
	file path:	Updater
	file base:	TransparentIcon
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef TRANSPARENTICON_H
#define TRANSPARENTICON_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CTransparentIcon
//*********************************************************************

class CTransparentIcon : public CStatic  
{
public:
	// Constructor & destructor
	CTransparentIcon();
	virtual ~CTransparentIcon();

	// Functions
	bool LoadIcon(CString sFilename);
	bool LoadIcon(UINT nResourceID);
	bool LoadIcon(HICON hIcon);
	void SetTransparentColor(COLORREF clrTransparent);
	void SetSize(int cx, int cy);

protected:
	// Functions
	void PrepareMask(CBitmap * pBitmapMask);
	void DrawTransparentIcon(CDC * pDC, int xStart,  int yStart,
                               int wWidth,  int wHeight, CDC * pTmpDC,
                               int xSource, int ySource);
	void ConvertIconToBitmap(HICON hIcon);
	afx_msg void OnPaint();

	// Variables
	CBitmap		m_bmpBitmap;
	CBitmap		m_bmpMask;
	COLORREF	m_clrTransparent;
	int			m_iWidth;
	int			m_iHeight;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // TRANSPARENTICON_H
