/********************************************************************
	created:	2005/08/03
	created:	3:8:2005   13:52
	filename: 	HyperlinkLabel.cpp
	file path:	Updater
	file base:	HyperlinkLabel
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "HyperlinkLabel.h"

// Registry message
UINT WM_HYPERLINK_CLICKED = ::RegisterWindowMessage(_T("WM_HYPERLINK_CLICKED"));

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CHyperlinkLabel, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CHyperlinkLabel::CHyperlinkLabel(CString sHyperlink /* = "" */, 
								 bool bHandleClickEvent /* = true */)
{
	// Set hyperlink
	m_sHyperlink = sHyperlink;

	// Set handle click event
	m_bHandleClickEvent = bHandleClickEvent;

	// Set auto resize
	m_bAutoResize = false;

	// Default user is not hovering
	m_bHover = false;
	
	// Set colors
	m_clrNormal = RGB(0, 0, 0);
	m_clrLink = RGB(175, 175, 255);
	m_clrHover = RGB(0, 0, 255);

	// Load cursor
	m_hCursor = LoadCursor(NULL, IDC_HAND);

	// Set current color
	if (m_sHyperlink.IsEmpty())
		m_clrCurrent = m_clrNormal;
	else
		m_clrCurrent = m_clrLink;
}

//=====================================================================

CHyperlinkLabel::~CHyperlinkLabel()
{
	// Release the capture
	ReleaseCapture();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

void CHyperlinkLabel::SetHyperlink(CString sHyperlink)
{
	// Set value
	m_sHyperlink = sHyperlink;

	// Set current color
	if (m_sHyperlink.IsEmpty())
		m_clrCurrent = m_clrNormal;
	else
		m_clrCurrent = m_clrLink;
}

//=====================================================================

void CHyperlinkLabel::SetClickHandling(bool bHandleClickEvent)
{
	// Set value
	m_bHandleClickEvent = bHandleClickEvent;
}

//=====================================================================

void CHyperlinkLabel::SetAutoResize(bool bAutoResize)
{
	// Set value
	m_bAutoResize = bAutoResize;
}

//=====================================================================

void CHyperlinkLabel::SetColorLink(COLORREF clrColor)
{
	// Set value
	m_clrLink = clrColor;
}

//=====================================================================

void CHyperlinkLabel::SetColorHover(COLORREF clrColor)
{
	// Set value
	m_clrHover = clrColor;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

HBRUSH CHyperlinkLabel::CtlColor(CDC* pDC, UINT /*nCtlColor*/) 
{
	// Declare variables
	CString sWindowText;
	CSize size;

	// Get window text
	GetWindowText(sWindowText);

	// Resize control according to text length
	if (m_bAutoResize)
	{
		size = pDC->GetOutputTextExtent(sWindowText);
		SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOREDRAW);
	}

	// Be sure we are getting notified
	ModifyStyle(0, SS_NOTIFY, 0);
	
	// Check which color to draw
	if (m_sHyperlink.IsEmpty())
	{
		// Use normal color
		m_clrCurrent = m_clrNormal;
	}
	else
	{
		// Hover or link color?
		if (m_bHover)
		{
			m_clrCurrent = m_clrHover;
		}
		else
		{
			// Set new color to normal link
			m_clrCurrent = m_clrLink;
		}	
	}

	// Set text color
	pDC->SetTextColor(m_clrCurrent);

	// Transparent text.
	pDC->SetBkMode(TRANSPARENT);

	// Return value
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

//=====================================================================

BOOL CHyperlinkLabel::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	// Call original function
	CStatic::OnSetCursor(pWnd, nHitTest, message);

	// Load hand cursor
	if (m_sHyperlink.IsEmpty())
	{
		// Set normal cursor
		::SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	else
	{
		// Set hand cursor
		::SetCursor(m_hCursor);
	}
	
	// Be sure to check hovering
	CheckHovering();

	return TRUE;
}

//=====================================================================

void CHyperlinkLabel::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Should we handle click event?
	if (!m_bHandleClickEvent)
	{
		// Send click message to parent window
		CWnd * pParent = GetParent();
		if (pParent && ::IsWindow(pParent->m_hWnd))
		{
			// wParam will contain control id
			pParent->SendMessage(WM_HYPERLINK_CLICKED, GetDlgCtrlID());
		}

		return;
	}

	// Prepare shellExecutInfo
	ShellExecute(m_hWnd, _T("open"), m_sHyperlink, _T(""), _T(""), SW_SHOWNORMAL);
}

//=====================================================================

LRESULT CHyperlinkLabel::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	// Set value
	m_bHover = true;

	// Redraw window
	Invalidate();

	// Return value
	return S_OK;
}

//=====================================================================

LRESULT CHyperlinkLabel::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	// Set value
	m_bHover = false;

	// Redraw window
	Invalidate();

	// Return value
	return S_OK;
}

//=====================================================================

BOOL CHyperlinkLabel::OnEraseBkgnd(CDC* pDC)
{
	// Declare variables
	CRect rcWnd;

	// Get some values
	GetWindowRect(&rcWnd);
	CWnd* pParent = GetParent();

	// Be sure we get rid of the messy hyperlink
	pParent->ScreenToClient(&rcWnd);
	pParent->RedrawWindow(&rcWnd, 0, RDW_NOCHILDREN | RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW);

	return TRUE;
}

//=====================================================================

void CHyperlinkLabel::CheckHovering()
{
	// Declare variables
	TRACKMOUSEEVENT msEvent;

	// Set up mouse event
	msEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	msEvent.dwFlags = TME_LEAVE | TME_HOVER;
	msEvent.hwndTrack = m_hWnd;
	msEvent.dwHoverTime = 10;

	// Check if we are hovering
	TrackMouseEvent(&msEvent);
}
