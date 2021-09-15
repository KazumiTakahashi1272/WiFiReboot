
// WiFiRebootDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "resource.h"		// メイン シンボル

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include <cstdio>
#include <iostream>
#include <string>       // ヘッダファイルインクルード
#include "xpbutton.h"
#include "afxcmn.h"
#include "CJFlatComboBox.h"
#include "EditFlat.h"
#include "ColorStatic.h"
#include "ColorEdit.h"

using namespace std;         //  名前空間指定

#include "BtnST.h"

// CWiFiRebootDlg ダイアログ
class CWiFiRebootDlg : public CDialog
{
// コンストラクション
public:
	CWiFiRebootDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

	HBITMAP m_hBmp;
	HRGN m_hClientRgn;
	HRGN m_hWndRgn;
	HRGN DIBToRgn(HBITMAP hBmp,COLORREF BkColor,BOOL Direct); // Handle the Skin

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

public:
	CButtonST	m_btnCancel;
	CButtonST	m_btnMinimize;

	T_CRYPTO_DATA	m_crypto;	// 暗号化情報
	HANDLE m_hCrypto;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMinimize();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedWifiReboot();

private:
	DWORD RunCmdProc(string cmd);

public:
	CCJFlatComboBox m_ctrlCbSSID;
	CXPButton m_ctrlWifiReboot;
	CProgressCtrl m_ctrlRebootProg;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEditFlat m_ctrlPsw;
	afx_msg void OnCbnSelchangeCbSsid();
	CColorStatic m_ctrlDesc;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CColorStatic m_ctrlStaticSsid;
	CColorStatic m_ctrlStaticPwd;
};
