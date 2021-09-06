
// WiFiRebootDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "WiFiReboot.h"
#include "WiFiRebootDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWiFiRebootDlg �_�C�A���O




CWiFiRebootDlg::CWiFiRebootDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWiFiRebootDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWiFiRebootDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SSID, m_ctrlEditSsid);
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

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
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

	CString str;
	std::string cmd1( "cmd /c netsh wlan disconnect" );
	std::string cmd2( "cmd /c netsh wlan connect name=" );

	m_ctrlEditSsid.GetWindowText( str );
	std::string ssid( str.GetBuffer() );

	cmd2 += ssid;

	RunCmdProc( cmd1 );
	RunCmdProc( cmd2 );
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
