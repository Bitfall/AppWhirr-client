/**********************************************************************
**
**	ThemeUtil.h : include file
**
**	by Andrzej Markowski June 2004
**
**********************************************************************/

#pragma once

#include "TmSchema.h"

#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED      0x031A
#endif

#ifndef SPI_GETGRADIENTCAPTIONS 
#define SPI_GETGRADIENTCAPTIONS             0x1008
#define COLOR_GRADIENTACTIVECAPTION			27
#define COLOR_GRADIENTINACTIVECAPTION		28
#endif

typedef struct _MY_MARGINS
{
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
} MY_MARGINS;

typedef struct _COLORGLYPH
{
	CDC* pDC;
	COLORREF rgbTransGlyph;
	int nWidth;
	int nHeight;
	struct _COLORCNT
	{
		COLORREF rgbGlyph;
		int	nCnt;
	} a[4];
} COLORGLYPH;

class CThemeUtil
{
public:
	static LONG GetWinVersion();
	CThemeUtil();
	virtual ~CThemeUtil();
	BOOL TransparentBlt(HDC hdcDest,int nXOriginDest,int nYOriginDest,int nWidthDest,int hHeightDest,
						HDC hdcSrc,int nXOriginSrc,int nYOriginSrc,int nWidthSrc,int nHeightSrc,UINT crTransparent);
	BOOL GradientFill(HDC hdc, CONST PTRIVERTEX pVertex, DWORD dwNumVertex, CONST PVOID pMesh, DWORD dwNumMesh, DWORD dwMode);
	BOOL OpenThemeData(HWND hWnd, LPCWSTR pszClassList);
	void CloseThemeData();
	BOOL DrawThemePart(HDC hdc, int iPartId, int iStateId, const RECT *pRect);
	int  GetThemeSysSize(int iSizeId);
	BOOL GetThemeColor(int iPartId, int iStateId, int iPropId, const COLORREF *pColor);
	BOOL GetThemeBool(int iPartId, int iStateId, int iPropId, OUT BOOL *pfVal);
	BOOL GetThemePosition(int iPartId, int iStateId, int iPropId, OUT POINT *pPoint);
	BOOL GetThemeInt(int iPartId, int iStateId, int iPropId, const int *piVal);
	BOOL GetThemeMargins(int iPartId, int iStateId, int iPropId, const MY_MARGINS *pMargins);
	BOOL GetThemeEnumValue(int iPartId, int iStateId, int iPropId, const int *piVal);
	BOOL GetThemeFilename(int iPartId, int iStateId, int iPropId, 
							OUT LPWSTR pszThemeFileName, int cchMaxBuffChars);
	BOOL GetCurrentThemeName(OUT LPWSTR pszThemeFileName, int cchMaxNameChars, 
							OUT OPTIONAL LPWSTR pszColorBuff, int cchMaxColorChars,
							OUT OPTIONAL LPWSTR pszSizeBuff, int cchMaxSizeChars);
	int IsThemeBackgroundPartiallyTransparent(int iPartId, int iStateId);
	BOOL IsThemeActive();
	BOOL IsLunaTheme() {return m_fLuna;}
	HBITMAP LoadBitmap(LPWSTR pszBitmapName);
private:
	BOOL IsWinXP(void);
	void FreeLibrary();
private:
	HINSTANCE m_hUxThemeDll;
	HINSTANCE m_hRcDll;
	HINSTANCE m_hDll;
	UINT m_hTheme;
	BOOL m_fLuna;
};
