// ListComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "ListComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListComboBox

CListComboBox::CListComboBox()
	:m_bControlActive(FALSE)
	,m_pListCtrl(NULL)
	,m_bAlertBkg(FALSE)
	,m_bAlertText(FALSE)
	,m_nDroppedHeight(150)
	,m_nDroppedWidth(100)
	,m_bShowTooltip(FALSE)
	,m_bTooltipActive(FALSE)
	,m_bTooltipOnInfo(FALSE)
	,m_bShowEditTooltipOverItem(TRUE)
{
	m_hWndToolTip = NULL;
	m_crAlertBkg = GetSysColor(COLOR_WINDOW);
	m_crAlertText = GetSysColor(COLOR_WINDOWTEXT);
	m_BrushAlert.CreateSolidBrush(m_crAlertBkg);
}

CListComboBox::~CListComboBox()
{
	m_BrushAlert.DeleteObject();
}

BEGIN_MESSAGE_MAP(CListComboBox, CCJFlatComboBox)
	//{{AFX_MSG_MAP(CListComboBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WMU_CLOSE_CONTROL, OnCloseControl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListComboBox message handlers

void CListComboBox::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

	CCJFlatComboBox::PreSubclassWindow();

	ASSERT(NULL != m_pListCtrl);	// use dynamic creation

	CRect rect(0, 0, 0, 0);
	DWORD dwStyle =  WS_POPUP | WS_BORDER;
	CWnd* pWnd = m_pListCtrl;
	pWnd->CreateEx(0, WC_LISTVIEW, NULL, dwStyle, rect, GetParent(), 0, NULL);

	m_pListCtrl->Init(this);

	GetClientRect(rect);
	SetDroppedWidth(rect.Width());
	SetDroppedHeight(m_nDroppedHeight);

	ASSERT(CBS_SIMPLE != (GetStyle() & CBS_DROPDOWNLIST));

	// create tooltip
	m_hWndToolTip = ::CreateWindowEx(WS_EX_TOPMOST, 
							  TOOLTIPS_CLASS, 
							  NULL, 
							  TTS_NOPREFIX | TTS_ALWAYSTIP, 
							  CW_USEDEFAULT, 
							  CW_USEDEFAULT, 
							  CW_USEDEFAULT, 
							  CW_USEDEFAULT, 
							  NULL, 
							  NULL, 
							  NULL, 
							  NULL);
	ASSERT(m_hWndToolTip);
	// initialize toolinfo struct
	memset(&m_ToolInfo, 0, sizeof(m_ToolInfo));
#ifdef _UNICODE
	m_ToolInfo.cbSize = sizeof(TTTOOLINFOW_V2_SIZE);
#else
	m_ToolInfo.cbSize = sizeof(TTTOOLINFOA_V2_SIZE);
#endif
	m_ToolInfo.uFlags = TTF_TRACK | TTF_TRANSPARENT;
	m_ToolInfo.hwnd = m_hWnd;
	::SendMessage(m_hWndToolTip, TTM_SETMAXTIPWIDTH, (WPARAM)0, (LPARAM)SHRT_MAX);
	::SendMessage(m_hWndToolTip, TTM_ADDTOOL, (WPARAM)0, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
}

BOOL CListComboBox::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	if(WM_KEYDOWN == pMsg->message && (VK_DOWN == pMsg->wParam || VK_F4 == pMsg->wParam))
	{
		DisplayList();
		return TRUE;
	}

	return CCJFlatComboBox::PreTranslateMessage(pMsg);
}

void CListComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	m_bControlActive = ! m_bControlActive;
	if(m_bControlActive)
		DisplayList();

//	CComboBox::OnLButtonDown(nFlags, point);
}

void CListComboBox::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	OnLButtonDown(nFlags, point);

//	CComboBox::OnLButtonDblClk(nFlags, point);
}

LRESULT CListComboBox::OnCloseControl(WPARAM wParam, LPARAM lParam)
{
	::SendMessage(GetParent()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_CLOSEUP), (LPARAM)m_hWnd);
	if(NULL != m_pListCtrl->GetSafeHwnd())
	{
		m_pListCtrl->ShowWindow(SW_HIDE);
	}
	m_bControlActive = FALSE;
	if(GetCount() > 0)
		SetCurSel(0);
	SetFocus();

	return 1;
}

BOOL CListComboBox::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	// TODO: Add your specialized code here and/or call the base class

	if((! m_bAlertText && ! m_bAlertBkg) || WM_CTLCOLOREDIT != message)
		return CCJFlatComboBox::OnChildNotify(message, wParam, lParam, pLResult);

	HDC hdcChild = (HDC)wParam;
	if(NULL != hdcChild)
	{
		if(m_bAlertText)
			SetTextColor(hdcChild, m_crAlertText);
		if(m_bAlertBkg)
			SetBkColor(hdcChild, m_crAlertBkg);
		*pLResult = (LRESULT)(m_BrushAlert.GetSafeHandle());
	}

	return TRUE;
//	return CComboBox::OnChildNotify(message, wParam, lParam, pLResult);
}

void CListComboBox::SetAlertColorBkg(const COLORREF crColor)
{
	m_crAlertBkg = crColor;
	m_BrushAlert.DeleteObject();
	m_BrushAlert.CreateSolidBrush(m_crAlertBkg);
}

void CListComboBox::DisplayList()
{
	NMHDR nm;
	nm.hwndFrom = GetSafeHwnd();
	nm.idFrom  = GetDlgCtrlID();
	nm.code = NM_DISPLAYLIST;
	::SendMessage(GetParent()->GetSafeHwnd(), WM_NOTIFY, nm.idFrom, (LPARAM)&nm);

	CRect rect = m_pListCtrl->GetDropDownRect();
	m_pListCtrl->Display(rect);
}

int CListComboBox::SetEditText(LPCTSTR lpszString)
{
	int nItemData = CB_ERR;
	if(1 == GetCount())
		nItemData = GetItemData(0);
	ResetContent();
	int nIndex = AddString(lpszString);
	if(CB_ERR != nIndex)
		SetItemData(nIndex, nItemData);

	return SetCurSel(nIndex);
}

int CListComboBox::SetEditItemData(int nItemData)
{
	if(1 != GetCount())
		return CB_ERR;

	return SetItemData(0, nItemData);
}

int CListComboBox::GetEditItemData()
{
	if(1 != GetCount())
		return CB_ERR;

	return GetItemData(0);
}

void CListComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CCJFlatComboBox::OnMouseMove(nFlags, point);

	if(! m_bShowTooltip)
		return;

	CRect rectClient;
	GetClientRect(&rectClient);
	const int nComboButtonWidth = GetSystemMetrics(SM_CXHTHUMB) + 2;
	rectClient.right = rectClient.right - nComboButtonWidth;

	if(rectClient.PtInRect(point) && ! m_bControlActive)
	{
		const int nIndex = GetCurSel();
		ClientToScreen(&rectClient);

		CString sText;
		if(! m_bTooltipOnInfo)
			GetWindowText(sText);
		else
			sText = m_sTooltipText;
		m_ToolInfo.lpszText = (LPTSTR)(LPCTSTR)sText;

		if(! m_bTooltipOnInfo)
		{
			LOGFONT lf;
			::memset(&lf,0,sizeof(lf));
			::strcpy(lf.lfFaceName,"MS UI Gothic");

			HDC hDC = ::GetDC(m_hWnd);
			ASSERT(hDC);
			CFont* pFont = GetFont();
			pFont->CreateFontIndirect( &lf );
			HFONT hOldFont = (HFONT)::SelectObject(hDC, (HFONT)*pFont);
			SIZE size;
			::GetTextExtentPoint32(hDC, sText, sText.GetLength(), &size);
			::SelectObject(hDC, hOldFont);
			::ReleaseDC(m_hWnd, hDC);

			if(! m_bTooltipActive && size.cx > rectClient.Width() - 6)
			{
				rectClient.left += 1;
				rectClient.top += 3;
				::SendMessage(m_hWndToolTip, TTM_UPDATETIPTEXT, (WPARAM)0, (LPARAM)&m_ToolInfo);
				::SendMessage(m_hWndToolTip, TTM_TRACKPOSITION, (WPARAM)0, (LPARAM)MAKELONG(rectClient.left, m_bShowEditTooltipOverItem ? rectClient.top : rectClient.top - nComboButtonWidth - 1));
				::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
				m_bTooltipActive = TRUE;
				SetTimer(1, 80, NULL);
			}
			else	// text fits inside client rect
			{
				SetTimer(1, 80, NULL);
			}
		}
		else	// tooltip text is retrieved from m_sTooltipText
		{
			if(! m_bTooltipActive && ! sText.IsEmpty())
			{
				::SendMessage(m_hWndToolTip, TTM_UPDATETIPTEXT, (WPARAM)0, (LPARAM)&m_ToolInfo);
				::SendMessage(m_hWndToolTip, TTM_TRACKPOSITION, (WPARAM)0, (LPARAM)MAKELONG(rectClient.left, m_bShowEditTooltipOverItem ? rectClient.top : rectClient.top - nComboButtonWidth - 1));
				::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
				m_bTooltipActive = TRUE;
				SetTimer(1, 80, NULL);
			}
			else
			{
				SetTimer(1, 80, NULL);
			}
		}
	}
	else	// not inside client rect
	{
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
		m_bTooltipActive = FALSE;
		SetTimer(1, 80, NULL);
	}
}

void CListComboBox::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	CCJFlatComboBox::OnTimer(nIDEvent);

	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);

	CRect rectClient;
	GetClientRect(&rectClient);
	int nComboButtonWidth = GetSystemMetrics(SM_CXHTHUMB) + 2;
	rectClient.right = rectClient.right - nComboButtonWidth;

	if(! rectClient.PtInRect(point))
	{
		KillTimer(nIDEvent);
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
		m_bTooltipActive = FALSE;
	}
}
