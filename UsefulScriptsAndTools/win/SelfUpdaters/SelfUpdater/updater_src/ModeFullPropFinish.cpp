/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:19
	filename: 	ModeFullPropFinish.cpp
	file path:	Updater
	file base:	ModeFullPropFinish
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeFullPropFinish.h"
#include "ModeFullSheet.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeFullPropFinish, CPropertyPageEx)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeFullPropFinish, CPropertyPageEx)

//=====================================================================

CModeFullPropFinish::CModeFullPropFinish()
	: CPropertyPageEx(CModeFullPropFinish::IDD)
{
	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();

	// Set title
	m_strHeaderTitle = m_pLanguage->GetString(IDS_FULL_FINISH_TITLE);
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_FINISH_SUBTITLE),
				 m_pSettings->GetAppName());
	m_strHeaderSubTitle = sTemp;

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

CModeFullPropFinish::~CModeFullPropFinish()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CModeFullPropFinish::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
	DDX_Control(pDX, chkCreateShortcuts, m_chkCreateShortcuts);
}

//=====================================================================

BOOL CModeFullPropFinish::OnInitDialog()
{
	// Call original function
	CPropertyPageEx::OnInitDialog();

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropFinish::OnSetActive()
{
	// Call original function
	CPropertyPageEx::OnSetActive();
	
	// Load language strings
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_FINISH_EXPLANATION), m_pSettings->GetAppName(),
		m_pSettings->GetAppName());
	m_lblExplanation.SetWindowText(sTemp);

	sTemp.Format(m_pLanguage->GetString(IDS_FULL_FINISH_CREATESHORTCUTS), m_pSettings->GetAppName());
	m_chkCreateShortcuts.SetWindowText(sTemp);

	// Check if we should hide the create shortcut checkbox
	if ((m_pUpdateInfo->GetShortcutsAskUser() == true) && (m_pUpdateInfo->GetShortcutCount() > 0))
		m_chkCreateShortcuts.ShowWindow(SW_SHOW);
	else
		m_chkCreateShortcuts.ShowWindow(SW_HIDE);

	// Always check item
	m_chkCreateShortcuts.SetCheck(1);

	// Return value
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropFinish::OnWizardFinish()
{
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();
	
	// Check if we should create shortcut
	if (m_chkCreateShortcuts.GetCheck() == 0)
	{
		// No
		m_pUpdateInfo->SetShortcutsCreateShortcuts(false);
	}
	else
	{
		// Yes
		m_pUpdateInfo->SetShortcutsCreateShortcuts(true);
	}

	// Start next task
	pParent->PressButton(PSWIZB_FINISH);
	
	// Don't change page yourself
	return -1;
}

//=====================================================================

HBRUSH CModeFullPropFinish::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	// Be sure to draw the checkbox (shortcuts) right
	if (pWnd->GetDlgCtrlID() == chkCreateShortcuts)
	{
		HBRUSH hBr = CreateSolidBrush(RGB(255, 255, 255));
		pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkMode(TRANSPARENT);
		return hBr;
	}	

	// Be sure to draw the checkbox (userdata) right
	if (pWnd->GetDlgCtrlID() == chkUserData)
	{
		HBRUSH hBr = CreateSolidBrush(RGB(255, 255, 255));
		pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkMode(TRANSPARENT);
		return hBr;
	}	

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

BOOL CModeFullPropFinish::OnEraseBkgnd(CDC * pDC)
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