#if !defined(AFX_LISTCOMBOBOX_H__63E2E52E_6DAD_41E3_A41F_58F69D623035__INCLUDED_)
#define AFX_LISTCOMBOBOX_H__63E2E52E_6DAD_41E3_A41F_58F69D623035__INCLUDED_

#include "ComboListCtrl.h"
#include "CJFlatComboBox.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListComboBox window

class CListComboBox : public CCJFlatComboBox
{
// Construction
public:
	CListComboBox();

// Attributes
public:

// Operations
public:
	virtual void DisplayList();
	virtual int SetEditText(LPCTSTR lpszString);
	virtual int SetEditItemData(int nItemData);
	virtual int GetEditItemData();
	BOOL IsAlertBkg() const {return m_bAlertBkg;}
	BOOL IsAlertText() const {return m_bAlertText;}
	void SetListCtrl(CComboListCtrl* pListCtrl){m_pListCtrl = pListCtrl;}
	BOOL IsControlActive() const {return m_bControlActive;}
	int GetDroppedWidth() const {return m_nDroppedWidth;}
	int GetDroppedHeight() const {return m_nDroppedHeight;}
	void SetDroppedWidth(const int nWidth){m_nDroppedWidth = nWidth;}
	void SetDroppedHeight(const int nHeight){m_nDroppedHeight = nHeight;}
	void AlertBkg(const BOOL bAlert = TRUE, BOOL bRedraw = FALSE)
	{
		m_bAlertBkg = bAlert;
		if(bRedraw)
			Invalidate();
	}
	void AlertText(const BOOL bAlert = TRUE, BOOL bRedraw = FALSE)
	{
		m_bAlertText = bAlert;
		if(bRedraw)
			Invalidate();
	}
	void SetEditTooltip(const BOOL bShowTooltip, BOOL bTooltipOnInfo = FALSE, BOOL bShowEditTooltipOverItem = TRUE)
	{
		m_bShowTooltip = bShowTooltip;
		m_bTooltipOnInfo = bTooltipOnInfo;
		m_bShowEditTooltipOverItem = bShowEditTooltipOverItem;
	}
	void GetEditTooltip(BOOL& bShowTooltip, BOOL& bTooltipOnInfo, BOOL& bShowEditTooltipOverItem)
	{
		bShowTooltip = m_bShowTooltip;
		bTooltipOnInfo = m_bTooltipOnInfo;
		bShowEditTooltipOverItem = m_bShowEditTooltipOverItem;
	}
	BOOL GetShowTooltip() const {return m_bShowTooltip;}
	void SetShowTooltip(const BOOL bSet){m_bShowTooltip = bSet;}
	BOOL GetTooltipOnInfo() const {return m_bTooltipOnInfo;}
	void SetTooltipOnInfo(const BOOL bSet){m_bTooltipOnInfo = bSet;}
	BOOL GetShowEditTooltipOverItem() const {return m_bShowEditTooltipOverItem;}
	void SetShowEditTooltipOverItem(const BOOL bSet){m_bShowEditTooltipOverItem = bSet;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListComboBox();
	void SetAlertColorBkg(const COLORREF crColor);
	COLORREF GetAlertColorBkg() const {return m_crAlertBkg;}
	void SetAlertColorText(const COLORREF crColor){m_crAlertText = crColor;}
	COLORREF GetAlertColorText() const {return m_crAlertText;}
	void SetTooltipText(CString sText){m_sTooltipText = sText;}

protected:
	HWND m_hWndToolTip;
	TOOLINFO m_ToolInfo;
	CString m_sTooltipText;
	CComboListCtrl* m_pListCtrl;

protected:
	BOOL m_bAlertBkg;
	BOOL m_bAlertText;
	BOOL m_bControlActive;
	int m_nDroppedWidth;
	int m_nDroppedHeight;
	CBrush m_BrushAlert;
	COLORREF m_crAlertBkg;
	COLORREF m_crAlertText;
	BOOL m_bShowTooltip;
	BOOL m_bTooltipActive;
	BOOL m_bTooltipOnInfo;
	BOOL m_bShowEditTooltipOverItem;

// Generated message map functions
protected:
	//{{AFX_MSG(CListComboBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnCloseControl(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCOMBOBOX_H__63E2E52E_6DAD_41E3_A41F_58F69D623035__INCLUDED_)
