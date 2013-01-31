/********************************************************************
	created:	2005/09/10
	created:	10:9:2005   19:46
	filename: 	ResultLabel.h
	file path:	Updater
	file base:	ResultLabel
	file ext:	h
	author:		Geert van Horrik
	
	purpose:	
*********************************************************************/

// Redefinition protection
#ifndef RESULTLABEL_H
#define RESULTLABEL_H

//*********************************************************************
// CONSTANTS
//*********************************************************************

const int LBLRESULT_NONE = -1;
const int LBLRESULT_TOCHECK = 0;
const int LBLRESULT_CHECKING = 1;
const int LBLRESULT_SUCCEEDED = 2;
const int LBLRESULT_NOTSUCCEEDED = 3;

//*********************************************************************
// INCLUDES
//*********************************************************************

//*********************************************************************
// CRESULTLABEL
//*********************************************************************

class CResultLabel : public CStatic
{
public:
	// Constructor & destructor
	CResultLabel();
	virtual ~CResultLabel();

	// Functions
	void	 SetState(int iState = LBLRESULT_NONE);
	int		 GetState();
	void	 SetColorToCheck(COLORREF color);
	COLORREF GetColorToCheck();
	void	 SetColorChecking(COLORREF color);
	COLORREF GetColorChecking();
	void	 SetColorSucceeded(COLORREF color);
	COLORREF GetColorSucceeded();
	void	 SetColorNotSucceeded(COLORREF color);
	COLORREF GetColorNotSucceeded();

protected:
	// Functions
	void ClearPixels(CPaintDC & dc);
	afx_msg void OnPaint();

	// Variables
	int		 m_iState;
	COLORREF m_clrToCheck;
	COLORREF m_clrChecking;
	COLORREF m_clrSucceeded;
	COLORREF m_clrNotSucceeded;
	COLORREF m_clrBackground;

	// Message map
	DECLARE_MESSAGE_MAP()
};

// End of redefinition protection
#endif // RESULTLABEL_H