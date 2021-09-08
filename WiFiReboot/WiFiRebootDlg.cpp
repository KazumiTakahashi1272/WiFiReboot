
// WiFiRebootDlg.cpp : �����t�@�C��
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


// CWiFiRebootDlg �_�C�A���O

static bool bScanWait = true;


void CWiFiRebootDlg::WlanNotification(WLAN_NOTIFICATION_DATA *wlanNotifData, VOID *p)
{
	//���P�[���w��
	setlocale(LC_ALL, "japanese");
	WCHAR strReason[1024] = { NULL };
	WCHAR strError[1024] = { NULL };

	//�ʒm����auto configuration module(ACM)�ɐݒ�
	if ( wlanNotifData->NotificationSource == WLAN_NOTIFICATION_SOURCE_ACM )
	{
		PWLAN_CONNECTION_NOTIFICATION_DATA pConnNotifData = NULL;
		WCHAR *notificationMessage;

		switch ( wlanNotifData->NotificationCode )
		{
		default:
			notificationMessage = L"Unknown:";
			break;

		case wlan_notification_acm_scan_complete: //�X�L��������
			notificationMessage = L"wlan_notification_acm_scan_complete";
			bScanWait = false;
			break;

		case wlan_notification_acm_scan_fail: //�X�L�������s
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

		//case wlan_notification_acm_scan_list_refresh: //�l�b�g���[�N�ꗗ���X�V���ꂽ
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
}

void CWiFiRebootDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT_SSID, m_ctrlEditSsid);
	DDX_Control(pDX, IDC_CB_SSID, m_ctrlCbSSID);
}

BEGIN_MESSAGE_MAP(CWiFiRebootDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_WIFI_REBOOT, &CWiFiRebootDlg::OnBnClickedWifiReboot)
END_MESSAGE_MAP()


// CWiFiRebootDlg ���b�Z�[�W �n���h��

BOOL CWiFiRebootDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
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

			//�R�[���o�b�N�̓o�^
			dwResult = WlanRegisterNotification( hClient,
												WLAN_NOTIFICATION_SOURCE_ACM,
												FALSE,
												(WLAN_NOTIFICATION_CALLBACK)WlanNotification,
												NULL,
												NULL,
												&dwPrevNotif );
			//���p�\�ȃl�b�g���[�N���X�L����
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

		//�ʒm�̉���
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

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
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

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CWiFiRebootDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CWiFiRebootDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWiFiRebootDlg::OnBnClickedWifiReboot()
{
	UpdateData();

	GetDlgItem(IDCANCEL)->EnableWindow( FALSE );
	GetDlgItem(IDC_WIFI_REBOOT)->EnableWindow( FALSE );

	CString str;
	std::string cmd1( "cmd /c netsh wlan disconnect" );
	std::string cmd2( "cmd /c powershell Restart-NetAdapter -Name Wi-Fi" );
	std::string cmd3( "cmd /c netsh wlan connect name=" );

	m_ctrlCbSSID.GetWindowText( str );
	std::string ssid( str.GetBuffer() );

	cmd3 += ssid;

	RunCmdProc( cmd1 );
	RunCmdProc( cmd2 );
	RunCmdProc( cmd3 );

	GetDlgItem(IDC_WIFI_REBOOT)->EnableWindow( TRUE );
	GetDlgItem(IDCANCEL)->EnableWindow( TRUE );

	MessageBox( "Wi-Fi �̍ċN�������݂܂���", "Wi-Fi�ċN��", MB_OK|MB_ICONASTERISK );
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
