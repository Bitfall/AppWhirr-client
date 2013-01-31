// Extractor BuilderDlg.cpp : implementation file
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
// CExtractorBuilderDlg dialog

CExtractorBuilderDlg::CExtractorBuilderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExtractorBuilderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtractorBuilderDlg)
	m_strExePath = _T("");
	m_strExtractorPath = _T("");
	m_strCurrFile = _T("");
	m_strZipInfo = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExtractorBuilderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtractorBuilderDlg)
	DDX_Control(pDX, IDC_ZIP_PROGRESS, m_ZipProgress);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Text(pDX, IDC_EXEPATH, m_strExePath);
	DDX_Text(pDX, IDC_EXTRACTOR_PATH, m_strExtractorPath);
	DDX_Text(pDX, IDC_CURR_FILE, m_strCurrFile);
	DDX_Text(pDX, IDC_ZIP_INFO, m_strZipInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExtractorBuilderDlg, CDialog)
	//{{AFX_MSG_MAP(CExtractorBuilderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSEEXE, OnBrowseexe)
	ON_BN_CLICKED(IDC_CREATE, OnCreate)
	ON_BN_CLICKED(IDC_READ, OnRead)
	ON_BN_CLICKED(IDC_BROWSE_EXTRACTOR, OnBrowseExtractor)
	ON_BN_CLICKED(IDC_ADD_DIR, OnAddDir)
	ON_BN_CLICKED(IDC_ADD_FILES, OnAddFiles)
	ON_BN_CLICKED(IDC_CLEAR_LIST, OnClearList)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtractorBuilderDlg message handlers

BOOL CExtractorBuilderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_strWorkingDir = GetWorkingDir(TRUE);

	ListView_SetExtendedListViewStyle(m_List, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_List.InsertColumn(1, "Name", LVCFMT_LEFT, 200);
	m_List.InsertColumn(2, "Size (bytes)", LVCFMT_LEFT, 200);
	
	// Init the progress controls
	m_Progress.SetStep(1);
	m_Progress.ShowWindow(SW_HIDE);

	m_ZipProgress.SetStep(1);
	m_ZipProgress.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CExtractorBuilderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}


void CExtractorBuilderDlg::OnPaint() 
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

HCURSOR CExtractorBuilderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


CString CExtractorBuilderDlg::GetWorkingDir(BOOL withBackSlash)
{
	char FullName[MAX_PATH+1];
	GetModuleFileName(NULL,FullName,MAX_PATH);
	CString Path(FullName);
	int num = Path.ReverseFind('\\');
	if(num != -1)
	{
		if(withBackSlash)
			num +=1;

		Path = Path.Left(num);
		return Path;
	}
	else
		return "";
}

void CExtractorBuilderDlg::OnBrowseExtractor() 
{
	CFileDialog dlg(TRUE, NULL, NULL, 
					OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
					"Exe Files (*.exe)|*.exe|All Files (*.*)|*.*||", this);
 
	dlg.m_ofn.lpstrTitle = "Select the Extractor Executable";
 
	if(dlg.DoModal() == IDOK)
	{
		m_strExtractorPath = dlg.GetPathName();
		UpdateData(FALSE);
	}	
}

void CExtractorBuilderDlg::OnBrowseexe() 
{
	CFileDialog dlg(FALSE,"*.exe", "Untitled.exe", 
					OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
					"Self-extracting Executables (*.exe)|*.exe|All Files (*.*)|*.*||", this);
 
	dlg.m_ofn.lpstrTitle = "Save As Self-Extracting Archive";

	if (dlg.DoModal() == IDOK)
	{
		m_strExePath = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CExtractorBuilderDlg::OnCreate() 
{
	if(m_strExePath.IsEmpty())
	{
		MessageBox("You need to select an output Exe before you can create the SFX");
		return;
	}

	m_Progress.SetRange(0, m_Extractor.GetFileCount());
	m_Progress.SetPos(0);
	m_ZipProgress.SetPos(0);
	m_Progress.ShowWindow(SW_SHOW);
	m_ZipProgress.ShowWindow(SW_SHOW);
	m_strZipInfo = "Compressing... ";

	UpdateData(FALSE);

	int ret = 0;
	if(m_strExtractorPath.IsEmpty())
		ret = m_Extractor.Create(IDR_SFX_EXE, m_strExePath, CExtractorBuilderDlg::AddCallBack, (void*)this);
	else
		ret = m_Extractor.Create(m_strExtractorPath, m_strExePath, CExtractorBuilderDlg::AddCallBack, (void*)this);

	m_strCurrFile = "Extraction Complete";
	m_strZipInfo = "";
	UpdateData(FALSE);
	
	switch(ret)
	{
	case NOTHING_TO_DO:
		MessageBox("You must add some files to the extractor");
		break;
	case SUCCESS:
		MessageBox("Archive Created Successfully");
		break;
	default:
		MessageBox("Unknown Error");
		break;
	}
}

void CExtractorBuilderDlg::OnRead() 
{
	CFileDialog dlg(TRUE, NULL, NULL, 
					OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
					"Self Extracting Files (*.exe)|*.exe|All Files (*.*)|*.*||", this);
 
	dlg.m_ofn.lpstrTitle = "Select the Self Extracting file";
 
	if(dlg.DoModal() == IDOK)
	{
		int ret = m_Extractor.ReadTOC(dlg.GetPathName());
		if(ret == SUCCESS)
		{
			UpdateList();
		}
		else if(ret == INVALID_SIG)
		{
			CString Temp;
			Temp.LoadString(IDS_INVALID_FORMAT);
			MessageBox(Temp);
		}
	}
}

void CExtractorBuilderDlg::OnAddDir() 
{
	CDirDialog dlg;
	dlg.SetWindowTitle("Select Directory");
	
	if(dlg.DoModal() == IDOK)
	{
		CString Path = dlg.GetPathname();
		Path += "\\*.*";

		CFileFind	finder;

		BOOL bContinue = finder.FindFile(Path);

		while(bContinue)
		{
			bContinue = finder.FindNextFile();
			m_Extractor.AddFile(finder.GetFilePath());
		}

		UpdateList();
	}
}

void CExtractorBuilderDlg::OnAddFiles() 
{
	CFileDialog dlg(TRUE, NULL, NULL, 
					OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT, 
					"All Files (*.*)|*.*||", this);
 
	char cbBuffer[10240];
	dlg.m_ofn.nMaxFile = 10240;
	dlg.m_ofn.lpstrFile = cbBuffer;
	dlg.m_ofn.lpstrTitle = "Add Files";

	if(dlg.DoModal() == IDOK)
	{
		POSITION pos;
		for (pos = dlg.GetStartPosition(); pos != NULL;)
		{
			CString Path = dlg.GetNextPathName(pos);
			m_Extractor.AddFile(Path);
		}
	}
	UpdateList();
}

void CExtractorBuilderDlg::UpdateList()
{
	m_List.DeleteAllItems();

	for(int i = 0; i < m_Extractor.GetFileCount(); i++)
	{
		m_List.InsertItem(i, m_Extractor.GetItem(i).GetFilename());
		CString Temp;
		Temp.Format("%d", m_Extractor.GetItem(i).GetFileSize());
		m_List.SetItemText(i, 1, Temp);
	}

	UpdateData(FALSE);
}

void CExtractorBuilderDlg::AddCallBack(int nMsgType, void* CallbackData, void *userData)
{
	CExtractorBuilderDlg* pDlg = static_cast<CExtractorBuilderDlg*>(userData);
	CSEFileInfo* pData = NULL;
	int nProgress;

	// Wake up the GUI
	MSG msg;
    while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
	{
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

	switch(nMsgType)
	{
	case SFX_FILE_START:
		pData = static_cast<CSEFileInfo*>(CallbackData);
		pDlg->m_strCurrFile = "Adding " + pData->GetPathname();
		pDlg->UpdateData(FALSE);
		break;
	case SFX_FILE_END:
		pDlg->m_Progress.StepIt();
		break;
	case SFX_ZIP_INFO:
		memcpy(&nProgress, CallbackData, sizeof(int));
		pDlg->m_ZipProgress.SetPos(nProgress);
		break;
	}
}

void CExtractorBuilderDlg::OnClearList() 
{
	m_Extractor.Reset();
	UpdateList();
}

void CExtractorBuilderDlg::OnAbout() 
{
		MessageBox("Extractor Builder by James Spibey created exclusively for The CodeProject.\r\n\r\nContains code by Luca Piergentili, Jean-loup Gailly and Mark Adler", "About Extractor Builder");	
}
