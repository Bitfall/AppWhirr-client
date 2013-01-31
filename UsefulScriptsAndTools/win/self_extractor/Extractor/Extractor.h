// Extractor.h : main header file for the EXTRACTOR application
//

#if !defined(AFX_EXTRACTOR_H__3DE9A2F5_4A44_11D3_A8BC_0050043A01C0__INCLUDED_)
#define AFX_EXTRACTOR_H__3DE9A2F5_4A44_11D3_A8BC_0050043A01C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CExtractorApp:
// See Extractor.cpp for the implementation of this class
//

class CExtractorApp : public CWinApp
{
public:
	CExtractorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtractorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CExtractorApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTRACTOR_H__3DE9A2F5_4A44_11D3_A8BC_0050043A01C0__INCLUDED_)
