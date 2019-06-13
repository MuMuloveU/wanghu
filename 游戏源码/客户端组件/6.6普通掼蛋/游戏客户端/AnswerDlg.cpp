// AnswerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AnswerDlg.h"
#include ".\answerdlg.h"


// CAnswerDlg 对话框

IMPLEMENT_DYNAMIC(CAnswerDlg, CSkinDialogEx)
CAnswerDlg::CAnswerDlg()
	: CSkinDialogEx(IDD_DIALOG)
	, m_answer(_T(""))
{
}

CAnswerDlg::~CAnswerDlg()
{
}

void CAnswerDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_answer);
	DDX_Control(pDX, IDOK, m_btOK);
}


BEGIN_MESSAGE_MAP(CAnswerDlg, CSkinDialogEx)
END_MESSAGE_MAP()


// CAnswerDlg 消息处理程序

BOOL CAnswerDlg::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText("对方的回复");
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
