/********************************************************************
	created:	2005/04/04
	created:	4:4:2005   8:12
	filename: 	ActionPoints.cpp
	file path:	Updater
	file base:	ActionPoints
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "ActionPoints.h"
#include "Unzipper.h"						// Zip support

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

int CActionPoints::CalculateActionPoints(CString sFileOrFolder)
{
	// Declare variables
	WIN32_FIND_DATA FileInfo = {0};
	HANDLE hFile;
	int iSize = 0;
	CFunctions * pFunctions = CFunctions::Instance();

	// Check if file exists
	if (PathFileExists(sFileOrFolder))
	{
		// Get file info
		hFile = FindFirstFile(sFileOrFolder, &FileInfo);

		// Check if object is file or folder
		if (hFile != INVALID_HANDLE_VALUE)
		{
			if ((FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				// It is a Directory
				iSize = pFunctions->GetFolderSize(sFileOrFolder);
			}
			else
			{
				// It is a file
				iSize = pFunctions->GetFileSize(sFileOrFolder);
			}
		}
		else
		{
			// It is a folder
			iSize = pFunctions->GetFolderSize(sFileOrFolder);
		}

		// Close handle
		FindClose(hFile);
	}
	else
	{
		// We can't ready file
		return 0;
	}

	return CalculateActionPoints(iSize);
}

//=====================================================================

int CActionPoints::CalculateActionPoints(CActionBase * pAction)
{
	// Check what action should be counted
	if (pAction->GetType() == ACTION_RUN)
	{
		return ACTIONPOINTS_RUN;
	}

	if (pAction->GetType() == ACTION_REGISTER)
	{
		return ACTIONPOINTS_REGISTER;
	}

	if (pAction->GetType() == ACTION_SETFILEATTR)
	{
		return ACTIONPOINTS_SETFILEATTR;
	}
	
	if (pAction->GetType() == ACTION_DELETE)
	{
		return ACTIONPOINTS_DELETE;
	}

	if (pAction->GetType() == ACTION_REGISTRY)
	{
		return ACTIONPOINTS_REGISTRY;
	}

	if (pAction->GetType() == ACTION_INI)
	{
		return ACTIONPOINTS_INI;
	}

	if (pAction->GetType() == ACTION_COPY)
	{
		// Get object
		CActionCopy * pCopy = (CActionCopy *) pAction;

		// Calculate action points
		return CalculateActionPoints(pCopy->GetOldLocation());
	}

	if (pAction->GetType() == ACTION_UNZIP)
	{
		// Get object
		int iActionPoints = 0;
		CActionUnzip * pUnzip = (CActionUnzip *) pAction;

		// Calculate number of files in zip file
		CUnzipper zipFile;
		zipFile.OpenZip(pUnzip->GetLocation());
		iActionPoints = zipFile.GetFileCount();
		zipFile.CloseZip();

		// Return value
		return iActionPoints;
	}

	// If we get here, 0 actionpoints for action
	return 0;
}

//=====================================================================

int CActionPoints::CalculateActionPoints(CRollbackActionBase * pRollbackAction)
{
	// Check what action should be counted
	if (pRollbackAction->GetType() == ROLLBACKACTION_COPY)
	{
		// Get object
		CRollbackActionCopy * pCopy = (CRollbackActionCopy *) pRollbackAction;

		// Calculate action points
		return CalculateActionPoints(pCopy->GetOldLocation());
	}

	if (pRollbackAction->GetType() == ROLLBACKACTION_DELETE)
	{
		return ACTIONPOINTS_DELETE;
	}

	if (pRollbackAction->GetType() == ROLLBACKACTION_REGISTRY)
	{
		return ACTIONPOINTS_REGISTRY;
	}

	if (pRollbackAction->GetType() == ROLLBACKACTION_UNREGISTER)
	{
		return ACTIONPOINTS_REGISTER;
	}

	// If we get here, 0 actionpoints for action
	return 0;
}

//=====================================================================

int CActionPoints::CalculateActionPoints(CRestoreActionBase * pRestoreAction)
{
	// Check what action should be counted
	if (pRestoreAction->GetType() == RESTOREACTION_COPY)
	{
		// Get object
		CRestoreActionCopy * pCopy = (CRestoreActionCopy *) pRestoreAction;

		// Calculate action points
		return CalculateActionPoints(pCopy->GetOldLocation());
	}

	if (pRestoreAction->GetType() == RESTOREACTION_DELETE)
	{
		return ACTIONPOINTS_DELETE;
	}

	if (pRestoreAction->GetType() == RESTOREACTION_REGISTRY)
	{
		return ACTIONPOINTS_REGISTRY;
	}

	if (pRestoreAction->GetType() == RESTOREACTION_UNREGISTER)
	{
		return ACTIONPOINTS_REGISTER;
	}

	// If we get here, 0 actionpoints for action
	return 0;
}

//=====================================================================

int CActionPoints::CalculateActionPoints(int iFileOrFolderSize)
{
	// Declare variables
	int iSize = iFileOrFolderSize;

	// Calculate actions points now we know the size
	if ((iSize > ACTIONPOINTS_COPYLEVEL0SIZE) && (iSize <= ACTIONPOINTS_COPYLEVEL1SIZE))
	{
		// File is of 1st level
		return ACTIONPOINTS_COPYLEVEL1;
	}

	if ((iSize > ACTIONPOINTS_COPYLEVEL1SIZE) && (iSize <= ACTIONPOINTS_COPYLEVEL2SIZE))
	{
		// File is of 2nd level
		return ACTIONPOINTS_COPYLEVEL2;
	}

	if ((iSize > ACTIONPOINTS_COPYLEVEL2SIZE) && (iSize <= ACTIONPOINTS_COPYLEVEL3SIZE))
	{
		// File is of 3th level
		return ACTIONPOINTS_COPYLEVEL3;
	}

	if ((iSize > ACTIONPOINTS_COPYLEVEL3SIZE) && (iSize <= ACTIONPOINTS_COPYLEVEL4SIZE))
	{
		// File is of 4th level
		return ACTIONPOINTS_COPYLEVEL4;
	}

	if (iSize > ACTIONPOINTS_COPYLEVEL4SIZE)
	{
		// File is of 5th level
		return ACTIONPOINTS_COPYLEVEL5;
	}

	// If we get here, 0 actionpoints for action
	return 0;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************
