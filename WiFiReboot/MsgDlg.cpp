// MsgDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "WiFiReboot.h"
#include "MsgDlg.h"

#include <iostream>
#include <string>       // �w�b�_�t�@�C���C���N���[�h
#include <vector>

using namespace std;         //  ���O��Ԏw��

// CMsgDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CMsgDlg, CDialog)

CMsgDlg::CMsgDlg(LPCTSTR ssid, CWiFiRebootDlg* pParent /*=NULL*/)
	: CDialog(CMsgDlg::IDD, pParent)
{
	m_pParent = pParent;
	m_ssid = ssid;
}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG, m_ctrlMsgEdit);
	DDX_Control(pDX, IDCANCEL, m_ctrlCancel);
	DDX_Control(pDX, IDOK, m_ctrlOk);
}


BEGIN_MESSAGE_MAP(CMsgDlg, CDialog)
END_MESSAGE_MAP()


// CMsgDlg ���b�Z�[�W �n���h��

BOOL CMsgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �����ɏ�������ǉ����Ă�������
	m_ctrlOk.SetThemeHelper( &m_ThemeHelper );
	m_ctrlOk.SetIcon( IDI_OK, (int)BTNST_AUTO_GRAY );
	m_ctrlOk.OffsetColor( CButtonST::BTNST_COLOR_BK_IN, 30 );

	m_ctrlCancel.SetThemeHelper( &m_ThemeHelper );
	m_ctrlCancel.SetIcon( IDI_CANCEL, (int)BTNST_AUTO_GRAY );
	m_ctrlCancel.OffsetColor( CButtonST::BTNST_COLOR_BK_IN, 30 );

	CFont font;
	LOGFONT lf;
	memset( &lf, 0, sizeof(LOGFONT) );
	lf.lfHeight = 15;
	lf.lfWeight = FW_MEDIUM;
	strcpy( lf.lfFaceName, "���C���I" );
	font.CreateFontIndirect( &lf );
	m_ctrlMsgEdit.SetFont( &font );

	if ( m_pParent == NULL )
	{
		m_msg = "(null)";
		return TRUE;
	}

	vector<wlan_Interface*>::iterator itrInt = m_pParent->m_vInterface.begin();
	while ( itrInt != m_pParent->m_vInterface.end() )
	{
		wlan_Interface* pInt = *itrInt;

		vector<wlan_Profile*>::iterator itrPro = pInt->vProfile.begin();
		while ( itrPro != pInt->vProfile.end() )
		{
			wlan_Profile* pProfile = *itrPro;
			if ( pProfile->Ssid == m_ssid )
			{
				m_msg = pProfile->msg;
				break;
			}
			itrPro++;
		}

		itrInt++;
	}

	m_ctrlMsgEdit.SetWindowText( m_msg.c_str() );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}
