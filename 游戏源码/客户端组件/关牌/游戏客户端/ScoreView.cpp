#include "StdAfx.h"
#include "ScoreView.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView(void)
{
	//设置数据
	m_lTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//加载资源
	m_ImageBack.SetLoadInfo(IDB_GAME_END,AfxGetInstanceHandle());
	m_ImageWinLose.SetLoadInfo(IDB_WIN_LOSE,AfxGetInstanceHandle());
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
	m_lTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	CImageHandle ImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	return 0;
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
	CImageHandle ImageHandle(&m_ImageBack);
	m_ImageBack.BitBlt(dc,0,0);

	//设置区域
	CRect rt;
	GetClientRect(&rt);
	CRgn WndRgn;
	WndRgn.CreateRoundRectRgn(rt.left,rt.top,rt.right+1,rt.bottom+1,10,10);
	SetWindowRgn(WndRgn,TRUE);
	WndRgn.DeleteObject();

	//显示分数
	CRect rcDraw;
	TCHAR szBuffer[64]=TEXT("");
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		//用户名字
		rcDraw.left=30;
		rcDraw.right=114;
		rcDraw.top=i*30+85;
		rcDraw.bottom=rcDraw.top+12;
		dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		//用户积分
		rcDraw.left=120;
		rcDraw.right=190;
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lGameScore[i]);
		dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		CImageHandle HandleWinLose(&m_ImageWinLose);
		INT nWinLoseWidth = m_ImageWinLose.GetWidth()/3;
		INT nWinLostHeight = m_ImageWinLose.GetHeight();
		//胜负标志
		BYTE index=0;
		if(m_lGameScore[i]>0) index=0;
		if(m_lGameScore[i]==0) index=2;
		if(m_lGameScore[i]<0) index=1;
		m_ImageWinLose.AlphaDrawImage(&dc,242-nWinLoseWidth/3,i*29+90-nWinLostHeight/3,nWinLoseWidth,nWinLostHeight,
				index*nWinLoseWidth,0,RGB(255,0,255));
	}

	return;
}

//重置积分
void CScoreView::ResetScore()
{
	//设置数据
	m_lTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//绘画视图
	Invalidate(NULL);
	
	return;
}

//设置税收
void CScoreView::SetTax(LONG lTax)
{
	if (m_lTax!=lTax)
	{
		m_lTax=lTax;
		Invalidate(NULL);
	}
	return;
}

//设置积分
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONG lScore)
{
	if (wChairID<CountArray(m_lGameScore))
	{
		m_lGameScore[wChairID]=lScore;
		lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));
		Invalidate(FALSE);
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
