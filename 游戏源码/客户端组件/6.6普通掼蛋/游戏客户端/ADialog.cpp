// ADialog.cpp : 实现文件
//

#include "stdafx.h"
#include "ADialog.h"
#include ".\adialog.h"


// CADialog 对话框

IMPLEMENT_DYNAMIC(CADialog, CSkinDialogEx)
CADialog::CADialog()
	: CSkinDialogEx(IDD_ANSWER)
	, m_bCause(_T(""))
{
}

CADialog::~CADialog()
{
}

void CADialog::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_bCause);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}


BEGIN_MESSAGE_MAP(CADialog, CSkinDialogEx)
END_MESSAGE_MAP()


// CADialog 消息处理程序

BOOL CADialog::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
    SetWindowText(m_title);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
