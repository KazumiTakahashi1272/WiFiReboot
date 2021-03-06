
// WiFiReboot.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル
#include "D:\Samples\Tutorial\CryptoRegApp\CryptoRegApp\CryptoReg.h"

#pragma comment(lib, "CryptoReg.lib")

// CWiFiRebootApp:
// このクラスの実装については、WiFiReboot.cpp を参照してください。
//

class CWiFiRebootApp : public CWinAppEx
{
public:
	CWiFiRebootApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

};

extern CWiFiRebootApp theApp;