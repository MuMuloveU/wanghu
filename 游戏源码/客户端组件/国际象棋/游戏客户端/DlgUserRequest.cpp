#include "Stdafx.h"
#include "DlgUserRequest.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define IDI_TIME					10									//定时器 ID

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgUserRequest, CSkinDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_AGREE, OnBnClickedApprove)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgUserRequest::CDlgUserRequest() : CSkinDialogEx(IDD_USER_REQ)
{
	//设置变量
	m_nElapse=15;
	m_uMessage=0;
	m_nResidualTime=0;

	return;
}

//析构函数
CDlgUserRequest::~CDlgUserRequest()
{
}

//控件绑定
void CDlgUserRequest::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AGREE, m_btApprove);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//初始化函数
BOOL CDlgUserRequest::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	SetWindowText(TEXT("用户请求"));
	SetDlgItemText(IDC_REQUEST_TEXT,m_strMessage);
	
	//设置定时器
	m_nResidualTime=m_nElapse;
	SetTimer(IDI_TIME,1000,NULL);

	return TRUE;
}

//确定消息
void CDlgUserRequest::OnOK()
{
	return;
}

//取消消息
void CDlgUserRequest::OnCancel()
{
	AfxGetMainWnd()->PostMessage(m_uMessage,0,0);
	DestroyWindow();

	return;
}

//配置请求
void CDlgUserRequest::InitRequest(UINT uMessage, UINT nElapse, LPCTSTR pszMessage)
{
	//设置变量
	m_nElapse=nElapse;
	m_uMessage=uMessage;
	m_strMessage=pszMessage;

	return;
}

//同意按钮
void CDlgUserRequest::OnBnClickedApprove()
{
	AfxGetMainWnd()->PostMessage(m_uMessage,TRUE,TRUE);
	DestroyWindow();

	return;
}

//定时器消息
void CDlgUserRequest::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==IDI_TIME)
	{
		//设置标题
		if (m_nResidualTime>0)
		{
			TCHAR szTitle[64]=TEXT("");
			_snprintf(szTitle,sizeof(szTitle),TEXT("用户请求 -- %ld 秒"),m_nResidualTime);
			SetWindowText(szTitle);
		}
		else 
		{
			KillTimer(IDI_TIME);
			PostMessage(WM_CLOSE,0,0);
		}

		//设置变量
		m_nResidualTime--;

		return;
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
