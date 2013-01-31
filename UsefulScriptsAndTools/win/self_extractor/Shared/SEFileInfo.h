/*************************************************************************************
*
*	File:		SEFileInfo.h
*	Version:	1.12
*
*	Author:		James Spibey
*	Date:		22/08/2002		
*	E-mail:		james@spibey.com
*
*	Specification of the CSEFileInfo class
*
*	You are free to use, distribute or modify this code
*	as long as this header is not removed or modified.
*
*	This class holds data regarding each file in an archive
*
*************************************************************************************/

#if !defined(AFX_SEFILEINFO_H__5C3D775E_497B_11D3_A8BC_0050043A01C0__INCLUDED_)
#define AFX_SEFILEINFO_H__5C3D775E_497B_11D3_A8BC_0050043A01C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSEFileInfo
{
public:
	CSEFileInfo();
	~CSEFileInfo();
	void		Serialize(CArchive &ar);

	BOOL		SetData(CString File);
	void		SetFilename(CString file){m_strFilename = file;}
	void		SetSize(int size){m_nSize = size;}
	void		SetCompressedSize(int size){m_nCompressedSize = size;}
	void		SetOffset(int offset){m_nOffset = offset;}
	
	CString		GetPathname(){return m_strPathname;}
	CString		GetFilename(){return m_strFilename;}
	int			GetFileSize(){return m_nSize;}
	int			GetCompressedSize(){return m_nCompressedSize;}
	int			GetFileOffset(){return m_nOffset;}

	void		Reset();

protected:
	CString		m_strFilename;		// Name of file
	CString		m_strPathname;		// Path to file to add
	int			m_nSize;			// Size of file
	int			m_nCompressedSize;	// Compressed Size of file
	int			m_nOffset;			// Start Offset
};

#endif // !defined(AFX_SEFILEINFO_H__5C3D775E_497B_11D3_A8BC_0050043A01C0__INCLUDED_)
