/*************************************************************************************
*
*	File:		SelfExtracter.cpp
*	Version:	1.12
*
*	Author:		James Spibey
*	Date:		22/08/2002
*	E-mail:		james@spibey.com
*
*	Implementation of the CSelfExtracter class
*
*	You are free to use, distribute or modify this code
*	as long as this header is not removed or modified.
*
*
*************************************************************************************/

#include "stdafx.h"
#include "SelfExtractor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
* 
* Function: CSelfExtractor::CSelfExtractor
* 
* Description: 
*	Default Constructor 
* 
* Parameters: 
*    None
* 
* Return: 
*    None
*******************************************************************************/
CSelfExtractor::CSelfExtractor()
{
	m_nFiles		= 0;
	m_nTOCSize		= 0;
	zalloc			= 0;	// z_stream member
    zfree			= 0;	// z_stream member
    opaque			= 0;	// z_stream member

	// Get the filename once and for all
	char FullName[MAX_PATH+1];
	GetModuleFileName(NULL,FullName,MAX_PATH);
	m_strThisFileName = FullName;
}

/*******************************************************************************
* 
* Function: CSelfExtractor::~CSelfExtractor
* 
* Description: 
*	 Destructor 
* 
* Parameters: 
*    None
* 
* Return: 
*    None
*******************************************************************************/
CSelfExtractor::~CSelfExtractor()
{

}

/*******************************************************************************
* 
* Function: CSelfExtractor::Create
* 
* Description: 
*    Creates the Self-extracting executable 
* 
* Parameters: 
*    CString ExtractorPath: Path to the Extractor Executable 
*    CString Filename:		Filename of the Self Extracting Archive to create 
*	 funcPtr pFn:			Pointer to a user defined callback function
*	 void*   UserData:		User defined data to pass to the callback function
* 
* Return: 
*    int: Error Code
*		NOTHING_TO_DO		- No files have been selected to be archived
*		COPY_FAILED			- Failed to copy the extractor
*		OUTPUT_FILE_ERROR	- Failed to open the copied file for appending
*		INPUT_FILE_ERROR	- Failed to open an input file
*******************************************************************************/
int CSelfExtractor::Create(CString ExtractorPath, CString Filename, funcPtr pFn /* = NULL */, void* userData /*=NULL*/)
{
	//Make sure we have some files to add
	if(m_nFiles < 1)
		return NOTHING_TO_DO;

	//Copy the extractor to the new archive
	CShellFileOp shOp;
	shOp.SetFlags(FOF_FILESONLY | FOF_NOCONFIRMATION | FOF_SILENT);
	shOp.AddFile(SH_SRC_FILE, ExtractorPath);
	shOp.AddFile(SH_DEST_FILE, Filename);
	if(shOp.CopyFiles() != 0)
		return COPY_FAILED;

	//Open the archive
	CFile file;
	if(!file.Open(Filename, CFile::modeWrite | CFile::shareDenyNone))
		return OUTPUT_FILE_ERROR;
	else
	{
		//Start at the end of the archive
		file.SeekToEnd();
		CreateArchive(file, pFn, userData);

		//Close the archive
		file.Close();
	}
	return SUCCESS;
}

/*******************************************************************************
* 
* Function: CSelfExtractor::Create
* 
* Description: 
*    Creates the Self-extracting executable from an extractor in resources.
*	 Simply import the exe into your resources, making sure you specify the type
*	 as "SFX_EXE". Then just past the resource ID as the first parameter
* 
* Parameters: 
*    UINT	 resource:		Resource ID (eg IDR_SFX_EXE)
*    CString Filename:		Filename of the Self Extracting Archive to create 
*	 funcPtr pFn:			Pointer to a user defined callback function
*	 void*   UserData:		User defined data to pass to the callback function
* 
* Return: 
*    int: Error Code
*		NOTHING_TO_DO		- No files have been selected to be archived
*		COPY_FAILED			- Failed to copy the extractor
*		OUTPUT_FILE_ERROR	- Failed to open the copied file for appending
*		INPUT_FILE_ERROR	- Failed to open an input file
*******************************************************************************/
int CSelfExtractor::Create(UINT resource, CString Filename, funcPtr pFn /* = NULL */, void* userData /*=NULL*/)
{
	//Make sure we have some files to add
	if(m_nFiles < 1)
		return NOTHING_TO_DO;

	//Load the extractor from resources
	HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(resource), "SFX_EXE");
	if(hrSrc == NULL)
		return RESOURCE_ERROR;

	HGLOBAL hGlobal = LoadResource(AfxGetResourceHandle(), hrSrc);
	if(hGlobal == NULL)
		return RESOURCE_ERROR;

	LPVOID lpExe = LockResource(hGlobal);
	if(lpExe == NULL)
		return RESOURCE_ERROR;

	//Create the new archive from the extractor in the resources
	CFile file;
	if(!file.Open(Filename, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		return OUTPUT_FILE_ERROR;
	else
	{
		//Write the extractor exe
		file.Write(lpExe, (UINT)SizeofResource(AfxGetResourceHandle(), hrSrc));
		
		//Do the rest
		CreateArchive(file, pFn, userData);

		//Close the archive
		file.Close();
	}
	return SUCCESS;
}

int CSelfExtractor::CreateArchive(CFile& File, funcPtr pFn, void* userData)
{
	char buffer[1000];	// Buffer for data
	CFile data;			// Input file
		
	try		
	{
		//Copy all the inout files into the archive
		for(int i = 0; i < m_nFiles; i++)
		{	
			// Open the input file
			if(data.Open(m_InfoArray[i].GetPathname() , CFile::modeRead | CFile::shareDenyNone))
			{
				// Call the user defined CallBack
				if(pFn != NULL)
					pFn(SFX_FILE_START, static_cast<void*>(&m_InfoArray[i]), userData);

				// Record Start Offset
				m_InfoArray[i].SetOffset(File.GetPosition());

				// Zlib Stuff
				int err = Z_OK;
				avail_in = 0;
				avail_out = ZLibOutputLength;
				next_out = m_ZLibOutputBuffer;
				m_ZLibFileLength = data.GetLength();
				int level = 6;
    
				deflateInit(this, level);
				
				for ( ; ; ) 
				{	
					// Load input data
					if(!ZLibLoadInput(data))
						break;

					err = deflate(this, Z_NO_FLUSH);
        
					ZLibFlushOutput(File);

					if(err != Z_OK)
						break;

					// Call the user defined CallBack
					if(pFn != NULL)
					{
						int nPercent = Percent();
						pFn(SFX_ZIP_INFO, &nPercent, userData);
					}
				}

				// Close this input file
				data.Close();
    
				for ( ; ; ) 
				{
					err = deflate(this, Z_FINISH);
					
					// Write the output
					if (!ZLibFlushOutput(File))
						break;
        
					if(err != Z_OK)
						break;
				}
				
				deflateEnd(this);

				// Update the info array with the new compressed size
				m_InfoArray[i].SetCompressedSize(total_out);

				// Call the user defined CallBack
				if(pFn != NULL)
					pFn(SFX_FILE_END, static_cast<void*>(&m_InfoArray[i]), userData);
			}
			else
				return INPUT_FILE_ERROR;
		}

		//Now Write the TOC
		for(int j = 0; j < m_nFiles; j++)
		{	
			//Write the File Offset
			int Offset = m_InfoArray[j].GetFileOffset();
			File.Write(&Offset, sizeof(int));

			//Write the File Size
			int len = m_InfoArray[j].GetFileSize();
			File.Write(&len, sizeof(int));

			//Write the Compressed File Size
			len = m_InfoArray[j].GetCompressedSize();
			File.Write(&len, sizeof(int));
				
			//Write the filename
			len = m_InfoArray[j].GetFilename().GetLength();
			strncpy(buffer, m_InfoArray[j].GetFilename(), len);
			File.Write(buffer, len);
		
			//Write the length of the filename
			File.Write(&len, sizeof(int));
		}
		
		//Write the total number of files
		File.Write((void*)&m_nFiles, sizeof(int));
		
		//Write the vesion
		strcpy(buffer, SFX_VERSION);
		File.Write(buffer, strlen(SFX_VERSION));

		//Write the SIG
		strcpy(buffer, SIGNATURE);
		File.Write(buffer, strlen(SIGNATURE));
	}
	catch(CFileException* e)
	{
		//Got sick of seeing 'unreferenced local variable'
		e->m_cause;
		return OUTPUT_FILE_ERROR;
	}

	return UNKNOWN_ERROR;
}
/*******************************************************************************
* 
* Function: CSelfExtractor::ExtractAll
* 
* Description: 
*	 Extract the current archive to the specified directory 
* 
* Parameters: 
*    CString Dir: Destination Directory 
*	 funcPtr pFn:			Pointer to a user defined callback function
*	 void*   UserData:		User defined data to pass to the callback function
* 
* Return: 
*    int: Error Code
*			INPUT_FILE_ERROR	- Failed to open the input file
*			OUTPUT_FILE_ERROR	- Failed to create an output file
*******************************************************************************/
int CSelfExtractor::ExtractAll(CString Dir, funcPtr pFn /*= NULL*/, void * userData /*= NULL*/)
{
	//Make sure the directory name has a trailing backslash
	EnsureTrailingBackSlash(Dir);

	CFile Thisfile;		//Archive (Usually itself)

	//Read the Table of Contents
	int res = ReadTOC(GetThisFileName());
	if(res != SUCCESS)
		return res;

	m_pUserData = userData;
	m_pFn = pFn;
	
	//Open the archive
	if(!Thisfile.Open(GetThisFileName(), CFile::modeRead | CFile::shareDenyNone))
		return INPUT_FILE_ERROR;
	else
	{
		int nErrorCount = 0;
		//Get the files out in reverse order so we can work out the offsets
		//Subtract 1 from the filecount as we are zero-based
		for(int i = (m_nFiles - 1); i >= 0 ; i--)
		{
			//Do the callback
			if(pFn != NULL)
				pFn(SFX_FILE_START, static_cast<void*>(&m_InfoArray[i]), m_pUserData);

			if(ExtractOne(Thisfile, i, Dir) == SUCCESS)
			{
				//Do the callback
				if(pFn != NULL)
					pFn(SFX_FILE_END, static_cast<void*>(&m_InfoArray[i]), m_pUserData);
			}
			else
				nErrorCount++;
		}

		//Close the archive
		Thisfile.Close();

		if(nErrorCount != 0)
			return OUTPUT_FILE_ERROR;
	}
	return SUCCESS;
}

/*******************************************************************************
* 
* Function: CSelfExtractor::Extract
* 
* Description: 
*	 Extract a single file from the current archive to the specified directory 
* 
* Parameters: 
*	 int index:		index in array of file
*    CString Dir:	Destination Directory 
* 
* Return: 
*    int: Error Code
*			INPUT_FILE_ERROR	- Failed to open the input file
*			OUTPUT_FILE_ERROR	- Failed to create an output file
*******************************************************************************/
int CSelfExtractor::Extract(int index, CString Dir)
{
	//Make sure the directory name has a trailing backslash
	EnsureTrailingBackSlash(Dir);

	CFile Thisfile;		//Archive (Usually itself)

	//Read the Table of Contents
	int res = ReadTOC(GetThisFileName());
	if(res != SUCCESS)
		return res;
	
	//Open the archive
	if(!Thisfile.Open(GetThisFileName(), CFile::modeRead | CFile::shareDenyNone))
		return INPUT_FILE_ERROR;
	else
	{
		ExtractOne(Thisfile, index, Dir);

		//Close the archive
		Thisfile.Close();
	}
	return SUCCESS;
}

/*******************************************************************************
* 
* Function: CSelfExtractor::ExtractOne
* 
* Description: 
*	Actual Data Extraction. Seeks to required offset in archive 
*	and writes new file 
* 
* Parameters: 
*    CFile& file:	Pointer to the archive 
*    int index:		Index of file in array 
*    CString Dir:	Destination Dir 
* 
* Return: 
*    int: Error Code
*******************************************************************************/
int CSelfExtractor::ExtractOne(CFile& File, int index, CString Dir)
{
	CFile NewFile;			// Extracted File
	
	// Get the file size (in bytes)
	int FileSize = m_InfoArray[index].GetFileSize();
	int CompSize = m_InfoArray[index].GetCompressedSize();
			
	// Create the new file
	if(!NewFile.Open(Dir + m_InfoArray[index].GetFilename() , CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		return OUTPUT_FILE_ERROR;
	
	// Seek to the correct Offset
	File.Seek(m_InfoArray[index].GetFileOffset(), CFile::begin);
	
	// Zlib stuff
	int err = Z_OK;
    avail_in = 0;
    avail_out = ZLibOutputLength;
    next_out = m_ZLibOutputBuffer;
    m_ZLibFileLength = CompSize;
    
	inflateInit(this);
    for ( ; ; ) 
	{
        if(!ZLibLoadInput(File))
            break;
        err = inflate( this, Z_NO_FLUSH );
        
		ZLibFlushOutput(NewFile);
        
		if ( err != Z_OK )
            break;
        
		// Call the user defined CallBack
		if(m_pFn != NULL)
		{
			int nPercent = Percent();
			m_pFn(SFX_ZIP_INFO, &nPercent, m_pUserData);
		}
    }

    for ( ; ; ) 
	{
        err = inflate( this, Z_FINISH );
        if(!ZLibFlushOutput(NewFile))
            break;

        if(err != Z_OK)
            break;
    }
    
    inflateEnd( this );

	//Close the output file
	NewFile.Close();

	return SUCCESS;
}

/*******************************************************************************
* 
* Function: CSelfExtractor::ReadTOC
* 
* Description: 
*	 Read the archive's Table of Contents 
* 
* Parameters: 
*    CString Filename: Filename of the archive (full path) 
* 
* Return: 
*    int: Error Code
*******************************************************************************/
int CSelfExtractor::ReadTOC(CString Filename)
{
	CFile Thisfile;		//Archive file
	char buffer[1000];	//Buffer to read and write with

	//Clear the CSEFileInfo class array
	Reset();
	
	//Open the archive
	if(!Thisfile.Open(Filename, CFile::modeRead | CFile::shareDenyNone))
		return NO_SOURCE;
	else
	{
		//Read in the signature
		Thisfile.Seek(- static_cast<int>(strlen(SIGNATURE)), CFile::end);
		Thisfile.Read(buffer, strlen(SIGNATURE));

		// Check that it matches
		if(strncmp(buffer, SIGNATURE, strlen(SIGNATURE)) != 0)
			return INVALID_SIG;

		// Read in the version
		int LastOffset = strlen(SIGNATURE) + strlen(SFX_VERSION);
		Thisfile.Seek(-LastOffset, CFile::end);
		Thisfile.Read(buffer, strlen(SFX_VERSION));

		// Check that the version matches
		if(strncmp(buffer, SFX_VERSION, strlen(SFX_VERSION)) != 0)
			return INVALID_SIG;
		
		// Read Number of files
		LastOffset += sizeof(int);
		Thisfile.Seek(-LastOffset, CFile::end);
		Thisfile.Read(&m_nFiles, sizeof(int));

		//If there are no files in the archive, there is nothing to extract
		if(m_nFiles == 0)
			return NOTHING_TO_DO;

		//Read the TOC in. The array is filled in reverse to ensure that it 
		//corresponds to the data segment
		for(int i = (m_nFiles - 1); i >= 0 ; i--)
		{
			int nSize		= 0;
			int nCompSize	= 0;
			int nOffset		= 0;
			int len			= 0;
			LastOffset += sizeof(int);
				
			// Get Length of Pathname
			Thisfile.Seek(-LastOffset, CFile::end);
			Thisfile.Read(&len, sizeof(int));
			LastOffset += len;
				
			// Get Path Name
			Thisfile.Seek(-LastOffset, CFile::end);
			Thisfile.Read(buffer, len);
			LastOffset += sizeof(int);

			// Get Compressed File Size
			Thisfile.Seek(-LastOffset, CFile::end);
			Thisfile.Read(&nCompSize, sizeof(int));
			LastOffset += sizeof(int);

			// Get File Size
			Thisfile.Seek(-LastOffset, CFile::end);
			Thisfile.Read(&nSize, sizeof(int));
			LastOffset += sizeof(int);

			// Get File Offset
			Thisfile.Seek(-LastOffset, CFile::end);
			Thisfile.Read(&nOffset, sizeof(int));

			//Set the data in the array
			m_InfoArray[i].SetSize(nSize);
			m_InfoArray[i].SetCompressedSize(nCompSize);
			CString Temp(buffer);
			m_InfoArray[i].SetFilename(Temp.Left(len));
			m_InfoArray[i].SetOffset(nOffset);
		}

		//Record the total size of the TOC for use 
		//when extracting the data segment
		m_nTOCSize = LastOffset;
	}
	
	//Close the archive
	Thisfile.Close();

	return SUCCESS;
}

/*******************************************************************************
* 
* Function: CSelfExtractor::AddFile
* 
* Description: 
*	 Add a file to the archive 
* 
* Parameters: 
*    CString File: Input File path 
* 
* Return: 
*    BOOL: Success or Failure
*******************************************************************************/
BOOL CSelfExtractor::AddFile(CString File)
{
	if(m_nFiles == MAX_FILES)
		return FALSE;

	if(m_InfoArray[m_nFiles].SetData(File))
	{
		m_nFiles++;
		return TRUE;
	}
	
	return FALSE;
}


/*******************************************************************************
* 
* Function: CSelfExtractor::Reset
* 
* Description: 
*	 Reset the CSEFileInfo Array 
* 
* Parameters: 
*    None
* 
* Return: 
*    None
*******************************************************************************/
void CSelfExtractor::Reset()
{
	for(int i = 0; i < MAX_FILES; i++)
		m_InfoArray[i].Reset();

	m_nFiles	= 0;
	m_nTOCSize	= 0;
}

/*******************************************************************************
* 
* Function: CSelfExtractor::EnsureTrailingBackSlash
* 
* Description: 
*	 Ensure that the string has a trailing backslash 
* 
* Parameters: 
*    CString &string: Pathname 
* 
* Return: 
*    CString: Pathname
*******************************************************************************/
CString CSelfExtractor::EnsureTrailingBackSlash(CString &string)
{
	int len = string.GetLength();
	if(string[len - 1] != '\\')
		string += "\\";
	return string;
}

/*******************************************************************************
* 
* Function: CSelfExtractor::Percent
* 
* Description: 
*	 Calculates the percentage of the file which has been compressed
*	 so far.  
* 
* Parameters: 
*    None
* 
* Return: 
*    int: Percentage as a whole number
*******************************************************************************/
int CSelfExtractor::Percent()
{
    if (m_ZLibFileLength == 0)
        return 100;
    else if (m_ZLibFileLength > 10000000L)
        return (total_in / (m_ZLibFileLength / 100 ) );
    else
        return (total_in * 100 / m_ZLibFileLength);
}


/*******************************************************************************
* 
* Function: CSelfExtractor::ZLibLoadInput
* 
* Description: 
*	 Loads data from the input file into the input buffer  
*	 which is then used by zlib when compressing/decompressing
* 
* Parameters: 
*    CFile& File - the file which is being read
* 
* Return: 
*    int: amount of data read this time
*******************************************************************************/
int CSelfExtractor::ZLibLoadInput(CFile& File)
{
    if(avail_in == 0) 
	{
        next_in = m_ZLibInputBuffer;
        avail_in = File.Read(m_ZLibInputBuffer, ZLibInputLength);
    }

    return avail_in;
}


/*******************************************************************************
* 
* Function: CSelfExtractor::ZLibFlushOutput
* 
* Description: 
*	 Writes processed data to the output file. 
* 
* Parameters: 
*    CFile& File - the output file
* 
* Return: 
*    int: amount of data written this time
*******************************************************************************/
int CSelfExtractor::ZLibFlushOutput(CFile& File)
{
    unsigned int count = ZLibOutputLength - avail_out;
    if(count) 
	{
        File.Write(m_ZLibOutputBuffer, count);
        next_out = m_ZLibOutputBuffer;
        avail_out = ZLibOutputLength;
    }

    return count;
}