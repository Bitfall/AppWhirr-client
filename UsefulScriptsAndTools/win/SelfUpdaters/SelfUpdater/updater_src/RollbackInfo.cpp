/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	RollbackInfo.cpp
	file path:	Updater
	file base:	RollbackInfo
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "RollbackInfo.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CRollbackInfo> CRollbackInfo::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CRollbackInfo::CRollbackInfo()
{
	// Set some values
	m_iRollbackActionCount = 0;
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CRollbackInfo * CRollbackInfo::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = auto_ptr<CRollbackInfo>(new CRollbackInfo);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
		sm_inst.reset(new CRollbackInfo);
	return sm_inst.get();
	*/
}

//=====================================================================

CRollbackActionBase * CRollbackInfo::GetAction(int iIndex)
{
	// Return action
	return m_arrRollbackActions[iIndex];
}

//=====================================================================

void CRollbackInfo::AddAction(CRollbackActionBase * pAction)
{
	// Declare variables
	bool bValid = false;

	// Check what type of action to add
	switch (pAction->GetType())
	{
	case ROLLBACKACTION_COPY:
		if (IsValidActionCopy(pAction))
			bValid = true;
		break;

	case ROLLBACKACTION_DELETE:
		if (IsValidActionDelete(pAction))
			bValid = true;
		break;

	case ROLLBACKACTION_REGISTRY:
		// Always perform registry actions
		bValid = true;
		break;

	case ROLLBACKACTION_UNREGISTER:
		// Always perform unregister actions
		bValid = true;
		break;
	}	

	// Add action to array
	if (bValid)
		m_arrRollbackActions[m_iRollbackActionCount++] = pAction;
}

//=====================================================================

void CRollbackInfo::AddActionCopy(CString sOldLocation, CString sNewLocation)
{
	// Set up information
	CRollbackActionBase * pAction = new CRollbackActionCopy(sOldLocation, sNewLocation);

	// Add to array
	AddAction(pAction);
}

//=====================================================================

void CRollbackInfo::AddActionDelete(CString sLocation)
{
	// Set up information
	CRollbackActionBase * pAction = new CRollbackActionDelete(sLocation);
	
	// Add to array
	AddAction(pAction);
}

//=====================================================================

void CRollbackInfo::AddActionRegistry(CString sKey, CString sValue)
{
	// Set up information
	CRollbackActionBase * pAction = new CRollbackActionRegistry(sKey, sValue);
	
	// Add to array
	AddAction(pAction);
}

//=====================================================================

void CRollbackInfo::AddActionUnregister(CString sFile)
{
	// Set up information
	CRollbackActionBase * pAction = new CRollbackActionUnregister(sFile);
	
	// Add to array
	AddAction(pAction);
}

//=====================================================================

int CRollbackInfo::GetActionCount()
{
	// Return value
	return m_iRollbackActionCount;
}

//=====================================================================

void CRollbackInfo::CleanUp()
{
	// Delete all rollback actions
	for (int i = 0; i < m_iRollbackActionCount; i++)
	{
		// Delete object
		delete m_arrRollbackActions[i];
	}
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************

bool CRollbackInfo::IsValidActionCopy(CRollbackActionBase * pAction)
{
	// Declare variables
	CRollbackActionCopy * pTemp;
	bool bValid = true;

	// Get object
	CRollbackActionCopy * pCopy = (CRollbackActionCopy *)pAction;

	// Check if file isn't already in to be copied
	for (int i = 0; i < GetActionCount(); i++)
	{
		if (GetAction(i)->GetType() == ROLLBACKACTION_COPY)
		{
			// Get temp object
			pTemp = (CRollbackActionCopy *)GetAction(i);

			// Check if file isn't already begin copied
			if (pCopy->GetNewLocation() == pTemp->GetNewLocation())
				bValid = false;
		}
	}

	// Return value
	return bValid;
}

//=====================================================================

bool CRollbackInfo::IsValidActionDelete(CRollbackActionBase * pAction)
{
	// Declare variables
	CRollbackActionDelete * pTemp;
	bool bValid = true;
	
	// Get object
	CRollbackActionDelete * pDelete = (CRollbackActionDelete *)pAction;
	
	// Check if file isn't already in to be copied
	for (int i = 0; i < GetActionCount(); i++)
	{
		if (GetAction(i)->GetType() == ROLLBACKACTION_DELETE)
		{
			// Get temp object
			pTemp = (CRollbackActionDelete *)GetAction(i);
			
			// Check if file isn't already being deleted
			if (pDelete->GetLocation() == pTemp->GetLocation())
				bValid = false;
		}
	}
	
	// Return value
	return bValid;
}