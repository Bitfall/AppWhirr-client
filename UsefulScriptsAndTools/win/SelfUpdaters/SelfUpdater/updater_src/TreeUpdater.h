/********************************************************************
	created:	2005/06/25
	created:	25:6:2005   11:56
	filename: 	TreeUpdater.h
	file path:	Updater
	file base:	TreeUpdater
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef TREEUPDATER_H
#define TREEUPDATER_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CTreeUpdater
//*********************************************************************

class CTreeUpdater : public CTreeCtrl
{
public:
	// Constructor & destructor
	CTreeUpdater();
	virtual ~CTreeUpdater();

	// Functions
	HTREEITEM InsertCheckBox(CString sText, BOOL bCheck = TRUE,
		BOOL bEnable = TRUE, HTREEITEM hParent = TVI_ROOT,
		HTREEITEM hInsertAfter = TVI_LAST);
	void SetCheck(HTREEITEM hItem, BOOL bCheck = TRUE);
	void SetEnable(HTREEITEM hItem, BOOL bEnable = TRUE);
	void SetKeepAllExpanded(BOOL bKeepAllExpanded = TRUE);

private:
	// Functions
	void HandleCheckbox(HTREEITEM hTreeItem, BOOL bHandleChilds,
		BOOL bFirstCall = TRUE);
	void HandleExpansion(HTREEITEM hTreeItem);
	int  CountJumpsToRoot(HTREEITEM hTreeItem);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnCustomDraw(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg BOOL OnDeleteItem(NMHDR * pNMHDR, LRESULT * pResult);

	// Variables
	BOOL m_bKeepAllExpanded;

	// Message map
	DECLARE_MESSAGE_MAP()
};

//*********************************************************************
// CTreeItemData
//*********************************************************************

class CTreeItemData
{
public:
	// Constructor & destructor
	CTreeItemData()
	{
		m_bEnabled = TRUE;
		m_bChecked = TRUE;
	}

	// Functions

	// Variables
	BOOL m_bEnabled;
	BOOL m_bChecked;

private:
	// Functions

	// Variables
};

// End of redefinition protection
#endif //TREEUPDATER_H
