#include "StdAfx.h"
#include "ScoreView.h"
#include ".\scoreview.h"


#define TIMES				5							//时间参数

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView(void)
{
	//设置数据
	m_lGameScore = 0L;
	m_bStart = false;
	m_bTimes = TIMES;

	//加载资源
	m_ImageBack.SetLoadInfo(IDB_GAME_WIN,AfxGetInstanceHandle());
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
	m_lGameScore = 0L;

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	CImageHandle ImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth()/3,m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	return TRUE;
}

//重画函数
void CScoreView::OnPaint() 
{
	if(this->IsWindowVisible()!=TRUE)return;

	CPaintDC dc(this); 
	//SetupRegion(&dc,m_ImageBack,RGB(255,0,255));

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(250,250,250));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画背景
	CImageHandle ImageHandle(&m_ImageBack);
	if (m_lGameScore > 0L)
	{
	//	m_ImageBack.BitBlt(dc.GetSafeHdc(),0,0,m_ImageBack.GetWidth()/3,m_ImageBack.GetHeight(),0,0,SRCCOPY);
		m_ImageBack.AlphaDrawImage(&dc,0,0,m_ImageBack.GetWidth()/3,m_ImageBack.GetHeight(),0,0,RGB(255,0,255));
	}
	else if(m_lGameScore == 0L)
	{
		m_ImageBack.AlphaDrawImage(&dc,0,0,m_ImageBack.GetWidth()/3,m_ImageBack.GetHeight(),m_ImageBack.GetWidth()/3,0,RGB(255,0,255));
	}
	else
	{
		m_ImageBack.AlphaDrawImage(&dc,0,0,m_ImageBack.GetWidth()/3,m_ImageBack.GetHeight(),2*m_ImageBack.GetWidth()/3,0,RGB(255,0,255));
	}

	//显示分数
	TCHAR szBuffer[64]=TEXT("");
	CRect rcScore(60,60,60+65,60+19);

	//用户积分
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lGameScore);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rcScore,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

	//dc.SetTextColor(RGB(0,0,0));

	//显示秒数
	TCHAR szBuffe[64]=TEXT("");
	int ileft=52;
	int itop=8;
	CRect rcScor(ileft,itop,ileft+15,itop+15);

	//创建资源
	CFont InfoFont;
	InfoFont.CreateFont(-14,0,0,0,10,0,0,0,4,0,2,1,2,TEXT("宋体"));

	//设置 DC
	dc.SetTextAlign(TA_CENTER|TA_TOP);
	CFont * pOldFont=dc.SelectObject(&InfoFont);

	//绘画信息
	_snprintf(szBuffe,sizeof(szBuffer),TEXT("%ld"),m_bTimes);
	//dc.Ellipse(&rcScor);
	dc.SetTextColor(RGB(255,255,255));
	dc.TextOut(ileft+132,itop-2,szBuffe);

	//清理资源
	dc.SelectObject(pOldFont);
	InfoFont.DeleteObject();

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

	//如果父窗口  最小化  则还原
	if(GetParent()->IsIconic())
		GetParent()->ShowWindow(SW_RESTORE);

	return;
}

//重置积分
void CScoreView::ResetScore()
{
	//设置数据
	m_lGameScore =0L;

	//绘画视图
	Invalidate(NULL);
	
	return;
}

//设置税收
void CScoreView::SetGameTax(LONG lGameTax)
{	
	return;
}

//设置积分
void CScoreView::SetGameScore(WORD wChairID,  LONG lScore)
{
	//设置变量
	if (wChairID<GAME_PLAYER)
	{
		m_lGameScore=lScore;
		Invalidate(FALSE);
	}

	return;
}

//开始时间
void CScoreView::SetStartTimes(bool bStart)
{
	//设置变量
	//ASSERT(bStart);
	m_bStart = bStart;

	if(!bStart)
	{
		m_bTimes = TIMES;
		KillTimer(DLG_IDI_CLOSE);
	}

	return;
}

//显示时间
void CScoreView::SetShowTimes()
{
	SetTimer(DLG_IDI_CLOSE,1000,NULL);

	return;
}

void CScoreView::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//关闭倒计时
	if(DLG_IDI_CLOSE==nIDEvent)
	{
		m_bTimes--;
		if(m_bTimes<=0)
		{
			m_bTimes = TIMES;
			KillTimer(DLG_IDI_CLOSE);
			this->ShowWindow(SW_HIDE);
			if(m_bStart)
			{
				m_bStart=false;
				AfxGetMainWnd()->PostMessage(IDM_START_TIMES,0,0);			
			}
		}
		else 
		{
			Invalidate(FALSE);
			UpdateWindow();
		}
	}

	return;
	//CDialog::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////