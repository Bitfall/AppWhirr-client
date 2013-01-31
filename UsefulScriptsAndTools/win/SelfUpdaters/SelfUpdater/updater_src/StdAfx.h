// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

// Redefinition protection
#ifndef STDAFX_H
#define STDAFX_H

#define _WIN32_WINNT	0x0501
#define _WIN32_IE		0x0500

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//*********************************************************************
// CUSTOM DEFINES
//*********************************************************************

// Give more information about memory leaks in debug mode
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif // _DEBUG

// Define manifest, this belongs in resource.h, but VC deletes the
// check every time, so this is a quick solution
#ifndef RT_MANIFEST
#define RT_MANIFEST	24
#endif

//*********************************************************************
// END CUSTOM DEFINES
//*********************************************************************

//*********************************************************************
// CUSTOM INCLUDES
//*********************************************************************

#include "RiskFunctions.h"		// Special risk function

#include <shlwapi.h>			// Needed for PathFileExists function

#include <atlconv.h>			// Conversion macros
#include <afxconv.h>			// Conversion macros

#include "StringFunctions.h"	// Special (custom) string functions

#include "Log.h"				// Log
#include "Constants.h"			// Constants to use in Updater
#include "Path.h"				// All paths that can be used in Updater
#include "Functions.h"			// General functions to use everywhere
#include "Language.h"			// Support for multi-language
#include "Settings.h"			// Settings
#include "UpdateInfo.h"			// Update info
#include "EventInfo.h"			// Event info
#include "RollbackInfo.h"		// Rollback info
#include "RestoreInfo.h"		// Restore info
#include "ActionPoints.h"		// Calculates action points
#include "GarbageCollector.h"	// Garbage collector
#include "InternalData.h"		// Internal storage

// Information dialogs
#include "ConfirmationDlg.h"
#include "ErrorDlg.h"
#include "InformationDlg.h"
#include "WarningDlg.h"

//*********************************************************************
// END CUSTOM INCLUDES
//*********************************************************************

// End of redefinition protection
#endif // STDAFX_H
