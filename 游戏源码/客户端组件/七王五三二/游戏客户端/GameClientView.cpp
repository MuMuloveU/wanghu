#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识

#define IDC_START						100								//开始按钮
#define IDC_OUT_CARD					101								//出牌按钮
#define IDC_PASS_CARD					102								//PASS按钮
#define IDC_OUT_PROMPT					103								//提示按钮
#define IDC_BAIPAI                      104                             //摆牌按钮
#define IDC_TOUXIANG                    105                             //投降按钮
#define IDC_ABNEGATE                    106                             //放弃按钮
#define IDC_JIU							107								//揪人按钮
#define IDC_DISPLAYTEN                  108                             //亮十

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_PASS_CARD, OnPassCard)
	ON_BN_CLICKED(IDC_OUT_PROMPT, OnOutPrompt)
	ON_BN_CLICKED(IDC_BAIPAI,OnBaiPai)
	ON_BN_CLICKED(IDC_TOUXIANG,OnTouXiang)
	ON_BN_CLICKED(IDC_ABNEGATE,OnAbnegate)
	ON_BN_CLICKED(IDC_JIU,OnJiu)
	ON_BN_CLICKED(IDC_DISPLAYTEN,OnDisplayTen)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//配置变量
	m_bDeasilOrder=true;

	//状态变量
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bMost,0,sizeof(m_bMost));
	memset(m_wBanker,0,sizeof(m_wBanker));
	memset(m_bCardCount,0,sizeof(m_bCardCount));

	
	//历史积分
	memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
	memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageBackGround.SetLoadInfo(IDB_BACK_GROUND,hInstance);
	m_ImageLc.SetLoadInfo(IDB_LC,hInstance);
	m_ImageRc.SetLoadInfo(IDB_RC,hInstance);
	
	m_ImageHeadBg.SetLoadInfo(IDB_HEAD_BG,hInstance);
	m_ImageUserPass.SetLoadInfo(IDB_USER_PASS,hInstance);
	m_RedT_Ten.SetLoadInfo(IDB_TAO_TEN,hInstance);
	m_Fang_Ten.SetLoadInfo(IDB_FANG_TEN,hInstance);
	//设置扑克
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_ScoreCardControl[i].SetCardImage(IDB_SCORE_CARD,true);
		m_ScoreCardControl[i].SetCardSpace(13,26,0);
	}
	return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建控件
	CRect CreateRect(0,0,0,0);
	m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,CreateRect,this,9);

	//创建扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_UserCardControl[i].SetDirection(true);
		m_UserCardControl[i].SetDisplayFlag(true);
		m_UserCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,20+i);
		m_ScoreCardControl[i].SetDirection(true);
		m_ScoreCardControl[i].SetDisplayFlag(true);
		m_ScoreCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,41+i);
	}
	for (WORD i=0;i<GAME_PLAYER-1;i++)
	{
		m_LeaveCardControl[i].SetDisplayFlag(true);
		m_LeaveCardControl[i].SetCardSpace(20,18,0);
		m_LeaveCardControl[i].SetDirection(false);
		m_LeaveCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,30+i);
	}
	m_HandCardControl.SetSinkWindow(AfxGetMainWnd());
	m_HandCardControl.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,40);

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_START);
	m_btOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OUT_CARD);
	m_btPassCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_PASS_CARD);
	m_btTouXiang.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_TOUXIANG);
	m_btBaiPai.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_BAIPAI);
	m_btAbnegate.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_ABNEGATE);
	//m_btOutPrompt.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OUT_PROMPT);
	m_btJiu.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_JIU);
	m_btDisplayTen.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_DISPLAYTEN);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_START,hInstance,false);
	m_btOutCard.SetButtonImage(IDB_OUT_CARD,hInstance,false);
	m_btPassCard.SetButtonImage(IDB_PASS,hInstance,false);
	//m_btOutPrompt.SetButtonImage(IDB_OUT_PROMPT,hInstance,false);
	m_btTouXiang.SetButtonImage(IDB_TOUXIANG,hInstance,false);
	m_btBaiPai.SetButtonImage(IDB_BAIPAI,hInstance,false);
	m_btAbnegate.SetButtonImage(IDB_ABNEGATE,hInstance,false);
	m_btJiu.SetButtonImage(IDB_JIU,hInstance,false);
	m_btDisplayTen.SetButtonImage(IDB_DISPLAYTEN,hInstance,false);

	//读取配置
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;

	return 0;
}

//鼠标消息
void CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_OUT_CARD,1,1);

	return;
}

//鼠标双击
void CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);

	//设置扑克
	m_btOutCard.EnableWindow(FALSE);
	m_HandCardControl.ShootAllCard(false);

	return;
}

//重置界面
void CGameClientView::ResetGameView()
{
	//状态变量
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bMost,0,sizeof(m_bMost));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_wBanker,0,sizeof(m_wBanker));


	//历史积分
	memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
	memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));

	//隐藏控件
	m_ScoreView.ShowWindow(SW_HIDE);

	//隐藏按钮
	m_btStart.ShowWindow(SW_HIDE);
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btPassCard.ShowWindow(SW_HIDE);
	m_btBaiPai.ShowWindow(SW_HIDE);
	m_btAbnegate.ShowWindow(SW_HIDE);
	m_btTouXiang.ShowWindow(SW_HIDE);
	m_btJiu.ShowWindow(SW_HIDE);
	m_btDisplayTen.ShowWindow(SW_HIDE);
	//m_btOutPrompt.ShowWindow(SW_HIDE);

	//禁用控件
	m_btOutCard.EnableWindow(FALSE);
	m_btPassCard.EnableWindow(FALSE);
	//m_btOutPrompt.EnableWindow(FALSE);

	//扑克控件
	m_HandCardControl.SetCardData(NULL,0);
	m_HandCardControl.SetPositively(false);
	m_HandCardControl.SetDisplayFlag(false);
	for(WORD i=0;i<GAME_PLAYER-1;i++)m_LeaveCardControl[i].SetCardData(NULL,0);
	for (WORD i=0;i<GAME_PLAYER;i++) m_UserCardControl[i].SetCardData(NULL,0);

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//设置坐标


	m_ptFace[0].x=nWidth/5-m_nXFace*3;//左上
	m_ptFace[0].y=nHeight/5+5;
	m_ptName[0].x=m_ptFace[0].x+m_nXFace/2;
	m_ptName[0].y=m_ptFace[0].y+m_nYFace;
	m_ptTimer[0].x=m_ptFace[0].x;
	m_ptTimer[0].y=m_ptFace[0].y-m_nYFace;
	m_ptReady[0].x=m_ptFace[0].x+3*m_nXFace/2;
	m_ptReady[0].y=m_ptFace[0].y+m_nYFace/2;
	m_PointUserPass[0].x=m_ptFace[0].x;
	m_PointUserPass[0].y=m_ptFace[0].y-2*m_nYFace;

	m_ptFace[1].x=nWidth/5-m_nXFace*3;//左下
	m_ptFace[1].y=3*nHeight/5+5;
	m_ptName[1].x=m_ptFace[1].x+m_nXFace/2;
	m_ptName[1].y=m_ptFace[1].y+m_nYFace;
	m_ptTimer[1].x=m_ptFace[1].x;
	m_ptTimer[1].y=m_ptFace[1].y-m_nYFace;
	m_ptReady[1].x=m_ptFace[1].x+3*m_nXFace/2;
	m_ptReady[1].y=m_ptFace[1].y+m_nYFace/2;
	m_PointUserPass[1].x=m_ptFace[1].x;
	m_PointUserPass[1].y=m_ptFace[1].y-2*m_nYFace;

	m_ptFace[2].x=nWidth/2-m_nXFace/2;//自己
	m_ptFace[2].y=nHeight-90;
	m_ptName[2].x=nWidth/2+5+m_nXFace/2;
	m_ptName[2].y=nHeight-80;
	m_ptTimer[2].x=nWidth/2-m_nXFace/2-m_nXTimer*2-5;
	m_ptTimer[2].y=nHeight-90;
	m_ptReady[2].x=nWidth/2-m_nXTimer-m_nXFace/2-5;
	m_ptReady[2].y=nHeight-80;
	m_PointUserPass[2].x=m_ptFace[2].x-2*m_nXFace;
	m_PointUserPass[2].y=m_ptFace[2].y;

	m_ptFace[3].x=4*nWidth/5+m_nXFace*3;//右下
	m_ptFace[3].y=3*nHeight/5+5;
	m_ptName[3].x=m_ptFace[3].x+m_nXFace/2;
	m_ptName[3].y=m_ptFace[3].y+m_nYFace;
	m_ptTimer[3].x=m_ptFace[3].x;
	m_ptTimer[3].y=m_ptFace[3].y-m_nYFace;
	m_ptReady[3].x=m_ptFace[3].x-m_nXFace/2;
	m_ptReady[3].y=m_ptFace[3].y+m_nYFace/2;
	m_PointUserPass[3].x=m_ptFace[3].x;
	m_PointUserPass[3].y=m_ptFace[3].y-2*m_nYFace;

	m_ptFace[4].x=4*nWidth/5+m_nXFace*3;//右上
	m_ptFace[4].y=nHeight/5+5;
	m_ptName[4].x=m_ptFace[4].x+m_nXFace/2;
	m_ptName[4].y=m_ptFace[4].y+m_nYFace;
	m_ptTimer[4].x=m_ptFace[4].x;
	m_ptTimer[4].y=m_ptFace[4].y-m_nYFace;
	m_ptReady[4].x=m_ptFace[4].x-m_nXFace/2;
	m_ptReady[4].y=m_ptFace[4].y+m_nYFace/2;
	m_PointUserPass[4].x=m_ptFace[4].x;
	m_PointUserPass[4].y=m_ptFace[4].y-2*m_nYFace;

	

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(11);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nHeight-220-m_nYBorder,0,0,uFlags);

	m_btOutCard.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcButton.Width(),nHeight-240-m_nYBorder,0,0,uFlags);
	m_btTouXiang.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btTouXiang,NULL,nWidth/2-rcButton.Width()*3/2+30-60,nHeight-240-m_nYBorder,0,0,uFlags);
	m_btBaiPai.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btBaiPai,NULL,nWidth/2+rcButton.Width()/2-30-30-100,nHeight-240-m_nYBorder,0,0,uFlags);
	m_btAbnegate.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btAbnegate,NULL,nWidth/2+rcButton.Width()/2+3*m_nXFace,nHeight-220-m_nYBorder+4*m_nYFace,0,0,uFlags);
	DeferWindowPos(hDwp,m_btJiu,NULL,nWidth/2+rcButton.Width()/2-30-30-100,nHeight-240-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btDisplayTen,NULL,nWidth/2+rcButton.Width()/2+3*m_nXFace,nHeight-m_nYBorder-300+4*m_nYFace,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth/2-rcButton.Width()/2,nHeight-193-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2+10,nHeight-240-m_nYBorder,0,0,uFlags);
	EndDeferWindowPos(hDwp);

	//移动扑克
	m_HandCardControl.SetBenchmarkPos(nWidth/2,nHeight-100,enXCenter,enYBottom);
	

	//用户扑克
	m_UserCardControl[0].SetBenchmarkPos(m_ptFace[0].x+3*m_nXFace,m_ptFace[0].y,enXCenter,enYCenter);
	m_UserCardControl[1].SetBenchmarkPos(m_ptFace[1].x+3*m_nXFace,m_ptFace[1].y,enXCenter,enYCenter);
	m_UserCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-250-m_nYBorder,enXCenter,enYBottom);
	m_UserCardControl[3].SetBenchmarkPos(m_ptFace[3].x-2*m_nXFace,m_ptFace[3].y,enXCenter,enYCenter);
	m_UserCardControl[4].SetBenchmarkPos(m_ptFace[4].x-2*m_nXFace,m_ptFace[4].y,enXCenter,enYCenter);
	//积分扑克
	m_ScoreCardControl[0].SetBenchmarkPos(nWidth/2+326,nHeight-198-m_nYBorder,enXRight,enYCenter);
	m_ScoreCardControl[1].SetBenchmarkPos(nWidth/2+326,nHeight-198-m_nYBorder+26,enXRight,enYCenter);
	m_ScoreCardControl[2].SetBenchmarkPos(nWidth/2+326,nHeight-198-m_nYBorder+2*26,enXRight,enYCenter);
	m_ScoreCardControl[3].SetBenchmarkPos(nWidth/2+326,nHeight-198-m_nYBorder+3*26,enXRight,enYCenter);
	m_ScoreCardControl[4].SetBenchmarkPos(nWidth/2+326,nHeight-198-m_nYBorder+4*26,enXRight,enYCenter);

	
	
	m_LeaveCardControl[0].SetBenchmarkPos(m_ptFace[0].x+3*m_nXFace,m_ptFace[0].y,enXCenter,enYCenter);
	m_LeaveCardControl[1].SetBenchmarkPos(m_ptFace[1].x+3*m_nXFace,m_ptFace[1].y,enXCenter,enYCenter);
	m_LeaveCardControl[2].SetBenchmarkPos(m_ptFace[3].x-2*m_nXFace,m_ptFace[3].y,enXCenter,enYCenter);
	m_LeaveCardControl[3].SetBenchmarkPos(m_ptFace[4].x-2*m_nXFace,m_ptFace[4].y,enXCenter,enYCenter);

	//积分视图
	CRect rcControl;
	m_ScoreView.GetWindowRect(&rcControl);
	m_ScoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,nHeight/2-160,0,0,SWP_NOZORDER|SWP_NOSIZE);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBackGround,enMode_Spread);
	DrawViewImage(pDC,m_ImageBack,enMode_Centent);
	CImageHandle HandleLc(&m_ImageLc);
	CImageHandle HandleRc(&m_ImageRc);
	
	CImageHandle HandleHeadBg(&m_ImageHeadBg);

	
	m_ImageLc.AlphaDrawImage(pDC,m_nXBorder,nHeight-m_ImageLc.GetHeight()-m_nYBorder,RGB(255,0,255));
	m_ImageRc.AlphaDrawImage(pDC,nWidth-m_ImageRc.GetWidth()-m_nXBorder,m_nYBorder,RGB(255,0,255));



	//绘画用户
	TCHAR szBuffer[32];
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer=GetUserTimer(i);
		const tagUserData * pUserData=GetUserInfo(i);
		//绘画用户
		if (pUserData!=NULL)
		{
			//m_ImageHeadBg.BitBlt(pDC->m_hDC,m_ptFace[i].x-2,m_ptFace[i].y-2);
       
			//用户名字
			pDC->SetTextColor((wUserTimer>0)?RGB(250,250,250):RGB(220,220,220));
			pDC->SetTextAlign(i==2?TA_TOP:TA_CENTER);
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName,lstrlen(pUserData->szName));

			//剩余扑克
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("（%ld 张）"),m_bCardCount[i]);
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y+18,szBuffer,lstrlen(szBuffer));

			//其他信息
			if (wUserTimer!=0) DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
		}
	}



	//放弃标志
	CImageHandle ImageHnadleUserPass(&m_ImageUserPass);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bPass[i]==true) 
		{
			m_ImageUserPass.AlphaDrawImage(pDC,m_PointUserPass[i].x-3*m_nXBorder,m_PointUserPass[i].y,RGB(255,0,255));
		}
	}
	////最大标志
	//for (WORD i=0;i<GAME_PLAYER;i++)
	//{
	//	//描述信息
	//	if (m_bMost[i])
	//	{
	//		//创建资源
	//		CFont InfoFont;
	//		InfoFont.CreateFont(-36,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));

	//		//设置 DC
	//		pDC->SetTextColor(RGB(201,229,133));
	//		pDC->SetTextAlign(TA_CENTER|TA_TOP);
	//		CFont * pOldFont=pDC->SelectObject(&InfoFont);

	//		//绘画信息
	//		pDC->TextOut(m_PointUserPass[i].x-2*m_nXBorder,m_PointUserPass[i].y-m_nYBorder,"最大");

	//		//清理资源
	//		pDC->SelectObject(pOldFont);
	//		InfoFont.DeleteObject();
	//	}
	//}
	////最大标志
	//for (WORD i=0;i<GAME_PLAYER;i++)
	//{
	//	//描述信息
	//	if (m_bMost[i])
	//	{
	//		//创建资源
	//		CFont InfoFont;
	//		InfoFont.CreateFont(-36,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));

	//		//设置 DC
	//		pDC->SetTextColor(RGB(201,229,133));
	//		pDC->SetTextAlign(TA_CENTER|TA_TOP);
	//		CFont * pOldFont=pDC->SelectObject(&InfoFont);

	//		//绘画信息
	//		pDC->TextOut(m_PointUserPass[i].x-3*m_nXBorder,m_PointUserPass[i].y,"庄家");

	//		//清理资源
	//		pDC->SelectObject(pOldFont);
	//		InfoFont.DeleteObject();
	//	}
	//}



	return;
}

//用户顺序
void CGameClientView::SetUserOrder(bool bDeasilOrder)
{
	//状态判断
	if (m_bDeasilOrder==bDeasilOrder) return;

	//设置变量
	m_bDeasilOrder=bDeasilOrder;
	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);

	//设置界面
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyGameView(rcClient.Width(),rcClient.Height());

	//刷新界面
	UpdateGameView(NULL);

	return;
}

//设置放弃
void CGameClientView::SetPassFlag(WORD wChairID, bool bPass)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bPass[i]=bPass;
	}
	else m_bPass[wChairID]=bPass;

	//更新界面
	UpdateGameView(NULL);

	return;
}
//设置最大标志
void CGameClientView::SetMostFlag(WORD wChairID,bool bMost)
{
		//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bMost[i]=bMost;
	}
	else m_bMost[wChairID]=bMost;

	//更新界面
	UpdateGameView(NULL);

	return;

}
//设置庄家标志
void CGameClientView::SetBankerFlag(WORD wChairID,bool wBanker)
{
		//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_wBanker[i]=wBanker;
	}
	else m_wBanker[wChairID]=wBanker;

	//更新界面
	UpdateGameView(NULL);

	return;


}
//一局一清理
void CGameClientView::ClearOneTime()
{
	//状态变量
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bMost,0,sizeof(m_bMost));
	memset(m_wBanker,0,sizeof(m_wBanker));
	for(BYTE i=0;i<GAME_PLAYER;i++)
		m_ScoreCardControl[i].SetCardData(NULL,0);

}

//扑克数目
void CGameClientView::SetCardCount(WORD wChairID, BYTE bCardCount)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bCardCount[i]=bCardCount;
	}
	else m_bCardCount[wChairID]=bCardCount;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//开始按钮
void CGameClientView::OnStart()
{
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	return;
}
//摆牌按钮
void CGameClientView::OnBaiPai()
{
	//控件隐藏
	m_btAbnegate.ShowWindow(false);
	m_btBaiPai.ShowWindow(false);
	m_btTouXiang.ShowWindow(false);

	AfxGetMainWnd()->SendMessage(IDM_BAIPAI,0,1);
	return;
}
void CGameClientView::OnAbnegate()
{
	//控件隐藏
	m_btAbnegate.ShowWindow(false);
	m_btBaiPai.ShowWindow(false);
	m_btTouXiang.ShowWindow(false);
	AfxGetMainWnd()->SendMessage(IDM_ABNEGATE,0,1);
	return;
}
void CGameClientView::OnJiu()
{
	//控件隐藏
	m_btJiu.ShowWindow(SW_HIDE);
	m_btAbnegate.ShowWindow(SW_HIDE);
	AfxGetMainWnd()->SendMessage(IDM_JIU,0,2);

}
void CGameClientView::OnDisplayTen()
{
	m_btDisplayTen.ShowWindow(SW_HIDE);
	m_btAbnegate.ShowWindow(SW_HIDE);
	AfxGetMainWnd()->SendMessage(IDM_DISPLAYTEN,0,3);
}
//投降按钮
void CGameClientView::OnTouXiang()
{
	//控件隐藏
	m_btAbnegate.ShowWindow(false);
	m_btBaiPai.ShowWindow(false);
	m_btTouXiang.ShowWindow(false);
	AfxGetMainWnd()->SendMessage(IDM_TOUXIANG,0,2);
}

//出牌按钮
void CGameClientView::OnOutCard()
{
	AfxGetMainWnd()->SendMessage(IDM_OUT_CARD,1,1);
	return;
}

//放弃按钮
void CGameClientView::OnPassCard()
{
	AfxGetMainWnd()->SendMessage(IDM_PASS_CARD,1,1);
	return;
}

//提示按钮
void CGameClientView::OnOutPrompt()
{
	AfxGetMainWnd()->SendMessage(IDM_OUT_PROMPT,0,0);
	return;
}

//LRESULT CALLBACK BProc(HWND	hwnd,UINT message,WPARAM wParam,LPARAM lParam)
//{
//	if(message==WM_RBUTTONDOWN)
//		MessageBox(NULL,"adsf",NULL,NULL);
//	return CallWindowProc(OldProc,hwnd,message,wParam,lParam);
//}



//////////////////////////////////////////////////////////////////////////

