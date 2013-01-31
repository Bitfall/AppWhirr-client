/********************************************************************
	created:	2004/11/07
	created:	7:11:2004   12:20
	filename: 	ModeFullPropHtmlMessage.h
	file path:	Updater
	file base:	ModeFullPropHtmlMessage
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLPROPHTMLMESSAGE_H
#define MODEFULLPROPHTMLMESSAGE_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "WebBrowser.h"

//*********************************************************************
// CModeFullPropHtmlMessage dialog
//*********************************************************************

class CModeFullPropHtmlMessage : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CModeFullPropHtmlMessage)

public:
	// Constructor & destructor
	CModeFullPropHtmlMessage();
	virtual ~CModeFullPropHtmlMessage();

	// Functions

	// Dialog Data
	enum { IDD = IDD_FULL_PROPPAGE_HTMLMESSAGE };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL	OnInitDialog();
	BOOL	OnSetActive();
	void	OnTimer(UINT nIDEvent);
	LRESULT OnWizardNext();

	// Variables
	CWebBrowser		 m_htmlMessage;
	CFunctions	   * m_pFunctions;
	CSettings	   * m_pSettings;
	CUpdateInfo    * m_pUpdateInfo;
	CLanguage	   * m_pLanguage;
	int				 m_iCountDown;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODEFULLPROPHTMLMESSAGE_H
