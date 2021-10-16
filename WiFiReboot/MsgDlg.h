#pragma once

#include "WiFiRebootDlg.h"
#include "afxwin.h"
#include "ThemeHelperST.h"
#include "XPStyleButtonST.h"

// CMsgDlg ダイアログ
using namespace std;         //  名前空間指定

class CMsgDlg : public CDialog
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(LPCTSTR ssid, CWiFiRebootDlg* pParent = NULL);   // 標準コンストラクタ
	virtual ~CMsgDlg();

	CThemeHelperST		m_ThemeHelper;

	CWiFiRebootDlg* m_pParent;
	string m_ssid;

	string m_msg;

// ダイアログ データ
	enum { IDD = IDD_MSG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_ctrlMsgEdit;
	CXPStyleButtonST m_ctrlOk;
	CXPStyleButtonST m_ctrlCancel;
};
