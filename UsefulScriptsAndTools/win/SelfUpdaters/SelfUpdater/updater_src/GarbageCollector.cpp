//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "GarbageCollector.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CGarbageCollector> CGarbageCollector::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CGarbageCollector::CGarbageCollector()
{
	// Initialize
	m_iFileCount = 0;
}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CGarbageCollector * CGarbageCollector::Instance()
{
	if(sm_inst.get() == 0)
	sm_inst = auto_ptr<CGarbageCollector>(new CGarbageCollector);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CGarbageCollector);
	return sm_inst.get();
	*/
}

//=====================================================================

void CGarbageCollector::CleanGarbage()
{
	// Delete all files
	for (int i = 0; i < m_iFileCount; i++)
	{
		DeleteFile(m_arrFiles[i]);
	}

	m_iFileCount = 0;
}

//=====================================================================

void CGarbageCollector::AddFileToRemove(CString sFilename)
{
	// Add file
	m_arrFiles[m_iFileCount] = sFilename;
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************