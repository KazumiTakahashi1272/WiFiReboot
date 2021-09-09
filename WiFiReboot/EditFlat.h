
#if !defined(AFX_EDITFLAT_H__300DD998_4052_41E0_BB91_A15267C90623__INCLUDED_)
#define AFX_EDITFLAT_H__300DD998_4052_41E0_BB91_A15267C90623__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditFlat.h : header file
//
#include <mmsystem.h>

// setup system static library for link
#pragma comment (lib, "winmm.lib")

/////////////////////////////////////////////////////////////////////////////
// CEditFlat window

// Flat edit control MFC class
class CEditFlat : public CEdit
{

public:

	// Set up sounds
	// LPCTSTR pStrActivateSound = sound when mouse on control activated
	// LPCTSTR pStrVerticalSound = sound when vertical scroll clicked
	// LPCTSTR pStrHorizontalSound = sound when horizontal scroll clicked
	BOOL SetSounds (LPCTSTR pStrActivateSound, LPCTSTR pStrVerticalSound = NULL, LPCTSTR pStrHorizontalSound = NULL);

	// Set up resource instance
	// (sound will be load from this resource)
	// if function was not called, default instance is used
	BOOL SetResourceHandle (HINSTANCE hInstance);


public:
	// Default constructor
	CEditFlat();
	// Destructor
	virtual ~CEditFlat();

// Overrides
// ClassWizard generated virtual function overrides
//{{AFX_VIRTUAL(CEditFlat)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	
	BOOL DrawEdit (VOID);

	// Handle of main applications
	HINSTANCE m_hInstance;
	
	// Sound names
	CString m_StrVScrollSound;
	CString m_StrHScrollSound;
	CString m_StrActivateSound;
	
	// class variables
	BOOL m_bIsActivateSound;
	BOOL m_bIsVScrollSound;
	BOOL m_bIsHScrollSound;
	BYTE m_bHorizontalFrameWidth;
	BYTE m_bVerticalFrameWidth;
	BYTE m_iHorizontalScrollWidth;
	BYTE m_iVerticalScrollWidthLeft;
	BYTE m_iVerticalScrollWidthRight;
	BOOL GetSysColors (VOID);
	BOOL m_bIsFocused;
	
	// Dafault system colors
	COLORREF m_clrButton;
	COLORREF m_clrDarkShadow;
	COLORREF m_clrShadow;
	COLORREF m_clrHiLite;

	// this variable is sets when mouse on edit
	BOOL m_bMouseOnEdit;
	BOOL m_bHScrollPressed;
	BOOL m_bVScrollPressed;
	CString m_StrWaveName;

	//{{AFX_MSG(CEditFlat)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSysColorChange();
	afx_msg void OnStyleChanging( int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITFLAT_H__300DD998_4052_41E0_BB91_A15267C90623__INCLUDED_)
