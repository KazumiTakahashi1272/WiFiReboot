#if !defined(AFX_COMBOLISTCTRL_H__1E713052_65AE_4C12_B17B_5B24DADBC429__INCLUDED_)
#define AFX_COMBOLISTCTRL_H__1E713052_65AE_4C12_B17B_5B24DADBC429__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboListCtrl.h : header file
//

#include "SkinListCtrl.h"
#include "SkinHeaderCtrl.h"
#include "SkinHorizontalScrollbar.h"
#include "SkinVerticleScrollbar.h"

#ifndef WMU_CLOSE_CONTROL
#define WMU_CLOSE_CONTROL					(WM_USER + 32)
#endif
#ifndef NM_DISPLAYLIST
#define	NM_DISPLAYLIST						(WM_USER + 33)
#endif
/////////////////////////////////////////////////////////////////////////////
// CComboListCtrl window

class CListComboBox;

class CComboListCtrl : public CSkinListCtrl
{
// Construction
public:
	CComboListCtrl();

protected:
	CListComboBox* m_pCombo;
	int m_nListItemHeight;

// Attributes
public:
	void InitSkin(void);

// Operations
public:
	virtual CRect GetDropDownRect();
	virtual void Display(CRect rect);
	void Init(CListComboBox* pCombo){m_pCombo = pCombo;}
	void SetListItemHeight(int nHeight){m_nListItemHeight = nHeight;}
	int GetListItemHeight(){return m_nListItemHeight;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboListCtrl)
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOLISTCTRL_H__1E713052_65AE_4C12_B17B_5B24DADBC429__INCLUDED_)
