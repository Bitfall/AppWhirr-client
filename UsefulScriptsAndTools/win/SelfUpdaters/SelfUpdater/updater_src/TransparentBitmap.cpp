/********************************************************************
	created:	2005/09/10
	created:	10:9:2005   20:28
	filename: 	TransparentBitmap.cpp
	file path:	Updater
	file base:	TransparentBitmap
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "TransparentBitmap.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CTransparentBitmap, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CTransparentBitmap::CTransparentBitmap()
{
	// Set default transparent color (purple)
	m_clrTransparent = RGB(255, 0, 255);
}

//=====================================================================

CTransparentBitmap::~CTransparentBitmap()
{
	// Clean up bitmaps
	m_bmpBitmap.DeleteObject();
	m_bmpMask.DeleteObject();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

bool CTransparentBitmap::LoadBitmap(CString sFilename)
{
	// Declare variables
	HBITMAP hBmpHeader;

	// Clean up bitmap
	m_bmpBitmap.DeleteObject();

	// Load header
	hBmpHeader = (HBITMAP) ::LoadImage(NULL, sFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// Try to load bitmap from file
	if (!m_bmpBitmap.Attach(hBmpHeader))
		return false;

	// Prepare mask
	PrepareMask(&m_bmpMask);

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CTransparentBitmap::LoadBitmap(UINT nResourceID)
{
	// Clean up bitmap
	m_bmpBitmap.DeleteObject();

	// Try to load bitmap from resource
	if (!m_bmpBitmap.LoadBitmap(nResourceID))
		return false;

	// Prepare mask
	PrepareMask(&m_bmpMask);

	// If we get here, function is successful
	return true;
}

//=====================================================================

void CTransparentBitmap::SetTransparentColor(COLORREF clrTransparent)
{
	// Set new color
	m_clrTransparent = clrTransparent;

	// Prepare mask
	PrepareMask(&m_bmpMask);
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CTransparentBitmap::PrepareMask(CBitmap * pBitmapMask)
{
	// Declare variables
	CBitmap * hbmSrcT, * hbmDstT;
	CDC hdcSrc, hdcDst;
	COLORREF clrSaveBK, clrSaveDstText;

	// Check if we have a valid object
	if (m_bmpBitmap.GetSafeHandle() == NULL)
		return;

	// Get dimensions of the source bitmap
	m_bmpBitmap.GetObject(sizeof(BITMAP), &m_bmInfo);

	// Create the mask bitmap
	pBitmapMask->DeleteObject();
	pBitmapMask->CreateBitmap(m_bmInfo.bmWidth, m_bmInfo.bmHeight, 1, 1, NULL);

	// Get 2 DC's
	hdcSrc.CreateCompatibleDC(NULL);
	hdcDst.CreateCompatibleDC(NULL);

	// Load the bitmaps into memory DC's
	hbmSrcT = (CBitmap *) hdcSrc.SelectObject(&m_bmpBitmap);
	hbmDstT = (CBitmap *) hdcDst.SelectObject(pBitmapMask);

	// Change the background to transparent color
	clrSaveBK = hdcSrc.SetBkColor(m_clrTransparent);

	// This call sets up the mask bitmap
	hdcDst.BitBlt(0, 0, m_bmInfo.bmWidth, m_bmInfo.bmHeight, &hdcSrc, 0, 0, SRCCOPY);
	
	// Set text color
	clrSaveDstText = hdcSrc.SetTextColor(RGB(255, 255, 255));
	hdcSrc.SetBkColor(RGB(0, 0, 0));

	// Bitbilt
	hdcSrc.BitBlt(0, 0, m_bmInfo.bmWidth, m_bmInfo.bmHeight, &hdcDst, 0, 0, SRCAND);

	// Clean up by deselecting any objects, and delete the DC's
	hdcDst.SetTextColor(clrSaveDstText);
	hdcSrc.SetBkColor(clrSaveBK);
	hdcSrc.SelectObject(hbmSrcT);
	hdcDst.SelectObject(hbmDstT);
	hdcSrc.DeleteDC();
	hdcDst.DeleteDC();
}	

//=====================================================================

void CTransparentBitmap::DrawTransparentBitmap(CDC * pDC, int xStart,  int yStart,
                                   int wWidth,  int wHeight, CDC * pTmpDC,
                                   int xSource, int ySource)
{
	// Declare variables
	CDC hdcMem;
	CBitmap * pTempBitmap;

	// Create DC
	hdcMem.CreateCompatibleDC(NULL);

	// Select mask
	pTempBitmap = hdcMem.SelectObject(&m_bmpMask);

	// Bitbilt
	pDC->BitBlt(xStart, yStart, wWidth, wHeight, &hdcMem, xSource, ySource, SRCAND);
    pDC->BitBlt(xStart, yStart, wWidth, wHeight, pTmpDC, xSource, ySource, SRCPAINT);

	// Clean up
	hdcMem.SelectObject(pTempBitmap);
	hdcMem.DeleteDC();
}

//=====================================================================

void CTransparentBitmap::OnPaint()
{
	// Declare variables
	CPaintDC dc(this);
	CDC dcMem;
	CBitmap * pTempBitmap;
	CRect rectClient;
	int iWidth, iHeight;

	// Get client rect
	GetClientRect(&rectClient);

	// Create compatible DC
	dcMem.CreateCompatibleDC(&dc);

	// Select the bitmap into the temp memory dc
	pTempBitmap = (CBitmap *) dcMem.SelectObject(&m_bmpBitmap);

	// Check which dimensions to use
	iWidth = (m_bmInfo.bmWidth < rectClient.Width()) ? m_bmInfo.bmWidth : rectClient.Width();
	iHeight = (m_bmInfo.bmHeight < rectClient.Height()) ? m_bmInfo.bmHeight : rectClient.Height();
		
	// Draw transparent bitmap
	DrawTransparentBitmap(&dc, rectClient.left, rectClient.top, iWidth, iHeight, &dcMem, 0, 0);

	// Select old bitmap again
	dcMem.SelectObject(pTempBitmap);

	// Clean up
	dcMem.DeleteDC();
}