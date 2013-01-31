/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   15:15
	filename: 	ModeSilent.h
	file path:	Updater
	file base:	ModeSilent
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODESILENT_H
#define MODESILENT_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "resource.h"
#include "SystemTray.h"				// System tray support
#include "PopupMenuData.h"
#include "NotifyDlg.h"

//*********************************************************************
// STRUCTS
//*********************************************************************

struct ToolTipInformation
{
	CString sTitle;					// Title of action
	CString sFileTitle;				// Title of file currently handled
	int		iFileProgress;			// File progress
	CString sFileProgress;			// File progress ([|||||    ])
	int		iTotalProgress;			// Total progress
	CString sTotalProgress;			// Total progress ([|||||    ])
};

//*********************************************************************
// CModeSilent
//*********************************************************************

class CModeSilent : public CDialog
{
public:
	// Constructor & destructor
	DECLARE_DYNAMIC(CModeSilent)
	CModeSilent(CWinThread * pUpdateProcess, int iCurrentTask = TASK_INIT, 
				CString sMode = MODE_SILENT);
	virtual ~CModeSilent();

	// Functions

	// Dialog Data
	enum { IDD = IDD_SILENT };

private:
	// Functions
	BOOL OnInitDialog();
	void ExecuteFile(CPopupMenuData * pTemp);
	afx_msg void OnPopupAbout();
	afx_msg void OnPopupExit();
	afx_msg void OnPopupCustom1();
	afx_msg void OnPopupCustom2();
	afx_msg void OnPopupCustom3();
	afx_msg void OnPopupCustom4();
	afx_msg void OnPopupCustom5();
	afx_msg void OnPopupCustom6();
	afx_msg void OnPopupCustom7();
	afx_msg void OnPopupCustom8();
	afx_msg void OnPopupCustom9();
	afx_msg void OnPopupCustom10();
	afx_msg LRESULT OnTaskStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskComplete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnError(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWarning(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnQuestion(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateGUI(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateGUIFileProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateGUITotalProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnShowNotifier(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread  * m_pUpdateProcess;
	int			  m_iCurrentTask;
	CSystemTray   m_TrayIcon;
	ToolTipInformation m_ToolTipInfo;
	CString		  m_sTrayToolTip;
	CString		  m_sMode;
	CFunctions  * m_pFunctions;
	CSettings   * m_pSettings;
	CUpdateInfo * m_pUpdateInfo;
	CLanguage	* m_pLanguage;
	CPath		* m_pPath;
	CMenu       * m_pPopup;
	bool		  m_bInit;
	bool		  m_bFinished;
	bool		  m_bRollingBack;
	bool		  m_bSkipNextNotify;
	CBitmap		  m_bmpMenuLogo;
	CBitmap		  m_bmpMenuLogoNegative;
	CBitmap		  m_arrBitmaps[9];
	CNotifyDlg  * m_pNotifyDlg;
	
	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODESILENT_H