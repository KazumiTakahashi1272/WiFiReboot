
// WiFiRebootDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "WiFiReboot.h"
#include "WiFiRebootDlg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <wchar.h>

#include <iostream>
#include <vector>
using namespace std;
using namespace CPlusPlusLogging;

#include <wlanapi.h>
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWiFiRebootDlg ダイアログ

static bool bScanWait = true;
static std::stringstream ss;
static string s;

// Log file name. File name should be change from here only
//const string logFileName = X;
//string g_strModulePath;

#define wlan_notification_acm_scan_list_refresh		0x0000001a
#define WLAN_PROFILE_GET_PLAINTEXT_KEY				0x00000004
#define REPORT	s = ss.str(); MsgReporter( s.c_str() );  ss.str( "" ); ss.clear(std::stringstream::goodbit);

void CWiFiRebootDlg::WlanNotification(WLAN_NOTIFICATION_DATA *wlanNotifData, VOID *p)
{
	//ロケール指定
	setlocale(LC_ALL, "japanese");
	TCHAR strReason[1024] = { NULL };
	TCHAR strError[1024] = { NULL };

	CWiFiRebootDlg* dlg = (CWiFiRebootDlg*)AfxGetApp();

	//通知元をauto configuration module(ACM)に設定
	if ( wlanNotifData->NotificationSource == WLAN_NOTIFICATION_SOURCE_ACM )
	{
		PWLAN_CONNECTION_NOTIFICATION_DATA pConnNotifData = NULL;
		TCHAR *notificationMessage;

		switch ( wlanNotifData->NotificationCode )
		{
		default:
			notificationMessage = "Unknown:";
			break;

		case wlan_notification_acm_scan_complete: //スキャン完了
			notificationMessage = "wlan_notification_acm_scan_complete";
			bScanWait = false;
			break;

		case wlan_notification_acm_scan_fail: //スキャン失敗
			notificationMessage = "wlan_notification_acm_scan_fail";
			pConnNotifData = (PWLAN_CONNECTION_NOTIFICATION_DATA)wlanNotifData->pData;
			if ( pConnNotifData->wlanReasonCode != ERROR_SUCCESS )
			{
				bScanWait = false;
				WlanReasonCodeToString( pConnNotifData->wlanReasonCode, 1024, CA2W(strReason), NULL );
				//dlg->MsgReporter( (LPSTR)strError, (LPCSTR)"ScanFailed Reason: %ls\n", strReason );
				::ss << "ScanFailed Reason: " << strReason << endl;
				::s = ::ss.str();
				dlg->MsgReporter( ::s.c_str() );
				::ss.str( "" ); ::ss.clear(std::stringstream::goodbit);
			}
			break;

		case wlan_notification_acm_scan_list_refresh: //ネットワーク一覧が更新された
			notificationMessage = "wlan_notification_acm_scan_list_refresh";
			break;
		}

		//dlg->MsgReporter( "%s", (LPCSTR)notificationMessage );
		::ss << notificationMessage << endl;
		::s = ::ss.str();
		dlg->MsgReporter( ::s.c_str() );
		::ss.str( "" ); ::ss.clear(std::stringstream::goodbit);
	}
}


CWiFiRebootDlg::CWiFiRebootDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWiFiRebootDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pListCtrl = new CComboListCtrlExt;
	m_ctrlCbSSID.SetListCtrl(m_pListCtrl);

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
	DDX_Control(pDX, IDC_HELPBTN, m_btnHelp);
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
	ON_BN_CLICKED(IDCANCEL, &CWiFiRebootDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_HELPBTN, &CWiFiRebootDlg::OnBnClickedHelpbtn)
END_MESSAGE_MAP()


// CWiFiRebootDlg メッセージ ハンドラ
BOOL CWiFiRebootDlg::OnInitDialog()
{
	CString str;

	CDialog::OnInitDialog();

	if ( m_hWndRgn )
		SetWindowRgn( m_hWndRgn, TRUE );

	CFont font;
	CClientDC dc(this);
	font.CreatePointFont( 120, _T("MS UI Gothic"), &dc );
	m_ctrlDesc.SetFont( &font );

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	m_pListCtrl->InitSkin();
	m_pListCtrl->SetBkColor( BKCOLOR );
	m_pListCtrl->SetTextColor( WHITE/*RGB(222,222,222)*/ );

	int nItemIndex = 0;
	m_pListCtrl->ModifyStyle(0, LVS_REPORT);
	m_pListCtrl->SetExtendedStyle(/*LVS_EX_CHECKBOXES |*/ LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_pListCtrl->InsertColumn( 0, "SSID", LVCFMT_LEFT, 200 );
	m_pListCtrl->InsertColumn( 1, "BSS Network type", LVCFMT_LEFT, 110 );
	m_pListCtrl->InsertColumn( 2, "Signal", LVCFMT_LEFT, 50 );
	m_pListCtrl->InsertColumn( 3, "Security", LVCFMT_LEFT, 70 );

	m_pListCtrl->m_SkinHeaderCtrl.SubclassWindow( m_pListCtrl->GetHeaderCtrl()->m_hWnd );

	m_btnHelp.SetBitmaps( IDB_HELPBTN, RGB(255, 0, 255) );
	m_btnHelp.DrawBorder( FALSE, TRUE );

	m_btnCancel.SetBitmaps( IDB_CLOSE, RGB(255, 0, 255) );
	m_btnCancel.DrawBorder( FALSE, TRUE );

	m_btnMinimize.SetBitmaps( IDB_MINIMIZE, RGB(255, 0, 255) );
	m_btnMinimize.DrawBorder( FALSE, TRUE );

	// TODO: 初期化をここに追加します。

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
	TCHAR strError[1024] = { NULL };

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
		ErrReporter( "WlanOpenHandle failed with error\n" );

		wsprintf( (LPSTR)strError, (LPCSTR)"WlanOpenHandle failed with error: %u\n", dwResult );
		MessageBox( (LPCTSTR)strError, (LPCTSTR)"WLAN error" );
		LOG_ERROR( strError );
		return TRUE;
		// You can use FormatMessage here to find out why the function failed
	}

	dwResult = WlanEnumInterfaces( hClient, NULL, &pIfList );
	if (dwResult != ERROR_SUCCESS)
	{
		ErrReporter( "WlanEnumInterfaces failed with error\n" );

		wsprintf( (LPSTR)strError, (LPCSTR)"WlanEnumInterfaces failed with error: %u\n", dwResult );
		MessageBox( (LPCTSTR)strError, (LPCTSTR)"WLAN error" );
		LOG_ERROR( strError );
		return TRUE;
		// You can use FormatMessage here to find out why the function failed
	}
	else
	{
		ss << "Num Entries: " << pIfList->dwNumberOfItems << endl; REPORT;
		ss << "Current Index: " << pIfList->dwIndex << endl; REPORT;

		for ( i=0 ; i < (int)pIfList->dwNumberOfItems ; i++ )
		{
			wlan_Interface* pInterface = new wlan_Interface;
			m_vInterface.push_back( pInterface );

			WLAN_PROFILE_INFO_LIST* pProfileList;
			PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;

			pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
			ss << "  Interface Index[" << i << "]:\t " << i << endl; REPORT;

			if ( ERROR_SUCCESS == WlanGetProfileList( hClient, &pIfInfo->InterfaceGuid, NULL, &pProfileList) )
			{
				for (DWORD j = 0 ; j < pProfileList->dwNumberOfItems ; j++ )
				{
					LPWSTR lpszXmlProfile;
					DWORD dwFlags = WLAN_PROFILE_GET_PLAINTEXT_KEY | WLAN_PROFILE_USER;
					DWORD dwAccess = WLAN_READ_ACCESS;

					WLAN_PROFILE_INFO* pProfileInfo = &pProfileList->ProfileInfo[j];
					WlanGetProfile(hClient, &pIfInfo->InterfaceGuid, pProfileInfo->strProfileName, NULL, &lpszXmlProfile, &dwFlags, &dwAccess);

					CString strXml = CW2A(lpszXmlProfile);
					WlanFreeMemory(lpszXmlProfile);

					int nFirstIndex = strXml.Find("<SSID>");
					int nLastIndex = strXml.Find("</SSID>");

					CString strSSID = CString(((LPCTSTR)strXml) + nFirstIndex, nLastIndex - nFirstIndex);
					nFirstIndex = strSSID.Find("<name>");
					nLastIndex = strSSID.Find("</name>");
					strSSID = CString(((LPCTSTR)strSSID) + nFirstIndex + 6, nLastIndex - (nFirstIndex + 6));

					CString strKey;
					nFirstIndex = strXml.Find(_T("<keyMaterial>"));
					if (nFirstIndex != -1)
					{
						nLastIndex = strXml.Find(_T("</keyMaterial>"));
						strKey = CString(((LPCTSTR)strXml) + nFirstIndex + 13, nLastIndex - (nFirstIndex + 13));

						ss << "  SSID[" << strSSID << "] = " << strKey << endl; REPORT;
					}
				}
			}

			iRet = StringFromGUID2( pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString, sizeof(GuidString) / sizeof(*GuidString) );
			ss << "  InterfaceGUID[" << i << "]: " << GuidString << endl; REPORT;

			m_ctrlDesc.SetWindowText( CW2A(pIfInfo->strInterfaceDescription) );

			ss << "  Interface Description[" << i << "]: " <<  pIfInfo->strInterfaceDescription << endl; REPORT;
			ss << "  Interface State[" << i << "]\t ";

			switch ( pIfInfo->isState )
			{
			default:
				ss << "Unknown state " << pIfInfo->isState << endl; REPORT;
				break;
			case wlan_interface_state_not_ready:
				ss << "Not ready" << endl; REPORT;
				break;
			case wlan_interface_state_connected:
				ss << "Connected" << endl; REPORT;
				break;
			case wlan_interface_state_ad_hoc_network_formed:
				ss << "First node in a ad hoc network" << endl; REPORT;
				break;
			case wlan_interface_state_disconnecting:
				ss << "Disconnecting" << endl; REPORT;
				break;
			case wlan_interface_state_disconnected:
				ss << "Not connected" << endl; REPORT;
				break;
			case wlan_interface_state_associating:
				ss << "Attempting to associate with a network" << endl; REPORT;
				break;
			case wlan_interface_state_discovering:
				ss << "Auto configuration is discovering settings for the network\n" << endl; REPORT;
				break;
			case wlan_interface_state_authenticating:
				ss << "In process of authenticating" << endl; REPORT;
				break;
			}

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
				ErrReporter( "WlanGetAvailableNetworkList failed with error" );
				LOG_ERROR( "WlanGetAvailableNetworkList failed with error" );
			}
			else
			{
				ss << "  Num Entries: " << pBssList->dwNumberOfItems << endl; REPORT;
				ss << "---------------------------------" << endl; REPORT;
				for ( j=0 ; j < pBssList->dwNumberOfItems ; j++ )
				{
					wlan_Profile* pProfile = new wlan_Profile;

					pBssEntry = (WLAN_AVAILABLE_NETWORK *)& pBssList->Network[j];
					ss << "  Profile Name[" << j << "]:  " << pBssEntry->strProfileName << endl; REPORT;

					ss << "  SSID[" << j << "]:\t\t ";
					if ( pBssEntry->dot11Ssid.uSSIDLength == 0 )
					{
						ss << endl; REPORT;
					}
					else
					{
						char ssid[1024];

						for ( k=0 ; k < pBssEntry->dot11Ssid.uSSIDLength ; k++ )
						{
							ssid[k] = pBssEntry->dot11Ssid.ucSSID[k];
						}
						ssid[k] = '\0';

						if ( strlen(ssid) == 0 )
							m_pListCtrl->InsertItem( nItemIndex, "(null)" );
						else
							m_pListCtrl->InsertItem( nItemIndex, ssid );

						ss << ssid << endl; REPORT;
						pProfile->Ssid = ssid;
					}

					ss << "  BSS Network type[" << j << "]:\t ";
					switch ( pBssEntry->dot11BssType )
					{
					default:
						ss << "Other (" << pBssEntry->dot11BssType << ")" << endl; REPORT;
						str.Format( "Other" );
						break;

					case dot11_BSS_type_infrastructure:
						ss << "Infrastructure (" << pBssEntry->dot11BssType << ")" << endl; REPORT;
						str.Format( "Infrastructure" );
						break;

					case dot11_BSS_type_independent:
						ss << "Independent (" << pBssEntry->dot11BssType << ")" << endl; REPORT;
						str.Format( "Independent" );
						break;
					}
					m_pListCtrl->SetItemText( nItemIndex, 1, str );

					ss << "  Number of BSSIDs[" << j << "]:\t " << pBssEntry->uNumberOfBssids << endl; REPORT;

					ss << "  Connectable[" << j << "]:\t ";
					if ( pBssEntry->bNetworkConnectable )
					{
						ss << "Yes" << endl; REPORT;
					}
					else
					{
						ss << "No" << endl;
						ss << "  Not connectable WLAN_REASON_CODE value[" << j << "]:\t " << pBssEntry->wlanNotConnectableReason << endl; REPORT;
					}

					ss << "  Number of PHY types supported[" << j << "]:\t " << pBssEntry->uNumberOfPhyTypes << endl; REPORT;

					if ( pBssEntry->wlanSignalQuality == 0 )
						iRSSI = -100;
					else if ( pBssEntry->wlanSignalQuality == 100 )
						iRSSI = -50;
					else
						iRSSI = -100 + (pBssEntry->wlanSignalQuality / 2);
					ss << "  Signal Quality[" << j << "]:\t " << std::dec << pBssEntry->wlanSignalQuality << "(RSSI: " << std::dec << iRSSI << " dBm)" << endl; REPORT;
					str.Format( "%d", pBssEntry->wlanSignalQuality );
					m_pListCtrl->SetItemText( nItemIndex, 2, str );

					ss << "  Security Enabled[" << j << "]:\t ";
					if ( pBssEntry->bSecurityEnabled )
					{
						pProfile->SecurityEnabled = true;
						ss << "Yes" << endl; REPORT;
						m_pListCtrl->SetItemText( nItemIndex, 3, "Yes" );
					}
					else
					{
						pProfile->SecurityEnabled = false;
						ss << "No" << endl; REPORT;
						m_pListCtrl->SetItemText( nItemIndex, 3, "No" );
					}

					ss << "  Default AuthAlgorithm[" << j << "]: ";
					switch ( pBssEntry->dot11DefaultAuthAlgorithm )
					{
					default:
						ss << "Other (" << pBssEntry->dot11DefaultAuthAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_AUTH_ALGO_80211_OPEN:
						ss << "802.11 Open (" << pBssEntry->dot11DefaultAuthAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_AUTH_ALGO_80211_SHARED_KEY:
						ss << "802.11 Shared (" << pBssEntry->dot11DefaultAuthAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_AUTH_ALGO_WPA:
						ss << "WPA (" << pBssEntry->dot11DefaultAuthAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_AUTH_ALGO_WPA_PSK:
						ss << "WPA-PSK (" << pBssEntry->dot11DefaultAuthAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_AUTH_ALGO_WPA_NONE:
						ss << "WPA-None (" << pBssEntry->dot11DefaultAuthAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_AUTH_ALGO_RSNA:
						ss << "RSNA (" << pBssEntry->dot11DefaultAuthAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_AUTH_ALGO_RSNA_PSK:
						ss << "RSNA with PSK(" << pBssEntry->dot11DefaultAuthAlgorithm << ")" << endl; REPORT;
						break;
					}

					ss << "  Default CipherAlgorithm[" << j << "]: ";
					switch ( pBssEntry->dot11DefaultCipherAlgorithm )
					{
					default:
						ss << "Other (0x" << std::hex << pBssEntry->dot11DefaultCipherAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_CIPHER_ALGO_NONE:
						ss << "None (0x" << std::hex << pBssEntry->dot11DefaultCipherAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_CIPHER_ALGO_WEP40:
						ss << "WEP-40 (0x" << std::hex << pBssEntry->dot11DefaultCipherAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_CIPHER_ALGO_TKIP:
						ss << "TKIP (0x" << std::hex << pBssEntry->dot11DefaultCipherAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_CIPHER_ALGO_CCMP:
						ss << "CCMP (0x" << std::hex << pBssEntry->dot11DefaultCipherAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_CIPHER_ALGO_WEP104:
						ss << "WEP-104 (0x" << std::hex << pBssEntry->dot11DefaultCipherAlgorithm << ")" << endl; REPORT;
						break;
					case DOT11_CIPHER_ALGO_WEP:
						ss << "WEP (0x" << std::hex << pBssEntry->dot11DefaultCipherAlgorithm << ")" << endl; REPORT;
						break;
					}

					ss << "  Flags[" << j << "]:\t 0x" << std::hex << pBssEntry->dwFlags;
					if (pBssEntry->dwFlags)
					{
						if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
							ss << " - Currently connected";
						if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
							ss << " - Has profile";
					}

					ss << endl; REPORT;
					ss << "---------------------------------" << endl; REPORT;

					pInterface->vProfile.push_back( pProfile );

					nItemIndex++;
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

	m_pListCtrl->SendMessage( CB_SETCURSEL, 0, 0 );
	GetDlgItem(IDC_CB_SSID)->SetFocus();

	OnCbnSelchangeCbSsid();

	return FALSE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CWiFiRebootDlg::OnBnClickedCancel()
{
	vector<wlan_Interface*>::iterator itrInt = m_vInterface.begin();
	while ( itrInt != m_vInterface.end() )
	{
		wlan_Interface* pInt = *itrInt;

		vector<wlan_Profile*>::iterator itrPro = pInt->vProfile.begin();
		while ( itrPro != pInt->vProfile.end() )
		{
			wlan_Profile* pProfile = *itrPro;

			delete (*itrPro);
			itrPro = pInt->vProfile.erase( itrPro );
		}

		delete (*itrInt);
		itrInt = m_vInterface.erase( itrInt );
	}

	OnCancel();
}

void CWiFiRebootDlg::MsgReporter(const char *format, ... )
{
	char szMsg[1024*2] = { NULL };
	va_list ap;

	va_start( ap, format );
	vsprintf( szMsg, format, ap );
	va_end( ap );

	LOG_INFO( szMsg );
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

	LOG_ERROR( szFinal );
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

	m_ctrlRebootProg.SetRange( 0, 7 );

	GetDlgItem(IDC_WIFI_REBOOT)->EnableWindow( FALSE );
	GetDlgItem(IDC_PASSWORD)->EnableWindow( FALSE );

	CString str, str2, str3;
	std::string cmd1( "cmd /c netsh wlan disconnect" );
	std::string cmd2( "cmd /c netsh interface set interface name=Wi-Fi admin=disabled" );
	std::string cmd3( "cmd /c powershell Restart-NetAdapter -Name Wi-Fi" );
	std::string cmd4;
	std::string cmd5( "cmd /c netsh wlan connect name=" );
	std::string cmd6( "cmd /c netsh interface set interface Wi-Fi disable" );
	std::string cmd7( "cmd /c netsh interface set interface Wi-Fi enabled" );

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

	cmd4 = str3;
	cmd5 += ssid;

	RunCmdProc( cmd1 ); Sleep( 1000 ); m_ctrlRebootProg.SetPos( 1 );
	RunCmdProc( cmd2 );	Sleep( 1000 ); m_ctrlRebootProg.SetPos( 2 );
	RunCmdProc( cmd3 );	Sleep( 1000 ); m_ctrlRebootProg.SetPos( 3 );
	RunCmdProc( cmd4 );	Sleep( 1000 ); m_ctrlRebootProg.SetPos( 4 );
	RunCmdProc( cmd5 );	Sleep( 1000 ); m_ctrlRebootProg.SetPos( 5 );
	RunCmdProc( cmd6 );	Sleep( 1000 ); m_ctrlRebootProg.SetPos( 6 );
	RunCmdProc( cmd7 );	Sleep( 5000 ); m_ctrlRebootProg.SetPos( 7 );

	GetDlgItem(IDC_PASSWORD)->EnableWindow( TRUE );
	GetDlgItem(IDC_WIFI_REBOOT)->EnableWindow( TRUE );

	MessageBox( "Wi-Fi の再起動を試みました", "Wi-Fi再起動", MB_OK|MB_ICONASTERISK );
	m_ctrlRebootProg.SetPos( 0 );
}

DWORD CWiFiRebootDlg::RunCmdProc(string cmd)
{
    STARTUPINFO  si;
    PROCESS_INFORMATION pi;
    DWORD ret, dwExitCode;
    HANDLE hWndmain;
 
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
 
	ret = CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
                      CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,
                      NULL, NULL, &si, &pi);

	hWndmain = pi.hProcess;
	CloseHandle( pi.hThread );

	GetExitCodeProcess( hWndmain, &dwExitCode );
	MsgReporter( "Return=%d, dwExitCode=%d\n", ret, dwExitCode );

    WaitForSingleObject( hWndmain, INFINITE );
    CloseHandle( hWndmain );

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

#pragma comment(lib, "version.lib")
void CWiFiRebootDlg::OnBnClickedHelpbtn()
{
    const int MAX_LEN = 2048;
    CString csMsg;
    CString csBuf;
    UINT uDmy;
    struct LANGANDCODEPAGE
	{
        WORD wLanguage; //日本語(0411)、英語(0409)
        WORD wCodePage;
    } *lpTran;
 
    LPTSTR    pBuf = csBuf.GetBuffer(MAX_LEN + 1);
    ::GetModuleFileName(NULL, pBuf, MAX_LEN + 1);
    DWORD dwZero = 0;
    DWORD dwVerInfoSize = GetFileVersionInfoSize(pBuf, &dwZero);
 
    unsigned char *pBlock = NULL;
    pBlock = new unsigned char[dwVerInfoSize];
    ::GetFileVersionInfo(pBuf, 0, dwVerInfoSize, pBlock);
    ::VerQueryValue(pBlock, "\\VarFileInfo\\Translation", (LPVOID*)&lpTran, &uDmy);
 
    //バージョンを取得するためのバッファ
    char name[256];
    void *pVer;
 
    //ファイルの説明(FileDescription)
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\FileDescription", lpTran[0].wLanguage, lpTran[0].wCodePage);
    ::VerQueryValue(pBlock, name, &pVer, &uDmy);
    csBuf.Format("ファイルの説明\t[%s]", pVer);
    csMsg = csBuf;

    //ファイルバージョン(FileVersion)
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\FileVersion", lpTran[0].wLanguage, lpTran[0].wCodePage);
    ::VerQueryValue(pBlock, name, &pVer, &uDmy);
    csBuf.Format("ファイルバージョン\t[%s]", pVer);
    csMsg += ("\r\n" + csBuf);

    //製品名(ProductName)
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\ProductName", lpTran[0].wLanguage, lpTran[0].wCodePage);
    ::VerQueryValue(pBlock, name, &pVer, &uDmy);
    csBuf.Format("製品名\t\t[%s]", pVer);
    csMsg += ("\r\n" + csBuf);

    //製品バージョン(ProductVersion)
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\ProductVersion", lpTran[0].wLanguage, lpTran[0].wCodePage);
    ::VerQueryValue(pBlock, name, &pVer, &uDmy);
    csBuf.Format("プロダクトバージョン\t[%s]", pVer);
    csMsg += ("\r\n" + csBuf);
 
    //著作権(LegalCopyright)
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\LegalCopyright", lpTran[0].wLanguage, lpTran[0].wCodePage);
    ::VerQueryValue(pBlock, name, &pVer, &uDmy);
    csBuf.Format("著作権\t\t[%s]", pVer);
    csMsg += ("\r\n" + csBuf);

    //会社名(CompanyName)
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\CompanyName", lpTran[0].wLanguage, lpTran[0].wCodePage);
    ::VerQueryValue(pBlock, name, &pVer, &uDmy);
    csBuf.Format("会社名\t\t[%s]", pVer);
    csMsg += ("\r\n" + csBuf);

    //メッセージボックスで結果表示
    AfxMessageBox(csMsg, MB_ICONINFORMATION);
    delete[] pBlock;
}
