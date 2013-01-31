/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:29
	filename: 	UserDataCollection.cpp
	file path:	Updater
	file base:	UserDataCollection
	file ext:	cpp
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"
#include "Updater.h"
#include "UserDataCollection.h"

//*********************************************************************
// INITIALISATION
//*********************************************************************

std::auto_ptr<CUserDataCollection> CUserDataCollection::sm_inst;

//*********************************************************************
// CONSTRUCTOR & DESTRUCTOR
//*********************************************************************

CUserDataCollection::CUserDataCollection()
{

}

//*********************************************************************
// PUBLIC FUNCTIONS
//*********************************************************************

CUserDataCollection * CUserDataCollection::Instance()
{
	if(sm_inst.get() == 0)
	sm_inst = auto_ptr<CUserDataCollection>(new CUserDataCollection);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CUserDataCollection);
	return sm_inst.get();
	*/
}

//*********************************************************************
// PRIVATE FUNCTIONS
//*********************************************************************