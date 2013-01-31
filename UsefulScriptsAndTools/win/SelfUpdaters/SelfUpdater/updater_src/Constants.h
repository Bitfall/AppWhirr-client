/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   9:40
	filename: 	Constants.h
	file path:	Updater
	file base:	Constants
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef CONSTANTS_H
#define CONSTANTS_H

//*********************************************************************
// LANGUAGE FILE VERSION
//*********************************************************************

const CString VERSION_LANGUAGE = _T("0.8.1.5");

//*********************************************************************
// MESSAGES
//*********************************************************************

// WPARAM: event type
// LPARAM: -- not used --
#define WMU_EVENT						WM_APP + 1

// WPARAM: error type
// LPARAM: task type
#define WMU_ERROR						WM_APP + 2

// WPARAM: task type
// LPARAM: -- not used --
#define WMU_TASK_START					WM_APP + 3

// WPARAM: task type
// LPARAM: -- not used --
#define WMU_TASK_COMPLETE				WM_APP + 4

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_UPDATE_GUI					WM_APP + 5

// WPARAM: progress of file in % (0 - 100)
// LPARAM: filename
#define WMU_UPDATE_GUI_FILEPROGRESS		WM_APP + 6

// WPARAM: progress of total process in % (0 - 100)
// LPARAM: -- not used --
#define WMU_UPDATE_GUI_TOTALPROGRESS	WM_APP + 7

// WPARAM: progress of total process in % (0 - 100)
// LPARAM: -- not used --
#define WMU_UPDATE_GUI_PROTECTION		WM_APP + 8

// WPARAM: pointer to parent window
// LPARAM: -- not used --
#define WMU_SETPARENT					WM_APP + 9

// WPARAM: -- unknown --
// LPARAM: -- unknown --
#define WMU_TRAYICON_NOTIFY				WM_APP + 10

// WPARAM: protection task
// LPARAM: task state
#define WMU_THREADMESSAGE				WM_APP + 11

// WPARAM: action points currently handled 
// LPARAM: total action points
#define WMU_COPY_STATUS					WM_APP + 12
#define WMU_DOWNLOAD_STATUS				WM_APP + 12

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_COPY_COMPLETE				WM_APP + 13
#define WMU_DOWNLOAD_COMPLETE			WM_APP + 13

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_COPY_START					WM_APP + 14
#define WMU_DOWNLOAD_START				WM_APP + 14

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_DOWNLOAD_STOP				WM_APP + 15

// WPARAM: InstallStatus 
// LPARAM: -- not used --
#define WMU_INSTALL_STATUS				WM_APP + 16

// WPARAM: install part
// LPARAM: -- not used --
#define WMU_INSTALL_COMPLETE			WM_APP + 17

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_INSTALL_START				WM_APP + 18

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_INSTALLFILES_COMPLETE		WM_APP + 19
#define WMU_INSTALLINI_COMPLETE			WM_APP + 19
#define WMU_INSTALLREGISTRY_COMPLETE	WM_APP + 19

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_INSTALLACTION_COMPLETE		WM_APP + 20

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_ROLLBACK_COMPLETE			WM_APP + 21
#define WMU_RESTORE_COMPLETE			WM_APP + 21

// WPARAM: actions currently performed
// LPARAM: actions to perform
#define WMU_BACKUP_STATUS				WM_APP + 22

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_BACKUP_COMPLETE				WM_APP + 23

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_SHOWABOUT					WM_APP + 24

// WPARAM: warning type
// LPARAM: task type
#define WMU_WARNING						WM_APP + 25

// WPARAM: -- not used --
// LPARAM: -- not used --
#define WMU_WARNING_COMPLETE			WM_APP + 26

// WPARAM: question type
// LPARAM: pointer to object with data -> READ ONLY
#define WMU_QUESTION					WM_APP + 27

// WPARAM: question type
// LPARAM: answer type
#define WMU_QUESTION_ANSWER				WM_APP + 28

// WPARAM: title
// LPARAM: description
#define WMU_SHOWNOTIFIER				WM_APP + 29

// WPARAM: UI-type
// LPARAM: -- not used --
#define WMU_SWITCHUI					WM_APP + 30

//*********************************************************************
// UPDATER RETURN VALUES
//*********************************************************************

const int RETURN_ERROR			= -1;
const int RETURN_OK				= 0;
const int RETURN_NEWVERSION		= 1;
const int RETURN_NONEWVERSION	= 2;

//*********************************************************************
// LANGUAGE TYPES
//*********************************************************************

const CString LANGUAGETYPE_USER = _T("user");
const CString LANGUAGETYPE_AUTO = _T("");

//*********************************************************************
// PROTECTION TASK & STATE
//*********************************************************************

const int PROTECTION_TASK_LICENSE	= 0;
const int PROTECTION_TASK_HASH		= 1;

const int PROTECTION_STATE_TOCHECK	= 0;
const int PROTECTION_STATE_CHECKING = 1;
const int PROTECTION_STATE_OK		= 2;
const int PROTECTION_STATE_NOTOK	= 3;

//*********************************************************************
// EVENTS
//*********************************************************************

const int EVENT_ONNEWVERSION		= 0;
const int EVENT_ONNONEWVERSION		= 1;
const int EVENT_BEFOREHTMLMESSAGE	= 2;
const int EVENT_AFTERHTMLMESSAGE	= 3;
const int EVENT_BEFORELICENSE		= 4;
const int EVENT_AFTERLICENSE		= 5;
const int EVENT_BEFOREPROTECTION	= 6;
const int EVENT_AFTERPROTECTION		= 7;
const int EVENT_BEFORESELECTUPDATE	= 8;
const int EVENT_AFTERSELECTUPDATE	= 9;
const int EVENT_BEFOREDOWNLOAD		= 10;
const int EVENT_AFTERDOWNLOAD		= 11;
const int EVENT_BEFOREINSTALL		= 12;
const int EVENT_AFTERINSTALL		= 13;
const int EVENT_BEFOREROLLBACK		= 14;
const int EVENT_AFTERROLLBACK		= 15;
const int EVENT_ONCLOSE				= 16;
const int EVENT_COUNT				= 17;

//*********************************************************************
// ERROR
//*********************************************************************

const int ERROR_UNKNOWN			= 0;	// Unknown error
const int ERROR_NOCONNECTION	= 1;	// There is no connection
const int ERROR_SERVER			= 2;	// Can't connect to server
const int ERROR_NODISKSPACE		= 3;	// Not enough diskspace
const int ERROR_FILENOTFOUND	= 4;	// File not found/available
const int ERROR_COPYFILE		= 5;	// Can't copy file
const int ERROR_DOWNLOADFILE	= 5;	// Can't download file
const int ERROR_DELETEFILE		= 6;	// Can't delete file
const int ERROR_RUNFILE			= 7;	// Can't execute file
const int ERROR_CLOSEAPP		= 8;	// Can't close application
const int ERROR_FILECORRUPT		= 9;	// File is corrupt
const int ERROR_NONEWVERSION	= 10;	// No new version available
const int ERROR_WRITEFILE		= 11;	// Can't write to file
const int ERROR_UNZIPFILE		= 12;	// Can't unzip file
const int ERROR_PROTECTION		= 13;	// Protection check failed

//*********************************************************************
// WARNINGS
//*********************************************************************

const int WARNING_REGISTER = 0;		// Can't register dll or ocx

//*********************************************************************
// QUESTIONS
//*********************************************************************

const int QUESTION_STOPUPDATING		= 0;
const int QUESTION_DELETEFILE		= 1;
const int QUESTION_CLOSEAPPLICATION = 2;
const int QUESTION_REGISTERFILE		= 3;

//*********************************************************************
// QUESTION RESULTS
//*********************************************************************

const int QUESTIONRESULT_YES	= 0;
const int QUESTIONRESULT_NO		= 1;

//*********************************************************************
// TASKS
//*********************************************************************

const int TASK_NEXTTASK		= -1;
const int TASK_INIT			= 0;
const int TASK_CHECKVERSION = 1;
const int TASK_HTMLMESSAGE	= 2;
const int TASK_LICENSE		= 3;
const int TASK_PROTECTION	= 4;
const int TASK_SELECTUPDATE = 5;
const int TASK_DOWNLOAD		= 6;
const int TASK_INSTALL		= 7;
const int TASK_FINISH		= 8;
const int TASK_ROLLBACK		= 9;

// Restore task does not belong to update process
const int TASK_RESTORE		= 100;

//*********************************************************************
// ACTIONS
//*********************************************************************

const CString ACTION_DOWNLOAD		= _T("download");
const CString ACTION_COPY			= _T("copy");
const CString ACTION_DELETE			= _T("delete");
const CString ACTION_RUN			= _T("run");
const CString ACTION_REGISTRY		= _T("registry");
const CString ACTION_INI			= _T("ini");
const CString ACTION_UNZIP			= _T("unzip");
const CString ACTION_SETFILEATTR	= _T("setfileattr");
const CString ACTION_REGISTER		= _T("register");

//*********************************************************************
// EVENTACTIONS
//*********************************************************************

const CString EVENTACTION_CLOSE				= _T("close");
const CString EVENTACTION_SETMODE			= _T("setmode");
const CString EVENTACTION_RUN				= _T("run");
const CString EVENTACTION_SKIPNEXTTASK		= _T("skipnexttask");
const CString EVENTACTION_STARTNEXTTASK		= _T("startnexttask");
const CString EVENTACTION_CLOSEAPPLICATION	= _T("closeapplication");
const CString EVENTACTION_SHOWNOTIFIER		= _T("shownotifier");

//*********************************************************************
// ROLLBACKACTIONS
//*********************************************************************

const int ROLLBACKACTION_COPY		= 0;
const int ROLLBACKACTION_DELETE		= 1;
const int ROLLBACKACTION_REGISTRY	= 2;
const int ROLLBACKACTION_UNREGISTER = 3;

//*********************************************************************
// RESTOREACTIONS
//*********************************************************************

const int RESTOREACTION_COPY		= 0;
const int RESTOREACTION_DELETE		= 1;
const int RESTOREACTION_REGISTRY	= 2;
const int RESTOREACTION_UNREGISTER	= 3;

//*********************************************************************
// MODES
//*********************************************************************

const CString MODE_FULL			= _T("full");
const CString MODE_SILENT		= _T("silent");
const CString MODE_VERYSILENT	= _T("verysilent");
const CString MODE_HIDDEN		= _T("hidden");

//*********************************************************************
// CLOSE OPTIONS
//*********************************************************************

const int CLOSE_TRUE				= 0;
const int CLOSE_FALSE				= 1;
const int CLOSE_RESTART				= 2;
const int CLOSE_RESTARTSYSTEM		= 3;
const int CLOSE_RESTARTSYSTEMAPP	= 4;
const int CLOSE_USER				= 5;

//*********************************************************************
// LOGO CONSTANTS
//*********************************************************************

const int LOGO_SMALL_HEIGHT		= 58;
const int LOGO_SMALL_WIDTH		= 435;
const int LOGO_LARGE_HEIGHT		= 314;
const int LOGO_LARGE_WIDTH		= 164;

//*********************************************************************
// MAXIMUMS
//*********************************************************************

const int MAX_FEATURES			= 5;
const int MAX_FILES				= 1024;
const int MAX_ACTIONS			= 1024;
const int MAX_INI				= 1024;
const int MAX_REGISTRY			= 1024;
const int MAX_DOWNLOADLOCATIONS = 100;
const int MAX_POPUPMENUITEMS	= 10;
const int MAX_DOWNLOADS			= 2048;
const int MAX_CUSTOMCONSTANTS	= 1024;
const int MAX_SECTIONS			= 1024;
const int MAX_SHORTCUTS			= 1024;
const int MAX_PROTECTIONHASHES	= 1024;

//*********************************************************************
// REGISTRY TYPES
//*********************************************************************

const CString REGTYPE_STRING	= _T("string");
const CString REGTYPE_BINARY	= _T("binary");
const CString REGTYPE_DWORD		= _T("dword");

//*********************************************************************
// THREAD ACTIONS
//*********************************************************************

const int THREAD_START			= 0;
const int THREAD_PAUSE			= 1;
const int THREAD_RESUME			= 2;
const int THREAD_STARTNEXTTASK	= 3;
const int THREAD_SKIPNEXTTASK	= 4;
const int THREAD_FINISH			= 5;
const int THREAD_CANCEL			= 6;
const int THREAD_STARTTASK		= 7;

//*********************************************************************
// PARAMETERS
//*********************************************************************

const CString PARAMETER_LANGUAGE			= _T("-language");
const CString PARAMETER_UPDATERFILE			= _T("-updatefile");
const CString PARAMETER_TIMEOUT				= _T("-timeout");
const CString PARAMETER_APPNAME				= _T("-appname");
const CString PARAMETER_APPVERSION			= _T("-appversion");
const CString PARAMETER_APPLOCATION			= _T("-applocation");
const CString PARAMETER_APPROOT				= _T("-approot");
const CString PARAMETER_MODE				= _T("-mode");
const CString PARAMETER_FTPUSERNAME			= _T("-ftpusername");
const CString PARAMETER_FTPPASSWORD			= _T("-ftppassword");
const CString PARAMETER_HTTPUSERNAME		= _T("-httpusername");
const CString PARAMETER_HTTPPASSWORD		= _T("-httppassword");
const CString PARAMETER_CHECKCONNECTION		= _T("-checkconnection");
const CString PARAMETER_SELFUPDATE			= _T("-selfupdatelocation");
const CString PARAMETER_UPDATESELECTOR		= _T("-updateselector");

const CString PARAMETER_RESTORE				= _T("-restore");
const CString PARAMETER_PROXYSETTINGS		= _T("-proxysettings");
const CString PARAMETER_SERVICE				= _T("-service");
const CString PARAMETER_LOG					= _T("-log");
const CString PARAMETER_CHECKFORUPDATES		= _T("-checkforupdates");
const CString PARAMETER_SETTINGSFILE		= _T("-settingsfile");

//*********************************************************************
// PATHS
//*********************************************************************

const CString PATH_APP						= _T("%app%");
const CString PATH_APPROOT					= _T("%approot%");
const CString PATH_UPDATER					= _T("%updater%");
const CString PATH_UPDATERTEMP				= _T("%updatertemp%");
const CString PATH_TEMP						= _T("%temp%");
const CString PATH_PROGRAMFILES				= _T("%programfiles%");
const CString PATH_SYSTEM					= _T("%system%");
const CString PATH_APPDATA					= _T("%appdata%");
const CString PATH_USERPROFILE				= _T("%userprofile%");
const CString PATH_COMMON_APPDATA			= _T("%commonappdata%");
const CString PATH_QUICKLAUNCHCURRENTUSER	= _T("%quicklaunchcurrentuser%");
const CString PATH_MENUSTARTCURRENTUSER		= _T("%menustartcurrentuser%");
const CString PATH_DESKTOPCURRENTUSER		= _T("%desktopcurrentuser%");
const CString PATH_QUICKLAUNCHALLUSERS		= _T("%quicklaunchallusers%");
const CString PATH_MENUSTARTALLUSERS		= _T("%menustartallusers%");
const CString PATH_DESKTOPALLUSERS			= _T("%desktopallusers%");
const CString PATH_WINDOWSDIRECTORY			= _T("%windowsdirectory%");
const CString PATH_UPDATESERVERPATH			= _T("%updateserverpath%");
const CString PATH_UPDATESERVERFILE			= _T("%updateserverfile%");

//*********************************************************************
// CHECK TYPES
//*********************************************************************

const CString CHECK_VERSION		= _T("version");
const CString CHECK_DATE		= _T("date");
const CString CHECK_DATETIME	= _T("datetime");
const CString CHECK_HASH		= _T("hash");

//*********************************************************************
// CHECK SETTINGS
//*********************************************************************

const CString CONNECTION_FTPUSERNAME	= _T("%ftpusername%");
const CString CONNECTION_FTPPASSWORD	= _T("%ftppassword%");
const CString CONNECTION_HTTPUSERNAME	= _T("%httpusername%");
const CString CONNECTION_HTTPPASSWORD	= _T("%httppassword%");

//*********************************************************************
// BUFFERS
//*********************************************************************

const int BUFFER_COPYFILE		= 1024;
const int BUFFER_DOWNLOADFILE	= 1024;

//*********************************************************************
// TIMERS
//*********************************************************************

const int TIMER_COUNTDOWN = 1;

//*********************************************************************
// INSTALL PARTS
//*********************************************************************

const int INSTALL_FILES		= 0;
const int INSTALL_INI		= 1;
const int INSTALL_REGISTRY	= 2;
const int INSTALL_MAX		= 3;

//*********************************************************************
// ACTIONPOINTS
//*********************************************************************

const int ACTIONPOINTS_REGISTRY			= 1;
const int ACTIONPOINTS_INI				= 1;
const int ACTIONPOINTS_DELETE			= 1;
const int ACTIONPOINTS_RUN				= 1;
const int ACTIONPOINTS_REGISTER			= 1;
const int ACTIONPOINTS_SETFILEATTR		= 1;
const int ACTIONPOINTS_COPYLEVEL0SIZE	= 0;		// 0 mb = 0 kb = 0 bytes
const int ACTIONPOINTS_COPYLEVEL1		= 10;		// File or folder 0 - 1 mb
const int ACTIONPOINTS_COPYLEVEL1SIZE	= 1048576;	// 1 mb = 1024 kb = 1048576 bytes
const int ACTIONPOINTS_COPYLEVEL2		= 20;		// File or folder 1 - 2,5 mb
const int ACTIONPOINTS_COPYLEVEL2SIZE	= 2621440;	// 2,5 mb = 2560 kb = 2621440 bytes
const int ACTIONPOINTS_COPYLEVEL3		= 30;		// File or folder 2,5 - 5 mb
const int ACTIONPOINTS_COPYLEVEL3SIZE	= 5242880;	// 5 mb = 5120 kb = 5242880 bytes
const int ACTIONPOINTS_COPYLEVEL4		= 40;		// File or folder 5 - 10 mb
const int ACTIONPOINTS_COPYLEVEL4SIZE	= 10485760;	// 10 mb = 10240 kb = 10485760 bytes
const int ACTIONPOINTS_COPYLEVEL5		= 50;		// File of folder 10+ mb

//*********************************************************************
// DEVELOPMENT CONSTANTS
//*********************************************************************

const CString UPDATER_WEBADDRESS		= _T("www.gvhsoftware.org");
const CString UPDATER_DEVELOPER			= _T("Geert van Horrik");
const CString UPDATER_YEARS				= _T("2004 - 2006");
const CString UPDATER_UPDATELOCATION	= _T("http://www.gvhsoftware.org/bin/updater/updater_selfupdate.xml");

//*********************************************************************
// COMPARE VERSION RETURN CODES
//*********************************************************************

const int COMPAREVERSION_EQUAL			= 0;
const int COMPAREVERSION_FIRSTLARGER	= 1;
const int COMPAREVERSION_FIRSTSMALLER	= 2;

//*********************************************************************
// FILE NAMES
//*********************************************************************

const CString FILENAME_RESTORE		= _T("restoreinfo.xml");
const CString FILENAME_UPDATEINFO	= _T("updateinfo.xml");
const CString FILENAME_SECTIONS		= _T("sections_%name%.xml");
const CString FILENAME_SETTINGS		= _T("settings.ini");

//*********************************************************************
// FILE ATTRIBUTES CONSTANTS
//*********************************************************************

const CString FILEATTR_READONLY		= _T("readonly");
const CString FILEATTR_SYSTEM		= _T("system");
const CString FILEATTR_HIDDEN		= _T("hidden");
const CString FILEATTR_ARCHIVE		= _T("archive");
const CString FILEATTR_COMPRESSED	= _T("compressed");

//*********************************************************************
// PROXY RETRY COUNTS
//*********************************************************************

const int PROXY_RETRYCOUNT = 3;

//*********************************************************************
// PROXY TYPES
//*********************************************************************

const CString PROXY_TYPE_DIRECTCONNECTION	= _T("directconnection");
const CString PROXY_TYPE_AUTODETECT			= _T("autodetect");
const CString PROXY_TYPE_MANUAL				= _T("manual");
const CString PROXY_TYPE_AUTOCONFIGURL		= _T("autoconfigurl");

//*********************************************************************
// ENCRYPTION/DECRYPTION KEYS
//*********************************************************************

const char CRYPT_KEY[]		= "updater_encrypti";
const char CRYPT_PREFIX[]	= "enc";

//*********************************************************************
// HTTP VERBS
//*********************************************************************

const HTTP_VERB_POST = 0;
const HTTP_VERB_GET = 1;
const HTTP_VERB_HEAD = 2;
const HTTP_VERB_PUT = 3;
const HTTP_VERB_LINK = 4;
const HTTP_VERB_DELETE = 5;
const HTTP_VERB_UNLINK = 6;

//*********************************************************************
// STRUCTS
//*********************************************************************

struct InstallStatus
{
	CString sTitle;				// File title
	int iInstallPart;			// Install part (see INSTALL_ constants)
	int iFileActionsPerformed;	// File actions performed
	int iFileActions;			// Total actions of file to perform
	int iTotalActionsPerformed;	// Total actions performed of install part
	int iTotalActions;			// Total actions of install part to perform
};

struct ProxyData
{
	DWORD	dwAccessType;
	CString sProxyName;
	CString sProxyBypass;
};

struct SectionInfo
{
	CString sName;
	CString sVersion;
};

struct FeatureData
{
	CString sText;
	CString sHyperlink;
};

struct ProtectionHashData
{
	CString sFilename;
	CString sHash;
};

struct MessageStore
{
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
};

//*********************************************************************
// MACROS
//*********************************************************************

// Convert a constant to a string
#define SYMBOL2STRING(sym)		#sym


// End of redefinition protection
#endif // CONSTANTS_H