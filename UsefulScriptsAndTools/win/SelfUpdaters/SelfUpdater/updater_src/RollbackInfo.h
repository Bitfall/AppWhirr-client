/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	RollbackInfo.h
	file path:	Updater
	file base:	RollbackInfo
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef ROLLBACKINFO_H
#define ROLLBACKINFO_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

#include "RollbackActions.h"

//*********************************************************************
// CRollbackInfo
//*********************************************************************

class CRollbackInfo  
{
public:
	// Functions
	static CRollbackInfo * Instance();
	CRollbackActionBase  * GetAction(int iIndex);
	void				   AddAction(CRollbackActionBase * pAction);
	void				   AddActionCopy(CString sOldLocation, CString sNewLocation);
	void				   AddActionDelete(CString sLocation);
	void				   AddActionRegistry(CString sKey, CString sValue);
	void				   AddActionUnregister(CString sFile);
	int					   GetActionCount();
	void				   CleanUp();

	// Variables

private:
	// Constructor & destructor
	CRollbackInfo();			// Private constructor because singleton!

	// Functions
	bool IsValidActionCopy(CRollbackActionBase * pAction);
	bool IsValidActionDelete(CRollbackActionBase * pAction);

	// Variables
	static std::auto_ptr<CRollbackInfo> sm_inst;
	int					  m_iRollbackActionCount;
	CRollbackActionBase * m_arrRollbackActions[MAX_ACTIONS];

};

// End of redefinition protection
#endif // ROLLBACKINFO_H