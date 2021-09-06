
// WiFiRebootDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"

#include <string>       // �w�b�_�t�@�C���C���N���[�h
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
	CEdit m_ctrlEditSsid;

private:
	DWORD RunCmdProc(string cmd);
};
