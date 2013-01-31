// Redefinition protection
#ifndef GARBAGECOLLECTOR_H
#define GARBAGECOLLECTOR_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include <memory>				// Needed for singleton class
using namespace std;

#include "stdafx.h"

//*********************************************************************
// CGarbageCollector
//*********************************************************************

class CGarbageCollector  
{
public:
	// Constructor & destructor
	// No public constructor since this class is a singleton

	// Functions
	static CGarbageCollector * Instance();
	void CleanGarbage();
	void AddFileToRemove(CString sFilename);

private:
	// Constructor & destructor
	CGarbageCollector();			// Private constructor because singleton!
	
	// Functions

	// Variables
	static std::auto_ptr<CGarbageCollector> sm_inst;
	CString		m_arrFiles[MAX_FILES];
	int			m_iFileCount;
};

// End of redefinition protection
#endif // GARBAGECOLLECTOR_H