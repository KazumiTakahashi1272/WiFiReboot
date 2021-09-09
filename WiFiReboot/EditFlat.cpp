// EditFlat.cpp : implementation file
//
// Legal copyright:
//
// Author:
//
// Avtukhov Roman Victorovich
// Lipetsk, RUSSIAN FEDERATION.
// E-Mail: riku_rv@yahoo.com
// all rights reserved.
//
// This code provided is for FREE (free changed, distribute, copy and others.
// I will be very glad, if You place my name in Your application where this code
// will be used.
// Best regards!
 

#include "stdafx.h"
#include "EditFlat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditFlat

BEGIN_MESSAGE_MAP(CEditFlat, CEdit)
	//{{AFX_MSG_MAP(CEditFlat)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	// do not remove
	ON_WM_STYLECHANGING ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CEditFlat::CEditFlat()
{
	CWinApp *pMainApplication;
	
	pMainApplication = AfxGetApp ();

	m_hInstance = pMainApplication->m_hInstance;

	m_bVScrollPressed			= TRUE;
	m_bHScrollPressed			= TRUE;

	m_bIsActivateSound			= FALSE;
	m_bIsHScrollSound			= FALSE;
	m_bIsVScrollSound			= FALSE;
	m_bMouseOnEdit				= FALSE;

	m_bIsFocused				= FALSE;

	m_iHorizontalScrollWidth	= NULL;

	m_iVerticalScrollWidthLeft  = NULL;

	m_iVerticalScrollWidthRight = NULL;

	m_bHorizontalFrameWidth		= NULL;

	m_bVerticalFrameWidth		= NULL;

	GetSysColors ();
}

CEditFlat::~CEditFlat()
{
}


void CEditFlat::OnMouseMove(UINT nFlags, CPoint point) 
{	
	CEdit::OnMouseMove(nFlags, point);

	if (m_bIsFocused) return;

	if (GetCapture() != this) 
	{
		m_bMouseOnEdit = TRUE;
		SetCapture();
		DrawEdit();

		// Play sound
		if (m_bIsActivateSound)
		{
			::PlaySound ((LPCTSTR)m_StrActivateSound, m_hInstance, SND_ASYNC | SND_RESOURCE);
		}
	}
	else
	{
		CRect RectEditControl;
		GetClientRect(&RectEditControl);

		if (!RectEditControl.PtInRect(point))
		{
				m_bMouseOnEdit = FALSE;
				DrawEdit();
				ReleaseCapture();

		}
	}

}

void CEditFlat::OnPaint() 
{	
	// Process default (do not remove)
	Default ();

	// Process control drawing (main!)
	DrawEdit();	
}


BOOL CEditFlat::DrawEdit(VOID)
{
	CRect RectEditControl;

	GetClientRect (&RectEditControl);

	CDC *pEditControlDC;

	pEditControlDC = GetDC ();

	// do not remove
	// this algoritm is calculated client size with use or not horizontal
	// and vertical scroll bars
	RectEditControl.top	   -= m_bHorizontalFrameWidth;

	RectEditControl.bottom += m_iHorizontalScrollWidth;
	RectEditControl.bottom += m_bHorizontalFrameWidth;

	RectEditControl.right  += m_iVerticalScrollWidthRight;
	RectEditControl.right  += m_bVerticalFrameWidth;

	RectEditControl.left   -= m_iVerticalScrollWidthLeft;
	RectEditControl.left   -= m_bVerticalFrameWidth;
	
	if (m_bMouseOnEdit)
	{
		// Mouse on edit

		pEditControlDC->Draw3dRect (&RectEditControl, m_clrShadow, m_clrHiLite);

		RectEditControl.DeflateRect (1,1);
			
		pEditControlDC->Draw3dRect (&RectEditControl, m_clrDarkShadow, m_clrButton);
	
	} else
	{

		// Mouse is not on edit

		pEditControlDC->Draw3dRect (&RectEditControl, m_clrHiLite, m_clrHiLite);

		RectEditControl.DeflateRect (1,1);
			
		pEditControlDC->Draw3dRect (&RectEditControl, m_clrHiLite, m_clrHiLite);

	}

	// Release DC ! do not remove!

	ReleaseDC (pEditControlDC);

	return (TRUE);
}

void CEditFlat::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);
		
	m_bIsFocused = TRUE;

	m_bMouseOnEdit = TRUE;

	Invalidate ();

}

void CEditFlat::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	m_bIsFocused = FALSE;

	m_bMouseOnEdit = FALSE;

	Invalidate ();
}

BOOL CEditFlat::GetSysColors(VOID)
{
	// set up system colors, used for borders of edit control
	m_clrHiLite		=	GetSysColor(COLOR_BTNHIGHLIGHT);
	m_clrShadow		=	GetSysColor(COLOR_BTNSHADOW);
	m_clrDarkShadow =	GetSysColor(COLOR_3DDKSHADOW);
	m_clrButton		=	GetSysColor(COLOR_BTNFACE);

	return (TRUE);
}

void CEditFlat::OnSysColorChange() 
{
	// system colors was changed - need to change edit colors !
	CEdit::OnSysColorChange();
	
	GetSysColors ();
}


void CEditFlat::PreSubclassWindow() 
{
	DWORD dwEditStyle;

	dwEditStyle = GetStyle ();

	DWORD dwEditStyleEx;

	dwEditStyleEx = GetExStyle ();

	if (dwEditStyle & WS_VSCROLL)
	{
		// Has a vertical scroll bar
		m_iVerticalScrollWidthRight =  (BYTE) ::GetSystemMetrics (SM_CXVSCROLL);

		if (dwEditStyleEx & WS_EX_LEFTSCROLLBAR)
		{
			m_iVerticalScrollWidthLeft = m_iVerticalScrollWidthRight;
			m_iVerticalScrollWidthRight = NULL;

		}
	} 

	if (dwEditStyle & WS_HSCROLL)
	{
		m_iHorizontalScrollWidth = (BYTE) ::GetSystemMetrics (SM_CYHSCROLL);
	}

	m_bHorizontalFrameWidth  = (BYTE) ::GetSystemMetrics (SM_CYEDGE);
	m_bVerticalFrameWidth    = (BYTE) ::GetSystemMetrics (SM_CXEDGE);


	CEdit::PreSubclassWindow();
}



void CEditFlat::OnStyleChanging( int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	// style changed - for example, right-to-left reading order was changed
	if (nStyleType == GWL_EXSTYLE)
	{
		if (lpStyleStruct->styleNew & WS_EX_LEFTSCROLLBAR)
		{
			m_iVerticalScrollWidthLeft = m_iVerticalScrollWidthRight;
			m_iVerticalScrollWidthRight = NULL;

		} else

		if (lpStyleStruct->styleOld & WS_EX_LEFTSCROLLBAR)
		{
			m_iVerticalScrollWidthRight = m_iVerticalScrollWidthLeft;
			m_iVerticalScrollWidthLeft = NULL;

		}

	}

	CEdit::OnStyleChanged (nStyleType, lpStyleStruct);
}



BOOL CEditFlat::SetSounds (LPCTSTR pStrActivateSound, LPCTSTR pStrVerticalSound, LPCTSTR pStrHorizontalSound)
{

	BOOL bReturnValue;
	
	bReturnValue = FALSE;

	if (FindResource (m_hInstance, pStrActivateSound, m_StrWaveName))
	{
		m_StrActivateSound = pStrActivateSound;
		m_bIsActivateSound = TRUE;
		bReturnValue = TRUE;
	}

	if (pStrVerticalSound)
	{
		if (FindResource (m_hInstance, pStrVerticalSound, m_StrWaveName))
		{
			m_StrVScrollSound  = pStrVerticalSound;
			m_bIsVScrollSound  = TRUE;
		} else
		{
			bReturnValue = FALSE;
		}
	}

	if (pStrHorizontalSound)
	{
		if (FindResource (m_hInstance, pStrHorizontalSound, m_StrWaveName))
		{
			m_StrHScrollSound  = pStrHorizontalSound;
			m_bIsHScrollSound  = TRUE;
		} else
		{
			bReturnValue = FALSE;
		}
	}

	return (bReturnValue);
}

void CEditFlat::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (m_bIsVScrollSound)
	{
		if (m_bVScrollPressed)
		{
			::PlaySound ((LPCTSTR)m_StrVScrollSound, m_hInstance, SND_ASYNC | SND_RESOURCE);
			m_bVScrollPressed = FALSE;
		} else
		{
			m_bVScrollPressed = TRUE;
		}
	}
	CEdit::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CEditFlat::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	
	if (m_bIsHScrollSound)
	{
		if (m_bHScrollPressed)
		{
			::PlaySound ((LPCTSTR)m_StrHScrollSound, m_hInstance, SND_ASYNC | SND_RESOURCE );
			m_bHScrollPressed = FALSE;
		} else
		{
			m_bHScrollPressed = TRUE;
		}
	}
	CEdit::OnHScroll(nSBCode, nPos, pScrollBar);
}



BOOL CEditFlat::SetResourceHandle(HINSTANCE hInstance)
{
	m_hInstance = hInstance;

	return (TRUE);
}






