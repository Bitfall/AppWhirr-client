/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	RestoreInfo.h
	file path:	Updater
	file base:	RestoreInfo
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef RESTOREINFO_H
#define RESTOREINFO_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

#include "RestoreActions.h"

//*********************************************************************
// CRestoreInfo
//*********************************************************************

class CRestoreInfo  
{
public:
	// Functions
	static CRestoreInfo * Instance();
	CRestoreActionBase  * GetAction(int iIndex);
	void				  AddAction(CRestoreActionBase * pAction);
	void				  AddActionCopy(CString sOldLocation, CString sNewLocation);
	void				  AddActionDelete(CString sLocation);
	void				  AddActionRegistry(CString sKey, CString sValue);
	void				  AddActionUnregister(CString sFile);
	int					  GetActionCount();
	void				  SetVersion(CString sVersion);
	CString				  GetVersion();
	void				  AddSection(CString sName, CString sVersion);
	int					  GetSectionCount();
	SectionInfo			  GetSection(int iIndex);
	bool				  RetrieveInformationFromRollback();
	bool				  WriteToFile(CString sLocation);
	bool				  ReadFromFile(CString sLocation);
	void				  CleanUp();

	// Variables

private:
	// Constructor & destructor
	CRestoreInfo();			// Private constructor because singleton!

	// Functions
	bool	IsValidActionCopy(CRestoreActionBase * pAction);
	bool	IsValidActionDelete(CRestoreActionBase * pAction);
	CString XML_to_CString(CString sFilename);

	// Variables
	static std::auto_ptr<CRestoreInfo> sm_inst;
	CString				 m_sVersion;
	int					 m_iSectionCount;
	SectionInfo			 m_arrSections[MAX_SECTIONS];
	int				     m_iRestoreActionCount;
	CRestoreActionBase * m_arrRestoreActions[MAX_ACTIONS];

};

// End of redefinition protection
#endif // RESTOREINFO_H
