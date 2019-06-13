#include "StdAfx.h"
#include "ScoreView.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView()
{
	//设置数据
	ZeroMemory(m_lGameTax,sizeof(m_lGameTax));
	ZeroMemory(m_lScore,sizeof(m_lScore));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));

	//加载资源
	m_ImageBack.SetLoadInfo(IDB_SCORE_VIEW,AfxGetInstanceHandle());
	m_ImageWinLose.SetLoadInfo(IDB_WIN_LOSE,AfxGetInstanceHandle());

	return;
}

//析构函数
CScoreView::~CScoreView()
{
}

//建立消息
int CScoreView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置数据
	ZeroMemory(m_lGameTax,sizeof(m_lGameTax));
	ZeroMemory(m_lScore,sizeof(m_lScore));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	CImageHandle HandleBack(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	return 0;
}

//鼠标消息
void CScoreView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//重画函数
void CScoreView::OnPaint() 
{
	CPaintDC dc(this); 

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//设置区域
	CRect rt;
	GetClientRect(&rt);
	CRgn WndRgn;
	WndRgn.CreateRoundRectRgn(rt.left,rt.top,rt.right,rt.bottom,10,10);
	SetWindowRgn(WndRgn,TRUE);
	WndRgn.DeleteObject();

	//绘画背景
	CImageHandle ImageHandle(&m_ImageBack);
	m_ImageBack.BitBlt(dc,0,0);

	//显示分数
	WORD wStation=0;
	TCHAR szBuffer[64]=TEXT("");
	CImageHandle HandleWinLose(&m_ImageWinLose);
	INT nWinLoseWidth = m_ImageWinLose.GetWidth()/3;
	INT nWinLostHeight = m_ImageWinLose.GetHeight();
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		if (m_szUserName[i][0]!=0)
		{
			//输出名字
			CRect rcName(28,wStation*29+79,117,(wStation+1)*29+79);
			dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcName,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//输出成绩
			CRect rcScore(117,wStation*29+79,182,(wStation+1)*29+79);
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lScore[i]);
			dc.DrawText(szBuffer,lstrlen(szBuffer),&rcScore,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//胜负标志
			BYTE byIndex = 0;
			if( m_lScore[i] == 0 ) byIndex = 2;
			else if( m_lScore[i] < 0 ) byIndex = 1;
			m_ImageWinLose.AlphaDrawImage(&dc,182+(253-182)/2-nWinLoseWidth/2,wStation*29+79+29/2-nWinLostHeight/2,nWinLoseWidth,nWinLostHeight,
				byIndex*nWinLoseWidth,0,RGB(255,0,255));

			//服务费用
			CRect rcService(253,wStation*29+79,321,(wStation+1)*29+79);
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lGameTax[i]);
			dc.DrawText(szBuffer,lstrlen(szBuffer),&rcService,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//设置变量
			wStation++;
		}
	}

	return;
}

//重置积分
void CScoreView::ResetScore()
{
	//设置数据
	ZeroMemory(m_lGameTax,sizeof(m_lGameTax));
	ZeroMemory(m_lScore,sizeof(m_lScore));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));

	//绘画视图
	if (m_hWnd!=NULL) Invalidate(FALSE);
	
	return;
}

//设置税收
void CScoreView::SetGameTax(WORD wChairID,LONG lGameTax)
{
	if( wChairID == INVALID_CHAIR )
	{
		ZeroMemory(m_lGameTax,sizeof(m_lGameTax));
		//重画窗口
		if (m_hWnd!=NULL) Invalidate(FALSE);
	}
	ASSERT( wChairID >= 0 && wChairID < GAME_PLAYER );
	if ( m_lGameTax[wChairID] != lGameTax )
	{
		//设置变量
		m_lGameTax[wChairID] = lGameTax;

		//重画窗口
		if (m_hWnd!=NULL) Invalidate(FALSE);
	}
	return;
}

//设置积分
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONG lScore)
{
	if (wChairID<CountArray(m_lScore))
	{
		//设置变量
		m_lScore[wChairID]=lScore;
		lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));

		//重画窗口
		if (m_hWnd!=NULL) Invalidate(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
