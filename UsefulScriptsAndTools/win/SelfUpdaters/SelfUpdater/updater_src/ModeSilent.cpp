/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   15:13
	filename: 	ModeSilent.cpp
	file path:	Updater
	file base:	ModeSilent
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeSilent.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeSilent, CWnd)
	ON_COMMAND(ID_SILENT_POPUP_ABOUT, OnPopupAbout)
	ON_COMMAND(ID_SILENT_POPUP_EXIT, OnPopupExit)
	ON_COMMAND(ID_SILENT_POPUP_CUSTOM1, OnPopupCustom1)
	ON_COMMAND(ID_SILENT_POPUP_CUSTOM2, OnPopupCustom2)
	ON_COMMAND(ID_SILENT_POPUP_CUSTOM3, OnPopupCustom3)
	ON_COMMAND(ID_SILENT_POPUP_CUSTOM4, OnPopupCustom4)
	ON_COMMAND(ID_SILENT_POPUP_CUSTOM5, OnPopupCustom5)
	ON_COMMAND(ID_SILENT_POPUP_CUSTOM6, OnPopupCustom6)
	ON_COMMAND(ID_SILENT_POPUP_CUSTOM7, OnPopupCustom7)
	ON_COMMAND(ID_SILENT_POPUP_CUSTOM8, OnPopupCustom8)
	ON_COMMAND(ID_SILENT_POPUP_CUSTOM9, OnPopupCustom9)
	ON_COMMAND(ID_SILENT_POPUP_CUSTOM10, OnPopupCustom10)
	ON_MESSAGE(WMU_TASK_START, OnTaskStart)
	ON_MESSAGE(WMU_TASK_COMPLETE, OnTaskComplete)
	ON_MESSAGE(WMU_ERROR, OnError)
	ON_MESSAGE(WMU_WARNING, OnWarning)
	ON_MESSAGE(WMU_QUESTION, OnQuestion)
	ON_MESSAGE(WMU_UPDATE_GUI, OnUpdateGUI)
	ON_MESSAGE(WMU_UPDATE_GUI_FILEPROGRESS, OnUpdateGUIFileProgress)
	ON_MESSAGE(WMU_UPDATE_GUI_TOTALPROGRESS, OnUpdateGUITotalProgress)
	ON_MESSAGE(WMU_TRAYICON_NOTIFY, OnTrayNotification)
	ON_MESSAGE(WMU_SHOWNOTIFIER, OnShowNotifier)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeSilent, CDialog)

//=====================================================================

CModeSilent::CModeSilent(CWinThread * pUpdateProcess,
						 int iCurrentTask /* = TASK_INIT */,
						 CString sMode /* = MODE_SILENT */)
					: CDialog(CModeSilent::IDD, (CWnd *)pUpdateProcess)
{
	// Set values
	m_pUpdateProcess = pUpdateProcess;
	m_iCurrentTask = iCurrentTask;
	m_bRollingBack = false;
	m_bSkipNextNotify = false;
	m_sMode = sMode;
}

//=====================================================================

CModeSilent::~CModeSilent()
{
	// Hide tray icon
	m_TrayIcon.HideIcon();

	// Destroy notify dialog
	delete m_pNotifyDlg;

	// Destroy menu
	delete m_pPopup;

	// Delete all popup bitmaps
	for (int i = 0; i < 9; i++)
	{
		m_arrBitmaps[i].DeleteObject();
	}
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

BOOL CModeSilent::OnInitDialog()
{
	// Call original function
	CDialog::OnInitDialog();

	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pLanguage = CLanguage::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pPath = CPath::Instance();

	// We are not initialized yet
	m_bInit = false;
	m_bFinished = false;

	// Init tooltip information
	m_ToolTipInfo.sTitle = _T("Updater");
	m_ToolTipInfo.iFileProgress = 0;
	m_ToolTipInfo.iTotalProgress = 0;

	// Create menu
	m_pPopup = new CMenu();
	m_pPopup->LoadMenu(IDR_SILENT_POPUP);

	// Load icon
	if (PathFileExists(m_pSettings->GetIcon()))
	{
		// Custom user icon
		HICON tmpIcon = (HICON)LoadImage(NULL, m_pSettings->GetIcon(), IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);

		// Check if we loaded successfully
		if (tmpIcon)
		{
			// Create with custom icon
			m_TrayIcon.Create(this, WMU_TRAYICON_NOTIFY, m_pLanguage->GetString(IDS_SILENT_WELCOME), 
				tmpIcon, IDR_SILENT_POPUP);
		}
		else
		{
			// Create with default Updater icon
			m_TrayIcon.Create(this, WMU_TRAYICON_NOTIFY, m_pLanguage->GetString(IDS_SILENT_WELCOME), 
				AfxGetApp()->LoadIcon(IDI_UPDATER), IDR_SILENT_POPUP);
		}
	}
	else
	{
			// Create with default Updater icon
			m_TrayIcon.Create(this, WMU_TRAYICON_NOTIFY, m_pLanguage->GetString(IDS_SILENT_WELCOME), 
				AfxGetApp()->LoadIcon(IDI_UPDATER), IDR_SILENT_POPUP);
	}

	// Load all menu bitmaps
	m_bmpMenuLogo.LoadBitmap(IDB_UPDATER);
	m_bmpMenuLogoNegative.LoadBitmap(IDB_UPDATER_NEG);

	// Create notify menu
	m_pNotifyDlg = new CNotifyDlg(m_pSettings->GetNotifySkin());
	m_pNotifyDlg->SetTitleColor(m_pSettings->GetNotifyTitleColor());
	m_pNotifyDlg->SetTitleRect(m_pSettings->GetNotifyTitleRect());
	m_pNotifyDlg->SetDescriptionRect(m_pSettings->GetNotifyDescriptionRect());
	m_pNotifyDlg->SetCloseRect(m_pSettings->GetNotifyCloseRect());
	m_pNotifyDlg->SetTitleFont(m_pSettings->GetNotifyTitleFontFace(), m_pSettings->GetNotifyTitleFontSize(), m_pSettings->GetNotifyTitleFontStyle());
	m_pNotifyDlg->SetFadeInSpeed(m_pSettings->GetNotifySpeedFadeIn());
	m_pNotifyDlg->SetFadeOutSpeed(m_pSettings->GetNotifySpeedFadeOut());
	m_pNotifyDlg->SetShowTime(m_pSettings->GetNotifyShowTime());
	m_pNotifyDlg->SetMaxTransparency(m_pSettings->GetNotifyTransparency());
	m_pNotifyDlg->SetTransparentColor(m_pSettings->GetNotifyTransparentColor());

	// Set right action
	PostMessage(WMU_TASK_START, m_iCurrentTask);

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

void CModeSilent::ExecuteFile(CPopupMenuData * pTemp)
{
	// Declare variables
	CString sDirectory;

	// Check if the user tries to open a website
	if (!m_pFunctions->IsWebAddress(pTemp->GetLocation()))
	{
		// Check if file exists
		if (!PathFileExists(pTemp->GetLocation()))
		{
			return;
		}
	}
	
	// Prepare shellExecutInfo
	SHELLEXECUTEINFO ShRun = {0};
	ShRun.cbSize = sizeof(SHELLEXECUTEINFO);
	ShRun.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShRun.hwnd = NULL;
	ShRun.lpVerb = NULL;
	ShRun.lpFile = pTemp->GetLocation();
	ShRun.lpParameters = pTemp->GetParameters();
	sDirectory = m_pPath->ExtractFilePath(m_pSettings->GetAppLocation());
	ShRun.lpDirectory = sDirectory;
	ShRun.nShow = SW_SHOW;
	ShRun.hInstApp = NULL;
	
	// Execute the file with the parameters
	ShellExecuteEx(&ShRun);
}

//=====================================================================

void CModeSilent::OnPopupAbout()
{
	// Show about dialog
	m_pUpdateProcess->PostThreadMessage(WMU_SHOWABOUT, 0, 0);
}

//=====================================================================

void CModeSilent::OnPopupExit()
{
	// Declare variables
	CString sTemp;

	// Are we finished?
	if (!m_bFinished)
	{
		// Set up confirmation text
		CString sTemp;
		sTemp.Format(m_pLanguage->GetString(IDS_CONFIRMATION_STOPUPDATING_EXPLANATION),
			m_pSettings->GetAppName(), m_pSettings->GetAppName(), m_pSettings->GetAppName());

		// Ask user for confirmation
		CConfirmationDlg dlgConfirm(m_pLanguage->GetString(IDS_CONFIRMATION_STOPUPDATING_TITLE),
			m_pLanguage->GetString(IDS_CONFIRMATION_TITLE), sTemp,
			m_pLanguage->GetString(IDS_GENERAL_YES), m_pLanguage->GetString(IDS_GENERAL_NO));
		if (dlgConfirm.DoModal() == IDNO)
		{
			// Don't quit
			return;
		}
	}

	// Stop the main thread
	if (m_bFinished)
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);
	else
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_CANCEL, 0);
}

//=====================================================================

void CModeSilent::OnPopupCustom1()
{
	// Execute right file
	ExecuteFile(m_pUpdateInfo->GetPopupMenuData(0));
}

//=====================================================================

void CModeSilent::OnPopupCustom2()
{
	// Execute right file
	ExecuteFile(m_pUpdateInfo->GetPopupMenuData(1));
}

//=====================================================================

void CModeSilent::OnPopupCustom3()
{
	// Execute right file
	ExecuteFile(m_pUpdateInfo->GetPopupMenuData(2));
}

//=====================================================================

void CModeSilent::OnPopupCustom4()
{
	// Execute right file
	ExecuteFile(m_pUpdateInfo->GetPopupMenuData(3));
}

//=====================================================================

void CModeSilent::OnPopupCustom5()
{
	// Execute right file
	ExecuteFile(m_pUpdateInfo->GetPopupMenuData(4));
}

//=====================================================================

void CModeSilent::OnPopupCustom6()
{
	// Execute right file
	ExecuteFile(m_pUpdateInfo->GetPopupMenuData(5));
}

//=====================================================================

void CModeSilent::OnPopupCustom7()
{
	// Execute right file
	ExecuteFile(m_pUpdateInfo->GetPopupMenuData(6));
}

//=====================================================================

void CModeSilent::OnPopupCustom8()
{
	// Execute right file
	ExecuteFile(m_pUpdateInfo->GetPopupMenuData(7));
}

//=====================================================================

void CModeSilent::OnPopupCustom9()
{
	// Execute right file
	ExecuteFile(m_pUpdateInfo->GetPopupMenuData(8));
}

//=====================================================================

void CModeSilent::OnPopupCustom10()
{
	// Execute right file
	ExecuteFile(m_pUpdateInfo->GetPopupMenuData(9));
}

//=====================================================================

LRESULT CModeSilent::OnTaskStart(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sTemp;
	
	// Get data
	int iTask = wParam;
	m_iCurrentTask = iTask;

	// Check what task is started
	switch (iTask)
	{
	case TASK_CHECKVERSION:
		// Set icon to new task
		m_TrayIcon.SetIconList(IDI_CHECK1, IDI_CHECK5);
		m_TrayIcon.Animate(300, -1);
		// Show that we started new task (only in silent mode, not in very silent mode)
		if (!m_bSkipNextNotify)
		{
			if (m_sMode == MODE_SILENT)
			{
				// Should we show custom notify?
				if (m_pSettings->GetCustomNotify())
				{
					// Yes
					m_pNotifyDlg->SetTitle(m_pLanguage->GetString(IDS_SILENT_NOTIFICATION));
					m_pNotifyDlg->SetDescription(m_pLanguage->GetString(IDS_SILENT_CHECKVERSION));
					m_pNotifyDlg->FadeIn();
				}
				else
				{
					// No
					m_TrayIcon.ShowBalloon(m_pLanguage->GetString(IDS_SILENT_CHECKVERSION), 
						m_pLanguage->GetString(IDS_SILENT_NOTIFICATION), NIIF_INFO, 10);
				}
			}
		}
		else
		{
			// We shouldn't skip next notify again
			m_bSkipNextNotify = false;
		}
		break;
		
	case TASK_HTMLMESSAGE:
		// Send task complete message
		m_pUpdateProcess->PostThreadMessage(WMU_TASK_COMPLETE, iTask, 0);
		break;

	case TASK_LICENSE:
		// Send task complete message
		m_pUpdateProcess->PostThreadMessage(WMU_TASK_COMPLETE, iTask, 0);
		break;

	case TASK_PROTECTION:
		// Don't perform any action
		break;

	case TASK_SELECTUPDATE:
		// Send task complete message
		m_pUpdateProcess->PostThreadMessage(WMU_TASK_COMPLETE, iTask, 0);
		break;

	case TASK_DOWNLOAD:
		// Update tooltip info
		m_ToolTipInfo.sTitle = m_pLanguage->GetString(IDS_SILENT_DOWNLOAD);
		m_ToolTipInfo.iTotalProgress = 0;
		m_ToolTipInfo.iFileProgress = 0;
		// Set icon to new task
		m_TrayIcon.SetIconList(IDI_DOWNLOAD1, IDI_DOWNLOAD4);
		m_TrayIcon.Animate(300, -1);
		// Show that we started new task (only in silent mode, not in very silent mode)
		if (!m_bSkipNextNotify)
		{
			if (m_sMode == MODE_SILENT)
			{
				// Should we show custom notify?
				if (m_pSettings->GetCustomNotify())
				{
					// Yes
					m_pNotifyDlg->SetTitle(m_pLanguage->GetString(IDS_SILENT_NOTIFICATION));
					m_pNotifyDlg->SetDescription(m_pLanguage->GetString(IDS_SILENT_DOWNLOAD));
					m_pNotifyDlg->FadeIn();
				}
				else
				{
					// No
					m_TrayIcon.ShowBalloon(m_pLanguage->GetString(IDS_SILENT_DOWNLOAD), 
						m_pLanguage->GetString(IDS_SILENT_NOTIFICATION), NIIF_INFO, 10);
				}
			}
		}
		else
		{
			// We shouldn't skip next notify again
			m_bSkipNextNotify = false;
		}
		break;
		
	case TASK_INSTALL:
		// Update tooltip info
		m_ToolTipInfo.sTitle = m_pLanguage->GetString(IDS_SILENT_INSTALL);
		m_ToolTipInfo.iTotalProgress = 0;
		m_ToolTipInfo.iFileProgress = 0;
		// Set icon to new task
		m_TrayIcon.SetIconList(IDI_INSTALL1, IDI_INSTALL8);
		m_TrayIcon.Animate(300, -1);
		// Show that we started new task (only in silent mode, not in very silent mode)
		if (!m_bSkipNextNotify)
		{
			if (m_sMode == MODE_SILENT)
			{
				// Should we show custom notify?
				if (m_pSettings->GetCustomNotify())
				{
					// Yes
					m_pNotifyDlg->SetTitle(m_pLanguage->GetString(IDS_SILENT_NOTIFICATION));
					m_pNotifyDlg->SetDescription(m_pLanguage->GetString(IDS_SILENT_INSTALL));
					m_pNotifyDlg->FadeIn();
				}
				else
				{
					// No
					m_TrayIcon.ShowBalloon(m_pLanguage->GetString(IDS_SILENT_INSTALL), 
						m_pLanguage->GetString(IDS_SILENT_NOTIFICATION), NIIF_INFO, 10);
				}
			}
		}
		else
		{
			// We shouldn't skip next notify again
			m_bSkipNextNotify = false;
		}
		break;

	case TASK_ROLLBACK:
		m_bRollingBack = true;
		// Show that we are rolling bakc changes (only in silent mode, not in very silent mode)
		if (!m_bSkipNextNotify)
		{
			if (m_sMode == MODE_SILENT)
			{
				// Should we show custom notify?
				if (m_pSettings->GetCustomNotify())
				{
					// Yes
					m_pNotifyDlg->SetTitle(m_pLanguage->GetString(IDS_SILENT_NOTIFICATION));
					m_pNotifyDlg->SetDescription(m_pLanguage->GetString(IDS_SILENT_ROLLBACK));
					m_pNotifyDlg->FadeIn();
				}
				else
				{
					// No
					m_TrayIcon.ShowBalloon(m_pLanguage->GetString(IDS_SILENT_ROLLBACK),
						m_pLanguage->GetString(IDS_SILENT_NOTIFICATION), NIIF_INFO, 10);
				}
			}
		}
		else
		{
			// We shouldn't skip next notify again
			m_bSkipNextNotify = false;
		}
		break;
	}

	return S_OK;
}

//=====================================================================

LRESULT CModeSilent::OnTaskComplete(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sTemp;

	// Get data
	int iTask = wParam;

	// Check what task is completed
	switch (iTask)
	{
	case TASK_ROLLBACK:
		// We are finished
		m_bFinished = true;
		m_bRollingBack = false;
		// If errors should not be shown, close app
		if (!m_pUpdateInfo->GetShowErrors())
		{
			// Finish update process
			m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);
		}
		else
		{
			// Sleep 3 seconds
			Sleep(3000);
			
			// Finish update process
			m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);
		}
		break;

	case TASK_INSTALL:
		// Set icon result
		m_TrayIcon.StopAnimation();
		m_TrayIcon.SetIcon(IDI_RESULT_GOOD);
		// Show that we are ready
		if (m_sMode == MODE_SILENT)
		{
			sTemp.Format(m_pLanguage->GetString(IDS_SILENT_FINISH), m_pUpdateInfo->GetNewVersion());

			if (!m_bSkipNextNotify)
			{
				// Should we show custom notify?
				if (m_pSettings->GetCustomNotify())
				{
					// Yes
					m_pNotifyDlg->SetTitle(m_pLanguage->GetString(IDS_SILENT_NOTIFICATION));
					m_pNotifyDlg->SetDescription(sTemp);
					m_pNotifyDlg->FadeIn();
				}
				else
				{
					// No
					m_TrayIcon.ShowBalloon(sTemp, m_pLanguage->GetString(IDS_SILENT_NOTIFICATION), NIIF_INFO, 10);
				}
			}
			else
			{
				// We shouldn't skip next notify again
				m_bSkipNextNotify = false;
			}

			m_TrayIcon.SetTooltipText(sTemp);
		}
		// We are finished
		m_bFinished = true;

		// If we don't have any popup menu items, finish the update
		if (m_pUpdateInfo->GetPopupMenuCount() <= 0)
		{
			// Finish
			m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);
		}
		break;
	}

	// If it is the last step, let user close application
	if ((iTask != TASK_INSTALL) && (iTask != TASK_ROLLBACK) && !m_bFinished)
	{
		// Sleep some time
		Sleep(100);

		// Task is complete, don't ask user for confirmation, go to next step
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_STARTTASK, iTask + 1);
	}

	return S_OK;
}

//=====================================================================

LRESULT CModeSilent::OnError(WPARAM wParam, LPARAM lParam)
{
	// Get data
	int iError = wParam;
	CString sError = _T(""), sTemp;
	bool bRollback = true;
	bool bShowError = true;

	// Check what error has occured
	switch (iError)
	{
	case ERROR_UNKNOWN:
		sError.Format(m_pLanguage->GetString(IDS_ERROR_UNKNOWN));
		break;

	case ERROR_NOCONNECTION:
		sError.Format(m_pLanguage->GetString(IDS_ERROR_NOCONNECTION));
		break;

	case ERROR_SERVER:
		sError.Format(m_pLanguage->GetString(IDS_ERROR_SERVER));
		break;

	case ERROR_NODISKSPACE:
		sError.Format(m_pLanguage->GetString(IDS_ERROR_NODISKSPACE), (lParam / 1024 / 1024));
		break;

	case ERROR_FILENOTFOUND:
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sError.Format(m_pLanguage->GetString(IDS_ERROR_FILENOTFOUND), sTemp);
		break;

	case ERROR_COPYFILE:
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sError.Format(m_pLanguage->GetString(IDS_ERROR_COPYFILE), sTemp);
		break;

	case ERROR_DELETEFILE:
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sError.Format(m_pLanguage->GetString(IDS_ERROR_DELETEFILE), sTemp);
		break;

	case ERROR_RUNFILE:
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sError.Format(m_pLanguage->GetString(IDS_ERROR_RUNFILE), sTemp);
		break;

	case ERROR_CLOSEAPP:
		sError.Format(m_pLanguage->GetString(IDS_ERROR_CLOSEAPP), lParam);
		break;

	case ERROR_FILECORRUPT:
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sError.Format(m_pLanguage->GetString(IDS_ERROR_FILECORRUPT), sTemp);
		break;

	case ERROR_NONEWVERSION:
		sError.Format(m_pLanguage->GetString(IDS_STATUS_NONEWVERSION));
		// Don_T('t rollback & don')t show as error
		bRollback = false;
		bShowError = false;
		break;

	case ERROR_WRITEFILE:
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sError.Format(m_pLanguage->GetString(IDS_ERROR_WRITEFILE), sTemp);
		break;

	case ERROR_UNZIPFILE:
		sTemp.Format(_T("%s"), lParam);
		sTemp = m_pFunctions->StripFileStatus(m_pPath->ExtractFileName(sTemp));
		sError.Format(m_pLanguage->GetString(IDS_ERROR_UNZIPFILE), sTemp);
		break;

	case ERROR_PROTECTION:
		sError = m_pLanguage->GetString(IDS_ERROR_PROTECTION);
		// Don't rollback
		bRollback = false;
		break;
	}

	// Set new tooltip
	m_TrayIcon.SetTooltipText(sError);

	// Set new error icon
	m_TrayIcon.StopAnimation();
	if (bShowError)
	{
		// Should we show error?
		if (m_pUpdateInfo->GetShowErrors())
		{
			m_TrayIcon.SetIcon(IDI_RESULT_ERROR);

			// Show error to user
			if (m_sMode == MODE_SILENT)
			{
				if (!m_bSkipNextNotify)
				{
					// Should we show custom notify?
					if (m_pSettings->GetCustomNotify())
					{
						// Yes
						m_pNotifyDlg->SetTitle(m_pLanguage->GetString(IDS_SILENT_NOTIFICATION));
						m_pNotifyDlg->SetDescription(sError);
						m_pNotifyDlg->FadeIn();
					}
					else
					{
						// No
						m_TrayIcon.ShowBalloon(sError, m_pLanguage->GetString(IDS_SILENT_ERROR), NIIF_ERROR, 10);
					}
				}
				else
				{
					// We shouldn't skip next notify again
					m_bSkipNextNotify = false;
				}

				// Sleep 3 seconds so user can read error message
				Sleep(3000);
			}
		}
		else
		{
			// We should now hide the tray icon
			m_TrayIcon.HideIcon();
		}

		if (bRollback)
		{
			// Start next task, which will be rollback
			m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_STARTTASK, TASK_ROLLBACK);
		}
	}
	else
	{
		m_TrayIcon.SetIcon(IDI_RESULT_GOOD);
		m_bFinished = true;

		// Show message (no real error) to user
		if (m_sMode == MODE_SILENT)
		{
			if (!m_bSkipNextNotify)
			{	
				// Should we show custom notify?
				if (m_pSettings->GetCustomNotify())
				{
					// Yes
					m_pNotifyDlg->SetTitle(m_pLanguage->GetString(IDS_SILENT_NOTIFICATION));
					m_pNotifyDlg->SetDescription(sError);
					m_pNotifyDlg->FadeIn();
				}
				else
				{
					// No
					m_TrayIcon.ShowBalloon(sError, m_pLanguage->GetString(IDS_SILENT_NOTIFICATION), NIIF_INFO, 10);
				}
			}
			else
			{
				// We shouldn't skip next notify again
				m_bSkipNextNotify = false;
			}
		}
	}

	// Sleep to show balloon to user
	Sleep(500);
	
	// Check if we should rollback
	if (m_bFinished)
		m_pUpdateProcess->PostThreadMessage(WMU_THREADMESSAGE, THREAD_FINISH, 0);

	return S_OK;
}

//=====================================================================

LRESULT CModeSilent::OnWarning(WPARAM wParam, LPARAM lParam)
{
	// Get data
	int iWarning = wParam;
	CString sWarning = _T(""), sTemp;

	// Check what error has occured
	switch (iWarning)
	{
	case WARNING_REGISTER:
		sWarning.Format(m_pLanguage->GetString(IDS_WARNING_REGISTER), lParam);
		break;
	}

	// Should we show warning?
	if (m_sMode == MODE_SILENT)
	{
		if (!m_bSkipNextNotify)
		{
			// Should we show custom notify?
			if (m_pSettings->GetCustomNotify())
			{
				// Yes
				m_pNotifyDlg->SetTitle(m_pLanguage->GetString(IDS_SILENT_NOTIFICATION));
				m_pNotifyDlg->SetDescription(sWarning);
				m_pNotifyDlg->FadeIn();
			}
			else
			{
				// No
				m_TrayIcon.ShowBalloon(sWarning, m_pLanguage->GetString(IDS_SILENT_NOTIFICATION), NIIF_WARNING, 10);
			}
		}
		else
		{
			// We shouldn't skip next notify again
			m_bSkipNextNotify = false;
		}
	}

	// Sleep to show balloon to user
	Sleep(500);

	// Continue
	m_pUpdateProcess->PostThreadMessage(WMU_WARNING_COMPLETE, 0, 0);

	return S_OK;
}

//=====================================================================

LRESULT CModeSilent::OnQuestion(WPARAM wParam, LPARAM lParam)
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
		// If mode is verysilent
		if (m_sMode == MODE_VERYSILENT)
		{
			// Answer yes
			iResult = QUESTIONRESULT_YES;
		}
		else
		{
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
		}
		break;

	case QUESTION_CLOSEAPPLICATION:
		// If mode is verysilent
		if (m_sMode == MODE_VERYSILENT)
		{
			// Answer yes
			iResult = QUESTIONRESULT_YES;
		}
		else
		{
			// Set up confirmation text
			sTemp.Format(m_pLanguage->GetString(IDS_CONFIRMATION_CLOSEAPPLICATION_EXPLANATION),
				lParam, m_pSettings->GetAppName(), lParam);					

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
			}
		}
		break;

	case QUESTION_REGISTERFILE:
		// If mode is verysilent
		if (m_sMode == MODE_VERYSILENT)
		{
			// Answer yes
			iResult = QUESTIONRESULT_YES;
		}
		else
		{
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

LRESULT CModeSilent::OnUpdateGUI(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sTrayToolTip;

	// Don't update if we are finished
	if (m_bFinished)
		return S_OK;

	// Update process of GUI
	sTrayToolTip.Format(_T("%s\n%s %d%%\n%s\n%s %d%%\n%s"),
		m_ToolTipInfo.sTitle,
		m_ToolTipInfo.sFileTitle, m_ToolTipInfo.iFileProgress,  
		m_ToolTipInfo.sFileProgress,
		m_pLanguage->GetString(IDS_FULL_TOTALPROGRESS), m_ToolTipInfo.iTotalProgress,
		m_ToolTipInfo.sTotalProgress);
	
	// Set tooltip
	m_TrayIcon.SetTooltipText(sTrayToolTip);

	return S_OK;
}

//=====================================================================

LRESULT CModeSilent::OnUpdateGUIFileProgress(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int i;
	
	// Get file progress
	m_ToolTipInfo.iFileProgress = static_cast<int>(wParam);
	
	// Calculate [|||   ] progress bar
	int iStripes = m_ToolTipInfo.iFileProgress / 10;
	int iSpaces = (10 - iStripes);
	
	// Now create progress bar
	CString sStripes, sSpaces;
	for (i = 0; i < iStripes; i++)
	{
		sStripes = sStripes + _T("||");
	}
	for (i = 0; i < iSpaces; i++)
	{
		sSpaces = sSpaces + _T("  ");
	}
	m_ToolTipInfo.sFileProgress.Format(_T("[%s%s]"), sStripes, sSpaces);
	
	// Now send message to update GUI
	PostMessage(WMU_UPDATE_GUI, 0, 0);

	return S_OK;
}

//=====================================================================

LRESULT CModeSilent::OnUpdateGUITotalProgress(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	int i;
	
	// Get file progress
	m_ToolTipInfo.iTotalProgress = static_cast<int>(wParam);
	
	// Calculate [|||   ] progress bar
	int iStripes = m_ToolTipInfo.iTotalProgress / 10;
	int iSpaces = (10 - iStripes);
	
	// Now create progress bar
	CString sStripes, sSpaces;
	for (i = 0; i < iStripes; i++)
	{
		sStripes = sStripes + _T("||");
	}
	for (i = 0; i < iSpaces; i++)
	{
		sSpaces = sSpaces + _T("  ");
	}
	m_ToolTipInfo.sTotalProgress.Format(_T("[%s%s]"), sStripes, sSpaces);
	
	// Now send message to update GUI
	PostMessage(WMU_UPDATE_GUI, 0, 0);

	return S_OK;
}

//=====================================================================

LRESULT CModeSilent::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CPopupMenuData * pTemp;
	CString sBitmapLocation;
	HBITMAP hBitmap;

#ifdef _DEBUG
	// Check if the user is double-clicking
	if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
	{
		// Are we in silent mode?
		if (m_sMode == MODE_SILENT)
		{
			TCHAR * pMode = new TCHAR[MODE_FULL.GetLength()];
			lstrcpy(pMode, MODE_FULL);

			// Switch to full mode
			m_pUpdateProcess->PostThreadMessage(WMU_SWITCHUI, (WPARAM)pMode, 0);
		}
	}
#endif

	// Check if right mouse button is clicked
	if (LOWORD(lParam) == WM_RBUTTONUP)
	{
		// Handle menu ourselves, because we are having problems with
		// the trayicon
	
		// Clear menu
		delete m_pPopup;

		// Load basic menu again
		m_pPopup = new CMenu();
		m_pPopup->LoadMenu(IDR_SILENT_POPUP);

		// Create the menu with the customized items
		CMenu * pPopup = m_pPopup->GetSubMenu(0);

		// Set Updater logo bitmap
		m_pPopup->SetMenuItemBitmaps(ID_SILENT_POPUP_ABOUT, MF_BYCOMMAND, &m_bmpMenuLogo, &m_bmpMenuLogoNegative);

		// Should we disable exit button or not?
		if (m_pUpdateInfo->GetForceUpdate())
		{
			// Disable
			pPopup->EnableMenuItem(ID_SILENT_POPUP_EXIT, MF_BYCOMMAND | MF_GRAYED);
		}

		// Get all items and add them to the menu, but only when we are finished
		if (m_bFinished)
		{
			// Always enable exit button
			pPopup->EnableMenuItem(ID_SILENT_POPUP_EXIT, MF_BYCOMMAND | MF_ENABLED);

			for (int i = 0; i < m_pUpdateInfo->GetPopupMenuCount(); i++)
			{
				// Get object
				pTemp = m_pUpdateInfo->GetPopupMenuData(i);

				// Should we add item or separator?
				if (pTemp->GetText() == _T("-"))
				{
					pPopup->InsertMenu(ID_SILENT_POPUP_EXIT, MF_BYCOMMAND | MF_SEPARATOR,
									   NULL, _T(""));
				}
				else
				{
					// Add menu item
					pPopup->InsertMenu(ID_SILENT_POPUP_EXIT, MF_BYCOMMAND | MF_ENABLED | MF_STRING,
									   ID_SILENT_POPUP_CUSTOM1 + i, pTemp->GetText());

					// Should we add bitmap?
					if (!pTemp->GetBitmap().IsEmpty())
					{
						// Get location to bitmap
						sBitmapLocation = m_pPath->GetPathUpdaterTemp() + m_pPath->ExtractFileName(pTemp->GetBitmap());

						// Load bitmap from file
						hBitmap = (HBITMAP) ::LoadImage(NULL, sBitmapLocation,
							IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

						// Delete previous object
						m_arrBitmaps[i].DeleteObject();

						// Set bitmap into CBitmap object
						m_arrBitmaps[i].Attach(hBitmap);

						// Load bitmap into menu
						pPopup->SetMenuItemBitmaps(ID_SILENT_POPUP_CUSTOM1 + i, MF_BYCOMMAND, &m_arrBitmaps[i], &m_arrBitmaps[i]);
					}
				}
			}
		}

		// Get mouse position
		CPoint pos;
		GetCursorPos(&pos);

		// Track popup menu (but not in rollback mode)
		if (!m_bRollingBack)
		{
			//::TrackPopupMenuEx(pPopup->m_hMenu, TPM_LEFTALIGN, pos.x, pos.y, m_hWnd, NULL);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);
		}
			
	}
	else
	{
		// Delegate all the work back to the default 
		// implementation in CSystemTray.
		m_TrayIcon.OnTrayNotification(wParam, lParam);
	}

	return S_OK;
}

//=====================================================================

LRESULT CModeSilent::OnShowNotifier(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	CString sTitle, sDescription;

	// Get data
	sTitle.Format(_T("%s"), wParam);
	sDescription.Format(_T("%s"), lParam);

	// Should we show custom notify?
	if (m_pSettings->GetCustomNotify())
	{
		// Yes
		m_pNotifyDlg->SetTitle(sTitle);
		m_pNotifyDlg->SetDescription(sDescription);
		m_pNotifyDlg->FadeIn();
	}
	else
	{
		// No
		m_TrayIcon.ShowBalloon(sDescription, sTitle, NIIF_INFO, 10);
	}

	// Skip next time we should show notifier
	m_bSkipNextNotify = true;

	return S_OK;
}
