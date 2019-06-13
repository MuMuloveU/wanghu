#include "StdAfx.h"
#include "ScoreView.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView(void)
{
	//设置数据
	m_lGameTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//加载资源
	m_ImageBack.SetLoadInfo(IDB_Game_End,AfxGetInstanceHandle());
}

//析构函数
CScoreView::~CScoreView(void)
{
}

//建立消息
int CScoreView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置数据
	m_lGameTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	CImageHandle ImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	return TRUE;
}

//重画函数
void CScoreView::OnPaint() 
{
	CPaintDC dc(this); 

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,0,0));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画背景
	CImageHandle ImageHandle(&m_ImageBack);
	m_ImageBack.BitBlt(dc,0,0);

	//显示分数
	CRect rcDraw;
	CString strScore ;
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		if(lstrcmp(m_szUserName[i],""))
		{
			//用户名字
			rcDraw.left=10;
			rcDraw.right=130;
			rcDraw.top=i*23+80;
			rcDraw.bottom=rcDraw.top+12;
			dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS|DT_NOCLIP);

			//用户积分
			rcDraw.left=150;
			rcDraw.right=290;
			strScore.Format("%ld" , m_lGameScore[i]) ;
			//_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lGameScore[i]);
			dc.DrawText(strScore,strScore.GetLength(),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS|DT_NOCLIP);
		}
	}

/*
	if(1)
	{
		//显示税收
		rcDraw.left=10;
		rcDraw.right=130;
		rcDraw.top=4*23+80;
		rcDraw.bottom=rcDraw.top+12;
		LPCTSTR pszTax=TEXT("交易税");
		dc.DrawText(pszTax,lstrlen(pszTax),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS|DT_NOCLIP);

		//显示税收
		rcDraw.left=150;
		rcDraw.right=290;
		_snprintf(szBuffer,sizeof(szBuffer),"%ld",m_lGameTax);
		dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS|DT_NOCLIP);
	}*/

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
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//绘画视图
	Invalidate(NULL);
	
	return;
}

//设置税收
void CScoreView::SetGameTax(LONG lGameTax)
{
	//设置变量

	m_lGameTax=lGameTax;
	Invalidate(FALSE);


	return;
}

//设置积分
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONG lScore)
{
	//设置变量
	if (wChairID<CountArray(m_lGameScore))
	{
		m_lGameScore[wChairID]=lScore;
		lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));
		Invalidate(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
