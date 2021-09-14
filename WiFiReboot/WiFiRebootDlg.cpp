
// WiFiRebootDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "WiFiReboot.h"
#include "WiFiRebootDlg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <wchar.h>

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWiFiRebootDlg ダイアログ

static bool bScanWait = true;


void CWiFiRebootDlg::WlanNotification(WLAN_NOTIFICATION_DATA *wlanNotifData, VOID *p)
{
	//ロケール指定
	setlocale(LC_ALL, "japanese");
	WCHAR strReason[1024] = { NULL };
	WCHAR strError[1024] = { NULL };

	//通知元をauto configuration module(ACM)に設定
	if ( wlanNotifData->NotificationSource == WLAN_NOTIFICATION_SOURCE_ACM )
	{
		PWLAN_CONNECTION_NOTIFICATION_DATA pConnNotifData = NULL;
		WCHAR *notificationMessage;

		switch ( wlanNotifData->NotificationCode )
		{
		default:
			notificationMessage = L"Unknown:";
			break;

		case wlan_notification_acm_scan_complete: //スキャン完了
			notificationMessage = L"wlan_notification_acm_scan_complete";
			bScanWait = false;
			break;

		case wlan_notification_acm_scan_fail: //スキャン失敗
			notificationMessage = L"wlan_notification_acm_scan_fail";
			pConnNotifData = (PWLAN_CONNECTION_NOTIFICATION_DATA)wlanNotifData->pData;
			if ( pConnNotifData->wlanReasonCode != ERROR_SUCCESS )
			{
				bScanWait = false;
				WlanReasonCodeToString( pConnNotifData->wlanReasonCode, 1024, strReason, NULL );
				CWiFiRebootDlg* dlg = (CWiFiRebootDlg*)AfxGetApp();
				dlg->MsgReporter( (LPSTR)strError, (LPCSTR)L"ScanFailed Reason: %ls\n", strReason );
			}
			break;

		//case wlan_notification_acm_scan_list_refresh: //ネットワーク一覧が更新された
		//	notificationMessage = L"wlan_notification_acm_scan_list_refresh";
		//	break;
		}

		OutputDebugString( (LPCSTR)notificationMessage );
	}
}


CWiFiRebootDlg::CWiFiRebootDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWiFiRebootDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hBmp = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_MAIN), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
	m_hClientRgn = CreateEllipticRgn( 33, 34, 34, 35 );
	m_hWndRgn = DIBToRgn( m_hBmp, 0x00ff00, FALSE );
}

void CWiFiRebootDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_SSID, m_ctrlCbSSID);
	DDX_Control(pDX, IDC_WIFI_REBOOT, m_ctrlWifiReboot);
	DDX_Control(pDX, IDC_REBOOT_PROG, m_ctrlRebootProg);
	DDX_Control(pDX, IDC_PASSWORD, m_ctrlPsw);
	DDX_Control(pDX, IDC_STATIC_DESC, m_ctrlDesc);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_MINIMIZE, m_btnMinimize);
	DDX_Control(pDX, IDC_STATIC_SSID, m_ctrlStaticSsid);
	DDX_Control(pDX, IDC_STATIC_PWD, m_ctrlStaticPwd);
}

BEGIN_MESSAGE_MAP(CWiFiRebootDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_WIFI_REBOOT, &CWiFiRebootDlg::OnBnClickedWifiReboot)
	ON_CBN_SELCHANGE(IDC_CB_SSID, &CWiFiRebootDlg::OnCbnSelchangeCbSsid)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_MINIMIZE, OnMinimize)
END_MESSAGE_MAP()


// CWiFiRebootDlg メッセージ ハンドラ

BOOL CWiFiRebootDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ( m_hWndRgn )
		SetWindowRgn( m_hWndRgn, TRUE );

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	m_btnCancel.SetBitmaps( IDB_CLOSE, RGB(255, 0, 255) );
	m_btnCancel.DrawBorder( FALSE, TRUE );

	m_btnMinimize.SetBitmaps( IDB_MINIMIZE, RGB(255, 0, 255) );
	m_btnMinimize.DrawBorder( FALSE, TRUE );

	// TODO: 初期化をここに追加します。
	OnCbnSelchangeCbSsid();

	m_ctrlDesc.SetTextColor( LIGHTBLUE );
	m_ctrlDesc.SetBkColor( RGB(175, 199, 223) );
	m_ctrlStaticSsid.SetBkColor( RGB(175, 199, 223) );
	m_ctrlStaticPwd.SetBkColor( RGB(175, 199, 223) );

	m_ctrlRebootProg.SetBkColor( RGB(175, 199, 223) );

	unsigned int i, j, k;

	HANDLE hClient = NULL;
	DWORD dwMaxClient = 2;      //    
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	DWORD dwRetVal = 0;
	int iRet = 0;
	WCHAR strError[1024] = { NULL };

	WCHAR GuidString[64] = { 0 };

	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;

	PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;
	PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;

	int iRSSI = 0;

	DWORD dwPrevNotif = 0;
	bScanWait = true;

	dwResult = WlanOpenHandle( dwMaxClient, NULL, &dwCurVersion, &hClient );
	if (dwResult != ERROR_SUCCESS)
	{
		wsprintf( (LPSTR)strError, (LPCSTR)L"WlanOpenHandle failed with error: %u\n", dwResult );
		MessageBox( (LPCTSTR)strError, (LPCTSTR)L"WLAN error" );
		return TRUE;
		// You can use FormatMessage here to find out why the function failed
	}

	dwResult = WlanEnumInterfaces( hClient, NULL, &pIfList );
	if (dwResult != ERROR_SUCCESS)
	{
		wsprintf( (LPSTR)strError, (LPCSTR)L"WlanEnumInterfaces failed with error: %u\n", dwResult );
		MessageBox( (LPCTSTR)strError, (LPCTSTR)L"WLAN error" );
		return TRUE;
		// You can use FormatMessage here to find out why the function failed
	}
	else
	{
		MsgReporter( "Num Entries: %lu\n", pIfList->dwNumberOfItems );
		MsgReporter( "Current Index: %lu\n", pIfList->dwIndex );

		for ( i=0 ; i < (int)pIfList->dwNumberOfItems ; i++ )
		{
			pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
			MsgReporter( "  Interface Index[%u]:\t %lu\n", i, i );

			iRet = StringFromGUID2( pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString, sizeof(GuidString) / sizeof(*GuidString) );
			MsgReporter( "  InterfaceGUID[%d]: %ws\n", i, GuidString );

			m_ctrlDesc.SetWindowText( CW2A(pIfInfo->strInterfaceDescription) );

			MsgReporter( "  Interface Description[%d]: %ws", i, pIfInfo->strInterfaceDescription );
			MsgReporter( "\n" );
			MsgReporter( "  Interface State[%d]:\t ", i );

			switch ( pIfInfo->isState )
			{
			case wlan_interface_state_not_ready:
				MsgReporter( "Not ready\n" );
				break;
			case wlan_interface_state_connected:
				MsgReporter( "Connected\n" );
				break;
			case wlan_interface_state_ad_hoc_network_formed:
				MsgReporter( "First node in a ad hoc network\n" );
				break;
			case wlan_interface_state_disconnecting:
				MsgReporter( "Disconnecting\n" );
				break;
			case wlan_interface_state_disconnected:
				MsgReporter( "Not connected\n" );
				break;
			case wlan_interface_state_associating:
				MsgReporter( "Attempting to associate with a network\n" );
				break;
			case wlan_interface_state_discovering:
				MsgReporter( "Auto configuration is discovering settings for the network\n" );
				break;
			case wlan_interface_state_authenticating:
				MsgReporter( "In process of authenticating\n" );
				break;
			default:
				MsgReporter( "Unknown state %ld\n", pIfInfo->isState );
				break;
			}
			MsgReporter( "\n" );

			//コールバックの登録
			dwResult = WlanRegisterNotification( hClient,
												WLAN_NOTIFICATION_SOURCE_ACM,
												FALSE,
												(WLAN_NOTIFICATION_CALLBACK)WlanNotification,
												NULL,
												NULL,
												&dwPrevNotif );
			//利用可能なネットワークをスキャン
			dwResult = WlanScan( hClient, &pIfInfo->InterfaceGuid, NULL, NULL, NULL );

			while ( bScanWait )
			{
				Sleep( 100 );
			}

			dwResult = WlanGetAvailableNetworkList( hClient,
													&pIfInfo->InterfaceGuid,
													0,
													NULL,
													&pBssList );
			if ( dwResult != ERROR_SUCCESS )
			{
				dwRetVal = 1;
				MsgReporter( "WlanGetAvailableNetworkList failed with error" );
			}
			else
			{
				MsgReporter( "  Num Entries: %lu\n\n", pBssList->dwNumberOfItems );
				for ( j=0 ; j < pBssList->dwNumberOfItems ; j++ )
				{
					pBssEntry = (WLAN_AVAILABLE_NETWORK *)& pBssList->Network[j];
					MsgReporter( "  Profile Name[%u]:  %ws\n", j, pBssEntry->strProfileName );
					//m_ctrlCbSSID.AddString( CW2A(pBssEntry->strProfileName) );

					MsgReporter( "  SSID[%u]:\t\t ", j );
					if ( pBssEntry->dot11Ssid.uSSIDLength == 0 )
						OutputDebugString( "\n" );
					else
					{
						char ssid[1024];

						for ( k=0 ; k < pBssEntry->dot11Ssid.uSSIDLength ; k++ )
						{
							MsgReporter( "%c", (int)pBssEntry->dot11Ssid.ucSSID[k] );
							ssid[k] = pBssEntry->dot11Ssid.ucSSID[k];
						}
						ssid[k] = '\0';

						if ( strlen(ssid) != 0 )
							m_ctrlCbSSID.AddString( (LPCTSTR)&ssid[0] );

						OutputDebugString( "\n" );
					}

					MsgReporter( "  BSS Network type[%u]:\t ", j );
					switch ( pBssEntry->dot11BssType )
					{
					default:
						MsgReporter( "Other (%lu)\n", pBssEntry->dot11BssType );
						break;

					case dot11_BSS_type_infrastructure:
						MsgReporter( "Infrastructure (%u)\n", pBssEntry->dot11BssType );
						break;

					case dot11_BSS_type_independent:
						MsgReporter( "Infrastructure (%u)\n", pBssEntry->dot11BssType );
						break;
					}

					MsgReporter( "  Number of BSSIDs[%u]:\t %u\n", j, pBssEntry->uNumberOfBssids );

					MsgReporter( "  Connectable[%u]:\t ", j );
					if ( pBssEntry->bNetworkConnectable )
						MsgReporter( "Yes\n");
					else
					{
						MsgReporter( "No\n" );
						MsgReporter( "  Not connectable WLAN_REASON_CODE value[%u]:\t %u\n", j, pBssEntry->wlanNotConnectableReason );
					}

					MsgReporter( "  Number of PHY types supported[%u]:\t %u\n", j, pBssEntry->uNumberOfPhyTypes );

					if ( pBssEntry->wlanSignalQuality == 0 )
						iRSSI = -100;
					else if ( pBssEntry->wlanSignalQuality == 100 )
						iRSSI = -50;
					else
						iRSSI = -100 + (pBssEntry->wlanSignalQuality / 2);
					MsgReporter( "  Signal Quality[%u]:\t %u (RSSI: %i dBm)\n", j, pBssEntry->wlanSignalQuality, iRSSI );

					MsgReporter( "  Security Enabled[%u]:\t ", j );
					if ( pBssEntry->bSecurityEnabled )
						MsgReporter( "Yes\n" );
					else
						MsgReporter( "No\n" );

					MsgReporter( "\n" );
				}
			}
		}

		//通知の解除
		if ( hClient != NULL )
		{
			dwResult = WlanRegisterNotification( hClient, WLAN_NOTIFICATION_SOURCE_NONE, TRUE, NULL, NULL, NULL, &dwPrevNotif );
		}

		if ( pBssList != NULL )
		{
			WlanFreeMemory( pBssList );
			pBssList = NULL;
		}

		if ( pIfList != NULL )
		{
			WlanFreeMemory(pIfList);
			pIfList = NULL;
		}
	}

	m_ctrlCbSSID.SetCurSel( 0 );
	GetDlgItem(IDC_CB_SSID)->SetFocus();

	return FALSE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CWiFiRebootDlg::MsgReporter(const char *format, ... )
{
	char szMsg[1024*2] = { NULL };
	va_list ap;

	va_start( ap, format );
	vsprintf( szMsg, format, ap );
	va_end( ap );

	OutputDebugString( szMsg );
}

void CWiFiRebootDlg::ErrReporter(const char *format, ... )
{
	char szMsg[1024*2] = { NULL };
	char* szFormat = "[%d]:%s :%s";
    char* szFinal;
    DWORD dwError;
	LPVOID szBuffer;
	va_list ap;

    dwError = GetLastError();

	va_start( ap, format );
	vsprintf( szMsg, format, ap );
	va_end( ap );

    DWORD dwExtSize = 0;
    dwExtSize = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 80 ,
                          NULL, dwError,
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPTSTR)&szBuffer, 0, NULL );
    if ( dwExtSize == 0 )
        szBuffer = (char*)LocalAlloc(LPTR, 1);

	szFinal = (char*)LocalAlloc( LPTR, strlen(szMsg) + dwExtSize + 32 );
	wsprintf( szFinal, szFormat, dwError, szBuffer, szMsg );

	OutputDebugString( szFinal );

	LocalFree( szFinal );
	LocalFree( szBuffer );
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CWiFiRebootDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CWiFiRebootDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRGN CWiFiRebootDlg::DIBToRgn(HBITMAP hBmp, COLORREF BkColor, BOOL Direct)
{
	// use to return the handle of the HGRN
  	HRGN hRgn = NULL;					
	#define MAX_ALLOC_RECTS  100
	//the difference of the color
	COLORREF  Tolerance=0x00101010;
	if (hBmp)
	{
		//creat the dib to save the dc
		HDC hMemDC = CreateCompatibleDC(NULL);		
		if (hMemDC)
		{
			BITMAP bm;
			//get the info of the bitmap
			GetObject(hBmp, sizeof(bm), &bm);	

			BITMAPINFOHEADER BmpInfoh = {					//the struct of the bitmap
					sizeof(BITMAPINFOHEADER),			// biSize
					bm.bmWidth,					// biWidth;
					bm.bmHeight,					// biHeight;
					1,						// biPlanes;
					32,						// biBitCount
					BI_RGB,						// biCompression;
					0,						// biSizeImage;
					0,						// biXPelsPerMeter;
					0,						// biYPelsPerMeter;
					0,						// biClrUsed;
					0						// biClrImportant;
			};
			//design a void point to point to the bitmap
			LPVOID pBit32; 
			//creat a DIB
			HBITMAP hDib32 = CreateDIBSection(hMemDC, 
					(BITMAPINFO *)&BmpInfoh, 
					DIB_RGB_COLORS, &pBit32, NULL, 0);
			if (hDib32)
			{
				//copy dib to DC
				HBITMAP hOldib32 = (HBITMAP)SelectObject(hMemDC, hDib32);
				// create a DC to save orgin bitmap
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					BITMAP bm32;
					// get the new 34 bit Dib size
					GetObject(hDib32, sizeof(bm32), &bm32);
					//make sure the 32Dib's every line pilex's is 4 's times
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;
					//copy the orginal dib to DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					//copy dib to memory DC
					BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);
					DWORD MaxRects = MAX_ALLOC_RECTS;
					SYSTEM_INFO  Sysinfo;
					//get memory size
					GetSystemInfo(&Sysinfo);
					//make a stack which can chang big
					//alloct memory
					HANDLE hRcData=HeapCreate(HEAP_GENERATE_EXCEPTIONS,Sysinfo.dwPageSize, 0);
					RGNDATA * pRcData=(RGNDATA*)HeapAlloc(hRcData,HEAP_ZERO_MEMORY,
						sizeof(RGNDATAHEADER)+sizeof(RECT)*MaxRects);
					 //fill the the RGNDATA struck
					pRcData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pRcData->rdh.iType = RDH_RECTANGLES;
					pRcData->rdh.nCount = pRcData->rdh.nRgnSize = 0;
					SetRect(&pRcData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
		             		BYTE hr,hg,hb,lr,lg,lb;
					switch(BkColor)
					{
					case RGB(255,255,255):	//if the bkcolor is white
						hr = GetRValue(BkColor);
						hg = GetGValue(BkColor);
						hb = GetBValue(BkColor);
						lr = min(0xff, hr - GetRValue(Tolerance));
						lg = min(0xff, hg - GetGValue(Tolerance));
						lb = min(0xff, hb - GetBValue(Tolerance));
						break;
					case RGB(0,0,0):	//if the bkcolor is black
						lr = GetRValue(BkColor);
						lg = GetGValue(BkColor);
						lb = GetBValue(BkColor);
						hr = min(0xff, lr + GetRValue(Tolerance));
						hg = min(0xff, lg + GetGValue(Tolerance));
						hb = min(0xff, lb + GetBValue(Tolerance));
						break;
					default:		//if the bkcolor is other color
						Tolerance=0x111111;
						lr =max(0, GetRValue(BkColor)-GetRValue(Tolerance));
						lg = max(0,GetGValue(BkColor)-GetGValue(Tolerance));
						lb = max(0,GetBValue(BkColor)-GetBValue(Tolerance));
						hr=min(0xff,GetRValue(BkColor)+GetRValue(Tolerance));
						hg=min(0xff,GetGValue(BkColor)+GetGValue(Tolerance));
						hb=min(0xff,GetBValue(BkColor)+GetBValue(Tolerance));
						break;
					}
					// Get the bit point and do the search
					BYTE *pBits = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						for (int x = 0; x < bm.bmWidth; x++)
						{
							int x0 = x;
							DWORD *pColor = (DWORD *)pBits + x;
							BYTE dr,dg,db;
							while (x < bm.bmWidth)
							{
								dr=GetRValue(*pColor);
								dg=GetGValue(*pColor);
								db=GetBValue(*pColor);

								if ((dr>= lr && dr<= hr)&&(dg>=lg&&dg<=hg)&&(db>=lb&&db<=hb))
								{
									if(Direct)
										break;
									else
									{
										pColor++;
										x++;
									}
							  	}
								else if(Direct)
								{
									pColor++;
									x++;
								}
								else
									break;

							}
							if (x > x0)
							{
								if (pRcData->rdh.nCount >= MaxRects)
								{
									MaxRects += MAX_ALLOC_RECTS;
									//re alloc the stack
									pRcData=(RGNDATA*)HeapReAlloc(
									hRcData,HEAP_ZERO_MEMORY,pRcData, 
									sizeof(RGNDATAHEADER)+sizeof(RECT)*MaxRects);
								}
								RECT *pr = (RECT *)&pRcData->Buffer;
								SetRect(&pr[pRcData->rdh.nCount], x0, y, x, y+1);
								pRcData->rdh.rcBound.left = x0;
								pRcData->rdh.rcBound.top = y;
								pRcData->rdh.rcBound.right = x;
								pRcData->rdh.rcBound.bottom = y+1;
								pRcData->rdh.nCount++;

								if (pRcData->rdh.nCount == 3000)
								{	
									HRGN tmphRgn = ExtCreateRegion(NULL,
									sizeof(RGNDATAHEADER) + (sizeof(RECT) * MaxRects),
									pRcData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, tmphRgn, RGN_OR);
										DeleteObject(tmphRgn);
									}
									else
										hRgn = tmphRgn;
									pRcData->rdh.nCount = 0;
									SetRect(&pRcData->rdh.rcBound, 
									MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// search next line
						pBits -= bm32.bmWidthBytes;
					}
					HRGN tmphRgn = ExtCreateRegion(NULL, 
							sizeof(RGNDATAHEADER) + (sizeof(RECT) * MaxRects), pRcData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, tmphRgn, RGN_OR);
						DeleteObject(tmphRgn);
					}
					else
						hRgn = tmphRgn;
					// make a rect ,use this rect xor to the  BkColor
					//then we can get the rect we want
					if(!Direct)
					{
						HRGN hRect=CreateRectRgn(0,0,bm.bmWidth,bm.bmHeight);
				                        if(hRect)
						{
							CombineRgn(hRgn,hRgn,hRect,RGN_XOR);
							DeleteObject(hRect);
						}
					    else
							return NULL;
					}
					//release the memory
					HeapFree(hRcData,HEAP_NO_SERIALIZE,pRcData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
					DeleteObject(holdBmp);
				}
				SelectObject(hMemDC,hOldib32);
				DeleteDC(hMemDC);
				DeleteObject(hOldib32);
				DeleteObject(hDib32);
			}
			else
				DeleteDC(hMemDC);
		}
	}
	return hRgn;
}

void CWiFiRebootDlg::OnBnClickedWifiReboot()
{
	CWiFiRebootApp* pApp = (CWiFiRebootApp*)AfxGetApp();

	UpdateData();

	m_ctrlRebootProg.SetRange( 0, 6 );

	GetDlgItem(IDC_WIFI_REBOOT)->EnableWindow( FALSE );
	GetDlgItem(IDC_PASSWORD)->EnableWindow( FALSE );

	CString str, str2, str3;
	std::string cmd1( "cmd /c netsh wlan disconnect" );
	std::string cmd2( "cmd /c powershell Restart-NetAdapter -Name Wi-Fi" );
	std::string cmd3;
	std::string cmd4( "cmd /c netsh wlan connect name=" );
	std::string cmd5( "cmd /c netsh interface set interface Wi-Fi disable" );
	std::string cmd6( "cmd /c netsh interface set interface Wi-Fi enabled" );

	m_ctrlCbSSID.GetWindowText( str );
	std::string ssid( str.GetBuffer() );

	m_ctrlPsw.GetWindowText( str2 );
	std::string psw( str2.GetBuffer() );

	m_crypto.m_encryptKey = "Sample_Key";	// 暗号化キー文字列
	m_crypto.m_hKey = keyHCU;
	m_crypto.m_subKey = "SOFTWARE\\WiFi-Reboot\\Crypto SSID List";
	m_crypto.m_valueKey = ssid;
	m_hCrypto = cryptoOpenRegistry( &m_crypto );
	cryptoWriteRegistry( m_hCrypto, psw.c_str() );
	cryptoCloseRegistry( m_hCrypto );

	str3.Format( "cmd /c netsh wlan set profileparameter name=%s keymaterial=%s connectionmode=auto", ssid.c_str(), psw.c_str() );

	cmd3 = str3;
	cmd4 += ssid;

	RunCmdProc( cmd1 ); Sleep( 1000 ); m_ctrlRebootProg.SetPos( 1 );
	RunCmdProc( cmd2 );	Sleep( 1000 ); m_ctrlRebootProg.SetPos( 2 );
	RunCmdProc( cmd3 );	Sleep( 1000 ); m_ctrlRebootProg.SetPos( 3 );
	RunCmdProc( cmd4 );	Sleep( 1000 ); m_ctrlRebootProg.SetPos( 4 );
	RunCmdProc( cmd5 );	Sleep( 1000 ); m_ctrlRebootProg.SetPos( 5 );
	RunCmdProc( cmd6 );	Sleep( 1000 ); m_ctrlRebootProg.SetPos( 6 );

	GetDlgItem(IDC_PASSWORD)->EnableWindow( TRUE );
	GetDlgItem(IDC_WIFI_REBOOT)->EnableWindow( TRUE );

	MessageBox( "Wi-Fi の再起動を試みました", "Wi-Fi再起動", MB_OK|MB_ICONASTERISK );
	m_ctrlRebootProg.SetPos( 0 );
}

DWORD CWiFiRebootDlg::RunCmdProc(string cmd)
{
    STARTUPINFO  si;
    PROCESS_INFORMATION pi;
    DWORD ret;
    HANDLE hWndmain;
 
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
 
	ret = CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
                      CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,
                      NULL, NULL, &si, &pi);

	hWndmain = pi.hProcess;
    CloseHandle(pi.hThread);
    WaitForSingleObject(hWndmain, INFINITE);
    CloseHandle(hWndmain);

	return ret;
}

BOOL CWiFiRebootDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
    if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_RETURN == pMsg->wParam )
		{
            return FALSE;
        }
        else if ( VK_ESCAPE == pMsg->wParam )
		{
			return TRUE;
        }
        else
		{
            return CDialog::PreTranslateMessage( pMsg );
        }
    }
    else
	{
        return CDialog::PreTranslateMessage( pMsg );
    }
}

void CWiFiRebootDlg::OnCbnSelchangeCbSsid()
{
	CString str;
	string pass;

	pass.resize( 128, ' ' );

	m_ctrlCbSSID.GetWindowText( str );
	std::string ssid( str.GetBuffer() );

	m_crypto.m_encryptKey = "Sample_Key";	// 暗号化キー文字列
	m_crypto.m_hKey = keyHCU;
	m_crypto.m_subKey = "SOFTWARE\\WiFi-Reboot\\Crypto SSID List";
	m_crypto.m_valueKey = ssid;
	m_hCrypto = cryptoOpenRegistry( &m_crypto );
	if ( cryptoReadRegistry(m_hCrypto, pass) == false )
		m_ctrlPsw.SetWindowText( "" );
	else
		m_ctrlPsw.SetWindowText( pass.c_str() );

	cryptoCloseRegistry( m_hCrypto );
}


BOOL CWiFiRebootDlg::OnEraseBkgnd(CDC* pDC)
{
	if(m_hBmp)
	{
		BITMAP bm;
		GetObject(m_hBmp,sizeof(bm),&bm);
		HDC hMemdc=CreateCompatibleDC(pDC->m_hDC); 
		if(hMemdc)
		{
		   HBITMAP hOldBmp=(HBITMAP)SelectObject(hMemdc,m_hBmp);
		   if(hOldBmp)
		   {
			   BitBlt(pDC->m_hDC,0,0,bm.bmWidth,bm.bmHeight,hMemdc,0,0,SRCCOPY);
			   SelectObject(hMemdc,hOldBmp);
			   DeleteDC(hMemdc);
			   DeleteObject(hOldBmp);
			   return TRUE;
		   }
		   else
			 DeleteDC(hMemdc);
		}
	}
	return CDialog::OnEraseBkgnd(pDC);
}

void CWiFiRebootDlg::OnMinimize() 
{
	ShowWindow( SW_MINIMIZE );	
}
