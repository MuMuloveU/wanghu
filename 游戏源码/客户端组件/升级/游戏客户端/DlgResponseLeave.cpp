// DlgResponseLeave.cpp : implementation file
//

#include "stdafx.h"
#include "DlgResponseLeave.h"

//////////////////////////////////////////////////////////////////////////

#define			IDI_DESTROY_DLG						500			//关闭对话框定时器

#define			TIME_CLOSE_DLG						15			//关闭对话框时间

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgResponseLeave, CSkinDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////


//构造函数
CDlgResponseLeave::CDlgResponseLeave(CWnd * pParentWnd):CSkinDialogEx(IDD_REQUEST_LEAVE,pParentWnd)
{
	m_wRequestUser = INVALID_CHAIR;
	m_pGameFrameDlg = NULL;
	m_wTimer = 0;
	m_strCaption = TEXT("玩家请求离开");
}

//析构函数
CDlgResponseLeave::~CDlgResponseLeave()
{
}

//控件绑定
void CDlgResponseLeave::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//初始化函数
BOOL CDlgResponseLeave::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(m_strCaption);


	return FALSE;
}

//确定函数
VOID CDlgResponseLeave::OnOK()
{
	//发送消息
	m_pGameFrameDlg->PostMessage(IDM_RESPONSE_LEAVE,(WPARAM)this,(LPARAM)TRUE);
	//关闭窗口
	DestroyWindow();
	return;
}

//取消消息
VOID CDlgResponseLeave::OnCancel()
{
	//发送消息
	m_pGameFrameDlg->PostMessage(IDM_RESPONSE_LEAVE,(WPARAM)this,FALSE);
	//关闭窗口
	DestroyWindow();
	return;
}

//显示消息
void CDlgResponseLeave::ShowRequestMessage(WORD wChairId,LPCTSTR pszCaption,LPCTSTR pszMessage)
{
	//效验
	ASSERT( m_pGameFrameDlg != NULL && m_wRequestUser == INVALID_CHAIR );
	//设置变量
	m_wRequestUser = wChairId;
	//设置文字
	((CStatic *)GetDlgItem(IDC_TEXT_CAPTION))->SetWindowText(pszCaption);
	((CEdit *)GetDlgItem(IDC_EDIT_REQUEST))->SetWindowText(pszMessage);
	//设置时间
	m_wTimer = TIME_CLOSE_DLG;
	SetTimer(IDI_DESTROY_DLG,1000,NULL);
	//显示
	ShowWindow(SW_SHOW);
	return ;
}

//////////////////////////////////////////////////////////////////////////

void CDlgResponseLeave::OnTimer(UINT nIDEvent)
{
	switch( nIDEvent )
	{
	case IDI_DESTROY_DLG:
		{
			if( --m_wTimer > 0 )
			{
				TCHAR szBuffer[32];
				_snprintf(szBuffer,CountArray(szBuffer),TEXT("%s(%d)"),m_strCaption,m_wTimer);
				SetWindowText(szBuffer);
				SetTimer(IDI_DESTROY_DLG,1000,NULL);
				return;
			}
			//发送消息
			m_pGameFrameDlg->PostMessage(IDM_RESPONSE_LEAVE,(WPARAM)this,(LPARAM)FALSE);
			//关闭对话框
			PostMessage(WM_CLOSE);
			return ;
		}
	}
	__super::OnTimer(nIDEvent);
}
