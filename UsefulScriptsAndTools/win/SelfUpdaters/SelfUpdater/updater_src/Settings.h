/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	Settings.h
	file path:	Updater
	file base:	Settings
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef SETTINGS_H
#define SETTINGS_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

#include "GetOpt.h"

//*********************************************************************
// CSettings
//*********************************************************************

class CSettings  
{
public:
	// Functions
	static   CSettings * Instance();
	void	 ReadFile(CString sSettingsFile);
	void	 SaveFile(CString sSettingsFile = _T(""));
	CString	 ParseCommandLineForSettingsFile(CString sCommand);
	void	 ParseCommandLine(CString sCommand);
	CString  GetLanguage();
	void	 SetLanguage(CString sValue);
	CString  GetRunmode();
	void	 SetRunmode(CString sValue);
	CString  GetURL();
	void	 SetURL(CString sValue);
	CString  GetUpdateSelector();
	void	 SetUpdateSelector(CString sValue);
	int      GetTimeout();
	void	 SetTimeout(int iValue);
	CString  GetAppName();
	void	 SetAppName(CString sValue);
	CString  GetAppLocation();
	void	 SetAppLocation(CString sValue);
	CString	 GetAppRoot();
	void	 SetAppRoot(CString sValue);
	CString  GetAppVersion();
	void	 SetAppVersion(CString sValue);
	CString  GetLogoSmall();
	void	 SetLogoSmall(CString sValue);
	CString  GetLogoLarge();
	void	 SetLogoLarge(CString sValue);
	CString  GetIcon();
	void	 SetIcon(CString sValue);
	CString  GetSelfUpdateLocation();
	void	 SetSelfUpdateLocation(CString sValue);
	bool     GetCheckConnection();
	void	 SetCheckConnection(bool bValue);
	CString  GetFtpUsername();
	void	 SetFtpUsername(CString sValue);
	CString  GetFtpPassword();
	void	 SetFtpPassword(CString sValue);
	CString  GetHttpUsername();
	void	 SetHttpUsername(CString sValue);
	CString  GetHttpPassword();
	void	 SetHttpPassword(CString sValue);
	CString  GetProxyUsername();
	void	 SetProxyUsername(CString sUsername);
	CString  GetProxyPassword();
	void	 SetProxyPassword(CString sPassword);
	CString  GetProxyType();
	void	 SetProxyType(CString sProxyType = PROXY_TYPE_AUTODETECT);
	CString	 GetProxyManualHttp();
	void	 SetProxyManualHttp(CString sURL);
	CString	 GetProxyManualFtp();
	void	 SetProxyManualFtp(CString sURL);
	CString  GetProxyAutoConfigURL();
	void	 SetProxyAutoConfigURL(CString sURL);
	bool	 GetRestoreOlderVersion();
	void	 SetRestoreOlderVersion(bool bValue);
	bool	 GetCustomNotify();
	void	 SetCustomNotify(bool bValue);
	COLORREF GetLinkColorLink();
	void	 SetLinkColorLink(COLORREF clrValue);
	COLORREF GetLinkColorHover();
	void	 SetLinkColorHover(COLORREF clrValue);
	CString  GetNotifySkin();
	void	 SetNotifySkin(CString sValue);
	COLORREF GetNotifyTitleColor();
	void	 SetNotifyTitleColor(COLORREF clrValue);
	CString	 GetNotifyTitleFontFace();
	void	 SetNotifyTitleFontFace(CString sValue);
	int		 GetNotifyTitleFontSize();
	void	 SetNotifyTitleFontSize(int iValue);
	CString  GetNotifyTitleFontStyle();
	void	 SetNotifyTitleFontStyle(CString sValue);
	CRect	 GetNotifyTitleRect();
	void	 SetNotifyTitleRect(CRect oValue);
	CRect	 GetNotifyDescriptionRect();
	void	 SetNotifyDescriptionRect(CRect oValue);
	CRect	 GetNotifyCloseRect();
	void	 SetNotifyCloseRect(CRect oValue);
	int		 GetNotifySpeedFadeIn();
	void	 SetNotifySpeedFadeIn(int iValue);
	int		 GetNotifySpeedFadeOut();
	void	 SetNotifySpeedFadeOut(int iValue);
	int		 GetNotifyShowTime();
	void	 SetNotifyShowTime(int iValue);
	int		 GetNotifyTransparency();
	void	 SetNotifyTransparency(int iValue);
	COLORREF GetNotifyTransparentColor();
	void	 SetNotifyTransparentColor(COLORREF clrValue);

private:
	// Constructor & destructor
	CSettings();			// Private constructor because singleton!

	// Functions
	void	ParseIni(CString sFilename);
	void	ParseColor(CString sColor, int & iRed, int & iGreen, int & iBlue);
	void	ParseRect(CString sRect, CRect & rect);
	void	SetParameterOptions(CGetOpt & oCommandLineParsing);
	CString GetSettingsFileFromCommandLine(CGetOpt & oCommandLineParsing);
	void	InitializeCommandOptions(CGetOpt * pGetOpt);

	// Variables
	static std::auto_ptr<CSettings> sm_inst;
	CString	 m_sSettingsFile;					// Location of the parsed settings file
	CString  m_sLanguage;						// Language for Updater
	CString  m_sRunmode;						// Run mode
	CString  m_sLogoLarge;						// Application logo	(large) 150 x 85
	CString  m_sLogoSmall;						// Application logo (small) 40 x 40
	CString  m_sIcon;							// Application icon
	CString  m_sSelfUpdateLocation;				// Self update location
	CString  m_sURL;							// URL to download update list
	CString  m_sUpdateSelector;					
	CString  m_sAppName;						// Application name
	CString  m_sAppVersion;						// Application version (current)
	CString  m_sAppLocation;					// Application exe location
	CString  m_sAppRoot;						// Application root path
	CString  m_sFtpUsername;					// Username for ftp connections
	CString  m_sFtpPassword;					// Password for ftp connections
	CString  m_sHttpUsername;					// Username for http connections
	CString  m_sHttpPassword;					// Password for http connections
	CString  m_sProxyUsername;					// Username for proxy
	CString  m_sProxyPassword;					// Password for proxy
	CString  m_sProxyType;						// Proxy type
	CString  m_sProxyManualHttp;				// Proxy manual http
	CString  m_sProxyManualFtp;					// Proxy manual ftp
	CString  m_sProxyAutoConfigURL;				// Proxy auto config URL
	int		 m_iTimeout;						// Timeout for server
	bool     m_bCheckConnection;				// True if connection should be tested
	bool	 m_bCustomNotify;					// True if custom notifier should be used
	bool	 m_bRestoreOlderVersion;			// If true, Updater should restore older version
	COLORREF m_clrLinkColorLink;
	COLORREF m_clrLinkColorHover;
	CString  m_sNotifySkin;
	COLORREF m_clrNotifyTitleColor;
	CString	 m_sNotifyTitleFontFace;
	int		 m_iNotifyTitleFontSize;
	CString  m_sNotifyTitleFontStyle;
	CRect	 m_rectNotifyTitleRect;
	CRect	 m_rectNotifyDescriptionRect;
	CRect	 m_rectNotifyCloseRect;
	int		 m_iNotifySpeedFadeIn;
	int		 m_iNotifySpeedFadeOut;
	int		 m_iNotifyShowTime;
	int		 m_iNotifyTransparency;
	COLORREF m_clrNotifyTransparentColor;
};

// End of redefinition protection
#endif // SETTINGS_H