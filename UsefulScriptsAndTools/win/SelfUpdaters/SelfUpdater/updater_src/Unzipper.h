// Unzipper.h: interface for the CUnzipper class (c) daniel godson 2002
//
// CUnzipper is a simple c++ wrapper for the 'unzip' file extraction
// api written by Gilles Vollant (c) 2002, which in turn makes use of 
// 'zlib' written by Jean-loup Gailly and Mark Adler (c) 1995-2002.
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely without restriction.
//
// Notwithstanding this, you are still bound by the conditions imposed
// by the original authors of 'unzip' and 'zlib'
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNZIPPER_H__EBC42716_31C7_4659_8EF3_9BF8D4409709__INCLUDED_)
#define AFX_UNZIPPER_H__EBC42716_31C7_4659_8EF3_9BF8D4409709__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const UINT MAX_COMMENT = 255;

// create our own fileinfo struct to hide the underlying implementation
struct UZ_FileInfo
{
	TCHAR szFileName[MAX_PATH + 1];
	TCHAR szComment[MAX_COMMENT + 1];
	
	DWORD dwVersion;  
	DWORD dwVersionNeeded;
	DWORD dwFlags;	 
	DWORD dwCompressionMethod; 
	DWORD dwDosDate;	
	DWORD dwCRC;   
	DWORD dwCompressedSize; 
	DWORD dwUncompressedSize;
	DWORD dwInternalAttrib; 
	DWORD dwExternalAttrib; 
	bool bFolder;
};

class CUnzipper  
{
public:
	CUnzipper(CString szFilePath = _T(""));
	virtual ~CUnzipper();
	
	// simple interface
	static bool Unzip(CString szFileName, CString szFolder = _T(""), bool bIgnoreFilePath = FALSE);
	
	// works with prior opened zip
	bool Unzip(bool bIgnoreFilePath = FALSE); // unzips to output folder or sub folder with zip name 
	bool UnzipTo(CString szFolder, bool bIgnoreFilePath = FALSE); // unzips to specified folder

	// extended interface
	bool OpenZip(CString szFilePath);
	bool CloseZip(); // for multiple reuse
	bool SetOutputFolder(CString szFolder); // will try to create
	
	// unzip by file index
	int GetFileCount();
	bool GetFileInfo(int nFile, UZ_FileInfo& info);
	bool UnzipFile(int nFile, CString szFolder =  _T(""), bool bIgnoreFilePath = FALSE);
	
	// unzip current file
	bool GotoFirstFile(CString szExt =  _T(""));
	bool GotoNextFile(CString szExt =  _T(""));
	bool GetFileInfo(UZ_FileInfo& info);
	bool UnzipFile(CString szFolder =  _T(""), bool bIgnoreFilePath = FALSE);

	// helpers
	bool GotoFile(CString szFileName, bool bIgnoreFilePath = TRUE);
	bool GotoFile(int nFile);
	
protected:
	void* m_uzFile;
	TCHAR m_szOutputFolder[MAX_PATH + 1];

protected:
	static bool CreateFolder(CString sFolder);
	static bool CreateFilePath(CString szFilePath); // truncates from the last '\'
	static bool SetFileModTime(CString szFilePath, DWORD dwDosDate);

};

#endif // !defined(AFX_UNZIPPER_H__EBC42716_31C7_4659_8EF3_9BF8D4409709__INCLUDED_)
