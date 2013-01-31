/*************************************************************************************
*
*	File:		SEFileInfo.cpp
*	Version:	1.12
*
*	Author:		James Spibey
*	Date:		22/08/2002
*	E-mail:		james@spibey.com
*
*	Implementation of the CSEFileInfo class
*
*	You are free to use, distribute or modify this code
*	as long as this header is not removed or modified.
*
*
*************************************************************************************/
#include "stdafx.h"
#include "SEFileInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/*******************************************************************************
* 
* Function: CSEFileInfo::CSEFileInfo
* 
* Description: 
*	 Default Constructor 
* 
* Parameters: 
*    None
* 
* Return: 
*    None
*******************************************************************************/
CSEFileInfo::CSEFileInfo()
{
	Reset();
}

/*******************************************************************************
* 
* Function: CSEFileInfo::~CSEFileInfo
* 
* Description: 
*	Destructor 
* 
* Parameters: 
*    None
* 
* Return: 
*    None
*******************************************************************************/
CSEFileInfo::~CSEFileInfo()
{
}

/*******************************************************************************
* 
* Function: CSEFileInfo::SetData
* 
* Description: 
*Set the data members for the class 
* 
* Parameters: 
*    CString Filename: Filename of file to gather data about 
* 
* Return: 
*    BOOL : Success or Failure
*******************************************************************************/
BOOL CSEFileInfo::SetData(CString Filename)
{
	CFile f;

	//Open the file
	if(!f.Open(Filename, CFile::modeRead))
		return FALSE;
	
	//Get the length in bytes
	m_nSize = f.GetLength();
	f.Close();

	m_strPathname = Filename;
	m_strFilename = Filename.Mid(Filename.ReverseFind('\\') + 1);
	return TRUE;
}

/*******************************************************************************
* 
* Function: CSEFileInfo::Reset
* 
* Description: 
*	 Reset the class data members 
* 
* Parameters: 
*    None
* 
* Return: 
*    None
*******************************************************************************/
void CSEFileInfo::Reset()
{
	m_nSize = 0;
	m_strPathname = "";
	m_strFilename = "";
}