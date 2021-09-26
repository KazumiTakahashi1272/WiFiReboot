#if !defined(AFX_COMBOLISTCTRLEXT_H__8C84BCCC_2D78_47B8_AD2D_6E6FC8218087__INCLUDED_)
#define AFX_COMBOLISTCTRLEXT_H__8C84BCCC_2D78_47B8_AD2D_6E6FC8218087__INCLUDED_

#include "ComboListCtrl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboListCtrlExt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComboListCtrlExt window

class CComboListCtrlExt : public CComboListCtrl
{
// Construction
public:
	CComboListCtrlExt();

protected:
	virtual void SetupComboEditItem();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboListCtrlExt)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboListCtrlExt();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboListCtrlExt)
	afx_msg BOOL OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOLISTCTRLEXT_H__8C84BCCC_2D78_47B8_AD2D_6E6FC8218087__INCLUDED_)
