
// WiFiRebootDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"

#include <string>       // ヘッダファイルインクルード
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
	CEdit m_ctrlEditSsid;

private:
	DWORD RunCmdProc(string cmd);
};
