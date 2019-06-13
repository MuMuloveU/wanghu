#include "StdAfx.h"
#include "ScoreView.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView(void) : CDialog(IDD_GAME_SCORE)
{
	//设置数据
	m_lGameTax=0L;
	m_lGameScore=0;

	//加载资源
	m_ImageBackWin.SetLoadInfo(IDB_GAME_END_WIN,AfxGetInstanceHandle());
	m_ImageBackLost.SetLoadInfo(IDB_GAME_END_LOST,AfxGetInstanceHandle());
}

//析构函数
CScoreView::~CScoreView(void)
{
}

//初始化函数
BOOL CScoreView::OnInitDialog()
{
	__super::OnInitDialog();

	//设置数据
	m_lGameTax=0L;
	m_lGameScore=0;

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	CImageHandle ImageHandleWin(&m_ImageBackWin);
	SetWindowPos(NULL,0,0,m_ImageBackWin.GetWidth(),m_ImageBackWin.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
	CImageHandle ImageHandleLost(&m_ImageBackLost);
	SetWindowPos(NULL,0,0,m_ImageBackLost.GetWidth(),m_ImageBackLost.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//居中窗口
	CenterWindow(GetParent());

	return TRUE;
}

//重画函数
void CScoreView::OnPaint() 
{
	CPaintDC dc(this); 

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(250,250,250));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画背景
	CImageHandle ImageHandleW(&m_ImageBackWin);
	CImageHandle ImageHandleL(&m_ImageBackLost);

	if(m_lGameScore>=0) 
		m_ImageBackWin.BitBlt(dc,0,0);
	else
		m_ImageBackLost.BitBlt(dc,0,0);
	
	//显示分数
	CRect rcDraw;
	TCHAR szBuffer[64]=TEXT("");

	//显示税收
	rcDraw.left=30;
	rcDraw.right=214;
	rcDraw.top=25+51;
	rcDraw.bottom=rcDraw.top+12;
	if(m_lGameScore>=0) _snprintf(szBuffer,sizeof(szBuffer),TEXT("你赢得了 %ld 。"),m_lGameScore);
	else _snprintf(szBuffer,sizeof(szBuffer),TEXT("你失去了 %ld 。"),-m_lGameScore);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

	//显示税收
	rcDraw.left=120;
	rcDraw.right=190;
	rcDraw.top=110+51;
	rcDraw.bottom=rcDraw.top+12;
	_snprintf(szBuffer,sizeof(szBuffer),"%ld",m_lGameTax);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

	return;
}

//绘画背景
BOOL CScoreView::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//鼠标消息
void CScoreView::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//重置积分
void CScoreView::ResetScore()
{
	//设置数据
	m_lGameTax=0L;
	m_lGameScore=0;

	//绘画视图
	Invalidate(NULL);
	
	return;
}

//设置税收
void CScoreView::SetGameTax(LONG lGameTax)
{
	//设置变量
	if (m_lGameTax!=lGameTax)
	{
		m_lGameTax=lGameTax;
		Invalidate(FALSE);
	}

	return;
}

//设置积分
void CScoreView::SetGameScore(LONG lScore)
{
	//设置变量
	m_lGameScore=lScore;
	Invalidate(FALSE);

	return;
}

//////////////////////////////////////////////////////////////////////////
