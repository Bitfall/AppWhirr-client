// Redefinition protection
#ifndef STRINGFUNCTIONS_H
#define STRINGFUNCTIONS_H

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// FUNCTIONS
//*********************************************************************

CString StringUpper(CString sText);
CString StringLower(CString sText);

LPSTR	ConvertToANSI(LPCWSTR sText);
LPWSTR	ConvertToUnicode(LPCSTR sText);

CString ConvertStringToHex(CString sText, int iBlockSize = 1);
CString ConvertHexToString(CString sText);

#endif // STRINGFUNCTIONS_H