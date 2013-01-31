// Extractor Builder.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Extractor Builder.h"
#include "Extractor BuilderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtractorBuilderApp

BEGIN_MESSAGE_MAP(CExtractorBuilderApp, CWinApp)
	//{{AFX_MSG_MAP(CExtractorBuilderApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtractorBuilderApp construction

CExtractorBuilderApp::CExtractorBuilderApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CExtractorBuilderApp object

CExtractorBuilderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CExtractorBuilderApp initialization

BOOL CExtractorBuilderApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CExtractorBuilderDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
