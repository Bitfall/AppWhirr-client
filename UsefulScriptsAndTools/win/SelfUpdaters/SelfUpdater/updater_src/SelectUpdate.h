/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   10:58
	filename: 	SelectUpdate.h
	file path:	Updater
	file base:	SelectUpdate
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef SELECTUPDATE_H
#define SELECTUPDATE_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CSelectUpdate
//*********************************************************************

class CSelectUpdate : public CWinThread
{
public:
	// Functions
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	// Constructor & destructor
	DECLARE_DYNCREATE(CSelectUpdate)
	CSelectUpdate();
	virtual ~CSelectUpdate();

	// Functions
	void Main();
	void PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnThreadMessage(WPARAM wParam, LPARAM lParam);

	// Variables
	CWinThread    * m_pParent;
	bool		    m_bStarted;
	CFunctions    * m_pFunctions;
	CSettings     * m_pSettings;
	CUpdateInfo   * m_pUpdateInfo;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // SELECTUPDATE_H