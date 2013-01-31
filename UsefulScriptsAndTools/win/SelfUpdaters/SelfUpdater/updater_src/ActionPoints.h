/********************************************************************
	created:	2005/04/04
	created:	4:4:2005   8:13
	filename: 	ActionPoints.h
	file path:	Updater
	file base:	ActionPoints
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef ACTIONPOINTS_H
#define ACTIONPOINTS_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "StdAfx.h"

//*********************************************************************
// CActionPoints
//*********************************************************************

class CActionPoints  
{
public:
	// Functions
	static int CalculateActionPoints(CString sFileOrFolder);
	static int CalculateActionPoints(CActionBase * pAction);
	static int CalculateActionPoints(CRollbackActionBase * pRollbackAction);
	static int CalculateActionPoints(CRestoreActionBase * pRestoreAction);
	static int CalculateActionPoints(int iFileOrFolderSize);

private:
	// Functions

	// Variables
};

// End of redefinition protection
#endif // ACTIONPOINTS_H