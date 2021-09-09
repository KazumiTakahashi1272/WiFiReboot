
// WiFiRebootDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include <cstdio>
#include <iostream>
#include <string>       // ヘッダファイルインクルード
#include "xpbutton.h"
#include "afxcmn.h"
using namespace std;         //  名前空間指定


// CWiFiRebootDlg ダイアログ
class CWiFiRebootDlg : public CDialog
{
// コンストラクション
public:
	CWiFiRebootDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_WIFIREBOOT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

public:
	static void WlanNotification(WLAN_NOTIFICATION_DATA *wlanNotifData, VOID *p);

	void MsgReporter(const char *format, ...);
	void ErrReporter(const char *format, ...);

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedWifiReboot();

private:
	DWORD RunCmdProc(string cmd);
public:
	CComboBox m_ctrlCbSSID;
	CXPButton m_ctrlWifiReboot;
	CProgressCtrl m_ctrlRebootProg;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
