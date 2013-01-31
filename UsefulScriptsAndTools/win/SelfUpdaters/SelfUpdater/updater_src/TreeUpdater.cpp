/********************************************************************
	created:	2005/06/25
	created:	25:6:2005   11:56
	filename: 	TreeUpdater.cpp
	file path:	Updater
	file base:	TreeUpdater
	file ext:	cpp
	author:		
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "TreeUpdater.h"

//*********************************************************************
// MESSAGE MAP
//*********************************************************************

BEGIN_MESSAGE_MAP(CTreeUpdater, CTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT_EX(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT_EX(TVN_DELETEITEM, OnDeleteItem)
END_MESSAGE_MAP()

//*********************************************************************
// CONSTRUCTOR & DESTUCTOR
//*********************************************************************

CTreeUpdater::CTreeUpdater()
{
	// By default, user should be able to collaps items
	m_bKeepAllExpanded = FALSE;
}

//=====================================================================

CTreeUpdater::~CTreeUpdater()
{
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

HTREEITEM CTreeUpdater::InsertCheckBox(CString sText, BOOL bCheck /* = TRUE */,
									   BOOL bEnable /* = TRUE */,
									   HTREEITEM hParent /* = TVI_ROOT */,
									   HTREEITEM hInsertAfter /* = TVI_LAST */)
{
	// Declare variables
	HTREEITEM hItem;
	CTreeItemData * pItemData;

	// Insert item
	hItem = InsertItem(sText, hParent, hInsertAfter);

	// Set item data
	pItemData = new CTreeItemData;
	pItemData->m_bEnabled = bEnable;
	pItemData->m_bChecked = bCheck;
	SetItemData(hItem, (DWORD_PTR) pItemData);

	// Set checkbox
	SetCheck(hItem, bCheck);

	// Return the item
	return hItem;
}

//=====================================================================

void CTreeUpdater::SetCheck(HTREEITEM hItem, BOOL bCheck /* = TRUE */)
{
	// Declare variables
	CTreeItemData * pItemData = NULL;

	// If we have a valid tree item
	if (hItem)
	{
		// Get item data
		pItemData = (CTreeItemData *) GetItemData(hItem);
	}

	// If the item data is valid
	if (pItemData)
	{
		// Set check
		pItemData->m_bChecked = bCheck;

		// Call original function
		CTreeCtrl::SetCheck(hItem, bCheck);
	}
}

//=====================================================================

void CTreeUpdater::SetEnable(HTREEITEM hItem, BOOL bEnable /* = TRUE */)
{
	// Declare variables
	CTreeItemData * pItemData = NULL;
	CRect rectItem;

	// If we have a valid tree item
	if (hItem)
	{
		// Get item data
		pItemData = (CTreeItemData *) GetItemData(hItem);
	}

	// If the item data is valid
	if (pItemData)
	{
		// Set enable
		pItemData->m_bEnabled = bEnable;

		// Repaint item
		GetItemRect(hItem, &rectItem, FALSE);
		InvalidateRect(&rectItem);
	}
}

//=====================================================================

void CTreeUpdater::SetKeepAllExpanded(BOOL bKeepAllExpanded /* = TRUE */)
{
	// Set value
	m_bKeepAllExpanded = bKeepAllExpanded;

	// Set style on or off
	if (bKeepAllExpanded)
	{
		// Remove button style
		ModifyStyle(TVS_HASBUTTONS, 0);
	}
	else
	{
		// Add button style
		ModifyStyle(0, TVS_HASBUTTONS);
	}
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

void CTreeUpdater::HandleCheckbox(HTREEITEM hTreeItem, BOOL bHandleChilds,
								  BOOL bFirstCall /* = TRUE */)
{
	// Declare variables
	HTREEITEM hTreeChild, hTreeParent;
	BOOL bChecked;
	CTreeItemData * pItemData;

	// Get item data
	pItemData = (CTreeItemData *) GetItemData(hTreeItem);

	// Get current state
	bChecked = GetCheck(hTreeItem);

	// If the item is disabled, change back to old check state
	if (!pItemData->m_bEnabled && bFirstCall)
	{
		// The user clicked this item, set back the state
		SetCheck(hTreeItem, !bChecked);

		// Stop the execution of the function
		return;
	}

	// If the item has children, then iterate through them and for all items
    // which are check boxes set their state to be the same as the parent
	hTreeChild = GetNextItem(hTreeItem, TVGN_CHILD);
	while (hTreeChild && bHandleChilds)
	{
		// Check the child		
		SetCheck(hTreeChild, bChecked);

		// Handle check for parent too
		HandleCheckbox(hTreeChild, TRUE, FALSE);

		// Move on to the next item
		hTreeChild = GetNextItem(hTreeChild, TVGN_NEXT);
	}

	// Get the parent item and if it is a checkbox, then iterate through 
	// all its children and if all the checkboxes are checked, then also
	// automatically check the parent. If no checkboxes are checked, then
	// also automatically uncheck the parent.
	hTreeParent = GetNextItem(hTreeItem, TVGN_PARENT);
	if (hTreeParent && bChecked)
	{
		// Check parent
		SetCheck(hTreeParent, bChecked);
		
		// Handle check for parent too
		HandleCheckbox(hTreeParent, FALSE, FALSE);
	}
}

//=====================================================================

void CTreeUpdater::HandleExpansion(HTREEITEM hTreeItem)
{
	// Declare variables
	UINT nTempFlags;

	// Should the items always be expanded?
	if (hTreeItem && m_bKeepAllExpanded)
	{
		// Yes, expand the item
		nTempFlags = GetItemState(hTreeItem, TVIF_STATE);

		if (!(nTempFlags & TVIS_EXPANDED))
			Expand(hTreeItem, TVE_EXPAND);
	}

	// Repaint
	RedrawWindow();
}

//=====================================================================

int CTreeUpdater::CountJumpsToRoot(HTREEITEM hTreeItem)
{
	// Declare variables
	int iCount = 0;
	HTREEITEM hTreeParent = hTreeItem;

	// Search for the parent until TVI_ROOT
	while ((hTreeParent != TVI_ROOT) && hTreeParent)
	{
		// Get parent again
		hTreeParent = GetNextItem(hTreeParent, TVGN_PARENT);

		// We found another one
		if (hTreeParent)
			iCount++;
	}

	// Return the count
	return iCount;
}

//=====================================================================

void CTreeUpdater::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Declare variables
	HTREEITEM hTreeItem;
	UINT nTempFlags = 0;

	// Call original function
	CTreeCtrl::OnLButtonDown(nTempFlags, point);

	// Get selected item
	hTreeItem = HitTest(point, &nTempFlags);

	// Check if the button is clicked
	if ((nTempFlags & TVHT_ONITEMSTATEICON) && hTreeItem)
	{
		// Handle checkbox
		HandleCheckbox(hTreeItem, TRUE);
	}

	// Should the items always be expanded?
	if (hTreeItem && m_bKeepAllExpanded)
	{
		// Yes, expand the item
		nTempFlags = GetItemState(hTreeItem, TVIF_STATE);

		if (!(nTempFlags & TVIS_EXPANDED))
			Expand(hTreeItem, TVE_EXPAND);
	}
}

//=====================================================================

void CTreeUpdater::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// Declare variables
	HTREEITEM hTreeItem;
	UINT nTempFlags = 0;

	// Call original function
	CTreeCtrl::OnLButtonDblClk(nTempFlags, point);

	// Get selected item
	hTreeItem = HitTest(point, &nTempFlags);

	// Handle expansion
	HandleExpansion(hTreeItem);
}

//=====================================================================

void CTreeUpdater::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Declare variables
	HTREEITEM hTreeItem;

	// Get selected item
	hTreeItem = GetSelectedItem();

	// Check which char is used
	switch (nChar)
	{
	case VK_SPACE:
		// Handle the checkbox
		HandleCheckbox(hTreeItem, TRUE);
		break;

	default:
		// Pass on to the parent since we didn't handle it
		CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
		break;
	}
}

//=====================================================================

void CTreeUpdater::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Declare variables
	HTREEITEM hTreeItem;

	// Get selected item
	hTreeItem = GetSelectedItem();

	// Check which char is used
	switch (nChar)
	{
    case VK_LEFT:
		// Call original function
		CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);

		// Handle expansion
		HandleExpansion(hTreeItem);
		break;

	default:
		// Pass on to the parent since we didn't handle it
		CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
		break;
	}
}

//=====================================================================

BOOL CTreeUpdater::OnCustomDraw(NMHDR * pNMHDR, LRESULT * pResult)
{
	// Declare variables
	NMTVCUSTOMDRAW * pCustomDraw = (NMTVCUSTOMDRAW *) pNMHDR;
	HTREEITEM hItem = NULL;
	CTreeItemData * pItemData = NULL;
	UINT nTempFlags;
	CRect rectCheckbox;

	// Check the draw stage
	switch (pCustomDraw->nmcd.dwDrawStage) 
	{
	case CDDS_PREPAINT:
		// Get notification on a per item basis
		*pResult = CDRF_NOTIFYPOSTPAINT;
		break;

	case CDDS_POSTPAINT:
		// Paint all items visible
		hItem = GetNextItem(TVI_ROOT, TVGN_FIRSTVISIBLE);

		// As long as we can find visible items
		while (hItem)
		{
			// Get item data
			pItemData = (CTreeItemData *) GetItemData(hItem);

			// Get item rect
			GetItemRect(hItem, &rectCheckbox, FALSE);

			// Calculate checkbox position
			rectCheckbox.left = rectCheckbox.left + (19 * CountJumpsToRoot(hItem)) + 5;
			rectCheckbox.right = rectCheckbox.left + 14;

			// Did we retrieve a valid item data?
			if (pItemData)
			{
				// Set style for button
				nTempFlags = DFCS_BUTTONCHECK;

				// Is the item enabled?
				nTempFlags |= pItemData->m_bEnabled ? 0 : DFCS_INACTIVE;

				// Is the item checked?
				nTempFlags |= GetCheck(hItem) ? DFCS_CHECKED : 0;

				// Is the item disabled?
				if (!pItemData->m_bEnabled)
				{
					DrawFrameControl(pCustomDraw->nmcd.hdc, &rectCheckbox,
						DFC_BUTTON, nTempFlags);
				}
			}

			// Find next visible item
			hItem = GetNextItem(hItem, TVGN_NEXTVISIBLE);
		}
		
		*pResult = CDRF_DODEFAULT;
		break;

	}

	// Return TRUE
	return TRUE;	
}

//=====================================================================

BOOL CTreeUpdater::OnDeleteItem(NMHDR * pNMHDR, LRESULT * pResult)
{
	// Declare variables
	NM_TREEVIEW * pNMTreeView = (NM_TREEVIEW *)pNMHDR;
	CTreeItemData * pItemData;
	
	// Set result
	*pResult = 0;

	// Get item data
	pItemData = (CTreeItemData *) GetItemData(pNMTreeView->itemOld.hItem);

	// Clean up memory
	if (pItemData)
		delete pItemData;

	return FALSE;
}