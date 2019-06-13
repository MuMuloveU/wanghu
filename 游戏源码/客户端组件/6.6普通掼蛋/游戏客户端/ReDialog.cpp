// ReDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "ReDialog.h"
#include ".\redialog.h"


// CReDialog 对话框

IMPLEMENT_DYNAMIC(CReDialog, CSkinDialogEx)
CReDialog::CReDialog()
	: CSkinDialogEx(IDD_REQUEST)
	, m_bCause(_T(""))
{
}

CReDialog::~CReDialog()
{
}

void CReDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_REQUEST, m_bCause);
	DDV_MaxChars(pDX, m_bCause, 50);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}


BEGIN_MESSAGE_MAP(CReDialog, CSkinDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CReDialog 消息处理程序

void CReDialog::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnCancel();
}

void CReDialog::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::EndDialog(IDQUIT);
}

BOOL CReDialog::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText("请求退出");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
