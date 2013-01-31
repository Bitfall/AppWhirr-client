/**********************************************************************
**
**	CustomBitmapButton.h : include file
**
**	by Andrzej Markowski August 2004
**
**********************************************************************/

#pragma once

#include "themeutil.h"
#include <afxtempl.h>
#include <afxmt.h>

#define CustomBitmapButton_CLASSNAME    _T("CCustomBitmapButton")

#define CBNST_NORMAL			DNHZS_NORMAL
#define CBNST_HOT				DNHZS_HOT
#define CBNST_PRESSED			DNHZS_PRESSED
#define CBNST_DISABLED			DNHZS_DISABLED

// button styles
#define CBNSTYLE_CAPTION			1	// caption button style
#define CBNSTYLE_USERDEFWIDTH		2	// user defined button width
										// used with CBNSTYLE_CAPTION style
// background styles
#define CBNBKGNDSTYLE_CAPTION		0
#define CBNBKGNDSTYLE_CLOSE			1
#define CBNBKGNDSTYLE_MDI			2
#define CBNBKGNDSTYLE_COMBO			3
#define CBNBKGNDSTYLE_SPINUP		4
#define CBNBKGNDSTYLE_SPINDN		5
#define CBNBKGNDSTYLE_SPINUPHOR		6
#define CBNBKGNDSTYLE_SPINDNHOR		7
#define CBNBKGNDSTYLE_SCROLLUP		8
#define CBNBKGNDSTYLE_SCROLLDOWN	9
#define CBNBKGNDSTYLE_SCROLLLEFT	10
#define CBNBKGNDSTYLE_SCROLLRIGHT	11

// error codes
#define CBNERR_NOERROR						0
#define CBNERR_OUTOFMEMORY					-1
#define CBNERR_INCORRECTPARAMETER			-2
#define CBNERR_INCORRECTBUTTONSTYLE			-3
#define CBNERR_INCORRECTFRAMESTYLE			-4
#define CBNERR_INCORRECTFUNCCALL			-5
#define CBNERR_CREATEBUTTONFAILED			-6
#define CBNERR_CREATETOOLTIPCTRLFAILED		-7
#define CBNERR_COPYIMAGEFAILED				-8
#define CBNERR_SETWINDOWLONGFAILED			-9
#define CBNERR_FRAMEALREADYCREATED			-10
#define CBNERR_FRAMENOTCREATED				-11

#define HELDNONE		0
#define HELDLCLK		1
#define HELDLDBLCLK		2
#define HELDRCLK		3
#define HELDRDBLCLK		4

#ifndef WM_NCMOUSEHOVER
#define WM_NCMOUSEHOVER     0x02A0
#endif

#ifndef WM_NCMOUSELEAVE
#define WM_NCMOUSELEAVE     0x02A2
#endif

#ifndef TME_NONCLIENT
#define TME_NONCLIENT   0x00000010
#endif

class CCustomBitmapButton : public CWnd
{
// Construction
public:
	CCustomBitmapButton();
	virtual ~CCustomBitmapButton();

	BOOL Create(UINT dwStyle, const CRect & rect, CWnd * pParentWnd, UINT nID);
	int CreateCaptionFrame(CWnd * pCaptionWnd, int nIDIcon);
	int DestroyCaptionFrame();
	int AddCaptionButton(LPCRECT lpRect, UINT nID, int nBkStyle, BOOL fUserDefWidth);
	CCustomBitmapButton* GetCaptionButtonPtr(int nButtonID);
	int SetTooltipText(CString sTxt);
	int SetBkStyle(int nBkStyle);
	int GetBkStyle() {return m_nBkStyle;}
	HBITMAP GetGlyphBitmap() {return m_hBmpGlyph;}
	int SetGlyphBitmap(HBITMAP hBmpGlyph);
	int EnableWindow(BOOL bEnable = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomBitmapButton)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomBitmapButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	LONG OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnThemeChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	virtual void PreSubclassWindow();

private:
	static LRESULT CALLBACK CaptionFrameWindowProc(HWND, UINT, WPARAM, LPARAM);
	static CMap <HWND,HWND,CCustomBitmapButton*,CCustomBitmapButton*> m_mapCaptionFrames;
	static CCriticalSection m_cs;

private:
	BOOL RegisterWindowClass();
	void Draw(CDC &dc);
	void DrawGlyph(CDC& dc, CRect& r, int nColorNdx);
	void DrawBk(CDC& dc, CRect& r, int nImageNdx);
	void DrawNCClasic(CWnd* pWnd);
	void DrawNCXP(CWnd* pWnd);
	int RecalcLayout(CWnd* pWnd, int& nCY, int& nPartID, CFont& fn, int& nBtnsWidth);
	void GetGlyphColorHor(int i, int iMin, int iMax, int ndx, COLORGLYPH& st);
	void GetGlyphColorVer(int i, int iMin, int iMax, int ndx, COLORGLYPH& st);

private:
	int m_nIDIcon;
	CRect m_rect;
	int m_nImageCount;
	int m_nImageOffset;
	int m_nBkStyle;
	BOOL m_fActive;
	int m_nState;
	int m_nHeldNdx;
	int m_nOverNdx;
	BYTE m_bHeld;
	CBitmap m_bmpMemCaption;
	CBitmap m_bmpMemButton;
	HBITMAP m_hBmpBk;
	HBITMAP m_hBmpGlyph;
	COLORREF m_rgbGlyph[8];
	BOOL m_fBkImageHorLayout;
	BOOL m_fBkTransparent;
	COLORREF m_rgbBkTransparent;
	MY_MARGINS m_mrgnBk;
	CThemeUtil m_Theme;
	CArray <CCustomBitmapButton*,CCustomBitmapButton*> m_aButtons;
	WNDPROC m_pOldWndProc;
	CWnd* m_pCaptionWnd;
	CToolTipCtrl m_ctrlToolTip;
};
