
// WiFiRebootDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include <cstdio>
#include <iostream>
#include <string>       // �w�b�_�t�@�C���C���N���[�h
#include "xpbutton.h"
#include "afxcmn.h"
using namespace std;         //  ���O��Ԏw��


// CWiFiRebootDlg �_�C�A���O
class CWiFiRebootDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CWiFiRebootDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_WIFIREBOOT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g

public:
	static void WlanNotification(WLAN_NOTIFICATION_DATA *wlanNotifData, VOID *p);

	void MsgReporter(const char *format, ...);
	void ErrReporter(const char *format, ...);

// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
