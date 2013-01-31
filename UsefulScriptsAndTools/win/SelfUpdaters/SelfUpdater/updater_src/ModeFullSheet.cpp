/********************************************************************
	created:	2004/10/29
	created:	29:10:2004   17:14
	filename: 	ModeFullSheet.cpp
	file path:	Updater
	file base:	ModeFullSheet
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeFullSheet.h"
#include "AboutDlg.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeFullSheet, CPropertySheetEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_REGISTERED_MESSAGE(WM_HYPERLINK_CLICKED, OnLblUpdater)
	ON_MESSAGE(WMU_TASK_START, OnTaskStart)
	ON_MESSAGE(WMU_TASK_COMPLETE, OnTaskComplete)
	ON_MESSAGE(WMU_ERROR, OnError)
	ON_MESSAGE(WMU_WARNING, OnWarning)
	ON_MESSAGE(WMU_QUESTION, OnQuestion)
	ON_MESSAGE(WMU_UPDATE_GUI, OnUpdateGUI)
	ON_MESSAGE(WMU_UPDATE_GUI_FILEPROGRESS, OnUpdateGUIFileProgress)
	ON_MESSAGE(WMU_UPDATE_GUI_TOTALPROGRESS, OnUpdateGUITotalProgress)
	ON_MESSAGE(WMU_UPDATE_GUI_PROTECTION, OnUpdateGUIProtection)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(NM_CLICK, TITLEBARBTN_TOTRAY, OnBtnToTray)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeFullSheet, CPropertySheetEx)

//=====================================================================

CModeFullSheet::CModeFullSheet(CWinThread * pUpdateProcess, int iCurrentTask)
{
	// Set thread
	m_pUpdateProcess = pUpdateProcess;

	// We are not initialized yet
	m_bInit = false;

	// Init singleton calsses
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();
	m_pPath = CPath::Instance();

	// Set start task
	m_iStartTask = iCurrentTask;

	// Create hollow brush
	m_HollowBrush.CreateStockObject(HOLLOW_BRUSH);

	// Add pages to sheet
	AddPage(&m_pgWelcome);
	AddPage(&m_pgCheck);
	AddPage(&m_pgHtmlMessage);
	AddPage(&m_pgLicense);
	AddPage(&m_pgProtection);
	AddPage(&m_pgSelectUpdate);
	AddPage(&m_pgDownload);
	AddPage(&m_pgInstall);
	AddPage(&m_pgFinish);
	AddPage(&m_pgRollback);

	// Set to wizard mode
	SetWizardMode();

	// Set style to wizard 97
	m_psh.dwFlags |= PSH_WIZARD97 | PSH_HEADER | PSH_USEHBMHEADER |
					 PSH_WATERMARK | PSH_USEHBMWATERMARK;	

	// Load header
	HBITMAP hBmpHeader = (HBITMAP) ::LoadImage(NULL, m_pSettings->GetLogoSmall(),
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	
	// Get size
	BITMAPINFOHEADER bmpHeader;
	::GetObject(hBmpHeader, sizeof(BITMAPINFOHEADER), &bmpHeader);
	if ((bmpHeader.biHeight == LOGO_SMALL_HEIGHT) && (bmpHeader.biWidth == LOGO_SMALL_WIDTH))
	{
		// Use current loaded logo
		m_bmpHeader.Attach(hBmpHeader);
	}
	else
	{
		// Load default logo
		m_bmpHeader.LoadBitmap(IDB_LOGO_SMALL);
	}

	// Be sure to have no values for HBM
	m_psh.hbmWatermark = NULL;
	m_psh.hbmHeader = NULL;

	// This code is needed for correct drawings (??)
	m_psh.hInstance = AfxGetInstanceHandle();

	// Delete help button
	m_psh.dwFlags &= ~PSH_HASHELP;
	m_pgWelcome.m_psp.dwFlags &= ~PSP_HASHELP;
	m_pgCheck.m_psp.dwFlags &= ~PSP_HASHELP;
	m_pgHtmlMessage.m_psp.dwFlags &= ~PSP_HASHELP;
	m_pgLicense.m_psp.dwFlags &= ~PSP_HASHELP;
	m_pgProtection.m_psp.dwFlags &= ~PSP_HASHELP;
	m_pgSelectUpdate.m_psp.dwFlags &= ~PSP_HASHELP;
	m_pgDownload.m_psp.dwFlags &= ~PSP_HASHELP;
	m_pgInstall.m_psp.dwFlags &= ~PSP_HASHELP;
	m_pgFinish.m_psp.dwFlags &= ~PSP_HASHELP;
	m_pgRollback.m_psp.dwFlags &= ~PSP_HASHELP;
}

//=====================================================================

CModeFullSheet::~CModeFullSheet()
{
	// Delete bitmap object
	m_bmpHeader.DeleteObject();
	m_bmpMenuLogo.DeleteObject();
	m_bmpMenuLogoNegative.DeleteObject();
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

BOOL CModeFullSheet::PressButton(int nButton)
{
	// Check what action to perform
	switch (nButton)
	{
	case PSBTN_NEXT:
		// Check if install is complete
		if (m_bInstallReady)
		{
			// We are finished
			m_bFinished = true;
		}

		// If we are finished
		if (m_bFinished)
		{
			// Show finish page
			SetActivePage(&m_pgFinish);
			// Set buttons
			SetWizardButtons(PSWIZB_FINISH);
			// Disable cancel button
			CWnd * pWnd = this->GetDlgItem(IDCANCEL);
			pWnd->EnableWindow(FALSE);
		}
		else
		{
			// License and sections are an exception
			if (m_iCurrentTask == TASK_LICENSE || m_iCurrentTask == TASK_SELECTUPDATE)
			{
				// Send task complete message
				m_pUpdateProcess->PostThreadMessage(WMU_TASK_COMPLETE, m_iCurrentTask, 0);
			}
			else
			{
				// Start next task
				m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_STARTNEXTTASK, 0);
			}
		}
		break;

	case PSWIZB_FINISH:
		// Stop without asking
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);
		break;

	default:
		// Other buttons, handle default
		CPropertySheetEx::PressButton(nButton);
		break;
	}

	// Return successful call
	return TRUE;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

BOOL CModeFullSheet::OnInitDialog()
{
	// Call original function
	CPropertySheetEx::OnInitDialog();

	// Modify style so we can see an icon
	ModifyStyle(0, WS_SYSMENU | WS_MINIMIZEBOX);

	// Edit system menu
	CMenu * pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		// Add about
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, m_pLanguage->GetString(IDS_ABOUT_TITLE) + _T("..."));

		// Add bitmap to menu
		m_bmpMenuLogo.LoadBitmap(IDB_UPDATER);
		m_bmpMenuLogoNegative.LoadBitmap(IDB_UPDATER_NEG);
		pSysMenu->SetMenuItemBitmaps(IDM_ABOUTBOX, MF_BYCOMMAND, &m_bmpMenuLogo, &m_bmpMenuLogoNegative);

		// Delete maximize
		pSysMenu->DeleteMenu(SC_MAXIMIZE, MF_BYCOMMAND);

		// Delete change size
		pSysMenu->DeleteMenu(SC_SIZE, MF_BYCOMMAND);
	}

	// Load icon
	if (PathFileExists(m_pSettings->GetIcon()))
	{
		// Custom user icon
		HICON tmpIcon = (HICON)LoadImage(NULL, m_pSettings->GetIcon(), IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);

		// Check if we loaded successfully
		if (tmpIcon)
		{
			SetIcon(tmpIcon, TRUE);
		}
		else
		{
			// Default Updater icon
			SetIcon(AfxGetApp()->LoadIcon(IDI_UPDATER), TRUE);
		}
	}
	else
	{
		// Default Updater icon
		SetIcon(AfxGetApp()->LoadIcon(IDI_UPDATER), TRUE);
	}
	
	// We are not finished yet
	m_bFinished = false;
	m_bInstallReady = false;

	// Set window title
	CString sTitle;
	sTitle.Format(m_pLanguage->GetString(IDS_GENERAL_TITLE), m_pSettings->GetAppName());
	SetTitle(sTitle);

	// Delete back-button because it's not used
	CWnd * pBack = this->GetDlgItem(ID_WIZBACK);
	pBack->ShowWindow(SW_HIDE);
	pBack->EnableWindow(FALSE);

	// Set text for buttons
	CWnd * pNext = this->GetDlgItem(ID_WIZNEXT);
	pNext->SetWindowText(m_pLanguage->GetString(IDS_GENERAL_NEXT));

	CWnd * pFinish = this->GetDlgItem(ID_WIZFINISH);
	pFinish->SetWindowText(m_pLanguage->GetString(IDS_GENERAL_FINISH));

	CWnd * pCancel = this->GetDlgItem(IDCANCEL);
	pCancel->SetWindowText(m_pLanguage->GetString(IDS_GENERAL_CANCEL));

	// Create updater static
	CRect rect;
	int iHeight = 15;
	int iWidth = 50;
	int iOffset = 10;
	GetClientRect(&rect);

	// Find a bottom-left point for the edit control in the client area
	int iX = rect.left + iOffset;
	int iY = rect.top + (rect.Height() - iHeight) - 23;

	// Finally, create the static controls
	rect.left = iX;
	rect.right = iX + iWidth;
	rect.top = iY + 8;
	rect.bottom = iY + iHeight + 8;
	m_lblUpdater.CreateEx(WS_EX_LEFT, _T("STATIC"), NULL,
					WS_CHILD | WS_VISIBLE | SS_NOTIFY, rect, this, lblUpdater, 0);

	// Set hyperlink for the label, but handle click ourself
	m_lblUpdater.SetHyperlink(_T("-"));
	m_lblUpdater.SetClickHandling(false);
	m_lblUpdater.SetAutoResize(true);
	m_lblUpdater.SetColorLink(m_pSettings->GetLinkColorLink());
	m_lblUpdater.SetColorHover(m_pSettings->GetLinkColorHover());

	// Set text for static controls
	m_lblUpdater.SetWindowText(_T("Updater"));

	// Set Updater font
	m_fntUpdater.DeleteObject();
	m_fntUpdater.CreateFont(
		13,								// nHeight
		0,								// nWidth
		0,								// nEscapement
		0,								// nOrientation
		0,								// nWeight
		FALSE,							// bItalic
		FALSE,							// bUnderline
		0,								// cStrikeOut
		DEFAULT_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,				// nOutPrecision
		CLIP_DEFAULT_PRECIS,			// nClipPrecision
		PROOF_QUALITY,					// nQuality
		DEFAULT_PITCH | FF_DONTCARE,	// nPitchAndFamily
		_T("Verdana"));
	m_lblUpdater.SetFont(&m_fntUpdater);

#ifdef _DEBUG
	// Create special title bar buttons
	CreateTitleBarButtons();
#endif

	// Set focus to current window
	ShowWindow(SW_NORMAL);
	::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// Set right page
	OnTaskStart(m_iStartTask, 0);

	// Return true
	return TRUE;
}

//=====================================================================

BOOL CModeFullSheet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// If user presses cancel
	if (LOWORD(wParam) == IDCANCEL)
	{
		// Handle close updater
		CloseUpdater();
		return TRUE;
	}

	// Always call the original function
	return CPropertySheetEx::OnCommand(wParam, lParam);
}

//=====================================================================

void CModeFullSheet::OnSysCommand(UINT nID, LPARAM lParam)
{
	// If user wants to see about box
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		// Show about dialog
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}

	// If someone is closing the app
	if (nID == 61536)
	{
		// Handle close updater		
		CloseUpdater();
		return;
	}

	// Call original function
	CPropertySheetEx::OnSysCommand(nID, lParam);
}

//=====================================================================

void CModeFullSheet::DisableCancel()
{
	// Disable cancel button
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

	// Only disable if finish button is not visible, else enable
	if (GetDlgItem(ID_WIZFINISH)->IsWindowVisible())
	{
		// Enable cross of system menu
		CMenu * pMenu = GetSystemMenu(FALSE);
		HMENU hMenu = pMenu->m_hMenu;
		EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
	}
	else
	{
		// Disable cross of system menu
		CMenu * pMenu = GetSystemMenu(FALSE);
		HMENU hMenu = pMenu->m_hMenu;
		EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	}
}

//=====================================================================

bool CModeFullSheet::CloseUpdater()
{
	// Are we finished?
	if (!m_bFinished)
	{
		// Is user allowed to exit?
		if (m_pUpdateInfo->GetForceUpdate())
		{
			// No
			return false;
		}

		// Yes, set up confirmation text
		CString sTemp;
		sTemp.Format(m_pLanguage->GetString(IDS_CONFIRMATION_STOPUPDATING_EXPLANATION),
			m_pSettings->GetAppName(), m_pSettings->GetAppName(), m_pSettings->GetAppName());

		// Ask user for confirmation
		CConfirmationDlg dlgConfirm(m_pLanguage->GetString(IDS_CONFIRMATION_STOPUPDATING_TITLE),
			m_pLanguage->GetString(IDS_CONFIRMATION_TITLE), sTemp,
			m_pLanguage->GetString(IDS_GENERAL_YES), m_pLanguage->GetString(IDS_GENERAL_NO));
		if (dlgConfirm.DoModal() == IDYES)
		{
			// Cancel the update process
			m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_CANCEL, 0);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		// Close application
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);
		return false;
	}
}

//=====================================================================

void CModeFullSheet::CreateTitleBarButtons()
{
	// Declare variables
	CBitmap bmpToTray, bmpMinimize, bmpMaximize, bmpClose;

	// Create special "to-tray" button
	BYTE arrToTray[] = {
		0xFF,0xE0,
		0xFF,0xE0,
		0xFF,0xE0,
		0xFF,0xE0,
		0xF3,0xA0,
		0xF1,0x20,
		0xF8,0x20,
		0xFC,0x20,
		0xF8,0x20,
		0xF0,0x20,
		0xFF,0xE0
	};
	bmpToTray.CreateBitmap(11, 11, 1, 1, arrToTray);

	// Create caption frame with the buttons
	m_ctrlCaptionFrame.CreateCaptionFrame(this, IDI_UPDATER);
	m_ctrlCaptionFrame.AddCaptionButton(CRect(0, 0, 0, 0), TITLEBARBTN_TOTRAY, CBNBKGNDSTYLE_CAPTION, FALSE);

	// Set styles
	CCustomBitmapButton * pToTray = m_ctrlCaptionFrame.GetCaptionButtonPtr(TITLEBARBTN_TOTRAY);
	if (pToTray)
	{
		pToTray->SetGlyphBitmap(bmpToTray);
	}
}

//=====================================================================

HBRUSH CModeFullSheet::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	// Declare variables
	CPropertyPageEx * pPage = NULL;

	// Call original function
	HBRUSH hBrush = CPropertySheetEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// First check if we have pages left
	int iPages = CPropertySheetEx::GetPageCount();
	if (iPages == 0)
	{
		// No, return the brush
		return hBrush;
	}

	// Get current page
	pPage = (CPropertyPageEx *) GetActivePage();
	
	if (NULL != pPage)
	{
		// Check if we should show header or not
		if ((pPage->m_psp.dwFlags & PSP_HIDEHEADER) == 0)
		{
			// If the page has a header, we need to paint the area above the border.
			// By inspection (Spy++), the border is a static control with ID 0x3027
			CWnd * pTopBorder = GetDlgItem(0x3027);

			// Did we retrieve a valid border?
			if (NULL != pTopBorder)
			{
				// Get the border rect
				CRect rectTopBorder;
				pTopBorder->GetWindowRect(rectTopBorder);
				ScreenToClient(rectTopBorder);
	
				// Get current rect of OnCtlColor function
				CRect rectCtrl;
				pWnd->GetWindowRect(rectCtrl);
				ScreenToClient(rectCtrl);

				// If right
				if (rectCtrl.top < rectTopBorder.bottom)
				{
					switch(nCtlColor)
					{
						case CTLCOLOR_STATIC:
							// The Slider Control has CTLCOLOR_STATIC, but doesn't let
							// the background shine through,
							TCHAR lpszClassName[255];
							GetClassName(pWnd->m_hWnd, lpszClassName, 255);
							if (_tcscmp(lpszClassName, TRACKBAR_CLASS) == 0)
							{
								return CPropertySheetEx::OnCtlColor(pDC, pWnd, nCtlColor);
							}

						case CTLCOLOR_BTN:
							// let static controls shine through
							pDC->SetBkMode(TRANSPARENT);
							return HBRUSH(m_HollowBrush);
					}
				}
			}
		}
	}

	// Return result of original function
	return hBrush;
}

//=====================================================================

LRESULT CModeFullSheet::OnLblUpdater(WPARAM wParam, LPARAM)
{
	// Show about box
	m_pUpdateProcess->PostThreadMessage(WMU_SHOWABOUT, 0, 0);

	return 0;
}

//=====================================================================

LRESULT CModeFullSheet::OnTaskStart(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sTemp;

	// Get data
	m_iCurrentTask = wParam;

	// Check what task is started
	switch (m_iCurrentTask)
	{
	case TASK_INIT:
		// Set active page
		SetActivePage(&m_pgWelcome);
		break;

	case TASK_CHECKVERSION:
		// Set active page
		SetActivePage(&m_pgCheck);
		// Disable next button
		SetWizardButtons(NULL);
		break;

	case TASK_HTMLMESSAGE:
		// Set active page
		SetActivePage(&m_pgHtmlMessage);
		// Disable next button
		SetWizardButtons(NULL);
		break;

	case TASK_LICENSE:
		SetActivePage(&m_pgLicense);
		// Disable next button
		SetWizardButtons(NULL);
		break;

	case TASK_PROTECTION:
		SetActivePage(&m_pgProtection);
		// Disable next button
		SetWizardButtons(NULL);
		break;

	case TASK_SELECTUPDATE:
		SetActivePage(&m_pgSelectUpdate);
		// Enable next button
		SetWizardButtons(PSWIZB_NEXT);
		break;

	case TASK_DOWNLOAD:
		SetActivePage(&m_pgDownload);
		// Disable next button
		SetWizardButtons(NULL);
		break;
		
	case TASK_INSTALL:
		SetActivePage(&m_pgInstall);
		// Disable next button
		SetWizardButtons(NULL);
		break;

	case TASK_ROLLBACK:
		// Remove all pages
		for (int i = GetPageCount() - 1; i >= 0; i--)
		{
			// If page is not rollback, remove it
			if (GetPage(i) != &m_pgRollback)
			{
				// Remove it
				RemovePage(i);
			}
		}

		// Set active page
		SetActivePage(&m_pgRollback);
		// Enable no buttons
		SetWizardButtons(NULL);
		// Disable even cancel button
		CWnd * pWnd = this->GetDlgItem(IDCANCEL);
		pWnd->EnableWindow(FALSE);
		break;
	}

	// Check if cancel button should be disabled
	if (m_pUpdateInfo->GetForceUpdate())
	{
		// Disable cancel button
		DisableCancel();
	}

	return S_OK;
}

//=====================================================================

LRESULT CModeFullSheet::OnTaskComplete(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sTemp;

	// Get data
	int iTask = wParam;

	// Check what task is completed
	switch (iTask)
	{
	case TASK_INIT:
		// Enable next button
		SetWizardButtons(PSWIZB_NEXT);
		break;

	case TASK_CHECKVERSION:
		// Set result
		m_pgCheck.SetVersionResult(true);
		// Enable next button
		SetWizardButtons(PSWIZB_NEXT);
		break;

	case TASK_HTMLMESSAGE:
		// This is a little different, buttons are already handled
		break;

	case TASK_LICENSE:
		// This is a little different, buttons are already handled
		break;

	case TASK_PROTECTION:
		// Enable next button
		SetWizardButtons(PSWIZB_NEXT);
		break;

	case TASK_SELECTUPDATE:
		// This is a little different, buttons are already handled
		break;

	case TASK_DOWNLOAD:
		// Enable next button
		SetWizardButtons(PSWIZB_NEXT);
		break;

	case TASK_INSTALL:
		// Enable next button
		SetWizardButtons(PSWIZB_NEXT);
		m_bInstallReady = true;
		break;

	case TASK_ROLLBACK:
		// Enable finish button
		SetWizardButtons(PSWIZB_FINISH);
		m_bFinished = true;
		break;
	}

	// Check if cancel button should be disabled
	if (m_pUpdateInfo->GetForceUpdate())
	{
		// Disable cancel button
		DisableCancel();
	}

	return S_OK;
}

//=====================================================================

LRESULT CModeFullSheet::OnError(WPARAM wParam, LPARAM lParam)
{
	// Get data
	int iError = wParam;
	CString sTitle = _T(""), sExplanation = _T(""), sMore = _T(""), sTemp;
	CWnd * pWnd;
	bool bRollback = true;
	bool bCloseApp = false;

	// Check what error has occured
	switch (iError)
	{
	case ERROR_UNKNOWN:
		sTitle = m_pLanguage->GetString(IDS_ERROR_UNKNOWN_TITLE);
		sExplanation = m_pLanguage->GetString(IDS_ERROR_UNKNOWN_EXPLANATION);
		sMore = _T("");
		break;

	case ERROR_NOCONNECTION:
		sTitle = m_pLanguage->GetString(IDS_ERROR_NOCONNECTION_TITLE);
		sExplanation = m_pLanguage->GetString(IDS_ERROR_NOCONNECTION_EXPLANATION);
		sMore = m_pLanguage->GetString(IDS_ERROR_NOCONNECTION_MORE);
		break;

	case ERROR_SERVER:
		sTitle = m_pLanguage->GetString(IDS_ERROR_SERVER_TITLE);
		sExplanation = m_pLanguage->GetString(IDS_ERROR_SERVER_EXPLANATION);
		sMore = m_pLanguage->GetString(IDS_ERROR_SERVER_MORE);
		bCloseApp = true;
		bRollback = false;
		break;

	case ERROR_NODISKSPACE:
		sTitle = m_pLanguage->GetString(IDS_ERROR_NODISKSPACE_TITLE);
		sExplanation.Format(m_pLanguage->GetString(IDS_ERROR_NODISKSPACE_EXPLANATION), (lParam / 1024 / 1024));
		sMore = m_pLanguage->GetString(IDS_ERROR_NODISKSPACE_MORE);
		break;

	case ERROR_FILENOTFOUND:
		sTitle = m_pLanguage->GetString(IDS_ERROR_FILENOTFOUND_TITLE);
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sExplanation.Format(m_pLanguage->GetString(IDS_ERROR_FILENOTFOUND_EXPLANATION), sTemp);
		sMore.Format(m_pLanguage->GetString(IDS_ERROR_FILENOTFOUND_MORE), m_pSettings->GetAppName());
		break;

	case ERROR_COPYFILE:
		sTitle = m_pLanguage->GetString(IDS_ERROR_COPYFILE_TITLE);
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sExplanation.Format(m_pLanguage->GetString(IDS_ERROR_COPYFILE_EXPLANATION), sTemp);
		sMore = m_pLanguage->GetString(IDS_ERROR_COPYFILE_MORE);
		break;

	case ERROR_DELETEFILE:
		sTitle = m_pLanguage->GetString(IDS_ERROR_DELETEFILE_TITLE);
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sExplanation.Format(m_pLanguage->GetString(IDS_ERROR_DELETEFILE_EXPLANATION), sTemp);
		sMore = m_pLanguage->GetString(IDS_ERROR_DELETEFILE_MORE);
		break;

	case ERROR_RUNFILE:
		sTitle = m_pLanguage->GetString(IDS_ERROR_RUNFILE_TITLE);
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sExplanation.Format(m_pLanguage->GetString(IDS_ERROR_RUNFILE_EXPLANATION), sTemp);
		sMore = m_pLanguage->GetString(IDS_ERROR_RUNFILE_MORE);
		break;

	case ERROR_CLOSEAPP:
		sTitle = m_pLanguage->GetString(IDS_ERROR_CLOSEAPP_TITLE);
		sExplanation.Format(m_pLanguage->GetString(IDS_ERROR_CLOSEAPP_EXPLANATION), lParam);
		sMore.Format(m_pLanguage->GetString(IDS_ERROR_CLOSEAPP_MORE), lParam);
		break;

	case ERROR_FILECORRUPT:
		sTitle = m_pLanguage->GetString(IDS_ERROR_FILECORRUPT_TITLE);
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sExplanation.Format(m_pLanguage->GetString(IDS_ERROR_FILECORRUPT_EXPLANATION), sTemp);
		sMore.Format(m_pLanguage->GetString(IDS_ERROR_FILECORRUPT_MORE), m_pSettings->GetAppName());
		break;

	case ERROR_NONEWVERSION:
		// Don't show error, change status text
		m_pgCheck.SetVersionResult(false);
		// Enable finish button
		SetWizardButtons(PSWIZB_FINISH);
		// Disable cancel button
		pWnd = this->GetDlgItem(IDCANCEL);
		pWnd->EnableWindow(FALSE);
		// Stop the function
		return S_OK;

	case ERROR_WRITEFILE:
		sTitle = m_pLanguage->GetString(IDS_ERROR_WRITEFILE_TITLE);
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sExplanation.Format(m_pLanguage->GetString(IDS_ERROR_WRITEFILE_EXPLANATION), sTemp);
		sMore = m_pLanguage->GetString(IDS_ERROR_WRITEFILE_MORE);
		break;

	case ERROR_UNZIPFILE:
		sTitle = m_pLanguage->GetString(IDS_ERROR_UNZIPFILE_TITLE);
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sExplanation.Format(m_pLanguage->GetString(IDS_ERROR_UNZIPFILE_EXPLANATION), sTemp);
		sMore.Format(m_pLanguage->GetString(IDS_ERROR_UNZIPFILE_MORE), m_pSettings->GetAppName());
		break;

	case ERROR_PROTECTION:
		// Don't roll back
		bRollback = false;

		// Enable finish button
		SetWizardButtons(PSWIZB_FINISH);
		// Disable cancel button
		pWnd = this->GetDlgItem(IDCANCEL);
		pWnd->EnableWindow(FALSE);

		// Set up error message
		sTitle = m_pLanguage->GetString(IDS_ERROR_PROTECTION_TITLE);
		sExplanation.Format(m_pLanguage->GetString(IDS_ERROR_PROTECTION_EXPLANATION), m_pSettings->GetAppName());
		sMore.Format(m_pLanguage->GetString(IDS_ERROR_PROTECTION_MORE), m_pSettings->GetAppName());
		break;
	}

	// Show error to user
	CErrorDlg dlgError(sTitle, m_pLanguage->GetString(IDS_ERROR_TITLE), sExplanation,
		sMore, m_pLanguage->GetString(IDS_GENERAL_OK), m_pLanguage->GetString(IDS_ERROR_MORE));
	dlgError.DoModal();

	if (bRollback)
	{
		// Start next task, which will be rollback
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_STARTTASK, TASK_ROLLBACK);
	}

	if (bCloseApp)
	{
		// Close Updater
		m_pUpdateProcess->PostThreadMessage(WM_QUIT, 0, 0);
	}

	return S_OK;
}

//=====================================================================

LRESULT CModeFullSheet::OnWarning(WPARAM wParam, LPARAM lParam)
{
	// Get data
	int iWarning = wParam;
	CString sTitle = _T(""), sExplanation = _T(""), sTemp;

	// Check what error has occured
	switch (iWarning)
	{
	case WARNING_REGISTER:
		sTitle = m_pLanguage->GetString(IDS_WARNING_REGISTER_TITLE);
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sExplanation.Format(m_pLanguage->GetString(IDS_WARNING_REGISTER_EXPLANATION), sTemp, m_pSettings->GetAppName());
		break;
	}

	// Show warning to user
	CWarningDlg dlgWarning(sTitle, m_pLanguage->GetString(IDS_WARNING_TITLE), sExplanation,
		m_pLanguage->GetString(IDS_GENERAL_OK));
	dlgWarning.DoModal();

	// Continue
	m_pUpdateProcess->PostThreadMessage(WMU_WARNING_COMPLETE, 0, 0);

	return S_OK;
}

//=====================================================================

LRESULT CModeFullSheet::OnQuestion(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int iQuestion = wParam;
	int iResult;
	CString sTemp;
	CConfirmationDlg * pConfirm = NULL;

	// Check what type of question
	switch (iQuestion)
	{
	case QUESTION_STOPUPDATING:
		// Ask user
		break;

	case QUESTION_DELETEFILE:
		// Set up question
		sTemp.Format(m_pLanguage->GetString(IDS_CONFIRMATION_DELETEFILE_EXPLANATION),
			((CActionDelete *)lParam)->GetDeleteLocation(), m_pSettings->GetAppName(), 
			((CActionDelete *)lParam)->GetDeleteLocation());

		// Ask user for confirmation
		pConfirm = new CConfirmationDlg(m_pLanguage->GetString(IDS_CONFIRMATION_DELETEFILE_TITLE),
			m_pLanguage->GetString(IDS_CONFIRMATION_TITLE), sTemp,
			m_pLanguage->GetString(IDS_GENERAL_YES), m_pLanguage->GetString(IDS_GENERAL_NO));
		if (pConfirm->DoModal() == IDNO)
		{
			iResult = QUESTIONRESULT_NO;
		}
		else
		{
			iResult = QUESTIONRESULT_YES;
		}
		break;

	case QUESTION_CLOSEAPPLICATION:
		// Set up confirmation text
		if (m_pUpdateInfo->GetCloseApplication() != CLOSE_USER)
		{
			sTemp.Format(m_pLanguage->GetString(IDS_CONFIRMATION_CLOSEAPPLICATION_EXPLANATION),
				lParam, m_pSettings->GetAppName(), lParam);					
		}
		else
		{
			sTemp.Format(m_pLanguage->GetString(IDS_CONFIRMATION_CLOSEAPPLICATION_USER_EXPLANATION),
				lParam, m_pSettings->GetAppName(), lParam);					
		}

		// Ask confirmation
		pConfirm = new CConfirmationDlg(m_pLanguage->GetString(IDS_CONFIRMATION_CLOSEAPPLICATION_TITLE),
			m_pLanguage->GetString(IDS_CONFIRMATION_TITLE), sTemp,
			m_pLanguage->GetString(IDS_GENERAL_YES), m_pLanguage->GetString(IDS_GENERAL_NO));
		if (pConfirm->DoModal() == IDNO)
		{
			iResult = QUESTIONRESULT_NO;
		}
		else
		{
			iResult = QUESTIONRESULT_YES;
			Sleep(200);
		}
		break;

	case QUESTION_REGISTERFILE:
		// Set up question
		sTemp.Format(m_pLanguage->GetString(IDS_CONFIRMATION_REGISTER_EXPLANATION),
			m_pPath->ExtractFileName(((CActionRegister *)lParam)->GetLocation()), m_pSettings->GetAppName(),
			m_pPath->ExtractFileName(((CActionRegister *)lParam)->GetLocation()));

		// Ask user for confirmation
		pConfirm = new CConfirmationDlg(m_pLanguage->GetString(IDS_CONFIRMATION_REGISTER_TITLE),
			m_pLanguage->GetString(IDS_CONFIRMATION_TITLE), sTemp,
			m_pLanguage->GetString(IDS_GENERAL_YES), m_pLanguage->GetString(IDS_GENERAL_NO));
		if (pConfirm->DoModal() == IDNO)
		{
			iResult = QUESTIONRESULT_NO;
		}
		else
		{
			iResult = QUESTIONRESULT_YES;
		}
		break;
	}

	// Clean up object
	if (pConfirm != NULL)
		delete pConfirm;

	// Return answer
	m_pUpdateProcess->PostThreadMessage(WMU_QUESTION_ANSWER, iQuestion, iResult);

	return S_OK;
}

//=====================================================================

LRESULT CModeFullSheet::OnUpdateGUI(WPARAM wParam, LPARAM lParam)
{
	// Update complete GUI

	return S_OK;
}

//=====================================================================

LRESULT CModeFullSheet::OnUpdateGUIFileProgress(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CPropertyPageEx * pPage = (CPropertyPageEx *) GetActivePage();

	// Update file progress bar
	if (pPage)
		pPage->PostMessage(WMU_UPDATE_GUI_FILEPROGRESS, wParam, lParam);

	return S_OK;
}

//=====================================================================

LRESULT CModeFullSheet::OnUpdateGUITotalProgress(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CPropertyPageEx * pPage = (CPropertyPageEx *) GetActivePage();

	// Update total progress bar
	if (pPage)
		pPage->PostMessage(WMU_UPDATE_GUI_TOTALPROGRESS, wParam, lParam);

	return S_OK;
}

//=====================================================================

LRESULT CModeFullSheet::OnUpdateGUIProtection(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CPropertyPageEx * pPage = (CPropertyPageEx *) GetActivePage();

	// Update total progress bar
	if (pPage)
		pPage->PostMessage(WMU_UPDATE_GUI_PROTECTION, wParam, lParam);

	return S_OK;
}

//=====================================================================

BOOL CModeFullSheet::OnEraseBkgnd(CDC * pDC)
{
	// Call original function
	BOOL bResult = CPropertySheetEx::OnEraseBkgnd(pDC);

	// Get current page
	CPropertyPageEx * pPage = (CPropertyPageEx *) GetActivePage();
	if (NULL != pPage)
	{
		// Check if we should show header or not
		if ((pPage->m_psp.dwFlags & PSP_HIDEHEADER) == 0)
		{
			// If the page has a header, we need to paint the area above the border.
			// By inspection (Spy++), the border is a static control with ID 0x3027
			CWnd * pTopBorder = GetDlgItem(0x3027);

			// Did we retrieve a valid border?
			if (NULL != pTopBorder)
			{
				// Get the border rect
				CRect rectTopBorder;
				pTopBorder->GetWindowRect(rectTopBorder);

				CRect rc;
				GetClientRect(rc);
				ScreenToClient(rectTopBorder);
				rc.bottom = rectTopBorder.top - 1;

				// Get paint field
			    CDC dcMem;
			    dcMem.CreateCompatibleDC(pDC);				
				CBitmap * pOldBitmap = dcMem.SelectObject(&m_bmpHeader);
				
				// Draw bitmap
				pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);

				// Select original object
				dcMem.SelectObject(pOldBitmap);  
				dcMem.DeleteDC();
			}
		}
	}

	// Return result
	return bResult;
}

//=====================================================================

void CModeFullSheet::OnBtnToTray(NMHDR * pNotifyStruct, LRESULT * result)
{
	// Declare variables
	TCHAR * pMode = new TCHAR[MODE_SILENT.GetLength()];
	lstrcpy(pMode, MODE_SILENT);

	// Switch view to silent mode
	m_pUpdateProcess->PostThreadMessage(WMU_SWITCHUI, (WPARAM)pMode, 0);
}
