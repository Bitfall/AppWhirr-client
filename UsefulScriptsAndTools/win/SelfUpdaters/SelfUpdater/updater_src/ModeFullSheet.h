/********************************************************************
	created:	2004/10/29
	created:	29:10:2004   17:48
	filename: 	ModeFullSheet.h
	file path:	Updater
	file base:	ModeFullSheet
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLSHEET_H
#define MODEFULLSHEET_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"

// Include propertypages
#include "ModeFullPropWelcome.h"
#include "ModeFullPropCheck.h"
#include "ModeFullPropHtmlMessage.h"
#include "ModeFullPropLicense.h"
#include "ModeFullPropProtection.h"
#include "ModeFullPropSelectUpdate.h"
#include "ModeFullPropDownload.h"
#include "ModeFullPropInstall.h"
#include "ModeFullPropFinish.h"
#include "ModeFullPropRollback.h"

// Include hyperlink label
#include "HyperlinkLabel.h"

// Include caption bar button
#include "CustomBitmapButton.h"

const int TITLEBARBTN_TOTRAY = 1;

//*********************************************************************
// CModeFullSheet
//*********************************************************************

class CModeFullSheet : public CPropertySheetEx
{
	DECLARE_DYNAMIC(CModeFullSheet)

public:
	// Constructor & destructor
	CModeFullSheet(CWinThread * pUpdateProcess, int iCurrentTask);
	virtual ~CModeFullSheet();

	// Functions
	BOOL PressButton(int nButton);

private:
	// Functions
	BOOL OnInitDialog();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnSysCommand(UINT nID, LPARAM lParam);
	void DisableCancel();
	bool CloseUpdater();
	void CreateTitleBarButtons();
	afx_msg HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnLblUpdater(WPARAM, LPARAM);
	afx_msg LRESULT OnTaskStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskComplete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnError(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWarning(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnQuestion(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateGUI(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateGUIFileProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateGUITotalProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateGUIProtection(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL	OnEraseBkgnd(CDC * pDC);
	afx_msg void	OnPaint();
	afx_msg void	OnBtnToTray(NMHDR * pNotifyStruct, LRESULT * result);

	// Variables
	CCustomBitmapButton			m_ctrlCaptionFrame;
	CHyperlinkLabel			    m_lblUpdater;
	CModeFullPropWelcome	    m_pgWelcome;
	CModeFullPropCheck		    m_pgCheck;
	CModeFullPropHtmlMessage    m_pgHtmlMessage;
	CModeFullPropLicense	    m_pgLicense;
	CModeFullPropProtection		m_pgProtection;
	CModeFullPropSelectUpdate   m_pgSelectUpdate;
	CModeFullPropDownload	    m_pgDownload;
	CModeFullPropInstall	    m_pgInstall;
	CModeFullPropFinish		    m_pgFinish;
	CModeFullPropRollback	    m_pgRollback;
	CWinThread				  * m_pUpdateProcess;
	CFunctions				  * m_pFunctions;
	CPath					  * m_pPath;
	CSettings				  * m_pSettings;
	CUpdateInfo				  * m_pUpdateInfo;
	CLanguage				  * m_pLanguage;
	bool						m_bInit;
	bool						m_bFinished;
	bool						m_bInstallReady;
	int							m_iStartTask;
	int							m_iCurrentTask;
	CFont				   		m_fntUpdater;
	CBitmap						m_bmpMenuLogo;
	CBitmap						m_bmpMenuLogoNegative;
	CBitmap						m_bmpHeader;
	CBrush						m_HollowBrush;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODEFULLSHEET_H
