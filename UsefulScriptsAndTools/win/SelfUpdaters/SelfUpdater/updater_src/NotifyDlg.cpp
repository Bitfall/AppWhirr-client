/********************************************************************
	created:	2005/09/08
	created:	8:9:2005   15:43
	filename: 	NotifyDlg.cpp
	file path:	Updater
	file base:	NotifyDlg
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "NotifyDlg.h"

typedef BOOL (WINAPI* SETLAYEREDWINDOWATTRIBUTES)(HWND, COLORREF, BYTE, DWORD); 

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CNotifyDlg, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CNotifyDlg, CWnd)

//=====================================================================

CNotifyDlg::CNotifyDlg(CString sBitmapLocation)
{
	// Declare variables
	CString sWindowClass;

	// Set all default values
	m_iMaxTransparency = FADE_MAX;
	m_iCurrentTransparency = FADE_MIN;
	m_iTimer = 0;
	m_iShowTime = 30;
	m_iFadeMode = FADEMODE_IN;
	m_iSpeedFadeIn = 10;
	m_iSpeedFadeOut = 10;
	m_clrTransparent = RGB(255, 0, 255);
	m_clrTitle = RGB(255, 255, 255);
	m_rectTitle = CRect(0, 0, 0, 0);
	m_rectDescription = CRect(0, 0, 0, 0);
	m_rectClose = CRect(0, 0, 0, 0);
	m_sTitle = _T("");
	m_sDescription = _T("");

	// Create window class
	sWindowClass = AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW), GetSysColorBrush(COLOR_WINDOW), NULL);

	// Create window with the specified class
	CreateEx(0, sWindowClass, NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL);

	// Set window to front
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// Hide window from taskbar
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	// Get screen coordinates
	GetScreenCoordinates();

	// Create the static control
	m_lblDescription.CreateEx(WS_EX_LEFT, _T("STATIC"), NULL,
		WS_CHILD | WS_VISIBLE, m_rectDescription, this, 0);

	// Load bitmap
	SetBitmap(sBitmapLocation);
}

//=====================================================================

CNotifyDlg::~CNotifyDlg()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

void CNotifyDlg::FadeIn()
{
	// Modify style
	ModifyStyleEx(0, WS_EX_LAYERED);

	// Set mode to fade in
	m_iFadeMode = FADEMODE_IN;

	// Start fade out timer
	m_iTimer = SetTimer(TIMER_TRANSPARENT, 1, NULL);
}

//=====================================================================

void CNotifyDlg::FadeOut()
{
	// Modify style
	ModifyStyleEx(0, WS_EX_LAYERED);

	// Set mode to fade out
	m_iFadeMode = FADEMODE_OUT;

	// Start fade out timer
	m_iTimer = SetTimer(TIMER_TRANSPARENT, 1, NULL);
}

//=====================================================================

void CNotifyDlg::SetMaxTransparency(int iTransparency /* = 255 */)
{
	// Set data
	m_iMaxTransparency = -1;

	if (iTransparency < FADE_MIN)
		m_iMaxTransparency = FADE_MAX;

	if (iTransparency > FADE_MAX)
		m_iMaxTransparency = FADE_MAX;

	if (m_iMaxTransparency == -1)
		m_iMaxTransparency = iTransparency;
}

//=====================================================================

int CNotifyDlg::GetMaxTransparency()
{
	// Return value
	return m_iMaxTransparency;
}

//=====================================================================

void CNotifyDlg::SetShowTime(int iTime)
{
	// Set data
	m_iShowTime = iTime;
}

//=====================================================================

int CNotifyDlg::GetShowTime()
{
	// Return value
	return m_iShowTime;
}

//=====================================================================

void CNotifyDlg::SetFadeInSpeed(int iSpeed)
{
	// Set data
	m_iSpeedFadeIn = iSpeed;
}

//=====================================================================

int	CNotifyDlg::GetFadeInSpeed()
{
	// Return value
	return m_iSpeedFadeIn;
}

//=====================================================================

void CNotifyDlg::SetFadeOutSpeed(int iSpeed)
{
	// Set data
	m_iSpeedFadeOut = iSpeed;
}

//=====================================================================

int	CNotifyDlg::GetFadeOutSpeed()
{
	// Return value
	return m_iSpeedFadeOut;
}

//=====================================================================

void CNotifyDlg::SetBitmap(CString sLocation)
{
	// Declare variables
	HBITMAP hBitmap;
	BITMAP bmpSkin;
	CRect rectWindow;

	// Set data
	m_sBitmapLocation = sLocation;

	// Delete old bitmap object
	m_bmpSkin.DeleteObject();

	// Load bitmap from file
	hBitmap = (HBITMAP) ::LoadImage(NULL, m_sBitmapLocation, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	
	// Use current loaded bitmap
	m_bmpSkin.Attach(hBitmap);

	// Get some extended information about the bitmap
	GetObject(m_bmpSkin.GetSafeHandle(), sizeof(bmpSkin), &bmpSkin);

	// Store size of bitmap
	m_iSkinHeigth = bmpSkin.bmHeight;
	m_iSkinWidth = bmpSkin.bmWidth;

	// Set rect of window
	rectWindow.bottom = m_iDesktopHeight - 10;
	rectWindow.right = m_iDesktopWidth - 10;
	rectWindow.top = rectWindow.bottom - m_iSkinHeigth;
	rectWindow.left = rectWindow.right - m_iSkinWidth;
	MoveWindow(&rectWindow);
}

//=====================================================================

CString CNotifyDlg::GetBitmap()
{
	// Return value
	return m_sBitmapLocation;
}

//=====================================================================

void CNotifyDlg::SetTransparentColor(COLORREF color)
{
	// Set data
	m_clrTransparent = color;
}

//=====================================================================

COLORREF CNotifyDlg::GetTransparentColor()
{
	// Return value
	return m_clrTransparent;
}

//=====================================================================

void CNotifyDlg::SetTitleColor(COLORREF color)
{
	// Set data
	m_clrTitle = color;
}

//=====================================================================

COLORREF CNotifyDlg::GetTitleColor()
{
	// Return value
	return m_clrTitle;
}

//=====================================================================

void CNotifyDlg::SetTitleRect(CRect rect)
{
	// Set data
	m_rectTitle = rect;
}

//=====================================================================

CRect CNotifyDlg::GetTitleRect()
{
	// Return value
	return m_rectTitle;
}

//=====================================================================

void CNotifyDlg::SetDescriptionRect(CRect rect)
{
	// Set data
	m_rectDescription = rect;
	m_lblDescription.MoveWindow(rect);
}

//=====================================================================

CRect CNotifyDlg::GetDescriptionRect()
{
	// Return value
	return m_rectDescription;
}

//=====================================================================

void CNotifyDlg::SetCloseRect(CRect rect)
{
	// Set data
	m_rectClose = rect;
}

//=====================================================================

CRect CNotifyDlg::GetCloseRect()
{
	// Return value
	return m_rectClose;
}

//=====================================================================

void CNotifyDlg::SetTitle(CString sText)
{
	// Set data
	m_sTitle = sText;

	// Redraw window
	RedrawWindow();
}

//=====================================================================

CString CNotifyDlg::GetTitle()
{
	// Return value
	return m_sTitle;
}

//=====================================================================

void CNotifyDlg::SetDescription(CString sText)
{
	// Set data
	m_sDescription = sText;
	m_lblDescription.SetWindowText(sText);

	/* TESTING */
	//m_lblDescription.SetWindowText("text with <font color=\"blue\"><a href=\"http://www.gvhsoftware.org\">hyperlink</a></font>");
}

//=====================================================================

CString CNotifyDlg::GetDescription()
{
	// Return value
	return m_sDescription;
}

//=====================================================================

void CNotifyDlg::SetTitleFont(CString sFace, int iSize, CString sStyle)
{
	// Declare variables
	LOGFONT font;
	CString sTemp;

	// Make lowercase
	sStyle = StringLower(sStyle);

	// Create font with face and size
	m_fntTitle.DeleteObject();
	m_fntTitle.CreatePointFont(iSize * 10, sFace);
	m_fntTitle.GetLogFont(&font);

	// Check for italic
	if (sStyle.Find(FONTSTYLE_ITALIC) != -1)
		font.lfItalic = TRUE;

	// Check for bold
	if (sStyle.Find(FONTSTYLE_BOLD) != -1)
		font.lfWeight = FW_BOLD;

	// Check for underline
	if (sStyle.Find(FONTSTYLE_UNDERLINE) != -1)
		font.lfUnderline = TRUE;

	// Check for strikout
	if (sStyle.Find(FONTSTYLE_STRIKEOUT) != -1)
		font.lfStrikeOut = TRUE;

	// Create new font
	m_fntTitle.DeleteObject();
	m_fntTitle.CreateFontIndirect(&font);
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CNotifyDlg::GetScreenCoordinates()
{
	// Declare variables
	CRect rectDesktop;

	// Get screen data
	m_iScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	m_iScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	// Get desktop data
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDesktop, 0);
	m_iDesktopWidth = rectDesktop.right - rectDesktop.left;
	m_iDesktopHeight = rectDesktop.bottom - rectDesktop.top;
}

//=====================================================================

BOOL CNotifyDlg::OnEraseBkgnd(CDC * pDC)
{
	// Declare variables
	CDC memDC;
	CBitmap * pOldBitmap;

	// Create compatible DC
	memDC.CreateCompatibleDC(pDC);

	// Select skin as object
	pOldBitmap=memDC.SelectObject(&m_bmpSkin);

	// Bitblt the object
	pDC->BitBlt(0, 0, m_iSkinWidth, m_iSkinHeigth, &memDC, 0, 0, SRCCOPY);

	// Select old bitmap again
	memDC.SelectObject(pOldBitmap);

	// Return value
	return TRUE;
}

//=====================================================================

void CNotifyDlg::OnPaint()
{
	// Declare variables
	CPaintDC dc(this);
	CFont * pOldFont;
	TCHAR * szBuffer;
		
	// Set transparency and rect
	dc.SetBkMode(TRANSPARENT); 

	// Draw title
	dc.SetTextColor(m_clrTitle);
	pOldFont = dc.SelectObject(&m_fntTitle);
	szBuffer = new TCHAR[m_sTitle.GetLength() + 1];
	lstrcpy(szBuffer, m_sTitle);
	dc.DrawText(szBuffer, -1, m_rectTitle, DT_WORDBREAK | DT_END_ELLIPSIS);

	// Clean up objects
	delete[] szBuffer;
	dc.SelectObject(pOldFont);

	/*// Draw description
	dc.SetTextColor(m_clrDescription);
	pOldFont = dc.SelectObject(&m_fntDescription);
	szBuffer = new char[m_sDescription.GetLength() + 1];
	strcpy(szBuffer, m_sDescription);
	dc.DrawText(szBuffer, -1, m_rectDescription, DT_WORDBREAK | DT_END_ELLIPSIS);

	// Clean up objects
	delete[] szBuffer;
	dc.SelectObject(pOldFont);*/
}

//=====================================================================

void CNotifyDlg::OnTimer(UINT nIDEvent)
{
	// Check if right ID is used
	if (nIDEvent == TIMER_TRANSPARENT)
	{
		// Check which mode
		switch (m_iFadeMode)
		{
		case FADEMODE_IN:
			// Check if window is visible
			if (!IsWindowVisible())
				ShowWindow(SW_SHOW);

			// Increase
			m_iCurrentTransparency += m_iSpeedFadeIn;

			// Check if we passed border
			if (m_iCurrentTransparency > m_iMaxTransparency)
			{
				// Yes, set to max
				m_iCurrentTransparency = m_iMaxTransparency;

				// Kill timer
				KillTimer(TIMER_TRANSPARENT);

				// Set show time remaining
				m_iShowTimeRemaining = m_iShowTime;

				// Set show timer
				SetTimer(TIMER_SHOW, 100, NULL);
			}	
			break;

		case FADEMODE_OUT:
			// Decrease
			m_iCurrentTransparency -= m_iSpeedFadeOut;
			
			// Check if we passed border
			if (m_iCurrentTransparency < FADE_MIN)
			{
				// Yes, set to min
				m_iCurrentTransparency = FADE_MIN;

				// Hide completely
				ShowWindow(SW_HIDE);

				// Kill timer
				KillTimer(TIMER_TRANSPARENT);
			}
			break;
		}

		// Are we running winxp or win2k?
		std::string sVersion;
		UINT uVersion, uPlatform;
		uVersion = getWinVersion(sVersion, uPlatform);
		if ((uVersion == WINDOWS_XP) || (uVersion == WINDOWS_2000) || (uVersion == WINDOWS_2003))
		{
			// Declare variables
			HMODULE hModule;
			SETLAYEREDWINDOWATTRIBUTES SetLayeredWindowAttributesProc; 

			// Load dll
			hModule = LoadLibrary(_T("user32.dll"));

			// Check if dll is loaded successfully
			if (hModule)
			{
				// Get proc address
				SetLayeredWindowAttributesProc = (SETLAYEREDWINDOWATTRIBUTES) GetProcAddress(hModule, "SetLayeredWindowAttributes");

				// Fade
				if (SetLayeredWindowAttributesProc != NULL)
				{
					SetLayeredWindowAttributesProc(m_hWnd, m_clrTransparent, m_iCurrentTransparency, LWA_ALPHA | LWA_COLORKEY);
				}

				// Release dll
				FreeLibrary(hModule);
			}
		}
		else
		{
			// No fading to support older windows version
			switch (m_iFadeMode)
			{
			case FADEMODE_IN:
				// Show window
				ShowWindow(SW_SHOW);

				// Kill timer
				KillTimer(TIMER_TRANSPARENT);
				break;

			case FADEMODE_OUT:
				// Hide window
				ShowWindow(SW_HIDE);

				// Kill timer
				KillTimer(TIMER_TRANSPARENT);
				break;
			}
		}
	}	

	if (nIDEvent == TIMER_SHOW)
	{
		// Decrease showtime
		m_iShowTimeRemaining--;

		// Check if we should hide
		if (m_iShowTimeRemaining < 0)
		{
			// Kill the timer
			KillTimer(TIMER_SHOW);

			// Call fadeout
			FadeOut();
		}
	}
}

//=====================================================================

BOOL CNotifyDlg::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	// Declare variables
	CPoint point;

	// Call original function
	CWnd::OnSetCursor(pWnd, nHitTest, message);

	// Get mouse position
	::GetCursorPos(&point);
	ScreenToClient(&point);

	// Check if mous is currently inside close rect
	if (m_rectClose.PtInRect(point))
	{	
		// Draw hand cursor
		::SetCursor(LoadCursor(NULL, IDC_HAND));
	}
	else
	{
		// Draw normal cursor
		::SetCursor(LoadCursor(NULL, IDC_ARROW));
	}

	// Return value
	return TRUE;
}

//=====================================================================

void CNotifyDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Call original function
	CWnd::OnLButtonUp(nFlags, point);
	
	// Check if point is inside close rect
	if (m_rectClose.PtInRect(point))
	{
		// Fade out button
		FadeOut();
	}
}

//=====================================================================

void CNotifyDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// Declare variables
	TRACKMOUSEEVENT trackMouseEvent;
	
	// Call original function
	CWnd::OnMouseMove(nFlags, point);
	
	// Prepare information
	trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	trackMouseEvent.dwFlags = TME_HOVER | TME_LEAVE;
	trackMouseEvent.hwndTrack = m_hWnd;
	trackMouseEvent.dwHoverTime = 1;

	// Be sure to enable the message mousehover and mouseleave
	::_TrackMouseEvent(&trackMouseEvent);
}

//=====================================================================

LRESULT CNotifyDlg::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	// Stop timer until we leave the window
	KillTimer(TIMER_SHOW);

	// Return value
	return S_OK;
}

//=====================================================================

LRESULT CNotifyDlg::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	// Restart timer again
	SetTimer(TIMER_SHOW, 100, NULL);

	// Return value
	return S_OK;
}
