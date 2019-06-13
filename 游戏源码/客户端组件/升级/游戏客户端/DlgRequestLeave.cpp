#include "Stdafx.h"
#include "Resource.h"
#include "DlgRequestLeave.h"
#include ".\dlgrequestleave.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgRequestLeave, CSkinDialogEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgRequestLeave::CDlgRequestLeave() : CSkinDialogEx(IDD_REQUEST_LEAVE)
{
	//设置变量
	m_pGameFrameDlg=NULL;

	return;
}

//析构函数
CDlgRequestLeave::~CDlgRequestLeave()
{
}

//控件绑定
void CDlgRequestLeave::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//初始化函数
BOOL CDlgRequestLeave::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("请求离开："));

	//设置限制
	((CEdit *)GetDlgItem(IDC_LEAVE_REASON))->LimitText(127);

	return FALSE;
}

//确定函数
VOID CDlgRequestLeave::OnOK()
{
	//获取消息
	TCHAR szLeaveReason[128]=TEXT("");
	GetDlgItemText(IDC_LEAVE_REASON,szLeaveReason,CountArray(szLeaveReason));

	//构造消息
	if (szLeaveReason[0]==0) lstrcpyn(szLeaveReason,TEXT("此用户比较懒，没有填写任何离开理由"),CountArray(szLeaveReason));

	//发送消息
	ASSERT(m_pGameFrameDlg!=NULL);
	m_pGameFrameDlg->PostMessage(IDM_REQUEST_LEAVE,(WPARAM)szLeaveReason,0);

	//关闭窗口
	DestroyWindow();

	return;
}

//取消消息
VOID CDlgRequestLeave::OnCancel()
{
	//关闭窗口
	DestroyWindow();

	return;
}

//////////////////////////////////////////////////////////////////////////
