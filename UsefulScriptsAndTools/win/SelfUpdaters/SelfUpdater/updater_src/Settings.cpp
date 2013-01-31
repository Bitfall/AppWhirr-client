/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	Settings.cpp
	file path:	Updater
	file base:	Settings
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "Settings.h"
#include "GetOpt.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CSettings> CSettings::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CSettings::CSettings()
{
	// Set default values for proxy username and password
	m_sProxyUsername = _T("");
	m_sProxyPassword = _T("");
	m_sSelfUpdateLocation = _T("");
	m_bRestoreOlderVersion = false;
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CSettings * CSettings::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = auto_ptr<CSettings>(new CSettings);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CSettings);
	return sm_inst.get();
	*/
}

//=====================================================================

void CSettings::ReadFile(CString sSettingsFile)
{
	// Parse ini file
	ParseIni(sSettingsFile);

	// Store path
	m_sSettingsFile = sSettingsFile;
}

//=====================================================================

void CSettings::SaveFile(CString sSettingsFile)
{
	// Check if settingsfile is empty
	if (sSettingsFile == _T(""))
	{
		// Save to stored path
		sSettingsFile = m_sSettingsFile;
	}

	// Read proxy type from ini-file
	WritePrivateProfileString(_T("PROXY"), _T("TYPE"), m_sProxyType, sSettingsFile);

	// Write proxy manual http from ini-file
	WritePrivateProfileString(_T("PROXY"), _T("HTTP"), m_sProxyManualHttp, sSettingsFile);

	// Write proxy manual ftp from ini-file
	WritePrivateProfileString(_T("PROXY"), _T("FTP"), m_sProxyManualFtp, sSettingsFile);

	// Write proxy auto config url from ini-file
	WritePrivateProfileString(_T("PROXY"), _T("AUTOCONFIGURL"), m_sProxyAutoConfigURL, sSettingsFile);
}

//=====================================================================

CString CSettings::ParseCommandLineForSettingsFile(CString sCommand)
{
	USES_CONVERSION;

	// Declare variables
	CGetOpt oCommandLineParsing;

	// Initialize command options
	InitializeCommandOptions(&oCommandLineParsing);

	// Now parse command line
	oCommandLineParsing.ParseCommandLine(W2A(sCommand));

	// Now try to get the settings file
	return GetSettingsFileFromCommandLine(oCommandLineParsing);
}

//=====================================================================

void CSettings::ParseCommandLine(CString sCommand)
{
	USES_CONVERSION;

	// Declare variables
	CGetOpt oCommandLineParsing;

	// Initialize command options
	InitializeCommandOptions(&oCommandLineParsing);

	// Now parse command line
	oCommandLineParsing.ParseCommandLine(W2A(sCommand));

	// Set parameter options
	SetParameterOptions(oCommandLineParsing);

	// Replace all instances of %username% and %password% in the update file url
	m_sURL = CFunctions::Instance()->ReplaceConnectionSettings(m_sURL);
}

//=====================================================================

CString CSettings::GetLanguage()
{
	// Return value
	return m_sLanguage;
}

void CSettings::SetLanguage(CString sValue)
{
	// Set value
	m_sLanguage = sValue;
}

//=====================================================================

CString CSettings::GetRunmode()
{
	// Return value
	return m_sRunmode;
}

void CSettings::SetRunmode(CString sValue)
{
	// Set value
	m_sRunmode = sValue;
}

//=====================================================================

CString CSettings::GetURL()
{
	// Return value
	return m_sURL;
}

void CSettings::SetURL(CString sValue)
{
	// Set value
	m_sURL = sValue;
}

//=====================================================================

CString CSettings::GetUpdateSelector()
{
	// Return value
	return m_sUpdateSelector;
}

void CSettings::SetUpdateSelector(CString sValue)
{
	// Set value
	m_sUpdateSelector = sValue;
}

//=====================================================================

int CSettings::GetTimeout()
{
	// Return value
	return m_iTimeout;
}

void CSettings::SetTimeout(int iValue)
{
	// Set value
	m_iTimeout = iValue;
}

//=====================================================================

CString CSettings::GetAppName()
{
	// Return value
	return m_sAppName;
}

void CSettings::SetAppName(CString sValue)
{
	// Set value
	m_sAppName = sValue;
}

//=====================================================================

CString CSettings::GetAppLocation()
{	
	// Return value
	return m_sAppLocation;
}

void CSettings::SetAppLocation(CString sValue)
{
	// Set value
	m_sAppLocation = sValue;
}

//=====================================================================

CString CSettings::GetAppRoot()
{	
	// Return value
	return m_sAppRoot;
}

void CSettings::SetAppRoot(CString sValue)
{
	// Set value
	m_sAppRoot = sValue;
}

//=====================================================================

CString CSettings::GetAppVersion()
{
	// Return value
	return m_sAppVersion;
}

void CSettings::SetAppVersion(CString sValue)
{
	// Set value
	m_sAppVersion = sValue;
}

//=====================================================================

CString CSettings::GetLogoSmall()
{
	// Return value
	return m_sLogoSmall;
}

void CSettings::SetLogoSmall(CString sValue)
{
	// Set value
	m_sLogoSmall = sValue;
}

//=====================================================================

CString CSettings::GetLogoLarge()
{
	// Return value
	return m_sLogoLarge;
}

void CSettings::SetLogoLarge(CString sValue)
{
	// Set value
	m_sLogoLarge = sValue;
}

//=====================================================================

CString CSettings::GetIcon()
{
	// Return value
	return m_sIcon;
}

void CSettings::SetIcon(CString sValue)
{
	// Set value
	m_sIcon = sValue;
}

//=====================================================================

CString CSettings::GetSelfUpdateLocation()
{
	// Return value
	return m_sSelfUpdateLocation;
}

void CSettings::SetSelfUpdateLocation(CString sValue)
{
	// Set value
	m_sSelfUpdateLocation = sValue;
}

//=====================================================================

bool CSettings::GetCheckConnection()
{
	// Return value
	return m_bCheckConnection;
}

void CSettings::SetCheckConnection(bool bValue)
{
	// Set value
	m_bCheckConnection = bValue;
}

//=====================================================================

CString CSettings::GetFtpUsername()
{
	// Return value
	return m_sFtpUsername;
}

void CSettings::SetFtpUsername(CString sValue)
{
	// Set value
	m_sFtpUsername = sValue;
}

//=====================================================================

CString CSettings::GetFtpPassword()
{
	// Return value
	return m_sFtpPassword;
}

void CSettings::SetFtpPassword(CString sValue)
{
	// Set value
	m_sFtpPassword = sValue;
}

//=====================================================================

CString CSettings::GetHttpUsername()
{
	// Return value
	return m_sHttpUsername;
}

void CSettings::SetHttpUsername(CString sValue)
{
	// Set value
	m_sHttpUsername = sValue;
}

//=====================================================================

CString CSettings::GetHttpPassword()
{
	// Return value
	return m_sHttpPassword;
}

void CSettings::SetHttpPassword(CString sValue)
{
	// Set value
	m_sHttpPassword = sValue;
}

//=====================================================================

CString CSettings::GetProxyUsername()
{
	// Return value
	return m_sProxyUsername;
}

void CSettings::SetProxyUsername(CString sUsername)
{
	// Set value
	m_sProxyUsername = sUsername;
}

//=====================================================================

CString CSettings::GetProxyPassword()
{
	// Return value
	return m_sProxyPassword;
}

void CSettings::SetProxyPassword(CString sPassword)
{
	// Set value
	m_sProxyPassword = sPassword;
}

//=====================================================================

CString CSettings::GetProxyType()
{
	// Return value
	return m_sProxyType;
}

void CSettings::SetProxyType(CString sProxyType)
{
	// Set value
	m_sProxyType = sProxyType;
}

//=====================================================================

CString CSettings::GetProxyManualHttp()
{
	// Return value
	return m_sProxyManualHttp;
}

void CSettings::SetProxyManualHttp(CString sURL)
{
	// Set value
	m_sProxyManualHttp = sURL;
}

//=====================================================================

CString CSettings::GetProxyManualFtp()
{
	// Return value
	return m_sProxyManualFtp;
}

void CSettings::SetProxyManualFtp(CString sURL)
{
	// Set value
	m_sProxyManualFtp = sURL;
}

//=====================================================================

CString CSettings::GetProxyAutoConfigURL()
{
	// Return value
	return m_sProxyAutoConfigURL;
}

void CSettings::SetProxyAutoConfigURL(CString sURL)
{
	// Set value
	m_sProxyAutoConfigURL = sURL;
}

//=====================================================================

bool CSettings::GetRestoreOlderVersion()
{
	// Return value
	return m_bRestoreOlderVersion;
}

void CSettings::SetRestoreOlderVersion(bool bValue)
{
	// Set value
	m_bRestoreOlderVersion = bValue;
}

//=====================================================================

bool CSettings::GetCustomNotify()
{
	// Return value
	return m_bCustomNotify;
}

void CSettings::SetCustomNotify(bool bValue)
{
	// Set value
	m_bCustomNotify = bValue;
}

//=====================================================================

COLORREF CSettings::GetLinkColorLink() 
{
	// Return value
	return m_clrLinkColorLink;
}

void CSettings::SetLinkColorLink(COLORREF clrValue)
{
	// Set value
	m_clrLinkColorLink = clrValue;
}

//=====================================================================

COLORREF CSettings::GetLinkColorHover()
{
	// Return value
	return m_clrLinkColorHover;
}

void CSettings::SetLinkColorHover(COLORREF clrValue)
{
	// Set value
	m_clrLinkColorHover = clrValue;
}

//=====================================================================

CString CSettings::GetNotifySkin()
{
	// Return data
	return m_sNotifySkin;
}

void CSettings::SetNotifySkin(CString sValue)
{
	// Set value
	m_sNotifySkin = sValue;
}

//=====================================================================

COLORREF CSettings::GetNotifyTitleColor()
{
	// Return data
	return m_clrNotifyTitleColor;
}

void CSettings::SetNotifyTitleColor(COLORREF clrValue)
{
	// Set value
	m_clrNotifyTitleColor = clrValue;
}

//=====================================================================

CString	CSettings::GetNotifyTitleFontFace()
{
	// Return data
	return m_sNotifyTitleFontFace;
}

void CSettings::SetNotifyTitleFontFace(CString sValue)
{
	// Set value
	m_sNotifyTitleFontFace = sValue;
}

//=====================================================================

int	CSettings::GetNotifyTitleFontSize()
{
	// Return data
	return m_iNotifyTitleFontSize;
}

void CSettings::SetNotifyTitleFontSize(int iValue)
{
	// Set value
	m_iNotifyTitleFontSize = iValue;
}

//=====================================================================

CString CSettings::GetNotifyTitleFontStyle()
{
	// Return data
	return m_sNotifyTitleFontStyle;
}

void CSettings::SetNotifyTitleFontStyle(CString sValue)
{
	// Set value
	m_sNotifyTitleFontStyle = sValue;
}

//=====================================================================

CRect CSettings::GetNotifyTitleRect()
{
	// Return data
	return m_rectNotifyTitleRect;
}

void CSettings::SetNotifyTitleRect(CRect oValue)
{
	// Set value
	m_rectNotifyTitleRect = oValue;
}

//=====================================================================

CRect CSettings::GetNotifyDescriptionRect()
{
	// Return data
	return m_rectNotifyDescriptionRect;
}

void CSettings::SetNotifyDescriptionRect(CRect oValue)
{
	// Set value
	m_rectNotifyDescriptionRect = oValue;
}

//=====================================================================

CRect CSettings::GetNotifyCloseRect()
{
	// Return data
	return m_rectNotifyCloseRect;
}

void CSettings::SetNotifyCloseRect(CRect oValue)
{
	// Set value
	m_rectNotifyCloseRect = oValue;
}

//=====================================================================
	
int	CSettings::GetNotifySpeedFadeIn()
{
	// Return data
	return m_iNotifySpeedFadeIn;
}

void CSettings::SetNotifySpeedFadeIn(int iValue)
{
	// Set value
	m_iNotifySpeedFadeIn = iValue;
}

//=====================================================================

int	CSettings::GetNotifySpeedFadeOut()
{
	// Return data
	return m_iNotifySpeedFadeOut;
}

void CSettings::SetNotifySpeedFadeOut(int iValue)
{
	// Set value
	m_iNotifySpeedFadeOut = iValue;
}

//=====================================================================

int	CSettings::GetNotifyShowTime()
{
	// Return data
	return m_iNotifyShowTime;
}

void CSettings::SetNotifyShowTime(int iValue)
{
	// Set value
	m_iNotifyShowTime = iValue;
}

//=====================================================================

int	CSettings::GetNotifyTransparency()
{
	// Return data
	return m_iNotifyTransparency;
}

void CSettings::SetNotifyTransparency(int iValue)
{
	// Set value
	m_iNotifyTransparency = iValue;
}

//=====================================================================

COLORREF CSettings::GetNotifyTransparentColor()
{
	// Return data
	return m_clrNotifyTransparentColor;
}

void CSettings::SetNotifyTransparentColor(COLORREF clrValue)
{
	// Set value
	m_clrNotifyTransparentColor = clrValue;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CSettings::ParseIni(CString sFilename)
{
	USES_CONVERSION;

	// Declare variables
	CString sTemp, sLowerTemp, sBuffer;
	int iRed, iGreen, iBlue;
	CFunctions * pFunctions = CFunctions::Instance();
	CPath * pPath = CPath::Instance();

	// Read language from ini-file
	GetPrivateProfileString(_T("UPDATER"), _T("LANGUAGE"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sTemp = StringLower(sTemp.GetBuffer(sTemp.GetLength()));
	m_sLanguage = sTemp;

	// Read runmode from ini-file
	GetPrivateProfileString(_T("UPDATER"), _T("RUNMODE"), MODE_FULL, sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sRunmode = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read small logo from ini-file
	GetPrivateProfileString(_T("UPDATER"), _T("LOGOSMALL"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sLogoSmall = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	m_sLogoSmall = pPath->ConvertRelativeToFullPath(m_sLogoSmall);

	// Read large logo from ini-file
	GetPrivateProfileString(_T("UPDATER"), _T("LOGOLARGE"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sLogoLarge = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	m_sLogoLarge = pPath->ConvertRelativeToFullPath(m_sLogoLarge);

	// Read icon from ini-file
	GetPrivateProfileString(_T("UPDATER"), _T("ICON"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sIcon = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	m_sIcon = pPath->ConvertRelativeToFullPath(m_sIcon);

	// Read custom notifier from ini-file
	GetPrivateProfileString(_T("UPDATER"), _T("CUSTOMNOTIFY"), _T("false"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sTemp = StringLower(sTemp.GetBuffer(sTemp.GetLength()));
	m_bCustomNotify = (sTemp == _T("true")) ? true : false;

	// Read self update location from ini-file
	GetPrivateProfileString(_T("UPDATER"), _T("SELFUPDATE"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sSelfUpdateLocation = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	if (!pFunctions->IsWebAddress(m_sSelfUpdateLocation))
	{
		m_sSelfUpdateLocation = pPath->ConvertRelativeToFullPath(m_sSelfUpdateLocation);
	}

	// Read link color - link from ini-file
	GetPrivateProfileString(_T("UPDATER"), _T("LINKCOLORLINK"), _T("R175,G175,B255"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sBuffer = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	ParseColor(sBuffer, iRed, iGreen, iBlue);
	m_clrLinkColorLink = RGB(iRed, iGreen, iBlue);	

	// Read link color - hover from ini-file
	GetPrivateProfileString(_T("UPDATER"), _T("LINKCOLORHOVER"), _T("R0,G0,B255"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sBuffer = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	ParseColor(sBuffer, iRed, iGreen, iBlue);
	m_clrLinkColorHover = RGB(iRed, iGreen, iBlue);

	// Read URL from ini-file
	GetPrivateProfileString(_T("UPDATEINFO"), _T("URL"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sURL = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	// This item can contain codes
	m_sURL = pPath->ReplacePathConstants(m_sURL);
	if (!pFunctions->IsWebAddress(m_sURL))
	{
		m_sURL = pPath->ConvertRelativeToFullPath(m_sURL);
	}

	// Read Update Selector from ini-file
	GetPrivateProfileString(_T("UPDATEINFO"), _T("UPDATESELECTOR"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sUpdateSelector = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	// This item can contain codes
	m_sUpdateSelector = pPath->ReplacePathConstants(m_sUpdateSelector);
	if (!pFunctions->IsWebAddress(m_sUpdateSelector))
	{
		m_sUpdateSelector = pPath->ConvertRelativeToFullPath(m_sUpdateSelector);
	}

	// Read server timeout from ini-file
	GetPrivateProfileString(_T("UPDATEINFO"), _T("SERVERTIMEOUT"), _T("5000"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_iTimeout = atoi(W2A(sTemp.GetBuffer(sTemp.GetLength())));
	sTemp.ReleaseBuffer();

	// Read app name from in-file
	GetPrivateProfileString(_T("APPLICATION"), _T("NAME"), _T("Unknown app"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sAppName = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read app location from ini-file
	GetPrivateProfileString(_T("APPLICATION"), _T("LOCATION"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sAppLocation = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	// This item can contain codes
	m_sAppLocation = pPath->ReplacePathConstants(m_sAppLocation);
	m_sAppLocation = pPath->ConvertRelativeToFullPath(m_sAppLocation);

	// Read app root from ini-file
	GetPrivateProfileString(_T("APPLICATION"), _T("ROOT"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sAppRoot = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	// This item can contain codes
	if (m_sAppRoot.IsEmpty())
	{
		m_sAppRoot = pPath->ExtractFilePath(m_sAppLocation);
	}
	else
	{
		m_sAppRoot = pPath->ReplacePathConstants(m_sAppRoot);
		m_sAppRoot = pPath->ConvertRelativeToFullPath(m_sAppRoot);
	}

	// Read app version from ini-file
	GetPrivateProfileString(_T("APPLICATION"), _T("VERSION"), _T("0.0.0.0"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sAppVersion = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	// This item can contain codes
	m_sAppVersion = pPath->ReplacePathConstants(m_sAppVersion);
	// Check if there is a file at this point
	if (PathFileExists(m_sAppVersion))
	{
		// Get value of file, and use that as version
		m_sAppVersion = pFunctions->GetFileVersion(m_sAppVersion);
	}

	// Read check connection from ini-file
	GetPrivateProfileString(_T("CONNECTION"), _T("CHECKCONNECTION"), _T("false"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sTemp = StringLower(sTemp.GetBuffer(sTemp.GetLength()));
	m_bCheckConnection = (sTemp == _T("true")) ? true : false;

	// Read ftp username from ini-file
	GetPrivateProfileString(_T("CONNECTION"), _T("FTPUSERNAME"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sFtpUsername = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read ftp password from ini-file
	GetPrivateProfileString(_T("CONNECTION"), _T("FTPPASSWORD"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sFtpPassword = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read http username from ini-file
	GetPrivateProfileString(_T("CONNECTION"), _T("HTTPUSERNAME"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sHttpUsername = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read http password from ini-file
	GetPrivateProfileString(_T("CONNECTION"), _T("HTTPPASSWORD"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sHttpPassword = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read proxy username from ini-file
	GetPrivateProfileString(_T("PROXY"), _T("USERNAME"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sProxyUsername = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read proxy password from ini-file
	GetPrivateProfileString(_T("PROXY"), _T("PASSWORD"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sProxyPassword = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read proxy type from ini-file
	GetPrivateProfileString(_T("PROXY"), _T("TYPE"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sTemp = StringLower(sTemp.GetBuffer(sTemp.GetLength()));
	m_sProxyType = sTemp;

	// Read proxy manual http from ini-file
	GetPrivateProfileString(_T("PROXY"), _T("HTTP"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sProxyManualHttp = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read proxy manual ftp from ini-file
	GetPrivateProfileString(_T("PROXY"), _T("FTP"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sProxyManualFtp = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read proxy auto config url from ini-file
	GetPrivateProfileString(_T("PROXY"), _T("AUTOCONFIGURL"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sProxyAutoConfigURL = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read skin from ini-file
	GetPrivateProfileString(_T("NOTIFY"), _T("SKIN"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sNotifySkin = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read title color
	GetPrivateProfileString(_T("NOTIFY"), _T("TITLECOLOR"), _T("R255,G255,B255"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sBuffer = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	ParseColor(sBuffer, iRed, iGreen, iBlue);
	m_clrNotifyTitleColor = RGB(iRed, iGreen, iBlue);	

	// Read title rect
	GetPrivateProfileString(_T("NOTIFY"), _T("TITLERECT"), _T("0,0,0,0"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sBuffer = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	ParseRect(sBuffer, m_rectNotifyTitleRect);

	// Read description rect
	GetPrivateProfileString(_T("NOTIFY"), _T("DESCRIPTIONRECT"), _T("0,0,0,0"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sBuffer = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	ParseRect(sBuffer, m_rectNotifyDescriptionRect);

	// Read close rect
	GetPrivateProfileString(_T("NOTIFY"), _T("CLOSERECT"), _T("0,0,0,0"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sBuffer = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	ParseRect(sBuffer, m_rectNotifyCloseRect);

	// Read title font face
	GetPrivateProfileString(_T("NOTIFY"), _T("TITLEFONTFACE"), _T("Verdana"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sNotifyTitleFontFace = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read title font size
	GetPrivateProfileString(_T("NOTIFY"), _T("TITLEFONTSIZE"), _T("12"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_iNotifyTitleFontSize = atoi(W2A(sTemp.GetBuffer(sTemp.GetLength())));
	sTemp.ReleaseBuffer();

	// Read title font style
	GetPrivateProfileString(_T("NOTIFY"), _T("TITLEFONTSTYLE"), _T(""), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_sNotifyTitleFontStyle = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();

	// Read speed fade in
	GetPrivateProfileString(_T("NOTIFY"), _T("SPEEDFADEIN"), _T("10"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_iNotifySpeedFadeIn = atoi(W2A(sTemp.GetBuffer(sTemp.GetLength())));
	sTemp.ReleaseBuffer();

	// Read speed fade out
	GetPrivateProfileString(_T("NOTIFY"), _T("SPEEDFADEOUT"), _T("10"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_iNotifySpeedFadeOut = atoi(W2A(sTemp.GetBuffer(sTemp.GetLength())));
	sTemp.ReleaseBuffer();

	// Read show time
	GetPrivateProfileString(_T("NOTIFY"), _T("SHOWTIME"), _T("30"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_iNotifyShowTime = atoi(W2A(sTemp.GetBuffer(sTemp.GetLength())));
	sTemp.ReleaseBuffer();

	// Read transparency
	GetPrivateProfileString(_T("NOTIFY"), _T("TRANSPARENCY"), _T("200"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	m_iNotifyTransparency = atoi(W2A(sTemp.GetBuffer(sTemp.GetLength())));
	sTemp.ReleaseBuffer();

	// Read transparent color
	GetPrivateProfileString(_T("NOTIFY"), _T("TRANSPARENTCOLOR"), _T("R255,G0,B255"), sTemp.GetBuffer(_MAX_PATH), _MAX_PATH, sFilename);
	sBuffer = sTemp.GetBuffer(sTemp.GetLength());
	sTemp.ReleaseBuffer();
	ParseColor(sBuffer, iRed, iGreen, iBlue);
	m_clrNotifyTransparentColor = RGB(iRed, iGreen, iBlue);

	// Replace all instances of %username% and %password% in the update file url
	m_sURL = pFunctions->ReplaceConnectionSettings(m_sURL);
}


//=====================================================================

void CSettings::ParseColor(CString sColor, int & iRed, int & iGreen, int & iBlue)
{	
	USES_CONVERSION;

	// Declare variables
	CString sTemp;
	int iBegin, iEnd, i;

	// Search for red
	iBegin = sColor.Find(_T('R'), 0);
	if (iBegin == -1)
		iRed = 255;
	else
	{
		iEnd = sColor.Find(_T(','), iBegin);
		if (iEnd == -1)
			iEnd = sColor.GetLength();

		// Get part of string
		sTemp = _T("");
		for (i = iBegin + 1; i < iEnd; i++)
			sTemp += sColor.GetAt(i);
		
		// Parse red color
		iRed = atoi(W2A(sTemp));
	}

	// Search for green
	iBegin = sColor.Find(_T('G'), 0);
	if (iBegin == -1)
		iGreen = 255;
	else
	{
		iEnd = sColor.Find(_T(','), iBegin);
		if (iEnd == -1)
			iEnd = sColor.GetLength();

		// Get part of string
		sTemp = _T("");
		for (i = iBegin + 1; i < iEnd; i++)
			sTemp += sColor.GetAt(i);
		
		// Parse green color
		iGreen = atoi(W2A(sTemp));
	}

	// Search for blue
	iBegin = sColor.Find(_T('B'), 0);
	if (iBegin == -1)
		iBlue = 255;
	else
	{
		iEnd = sColor.Find(_T(','), iBegin);
		if (iEnd == -1)
			iEnd = sColor.GetLength();

		// Get part of string
		sTemp = _T("");
		for (i = iBegin + 1; i < iEnd; i++)
			sTemp += sColor.GetAt(i);
		
		// Parse blue color
		iBlue = atoi(W2A(sTemp));
	}
}

//=====================================================================

void CSettings::ParseRect(CString sRect, CRect & rect)
{
	USES_CONVERSION;

	// Declare variables
	CString sTemp;
	int iBegin, iEnd;

	// Set begin values
	iBegin = 0;

	// Search for values
	for (int i = 0; i < 4; i++)
	{
		// Search for next comma
		iEnd = sRect.Find(_T(','), iBegin);

		// Check if we found end
		if (iEnd == -1)
			iEnd = sRect.GetLength();

		// Get part of string
		sTemp = _T("");
		for (int j = iBegin; j < iEnd; j++)
			sTemp += sRect.GetAt(j);

		// Set begin for next search
		iBegin = iEnd + 1;

		// Now set value
		switch (i)
		{
		case 0:
			// Left
			rect.left = atoi(W2A(sTemp));
			break;

		case 1:
			// Top
			rect.top = atoi(W2A(sTemp));
			break;

		case 2:
			// Right
			rect.right = atoi(W2A(sTemp));
			break;

		case 3:
			// Bottom
			rect.bottom = atoi(W2A(sTemp));
			break;
		}	
	}
}

//=====================================================================

void CSettings::SetParameterOptions(CGetOpt & oCommandLineParsing)
{
	USES_CONVERSION;

	// Declare variables
	CFunctions * pFunctions = CFunctions::Instance();
	CPath * pPath = CPath::Instance();
	Param oParamInfo;
	CString sTemp, sLowerTemp;

	// Loop all used parameters
	for (int i = 0; i < oCommandLineParsing.GetParameterResultCount(); i++)
	{
		// Get object
		oParamInfo = oCommandLineParsing.GetParameterResultInfo(i);

		// Check which parameter is used
		if (oParamInfo.sParameter == W2A(PARAMETER_LANGUAGE))
		{
			m_sLanguage.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_UPDATERFILE))
		{
			m_sURL.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_TIMEOUT))
		{
			m_iTimeout = atoi(oParamInfo.sValue.c_str());
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_APPNAME))
		{
			m_sAppName.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_APPVERSION))
		{
			m_sAppVersion.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_APPLOCATION))
		{
			m_sAppLocation.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_APPROOT))
		{
			m_sAppRoot.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_MODE))
		{
			m_sRunmode.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_FTPUSERNAME))
		{
			m_sFtpUsername.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_FTPPASSWORD))
		{
			m_sFtpPassword.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_HTTPUSERNAME))
		{
			m_sHttpUsername.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_HTTPPASSWORD))
		{
			m_sHttpPassword.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_CHECKCONNECTION))
		{
			sTemp.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
			sTemp = StringLower(sTemp);
			m_bCheckConnection = (sTemp == _T("true")) ? true : false;
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_SELFUPDATE))
		{
			m_sSelfUpdateLocation.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_UPDATESELECTOR))
		{
			m_sUpdateSelector.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}

		if (oParamInfo.sParameter == W2A(PARAMETER_SETTINGSFILE))
		{
			m_sSettingsFile.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}
	}
}

//=====================================================================

CString CSettings::GetSettingsFileFromCommandLine(CGetOpt & oCommandLineParsing)
{
	USES_CONVERSION;

	// Declare variables
	CFunctions * pFunctions = CFunctions::Instance();
	Param oParamInfo;
	CString sFile = _T("");

	// Loop all used parameters
	for (int i = 0; i < oCommandLineParsing.GetParameterResultCount(); i++)
	{
		// Get object
		oParamInfo = oCommandLineParsing.GetParameterResultInfo(i);

		// Check if the settings file parameter is used
		if (oParamInfo.sParameter == W2A(PARAMETER_SETTINGSFILE))
		{
			sFile.Format(_T("%s"), A2W(oParamInfo.sValue.c_str()));
		}
	}

	// Return the file
	return sFile;
}

//=====================================================================

void CSettings::InitializeCommandOptions(CGetOpt * pGetOpt)
{
	USES_CONVERSION;

	// Add parameters
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_LANGUAGE), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_UPDATERFILE), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_TIMEOUT), true, PARAMETERTYPE_INTEGER);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_APPNAME), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_APPVERSION), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_APPLOCATION), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_APPROOT), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_MODE), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_FTPUSERNAME), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_FTPPASSWORD), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_HTTPUSERNAME), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_HTTPPASSWORD), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_CHECKCONNECTION), true, PARAMETERTYPE_BOOL);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_SELFUPDATE), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_UPDATESELECTOR), true);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_SETTINGSFILE), true);

	pGetOpt->AddExpectedParameter(W2A(PARAMETER_RESTORE), false);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_PROXYSETTINGS), false);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_SERVICE), false);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_LOG), false);
	pGetOpt->AddExpectedParameter(W2A(PARAMETER_CHECKFORUPDATES), false);	
}