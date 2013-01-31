// ExtractorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Extractor.h"
#include "ExtractorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtractorDlg dialog

CExtractorDlg::CExtractorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExtractorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtractorDlg)
	m_strPath = "";
	m_strTotalStatus = _T("");
	m_strFileStatus = _T("");
	m_strIntroText = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExtractorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtractorDlg)
	DDX_Control(pDX, IDC_PATH, m_Path);
	DDX_Control(pDX, IDC_EXTRACT, m_Extract);
	DDX_Control(pDX, IDC_BROWSE, m_Browse);
	DDX_Control(pDX, IDC_ZIP_PROGRESS, m_ZipProgress);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Text(pDX, IDC_PATH, m_strPath);
	DDX_Text(pDX, IDC_TOTAL_STATUS, m_strTotalStatus);
	DDX_Text(pDX, IDC_FILE_STATUS, m_strFileStatus);
	DDX_Text(pDX, IDC_INTRO_TEXT, m_strIntroText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExtractorDlg, CDialog)
	//{{AFX_MSG_MAP(CExtractorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_EXTRACT, OnExtract)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtractorDlg message handlers

BOOL CExtractorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Set up the progress bars
	m_Progress.SetShowText(TRUE); 
	m_Progress.SetStep(1);
	m_Progress.ShowWindow(SW_HIDE);

	m_ZipProgress.SetShowText(TRUE); 
	m_ZipProgress.SetStep(1);
	m_ZipProgress.ShowWindow(SW_HIDE);

	// Force a read of the TOC to get
	// info about this archive
	int ret = m_Extractor.ReadTOC(m_Extractor.GetThisFileName());
	
	if(ret == INVALID_SIG)
	{
		m_Browse.EnableWindow(FALSE);
		m_Extract.EnableWindow(FALSE);
		m_Path.ShowWindow(SW_HIDE);
		m_strIntroText.LoadString(IDS_NO_DATA_INTRO);
	}
	else
	{
		char buffer[MAX_PATH];
		GetTempPath(MAX_PATH, buffer);
		m_strPath.Format("%s", buffer);
		m_strIntroText.LoadString(IDS_INTRO_TEXT);	
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CExtractorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CExtractorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CExtractorDlg::OnBrowse() 
{
	CDirDialog	dlg;
	dlg.SetWindowTitle("Select Extraction Directory");
	dlg.SetSelDir(m_strPath);
	
	if(dlg.DoModal(this) == IDOK)
	{
		m_strPath = dlg.GetPathname();
		UpdateData(FALSE);
	}
}

void CExtractorDlg::OnExtract() 
{
	m_Progress.SetRange(0, m_Extractor.GetFileCount());
	m_Progress.ShowWindow(SW_SHOW);
	m_ZipProgress.ShowWindow(SW_SHOW);
	m_strFileStatus = "Current File";
	UpdateData(FALSE);
	int ret = m_Extractor.ExtractAll(m_strPath, CExtractorDlg::ExtractCallBack, static_cast<void*>(this));
	m_Progress.ShowWindow(SW_HIDE);
	m_ZipProgress.ShowWindow(SW_HIDE);
	m_strFileStatus = "";
	m_strTotalStatus = "";
	UpdateData(FALSE);

	CString Msg, strTemp;
	switch(ret)
	{
	case SUCCESS:
		strTemp.LoadString(IDS_SUCCESS_FORMATTER);
		Msg.Format(strTemp, m_Extractor.GetFileCount());
		break;
	case INVALID_SIG:
	case NOTHING_TO_DO:
		Msg.LoadString(IDS_NOTHING_TO_DO);
		break;
	case OUTPUT_FILE_ERROR:
		Msg.LoadString(IDS_OUTFILE_ERROR);
		break;
	default:
		Msg.LoadString(IDS_DEFAULT_ERROR);
		break;
	}

	AfxMessageBox(Msg);
}

void CExtractorDlg::ExtractCallBack(int nMsgType, void *ExtractData, void* userData)
{
	CExtractorDlg* pDlg = static_cast<CExtractorDlg*>(userData);
	CSEFileInfo* pData = NULL;
	int nProgress;

	// Wake up the GUI
	MSG msg;
    while(PeekMessage( &msg, NULL, 0, 0, PM_REMOVE)) 
	{
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	switch(nMsgType)
	{
	case SFX_FILE_START:
		pData = static_cast<CSEFileInfo*>(ExtractData);
		pDlg->m_strFileStatus = "Extracting " + pData->GetPathname();
		pDlg->UpdateData(FALSE);
		break;
	case SFX_FILE_END:
		pDlg->m_Progress.StepIt();
		break;
	case SFX_ZIP_INFO:
		memcpy(&nProgress, ExtractData, sizeof(int));
		pDlg->m_ZipProgress.SetPos(nProgress);
		break;
	}
}

void CExtractorDlg::OnAbout() 
{
	MessageBox("Self Extractor by James Spibey created exclusively for The CodeProject.\r\n\r\nContains code by Luca Piergentili, Jean-loup Gailly and Mark Adler", "About Self Extractor");
}
