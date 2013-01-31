/********************************************************************
	created:	2005/09/10
	created:	10:9:2005   20:28
	filename: 	ResultLabel.cpp
	file path:	Updater
	file base:	ResultLabel
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"
#include "ResultLabel.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CResultLabel, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CResultLabel::CResultLabel()
{
	// Set default state
	m_iState = LBLRESULT_NONE;

	// Set default colors
	m_clrToCheck = RGB(0, 0, 0);
	m_clrChecking = RGB(0, 0, 0);
	m_clrSucceeded = RGB(0, 175, 0);
	m_clrNotSucceeded = RGB(255, 0, 0);
	m_clrBackground = ::GetSysColor(COLOR_3DFACE);
}

//=====================================================================

CResultLabel::~CResultLabel()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

void CResultLabel::SetState(int iState /* = LBLRESULT_TOCHECK */)
{
	// Set data
	m_iState = iState;

	// Redraw window
	Invalidate();
}

//=====================================================================

int CResultLabel::GetState()
{
	// Return value
	return m_iState;
}

//=====================================================================

void CResultLabel::SetColorToCheck(COLORREF color)
{
	// Set data
	m_clrToCheck = color;
}

//=====================================================================

COLORREF CResultLabel::GetColorToCheck()
{
	// Return value
	return m_clrToCheck;
}

//=====================================================================

void CResultLabel::SetColorChecking(COLORREF color)
{
	// Set data
	m_clrChecking = color;
}

//=====================================================================

COLORREF CResultLabel::GetColorChecking()
{
	// Return value
	return m_clrChecking;
}

//=====================================================================

void CResultLabel::SetColorSucceeded(COLORREF color)
{
	// Set data
	m_clrSucceeded = color;
}

//=====================================================================

COLORREF CResultLabel::GetColorSucceeded()
{
	// Return value
	return m_clrSucceeded;
}

//=====================================================================

void CResultLabel::SetColorNotSucceeded(COLORREF color)
{
	// Set data
	m_clrNotSucceeded = color;
}

//=====================================================================

COLORREF CResultLabel::GetColorNotSucceeded()
{
	// Return value
	return m_clrNotSucceeded;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CResultLabel::ClearPixels(CPaintDC & dc)
{
	// Declare variables
	CRect rectWindow;

	// Get current rect
	GetWindowRect(&rectWindow);

	// Clear all pixels
	for (int i = 0; i < rectWindow.Width(); i++)
	{
		for (int j = 0; j < rectWindow.Height(); j++)
		{
			// Clear pixel
			dc.SetPixel(i, j, m_clrBackground);
		}
	}
}

//=====================================================================

void CResultLabel::OnPaint()
{
	// Declare variables
	CString sWindowText;
	CPaintDC dc(this);
	TCHAR * szBuffer;
	CRect rectText;
	CFont * pFont = GetFont();
	bool bMoveText = true;

	// Call original function
	CStatic::OnPaint();

	// Get window text
	GetWindowText(sWindowText);

	// Get current window rect
	GetWindowRect(&rectText);

	// Clear pixels
	ClearPixels(dc);

	// Check what state we should draw
	switch (m_iState)
	{
	case LBLRESULT_NONE:
		// Don't draw anything, don't move text
		bMoveText = false;
		break;

	case LBLRESULT_TOCHECK:
		// Draw circle
		dc.SetPixel(5, 6, m_clrToCheck);
		dc.SetPixel(5, 7, m_clrToCheck);

		dc.SetPixel(6, 5, m_clrToCheck);
		dc.SetPixel(6, 6, m_clrToCheck);
		dc.SetPixel(6, 7, m_clrToCheck);
		dc.SetPixel(6, 8, m_clrToCheck);

		dc.SetPixel(7, 4, m_clrToCheck);
		dc.SetPixel(7, 5, m_clrToCheck);
		dc.SetPixel(7, 6, m_clrToCheck);
		dc.SetPixel(7, 7, m_clrToCheck);
		dc.SetPixel(7, 8, m_clrToCheck);
		dc.SetPixel(7, 9, m_clrToCheck);

		dc.SetPixel(8, 4, m_clrToCheck);
		dc.SetPixel(8, 5, m_clrToCheck);
		dc.SetPixel(8, 6, m_clrToCheck);
		dc.SetPixel(8, 7, m_clrToCheck);
		dc.SetPixel(8, 8, m_clrToCheck);
		dc.SetPixel(8, 9, m_clrToCheck);

		dc.SetPixel(9, 5, m_clrToCheck);
		dc.SetPixel(9, 6, m_clrToCheck);
		dc.SetPixel(9, 7, m_clrToCheck);
		dc.SetPixel(9, 8, m_clrToCheck);

		dc.SetPixel(10, 6, m_clrToCheck);
		dc.SetPixel(10, 7, m_clrToCheck);
		break;

	case LBLRESULT_CHECKING:
		// Draw arrow
		dc.SetPixel(3, 6, m_clrChecking);
		dc.SetPixel(3, 7, m_clrChecking);

		dc.SetPixel(4, 6, m_clrChecking);
		dc.SetPixel(4, 7, m_clrChecking);

		dc.SetPixel(5, 6, m_clrChecking);
		dc.SetPixel(5, 7, m_clrChecking);

		dc.SetPixel(6, 6, m_clrChecking);
		dc.SetPixel(6, 7, m_clrChecking);

		dc.SetPixel(7, 3, m_clrChecking);
		dc.SetPixel(7, 4, m_clrChecking);
		dc.SetPixel(7, 5, m_clrChecking);
		dc.SetPixel(7, 6, m_clrChecking);
		dc.SetPixel(7, 7, m_clrChecking);
		dc.SetPixel(7, 8, m_clrChecking);
		dc.SetPixel(7, 9, m_clrChecking);
		dc.SetPixel(7, 10, m_clrChecking);

		dc.SetPixel(8, 4, m_clrChecking);
		dc.SetPixel(8, 5, m_clrChecking);
		dc.SetPixel(8, 6, m_clrChecking);
		dc.SetPixel(8, 7, m_clrChecking);
		dc.SetPixel(8, 8, m_clrChecking);
		dc.SetPixel(8, 9, m_clrChecking);

		dc.SetPixel(9, 5, m_clrChecking);
		dc.SetPixel(9, 6, m_clrChecking);
		dc.SetPixel(9, 7, m_clrChecking);
		dc.SetPixel(9, 8, m_clrChecking);

		dc.SetPixel(10, 6, m_clrChecking);
		dc.SetPixel(10, 7, m_clrChecking);
		break;

	case LBLRESULT_SUCCEEDED:
		// Draw succeeded
		dc.SetPixel(3, 7, m_clrSucceeded);
		dc.SetPixel(3, 8, m_clrSucceeded);

		dc.SetPixel(4, 8, m_clrSucceeded);
		dc.SetPixel(4, 9, m_clrSucceeded);

		dc.SetPixel(5, 9, m_clrSucceeded);
		dc.SetPixel(5, 10, m_clrSucceeded);

		dc.SetPixel(6, 8, m_clrSucceeded);
		dc.SetPixel(6, 9, m_clrSucceeded);

		dc.SetPixel(7, 7, m_clrSucceeded);
		dc.SetPixel(7, 8, m_clrSucceeded);

		dc.SetPixel(8, 6, m_clrSucceeded);
		dc.SetPixel(8, 7, m_clrSucceeded);

		dc.SetPixel(9, 5, m_clrSucceeded);
		dc.SetPixel(9, 6, m_clrSucceeded);

		dc.SetPixel(10, 4, m_clrSucceeded);
		dc.SetPixel(10, 5, m_clrSucceeded);
		break;

	case LBLRESULT_NOTSUCCEEDED:
		// Draw cross
		//dc.SetPixel(3, 3, m_clrNotSucceeded);
		//dc.SetPixel(3, 10, m_clrNotSucceeded);

		dc.SetPixel(4, 3, m_clrNotSucceeded);
		dc.SetPixel(4, 4, m_clrNotSucceeded);
		dc.SetPixel(4, 9, m_clrNotSucceeded);
		dc.SetPixel(4, 10, m_clrNotSucceeded);

		dc.SetPixel(5, 3, m_clrNotSucceeded);
		dc.SetPixel(5, 4, m_clrNotSucceeded);
		dc.SetPixel(5, 5, m_clrNotSucceeded);
		dc.SetPixel(5, 8, m_clrNotSucceeded);
		dc.SetPixel(5, 9, m_clrNotSucceeded);
		dc.SetPixel(5, 10, m_clrNotSucceeded);

		dc.SetPixel(6, 4, m_clrNotSucceeded);
		dc.SetPixel(6, 5, m_clrNotSucceeded);
		dc.SetPixel(6, 6, m_clrNotSucceeded);
		dc.SetPixel(6, 7, m_clrNotSucceeded);
		dc.SetPixel(6, 8, m_clrNotSucceeded);
		dc.SetPixel(6, 9, m_clrNotSucceeded);

		dc.SetPixel(7, 4, m_clrNotSucceeded);
		dc.SetPixel(7, 5, m_clrNotSucceeded);
		dc.SetPixel(7, 6, m_clrNotSucceeded);
		dc.SetPixel(7, 7, m_clrNotSucceeded);
		dc.SetPixel(7, 8, m_clrNotSucceeded);
		dc.SetPixel(7, 9, m_clrNotSucceeded);

		dc.SetPixel(8, 3, m_clrNotSucceeded);
		dc.SetPixel(8, 4, m_clrNotSucceeded);
		dc.SetPixel(8, 5, m_clrNotSucceeded);
		dc.SetPixel(8, 8, m_clrNotSucceeded);
		dc.SetPixel(8, 9, m_clrNotSucceeded);
		dc.SetPixel(8, 10, m_clrNotSucceeded);

		dc.SetPixel(9, 3, m_clrNotSucceeded);
		dc.SetPixel(9, 4, m_clrNotSucceeded);
		dc.SetPixel(9, 9, m_clrNotSucceeded);
		dc.SetPixel(9, 10, m_clrNotSucceeded);

		//dc.SetPixel(10, 3, m_clrNotSucceeded);
		//dc.SetPixel(10, 10, m_clrNotSucceeded);
		break;

	default:
		// Don't draw anything
		bMoveText = false;
		break;
	}

	// Set transparency and rect
	dc.SetBkMode(TRANSPARENT); 

	// Copy text to buffer
	szBuffer = new TCHAR[sWindowText.GetLength() + 1];
	lstrcpy(szBuffer, sWindowText);

	// Select right font
	dc.SelectObject(pFont);

	// Should we move text?
	if (bMoveText)
	{
		// Move text 15 px to right
		rectText.left += 15;
	}

	// Draw text
	ScreenToClient(&rectText);
	dc.DrawText(szBuffer, -1, &rectText, DT_WORDBREAK | DT_END_ELLIPSIS);

	// Clean up objects
	delete[] szBuffer;
}
