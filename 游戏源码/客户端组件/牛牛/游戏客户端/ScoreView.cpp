#include "StdAfx.h"
#include "ScoreView.h"
#include ".\scoreview.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView()
{
	//设置数据
	ZeroMemory(m_lTax,sizeof(m_lTax));
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

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	CImageHandle ImageHandle(&m_ImageBack);
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

	//加载资源
	CImageHandle HandleBack(&m_ImageBack);

	//绘画背景
	m_ImageBack.AlphaDrawImage(&dc,0,0,RGB(255,0,255));

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(250,250,250));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//提示信息
	TCHAR szBuffer[128]=TEXT("");

	//显示分数
	CImageHandle HandleWinLose(&m_ImageWinLose);
	INT nWinLoseWidth = m_ImageWinLose.GetWidth()/3;
	INT nWinLostHeight = m_ImageWinLose.GetHeight();
	WORD wCount = 0;
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		if (m_szUserName[i][0]!=0)
		{
			//输出名字
			INT nScPos = 80;
			CRect rcName(26,wCount*29+nScPos,126,(wCount+1)*29+nScPos);
			dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcName,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//输出成绩
			CRect rcScore(95,wCount*29+nScPos,195,(wCount+1)*29+nScPos);
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lScore[i]);
			dc.DrawText(szBuffer,lstrlen(szBuffer),&rcScore,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//胜负标志
			m_ImageWinLose.AlphaDrawImage(&dc,218-nWinLoseWidth/2,wCount*29+nScPos+13-nWinLostHeight/2,nWinLoseWidth,nWinLostHeight,
				(m_lScore[i]>0)?0:((m_lScore[i]<0)?nWinLoseWidth:nWinLoseWidth*2),0,RGB(255,0,255));

			//输出税收
			CRect rcTax(235,wCount*29+nScPos,335,(wCount+1)*29+nScPos);
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lTax[i]);
			dc.DrawText(szBuffer,lstrlen(szBuffer),&rcTax,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
			
			wCount++;
		}
	}

	return;
}

//设置积分
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONG lScore)
{
	if (wChairID<CountArray(m_lScore))
	{
		m_lScore[wChairID]=lScore;
		lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));
		Invalidate(FALSE);
	}
	return;
}

//设置税收
void CScoreView::SetGameTax(LONG lTax,WORD wChairID)
{
	m_lTax[wChairID]=lTax;

	return;
}

//清理数据
void CScoreView::ResetScore()
{
	//设置数据
	ZeroMemory(m_lTax,sizeof(m_lTax));
	ZeroMemory(m_lScore,sizeof(m_lScore));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));

	return;
}

//////////////////////////////////////////////////////////////////////////

BOOL CScoreView::OnEraseBkgnd(CDC* pDC)
{
	//
	CRect rt;
	GetClientRect(&rt);
	CRgn WndRgn;
	WndRgn.CreateRoundRectRgn(rt.left,rt.top,rt.right+1,rt.bottom+1,10,10);
	SetWindowRgn(WndRgn,TRUE);
	WndRgn.DeleteObject();
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

void CScoreView::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);

	InvalidateRect(NULL,FALSE);
}
