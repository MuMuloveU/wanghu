#include "StdAfx.h"
#include "GameRuleSet.h"
#include "GameClientDlg.h"
#include ".\gameruleset.h"

IMPLEMENT_DYNAMIC(CGameRuleSet, CSkinDialogEx)

BEGIN_MESSAGE_MAP(CGameRuleSet, CSkinDialogEx)
	ON_BN_CLICKED(IDC_STRAT_PLAY, OnBnClickedStratPlay)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

//构造函数
CGameRuleSet::CGameRuleSet(CWnd* pParent /*=NULL*/)
	: CSkinDialogEx(CGameRuleSet::IDD, pParent)
{
}

//析构函数
CGameRuleSet::~CGameRuleSet()
{
}

//DDX/DDV 支持
void CGameRuleSet::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STRAT_PLAY, m_Play);
}

//初始化函数
BOOL CGameRuleSet::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();
	return TRUE;
}


void CGameRuleSet::OnBnClickedStratPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	((CGameClientDlg*)AfxGetMainWnd())->GameStart();
	this->OnOK();
}
void CGameRuleSet::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	((CGameClientDlg*)AfxGetMainWnd())->GameQuit();
	CSkinDialogEx::OnClose();
}
