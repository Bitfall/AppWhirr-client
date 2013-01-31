/********************************************************************
	created:	2005/09/08
	created:	8:9:2005   15:41
	filename: 	NotifyDlg.h
	file path:	Updater
	file base:	NotifyDlg
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef NOTIFYDLG_H
#define NOTIFYDLG_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "XHTMLStatic.h"

//*********************************************************************
// CONSTANTS
//*********************************************************************

const int TIMER_TRANSPARENT = 0;
const int TIMER_SHOW = 1;

const int FADEMODE_IN = 0;
const int FADEMODE_OUT = 1;

const int FADE_MIN = 0;
const int FADE_MAX = 255;

const CString FONTSTYLE_BOLD = "bold";
const CString FONTSTYLE_ITALIC = "italic";
const CString FONTSTYLE_UNDERLINE = "underline";
const CString FONTSTYLE_STRIKEOUT = "strikeout";

//*********************************************************************
// CNotifyDlg
//*********************************************************************

class CNotifyDlg : public CWnd
{
	DECLARE_DYNAMIC(CNotifyDlg)

public:
	// Constructor & destructor
	CNotifyDlg(CString sBitmapLocation);
	virtual ~CNotifyDlg();

	// Functions
	void	 FadeIn();
	void	 FadeOut();
	void	 SetMaxTransparency(int iTransparency = 255);
	int		 GetMaxTransparency();
	void	 SetShowTime(int iTime);
	int		 GetShowTime();
	void	 SetFadeInSpeed(int iSpeed = 10);
	int		 GetFadeInSpeed();
	void	 SetFadeOutSpeed(int iSpeed = 10);
	int		 GetFadeOutSpeed();
	void	 SetBitmap(CString sLocation);
	CString  GetBitmap();
	void	 SetTransparentColor(COLORREF color);
	COLORREF GetTransparentColor();
	void	 SetTitleColor(COLORREF color);
	COLORREF GetTitleColor();
	void	 SetTitleRect(CRect rect);
	CRect	 GetTitleRect();
	void	 SetDescriptionRect(CRect rect);
	CRect	 GetDescriptionRect();
	void	 SetCloseRect(CRect rect);
	CRect	 GetCloseRect();
	void	 SetTitle(CString sText);
	CString	 GetTitle();
	void	 SetDescription(CString sText);
	CString	 GetDescription();
	void	 SetTitleFont(CString sFace, int iSize, CString sStyle);

protected:
	// Functions
	void GetScreenCoordinates();
	afx_msg BOOL    OnEraseBkgnd(CDC * pDC);
	afx_msg void    OnPaint();
	afx_msg void    OnTimer(UINT nIDEvent);
	afx_msg BOOL    OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
	afx_msg void    OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void    OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

	// Variables
	int		 m_iMaxTransparency;		// Max transparency level
	int		 m_iCurrentTransparency;	// Current transparency level when fading in or out
	int		 m_iTimer;					// Timer ID
	int		 m_iShowTime;				// Time to show the dialog when fully visible (when max transparency is reached)
	int		 m_iShowTimeRemaining;		// Show time remaining
	int		 m_iFadeMode;				// Are we fading in or out, it's in here
	int		 m_iSpeedFadeIn;			// Fade in speed
	int		 m_iSpeedFadeOut;			// Fade out speed
	int		 m_iSkinWidth;				// Skin width
	int		 m_iSkinHeigth;				// Skin height
	int		 m_iScreenWidth;			// Screen width
	int		 m_iScreenHeight;			// Screen height
	int		 m_iDesktopWidth;			// Desktop width
	int		 m_iDesktopHeight;			// Desktop height
	COLORREF m_clrTransparent;			// The color that should be transparent
	COLORREF m_clrTitle;				// The color of the title
	CRect	 m_rectTitle;				// Title rect
	CRect	 m_rectDescription;			// Description rect
	CRect	 m_rectClose;				// Close button rect
	CString	 m_sTitle;					// Title text
	CString	 m_sDescription;			// Description text	
	CString  m_sBitmapLocation;			// Bitmap location
	CFont	 m_fntTitle;				// Title font
	CBitmap  m_bmpSkin;					// Skin image
	CXHTMLStatic m_lblDescription;		// Label that supports HTML tags

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // NOTIFYDLG_H
