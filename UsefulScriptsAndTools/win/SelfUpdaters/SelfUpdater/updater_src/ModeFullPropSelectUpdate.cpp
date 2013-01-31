/********************************************************************
	created:	2005/06/20
	created:	20:06:2005   12:16
	filename: 	ModeFullPropSelectUpdate.cpp
	file path:	Updater
	file base:	ModeFullPropSelectUpdate
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// Includes
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ModeFullPropSelectUpdate.h"
#include "ModeFullSheet.h"
#include "Sections.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CModeFullPropSelectUpdate, CPropertyPageEx)
	ON_NOTIFY(NM_CLICK, treeUpdateSelection, OnTreeSelectUpdate)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

IMPLEMENT_DYNAMIC(CModeFullPropSelectUpdate, CPropertyPageEx)

//=====================================================================

CModeFullPropSelectUpdate::CModeFullPropSelectUpdate()
	: CPropertyPageEx(CModeFullPropSelectUpdate::IDD)
{
	// Init singleton classes
	m_pFunctions = CFunctions::Instance();
	m_pSettings = CSettings::Instance();
	m_pUpdateInfo = CUpdateInfo::Instance();
	m_pLanguage = CLanguage::Instance();

	// Set title
	m_strHeaderTitle = m_pLanguage->GetString(IDS_FULL_SELECTUPDATE_TITLE);
	m_strHeaderSubTitle = m_pLanguage->GetString(IDS_FULL_SELECTUPDATE_SUBTITLE);
}

//=====================================================================

CModeFullPropSelectUpdate::~CModeFullPropSelectUpdate()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CModeFullPropSelectUpdate::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageEx::DoDataExchange(pDX);
	DDX_Control(pDX, lblExplanation, m_lblExplanation);
	DDX_Control(pDX, lblDescriptionTitle, m_lblDescriptionTitle);
	DDX_Control(pDX, lblDescription, m_lblDescription);
	DDX_Control(pDX, lblMoreInfo, m_lblMoreInfo);
	DDX_Control(pDX, treeUpdateSelection, m_treeUpdateSelection);
}

//=====================================================================

BOOL CModeFullPropSelectUpdate::OnInitDialog()
{
	// Call original function
	CPropertyPageEx::OnInitDialog();

	// Set style for the tree control
	m_treeUpdateSelection.ModifyStyle(TVS_CHECKBOXES, 0);
	m_treeUpdateSelection.ModifyStyle(0, TVS_CHECKBOXES);

	// Return TRUE unless you want to set focus to a control
	return TRUE;
}

//=====================================================================

BOOL CModeFullPropSelectUpdate::OnSetActive()
{
	// Declare variables
	CSections * pSections = CSections::Instance();

	// Call original function
	CPropertyPageEx::OnSetActive();
	
	// Load language strings
	CString sTemp;
	sTemp.Format(m_pLanguage->GetString(IDS_FULL_SELECTUPDATE_EXPLANATION), m_pSettings->GetAppName());
	m_lblExplanation.SetWindowText(sTemp);

	// Clear the description fields
	m_lblDescriptionTitle.SetWindowText(_T(""));
	m_lblDescription.SetWindowText(_T(""));

	// Disable the horizontal scrollbar
	m_treeUpdateSelection.EnableScrollBarCtrl(1, FALSE);

	// Don't update the screen
	UpdateData(FALSE);

	// Clear tree
	m_treeUpdateSelection.DeleteAllItems();

	// Add all root tree items
	for (int i = 0; i < m_pUpdateInfo->GetSectionCount(); i++)
	{
		if (m_pUpdateInfo->GetSectionData(i)->GetParent().IsEmpty())
		{
			AddTreeItem(m_pUpdateInfo->GetSectionData(i)->GetName(), _T(""));
		}
	}

	// Always keep everything expanded
	m_treeUpdateSelection.SetKeepAllExpanded();

	// Update the screen again
	UpdateData(TRUE);

	// If we have no items in the treeview, quickly go to next task
	if (m_treeUpdateSelection.GetCount() <= 0)
	{
		// Get parent
		CModeFullSheet * pParent = (CModeFullSheet *)GetParent();
		
		// Start next task
		pParent->PressButton(PSBTN_NEXT);
	}

	// Set description title font
	m_fntDescriptionTitle.DeleteObject();
	m_fntDescriptionTitle.CreateFont(
		15,								// nHeight
		0,								// nWidth
		0,								// nEscapement
		0,								// nOrientation
		FW_BOLD,						// nWeight
		FALSE,							// bItalic
		FALSE,							// bUnderline
		0,								// cStrikeOut
		DEFAULT_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,				// nOutPrecision
		CLIP_DEFAULT_PRECIS,			// nClipPrecision
		PROOF_QUALITY,					// nQuality
		DEFAULT_PITCH | FF_DONTCARE,	// nPitchAndFamily
		_T(""));
	m_lblDescriptionTitle.SetFont(&m_fntDescriptionTitle);

	// Autosize hyperlink field
	m_lblMoreInfo.SetAutoResize(true);
	m_lblMoreInfo.SetWindowText(m_pLanguage->GetString(IDS_FULL_SELECTUPDATE_MOREINFO));

	// Set first item as default text
	if (m_pUpdateInfo->GetSectionCount() > 0)
	{
		m_lblDescriptionTitle.SetWindowText(m_pUpdateInfo->GetSectionData(0)->GetTitle());
		m_lblDescription.SetWindowText(m_pUpdateInfo->GetSectionData(0)->GetDescription());

		// Do we have a hyperlink?
		if (m_pUpdateInfo->GetSectionData(0)->GetURL().IsEmpty())
		{
			// No, hide the window
			m_lblMoreInfo.ShowWindow(SW_HIDE);
		}
		else
		{
			// Yes, show it and set hyperlink
			m_lblMoreInfo.ShowWindow(SW_SHOW);
			m_lblMoreInfo.SetHyperlink(m_pUpdateInfo->GetSectionData(0)->GetURL());
		}
	}

	// Return value
	return TRUE;
}

//=====================================================================

LRESULT CModeFullPropSelectUpdate::OnWizardNext()
{
	// Declare variables
	CString sTitle;
	HTREEITEM hTreeItem;
	
	// Get root item
	hTreeItem = m_treeUpdateSelection.GetChildItem(TVI_ROOT);
	
	// Retrieve childs
	while (hTreeItem)
	{
		// Check the state
		CheckTreeItemCheck(hTreeItem);

		// Get next item
		hTreeItem = m_treeUpdateSelection.GetNextItem(hTreeItem, TVGN_NEXT);
	}
	
	// Get parent
	CModeFullSheet * pParent = (CModeFullSheet *)GetParent();
	
	// Start next task
	pParent->PressButton(PSBTN_NEXT);
	
	// Don't change page yourself
	return -1;
}

//=====================================================================

void CModeFullPropSelectUpdate::AddTreeItem(CString sName, CString sParent, HTREEITEM hTreeParent /* = TVI_ROOT */)
{
	// Declare variables
	CSections * pSections = CSections::Instance();
	HTREEITEM hTreeItem;
	int iSection, i;

	// Calculate which section number we are adding
	for (i = 0; i < m_pUpdateInfo->GetSectionCount(); i++)
	{
		if (m_pUpdateInfo->GetSectionData(i)->GetName() == sName)
			iSection = i;
	}

	// Add item
	if (m_pFunctions->CompareVersions(m_pUpdateInfo->GetSectionData(iSection)->GetVersion(),
		pSections->GetSectionVersion(m_pUpdateInfo->GetSectionData(iSection)->GetName())) == COMPAREVERSION_FIRSTLARGER)
	{
		// Insert data
		hTreeItem = m_treeUpdateSelection.InsertCheckBox(m_pUpdateInfo->GetSectionData(iSection)->GetTitle(),
			m_pUpdateInfo->GetSectionData(iSection)->GetCheck(),
			m_pUpdateInfo->GetSectionData(iSection)->GetEnable(),
			hTreeParent);

		// Expand the item
		m_treeUpdateSelection.Expand(hTreeParent, TVE_EXPAND);
	}	

	// Add all childs
	/*for (i = 0; i < m_pUpdateInfo->GetSectionCount(); i++)
	{
		if (m_pUpdateInfo->GetSectionData(i)->GetParent() == sName)
			AddTreeItem(m_pUpdateInfo->GetSectionData(i)->GetName(), m_pUpdateInfo->GetSectionData(iSection)->GetName(), hTreeItem);
	}*/
}

//=====================================================================

void CModeFullPropSelectUpdate::CheckTreeItemCheck(HTREEITEM hTreeItem)
{
	// Declare variables
	CString sTitle;
	HTREEITEM hTreeChild;

	// Get title
	sTitle = m_treeUpdateSelection.GetItemText(hTreeItem);

	// Get right item
	for (int i = 0; i < m_pUpdateInfo->GetSectionCount(); i++)
	{
		if (m_pUpdateInfo->GetSectionData(i)->GetTitle() == sTitle)
		{
			// Set state
			if (m_treeUpdateSelection.GetCheck(hTreeItem))
				m_pUpdateInfo->GetSectionData(i)->SetSelectedByUser(true);
			else
				m_pUpdateInfo->GetSectionData(i)->SetSelectedByUser(false);
		}
	}

	// Now loop all childs
	hTreeChild = m_treeUpdateSelection.GetNextItem(hTreeItem, TVGN_CHILD);
	while (hTreeChild)
	{
		// Check the item
		CheckTreeItemCheck(hTreeChild);

		// Get next child
		hTreeChild = m_treeUpdateSelection.GetNextItem(hTreeChild, TVGN_NEXT);
	}

}

//=====================================================================

void CModeFullPropSelectUpdate::OnTreeSelectUpdate(NMHDR * pNMHDR, LRESULT * pResult)
{
	// Declare variables
	HTREEITEM hTreeItem;
	CString sTitle, sDescription;
	CPoint point;
	UINT uFlags;
	int iSection;

	// Set result
	*pResult = 0;

	// Get cursor position
	GetCursorPos(&point);
	m_treeUpdateSelection.ScreenToClient(&point);

	// Get selected item
	hTreeItem = m_treeUpdateSelection.HitTest(point, &uFlags);

	// If we haven't selected anything
	if (!hTreeItem)
	{
		// Get selected item
		hTreeItem = m_treeUpdateSelection.GetSelectedItem();

		if (!hTreeItem)
		{
			// Clear description fields
			m_lblDescriptionTitle.SetWindowText(_T(""));
			m_lblDescription.SetWindowText(_T(""));
			return;
		}
	}

	// Get title
	sTitle = m_treeUpdateSelection.GetItemText(hTreeItem);

	// Get the description
	for (int i = 0; i < m_pUpdateInfo->GetSectionCount(); i++)
	{
		if (m_pUpdateInfo->GetSectionData(i)->GetTitle() == sTitle)
		{
			// Get description
			sDescription = m_pUpdateInfo->GetSectionData(i)->GetDescription();

			// Get section index
			iSection = i;
		}
	}
	
	// Set the description fields
	m_lblDescriptionTitle.SetWindowText(sTitle);
	m_lblDescription.SetWindowText(sDescription);

	// Do we have a hyperlink?
	if (m_pUpdateInfo->GetSectionData(iSection)->GetURL().IsEmpty())
	{
		// No, hide the window
		m_lblMoreInfo.ShowWindow(SW_HIDE);
	}
	else
	{
		// Yes, show it and set hyperlink
		m_lblMoreInfo.ShowWindow(SW_SHOW);
		m_lblMoreInfo.SetHyperlink(m_pUpdateInfo->GetSectionData(iSection)->GetURL());
	}

	// Select the item
	m_treeUpdateSelection.SelectItem(hTreeItem);
}