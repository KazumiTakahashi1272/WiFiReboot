// ComboListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ComboListCtrl.h"

#include "ListComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboListCtrl

CComboListCtrl::CComboListCtrl()
	:m_pCombo(NULL)
	,m_nListItemHeight(16)
{
}

CComboListCtrl::~CComboListCtrl()
{
}

BEGIN_MESSAGE_MAP(CComboListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CComboListCtrl)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboListCtrl message handlers

void CComboListCtrl::Display(CRect rect)
{
	if(NULL != GetSafeHwnd())
	{
		SetWindowPos(NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOOWNERZORDER);
		CWnd* pTopParent = GetParent()->GetParentOwner();
		if(NULL != pTopParent->GetSafeHwnd())
		{
			pTopParent->SendMessage(WM_NCACTIVATE, (WPARAM)TRUE, (LPARAM)0);
			pTopParent->SetRedraw(TRUE);
		}
	}
}

BOOL CComboListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	if(pMsg->hwnd == m_hWnd && WM_SYSKEYDOWN == pMsg->message && VK_F4 == pMsg->wParam)
		return TRUE;

	if(pMsg->hwnd == m_hWnd && WM_KEYDOWN == pMsg->message &&
		(VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam))
	{
		m_pCombo->SetFocus();
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

void CComboListCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	::PostMessage(m_pCombo->GetSafeHwnd(), WMU_CLOSE_CONTROL, (WPARAM)0, (LPARAM)0);

	*pResult = 0;
}

CRect CComboListCtrl::GetDropDownRect()
{
	CRect rect;
	m_pCombo->GetWindowRect(&rect);

	int nDroppedWidth = rect.Width();
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	int nDesiredDroppedHeight, nDroppedHeight = m_pCombo->GetDroppedHeight();

	int nCount = GetItemCount();
	if(nCount > 1)
	{
		if(LVS_REPORT == (GetStyle() & LVS_TYPEMASK))
			nCount++;
		if(LVS_EX_CHECKBOXES & GetExtendedStyle())
			nCount++;
		nDesiredDroppedHeight = nCount * (m_nListItemHeight + GetSystemMetrics(SM_CXEDGE) / 2);
		if(nDesiredDroppedHeight > nScreenHeight / 2)
			nDesiredDroppedHeight = nScreenHeight / 2;
		if(nDroppedHeight < nDesiredDroppedHeight)
			nDroppedHeight = nDesiredDroppedHeight;
	}

	if(rect.bottom + nDroppedHeight + GetSystemMetrics(SM_CYCAPTION) > nScreenHeight)
	{
		rect.top = rect.top - GetSystemMetrics(SM_CXEDGE) - nDroppedHeight;
		rect.bottom = nDroppedHeight + 1;
	}
	else
	{
		rect.top = rect.bottom;
		rect.bottom = nDroppedHeight;
	}

	CRect rectHeader;
	int nHeaderWidth = 0;
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	for(int i = 0;i < pHeaderCtrl->GetItemCount();++i)
	{
		rectHeader.SetRectEmpty();
		pHeaderCtrl->GetItemRect(i, &rectHeader);
		nHeaderWidth += rectHeader.Width();
	}
	if(nHeaderWidth > nDroppedWidth)
		rect.right = nHeaderWidth;
	else
		rect.right = nDroppedWidth;

	return rect;
}
