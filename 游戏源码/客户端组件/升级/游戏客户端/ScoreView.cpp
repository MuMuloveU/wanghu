#include "StdAfx.h"
#include "ScoreView.h"
#include ".\scoreview.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView()
{
	//结束信息
	m_wGameScore=0;
	m_wConcealTime=0;
	m_wConcealScore=0;
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//设置数据
	ZeroMemory(m_lScore,sizeof(m_lScore));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));

	//加载资源
	m_ImageCard.SetLoadInfo(IDB_CARD,AfxGetInstanceHandle());
	m_ImageBack.SetLoadInfo(IDB_SCORE_BACK,AfxGetInstanceHandle());
	m_ImageWinLose.SetLoadInfo(IDB_WIN_LOSE,AfxGetInstanceHandle());

	//获取大小
	CImageHandle CardImageHandle(&m_ImageCard);
	m_CardSize.cx=m_ImageCard.GetWidth()/13;
	m_CardSize.cy=m_ImageCard.GetHeight()/5;

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
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),/*SWP_NOMOVE|*/SWP_NOZORDER);

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
	CImageHandle HandleCard(&m_ImageCard);

	//设置区域
	CRect rt;
	GetClientRect(&rt);
	CRgn WndRgn;
	WndRgn.CreateRoundRectRgn(rt.left,rt.top,rt.right+1,rt.bottom+1,10,10);
	SetWindowRgn(WndRgn,TRUE);
	WndRgn.DeleteObject();

	//绘画背景
	m_ImageBack.AlphaDrawImage(&dc,0,0,RGB(255,0,255));
	//m_ImageBack.BlendDrawImage(&dc,0,0,RGB(255,0,255),64);

	//绘画扑克
	if (m_cbConcealCount>0)
	{
		//获取位置
		CRect rcClient;
		GetClientRect(&rcClient);

		//计算大小
		CSize ControlSize;
		ControlSize.cy=m_CardSize.cy;
		ControlSize.cx=m_CardSize.cx+(m_cbConcealCount-1)*18;

		//定义位置
		int nYPos=48;
		int nXPos=(rcClient.Width()-ControlSize.cx)/2;

		//绘画扑克
		for (BYTE i=0;i<m_cbConcealCount;i++)
		{
			//变量定义
			int nXImagePos=0,nYImagePos=0;

			//图片位置
			if (m_cbConcealCard[i]!=0)
			{
				if ((m_cbConcealCard[i]==0x4E)||(m_cbConcealCard[i]==0x4F))
				{
					nYImagePos=((m_cbConcealCard[i]&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
					nXImagePos=((m_cbConcealCard[i]&CARD_MASK_VALUE)%14)*m_CardSize.cx;
				}
				else
				{
					nXImagePos=((m_cbConcealCard[i]&CARD_MASK_VALUE)-1)*m_CardSize.cx;
					nYImagePos=((m_cbConcealCard[i]&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
				}
			}
			else
			{
				nXImagePos=m_CardSize.cx*2;
				nYImagePos=m_CardSize.cy*4;
			}

			//绘画扑克
			m_ImageCard.AlphaDrawImage(&dc,nXPos+i*18,nYPos,m_CardSize.cx,m_CardSize.cy,nXImagePos,nYImagePos,RGB(255,0,255));
		}
	}

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(250,250,250));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//提示信息
	TCHAR szBuffer[128]=TEXT("");
	_snprintf(szBuffer,CountArray(szBuffer),TEXT("游戏得分 %d + 底牌分 %d × %d 倍 = %d"),m_wGameScore,
		m_wConcealScore,m_wConcealTime,m_wGameScore+m_wConcealScore*m_wConcealTime);

	//绘画提示
	CRect rcString(25,166,307,184);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rcString,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	//显示分数
	CImageHandle HandleWinLose(&m_ImageWinLose);
	INT nWinLoseWidth = m_ImageWinLose.GetWidth()/3;
	INT nWinLostHeight = m_ImageWinLose.GetHeight();
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		if (m_szUserName[i][0]!=0)
		{
			//输出名字
			CRect rcName(26,i*29+225,126,(i+1)*29+225);
			dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcName,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//输出成绩
			CRect rcScore(127,i*29+225,216,(i+1)*29+225);
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lScore[i]);
			dc.DrawText(szBuffer,lstrlen(szBuffer),&rcScore,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//胜负标志
			BYTE byIndex = 0;
			if( m_lScore[i] == 0 ) byIndex = 2;
			else if( m_lScore[i] < 0 ) byIndex = 1;
			m_ImageWinLose.AlphaDrawImage(&dc,261-nWinLoseWidth/2,i*29+239-nWinLostHeight/2,nWinLoseWidth,nWinLostHeight,
				byIndex*nWinLoseWidth,0,RGB(255,0,255));
		}
	}

	return;
}

//设置信息
void CScoreView::SetScoreViewInfo(tagScoreViewInfo & ScoreViewInfo)
{
	//设置变量
	m_wGameScore=ScoreViewInfo.wGameScore;
	m_wConcealTime=ScoreViewInfo.wConcealTime;
	m_wConcealScore=ScoreViewInfo.wConcealScore;
	m_cbConcealCount=ScoreViewInfo.cbConcealCount;
	CopyMemory(m_cbConcealCard,ScoreViewInfo.cbConcealCard,m_cbConcealCount*sizeof(BYTE));

	//刷新界面
	if (m_hWnd!=NULL) Invalidate(FALSE);

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

//鼠标激活
int CScoreView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	//Invalidate(FALSE);
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

//////////////////////////////////////////////////////////////////////////



