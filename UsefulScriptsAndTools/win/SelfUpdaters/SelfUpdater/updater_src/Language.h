/********************************************************************
	created:	2005/03/02
	created:	2:3:2005   13:30
	filename: 	Language.h
	file path:	Updater
	file base:	Language
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef LANGUAGE_H
#define LANGUAGE_H

//*********************************************************************
// INCLUDES
//*********************************************************************

#include "Markup.h"						// XML support

#include <memory>				// Needed for singleton class
using namespace std;

//*********************************************************************
// STRUCTS
//*********************************************************************

struct LanguageInfo
{
	CString sXML;				// XML name
	LANGID	arrLanguages[10];	// Languages that should use this xml
	int		iLanguageCount;		// Number of languages
};

//*********************************************************************
// CLanguage
//*********************************************************************

class CLanguage  
{
public:
	// Functions
	static  CLanguage * Instance();
	bool	LoadLanguage(LANGID language);
	bool	LoadLanguage(CString sXmlLocation, CString sLanguageName, bool bShowErrors = true);
	void	ReleaseLanguage();
	CString GetString(UINT ResourceNumber);
	CString GetLanguageName();
	CString GetLanguageTranslator();
	CString GetLanguageVersion();

private:
	// Constructor & destructor
	CLanguage();			// Private constructor because singleton!

	// Functions
	bool	ReadLanguageInfo();
	CString ReadFromXml(UINT ResourceNumber);
	CString XML_to_CString(CString sFilename);
	void	InitLanguagesAvailable();
	CString GetStringConstant(UINT nResourceNumber);

	// Variables
	static std::auto_ptr<CLanguage> sm_inst;
	CString		 m_sLanguageName;
	CString		 m_sLanguageTranslator;
	CString		 m_sLanguageVersion;
	CMarkup		 m_oLanguageXml;
	bool		 m_bParsed;
	LanguageInfo m_arrLanguages[100];
	int			 m_iLanguageCount;
};

// End of redefinition protection
#endif // LANGUAGE_H