// CJFlatComboBox.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJFLATCOMBOBOX_H__
#define __CJFLATCOMBOBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CCJComboBox is a CComboBox derived class which is simular to
// the flat combobox as seen in the Microsoft office products.
class CCJFlatComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CCJFlatComboBox)

public:
	
	// Default constructor
	//
	CCJFlatComboBox();

	// Virtual destructor
	//
	virtual ~CCJFlatComboBox();

protected:

	int			m_nOffset;		// offset used during paint.
	BOOL		m_bLBtnDown;	// TRUE if left mouse button is pressed
	BOOL		m_bPainted;		// used during paint operations
	BOOL		m_bHasFocus;	// TRUE if control has focus
	BOOL		m_bAutoComp;	// Used by Autocompleting.
	COLORREF	m_clrBtnHilite;	// set to the system color COLOR_BTNHILIGHT
	COLORREF	m_clrBtnShadow;	// set to the system color COLOR_BTNSHADOW
	COLORREF	m_clrBtnFace;	// set to the system color COLOR_BTNFACE

	// enum used to determine the state the combo box should be
	//
	enum STATE { normal = 1, raised = 2, pressed = 3 };

public:

	// Call this member function to enable auto completion.
	//
	void EnableAutoCompletion(BOOL bEnable=TRUE);

protected:

	// this member function is called by the combo box whenever a paint
	// operation should occur.
	//
	void DrawCombo(STATE eState, COLORREF clrTopLeft, COLORREF clrBottomRight);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJFlatComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJFlatComboBox)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus();
	afx_msg void OnKillFocus();
	afx_msg void OnEditUpdate();
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

inline void CCJFlatComboBox::EnableAutoCompletion(BOOL bEnable/*=TRUE*/)
	{ ASSERT(::IsWindow(m_hWnd)); m_bAutoComp = bEnable; }

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CJCOMBOBOX_H__
