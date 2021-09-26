// ComboListCtrlExt.cpp : implementation file
//

#include "stdafx.h"
#include "ComboListCtrlExt.h"

#include "ListComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboListCtrlExt

CComboListCtrlExt::CComboListCtrlExt()
{
}

CComboListCtrlExt::~CComboListCtrlExt()
{
}

BEGIN_MESSAGE_MAP(CComboListCtrlExt, CComboListCtrl)
	//{{AFX_MSG_MAP(CComboListCtrlExt)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT_EX(NM_DBLCLK, CComboListCtrlExt::OnNMDblclk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboListCtrlExt message handlers

void CComboListCtrlExt::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Catch event before the parent listctrl gets it to avoid extra scrolling
	//	- OBS! This can also prevent the key-events to reach LVN_KEYDOWN handlers

	if(VK_RETURN == nChar)
		SetupComboEditItem();

	CComboListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CComboListCtrlExt::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetupComboEditItem();

	m_pCombo->SetFocus();

	return *pResult;
}

void CComboListCtrlExt::SetupComboEditItem()
{
	int nItem = GetSelectionMark();
	TRACE1(_T("Item selected: %d\n"), nItem);
	if(1 == GetSelectedCount() && nItem >= 0)
	{
		TRACE1(_T("ItemData selected: %d\n"), GetItemData(nItem));
		m_pCombo->SetEditText(GetItemText(nItem, 0));
		m_pCombo->SetEditItemData(GetItemData(nItem));
	}
	else
	{
		m_pCombo->ResetContent();
	}
}
