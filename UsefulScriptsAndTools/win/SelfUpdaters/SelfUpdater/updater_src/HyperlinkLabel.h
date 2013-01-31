/********************************************************************
	created:	2005/08/03
	created:	3:8:2005   13:49
	filename: 	HyperlinkLabel.h
	file path:	Updater
	file base:	HyperlinkLabel
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef HYPERLINKLABEL_H
#define HYPERLINKLABEL_H

// Define message
extern UINT WM_HYPERLINK_CLICKED;

// Define timer
const UINT TIMER_HOVER = 100;

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CHyperlinkLabel
//*********************************************************************

class CHyperlinkLabel : public CStatic
{
public:
	// Constructor & destructor
	CHyperlinkLabel(CString sHyperlink = _T(""), bool bHandleClickEvent = true);
	virtual ~CHyperlinkLabel();

	// Functions
	void SetHyperlink(CString sHyperlink);
	void SetClickHandling(bool bHandleClickEvent);
	void SetAutoResize(bool bAutoResize);
	void SetColorLink(COLORREF clrColor);
	void SetColorHover(COLORREF clrColor);

protected:
	// Functions
	afx_msg HBRUSH  CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL    OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
	afx_msg void    OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	void CheckHovering();

	// Variables
	CString  m_sHyperlink;
	bool	 m_bHandleClickEvent;
	bool	 m_bAutoResize;
	bool	 m_bHover;
	COLORREF m_clrNormal;
	COLORREF m_clrLink;
	COLORREF m_clrHover;
	COLORREF m_clrCurrent;
	HCURSOR  m_hCursor;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // HYPERLINKLABEL_H
