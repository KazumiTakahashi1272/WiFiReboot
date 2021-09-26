#if !defined(AFX_MYCOMBOLISTCTRL_H__7B2FB58F_B9EE_4020_B38F_D73CA316D631__INCLUDED_)
#define AFX_MYCOMBOLISTCTRL_H__7B2FB58F_B9EE_4020_B38F_D73CA316D631__INCLUDED_

#include "ComboListCtrl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyComboListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyComboListCtrl window

class CMyComboListCtrl : public CComboListCtrl
{
// Construction
public:
	CMyComboListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyComboListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyComboListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyComboListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYCOMBOLISTCTRL_H__7B2FB58F_B9EE_4020_B38F_D73CA316D631__INCLUDED_)
