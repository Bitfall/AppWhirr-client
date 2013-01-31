/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   9:08
	filename: 	Updater.h
	file path:	Updater
	file base:	Updater
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef UPDATER_H
#define UPDATER_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "resource.h"		// main symbols

//*********************************************************************
// CUpdaterApp
//*********************************************************************

class CUpdaterApp : public CWinApp
{
public:
	// Constructor & destructor
	CUpdaterApp();

	// Functions
	virtual BOOL InitInstance();
	virtual int	 ExitInstance();

private:
	// Functions
	BOOL	Main();
	void	StartApplication(CString sCommandLine);
	void	StartUpdater();
	void	StartProxySettings();
	void	StartRestore();
	void	StartViewLog();
	void	StartCheckForUpdates();
	bool	IsUserLoggedIn();
	CString ConvertTime(int iMilliSeconds);

	// Variables
	int m_iReturnValue;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // UPDATER_H
