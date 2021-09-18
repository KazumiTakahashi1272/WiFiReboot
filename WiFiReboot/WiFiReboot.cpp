
// WiFiReboot.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "WiFiReboot.h"
#include "WiFiRebootDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWiFiRebootApp

BEGIN_MESSAGE_MAP(CWiFiRebootApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWiFiRebootApp コンストラクション

CWiFiRebootApp::CWiFiRebootApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CWiFiRebootApp オブジェクトです。

CWiFiRebootApp theApp;


// CWiFiRebootApp 初期化

BOOL CWiFiRebootApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	CWiFiRebootDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//  記述してください。
	}

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

int CWiFiRebootApp::ExitInstance()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	return CWinAppEx::ExitInstance();
}
