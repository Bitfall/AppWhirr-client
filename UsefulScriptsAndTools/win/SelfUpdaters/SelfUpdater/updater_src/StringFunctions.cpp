//*********************************************************************
// INCLUDES
//*********************************************************************

#include "stdafx.h"

//*********************************************************************
// FUNCTIONS
//*********************************************************************

CString StringUpper(CString sText)
{
	// Declare variables
	CString sTemp;

	// Make uppercase
#if (_MFC_VER < 0x0700)
	sText.MakeUpper();
#else
	sTemp = sText;
	sText = sTemp.MakeUpper();
#endif

	// Return string
	return sText;
}

//=====================================================================

CString StringLower(CString sText)
{
	// Declare variables
	CString sTemp;

	// Make lowercase
#if (_MFC_VER < 0x0700)
	sText.MakeLower();
#else
	sTemp = sText;
	sText = sTemp.MakeLower();
#endif

	// Return string
	return sText;
}

//=====================================================================

LPSTR ConvertToANSI(LPCWSTR sText)
{
	// len will hold the required length of converted string
	int len = WideCharToMultiByte(CP_ACP, 0, sText, -1, 0, 0, 0, 0);

	// allocate the buffer to hold the converted string
	LPSTR result = new char[len + 1];

	// do the conversion
	WideCharToMultiByte(CP_ACP, 0, sText, -1, result, len, 0, 0);

	// Return result
	return result;
}

//=====================================================================

LPWSTR ConvertToUnicode(LPCSTR sText)
{
	// len will hold the required length of converted string
	int len = MultiByteToWideChar(CP_UTF8, 0, sText, -1, 0, 0);

	// allocate the buffer to hold the converted string
	LPWSTR result = new TCHAR[len];

	// do the conversion
	MultiByteToWideChar(CP_UTF8, 0, sText, -1, result, len);

	// Return result
	return result;
}

//=====================================================================

CString ConvertStringToHex(CString sText, int iBlockSize /* = 1 */)
{
	// Declare variables
	CString sResult = _T(""), sTemp;
	TCHAR c;

	// Loop string
	for (int i = 0; i < sText.GetLength(); ++i)
	{
		c = sText.GetAt(i);
		sTemp.Format(_T("%04X"), c);
		sResult += sTemp;
	}

	// Make sure the string is
	while (sResult.GetLength() % iBlockSize != 0)
	{
		// Add a space
		sTemp.Format(_T("%04X"), _T(" "));
		sResult += sTemp;
	}

	// Return result
	return sResult;
}

//=====================================================================

CString ConvertHexToString(CString sText)
{ 
	// Declare variables
	wchar_t c;
	CString sResult, sTemp;
	int iTemp, iSize, iMultiplier;
	unsigned int ctemp;

	// Get text length
	iSize = sText.GetLength();

	// Loop the hex string
	for (int i = 0; i < iSize; i += 4)
	{
		// Initialize temp value
		iTemp = 0;

		// Loop the four hex numbers
		for (int j = 0; j < 4; j++)
		{
			// Get char
			c = sText.GetAt(i + j);

			// Calculate multiplier
			iMultiplier = 3 - j;

			// Convert back to a valid integer
			if ((c == 'A') || (c == 'B') || (c == 'C') || (c == 'D') || (c == 'E') || (c == 'F'))
			{
				ctemp = c;
				if (iMultiplier <= 0)
				{
					iTemp += (ctemp - 55);
				}
				else
				{
					iTemp += (ctemp - 55) * (iMultiplier * 16);
				}
			}
			else
			{
				if (iMultiplier <= 0)
				{
					iTemp += _ttoi(&c);
				}
				else
				{
					iTemp += _ttoi(&c) * (iMultiplier * 16);
				}
			}
		}

		// Add character to result
		sTemp.Format(_T("%c"), iTemp);
		sResult += sTemp;
	}

	// Return result
	return sResult;
}
