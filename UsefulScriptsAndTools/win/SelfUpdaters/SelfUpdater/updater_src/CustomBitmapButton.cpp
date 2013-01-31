/**********************************************************************
**
**	CustomBitmapButton.cpp : implementation file of the
**								CCustomBitmapButton class
**
**	by Andrzej Markowski August 2004
**
**********************************************************************/

#include "stdafx.h"
#include "CustomBitmapButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomBitmapButton

CMap <HWND,HWND,CCustomBitmapButton*,CCustomBitmapButton*> CCustomBitmapButton::m_mapCaptionFrames;
CCriticalSection CCustomBitmapButton::m_cs;

CCustomBitmapButton::CCustomBitmapButton() :
		m_nBkStyle(CBNBKGNDSTYLE_CAPTION),
		m_fActive(TRUE),
		m_nState(CBNST_NORMAL),
		m_nHeldNdx(-1),
		m_bHeld(HELDNONE),
		m_nOverNdx(-1),
		m_hBmpBk(NULL),
		m_hBmpGlyph(NULL),
		m_pOldWndProc(NULL),
		m_pCaptionWnd(NULL)
{
	RegisterWindowClass();
}

CCustomBitmapButton::~CCustomBitmapButton()
{
	::DeleteObject(m_hBmpBk);
	m_hBmpBk = NULL;
	::DeleteObject(m_hBmpGlyph);
	m_hBmpGlyph = NULL;
}

BEGIN_MESSAGE_MAP(CCustomBitmapButton, CWnd)
	//{{AFX_MSG_MAP(CCustomBitmapButton)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(WM_THEMECHANGED,OnThemeChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomBitmapButton message handlers

BOOL CCustomBitmapButton::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, CustomBitmapButton_CLASSNAME, &wndcls)))
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = CustomBitmapButton_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CCustomBitmapButton::Create(UINT dwStyle, const CRect & rect, CWnd * pParentWnd, UINT nID)
{
	return CWnd::Create(CustomBitmapButton_CLASSNAME, _T(""), dwStyle, rect, pParentWnd, nID);
}

int CCustomBitmapButton::CreateCaptionFrame(CWnd * pCaptionWnd, int nIDIcon)
{
	if(m_hWnd)
		return CBNERR_INCORRECTFUNCCALL;
	
	if(pCaptionWnd==NULL || pCaptionWnd->m_hWnd==NULL)
		return CBNERR_INCORRECTPARAMETER;
	
	if(m_pOldWndProc)
		return CBNERR_FRAMEALREADYCREATED;
	
	LONG dwStyle = pCaptionWnd->GetStyle();
	if(!(dwStyle & WS_CAPTION))
		return CBNERR_INCORRECTFRAMESTYLE;

	CCustomBitmapButton* p;
	m_cs.Lock();
	if(m_mapCaptionFrames.Lookup(pCaptionWnd->m_hWnd,p))
	{
		m_cs.Unlock();
		return CBNERR_INCORRECTPARAMETER;
	}
	if(!(m_pOldWndProc = (WNDPROC)::SetWindowLong(pCaptionWnd->m_hWnd, GWL_WNDPROC, (DWORD)CaptionFrameWindowProc)))
	{
		m_cs.Unlock();
		return CBNERR_SETWINDOWLONGFAILED;
	}
	m_mapCaptionFrames.SetAt(pCaptionWnd->m_hWnd,this);
	m_pCaptionWnd = pCaptionWnd;
	m_cs.Unlock();

	m_nIDIcon = nIDIcon;

	int nRet;
	// Close button
	if((nRet=AddCaptionButton(CRect(0,0,0,0),0,CBNBKGNDSTYLE_CLOSE,FALSE)))
	{
		DestroyCaptionFrame();
		return nRet;
	}
	// Maximize button
	if((nRet=AddCaptionButton(CRect(0,0,0,0),0,CBNBKGNDSTYLE_CAPTION,FALSE)))
	{
		DestroyCaptionFrame();
		return nRet;
	}
	// Minimize button
	if((nRet=AddCaptionButton(CRect(0,0,0,0),0,CBNBKGNDSTYLE_CAPTION,FALSE)))
	{
		DestroyCaptionFrame();
		return nRet;
	}
	// Help button
	if((nRet=AddCaptionButton(CRect(0,0,0,0),0,CBNBKGNDSTYLE_CAPTION,FALSE)))
	{
		DestroyCaptionFrame();
		return nRet;
	}
	m_Theme.CloseThemeData();
	m_Theme.OpenThemeData(pCaptionWnd->m_hWnd,L"WINDOW");
	return CBNERR_NOERROR;
}

int CCustomBitmapButton::DestroyCaptionFrame()
{
	if(!m_pOldWndProc)
		return CBNERR_NOERROR;
	m_cs.Lock();
	if(!((WNDPROC)::SetWindowLong(m_pCaptionWnd->m_hWnd, GWL_WNDPROC, (DWORD)m_pOldWndProc)))
	{
		m_cs.Unlock();
		return CBNERR_SETWINDOWLONGFAILED;
	}
	m_mapCaptionFrames.RemoveKey(m_pCaptionWnd->m_hWnd);
	m_pOldWndProc = NULL;
	m_pCaptionWnd = NULL;
	m_cs.Unlock();

	for(int i=0;i<m_aButtons.GetSize();i++)
	{
		CCustomBitmapButton* pBn = m_aButtons[i];
		pBn->DestroyWindow();
		delete pBn;
	}
	m_aButtons.RemoveAll();
	m_Theme.CloseThemeData();
	return CBNERR_NOERROR;
}

int CCustomBitmapButton::AddCaptionButton(LPCRECT lpRect, UINT nID, int nBkStyle, BOOL fUserDefWidth)
{
	if(!m_pOldWndProc)
		return CBNERR_FRAMENOTCREATED;
	CCustomBitmapButton* pBn = new CCustomBitmapButton();
	if(pBn==NULL)
		return CBNERR_OUTOFMEMORY;
	pBn->m_rect = *lpRect;
	if(nBkStyle>=CBNBKGNDSTYLE_CAPTION && nBkStyle<=CBNBKGNDSTYLE_SPINDNHOR)
		pBn->m_nBkStyle = nBkStyle;
	else
	{
		delete pBn;
		return CBNERR_INCORRECTPARAMETER;
	}
	
	DWORD dwStyle = WS_CHILD|CBNSTYLE_CAPTION;
	if(fUserDefWidth)
		dwStyle |= CBNSTYLE_USERDEFWIDTH;

	if(!pBn->Create(dwStyle,lpRect,m_pCaptionWnd,nID))
	{
		delete pBn;
		return CBNERR_CREATEBUTTONFAILED;
	}
	pBn->m_pCaptionWnd = m_pCaptionWnd;
	int ndx = m_aButtons.Add(pBn);

	if(ndx>=4)
	{
		CRect r;
		m_pCaptionWnd->GetWindowRect(r);
		m_pCaptionWnd->MoveWindow(r);
	}
	return CBNERR_NOERROR;
}

int CCustomBitmapButton::SetTooltipText(CString sText)
{
	if(m_pOldWndProc)
		return CBNERR_INCORRECTFUNCCALL;
	CWnd* pTooltipWnd;
	CCustomBitmapButton* pTooltipBn;
	CRect r;
	if(GetStyle()&CBNSTYLE_CAPTION)
	{
		if(m_pCaptionWnd==NULL)
			return CBNERR_INCORRECTBUTTONSTYLE;
		pTooltipWnd = m_pCaptionWnd;
		m_cs.Lock();
		if(!m_mapCaptionFrames.Lookup(m_pCaptionWnd->m_hWnd,pTooltipBn))
		{
			m_cs.Unlock();
			return CBNERR_INCORRECTBUTTONSTYLE;
		}
		m_cs.Unlock();
		r = m_rect;
		r.SetRect(0,0,0,0);
	}
	else
	{
		pTooltipWnd = this;
		pTooltipBn = this;
		GetClientRect(r);
	}
	if(pTooltipBn->m_ctrlToolTip.m_hWnd==NULL)
	{
		if(!pTooltipBn->m_ctrlToolTip.Create(pTooltipWnd))
			return CBNERR_CREATETOOLTIPCTRLFAILED;
		pTooltipBn->m_ctrlToolTip.Activate(TRUE);
	}
	pTooltipBn->m_ctrlToolTip.DelTool(pTooltipWnd,GetDlgCtrlID());
	pTooltipBn->m_ctrlToolTip.AddTool(pTooltipWnd,sText,r,GetDlgCtrlID());
	return CBNERR_NOERROR;
}

int CCustomBitmapButton::SetBkStyle(int nBkStyle)
{
	if(m_pOldWndProc)
		return CBNERR_INCORRECTFUNCCALL;
	if(GetStyle()&CBNSTYLE_CAPTION && m_pCaptionWnd==NULL)
		return CBNERR_INCORRECTBUTTONSTYLE;

	if(nBkStyle>=CBNBKGNDSTYLE_CAPTION && nBkStyle<=CBNBKGNDSTYLE_SCROLLRIGHT)
	{
		m_nBkStyle = nBkStyle;
		OnThemeChanged(0,0);
		if(GetStyle()&CBNSTYLE_CAPTION)
			m_pCaptionWnd->RedrawWindow(m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
		else
			Invalidate();
		return CBNERR_NOERROR;
	}
	return CBNERR_INCORRECTPARAMETER;
}

int CCustomBitmapButton::SetGlyphBitmap(HBITMAP hBmpGlyph)
{
	if(m_pOldWndProc)
		return CBNERR_INCORRECTFUNCCALL;
	if(GetStyle()&CBNSTYLE_CAPTION && m_pCaptionWnd==NULL)
		return CBNERR_INCORRECTBUTTONSTYLE;

	::DeleteObject(m_hBmpGlyph);
	m_hBmpGlyph = NULL;
	if(hBmpGlyph==NULL)
	{
		if(GetStyle()&CBNSTYLE_CAPTION)
			m_pCaptionWnd->RedrawWindow(m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
		else
			Invalidate();
		return CBNERR_NOERROR;
	}
	
	BITMAP bm;
	CBitmap::FromHandle(hBmpGlyph)->GetBitmap(&bm);
	m_hBmpGlyph = (HBITMAP)::CopyImage(hBmpGlyph,IMAGE_BITMAP,bm.bmWidth,bm.bmHeight,LR_MONOCHROME);
	if(m_hBmpGlyph==NULL)
		return CBNERR_COPYIMAGEFAILED;

	if(GetStyle()&CBNSTYLE_CAPTION)
		m_pCaptionWnd->RedrawWindow(m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
	else
		Invalidate();
	return CBNERR_NOERROR;
}

int CCustomBitmapButton::EnableWindow(BOOL fEnable)
{
	if(m_pOldWndProc)
		return CBNERR_INCORRECTFUNCCALL;
	if(GetStyle()&CBNSTYLE_CAPTION && m_pCaptionWnd==NULL)
		return CBNERR_INCORRECTBUTTONSTYLE;
	CWnd::EnableWindow(fEnable);
	if(GetStyle()&CBNSTYLE_CAPTION)
		m_pCaptionWnd->RedrawWindow(m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
	else
		Invalidate();
	return CBNERR_NOERROR;
}

BOOL CCustomBitmapButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CCustomBitmapButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Draw(dc);	
}

void CCustomBitmapButton::DrawBk(CDC& dc, CRect& r, int nImageNdx)
{
	MY_MARGINS m = {m_mrgnBk.cxLeftWidth,m_mrgnBk.cxRightWidth,m_mrgnBk.cyBottomHeight,m_mrgnBk.cyTopHeight};
	
	if((m.cxLeftWidth+m.cxRightWidth>r.Width()) || (m.cyTopHeight+m.cyBottomHeight>r.Height()))
		return;	// the rectangle too small

	if(2*(m.cxLeftWidth+m.cxRightWidth)>r.Width())
	{
		int nDelta = r.Width() - (m.cxLeftWidth+m.cxRightWidth);
		m.cxLeftWidth += nDelta/2;
		m.cxRightWidth += nDelta/2 + nDelta%2;
	}

	if(2*(m.cyTopHeight+m.cyBottomHeight)>r.Height())
	{
		int nDelta = r.Height() - (m.cyTopHeight+m.cyBottomHeight);
		m.cyTopHeight += nDelta/2;
		m.cyBottomHeight += nDelta/2 + nDelta%2;
	}

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap* pBmp = CBitmap::FromHandle(m_hBmpBk);
	BITMAP bm;
	pBmp->GetBitmap(&bm);
	CBitmap* pOldBmp = dcMem.SelectObject(pBmp);
	if(m_fBkImageHorLayout)
	{
		// left-top
		dc.BitBlt(r.left,
				r.top,
				m.cxLeftWidth,
				m.cyTopHeight,
				&dcMem,
				nImageNdx*bm.bmWidth/m_nImageCount,
				0,
				SRCCOPY);
		
		// right-top
		dc.BitBlt(r.right-m.cxRightWidth,
				r.top,
				m.cxRightWidth,
				m.cyTopHeight,
				&dcMem,
				(nImageNdx+1)*bm.bmWidth/m_nImageCount-m.cxRightWidth,
				0,
				SRCCOPY);

		// left-bottom
		dc.BitBlt(r.left,
				r.bottom-m.cyBottomHeight,
				m.cxLeftWidth,
				m.cyBottomHeight,
				&dcMem,
				nImageNdx*bm.bmWidth/m_nImageCount,
				bm.bmHeight-m.cyBottomHeight,
				SRCCOPY);

		// right-bottom
		dc.BitBlt(r.right-m.cxRightWidth,
				r.bottom-m.cyBottomHeight,
				m.cxRightWidth,
				m.cyBottomHeight,
				&dcMem,
				(nImageNdx+1)*bm.bmWidth/m_nImageCount-m.cxRightWidth,
				bm.bmHeight-m.cyBottomHeight,
				SRCCOPY);

		// middle-top
		dc.StretchBlt(r.left+m.cxLeftWidth,
			r.top,
			r.Width()-m.cxLeftWidth-m.cxRightWidth,
			m.cyTopHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/m_nImageCount+m.cxLeftWidth,
			0,
			bm.bmWidth/m_nImageCount-m.cxLeftWidth-m.cxRightWidth,
			m.cyTopHeight,
			SRCCOPY);

		// middle-bottom
		dc.StretchBlt(r.left+m.cxLeftWidth,
			r.bottom-m.cyBottomHeight,
			r.Width()-m.cxLeftWidth-m.cxRightWidth,
			m.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/m_nImageCount+m.cxLeftWidth,
			bm.bmHeight-m.cyBottomHeight,
			bm.bmWidth/m_nImageCount-m.cxLeftWidth-m.cxRightWidth,
			m.cyBottomHeight,
			SRCCOPY);

		// middle-left
		dc.StretchBlt(r.left,
			r.top+m.cyTopHeight,
			m.cxLeftWidth,
			r.Height()-m.cyTopHeight-m.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/m_nImageCount,
			m.cyTopHeight,
			m.cxLeftWidth,
			bm.bmHeight-m.cyTopHeight-m.cyBottomHeight,
			SRCCOPY);

		// middle-right
		dc.StretchBlt(r.right-m.cxRightWidth,
			r.top+m.cyTopHeight,
			m.cxRightWidth,
			r.Height()-m.cyTopHeight-m.cyBottomHeight,
			&dcMem,
			(nImageNdx+1)*bm.bmWidth/m_nImageCount-m.cxRightWidth,
			m.cyTopHeight,
			m.cxRightWidth,
			bm.bmHeight-m.cyTopHeight-m.cyBottomHeight,
			SRCCOPY);

		// middle
		dc.StretchBlt(
			r.left+m.cxLeftWidth,
			r.top+m.cyTopHeight,
			r.Width()-m.cxLeftWidth-m.cxRightWidth,
			r.Height()-m.cyTopHeight-m.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/m_nImageCount + m.cxLeftWidth,
			m.cyTopHeight,
			bm.bmWidth/m_nImageCount-m.cxLeftWidth-m.cxRightWidth,
			bm.bmHeight-m.cyTopHeight-m.cyBottomHeight,
			SRCCOPY);
	}
	else
	{
		// left-top
		dc.BitBlt(r.left,
				r.top,
				m.cxLeftWidth,
				m.cyTopHeight,
				&dcMem,
				0,
				nImageNdx*bm.bmHeight/m_nImageCount,
				SRCCOPY);
		
		// right-top
		dc.BitBlt(r.right-m.cxRightWidth,
				r.top,
				m.cxRightWidth,
				m.cyTopHeight,
				&dcMem,
				bm.bmWidth-m.cxRightWidth,
				nImageNdx*bm.bmHeight/m_nImageCount,
				SRCCOPY);
		
		// left-bottom
		dc.BitBlt(r.left,
				r.bottom-m.cyBottomHeight,
				m.cxLeftWidth,
				m.cyBottomHeight,
				&dcMem,
				0,
				(nImageNdx+1)*bm.bmHeight/m_nImageCount-m.cyBottomHeight,
				SRCCOPY);

		// right-bottom
		dc.BitBlt(r.right-m.cxRightWidth,
				r.bottom-m.cyBottomHeight,
				m.cxRightWidth,
				m.cyBottomHeight,
				&dcMem,
				bm.bmWidth-m.cxRightWidth,
				(nImageNdx+1)*bm.bmHeight/m_nImageCount-m.cyBottomHeight,
				SRCCOPY);

		// middle-top
		dc.StretchBlt(r.left+m.cxLeftWidth,
			r.top,
			r.Width()-m.cxLeftWidth-m.cxRightWidth,
			m.cyTopHeight,
			&dcMem,
			m.cxLeftWidth,
			nImageNdx*bm.bmHeight/m_nImageCount,
			bm.bmWidth-m.cxLeftWidth-m.cxRightWidth,
			m.cyTopHeight,
			SRCCOPY);

		// middle-bottom
		dc.StretchBlt(r.left+m.cxLeftWidth,
			r.bottom-m.cyBottomHeight,
			r.Width()-m.cxLeftWidth-m.cxRightWidth,
			m.cyBottomHeight,
			&dcMem,
			m.cxLeftWidth,
			(nImageNdx+1)*bm.bmHeight/m_nImageCount-m.cyBottomHeight,
			bm.bmWidth-m.cxLeftWidth-m.cxRightWidth,
			m.cyBottomHeight,
			SRCCOPY);

		// middle-left
		dc.StretchBlt(r.left,
			r.top+m.cyTopHeight,
			m.cxLeftWidth,
			r.Height()-m.cyTopHeight-m.cyBottomHeight,
			&dcMem,
			0,
			nImageNdx*bm.bmHeight/m_nImageCount+m.cyTopHeight,
			m.cxLeftWidth,
			bm.bmHeight/m_nImageCount-m.cyTopHeight-m.cyBottomHeight,
			SRCCOPY);

		// middle-right
		dc.StretchBlt(r.right-m.cxRightWidth,
			r.top+m.cyTopHeight,
			m.cxRightWidth,
			r.Height()-m.cyTopHeight-m.cyBottomHeight,
			&dcMem,
			bm.bmWidth-m.cxRightWidth,
			nImageNdx*bm.bmHeight/m_nImageCount+m.cyTopHeight,
			m.cxRightWidth,
			bm.bmHeight/m_nImageCount-m.cyTopHeight-m.cyBottomHeight,
			SRCCOPY);

		// middle
		dc.StretchBlt(
			r.left+m.cxLeftWidth,
			r.top+m.cyTopHeight,
			r.Width()-m.cxLeftWidth-m.cxRightWidth,
			r.Height()-m.cyTopHeight-m.cyBottomHeight,
			&dcMem,
			m.cxLeftWidth,
			nImageNdx*bm.bmHeight/m_nImageCount+m.cyTopHeight,
			bm.bmWidth-m.cxLeftWidth-m.cxRightWidth,
			bm.bmHeight/m_nImageCount-m.cyTopHeight-m.cyBottomHeight,
			SRCCOPY);
	}
	dcMem.SelectObject(pOldBmp);
}

void CCustomBitmapButton::DrawGlyph(CDC& dc, CRect& r, int nColorNdx)
{
	if(m_hBmpGlyph==NULL)
		return;
	
	BITMAP bm;
	CBitmap::FromHandle(m_hBmpGlyph)->GetBitmap(&bm);
	CPoint pt(r.left+(r.Width()-bm.bmWidth)/2,r.top+(r.Height()-bm.bmHeight)/2);
	
	CDC dcMem, dcMemMono;
	dcMem.CreateCompatibleDC(&dc);
	dcMemMono.CreateCompatibleDC(&dc);

	CBitmap* pOldBmpGlyphMono = dcMemMono.SelectObject(CBitmap::FromHandle(m_hBmpGlyph));

	CBitmap bmpGlyphColor;
	bmpGlyphColor.CreateCompatibleBitmap(&dc,bm.bmWidth,bm.bmHeight);
	
	CBitmap* pOldBmpGlyphColor = dcMem.SelectObject(&bmpGlyphColor);

	COLORREF rgbOldTextGlyph = dcMem.SetTextColor(m_rgbGlyph[nColorNdx]);
	dcMem.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcMemMono, 0, 0, SRCCOPY);
	dcMem.SetTextColor(rgbOldTextGlyph);

	COLORREF rgbOldBk = dc.SetBkColor(RGB(255,255,255));
	COLORREF rgbOldText = dc.SetTextColor(RGB(0,0,0));
	dc.BitBlt(pt.x, pt.y, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, SRCINVERT);
	dc.BitBlt(pt.x, pt.y, bm.bmWidth, bm.bmHeight, &dcMemMono, 0, 0, SRCAND);
	dc.BitBlt(pt.x, pt.y, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, SRCINVERT);

	dcMem.SelectObject(pOldBmpGlyphColor);
	dcMemMono.SelectObject(pOldBmpGlyphMono);
	dc.SetBkColor(rgbOldBk);
	dc.SetTextColor(rgbOldText);
}

void CCustomBitmapButton::GetGlyphColorHor(int i, int iMin, int iMax, int ndx, COLORGLYPH& st)
{
	if(i==iMin)
	{
		for(int k=0; k<4; k++)
		{
			st.a[k].rgbGlyph = st.rgbTransGlyph;
			st.a[k].nCnt = 0;
		}
	}
	for(int j=0;j<st.nHeight;j++)
	{
		if(st.pDC->GetPixel(i,j)!=st.rgbTransGlyph)
		{
			for(int k=0; k<4; k++)
			{
				if(st.a[k].nCnt==0)
				{
					st.a[k].rgbGlyph = st.pDC->GetPixel(i,j);
					st.a[k].nCnt = 1;
					break;
				}
				else if(st.a[k].rgbGlyph == st.pDC->GetPixel(i,j))
				{
					st.a[k].nCnt++;
					break;
				}
			}	
		}
	}
	if(i==iMax-1)
	{
		if(st.a[0].nCnt==0)
			AfxThrowUserException();
		int nMaxCnt = 0;
		int nMaxNdx=0;
		for(int k=0; k<4; k++)
		{
			if(st.a[k].nCnt>nMaxCnt)
			{
				nMaxCnt = st.a[k].nCnt;
				nMaxNdx = k;
			}
		}
		m_rgbGlyph[ndx] = st.a[nMaxNdx].rgbGlyph;
	}
}

void CCustomBitmapButton::GetGlyphColorVer(int i, int iMin, int iMax, int ndx, COLORGLYPH& st)
{
	if(i==iMin)
	{
		for(int k=0; k<4; k++)
		{
			st.a[k].rgbGlyph = st.rgbTransGlyph;
			st.a[k].nCnt = 0;
		}
	}
	for(int j=0;j<st.nWidth;j++)
	{
		if(st.pDC->GetPixel(j,i)!=st.rgbTransGlyph)
		{
			for(int k=0; k<4; k++)
			{
				if(st.a[k].nCnt==0)
				{
					st.a[k].rgbGlyph = st.pDC->GetPixel(j,i);
					st.a[k].nCnt = 1;
					break;
				}
				else if(st.a[k].rgbGlyph == st.pDC->GetPixel(j,i))
				{
					st.a[k].nCnt++;
					break;
				}
			}	
		}
	}
	if(i==iMax-1)
	{
		if(st.a[0].nCnt==0)
			AfxThrowUserException();
		int nMaxCnt = 0;
		int nMaxNdx=0;
		for(int k=0; k<4; k++)
		{
			if(st.a[k].nCnt>nMaxCnt)
			{
				nMaxCnt = st.a[k].nCnt;
				nMaxNdx = k;
			}
		}
		m_rgbGlyph[ndx] = st.a[nMaxNdx].rgbGlyph;
	}
}

LONG CCustomBitmapButton::OnThemeChanged(WPARAM wParam, LPARAM lParam) 
{
	::DeleteObject(m_hBmpBk);
	m_hBmpBk = NULL;
	
	m_Theme.CloseThemeData();

	int nPartID;
	int nGlyphPropID;
	WCHAR sClass[50];
	switch(m_nBkStyle)
	{
	case CBNBKGNDSTYLE_CAPTION:
		m_nImageCount = 8;
		m_nImageOffset = 0;
		nPartID = WP_MAXBUTTON;
		nGlyphPropID = TMT_IMAGEFILE1;
		wcscpy(sClass,L"WINDOW");
		break;
	case CBNBKGNDSTYLE_CLOSE:
		m_nImageCount = 8;
		m_nImageOffset = 0;
		nPartID = WP_CLOSEBUTTON;
		nGlyphPropID = TMT_IMAGEFILE1;
		wcscpy(sClass,L"WINDOW");
		break;
	case CBNBKGNDSTYLE_MDI: 
		m_nImageCount = 4;
		m_nImageOffset = 0;
		nPartID = WP_MDIMINBUTTON;
		nGlyphPropID = TMT_GLYPHIMAGEFILE;
		wcscpy(sClass,L"WINDOW");
		break;
	case CBNBKGNDSTYLE_COMBO: 
		m_nImageCount = 4;
		m_nImageOffset = 0;
		nPartID = CP_DROPDOWNBUTTON;
		nGlyphPropID = TMT_GLYPHIMAGEFILE;
		wcscpy(sClass,L"COMBOBOX");
		break;
	case CBNBKGNDSTYLE_SPINUP: 
		m_nImageCount = 4;
		m_nImageOffset = 0;
		nPartID = SPNP_UP;
		nGlyphPropID = TMT_GLYPHIMAGEFILE;
		wcscpy(sClass,L"SPIN");
		break;
	case CBNBKGNDSTYLE_SPINDN: 
		m_nImageCount = 4;
		m_nImageOffset = 0;
		nPartID = SPNP_DOWN;
		nGlyphPropID = TMT_GLYPHIMAGEFILE;
		wcscpy(sClass,L"SPIN");
		break;
	case CBNBKGNDSTYLE_SPINUPHOR: 
		m_nImageCount = 4;
		m_nImageOffset = 0;
		nPartID = SPNP_UPHORZ;
		nGlyphPropID = TMT_GLYPHIMAGEFILE;
		wcscpy(sClass,L"SPIN");
		break;
	case CBNBKGNDSTYLE_SPINDNHOR: 
		m_nImageCount = 4;
		m_nImageOffset = 0;
		nPartID = SPNP_DOWNHORZ;
		nGlyphPropID = TMT_GLYPHIMAGEFILE;
		wcscpy(sClass,L"SPIN");
		break;
	case CBNBKGNDSTYLE_SCROLLUP: 
		m_nImageCount = 16;
		m_nImageOffset = 0;
		nPartID = SBP_ARROWBTN;
		nGlyphPropID = TMT_IMAGEFILE1;
		wcscpy(sClass,L"SCROLLBAR");
		break;
	case CBNBKGNDSTYLE_SCROLLDOWN: 
		m_nImageCount = 16;
		m_nImageOffset = 4;
		nPartID = SBP_ARROWBTN;
		nGlyphPropID = TMT_IMAGEFILE1;
		wcscpy(sClass,L"SCROLLBAR");
		break;
	case CBNBKGNDSTYLE_SCROLLLEFT: 
		m_nImageCount = 16;
		m_nImageOffset = 8;
		nPartID = SBP_ARROWBTN;
		nGlyphPropID = TMT_IMAGEFILE1;
		wcscpy(sClass,L"SCROLLBAR");
		break;
	case CBNBKGNDSTYLE_SCROLLRIGHT: 
		m_nImageCount = 16;
		m_nImageOffset = 12;
		nPartID = SBP_ARROWBTN;
		nGlyphPropID = TMT_IMAGEFILE1;
		wcscpy(sClass,L"SCROLLBAR");
		break;
	default:
		return 0;
		break;
	}

	HBITMAP hBmpGlyph = NULL;
	CDC dcGlyph;
	dcGlyph.CreateCompatibleDC(NULL);
	CBitmap* pOldBmpGlyph = NULL;

	try
	{
		if(!m_Theme.OpenThemeData(m_hWnd, sClass))
			AfxThrowUserException();

		{
			// background
			int nBkType;
			if(!m_Theme.GetThemeEnumValue(nPartID,0,TMT_BGTYPE,&nBkType))
				AfxThrowUserException();
			if(nBkType!=BT_IMAGEFILE)
				AfxThrowUserException();

			int nImageCount;
			if(!m_Theme.GetThemeInt(nPartID,0,TMT_IMAGECOUNT,&nImageCount))
				AfxThrowUserException();
			if(nImageCount!=m_nImageCount)
				AfxThrowUserException();

			WCHAR szBkBitmapFilename[MAX_PATH];
			if(!m_Theme.GetThemeFilename(nPartID,0,TMT_IMAGEFILE,szBkBitmapFilename,MAX_PATH))
				AfxThrowUserException();
			CString sFile(szBkBitmapFilename);
			if(!(m_hBmpBk = m_Theme.LoadBitmap(szBkBitmapFilename)))
				AfxThrowUserException();

			int nRet = m_Theme.IsThemeBackgroundPartiallyTransparent(nPartID,0);
			if(nRet==-1)
				AfxThrowUserException();
			m_fBkTransparent = nRet;
			if(nRet)
			{
				m_rgbBkTransparent = RGB(255,0,255);
				if(!m_Theme.GetThemeColor(nPartID,0,TMT_TRANSPARENTCOLOR,&m_rgbBkTransparent))
					AfxThrowUserException();
			}
			
			int nImageLayout;
			if(!m_Theme.GetThemeEnumValue(nPartID,0,TMT_IMAGELAYOUT,&nImageLayout))
				AfxThrowUserException();
			if(nImageLayout==IL_VERTICAL)
				m_fBkImageHorLayout = FALSE;
			else
				m_fBkImageHorLayout = TRUE;
			
			if(!m_Theme.GetThemeMargins(nPartID,0,TMT_SIZINGMARGINS,&m_mrgnBk))
				AfxThrowUserException();

			if(m_Theme.IsLunaTheme() && (m_nBkStyle==CBNBKGNDSTYLE_CAPTION || m_nBkStyle==CBNBKGNDSTYLE_CLOSE))
			{
				// Set the corners to the transparent color
				CBitmap* pBmp = CBitmap::FromHandle(m_hBmpBk);
				BITMAP bm;
				pBmp->GetBitmap(&bm);
				CDC dcBk;
				dcBk.CreateCompatibleDC(NULL);
				CBitmap* pOldBmp = dcBk.SelectObject(pBmp);
				if(m_fBkImageHorLayout)
				{
					CRect r(0,0,bm.bmWidth/8-1,bm.bmHeight-1);
					for(int i=0; i<8; i++)
					{
						dcBk.SetPixel(r.left,r.top,m_rgbBkTransparent);
						dcBk.SetPixel(r.right,r.top,m_rgbBkTransparent);
						dcBk.SetPixel(r.left,r.bottom,m_rgbBkTransparent);
						dcBk.SetPixel(r.right,r.bottom,m_rgbBkTransparent);
						r.OffsetRect(bm.bmWidth/8,0);
					}
				}
				else
				{
					CRect r(0,0,bm.bmWidth-1,bm.bmHeight/8-1);
					for(int i=0; i<8; i++)
					{
						dcBk.SetPixel(r.left,r.top,m_rgbBkTransparent);
						dcBk.SetPixel(r.right,r.top,m_rgbBkTransparent);
						dcBk.SetPixel(r.left,r.bottom,m_rgbBkTransparent);
						dcBk.SetPixel(r.right,r.bottom,m_rgbBkTransparent);
						r.OffsetRect(0,bm.bmHeight/8);
					}
				}
				dcBk.SelectObject(pOldBmp);
			}
		}
		{
			// glyph color
			int nGlyphType;
			if(!m_Theme.GetThemeEnumValue(nPartID,0,TMT_GLYPHTYPE,&nGlyphType))
				AfxThrowUserException();
			
			if(nGlyphType==GT_IMAGEGLYPH)
			{
				COLORREF rgbTransGlyph = RGB(255,0,255);
				if(!m_Theme.GetThemeColor(nPartID,0,TMT_GLYPHTRANSPARENTCOLOR,&rgbTransGlyph))
					AfxThrowUserException();
				WCHAR szSpinGlyphIconFilename[MAX_PATH];
				if(!m_Theme.GetThemeFilename(nPartID,0,nGlyphPropID,szSpinGlyphIconFilename,MAX_PATH))
					AfxThrowUserException();
				if(!(hBmpGlyph = m_Theme.LoadBitmap(szSpinGlyphIconFilename)))
					AfxThrowUserException();

				CBitmap* pBmp = CBitmap::FromHandle(hBmpGlyph);
				if(pBmp==NULL)
					AfxThrowUserException();
				pOldBmpGlyph = dcGlyph.SelectObject(pBmp);

				BITMAP bm;
				pBmp->GetBitmap(&bm);
				m_rgbGlyph[0] = rgbTransGlyph;
				m_rgbGlyph[1] = rgbTransGlyph;
				m_rgbGlyph[2] = rgbTransGlyph;
				m_rgbGlyph[3] = rgbTransGlyph;
				m_rgbGlyph[4] = rgbTransGlyph;
				m_rgbGlyph[5] = rgbTransGlyph;
				m_rgbGlyph[6] = rgbTransGlyph;
				m_rgbGlyph[7] = rgbTransGlyph;
				
				COLORGLYPH st;
				st.rgbTransGlyph = rgbTransGlyph;
				st.nWidth = bm.bmWidth;
				st.nHeight = bm.bmHeight;
				st.pDC = &dcGlyph;
				switch(m_nBkStyle)
				{
				case CBNBKGNDSTYLE_CAPTION:
				case CBNBKGNDSTYLE_CLOSE:
					{
						if(m_fBkImageHorLayout)
						{
							for(int i=0;i<bm.bmWidth;i++)
							{
								if(i<bm.bmWidth/8)
									GetGlyphColorHor(i,0,bm.bmWidth/8,0,st);
								else if(i<bm.bmWidth/4)
									GetGlyphColorHor(i,bm.bmWidth/8,bm.bmWidth/4,1,st);
								else if(i<3*bm.bmWidth/8)
									GetGlyphColorHor(i,bm.bmWidth/4,3*bm.bmWidth/8,2,st);
								else if(i<bm.bmWidth/2)
									GetGlyphColorHor(i,3*bm.bmWidth/8,bm.bmWidth/2,3,st);
								else if(i<5*bm.bmWidth/8)
									GetGlyphColorHor(i,bm.bmWidth/2,5*bm.bmWidth/8,4,st);
								else if(i<6*bm.bmWidth/8)
									GetGlyphColorHor(i,5*bm.bmWidth/8,6*bm.bmWidth/8,5,st);
								else if(i<7*bm.bmWidth/8)
									GetGlyphColorHor(i,6*bm.bmWidth/8,7*bm.bmWidth/8,6,st);
								else if(i<bm.bmWidth)
									GetGlyphColorHor(i,7*bm.bmWidth/8,bm.bmWidth,7,st);
							}
						}
						else
						{
							for(int i=0;i<bm.bmHeight;i++)
							{
								if(i<bm.bmHeight/8)
									GetGlyphColorVer(i,0,bm.bmHeight/8,0,st);
								else if(i<bm.bmHeight/4)
									GetGlyphColorVer(i,bm.bmHeight/8,bm.bmHeight/4,1,st);
								else if(i<3*bm.bmHeight/8)
									GetGlyphColorVer(i,bm.bmHeight/4,3*bm.bmHeight/8,2,st);
								else if(i<bm.bmHeight/2)
									GetGlyphColorVer(i,3*bm.bmHeight/8,bm.bmHeight/2,3,st);
								else if(i<5*bm.bmHeight/8)
									GetGlyphColorVer(i,bm.bmHeight/2,5*bm.bmHeight/8,4,st);
								else if(i<6*bm.bmHeight/8)
									GetGlyphColorVer(i,5*bm.bmHeight/8,6*bm.bmHeight/8,5,st);
								else if(i<7*bm.bmHeight/8)
									GetGlyphColorVer(i,6*bm.bmHeight/8,7*bm.bmHeight/8,6,st);
								else if(i<bm.bmHeight)
									GetGlyphColorVer(i,7*bm.bmHeight/8,bm.bmHeight,7,st);
							}
						}
					}
					break;
				default:
					{
						if(m_fBkImageHorLayout)
						{
							int nBnWidth = bm.bmWidth/m_nImageCount;
							int nBnOffset = nBnWidth*m_nImageOffset;
							for(int i=nBnOffset;i<nBnOffset+4*nBnWidth;i++)
							{
								if(i<nBnOffset+nBnWidth)
									GetGlyphColorHor(i,nBnOffset,nBnOffset+nBnWidth,0,st);
								else if(i<nBnOffset+2*nBnWidth)
									GetGlyphColorHor(i,nBnOffset+nBnWidth,nBnOffset+2*nBnWidth,1,st);
								else if(i<nBnOffset+3*nBnWidth)
									GetGlyphColorHor(i,nBnOffset+2*nBnWidth,nBnOffset+3*nBnWidth,2,st);
								else if(i<nBnOffset+4*nBnWidth)
									GetGlyphColorHor(i,nBnOffset+3*nBnWidth,nBnOffset+4*nBnWidth,3,st);
							}
						}
						else
						{
							int nBnHeight = bm.bmHeight/m_nImageCount;
							int nBnOffset = nBnHeight*m_nImageOffset;
							for(int i=nBnOffset;i<nBnOffset+4*nBnHeight;i++)
							{
								if(i<nBnOffset+nBnHeight)
									GetGlyphColorVer(i,nBnOffset,nBnOffset+nBnHeight,0,st);
								else if(i<nBnOffset+2*nBnHeight)
									GetGlyphColorVer(i,nBnOffset+nBnHeight,nBnOffset+2*nBnHeight,1,st);
								else if(i<nBnOffset+3*nBnHeight)
									GetGlyphColorVer(i,nBnOffset+2*nBnHeight,nBnOffset+3*nBnHeight,2,st);
								else if(i<nBnOffset+4*nBnHeight)
									GetGlyphColorVer(i,nBnOffset+3*nBnHeight,nBnOffset+4*nBnHeight,3,st);
							}
						}
					}
					break;
				}
				dcGlyph.SelectObject(pOldBmpGlyph);
				pOldBmpGlyph = NULL;
				::DeleteObject(hBmpGlyph);
				hBmpGlyph = NULL;
			}
			else
				AfxThrowUserException();
		}
	}
	catch(CUserException* e)
	{
		e->Delete();
		::DeleteObject(m_hBmpBk);
		m_hBmpBk = NULL;
		if(pOldBmpGlyph)
			dcGlyph.SelectObject(pOldBmpGlyph);
		::DeleteObject(hBmpGlyph);
		hBmpGlyph = NULL;
	}
	if(!(GetStyle()&CBNSTYLE_CAPTION))
		m_Theme.CloseThemeData();
	return 0;
}

void CCustomBitmapButton::PreSubclassWindow()
{
	OnThemeChanged(0,0);
	CWnd::PreSubclassWindow();
}

void CCustomBitmapButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rCl;
	GetClientRect(&rCl);
	if(rCl.PtInRect(point))
	{
		SetCapture();
		m_nState = CBNST_PRESSED;
		m_bHeld = HELDLCLK;
		Invalidate(FALSE);
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CCustomBitmapButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect rCl;
	GetClientRect(&rCl);
	if(rCl.PtInRect(point))
	{
		SetCapture();
		m_nState = CBNST_PRESSED;
		m_bHeld = HELDLDBLCLK;
		Invalidate(FALSE);
	}
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CCustomBitmapButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bHeld==HELDLCLK || m_bHeld==HELDLDBLCLK)
	{
		CRect rCl;
		GetClientRect(&rCl);
		if(rCl.PtInRect(point))
		{
			NMHDR nmh;
			memset(&nmh,0,sizeof(NMHDR));
			nmh.hwndFrom = GetSafeHwnd();
			nmh.idFrom = GetDlgCtrlID();
			if(m_bHeld==HELDLCLK)
				nmh.code = NM_CLICK;
			else
				nmh.code = NM_DBLCLK;
			GetParent()->SendMessage(WM_NOTIFY,GetDlgCtrlID(),(LPARAM)&nmh);
			CPoint pt;
			::GetCursorPos(&pt);
			ScreenToClient(&pt);
			if(rCl.PtInRect(pt))
				m_nState = CBNST_HOT;
			else
				m_nState = CBNST_NORMAL;
		}
		m_bHeld = HELDNONE;
		ReleaseCapture();
		Invalidate(FALSE);
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CCustomBitmapButton::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRect rCl;
	GetClientRect(&rCl);
	if(rCl.PtInRect(point))
	{
		SetCapture();
		m_nState = CBNST_PRESSED;
		m_bHeld = HELDRCLK;
		Invalidate(FALSE);
	}
	CWnd::OnRButtonDown(nFlags, point);
}

void CCustomBitmapButton::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect rCl;
	GetClientRect(&rCl);
	if(rCl.PtInRect(point))
	{
		SetCapture();
		m_nState = CBNST_PRESSED;
		m_bHeld = HELDRDBLCLK;
		Invalidate(FALSE);
	}
	CWnd::OnRButtonDblClk(nFlags, point);
}

void CCustomBitmapButton::OnRButtonUp(UINT nFlags, CPoint point) 
{	
	if(m_bHeld==HELDRCLK || m_bHeld==HELDRDBLCLK)
	{
		CRect rCl;
		GetClientRect(&rCl);
		if(rCl.PtInRect(point))
		{
			NMHDR nmh;
			memset(&nmh,0,sizeof(NMHDR));
			nmh.hwndFrom = GetSafeHwnd();
			nmh.idFrom = GetDlgCtrlID();
			if(m_bHeld==HELDRCLK)
				nmh.code = NM_RCLICK;
			else
				nmh.code = NM_RDBLCLK;
			GetParent()->SendMessage(WM_NOTIFY,GetDlgCtrlID(),(LPARAM)&nmh);
			CPoint pt;
			::GetCursorPos(&pt);
			ScreenToClient(&pt);
			if(rCl.PtInRect(pt))
				m_nState = CBNST_HOT;
			else
				m_nState = CBNST_NORMAL;
		}
		m_bHeld = HELDNONE;
		ReleaseCapture();
		Invalidate(FALSE);
	}
	CWnd::OnRButtonUp(nFlags, point);
}

LONG CCustomBitmapButton::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	m_nState = CBNST_NORMAL;
	Invalidate(FALSE);
	return 0;
}

void CCustomBitmapButton::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT trackmouseevent;
	trackmouseevent.cbSize = sizeof(trackmouseevent);
	trackmouseevent.dwFlags = TME_LEAVE;
	trackmouseevent.hwndTrack = GetSafeHwnd();
	trackmouseevent.dwHoverTime = 0;
	_TrackMouseEvent(&trackmouseevent);

	CRect rCl;
	GetClientRect(&rCl);

	if(GetCapture()==this)
	{
		if(rCl.PtInRect(point))
			m_nState = CBNST_PRESSED;
		else
			m_nState = CBNST_NORMAL;
	}
	else
		m_nState = CBNST_HOT;
	Invalidate(FALSE);
	CWnd::OnMouseMove(nFlags, point);
}

void CCustomBitmapButton::Draw(CDC &dc)
{
	if(!m_hBmpBk)
	{
		m_rgbGlyph[0] = ::GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[1] = ::GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[2] = ::GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[3] = ::GetSysColor(COLOR_GRAYTEXT);
		m_rgbGlyph[4] = ::GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[5] = ::GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[6] = ::GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[7] = ::GetSysColor(COLOR_GRAYTEXT);
	}

	CRect r;
	CPoint pt(0,0);
	if(GetStyle()&CBNSTYLE_CAPTION)
	{
		r = m_rect;
		pt = r.TopLeft();
		r.OffsetRect(-pt.x,-pt.y);
	}
	else
		GetClientRect(&r);

	CDC dcMem;
	if(!dcMem.CreateCompatibleDC(&dc))
		return;
	if(m_bmpMemButton.m_hObject)
	{
		BITMAP bm;
		m_bmpMemButton.GetBitmap(&bm);
		if(bm.bmWidth!=r.Width() || bm.bmHeight!=r.Height())
		{
			m_bmpMemButton.DeleteObject();
			if(!m_bmpMemButton.CreateCompatibleBitmap(&dc,r.Width(),r.Height()))
				return;
		}
	}
	else
	{
		if(!m_bmpMemButton.CreateCompatibleBitmap(&dc,r.Width(),r.Height()))
			return;
	}

	CBitmap* pOldBmp = dcMem.SelectObject(&m_bmpMemButton);
	
	DWORD dwState = 0;

	if(IsWindowEnabled())
	{
		if(m_nState==CBNST_DISABLED)
			m_nState = CBNST_NORMAL;
		if(m_nState==CBNST_PRESSED)
			dwState=DFCS_PUSHED;
	}
	else 
	{
		dwState=DFCS_INACTIVE;
		m_nState=CBNST_DISABLED;
	}
	
	int nImageOffset=0;
	if(m_nBkStyle>=CBNBKGNDSTYLE_CAPTION && m_nBkStyle<=CBNBKGNDSTYLE_CLOSE && !m_fActive)
		nImageOffset = 4;

	int nState = m_nState-1;
	
	if(m_hBmpBk)
		DrawBk(dcMem,r,nState+nImageOffset+m_nImageOffset);
	else
		dcMem.DrawFrameControl(r,DFC_BUTTON,DFCS_BUTTONPUSH|dwState);
	DrawGlyph(dcMem,r,nState+nImageOffset);
	
	if(m_hBmpBk && m_fBkTransparent)
		m_Theme.TransparentBlt(dc.m_hDC,pt.x,pt.y,r.Width(),r.Height(),dcMem.m_hDC,0,0,r.Width(),r.Height(),m_rgbBkTransparent);
	else
		dc.BitBlt(pt.x,pt.y,r.Width(),r.Height(),&dcMem,r.left,r.top,SRCCOPY);
	dcMem.SelectObject(pOldBmp);
}

LRESULT CALLBACK CCustomBitmapButton::CaptionFrameWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CCustomBitmapButton* p;
	m_cs.Lock();
	if(!m_mapCaptionFrames.Lookup(hWnd,p))
	{
		m_cs.Unlock();
		return 0;
	}
	m_cs.Unlock();
	
	CWnd* pWnd = CWnd::FromHandle(hWnd);
	switch(message)
	{
	case WM_SETTEXT:
		{
			CRect r;
			pWnd->GetWindowRect(r);
			r.OffsetRect(-r.left,-r.top);
			pWnd->RedrawWindow(r,NULL,RDW_FRAME|RDW_INVALIDATE);
		}
		break;
	case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpmmi = (LPMINMAXINFO) lParam;
			CFont fn;
			int nCY, nPartID, nBtnsWidth;
			int nMinWidth = p->RecalcLayout(pWnd,nCY,nPartID,fn,nBtnsWidth);
			if(nMinWidth==-1 || nMinWidth > lpmmi->ptMinTrackSize.x)
				lpmmi->ptMinTrackSize.x = nMinWidth;
		}
		break;
	case WM_THEMECHANGED:
		{
			p->m_Theme.CloseThemeData();
			p->m_Theme.OpenThemeData(hWnd,L"WINDOW");
		}
		break;
	case WM_ACTIVATE:
		{
			p->m_fActive = LOWORD(wParam);
			for(int i=0; i<p->m_aButtons.GetSize(); i++)
				p->m_aButtons[i]->m_fActive = LOWORD(wParam);
			CRect r;
			pWnd->GetWindowRect(r);
			r.OffsetRect(-r.left,-r.top);
			pWnd->RedrawWindow(r,NULL,RDW_FRAME|RDW_INVALIDATE);
			if(pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
			{
				CWnd* pActiveWnd = ((CMDIFrameWnd*)pWnd)->MDIGetActive();
				TRACE(_T("ACTIVE %p\n"),pActiveWnd);
				if(pActiveWnd!=NULL)
					pActiveWnd->PostMessage(WM_NCACTIVATE,LOWORD(wParam),0);
			}
			return 0;
		}
		break;
	case WM_NCACTIVATE:
		{
			if(pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
			{
				p->m_fActive = wParam;
				for(int i=0; i<p->m_aButtons.GetSize(); i++)
					p->m_aButtons[i]->m_fActive = wParam;
				CRect r;
				pWnd->GetWindowRect(r);
				r.OffsetRect(-r.left,-r.top);
				pWnd->RedrawWindow(r,NULL,RDW_FRAME|RDW_INVALIDATE);
			}
			return TRUE;
		}
		break;
	case WM_NCPAINT:
		{
			if(p->m_Theme.IsThemeActive())
				p->DrawNCXP(pWnd);
			else
				p->DrawNCClasic(pWnd);
			if(p->m_ctrlToolTip.m_hWnd)
			{
				for(int i=4;i<p->m_aButtons.GetSize();i++)
				{
					if(p->m_aButtons[i]->m_rect.IsRectEmpty() || p->m_aButtons[i]->m_nState==CBNST_DISABLED)
						p->m_ctrlToolTip.SetToolRect(pWnd,p->m_aButtons[i]->GetDlgCtrlID(),CRect(0,0,0,0));
					else
					{
						CRect r = p->m_aButtons[i]->m_rect;
						CRect rW;
						pWnd->GetWindowRect(rW);
						CRect rC;
						pWnd->GetClientRect(rC);
						pWnd->ClientToScreen(rC);
						r.OffsetRect(rW.left-rC.left,rW.top-rC.top);
						p->m_ctrlToolTip.SetToolRect(pWnd,p->m_aButtons[i]->GetDlgCtrlID(),r);
					}
				}
			}
			return 0;
		}
		break;
	case WM_NCHITTEST:
		{
			CPoint pt(MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y);
			CRect r;
			pWnd->GetWindowRect(r);
			pt.Offset(-r.left,-r.top);
			for(int i=0;i<p->m_aButtons.GetSize();i++)
			{
				if(p->m_aButtons[i]->m_rect.PtInRect(pt))
				{
					switch(i)
					{
					case 0:
						return HTCLOSE;
					case 1:
						return HTMAXBUTTON;
					case 2:
						return HTMINBUTTON;
					case 3:
						return HTHELP;
					default:
						return HTCAPTION;
					}
				}
			}
		}
		break;
	case WM_NCMOUSEMOVE:
		{
			TRACKMOUSEEVENT trackmouseevent;
			trackmouseevent.cbSize = sizeof(trackmouseevent);
			trackmouseevent.dwFlags = TME_NONCLIENT;
			trackmouseevent.hwndTrack = pWnd->GetSafeHwnd();
			trackmouseevent.dwHoverTime = 0;
			::_TrackMouseEvent(&trackmouseevent);

			CPoint pt(MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y);
			CRect r;
			pWnd->GetWindowRect(r);
			pt.Offset(-r.left,-r.top);
			for(int i=0;i<p->m_aButtons.GetSize();i++)
			{
				if(p->m_aButtons[i]->m_rect.PtInRect(pt))
				{
					if(p->m_aButtons[i]->m_nState!=CBNST_DISABLED)
						p->m_aButtons[i]->m_nState = CBNST_HOT;
					pWnd->RedrawWindow(p->m_aButtons[i]->m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
					break;
				}
			}
			if(p->m_nOverNdx!=-1 && p->m_nOverNdx!=i)
			{
				if(p->m_aButtons[p->m_nOverNdx]->m_nState!=CBNST_DISABLED)
					p->m_aButtons[p->m_nOverNdx]->m_nState = CBNST_NORMAL;
				pWnd->RedrawWindow(p->m_aButtons[p->m_nOverNdx]->m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
			}
			if(i==p->m_aButtons.GetSize())
				p->m_nOverNdx=-1;
			else
				p->m_nOverNdx=i;
			if(p->m_ctrlToolTip.m_hWnd)
			{
				MSG msg;
				msg.lParam = lParam; 
                msg.wParam = wParam; 
                msg.message = message; 
                msg.hwnd = hWnd; 
                p->m_ctrlToolTip.RelayEvent(&msg); 
			}
			if(pWnd->IsIconic())
				return 0;
		}
		break;
	case WM_NCMOUSELEAVE:
		{	
			if(p->m_nOverNdx!=-1)
			{
				if(p->m_nHeldNdx!=-1)
					break;
				if(p->m_aButtons[p->m_nOverNdx]->m_nState!=CBNST_DISABLED)
					p->m_aButtons[p->m_nOverNdx]->m_nState = CBNST_NORMAL;
				pWnd->RedrawWindow(p->m_aButtons[p->m_nOverNdx]->m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
				p->m_nOverNdx = -1;
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			CPoint pt(MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y);
			pWnd->ClientToScreen(&pt);
			CRect r;
			pWnd->GetWindowRect(r);
			pt.Offset(-r.left,-r.top);
			if(p->m_nHeldNdx!=-1)
			{
				if(p->m_aButtons[p->m_nHeldNdx]->m_nState!=CBNST_DISABLED)
				{
					if(p->m_aButtons[p->m_nHeldNdx]->m_rect.PtInRect(pt))
						p->m_aButtons[p->m_nHeldNdx]->m_nState = CBNST_PRESSED;
					else
						p->m_aButtons[p->m_nHeldNdx]->m_nState = CBNST_NORMAL;
				}
				pWnd->RedrawWindow(p->m_aButtons[p->m_nHeldNdx]->m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
			}
		}
		break;
	case WM_NCLBUTTONDOWN:
		{
			if(p->m_ctrlToolTip.m_hWnd)
			{
				MSG msg;
				msg.lParam = lParam; 
                msg.wParam = wParam; 
                msg.message = message; 
                msg.hwnd = hWnd; 
                p->m_ctrlToolTip.RelayEvent(&msg); 
			}
			CPoint pt(MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y);
			CRect r;
			pWnd->GetWindowRect(r);
			pt.Offset(-r.left,-r.top);
			for(int i=0;i<p->m_aButtons.GetSize();i++)
			{
				if(p->m_aButtons[i]->m_rect.PtInRect(pt))
				{
					if(!p->m_fActive)
						pWnd->SetActiveWindow();
					if(p->m_aButtons[i]->m_nState!=CBNST_DISABLED)
					{
						pWnd->SetCapture();
						p->m_aButtons[i]->m_nState = CBNST_PRESSED;
						p->m_nHeldNdx = i;
						p->m_bHeld = HELDLCLK;
						pWnd->RedrawWindow(p->m_aButtons[i]->m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
					}
					return 0;
				}
			}
		}
		break;
	case WM_NCLBUTTONDBLCLK:
		{
			CPoint pt(MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y);
			CRect r;
			pWnd->GetWindowRect(r);
			pt.Offset(-r.left,-r.top);
			for(int i=0;i<p->m_aButtons.GetSize();i++)
			{
				if(p->m_aButtons[i]->m_rect.PtInRect(pt))
				{
					if(p->m_aButtons[i]->m_nState!=CBNST_DISABLED)
					{
						pWnd->SetCapture();
						p->m_aButtons[i]->m_nState = CBNST_PRESSED;
						p->m_nHeldNdx = i;
						p->m_bHeld = HELDLDBLCLK;
						pWnd->RedrawWindow(p->m_aButtons[i]->m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
					}
					return 0;
				}
			}
		}
		break;
	case WM_NCRBUTTONDOWN:
		{
			CPoint pt(MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y);
			CRect r;
			pWnd->GetWindowRect(r);
			pt.Offset(-r.left,-r.top);
			for(int i=0;i<p->m_aButtons.GetSize();i++)
			{
				if(p->m_aButtons[i]->m_rect.PtInRect(pt))
				{
					if(!p->m_fActive)
						pWnd->SetActiveWindow();
					if(p->m_aButtons[i]->m_nState!=CBNST_DISABLED)
					{
						pWnd->SetCapture();
						p->m_aButtons[i]->m_nState = CBNST_PRESSED;
						p->m_nHeldNdx = i;
						p->m_bHeld = HELDRCLK;
						pWnd->RedrawWindow(p->m_aButtons[i]->m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
					}
					return 0;
				}
			}
		}
		break;
	case WM_NCRBUTTONDBLCLK:
		{
			CPoint pt(MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y);
			CRect r;
			pWnd->GetWindowRect(r);
			pt.Offset(-r.left,-r.top);
			for(int i=0;i<p->m_aButtons.GetSize();i++)
			{
				if(p->m_aButtons[i]->m_rect.PtInRect(pt))
				{
					if(p->m_aButtons[i]->m_nState!=CBNST_DISABLED)
					{
						pWnd->SetCapture();
						p->m_aButtons[i]->m_nState = CBNST_PRESSED;
						p->m_nHeldNdx = i;
						p->m_bHeld = HELDRDBLCLK;
						pWnd->RedrawWindow(p->m_aButtons[i]->m_rect,NULL,RDW_FRAME|RDW_INVALIDATE);
					}
					return 0;
				}
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			if(p->m_ctrlToolTip.m_hWnd)
			{
				MSG msg;
				msg.lParam = lParam; 
                msg.wParam = wParam; 
                msg.message = message; 
                msg.hwnd = hWnd; 
                p->m_ctrlToolTip.RelayEvent(&msg); 
			}
			CPoint pt(MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y);
			pWnd->ClientToScreen(&pt);
			CRect r;
			pWnd->GetWindowRect(r);
			pt.Offset(-r.left,-r.top);
			if(p->m_nHeldNdx!=-1 && (p->m_bHeld==HELDLCLK || p->m_bHeld==HELDLDBLCLK))
			{
				if(p->m_aButtons[p->m_nHeldNdx]->m_rect.PtInRect(pt) &&
					p->m_aButtons[p->m_nHeldNdx]->m_nState!=CBNST_DISABLED)
				{
					switch(p->m_nHeldNdx)
					{
					case 0:
						if(p->m_bHeld==HELDLCLK)
							pWnd->PostMessage(WM_SYSCOMMAND,SC_CLOSE,lParam);
						break;
					case 1:
						{
							if(p->m_bHeld==HELDLCLK)
							{
								if(pWnd->IsZoomed())
									pWnd->PostMessage(WM_SYSCOMMAND,SC_RESTORE,lParam);
								else
									pWnd->PostMessage(WM_SYSCOMMAND,SC_MAXIMIZE,lParam);
							}
						}
						break;
					case 2:
						{
							if(p->m_bHeld==HELDLCLK)
							{
								if(pWnd->IsIconic())
									pWnd->PostMessage(WM_SYSCOMMAND,SC_RESTORE,lParam);
								else
									pWnd->PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,lParam);
							}
						}
						break;
					case 3:
						if(p->m_bHeld==HELDLCLK)
							pWnd->PostMessage(WM_SYSCOMMAND,SC_CONTEXTHELP,lParam);
						break;
					default:
						{
							NMHDR nmh;
							memset(&nmh,0,sizeof(NMHDR));
							nmh.hwndFrom = p->m_aButtons[p->m_nHeldNdx]->GetSafeHwnd();
							nmh.idFrom = p->m_aButtons[p->m_nHeldNdx]->GetDlgCtrlID();
							if(p->m_bHeld==HELDLCLK)
								nmh.code = NM_CLICK;
							else
								nmh.code = NM_DBLCLK;
							pWnd->SendMessage(WM_NOTIFY,p->m_aButtons[p->m_nHeldNdx]->GetDlgCtrlID(),(LPARAM)&nmh);
						}
						break;
					}
				}
				p->m_nHeldNdx = -1;
				p->m_bHeld = HELDNONE;
				::ReleaseCapture();
				pt.Offset(r.TopLeft());
				pWnd->PostMessage(WM_NCMOUSEMOVE,HTCAPTION,MAKELPARAM(pt.x,pt.y));
				return 0;
			}
		}
		break;
	case WM_RBUTTONUP:
		{
			CPoint pt(MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y);
			pWnd->ClientToScreen(&pt);
			CRect r;
			pWnd->GetWindowRect(r);
			pt.Offset(-r.left,-r.top);
			if(p->m_nHeldNdx!=-1 && (p->m_bHeld==HELDRCLK || p->m_bHeld==HELDRDBLCLK))
			{
				if(p->m_aButtons[p->m_nHeldNdx]->m_rect.PtInRect(pt) && 
					p->m_aButtons[p->m_nHeldNdx]->m_nState!=CBNST_DISABLED)
				{
					NMHDR nmh;
					memset(&nmh,0,sizeof(NMHDR));
					nmh.hwndFrom = p->m_aButtons[p->m_nHeldNdx]->GetSafeHwnd();
					nmh.idFrom = p->m_aButtons[p->m_nHeldNdx]->GetDlgCtrlID();
					if(p->m_bHeld==HELDRCLK)
						nmh.code = NM_RCLICK;
					else
						nmh.code = NM_RDBLCLK;
					pWnd->SendMessage(WM_NOTIFY,p->m_aButtons[p->m_nHeldNdx]->GetDlgCtrlID(),(LPARAM)&nmh);
				}
				p->m_nHeldNdx = -1;
				p->m_bHeld = HELDNONE;
				::ReleaseCapture();
				pt.Offset(r.TopLeft());
				pWnd->PostMessage(WM_NCMOUSEMOVE,HTCAPTION,MAKELPARAM(pt.x,pt.y));
				return 0;
			}
		}
		break;
	}
	
	if(message==174)
		return 0;

	return ::CallWindowProc(p->m_pOldWndProc, hWnd, message, wParam, lParam);
}

int CCustomBitmapButton::RecalcLayout(CWnd *pWnd, int& nCY, int& nPartID, CFont& fn, int& nBtnsWidth)
{
	CRect rect;
	LONG dwStyle = pWnd->GetStyle();
	pWnd->GetWindowRect(&rect);

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm,0);	 

	CSize sizeIcon(::GetSystemMetrics(SM_CXSMICON),::GetSystemMetrics(SM_CYSMICON));
	CSize sizeBtns;
	CSize sizeFrame(::GetSystemMetrics(SM_CXDLGFRAME),::GetSystemMetrics(SM_CYDLGFRAME));
	if(dwStyle&WS_THICKFRAME)
	{
		sizeFrame.cx = ::GetSystemMetrics(SM_CXFRAME);
		sizeFrame.cy = ::GetSystemMetrics(SM_CYFRAME);
	}

	BOOL fA = m_Theme.IsThemeActive();

	if(pWnd->GetExStyle()&WS_EX_TOOLWINDOW)
	{
		if(!fn.CreateFontIndirect(&ncm.lfSmCaptionFont))
			return -1;
		nCY = ::GetSystemMetrics(SM_CYSMCAPTION);
		nPartID = WP_SMALLCAPTION;
		if(fA)
		{
			sizeBtns.cx = ::GetSystemMetrics(SM_CXSMSIZE)-4;
			sizeBtns.cy = ::GetSystemMetrics(SM_CYSMSIZE)-4;
		}
		else
		{
			sizeBtns.cx = ::GetSystemMetrics(SM_CXSMSIZE)-2;
			sizeBtns.cy = ::GetSystemMetrics(SM_CYSMSIZE)-4;
		}
	}
	else
	{
		if(!fn.CreateFontIndirect(&ncm.lfCaptionFont))
			return -1;
		nCY = ::GetSystemMetrics(SM_CYCAPTION);
		nPartID = WP_CAPTION;
		if(fA)
		{
			sizeBtns.cx = ::GetSystemMetrics(SM_CXSIZE)-4;
			sizeBtns.cy = ::GetSystemMetrics(SM_CYSIZE)-4;
		}
		else
		{
			sizeBtns.cx = ::GetSystemMetrics(SM_CXSIZE)-2;
			sizeBtns.cy = ::GetSystemMetrics(SM_CYSIZE)-4;
		}
	}

	CRect rectCaption;
	if(fA)
	{
		nBtnsWidth=sizeFrame.cx;
		rectCaption.SetRect(0,0,rect.Width(),nCY+sizeFrame.cy);
	}
	else
	{
		rectCaption.SetRect(0,0,rect.Width(),nCY-1+sizeFrame.cy);
		rectCaption.InflateRect(-sizeFrame.cx, 0);
		rectCaption.top += sizeFrame.cy;
		nBtnsWidth=0;
	}

	if (dwStyle & WS_SYSMENU)
	{
		m_aButtons[0]->m_rect.SetRect(0,0,sizeBtns.cx,sizeBtns.cy);
		nBtnsWidth += m_aButtons[0]->m_rect.Width() + 2;
		m_aButtons[0]->m_rect.OffsetRect(rectCaption.Width()-nBtnsWidth,fA ? 2+sizeFrame.cy : 2);
		if (dwStyle&WS_MAXIMIZEBOX || dwStyle&WS_MINIMIZEBOX)
		{
			m_aButtons[1]->m_rect.SetRect(0,0,sizeBtns.cx,sizeBtns.cy);
			nBtnsWidth += m_aButtons[1]->m_rect.Width() + 2;
			m_aButtons[1]->m_rect.OffsetRect(rectCaption.Width()-nBtnsWidth,fA ? 2+sizeFrame.cy : 2);
			if(!(dwStyle&WS_MAXIMIZEBOX))
				m_aButtons[1]->m_nState = CBNST_DISABLED;
			else if(m_aButtons[1]->m_nState==CBNST_DISABLED)
				m_aButtons[1]->m_nState = CBNST_NORMAL;
			m_aButtons[2]->m_rect.SetRect(0,0,sizeBtns.cx,sizeBtns.cy);
			if(fA)
				nBtnsWidth += m_aButtons[2]->m_rect.Width() + 2;
			else
				nBtnsWidth += m_aButtons[2]->m_rect.Width();
			m_aButtons[2]->m_rect.OffsetRect(rectCaption.Width()-nBtnsWidth ,fA ? 2+sizeFrame.cy : 2);
			if(!(dwStyle&WS_MINIMIZEBOX))
				m_aButtons[2]->m_nState = CBNST_DISABLED;
			else if(m_aButtons[2]->m_nState==CBNST_DISABLED)
				m_aButtons[2]->m_nState = CBNST_NORMAL;
			m_aButtons[3]->m_rect.SetRectEmpty();
		}
		else if(pWnd->GetExStyle()&WS_EX_CONTEXTHELP)
		{
			m_aButtons[1]->m_rect.SetRectEmpty();
			m_aButtons[2]->m_rect.SetRectEmpty();
			m_aButtons[3]->m_rect.SetRect(0,0,sizeBtns.cx,sizeBtns.cy);
			nBtnsWidth += m_aButtons[3]->m_rect.Width() + 2;
			m_aButtons[3]->m_rect.OffsetRect(rectCaption.Width()-nBtnsWidth,fA ? 2+sizeFrame.cy : 2);
		}
		else
		{
			m_aButtons[1]->m_rect.SetRectEmpty();
			m_aButtons[2]->m_rect.SetRectEmpty();
			m_aButtons[3]->m_rect.SetRectEmpty();
		}
	}
	else
	{
		m_aButtons[0]->m_rect.SetRectEmpty();
		m_aButtons[1]->m_rect.SetRectEmpty();
		m_aButtons[2]->m_rect.SetRectEmpty();
		m_aButtons[3]->m_rect.SetRectEmpty();
	}
	
	for(int i=4; i<m_aButtons.GetSize(); i++)
	{
		if(pWnd->IsIconic())
		{
			m_aButtons[i]->m_rect.top = 0;
			m_aButtons[i]->m_rect.bottom = 0;
		}
		else
		{
			if(m_aButtons[i]->GetStyle()&CBNSTYLE_USERDEFWIDTH)
				m_aButtons[i]->m_rect.SetRect(0,0,m_aButtons[i]->m_rect.Width(),sizeBtns.cy);
			else
				m_aButtons[i]->m_rect.SetRect(0,0,sizeBtns.cx,sizeBtns.cy);
			nBtnsWidth += m_aButtons[i]->m_rect.Width() + 2;
			m_aButtons[i]->m_rect.OffsetRect(rectCaption.Width()-nBtnsWidth,fA ? 2+sizeFrame.cy : 2);
		}
	}

	CWindowDC dc(pWnd);
	CFont* pFontOld = dc.SelectObject(&fn);
	CRect r;
	dc.DrawText(_T("W..."),r,DT_CALCRECT|DT_SINGLELINE);
	dc.SelectObject(pFontOld);
	return sizeFrame.cx + (dwStyle & WS_SYSMENU ? sizeIcon.cx+6 : 4) + r.Width() + nBtnsWidth;
}

void CCustomBitmapButton::DrawNCClasic(CWnd* pWnd)
{
	CWindowDC dc(pWnd);
	LONG dwStyle = pWnd->GetStyle();
	CRect rWnd;
	pWnd->GetWindowRect(&rWnd);
	CSize sizeIcon(::GetSystemMetrics(SM_CXSMICON),::GetSystemMetrics(SM_CYSMICON));

	CSize sizeFrame(::GetSystemMetrics(SM_CXDLGFRAME),::GetSystemMetrics(SM_CYDLGFRAME));
	if(dwStyle&WS_THICKFRAME)
	{
		sizeFrame.cx = ::GetSystemMetrics(SM_CXFRAME);
		sizeFrame.cy = ::GetSystemMetrics(SM_CYFRAME);
	}

	CFont fn;
	int nCY, nPartID, nBtnsWidth;

	if(RecalcLayout(pWnd,nCY,nPartID,fn,nBtnsWidth)==-1)
		return;

	CRect rCaption(0,0,rWnd.Width(),nCY-1+sizeFrame.cy);
	rCaption.InflateRect(-sizeFrame.cx, 0);
	rCaption.top += sizeFrame.cy;

	CRgn rgn1;
	rgn1.CreateRectRgn(rWnd.left,rWnd.top,rWnd.right,rWnd.bottom);
	CRgn rgn2;
	rgn2.CreateRectRgn(rWnd.left+rCaption.left,rWnd.top+rCaption.top,rWnd.left+rCaption.right,rWnd.top+rCaption.bottom);
	CRgn rgn;
	rgn.CreateRectRgn(0,0,0,0);
	rgn.CombineRgn(&rgn1,&rgn2,RGN_DIFF);
	::DefWindowProc(pWnd->m_hWnd,WM_NCPAINT,(WPARAM)rgn.m_hObject,0);

	rWnd.OffsetRect(-rWnd.left, -rWnd.top);

	CPoint ptOffset(rCaption.left,rCaption.top);
	rCaption.OffsetRect(-ptOffset.x, -ptOffset.y);

	CBrush brCaption;
	brCaption.CreateSolidBrush(::GetSysColor(m_fActive ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION));
	COLORREF rgbFrom = ::GetSysColor(m_fActive ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION);
	COLORREF rgbTo = ::GetSysColor(m_fActive ? COLOR_GRADIENTACTIVECAPTION : COLOR_GRADIENTINACTIVECAPTION);

	// Draw the caption
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	if(m_bmpMemCaption.m_hObject)
	{
		BITMAP bm;
		m_bmpMemCaption.GetBitmap(&bm);
		if(bm.bmWidth!=rCaption.Width() || bm.bmHeight!=rCaption.Height())
		{
			m_bmpMemCaption.DeleteObject();
			if(!m_bmpMemCaption.CreateCompatibleBitmap(&dc,rCaption.Width(),rCaption.Height()))
				return;
		}
	}
	else
	{
		if(!m_bmpMemCaption.CreateCompatibleBitmap(&dc,rCaption.Width(),rCaption.Height()))
			return;
	}
	
	CBitmap* pOldBmp = dcMem.SelectObject(&m_bmpMemCaption);

	BOOL fRet = FALSE;
	::SystemParametersInfo(SPI_GETGRADIENTCAPTIONS, 0, &fRet,0);

	if(fRet)
	{
		TRIVERTEX        vert[2] ;
		GRADIENT_RECT    gRect;

		vert[0] .x      = rCaption.left;
		vert[0] .y      = rCaption.top;
		vert[0] .Red    = GetRValue(rgbFrom)<<8;
		vert[0] .Green  = GetGValue(rgbFrom)<<8;
		vert[0] .Blue   = GetBValue(rgbFrom)<<8;
		vert[0] .Alpha  = 0x0000;

		vert[1] .x      = rCaption.right - nBtnsWidth;
		vert[1] .y      = rCaption.bottom; 
		vert[1] .Red    = GetRValue(rgbTo)<<8;
		vert[1] .Green  = GetGValue(rgbTo)<<8;
		vert[1] .Blue   = GetBValue(rgbTo)<<8;
		vert[1] .Alpha  = 0x0000;

		gRect.UpperLeft  = 0;
		gRect.LowerRight = 1;

		m_Theme.GradientFill(dcMem.m_hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
		dcMem.FillSolidRect(rCaption.right - nBtnsWidth,rCaption.top,nBtnsWidth,rCaption.Height(),rgbTo);
	}
	else
		dcMem.FillRect(&rCaption, &brCaption);

	// Draw the system menu.
	if (dwStyle & WS_SYSMENU)
	{
		HICON hIcon = ::AfxGetApp()->LoadIcon(m_nIDIcon);
		if(hIcon!=NULL)
			::DrawIconEx(dcMem.m_hDC,2,(rCaption.Height()-sizeIcon.cy)/2,hIcon,sizeIcon.cx,sizeIcon.cy,0,NULL,DI_NORMAL);
	}

	// Draw the buttons
	
	for(int i=0; i<m_aButtons.GetSize(); i++)
	{
		if(!m_aButtons[i]->m_rect.IsRectEmpty())
		{
			DWORD dwState = 0;
			if(m_aButtons[i]->m_nState==CBNST_DISABLED)
				dwState=DFCS_INACTIVE;
			else if(m_aButtons[i]->m_nState==CBNST_PRESSED)
				dwState=DFCS_PUSHED;
			switch(i)
			{
			case 0:
				dcMem.DrawFrameControl(m_aButtons[i]->m_rect,DFC_CAPTION,DFCS_CAPTIONCLOSE|dwState);
				break;
			case 1:
				{
					if(pWnd->IsZoomed())
						dcMem.DrawFrameControl(m_aButtons[i]->m_rect,DFC_CAPTION,DFCS_CAPTIONRESTORE|dwState);
					else
						dcMem.DrawFrameControl(m_aButtons[i]->m_rect,DFC_CAPTION,DFCS_CAPTIONMAX|dwState);
				}
				break;
			case 2:
				{
					if(pWnd->IsIconic())
						dcMem.DrawFrameControl(m_aButtons[i]->m_rect,DFC_CAPTION,DFCS_CAPTIONRESTORE|dwState);
					else
						dcMem.DrawFrameControl(m_aButtons[i]->m_rect,DFC_CAPTION,DFCS_CAPTIONMIN|dwState);
				}
				break;
			case 3:
				dcMem.DrawFrameControl(m_aButtons[i]->m_rect,DFC_CAPTION,DFCS_CAPTIONHELP|dwState);
				break;
			default:
				m_aButtons[i]->Draw(dcMem);
				break;
			}
		}
	}

	// Draw the caption text

	CFont* pFontOld = dcMem.SelectObject(&fn);

	CString strTitle;
	pWnd->GetWindowText(strTitle);
	int xLeft = rCaption.left + 4 + (dwStyle & WS_SYSMENU ? sizeIcon.cx : 0);

	COLORREF rgbOldTxtColor = dcMem.SetTextColor(::GetSysColor(m_fActive ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT));
	int nOldBkMode = dcMem.SetBkMode(TRANSPARENT);
	UINT uFormat = DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS;
	if(pWnd->GetExStyle()&WS_EX_RIGHT)
		uFormat |= DT_RIGHT;
	dcMem.DrawText(strTitle,CRect(xLeft,0,rCaption.right-nBtnsWidth-2,rCaption.bottom),uFormat);
	dcMem.SetBkMode(nOldBkMode);
	dcMem.SetTextColor(rgbOldTxtColor);
	dcMem.SelectObject(pFontOld);

	dc.BitBlt(ptOffset.x,ptOffset.y,rCaption.Width(),rCaption.Height(),&dcMem,0,0,SRCCOPY);
	dcMem.SelectObject(pOldBmp);
	for(i=0; i<m_aButtons.GetSize(); i++)
	{
		if(!m_aButtons[i]->m_rect.IsRectEmpty())
			m_aButtons[i]->m_rect.OffsetRect(sizeFrame);
	}
}

void CCustomBitmapButton::DrawNCXP(CWnd* pWnd)
{
	CWindowDC dc(pWnd);
	LONG dwStyle = pWnd->GetStyle();
	CRect rWnd;
	pWnd->GetWindowRect(&rWnd);
	CSize sizeIcon(::GetSystemMetrics(SM_CXSMICON),::GetSystemMetrics(SM_CYSMICON));

	CSize sizeFrame(::GetSystemMetrics(SM_CXDLGFRAME),::GetSystemMetrics(SM_CYDLGFRAME));
	if(dwStyle&WS_THICKFRAME)
	{
		sizeFrame.cx = ::GetSystemMetrics(SM_CXFRAME);
		sizeFrame.cy = ::GetSystemMetrics(SM_CYFRAME);
	}

	CFont fn;
	int nCY, nPartID, nBtnsWidth;

	if(RecalcLayout(pWnd,nCY,nPartID,fn,nBtnsWidth)==-1)
		return;

	int nFrameState = FS_INACTIVE;
	if(m_fActive)
		nFrameState = FS_ACTIVE;

	CRect rCaption(0,0,rWnd.Width(),nCY+sizeFrame.cy);

	CRect rDef(rWnd.left+sizeFrame.cx,rWnd.top+rCaption.Height(),rWnd.right-sizeFrame.cx,rWnd.bottom-sizeFrame.cy);
	CRgn rgn;
	rgn.CreateRectRgn(rDef.left,rDef.top,rDef.right,rDef.bottom);

	::DefWindowProc(pWnd->m_hWnd,WM_NCPAINT,(WPARAM)rgn.m_hObject,0);
	
	rWnd.OffsetRect(-rWnd.left, -rWnd.top);
	
	CRect rLeftFrm(rWnd.left,rWnd.top+rCaption.Height(),rWnd.left+sizeFrame.cx,rWnd.bottom);
	CRect rRightFrm(rWnd.right-sizeFrame.cx,rWnd.top+rCaption.Height(),rWnd.right,rWnd.bottom);
	CRect rBottomFrm(rWnd.left,rWnd.bottom-sizeFrame.cy,rWnd.right,rWnd.bottom);
	if(pWnd->GetExStyle()&WS_EX_TOOLWINDOW)
	{
		m_Theme.DrawThemePart(dc.m_hDC,WP_SMALLFRAMELEFT,nFrameState,rLeftFrm);
		m_Theme.DrawThemePart(dc.m_hDC,WP_SMALLFRAMERIGHT,nFrameState,rRightFrm);
		m_Theme.DrawThemePart(dc.m_hDC,WP_SMALLFRAMEBOTTOM,nFrameState,rBottomFrm);
	}
	else
	{
		m_Theme.DrawThemePart(dc.m_hDC,WP_FRAMELEFT,nFrameState,rLeftFrm);
		m_Theme.DrawThemePart(dc.m_hDC,WP_FRAMERIGHT,nFrameState,rRightFrm);
		m_Theme.DrawThemePart(dc.m_hDC,WP_FRAMEBOTTOM,nFrameState,rBottomFrm);
	}

	// Draw the caption
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	if(m_bmpMemCaption.m_hObject)
	{
		BITMAP bm;
		m_bmpMemCaption.GetBitmap(&bm);
		if(bm.bmWidth!=rCaption.Width() || bm.bmHeight!=rCaption.Height())
		{
			m_bmpMemCaption.DeleteObject();
			if(!m_bmpMemCaption.CreateCompatibleBitmap(&dc,rCaption.Width(),rCaption.Height()))
				return;
		}
	}
	else
	{
		if(!m_bmpMemCaption.CreateCompatibleBitmap(&dc,rCaption.Width(),rCaption.Height()))
			return;
	}
	
	CBitmap* pOldBmp = dcMem.SelectObject(&m_bmpMemCaption);

	m_Theme.DrawThemePart(dcMem.m_hDC,nPartID,nFrameState,rCaption);

	// Draw the system menu.
	if (dwStyle & WS_SYSMENU)
	{
		HICON hIcon = ::AfxGetApp()->LoadIcon(m_nIDIcon);
		if(hIcon!=NULL)
			::DrawIconEx(dcMem.m_hDC,2+sizeFrame.cx,sizeFrame.cy-1+(rCaption.Height()-sizeIcon.cy-sizeFrame.cy)/2,hIcon,sizeIcon.cx,sizeIcon.cy,0,NULL,DI_NORMAL);
	}

	// Draw the buttons
	for(int i=0; i<m_aButtons.GetSize(); i++)
	{
		if(!m_aButtons[i]->m_rect.IsRectEmpty())
		{
			int nStateOffset = 4;
			if(m_aButtons[i]->m_fActive)
				nStateOffset = 0;
			switch(i)
			{
			case 0:
				m_Theme.DrawThemePart(dcMem.m_hDC,WP_CLOSEBUTTON,m_aButtons[i]->m_nState+nStateOffset,m_aButtons[i]->m_rect);
				break;
			case 1:
				{
					if(pWnd->IsZoomed())
						m_Theme.DrawThemePart(dcMem.m_hDC,WP_RESTOREBUTTON,m_aButtons[i]->m_nState+nStateOffset,m_aButtons[i]->m_rect);
					else
						m_Theme.DrawThemePart(dcMem.m_hDC,WP_MAXBUTTON,m_aButtons[i]->m_nState+nStateOffset,m_aButtons[i]->m_rect);
				}
				break;
			case 2:
				{
					if(pWnd->IsIconic())
						m_Theme.DrawThemePart(dcMem.m_hDC,WP_RESTOREBUTTON,m_aButtons[i]->m_nState+nStateOffset,m_aButtons[i]->m_rect);
					else
						m_Theme.DrawThemePart(dcMem.m_hDC,WP_MINBUTTON,m_aButtons[i]->m_nState+nStateOffset,m_aButtons[i]->m_rect);
				}
				break;
			case 3:
				m_Theme.DrawThemePart(dcMem.m_hDC,WP_HELPBUTTON,m_aButtons[i]->m_nState+nStateOffset,m_aButtons[i]->m_rect);
				break;
			default:
				m_aButtons[i]->Draw(dcMem);
				break;
			}
		}
	}

	// Draw the caption text
	
	TCHAR strTitle[512];
	pWnd->GetWindowText(strTitle,512);
	int nTextXPos = sizeFrame.cx + (dwStyle & WS_SYSMENU ? sizeIcon.cx+6 : 4);

	CPoint ptShadow(0,0);
	m_Theme.GetThemePosition(nPartID,nFrameState,TMT_TEXTSHADOWOFFSET,&ptShadow);
		
	COLORREF rgbTxtShadowColor;
	COLORREF rgbTxtColor = ::GetSysColor(m_fActive ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT);
	m_Theme.GetThemeColor(nPartID,nFrameState,TMT_TEXTSHADOWCOLOR,&rgbTxtShadowColor);
	m_Theme.GetThemeColor(nPartID,nFrameState,TMT_CAPTIONTEXT,&rgbTxtColor);
	COLORREF rgbOldTxtColor = dcMem.SetTextColor(rgbTxtShadowColor);

	CFont* pFontOld = dcMem.SelectObject(&fn);
	int nOldBkMode = dcMem.SetBkMode(TRANSPARENT);
	CRect rTxt(nTextXPos+ptShadow.x,sizeFrame.cy+ptShadow.y,rCaption.right-nBtnsWidth-2,rCaption.Height()+ptShadow.y);
	UINT uFormat = DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS;
	if(pWnd->GetExStyle()&WS_EX_RIGHT)
		uFormat |= DT_RIGHT;
	if(ptShadow!=CPoint(0,0))
		dcMem.DrawText(strTitle,rTxt,uFormat);
	dcMem.SetTextColor(rgbTxtColor);
	rTxt.OffsetRect(-ptShadow.x,-ptShadow.y);
	dcMem.DrawText(strTitle,rTxt,uFormat);
	dcMem.SetBkMode(nOldBkMode);
	dcMem.SetTextColor(rgbOldTxtColor);
	dcMem.SelectObject(pFontOld);

	dc.BitBlt(0,0,rCaption.Width(),rCaption.Height(),&dcMem,0,0,SRCCOPY);
	dcMem.SelectObject(pOldBmp);
}

CCustomBitmapButton* CCustomBitmapButton::GetCaptionButtonPtr(int nButtonID)
{
	if(!m_pOldWndProc)
		return NULL;
	for(int i=4;i<m_aButtons.GetSize();i++)
	{
		if(m_aButtons[i]->GetDlgCtrlID()==nButtonID)
			return m_aButtons[i];
	}
	return NULL;
}

BOOL CCustomBitmapButton::PreTranslateMessage(MSG* pMsg) 
{
	if(m_ctrlToolTip.m_hWnd && 
		(pMsg->message==WM_LBUTTONDOWN || pMsg->message==WM_LBUTTONUP || pMsg->message==WM_MOUSEMOVE))
			m_ctrlToolTip.RelayEvent(pMsg);	
	return CWnd::PreTranslateMessage(pMsg);
}
