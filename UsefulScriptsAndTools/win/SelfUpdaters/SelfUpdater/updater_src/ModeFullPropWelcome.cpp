/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:15
	filename: 	ModeFullPropWelcome.cpp
	file path:	Updater
	file base:	ModeFullPropWelcome
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeFullPropWelcome.h"
#include "ModeFullSheet.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeFullPropWelcome, CPropertyPageEx)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeFullPropWelcome, CPropertyPageEx)

//=====================================================================

CModeFullPropWelcome::CModeFullPropWelcome()
	: CPropertyPageEx(CModeFullPropWelcome::IDD)
{
	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();

	// Create hollow brush
	m_oHollowBrush.CreateStockObject(HOLLOW_BRUSH);

	// Hide header because this is the first page
	m_psp.dwFlags |= PSP_DEFAULT | PSP_USETITLE | PSP_HIDEHEADER;

	// Load watermark
	HBITMAP hBmpWatermark = (HBITMAP) ::LoadImage(NULL, m_pSettings->GetLogoLarge(),
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// Get size
	BITMAPINFOHEADER bmpWatermark;
	::GetObject(hBmpWatermark, sizeof(BITMAPINFOHEADER), &bmpWatermark);
	if ((bmpWatermark.biHeight == LOGO_LARGE_HEIGHT) && (bmpWatermark.biWidth == LOGO_LARGE_WIDTH))
	{
		// Use current loaded logo
		m_bmpWatermark.Attach(hBmpWatermark);
	}
	else
	{
		// Load default logo
		m_bmpWatermark.LoadBitmap(IDB_LOGO_LARGE);
	}
}

//=====================================================================

CModeFullPropWelcome::~CModeFullPropWelcome()
{
	// Delete brushes
	m_oHollowBrush.DeleteObject();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************


//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CModeFullPropWelcome::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	DDX_Control(pDX, lblTitle, m_lblTitle);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
}

//=====================================================================

BOOL CModeFullPropWelcome::OnInitDialog()
{
	// Call original function
	CPropertyPageEx::OnInitDialog();

	// Set title
	m_lblTitle.SetWindowText(m_pLanguage->GetString(IDS_FULL_WELCOME_TITLE));

	// Set font for title
	m_fntTitle.DeleteObject();
	m_fntTitle.CreateFont(
		19,								// nHeight
		0,								// nWidth
		0,								// nEscapement
		0,								// nOrientation
		FW_BOLD,						// nWeight
		FALSE,							// bItalic
		FALSE,							// bUnderline
		0,								// cStrikeOut
		ANSI_CHARSET,					// nCharSet
		OUT_DEFAULT_PRECIS,				// nOutPrecision
		CLIP_DEFAULT_PRECIS,			// nClipPrecision
		PROOF_QUALITY,					// nQuality
		DEFAULT_PITCH | FF_DONTCARE,	// nPitchAndFamily
		_T("Verdana Bold"));
	m_lblTitle.SetFont(&m_fntTitle);

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropWelcome::OnSetActive()
{
	// Call original function
	CPropertyPageEx::OnSetActive();

	// Load language strings
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_WELCOME_EXPLANATION), m_pSettings->GetAppName());
	m_lblExplanation.SetWindowText(sTemp);

	// Return value
	return TRUE;
}

//=====================================================================

LRESULT CModeFullPropWelcome::OnWizardNext()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();
	
	// Start next task
	pParent->PressButton(PSBTN_NEXT);

	// Don't change page yourself
	return -1;
}

//=====================================================================

HBRUSH CModeFullPropWelcome::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
		case CTLCOLOR_STATIC:
			// The Slider Control has CTLCOLOR_STATIC, but doesn't let
			// the background shine through,
			TCHAR lpszClassName[255];
			GetClassName(pWnd->m_hWnd, lpszClassName, 255);
			if(_tcscmp(lpszClassName, TRACKBAR_CLASS) == 0)
			{
				return CPropertyPageEx::OnCtlColor(pDC, pWnd, nCtlColor);
			}

			// Let static controls shine through
			pDC->SetBkMode(TRANSPARENT);
			return HBRUSH(m_oHollowBrush);

		default:
			break;
	}

	// If we reach this line, we haven't set a brush so far
	return CPropertyPageEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

//=====================================================================

BOOL CModeFullPropWelcome::OnEraseBkgnd(CDC * pDC)
{
	// Declare variables
	CRect rectBitmap;
	COLORREF clrWhite = 0x00FFFFFF;

	// Get client rect
	GetClientRect(rectBitmap);

	// Background color should be white
	pDC->FillSolidRect(rectBitmap, clrWhite);

	// Get paint field
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);				
	CBitmap * pOldBitmap = dcMem.SelectObject(&m_bmpWatermark);

	// Draw bitmap
	pDC->BitBlt(0, 0, LOGO_LARGE_WIDTH, LOGO_LARGE_HEIGHT, &dcMem, 0, 0, SRCCOPY);

	// Select original object
	dcMem.SelectObject(pOldBitmap);  
	dcMem.DeleteDC();

	return TRUE;
}