#include "Stdafx.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameOption, CSkinDialogEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameOption::CGameOption() : CSkinDialogEx(IDD_OPTION)
{
	m_bEnableSound=true;
	m_bShowChessName=false;

	return;
}

//析构函数
CGameOption::~CGameOption()
{
}

//控件绑定
void CGameOption::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//初始化函数
BOOL CGameOption::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("游戏配置"));

	//设置控件
	if (m_bEnableSound==true) ((CButton *)GetDlgItem(IDC_ENABLE_SOUND))->SetCheck(BST_CHECKED);
	if (m_bShowChessName==true) ((CButton *)GetDlgItem(IDC_SHOW_NAME))->SetCheck(BST_CHECKED);

	return TRUE;
}

//确定消息
void CGameOption::OnOK()
{
	//获取变量
	m_bShowChessName=(((CButton *)GetDlgItem(IDC_SHOW_NAME))->GetCheck()==BST_CHECKED);
	m_bEnableSound=(((CButton *)GetDlgItem(IDC_ENABLE_SOUND))->GetCheck()==BST_CHECKED);

	__super::OnOK();
}

//////////////////////////////////////////////////////////////////////////
