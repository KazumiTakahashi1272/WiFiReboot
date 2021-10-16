#pragma once

#include "WiFiRebootDlg.h"
#include "afxwin.h"
#include "ThemeHelperST.h"
#include "XPStyleButtonST.h"

// CMsgDlg �_�C�A���O
using namespace std;         //  ���O��Ԏw��

class CMsgDlg : public CDialog
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(LPCTSTR ssid, CWiFiRebootDlg* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CMsgDlg();

	CThemeHelperST		m_ThemeHelper;

	CWiFiRebootDlg* m_pParent;
	string m_ssid;

	string m_msg;

// �_�C�A���O �f�[�^
	enum { IDD = IDD_MSG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_ctrlMsgEdit;
	CXPStyleButtonST m_ctrlOk;
	CXPStyleButtonST m_ctrlCancel;
};
