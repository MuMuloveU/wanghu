#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识 

#define IDC_START						100								//开始按钮
#define IDC_ONE_SCORE					101								//1 分按钮
#define IDC_TWO_SCORE					102								//2 分按钮
#define IDC_THREE_SCORE					103								//3 分按钮
#define IDC_GIVE_UP_SCORE				104								//放弃按钮
#define IDC_OUT_CARD					105								//出牌按钮
#define IDC_PASS_CARD					106								//PASS按钮

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_PASS_CARD, OnPassCard)
	ON_BN_CLICKED(IDC_ONE_SCORE, OnOneScore)
	ON_BN_CLICKED(IDC_TWO_SCORE, OnTwoScore)
	ON_BN_CLICKED(IDC_THREE_SCORE, OnThreeScore)
	ON_BN_CLICKED(IDC_GIVE_UP_SCORE, OnGiveUpScore)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//游戏变量
	m_lBaseScore=0;
	m_bCavernScore=0;
	m_wCavernUser=INVALID_CHAIR;

	//配置变量
	m_bDeasilOrder=true;

	//状态变量
	m_bCavernTitle=false;
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bScore,0,sizeof(m_bScore));
	memset(m_bCardCount,0,sizeof(m_bCardCount));

	//位置信息
	memset(m_ptScore,0,sizeof(m_ptScore));
	memset(m_ptCavern,0,sizeof(m_ptCavern));

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageScore.SetLoadInfo(IDB_SCORE,hInstance);
	m_ImageCavern.SetLoadInfo(IDB_CAVERN,hInstance);
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageBackGround.SetLoadInfo(IDB_BACK_GROUND,hInstance);

	//计算位置
	CImageHandle ImageHandleCavern(&m_ImageCavern);
	m_CavernSize.cx=m_ImageCavern.GetWidth();
	m_CavernSize.cy=m_ImageCavern.GetHeight();

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
	m_ScoreView.Create(IDD_GAME_SCORE,this);

	//创建扑克
	for (WORD i=0;i<3;i++)
	{
		m_UserCardControl[i].SetDirection(true);
		m_UserCardControl[i].SetDisplayFlag(true);
		m_UserCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,20+i);
	}
	for (WORD i=0;i<2;i++) 
	{
		m_LeaveCardControl[i].SetDirection(false);
		m_LeaveCardControl[i].SetDisplayFlag(true);
		m_LeaveCardControl[i].SetCardSpace(0,18,0);
		m_LeaveCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,30+i);
	}
	m_BackCardControl.SetCardSpace(25,0,0);
	m_BackCardControl.SetDisplayFlag(true);
	m_HandCardControl.SetSinkWindow(AfxGetMainWnd());
	m_HandCardControl.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,40);
	m_BackCardControl.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,41);

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_START);
	m_btOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OUT_CARD);
	m_btPassCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_PASS_CARD);
	m_btOneScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_ONE_SCORE);
	m_btTwoScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_TWO_SCORE);
	m_btThreeScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_THREE_SCORE);
	m_btGiveUpScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_GIVE_UP_SCORE);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_START,hInstance,false);
	m_btOutCard.SetButtonImage(IDB_OUT_CARD,hInstance,false);
	m_btPassCard.SetButtonImage(IDB_PASS,hInstance,false);
	m_btOneScore.SetButtonImage(IDB_ONE_SCORE,hInstance,false);
	m_btTwoScore.SetButtonImage(IDB_TWO_SCORE,hInstance,false);
	m_btThreeScore.SetButtonImage(IDB_THREE_SCORE,hInstance,false);
	m_btGiveUpScore.SetButtonImage(IDB_GIVE_UP,hInstance,false);

	//设置颜色
	/*m_btStart.SetTextColor(RGB(242,251,225));
	m_btOutCard.SetTextColor(RGB(242,251,225));
	m_btPassCard.SetTextColor(RGB(242,251,225));
	m_btOneScore.SetTextColor(RGB(242,251,225));
	m_btTwoScore.SetTextColor(RGB(242,251,225));
	m_btThreeScore.SetTextColor(RGB(242,251,225));
	m_btGiveUpScore.SetTextColor(RGB(242,251,225));*/

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
	//游戏变量
	m_lBaseScore=0;
	m_bCavernScore=0;
	m_wCavernUser=INVALID_CHAIR;

	//状态变量
	m_bCavernTitle=false;
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bScore,0,sizeof(m_bScore));
	memset(m_bCardCount,0,sizeof(m_bCardCount));

	//隐藏控件
	m_ScoreView.ShowWindow(SW_HIDE);

	//隐藏按钮
	m_btStart.ShowWindow(SW_HIDE);
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btPassCard.ShowWindow(SW_HIDE);
	m_btOneScore.ShowWindow(SW_HIDE);
	m_btTwoScore.ShowWindow(SW_HIDE);
	m_btThreeScore.ShowWindow(SW_HIDE);
	m_btGiveUpScore.ShowWindow(SW_HIDE);

	//禁用控件
	m_btOutCard.EnableWindow(FALSE);
	m_btPassCard.EnableWindow(FALSE);

	//扑克控件
	m_BackCardControl.SetCardData(NULL,0);
	m_HandCardControl.SetCardData(NULL,0);
	m_HandCardControl.SetPositively(false);
	m_HandCardControl.SetDisplayFlag(false);
	m_LeaveCardControl[0].SetCardData(NULL,0);
	m_LeaveCardControl[1].SetCardData(NULL,0);
	for (WORD i=0;i<GAME_PLAYER;i++) m_UserCardControl[i].SetCardData(NULL,0);

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//设置坐标
	if (m_bDeasilOrder==true)
	{
		m_ptFace[0].x=nWidth-m_nXBorder-m_nXFace-5;
		m_ptFace[0].y=nHeight/2-m_nYFace;
		m_ptName[0].x=nWidth-m_nXBorder-m_nXFace-10;
		m_ptName[0].y=nHeight/2-m_nYFace+3;
		m_ptTimer[0].x=nWidth-m_nXBorder-m_nXFace-5;
		m_ptTimer[0].y=nHeight/2-m_nYFace-m_nYTimer-5;
		m_ptReady[0].x=nWidth-m_nXBorder-m_nXTimer-5;
		m_ptReady[0].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
		m_ptScore[0].x=nWidth-m_nXBorder-m_nXFace-150;
		m_ptScore[0].y=nHeight/2-m_nYFace+5;
		m_ptCavern[0].x=nWidth-m_nXBorder-m_CavernSize.cx-3;
		m_ptCavern[0].y=nHeight/2+15;

		m_ptFace[2].x=m_nXBorder+25;
		m_ptFace[2].y=nHeight/2-m_nYFace;
		m_ptName[2].x=m_nXBorder+m_nXFace+30;
		m_ptName[2].y=nHeight/2-m_nYFace+3;
		m_ptTimer[2].x=m_nXBorder+25;
		m_ptTimer[2].y=nHeight/2-m_nYFace-5-m_nYTimer;
		m_ptReady[2].x=m_nXBorder+25+m_nXTimer;
		m_ptReady[2].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
		m_ptScore[2].x=m_nXBorder+m_nXFace+127;
		m_ptScore[2].y=nHeight/2-m_nYFace+5;
		m_ptCavern[2].x=m_nXBorder+23;
		m_ptCavern[2].y=nHeight/2+15;
	}
	else
	{
		m_ptFace[2].x=nWidth-m_nXBorder-m_nXFace-5;
		m_ptFace[2].y=nHeight/2-m_nYFace;
		m_ptName[2].x=nWidth-m_nXBorder-m_nXFace-10;
		m_ptName[2].y=nHeight/2-m_nYFace+3;
		m_ptTimer[2].x=nWidth-m_nXBorder-m_nXFace-5;
		m_ptTimer[2].y=nHeight/2-m_nYFace-m_nYTimer-5;
		m_ptReady[2].x=nWidth-m_nXBorder-m_nXTimer-5;
		m_ptReady[2].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
		m_ptScore[2].x=nWidth-m_nXBorder-m_nXFace-150;
		m_ptScore[2].y=nHeight/2-m_nYFace+5;
		m_ptCavern[2].x=nWidth-m_nXBorder-m_CavernSize.cx-3;
		m_ptCavern[2].y=nHeight/2+15;

		m_ptFace[0].x=m_nXBorder+25;
		m_ptFace[0].y=nHeight/2-m_nYFace;
		m_ptName[0].x=m_nXBorder+m_nXFace+30;
		m_ptName[0].y=nHeight/2-m_nYFace+3;
		m_ptTimer[0].x=m_nXBorder+25;
		m_ptTimer[0].y=nHeight/2-m_nYFace-5-m_nYTimer;
		m_ptReady[0].x=m_nXBorder+25+m_nXTimer;
		m_ptReady[0].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
		m_ptScore[0].x=m_nXBorder+m_nXFace+127;
		m_ptScore[0].y=nHeight/2-m_nYFace+5;
		m_ptCavern[0].x=m_nXBorder+23;
		m_ptCavern[0].y=nHeight/2+15;
	}

	m_ptFace[1].x=nWidth/2-m_nXFace/2;
	m_ptFace[1].y=nHeight-90;
	m_ptName[1].x=nWidth/2+5+m_nXFace/2;
	m_ptName[1].y=nHeight-80;
	m_ptTimer[1].x=nWidth/2-m_nXFace/2-m_nXTimer*2-5;
	m_ptTimer[1].y=nHeight-90;
	m_ptReady[1].x=nWidth/2-m_nXTimer-m_nXFace/2-5;
	m_ptReady[1].y=nHeight-80;
	m_ptScore[1].x=nWidth/2-21;
	m_ptScore[1].y=nHeight-m_nYBorder-m_nYFace-210;
	m_ptCavern[1].x=nWidth/2-m_nXFace/2-m_CavernSize.cx-m_nXTimer*2-10;
	m_ptCavern[1].y=nHeight-m_nXBorder-m_CavernSize.cy-35;

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(7);
	m_btStart.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nHeight-230-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcButton.Width()-10,nHeight-235-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2+10,nHeight-235-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOneScore,NULL,nWidth/2-rcButton.Width()*2-30,nHeight-235-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btTwoScore,NULL,nWidth/2-rcButton.Width()-10,nHeight-235-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btThreeScore,NULL,nWidth/2+10,nHeight-235-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUpScore,NULL,nWidth/2+30+rcButton.Width(),nHeight-235-m_nYBorder,0,0,uFlags);
	EndDeferWindowPos(hDwp);

	//移动扑克
	m_BackCardControl.SetBenchmarkPos(nWidth/2,m_nYBorder+18,enXCenter,enYTop);
	m_HandCardControl.SetBenchmarkPos(nWidth/2,nHeight-100,enXCenter,enYBottom);
	m_UserCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-240-m_nYBorder,enXCenter,enYBottom);

	//用户扑克
	if (m_bDeasilOrder==true)
	{
		m_UserCardControl[0].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-90,nHeight/2-30,enXRight,enYCenter);
		m_UserCardControl[2].SetBenchmarkPos(m_nXBorder+m_nXFace+90,nHeight/2-30,enXLeft,enYCenter);
		m_LeaveCardControl[0].SetBenchmarkPos(nWidth-m_nXBorder-m_nXFace-50,nHeight/2-50,enXRight,enYCenter);
		m_LeaveCardControl[1].SetBenchmarkPos(m_nXBorder+m_nXFace+50,nHeight/2-50,enXLeft,enYCenter);
	}
	else 
	{
		m_UserCardControl[2].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-90,nHeight/2-30,enXRight,enYCenter);
		m_UserCardControl[0].SetBenchmarkPos(m_nXBorder+m_nXFace+90,nHeight/2-30,enXLeft,enYCenter);
		m_LeaveCardControl[1].SetBenchmarkPos(nWidth-m_nXBorder-m_nXFace-50,nHeight/2-50,enXRight,enYCenter);
		m_LeaveCardControl[0].SetBenchmarkPos(m_nXBorder+m_nXFace+50,nHeight/2-50,enXLeft,enYCenter);
	}

	//积分视图
	CRect rcControl;
	m_ScoreView.GetWindowRect(&rcControl);
	m_ScoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,150,0,0,SWP_NOZORDER|SWP_NOSIZE);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	CImageHandle HandleBack(&m_ImageBack);
	CImageHandle HandleBackGroud(&m_ImageBackGround);
	DrawViewImage(pDC,m_ImageBackGround,enMode_Spread);
	//m_ImageBack.StretchBlt(pDC->m_hDC,(nWidth-m_ImageBack.GetWidth())/2,0);
//	DrawViewImage(pDC,m_ImageBackGround,enMode_Spread);
	DrawViewImage(pDC,m_ImageBack,enMode_Centent);

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
			//用户名字
			pDC->SetTextColor((wUserTimer>0)?RGB(255,204,0):RGB(128,0,0));
			pDC->SetTextAlign(((i==0)&&(m_bDeasilOrder==true))||((i==2)&&(m_bDeasilOrder==false))?TA_RIGHT:TA_LEFT);
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName,lstrlen(pUserData->szName));

			//剩余扑克
			if (pUserData->cbUserStatus>=US_PLAY)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("（%ld 张）"),m_bCardCount[i]);
				pDC->TextOut(m_ptName[i].x,m_ptName[i].y+18,szBuffer,lstrlen(szBuffer));
			}

			//其他信息
			if (wUserTimer!=0) DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
		}
	}

	//叫分标志
	CImageHandle ImageHnadleScore(&m_ImageScore);
	int nXImagePos,nImageWidth=m_ImageScore.GetWidth(),nImageHeight=m_ImageScore.GetHeight();
	for (WORD i=0;i<3;i++)
	{
		if ((m_bScore[i]!=0)||(m_bPass[i]==true))
		{
			if ((m_bPass[i]==true)||(m_bScore[i]==255)) nXImagePos=m_ImageScore.GetWidth()*3/4;
			else nXImagePos=(m_bScore[i]-1)*m_ImageScore.GetWidth()/4;
			m_ImageScore.AlphaDrawImage(pDC,m_ptScore[i].x,m_ptScore[i].y,nImageWidth/4,nImageHeight,nXImagePos,0,RGB(255,0,255));
		}
	}

	//庄家位置
	if (m_wCavernUser!=INVALID_CHAIR)
	{
		//获取用户
		const tagUserData * pUserData=GetUserInfo(m_wCavernUser);
		if (pUserData!=NULL)
		{
			//变量定义
			TCHAR szBuffer[64];
			CImageHandle ImageHandleCavern(&m_ImageCavern);

			//设置 DC
			pDC->SetTextColor(RGB(255,0,255));
			pDC->SetTextAlign(TA_LEFT|TA_TOP);

			//绘画信息
			m_ImageCavern.AlphaDrawImage(pDC,m_ptCavern[m_wCavernUser].x,m_ptCavern[m_wCavernUser].y-6,RGB(255,0,255));
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("坑主：%s"),pUserData->szName);
			pDC->TextOut(m_nXBorder+m_nXFace+30,m_nYBorder+18,szBuffer,lstrlen(szBuffer));
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("倍数：%d 倍"),m_bCavernScore);
			pDC->TextOut(m_nXBorder+m_nXFace+30,m_nYBorder+35,szBuffer,lstrlen(szBuffer));
			DrawUserFace(pDC,pUserData,m_nXBorder+25,m_nYBorder+15,false);
		}
	}

	//描述信息
	if (m_bCavernTitle==true)
	{
		//创建资源
		CFont InfoFont;
		InfoFont.CreateFont(-36,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));

		//设置 DC
		pDC->SetTextColor(RGB(128,0,0));
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		CFont * pOldFont=pDC->SelectObject(&InfoFont);

		//绘画信息
		pDC->TextOut(nWidth/2,200,"正等待用户挖坑");

		//清理资源
		pDC->SelectObject(pOldFont);
		InfoFont.DeleteObject();
	}

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

//基础分数
void CGameClientView::SetBaseScore(LONG lBaseScore)
{
	//设置变量
	m_lBaseScore=lBaseScore;

	//刷新界面
	UpdateGameView(NULL);

	return;
}

//显示提示
void CGameClientView::ShowCavernTitle(bool bCavernTitle)
{
	//设置变量
	m_bCavernTitle=bCavernTitle;

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

//挖坑分数
void CGameClientView::SetCavernUser(WORD wChairID, BYTE bCavernScore)
{
	//设置变量
	m_wCavernUser=wChairID;
	m_bCavernScore=bCavernScore;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//挖坑分数
void CGameClientView::SetCavernScore(WORD wChairID, BYTE bCavernScore)
{
	//设置变量
	if (wChairID!=INVALID_CHAIR) m_bScore[wChairID]=bCavernScore;
	else memset(m_bScore,0,sizeof(m_bScore));

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

//出牌按钮
void CGameClientView::OnOutCard()
{
	AfxGetMainWnd()->SendMessage(IDM_OUT_CARD,0,0);
	return;
}

//放弃按钮
void CGameClientView::OnPassCard()
{
	AfxGetMainWnd()->SendMessage(IDM_PASS_CARD,0,0);
	return;
}

//叫分按钮
void CGameClientView::OnOneScore()
{
	AfxGetMainWnd()->SendMessage(IDM_CAVERN_SCORE,1,1);
	return;
}

//叫分按钮
void CGameClientView::OnTwoScore()
{
	AfxGetMainWnd()->SendMessage(IDM_CAVERN_SCORE,2,2);
	return;
}

//叫分按钮
void CGameClientView::OnThreeScore()
{
	AfxGetMainWnd()->SendMessage(IDM_CAVERN_SCORE,3,3);
	return;
}

//放弃按钮
void CGameClientView::OnGiveUpScore()
{
	AfxGetMainWnd()->SendMessage(IDM_CAVERN_SCORE,255,255);
	return;
}

//////////////////////////////////////////////////////////////////////////

