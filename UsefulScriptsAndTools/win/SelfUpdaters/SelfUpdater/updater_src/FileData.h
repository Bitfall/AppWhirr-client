/********************************************************************
	created:	2005/02/21
	created:	21:2:2005   11:51
	filename: 	FileData.h
	file path:	Updater
	file base:	FileData
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef FILEDATA_H
#define FILEDATA_H

#include "Actions.h"

//=====================================================================
// CFileData
//=====================================================================

class CFileData  
{
public:
	// Constructor & destructor
	CFileData(CString sName, CString sFilename, CString sVersion,
			  CString sDate, CString sHash, CString sCheckType,
			  CString sSection);
	virtual ~CFileData();

	// Functions
	CString       GetName();
	CString       GetFilename();
	CString       GetVersion();
	CString		  GetDate();
	CString		  GetHash();
	CString		  GetCheckType();
	CString		  GetSection();
	int		      GetActionCount();
	CActionBase * GetAction(int index);
	bool 	      GetErrorFree();
	void		  SetErrorFree(bool bErrorFree = false);
	bool		  GetUpdateFile();
	void		  SetUpdateFile(bool bUpdateFile = false);
	void		  AddAction(CActionBase * pAction);

private:
	// Functions

	// Variables
	CString		  m_sName;
	CString		  m_sFilename;
	CString		  m_sVersion;
	CString		  m_sDate;
	CString		  m_sHash;
	CString		  m_sCheckType;
	CString		  m_sSection;
	int			  m_iActionCount;
	CActionBase * m_arrActions[MAX_ACTIONS];
	bool		  m_bErrorFree;
	bool		  m_bUpdateFile;
	bool		  m_bKeepTimestamp;
};

// End of redefinition protection
#endif // FILEDATA_H

