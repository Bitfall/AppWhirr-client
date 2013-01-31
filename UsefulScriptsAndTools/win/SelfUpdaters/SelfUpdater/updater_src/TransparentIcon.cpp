/********************************************************************
	created:	2005/09/10
	created:	10:9:2005   20:28
	filename: 	TransparentIcon.cpp
	file path:	Updater
	file base:	TransparentIcon
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "TransparentIcon.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CTransparentIcon, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CTransparentIcon::CTransparentIcon()
{
	// Set default transparent color (purple)
	m_clrTransparent = RGB(255, 0, 255);

	// Set default size (32 x 32)
	m_iWidth = 32;
	m_iHeight = 32;
}

//=====================================================================

CTransparentIcon::~CTransparentIcon()
{
	// Clean up bitmaps
	m_bmpBitmap.DeleteObject();
	m_bmpMask.DeleteObject();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

bool CTransparentIcon::LoadIcon(CString sFilename)
{
	// Declare variables
	HICON hIcon;

	// Load header
	hIcon = (HICON) ::LoadImage(NULL, sFilename, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

	// Try to load bitmap from file
	ConvertIconToBitmap(hIcon);

	// Destroy icon
	DestroyIcon(hIcon);

	// Prepare mask
	PrepareMask(&m_bmpMask);

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CTransparentIcon::LoadIcon(UINT nResourceID)
{
	// Declare variables
	HICON hIcon;

	// Load header
	hIcon = (HICON) ::LoadImage(NULL, MAKEINTRESOURCE(nResourceID), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);

	// Try to load bitmap from file
	ConvertIconToBitmap(hIcon);

	// Destroy icon
	DestroyIcon(hIcon);

	// Prepare mask
	PrepareMask(&m_bmpMask);

	// If we get here, function is successful
	return true;
}

//=====================================================================

bool CTransparentIcon::LoadIcon(HICON hIcon)
{
	// Try to load bitmap from file
	ConvertIconToBitmap(hIcon);

	// Prepare mask
	PrepareMask(&m_bmpMask);

	// If we get here, function is successful
	return true;
}

//=====================================================================

void CTransparentIcon::SetTransparentColor(COLORREF clrTransparent)
{
	// Set new color
	m_clrTransparent = clrTransparent;

	// Prepare mask
	PrepareMask(&m_bmpMask);
}

//=====================================================================

void CTransparentIcon::SetSize(int cx, int cy)
{
	m_iWidth = cx;
	m_iHeight = cy;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CTransparentIcon::PrepareMask(CBitmap * pBitmapMask)
{
	// Declare variables
	CBitmap * hbmSrcT, * hbmDstT;
	CDC hdcSrc, hdcDst;
	COLORREF clrSaveBK, clrSaveDstText;
	BITMAP bmInfo;

	// Check if we have a valid object
	if (m_bmpBitmap.GetSafeHandle() == NULL)
		return;

	// Get dimensions of the source bitmap
	m_bmpBitmap.GetObject(sizeof(BITMAP), &bmInfo);

	// Create the mask bitmap
	pBitmapMask->DeleteObject();
	pBitmapMask->CreateBitmap(bmInfo.bmWidth, bmInfo.bmHeight, 1, 1, NULL);

	// Get 2 DC's
	hdcSrc.CreateCompatibleDC(NULL);
	hdcDst.CreateCompatibleDC(NULL);

	// Load the bitmaps into memory DC's
	hbmSrcT = (CBitmap *) hdcSrc.SelectObject(&m_bmpBitmap);
	hbmDstT = (CBitmap *) hdcDst.SelectObject(pBitmapMask);

	// Change the background to transparent color
	clrSaveBK = hdcSrc.SetBkColor(m_clrTransparent);

	// This call sets up the mask bitmap
	hdcDst.BitBlt(0, 0, bmInfo.bmWidth, bmInfo.bmHeight, &hdcSrc, 0, 0, SRCCOPY);
	
	// Set text color
	clrSaveDstText = hdcSrc.SetTextColor(RGB(255, 255, 255));
	hdcSrc.SetBkColor(RGB(0, 0, 0));

	// Bitbilt
	hdcSrc.BitBlt(0, 0, bmInfo.bmWidth, bmInfo.bmHeight, &hdcDst, 0, 0, SRCAND);

	// Clean up by deselecting any objects, and delete the DC's
	hdcDst.SetTextColor(clrSaveDstText);
	hdcSrc.SetBkColor(clrSaveBK);
	hdcSrc.SelectObject(hbmSrcT);
	hdcDst.SelectObject(hbmDstT);
	hdcSrc.DeleteDC();
	hdcDst.DeleteDC();
}	

//=====================================================================

void CTransparentIcon::DrawTransparentIcon(CDC * pDC, int xStart,  int yStart,
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

void CTransparentIcon::ConvertIconToBitmap(HICON hIcon)
{
	// Declare variables
	CClientDC clDC(this);
	CDC dc;
	CBitmap * pTemp;

	// Create compatible device context
	dc.CreateCompatibleDC(&clDC);

	// Clean up old object
	m_bmpBitmap.DeleteObject();
	
	// Create bitmap
	m_bmpBitmap.CreateCompatibleBitmap(&clDC, m_iWidth, m_iHeight);

	// Select new object
	pTemp = dc.SelectObject(&m_bmpBitmap);

	// Draw icon
	::DrawIconEx(dc.GetSafeHdc(), 0, 0, hIcon, m_iWidth, m_iHeight, 0,
		NULL, DI_NORMAL);

	// Select old object again
	dc.SelectObject(pTemp);

	// Delete temp dc
	dc.DeleteDC();
}

//=====================================================================

void CTransparentIcon::OnPaint()
{
	// Declare variables
	CPaintDC dc(this);
	CDC dcMem;
	CBitmap * pTempBitmap;
	CRect rectClient;

	// Get client rect
	GetClientRect(&rectClient);

	// Create compatible DC
	dcMem.CreateCompatibleDC(&dc);

	// Select the bitmap into the temp memory dc
	pTempBitmap = (CBitmap *) dcMem.SelectObject(&m_bmpBitmap);
		
	// Draw transparent bitmap
	DrawTransparentIcon(&dc, rectClient.left, rectClient.top, m_iWidth, m_iHeight, &dcMem, 0, 0);

	// Select old bitmap again
	dcMem.SelectObject(pTempBitmap);

	// Clean up
	dcMem.DeleteDC();
}