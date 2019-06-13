#include "stdafx.h"
#include "Prompt.h"
#include ".\Prompt.h"

//消息定义
#define IDM_EXIT					WM_USER+101							//离开消息

///////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPrompt, CDialog)

	ON_WM_PAINT()

END_MESSAGE_MAP()

//构造函数
CPrompt::CPrompt(): CDialog(IDD_DIALOG2)
{
	ZeroMemory(m_tStr,sizeof(m_tStr));
}

//析构函数
CPrompt::~CPrompt()
{
}

//控件绑定
void CPrompt::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//初始化函数
BOOL CPrompt::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;
}

//重画函数
void CPrompt::OnPaint() 
{
	CPaintDC dc(this); 

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0,0,0));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//创建字体
	CFont ViewFont;
	ViewFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,1,TEXT("Arial"));
	CFont *pOldFont=dc.SelectObject(&ViewFont);

	CRect rcScore(0,30,220,30+19);
	dc.DrawText(m_tStr,lstrlen(m_tStr),&rcScore,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

	//还原字体
	dc.SelectObject(pOldFont);
	ViewFont.DeleteObject();

	return;
}

//命令信息
BOOL CPrompt::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	BYTE bTemp = (BYTE)wParam;

	if(bTemp==IDOK || bTemp==IDCANCEL)
	{
		AfxGetMainWnd()->PostMessage(IDM_EXIT,0,0);	

		OnOK();
		return TRUE;
	}

	return CDialog::OnCommand(wParam, lParam);
}

//设置对白
void CPrompt::SetString(TCHAR szString[])
{
	CopyMemory(m_tStr,szString,CountArray(m_tStr));

	//绘画视图
	Invalidate(NULL);

	return ;
}

//命令信息
BOOL CPrompt::PreTranslateMessage(MSG* pMsg)
{
	//过滤字母
	if(pMsg->message==WM_KEYDOWN)
	{
		BYTE bMesValue = (BYTE)(pMsg->wParam); 
		if(bMesValue==VK_SPACE || bMesValue==VK_RETURN) return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
///////////////////////////////////////////////////////////////////////////

