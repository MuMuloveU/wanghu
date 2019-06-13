#include "StdAfx.h"
#include "GameScoreWnd.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameScoreWnd, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameScoreWnd::CGameScoreWnd() : CDialog(IDD_OPTION)
{
	//设置变量
	memset(&m_GameScoreInfo,0,sizeof(m_GameScoreInfo));
	m_ImageBack.SetLoadInfo(IDB_GAME_SCORE,AfxGetInstanceHandle());

	return;
}

//析构函数
CGameScoreWnd::~CGameScoreWnd()
{
}

//控件绑定
void CGameScoreWnd::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE, m_btClose);
}

//初始化函数
BOOL CGameScoreWnd::OnInitDialog()
{
	__super::OnInitDialog();

	//移动窗口
	CImageHandle BackImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOZORDER|SWP_NOREPOSITION);

	//设置按钮
	m_btClose.SetButtonImage(IDB_BT_CLOSE,AfxGetInstanceHandle(),false);
	m_btClose.SetWindowPos(NULL,m_ImageBack.GetWidth()-25,10,0,0,SWP_NOSIZE|SWP_NOZORDER);

	return TRUE;
}

//关闭按钮
void CGameScoreWnd::OnClose()
{
	ShowWindow(SW_HIDE);
	return;
}

//重画函数
void CGameScoreWnd::OnPaint()
{
	CPaintDC dc(this);

	//绘画背景
	CImageHandle BackImageHandle(&m_ImageBack);
	m_ImageBack.BitBlt(dc,0,0);

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//标题
	dc.SetTextColor(RGB(10,10,10));
	dc.TextOut(105,9,TEXT("游戏成绩单"),10);

	//变量定义
	CRect rcDraw;
	TCHAR szBuffer[64];
	dc.SetTextColor(RGB(200,200,200));

	//用户成绩
	for (int i=0;i<GAME_PLAYER;i++)
	{
		//用户名字
		rcDraw.left=40;
		rcDraw.top=79+i*33;
		rcDraw.bottom=rcDraw.top+12;
		rcDraw.right=m_ImageBack.GetWidth()/2-5;
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_GameScoreInfo.lScore[i]);
		dc.DrawText(m_GameScoreInfo.szUserName[i],lstrlen(m_GameScoreInfo.szUserName[i]),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		//用户积分
		rcDraw.top=79+i*33;
		rcDraw.bottom=rcDraw.top+12;
		rcDraw.left=m_ImageBack.GetWidth()/2+5;
		rcDraw.right=m_ImageBack.GetWidth()-45;
		dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}

	return;
}

//绘画背景
BOOL CGameScoreWnd::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//鼠标消息
void CGameScoreWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//设置积分
void CGameScoreWnd::SetScoreInfo(tagGameScoreInfo & GameScoreInfo)
{
	//设置变量
	m_GameScoreInfo=GameScoreInfo;

	//显示窗口
	Invalidate(NULL);
	ShowWindow(SW_SHOW);

	return;
}

//////////////////////////////////////////////////////////////////////////
