//***********************************************
//File		: SettingDlg.cpp 
//Time		: 2005-05-09
//Author	: 
//Comment	: 
//***********************************************


#include "stdafx.h"
#include "SettingDlg.h"


IMPLEMENT_DYNAMIC(CSettingDlg, CSkinDialogEx)
CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CSkinDialogEx(CSettingDlg::IDD, pParent)
{
	m_IsSound=true;
	m_bShunShiZhen=false;
	m_bIsCanLook =true;
	m_bIsCanChoose=true;
}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CSkinDialogEx)
END_MESSAGE_MAP()

//
//初始化
//
BOOL CSettingDlg::OnInitDialog()
{
	__super::OnInitDialog();

	SetWindowText("设置选项：");

	if (m_IsSound)
	{
		CButton *pSoundBtn = (CButton *)this->GetDlgItem(IDC_CK_SOUND);
		pSoundBtn->SetCheck(BST_CHECKED);
	}
	if(m_bShunShiZhen)
	{
		CButton *pSoundBtn = (CButton *)this->GetDlgItem(IDC_SHI_ZHEN);
		pSoundBtn->SetCheck(BST_CHECKED);
	}
	if(m_bIsCanLook)
	{
		CButton *pSoundBtn = (CButton *)this->GetDlgItem(IDC_CAN_LOOK);
		pSoundBtn->SetCheck(BST_CHECKED);
	}
	if(!m_bIsCanChoose)
	{
		CButton *pSoundBtn = (CButton *)this->GetDlgItem(IDC_CAN_LOOK);
		pSoundBtn->EnableWindow(FALSE);
		pSoundBtn = (CButton *)this->GetDlgItem(IDC_SHI_ZHEN);
		pSoundBtn->EnableWindow(FALSE);
	}

	return TRUE;  
}

//
//屏蔽 Enter 键
//
BOOL CSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message==WM_KEYDOWN)
		&& (pMsg->wParam==VK_RETURN))
	{
		return true;
	}

	return __super::PreTranslateMessage(pMsg);
}

//
//OK
//
void CSettingDlg::OnOK()
{

	CButton *pBtn = (CButton *)this->GetDlgItem(IDC_CK_SOUND);
	if(!pBtn)
	{
		return ;
	}

	if(pBtn->GetCheck() == BST_CHECKED)
	{
		m_IsSound = TRUE;
	}
	else
	{
		m_IsSound = FALSE;
	}
	pBtn = (CButton *)this->GetDlgItem(IDC_SHI_ZHEN);
	if(pBtn->GetCheck() == BST_CHECKED)
	{
		m_bShunShiZhen = TRUE;
	}
	else
	{
		m_bShunShiZhen = FALSE;
	}
	pBtn = (CButton *)this->GetDlgItem(IDC_CAN_LOOK);
	if(pBtn->GetCheck() == BST_CHECKED)
	{
		m_bIsCanLook = TRUE;
	}
	else
	{
		m_bIsCanLook = FALSE;
	}

	__super::OnOK();
}

//
//Cancel
//
void CSettingDlg::OnCancel()
{
	__super::OnCancel();
}


//-----------------------------------------------
//					the end
//-----------------------------------------------
