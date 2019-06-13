#include "StdAfx.h"
#include "GameScoreWnd.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameScoreWnd, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameScoreWnd::CGameScoreWnd()
{
	//设置变量
	memset(&m_GameScoreInfo,0,sizeof(m_GameScoreInfo));

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBack.SetLoadInfo(IDB_GAME_SCORE,hInstance);

	//获取大小
	CImageHandle BackImageHandle(&m_ImageBack);
	m_ScoreSize.cx=m_ImageBack.GetWidth();
	m_ScoreSize.cy=m_ImageBack.GetHeight();

	return;
}

//析构函数
CGameScoreWnd::~CGameScoreWnd()
{
}

//重画函数
void CGameScoreWnd::OnPaint()
{
	CPaintDC dc(this);

	//加载资源
	CImageHandle BackHandle(&m_ImageBack);

	//绘画背景
	m_ImageBack.BitBlt(dc,0,0);

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//用户成绩
	TCHAR szBuffer[64];
	for (int i=0;i<GAME_PLAYER;i++)
	{
		dc.SetTextAlign((i==0)?TA_LEFT:TA_RIGHT);
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_GameScoreInfo.lScore[i]);
		dc.TextOut(20+230*i,66,m_GameScoreInfo.szUserName[i],lstrlen(m_GameScoreInfo.szUserName[i]));
		dc.SetTextAlign(TA_CENTER);
		dc.TextOut(100+75*i,45,szBuffer,lstrlen(szBuffer));
	}

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
