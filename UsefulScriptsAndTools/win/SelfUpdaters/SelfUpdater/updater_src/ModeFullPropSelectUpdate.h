/********************************************************************
	created:	2005/06/20
	created:	20:6:2005   12:40
	filename: 	ModeFullPropSelectUpdate.h
	file path:	Updater
	file base:	ModeFullPropSelectUpdate
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLPROPSELECTUPDATE_H
#define MODEFULLPROPSELECTUPDATE_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "TreeUpdater.h"
#include "HyperlinkLabel.h"

//*********************************************************************
// CModeFullPropSelectUpdate dialog
//*********************************************************************

class CModeFullPropSelectUpdate : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CModeFullPropSelectUpdate)

public:
	// Constructor & destructor
	CModeFullPropSelectUpdate();
	virtual ~CModeFullPropSelectUpdate();

	// Functions
	
	// Dialog Data
	enum { IDD = IDD_FULL_PROPPAGE_SELECTUPDATE };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange * pDX);    // DDX/DDV support
	BOOL	OnInitDialog();
	BOOL	OnSetActive();
	LRESULT OnWizardNext();
	void	AddTreeItem(CString sName, CString sParent, HTREEITEM hTreeParent = TVI_ROOT);
	void	CheckTreeItemCheck(HTREEITEM hTreeItem);
	afx_msg void OnTreeSelectUpdate(NMHDR * pNMHDR, LRESULT * pResult);

	// Variables
	CStatic			 m_lblExplanation;
	CStatic			 m_lblDescriptionTitle;
	CStatic			 m_lblDescription;
	CHyperlinkLabel  m_lblMoreInfo;
	CTreeUpdater	 m_treeUpdateSelection;
	CFont			 m_fntDescriptionTitle;
	CFunctions	   * m_pFunctions;
	CSettings	   * m_pSettings;
	CUpdateInfo    * m_pUpdateInfo;
	CLanguage	   * m_pLanguage;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODEFULLPROPSELECTUPDATE_H
