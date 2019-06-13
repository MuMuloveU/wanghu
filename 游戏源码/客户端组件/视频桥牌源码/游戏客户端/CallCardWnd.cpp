#include "StdAfx.h"
#include "GameLogic.h"
#include "CallCardWnd.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识
#define IDC_CALL_NT						100								//叫牌按钮
#define IDC_CALL_HEI_TAO				101								//叫牌按钮
#define IDC_CALL_HONG_TAO				102								//叫牌按钮
#define IDC_CALL_MEI_HUA				103								//叫牌按钮
#define IDC_CALL_FANG_KUAI				104								//叫牌按钮

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCallCardWnd, CWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CCallCardWnd::CCallCardWnd()
{
}

//析构函数
CCallCardWnd::~CCallCardWnd()
{
}

//命令函数
BOOL CCallCardWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_CALL_NT:			//无主
		{
			//发送消息
			AfxGetMainWnd()->PostMessage(IDM_CALL_CARD,COLOR_NT,COLOR_NT);

			return TRUE;
		}
	case IDC_CALL_HEI_TAO:		//黑桃
		{
			//发送消息
			AfxGetMainWnd()->PostMessage(IDM_CALL_CARD,COLOR_HEI_TAO,COLOR_HEI_TAO);

			return TRUE;
		}
	case IDC_CALL_HONG_TAO:		//红桃
		{
			//发送消息
			AfxGetMainWnd()->PostMessage(IDM_CALL_CARD,COLOR_HONG_TAO,COLOR_HONG_TAO);

			return TRUE;
		}
	case IDC_CALL_MEI_HUA:		//梅花
		{
			//发送消息
			AfxGetMainWnd()->PostMessage(IDM_CALL_CARD,COLOR_MEI_HUA,COLOR_MEI_HUA);

			return TRUE;
		}
	case IDC_CALL_FANG_KUAI:	//方块
		{
			//发送消息
			AfxGetMainWnd()->PostMessage(IDM_CALL_CARD,COLOR_FANG_KUAI,COLOR_FANG_KUAI);

			return TRUE;
		}
	}

	return __super::OnCommand(wParam,lParam);
}

//更新控制
void CCallCardWnd::UpdateCallControl(BYTE cbCallColor)
{
	//更新控件
	/*
	m_btCallNT.EnableWindow((cbCallColor&CALL_NT)?TRUE:FALSE);
	m_btCallHeiTao.EnableWindow((cbCallColor&CALL_HEI_TAO)?TRUE:FALSE);
	m_btCallHongTao.EnableWindow((cbCallColor&CALL_HONG_TAO)?TRUE:FALSE);
	m_btCallMeiHua.EnableWindow((cbCallColor&CALL_MEI_HUA)?TRUE:FALSE);
	m_btCallFangKuai.EnableWindow((cbCallColor&CALL_FANG_KUAI)?TRUE:FALSE);
	*/
	m_btCallNT.EnableWindow(CALL_NT);
	m_btCallHeiTao.EnableWindow(CALL_HEI_TAO);
	m_btCallHongTao.EnableWindow(CALL_HONG_TAO);
	m_btCallMeiHua.EnableWindow(CALL_MEI_HUA);
	m_btCallFangKuai.EnableWindow(CALL_FANG_KUAI);

	return;
}

//更新控制
void CCallCardWnd::UpdateCallControl(bool bCallNT, bool bCallHeiTao, bool bCallHongTao, bool CallMeiHua, bool bCallFangKuai)
{
	//更新控件
	m_btCallNT.EnableWindow((bCallNT==true)?TRUE:FALSE);
	m_btCallHeiTao.EnableWindow((bCallHeiTao==true)?TRUE:FALSE);
	m_btCallHongTao.EnableWindow((bCallHongTao==true)?TRUE:FALSE);
	m_btCallMeiHua.EnableWindow((CallMeiHua==true)?TRUE:FALSE);
	m_btCallFangKuai.EnableWindow((bCallFangKuai==true)?TRUE:FALSE);

	return;
}

//建立消息
int CCallCardWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建按钮
	CRect rcCreate(0,0,0,0);
	m_btCallNT.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CALL_NT);
	m_btCallHeiTao.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CALL_HEI_TAO);
	m_btCallHongTao.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CALL_HONG_TAO);
	m_btCallMeiHua.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CALL_MEI_HUA);
	m_btCallFangKuai.Create(TEXT(""),WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CALL_FANG_KUAI);

	//获取位置
	CRect rcButton;
	m_btCallNT.GetClientRect(&rcButton);

	//移动按钮
	m_btCallNT.SetWindowPos(NULL,0,0,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);
	m_btCallHeiTao.SetWindowPos(NULL,rcButton.Width(),0,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);
	m_btCallHongTao.SetWindowPos(NULL,rcButton.Width()*2,0,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);
	m_btCallMeiHua.SetWindowPos(NULL,rcButton.Width()*3,0,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);
	m_btCallFangKuai.SetWindowPos(NULL,rcButton.Width()*4,0,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);

	//移动窗口
	SetWindowPos(NULL,0,0,rcButton.Width()*5,rcButton.Height(),SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
