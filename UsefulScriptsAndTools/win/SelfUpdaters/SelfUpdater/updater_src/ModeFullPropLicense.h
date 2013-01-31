/********************************************************************
	created:	2005/06/20
	created:	20:6:2005   12:40
	filename: 	ModeFullPropLicense.h
	file path:	Updater
	file base:	ModeFullPropLicense
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef MODEFULLPROPLICENSE_H
#define MODEFULLPROPLICENSE_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "EditLicense.h"

//*********************************************************************
// CModeFullPropLicense dialog
//*********************************************************************

class CModeFullPropLicense : public CPropertyPageEx
{
	DECLARE_DYNAMIC(CModeFullPropLicense)

public:
	// Constructor & destructor
	CModeFullPropLicense();
	virtual ~CModeFullPropLicense();

	// Functions

	// Dialog Data
	enum { IDD = IDD_FULL_PROPPAGE_LICENSE };

private:
	// Functions
	virtual void DoDataExchange(CDataExchange * pDX);    // DDX/DDV support
	BOOL	OnInitDialog();
	BOOL	OnSetActive();
	LRESULT OnWizardNext();
	void	OnAgree();
	void	OnNotAgree();

	// Variables
	CButton			 m_rdLicenseAgree;
	CButton			 m_rdLicenseNotAgree;
	CEditLicense	 m_txtLicense;
	CFunctions	   * m_pFunctions;
	CSettings	   * m_pSettings;
	CUpdateInfo    * m_pUpdateInfo;
	CLanguage	   * m_pLanguage;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // MODEFULLPROPLICENSE_H
