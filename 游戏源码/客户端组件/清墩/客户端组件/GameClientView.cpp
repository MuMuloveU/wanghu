#include "StdAfx.h"
#include "..\服务器组件\GameLogic.h"
#include "GameClient.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////
//宏定义
#define ACCOUNTS_HEIGHT 32
//爆炸标识
#define IDI_BOMB_EFFECT					101								//爆炸标识

//爆炸数目
#define BOMB_EFFECT_COUNT				6								//爆炸数目

//////////////////////////////////////////////////////////////////////////
//按钮标识 

//控制按钮
#define IDC_START						100								//开始按钮
#define IDC_TRUSTEE						101								//托管控制
#define IDC_LAST_TURN					102								//上轮扑克
#define IDC_SORT_CARD_ORDER				103								//扑克排序
#define IDC_SORT_CARD_COLOR				104								//扑克排序
#define IDC_SORT_CARD_COUNT				105								//扑克排序
#define IDC_SORT_CARD					106								//扑克排序

//游戏按钮
#define IDC_OUT_CARD					120								//出牌按钮
#define IDC_PASS_CARD					121								//PASS按钮
#define IDC_OUT_PROMPT	                122                             //提示按钮
#define IDC_TRUSTEE_CONTROL				203								//托管控制
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)

	//系统消息
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()

	//控制按钮
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_TRUSTEE, OnBnClickedStustee)
	ON_BN_CLICKED(IDC_LAST_TURN, OnBnClickedLastTurn)
	
	ON_BN_CLICKED(IDC_SORT_CARD, OnBnClickedSort)
	ON_BN_CLICKED(IDC_SORT_CARD_COLOR, OnBnClickedSortCardColor)
	ON_BN_CLICKED(IDC_SORT_CARD_COUNT, OnBnClickedSortCardCount)
	ON_BN_CLICKED(IDC_SORT_CARD_ORDER, OnBnClickedSortCardOrder)
	
	//游戏按钮
	ON_BN_CLICKED(IDC_OUT_CARD, OnBnClickedOutCard)
	ON_BN_CLICKED(IDC_PASS_CARD, OnBnClickedPassCard)
	ON_BN_CLICKED(IDC_OUT_PROMPT, OnBnClickedOutPrompt)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//游戏变量
	m_lCellScore=0L;
	m_cbMainValue=0;
	ZeroMemory(m_cbValueOrder,sizeof(m_cbValueOrder));
	m_nLButtonUpCount=0;

	//得分变量
	m_TurnScore=0;
	ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));

	//用户状态
	ZeroMemory(m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(m_bUserContinue,sizeof(m_bUserContinue));

	//游戏状态
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));
	ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));

	//爆炸动画
	m_bBombEffect=false;
	m_cbBombFrameIndex=0;

	//移动变量
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_wHoverCardItem=INVALID_ITEM;
	m_wMouseDownItem=INVALID_ITEM;

	//状态变量
	m_bShowScore=false;
	m_bLastTurnCard=false;

	//加载资源
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageNumber.SetLoadInfo(IDB_NUMBER,hResInstance);
	m_ImageViewFill.SetLoadInfo(IDB_VIEW_FILL,hResInstance);
	m_ImageViewBack.SetLoadInfo(IDB_VIEW_BACK,hResInstance);
	m_ImageUserPass.SetLoadInfo(IDB_USER_PASS,hResInstance);
	m_ImageStatusInfo.SetLoadInfo(IDB_STATUS_INFO,hResInstance);
	m_ImageBombEffect.SetLoadInfo(IDB_BOMB_EFFECT,hResInstance);
	m_ImageValueOrder.SetLoadInfo(IDB_VALUE_ORDER,hResInstance);
	m_ImageLastTurnTip.SetLoadInfo(IDB_LAST_TURN_TIP,hResInstance);
	m_ImageTurnScore.SetLoadInfo(IDB_TURN_SCORE,hResInstance);

	return;
}

//析构函数
CGameClientView::~CGameClientView()
{
}

//消息解释
BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
{
	//提示消息
	if (m_ToolTipCtrl.m_hWnd!=NULL) 
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//重置界面
void CGameClientView::ResetGameView()
{
	//删除时间
	KillTimer(IDI_BOMB_EFFECT);

	//得分变量
	m_TurnScore=0;
	ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));

	//游戏变量
	m_lCellScore=0L;
	m_cbMainValue=0;
	ZeroMemory(m_cbValueOrder,sizeof(m_cbValueOrder));
	m_nLButtonUpCount=0;

	//用户状态
	ZeroMemory(m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(m_bUserContinue,sizeof(m_bUserContinue));

	//游戏状态
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));
	ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));

	//移动变量
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_wHoverCardItem=INVALID_ITEM;
	m_wMouseDownItem=INVALID_ITEM;

	//爆炸动画
	m_bBombEffect=false;
	m_cbBombFrameIndex=0;

	//状态变量
	m_bShowScore=false;
	m_bLastTurnCard=false;

	//控制按钮
	m_btStart.ShowWindow(SW_HIDE);

	//禁止按钮
	m_btStustee.EnableWindow(FALSE);
	m_btLastTurn.EnableWindow(FALSE);
	m_btSort.EnableWindow(FALSE);
	m_btSortCardCount.EnableWindow(FALSE);
	m_btSortCardColor.EnableWindow(FALSE);
	m_btSortCardOrder.EnableWindow(FALSE);

	//游戏按钮
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btPassCard.ShowWindow(SW_HIDE);
	//m_btOutPrompt.ShowWindow(SW_HIDE);

	//还原按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btStustee.SetButtonImage(IDB_BT_START_TRUSTEE,hResInstance,false);

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_UserCardControl[i].SetCardData(NULL,0);
		m_HandCardControl[i].SetCardData(NULL,0);
		m_HandCardControl[i].SetDisplayItem(false);
	}

	//扑克控件
	m_HandCardControl[MYSELF_VIEW_ID].SetPositively(false);

	return ;
}

//调整控件
VOID CGameClientView::RectifyGameView(INT nWidth, INT nHeight)
{
	//头像位置
	m_ptFace[0].x=(nWidth-m_nXFace)/2;
	m_ptFace[0].y=5;
	m_ptName[0].x=m_ptFace[0].x+m_nXFace+10;
	m_ptName[0].y=m_ptFace[0].y;
	m_ptTimer[0].x=m_ptFace[0].x-m_nXTimer/2-5;
	m_ptTimer[0].y=m_ptFace[0].y+m_nYTimer/2;
	m_ptReady[0].x=nWidth/2;
	m_ptReady[0].y=200;
	m_ptAuto[0].x=nWidth/2+m_nXFace+m_nXTimer+35;
	m_ptAuto[0].y=m_nYBorder+5;

	m_ptFace[2].x=(nWidth-m_nXFace)/2;
	m_ptFace[2].y=nHeight-m_nYFace-5;
	m_ptName[2].x=m_ptFace[2].x+m_nXFace+10;
	m_ptName[2].y=m_ptFace[2].y+m_nYFace/2+5;
	m_ptTimer[2].x=m_ptFace[0].x-m_nXTimer/2-5;
	m_ptTimer[2].y=m_ptFace[2].y+5;
	m_ptReady[2].x=nWidth/2;
	m_ptReady[2].y=nHeight-260;
	m_ptAuto[2].x=nWidth/2+m_nXFace+m_nXTimer+35;
	m_ptAuto[2].y=nHeight-m_nYBorder-m_nYFace-10;

	m_ptFace[1].x=5;
	m_ptFace[1].y=(nHeight-m_nYFace)/2-50;
	m_ptName[1].x=5;
	m_ptName[1].y=m_ptFace[1].y+m_nYFace+10;
	m_ptTimer[1].x=m_ptFace[1].x+m_nXTimer/2;
	m_ptTimer[1].y=m_ptFace[1].y-m_nYTimer/2-5;
	m_ptReady[1].x=nWidth*3/10;
	m_ptReady[1].y=nHeight/2-50;
	m_ptAuto[1].x=m_nXBorder+5;
	m_ptAuto[1].y=nHeight/2-m_nYBorder-m_nYFace-m_nYTimer-83;

	m_ptFace[3].x=nWidth-m_nXFace-5;
	m_ptFace[3].y=(nHeight-m_nYFace)/2-50;
	m_ptName[3].x=nWidth-5;
	m_ptName[3].y=m_ptFace[3].y+m_nYFace+10;
	m_ptTimer[3].x=m_ptFace[3].x+m_nXTimer/2-15;
	m_ptTimer[3].y=m_ptFace[1].y-m_nYTimer/2-5;
	m_ptReady[3].x=nWidth*7/10;
	m_ptReady[3].y=nHeight/2-50;
	m_ptAuto[3].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptAuto[3].y=nHeight/2-m_nYBorder-m_nYFace-m_nYTimer-83;

	//扑克位置
	m_ptHeap[0].SetPoint(m_ptName[0].x+5,m_ptName[0].y+18);
	m_ptHeap[1].SetPoint(m_ptName[1].x+5,m_ptName[1].y+18);
	m_ptHeap[2].SetPoint(m_ptName[2].x+5,m_ptName[2].y+18);
	m_ptHeap[3].SetPoint(m_ptName[3].x-80,m_ptName[3].y+18);

	//放弃位置
	m_ptPass[0].SetPoint(nWidth/2,180);
	m_ptPass[1].SetPoint(nWidth*3/10,nHeight/2-50);
	m_ptPass[2].SetPoint(nWidth/2,nHeight-260);
	m_ptPass[3].SetPoint(nWidth*7/10,nHeight/2-50);

	//用户扑克
	m_HandCardControl[0].SetBenchmarkPos(nWidth/2,60,enXCenter,enYTop);
	m_HandCardControl[1].SetBenchmarkPos(75,nHeight/2-50,enXLeft,enYCenter);
	m_HandCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-60,enXCenter,enYBottom);
	m_HandCardControl[3].SetBenchmarkPos(nWidth-75,nHeight/2-50,enXRight,enYCenter);

	//出牌扑克
	m_UserCardControl[0].SetBenchmarkPos(nWidth/2,145,enXCenter,enYTop);
	m_UserCardControl[1].SetBenchmarkPos(nWidth*3/10,nHeight/2-50,enXCenter,enYCenter);
	m_UserCardControl[2].SetBenchmarkPos(nWidth/2,nHeight-300,enXCenter,enYCenter);
	m_UserCardControl[3].SetBenchmarkPos(nWidth*7/10,nHeight/2-50,enXCenter,enYCenter);

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//计算位置
	CSize CardSize=m_HandCardControl[MYSELF_VIEW_ID].GetCardSize();
	INT nYPosButton=nHeight-CardSize.cy-DEF_SHOOT_DISTANCE-63;

	//控制按钮
	CRect rcStart;
	m_btStart.GetWindowRect(&rcStart);
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcStart.Width())/2,nYPosButton-rcStart.Height(),0,0,uFlags);

	//控制按钮
	CRect rcControl;
	m_btStustee.GetWindowRect(&rcControl);
	DeferWindowPos(hDwp,m_btStustee,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*1-5,0,0,uFlags);
	DeferWindowPos(hDwp,m_btLastTurn,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*2-10,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSortCardCount,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*3-15,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSortCardColor,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*4-20,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSort,		  NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*5-25,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSortCardOrder,NULL,nWidth-rcControl.Width()-5,nHeight-rcControl.Height()*6-30,0,0,uFlags);

	//游戏按钮
	CRect rcOutCard;
	m_btOutCard.GetWindowRect(&rcOutCard);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcOutCard.Width()*3/2-10,nYPosButton-rcOutCard.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2+rcOutCard.Width()/2+10,nYPosButton-rcOutCard.Height(),0,0,uFlags);
	//DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth/2-rcOutCard.Width()/2,nYPosButton-rcOutCard.Height(),0,0,uFlags);
	//DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth,nHeight,0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDwp);

	return;
}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageViewFill,enMode_Spread);
	DrawViewImage(pDC,m_ImageViewBack,enMode_Centent);

	//构造字符
	TCHAR szBuffer[32]=TEXT("");

	//状态信息
	//CImageHandle HandleStatusInfo(&m_ImageStatusInfo);
	//m_ImageStatusInfo.BitBlt(pDC->m_hDC,5,5,SRCCOPY);
	//_sntprintf(szBuffer,CountArray(szBuffer),TEXT(" %d 分"),m_PlayerScore[0]+m_PlayerScore[2]);
	//pDC->TextOut(100,23,szBuffer,lstrlen(szBuffer));
	//_sntprintf(szBuffer,CountArray(szBuffer),TEXT(" %d 分"),m_PlayerScore[1]+m_PlayerScore[3]);
	//pDC->TextOut(100,46,szBuffer,lstrlen(szBuffer));		

	pDC->SetTextColor(RGB(240,240,240));
	//本轮分数信息
	if(m_TurnScore>0)
	{
		CImageHandle HandleTurnScore(&m_ImageTurnScore);
		m_ImageTurnScore.AlphaDrawImage(pDC,nWidth/2-m_ImageTurnScore.GetWidth()/2,nHeight/2-m_ImageTurnScore.GetHeight()/2,RGB(255,0,255));
		DrawNumberString(pDC,m_TurnScore,nWidth/2+m_ImageTurnScore.GetWidth()/2+30,nHeight/2);
	}

	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		const tagUserData * pUserData=GetUserInfo(i);
		WORD wUserTimer=GetUserTimer(i);
		if (pUserData==NULL) continue;

		//绘画属性
		DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
		if (wUserTimer!=0&&wUserTimer<=30) DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,GetUserTimer(i),99L);

		//绘画用户
		pDC->SetTextAlign((i==3)?TA_RIGHT|TA_TOP:TA_LEFT|TA_TOP);
		pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName,lstrlen(pUserData->szName));
		
		//得分信息
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("得分： %d 分"),m_PlayerScore[i]);
		if(i!=2)
		{
			if(i==3){ pDC->SetTextAlign(TA_LEFT|TA_TOP);pDC->TextOut(m_ptHeap[i].x,m_ptHeap[i].y+18,szBuffer,lstrlen(szBuffer));}
			else pDC->TextOut(m_ptHeap[i].x-5,m_ptHeap[i].y+18,szBuffer,lstrlen(szBuffer));
		}
		if(i==2) pDC->TextOut(5,nHeight-36,szBuffer,lstrlen(szBuffer));

		//托管标志
		if(m_bAutoPlayer[i]==true)
		{
			CPngImage ImageScore;
			ImageScore.LoadImage(AfxGetInstanceHandle(),TEXT("TRUSTEE"));
			ImageScore.DrawImage(pDC,m_ptAuto[i].x,m_ptAuto[i].y);
		}
	}


	//用户扑克
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		//用户扑克
		m_UserCardControl[i].DrawCardControl(pDC);

		//玩家扑克
		if (i==MYSELF_VIEW_ID)
		{
			//绘画扑克
			m_HandCardControl[i].DrawCardControl(pDC);

			//绘画数字
			if (m_HandCardControl[i].GetCardCount()>0)
			{
				//构造字符
				TCHAR szCardCount[32]=TEXT("");
				_sntprintf(szCardCount,CountArray(szCardCount),TEXT("牌数：剩余 %d 张"),m_HandCardControl[i].GetCardCount());

				//输出字符
				pDC->SetTextAlign(TA_LEFT|TA_TOP);
				pDC->TextOut(5,nHeight-18,szCardCount,lstrlen(szCardCount));
			}
		}

		//用户扑克
		if ((i!=MYSELF_VIEW_ID)&&(m_HandCardControl[i].GetCardCount()>0))
		{
			if (m_HandCardControl[i].GetDisplayItem()==false)
			{
				//绘画数字
				WORD wCardCount=m_HandCardControl[i].GetCardCount();
						
				if(wCardCount<=10)
				{
					//构造字符
					TCHAR szCardCount[32]=TEXT("");
					_sntprintf(szCardCount,CountArray(szCardCount),TEXT("剩余        张"));

					//输出字符
					pDC->SetTextAlign(TA_LEFT|TA_TOP);
					pDC->TextOut(m_ptHeap[i].x-5,m_ptHeap[i].y,szCardCount,lstrlen(szCardCount));
					
					DrawNumberString(pDC,wCardCount,m_ptHeap[i].x+42,m_ptHeap[i].y+5);
				}
			}
			else
			{
				//绘画扑克
				m_HandCardControl[i].DrawCardControl(pDC);
			}
		}
	}

	//用户扑克
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_UserCardControl[i].DrawCardControl(pDC);
	}

	//加载资源
	CPngImage ImageWinOrder;
	ImageWinOrder.LoadImage(AfxGetInstanceHandle(),TEXT("WIN_ORDER"));
	CSize SizeWinOrder(ImageWinOrder.GetWidth()/4,ImageWinOrder.GetHeight());

	//绘画名次
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_wWinOrder[i]!=0)
		{
			//获取位置
			CPoint CenterPoint;
			m_HandCardControl[i].GetCenterPoint(CenterPoint);

			//绘画信息
			ImageWinOrder.DrawImage(pDC,CenterPoint.x-SizeWinOrder.cx/2,CenterPoint.y-SizeWinOrder.cy/2,SizeWinOrder.cx,SizeWinOrder.cy,
				(m_wWinOrder[i]-1)*SizeWinOrder.cx,0,SizeWinOrder.cx,SizeWinOrder.cy);
		}
	}

	//用户标志
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		const tagUserData * pUserData=GetUserInfo(i);
		if (pUserData==NULL) continue;

		//同意标志
		BYTE cbUserStatus=pUserData->cbUserStatus;
		if ((cbUserStatus==US_READY)||(m_bUserContinue[i]==true)) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
	}

	//加载资源
	CImageHandle HandleUserPass(&m_ImageUserPass);
	CSize SizeUserPass(m_ImageUserPass.GetWidth(),m_ImageUserPass.GetHeight());

	//绘画状态
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bUserPass[i]==true)
		{
			INT nXDrawPos=m_ptPass[i].x-SizeUserPass.cx/2;
			INT nYDrawPos=m_ptPass[i].y-SizeUserPass.cy/2;
			m_ImageUserPass.AlphaDrawImage(pDC,nXDrawPos,nYDrawPos,RGB(255,0,255));
		}
	}

	//上轮标志
	if (m_bLastTurnCard==true)
	{
		//加载资源
		CImageHandle HandleLastTurnTip(&m_ImageLastTurnTip);

		//绘画标志
		INT nImageWidth=m_ImageLastTurnTip.GetWidth();
		INT nImageHegith=m_ImageLastTurnTip.GetHeight();
		m_ImageLastTurnTip.AlphaDrawImage(pDC,(nWidth-nImageWidth)/2,(nHeight-nImageHegith)/2-50,RGB(255,0,255));
	}

	//绘画爆炸
	if (m_bBombEffect==true)
	{
		//加载资源
		CImageHandle HandleBombEffect(&m_ImageBombEffect);

		//绘画效果
		INT nImageHeight=m_ImageBombEffect.GetHeight();
		INT nImageWidth=m_ImageBombEffect.GetWidth()/BOMB_EFFECT_COUNT;
		m_ImageBombEffect.AlphaDrawImage(pDC,(nWidth-nImageWidth)/2,0,nImageWidth,nImageHeight,
			nImageWidth*(m_cbBombFrameIndex%BOMB_EFFECT_COUNT),0,RGB(255,0,255));
	}

	//积分视图
	m_ScoreControl.DrawScoreView(pDC,nWidth/2,nHeight*2/5);

	return;
}

//主牌数值
bool CGameClientView::SetMainValue(BYTE cbMainValue)
{
	//设置变量
	m_cbMainValue=cbMainValue;

	//更新界面
	UpdateGameView(NULL);

	return true;
}

//单元积分
bool CGameClientView::SetCellScore(LONG lCellScore)
{
	//设置变量
	m_lCellScore=lCellScore;

	//更新界面
	UpdateGameView(NULL);

	return true;
}

//设置爆炸
bool CGameClientView::SetBombEffect(bool bBombEffect)
{
	if (bBombEffect==true)
	{
		//设置变量
		m_bBombEffect=true;
		m_cbBombFrameIndex=0;

		//启动时间
		SetTimer(IDI_BOMB_EFFECT,100,NULL);
	}
	else
	{
		//停止动画
		if (m_bBombEffect==true)
		{
			//删除时间
			KillTimer(IDI_BOMB_EFFECT);

			//设置变量
			m_bBombEffect=false;
			m_cbBombFrameIndex=0;

			//更新界面
			UpdateGameView(NULL);
		}
	}

	return true;
}

//设置上轮
VOID CGameClientView::SetLastTurnCard(bool bLastTurnCard)
{
	//设置变量
	if (bLastTurnCard!=m_bLastTurnCard)
	{
		//设置变量
		m_bLastTurnCard=bLastTurnCard;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//设置放弃
bool CGameClientView::SetUserPass(WORD wChairID, bool bUserPass)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bUserPass[i]=bUserPass;
	}
	else
	{
		ASSERT(wChairID<GAME_PLAYER);
		m_bUserPass[wChairID]=bUserPass;
	}

	//更新界面
	UpdateGameView(NULL);

	return true;
}

//设置继续
bool CGameClientView::SetUserContinue(WORD wChairID, bool bContinue)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bUserContinue[i]=bContinue;
	}
	else
	{
		ASSERT(wChairID<GAME_PLAYER);
		m_bUserContinue[wChairID]=bContinue;
	}

	//更新界面
	UpdateGameView(NULL);

	return true;
}

//设置名次
bool CGameClientView::SetUserWinOrder(WORD wChairID, WORD wWinOrder)
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_wWinOrder[i]=wWinOrder;
	}
	else
	{
		ASSERT(wChairID<GAME_PLAYER);
		m_wWinOrder[wChairID]=wWinOrder;
	}

	//更新界面
	UpdateGameView(NULL);

	return true;
}

//设置信息
bool CGameClientView::SetUserPersist(WORD wChairID, WORD wPersistInfo[2])
{
	//设置变量
	if (wChairID<GAME_PLAYER)
	{
		m_wPersistInfo[wChairID][0]=wPersistInfo[0];
		m_wPersistInfo[wChairID][1]=wPersistInfo[1];
	}
	else
	{
		ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));
	}

	//更新界面
	UpdateGameView(NULL);

	return true;
}

//设置等级
bool CGameClientView::SetUserValueOrder(BYTE cbMySelfOrder, BYTE cbEnemyOrder)
{
	//设置变量
	m_cbValueOrder[0]=cbMySelfOrder;
	m_cbValueOrder[1]=cbEnemyOrder;

	//更新界面
	UpdateGameView(NULL);

	return true;
}

//设置游戏分数
bool CGameClientView::SetScoreInfo(int TurnScore,int PlayerScore[4])
{
	m_TurnScore=TurnScore;
	CopyMemory(m_PlayerScore,PlayerScore,sizeof(m_PlayerScore));
	UpdateGameView(NULL);
	return true;
}

//绘画数字
VOID CGameClientView::DrawNumberString(CDC * pDC, LONG lNumber, INT nXPos, INT nYPos)
{
	//加载资源
	CImageHandle HandleNumber(&m_ImageNumber);
	INT nNumberHeight=m_ImageNumber.GetHeight();
	INT nNumberWidth=m_ImageNumber.GetWidth()/10;

	//计算数目
	LONG lNumberCount=0;
	LONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//位置定义
	INT nYDrawPos=nYPos-nNumberHeight/2;
	INT nXDrawPos=nXPos+lNumberCount*nNumberWidth/2-nNumberWidth;

	//绘画桌号
	for (LONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		LONG lCellNumber=lNumber%10;
		m_ImageNumber.AlphaDrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,lCellNumber*nNumberWidth,0,RGB(255,0,255));

		//设置变量
		lNumber/=10;
		nXDrawPos-=nNumberWidth;
	};

	return;
}

//开始按钮
VOID CGameClientView::OnBnClickedStart()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);

	return;
}

//出牌按钮
VOID CGameClientView::OnBnClickedOutCard()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_OUT_CARD,0,0);

	return;
}

//PASS按钮
VOID CGameClientView::OnBnClickedPassCard()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_PASS_CARD,0,0);

	return;
}

//提示按钮
VOID CGameClientView::OnBnClickedOutPrompt()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_OUT_PROMPT,0,0);

	return;
}

//拖管按钮
VOID CGameClientView::OnBnClickedStustee()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_TRUSTEE_CONTROL,0,0);

	return;
}
//理牌按钮
VOID CGameClientView::OnBnClickedSort()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_SORT_HAND_CARD,255,255);

	return;
}
//排序按钮
VOID CGameClientView::OnBnClickedSortCardColor()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_SORT_HAND_CARD,ST_TONGHUA,ST_TONGHUA);

	return;
}

//排序按钮
VOID CGameClientView::OnBnClickedSortCardCount()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_SORT_HAND_CARD,ST_QINGDUN,ST_QINGDUN);

	return;
}

//排序按钮
VOID CGameClientView::OnBnClickedSortCardOrder()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_SORT_HAND_CARD,ST_ORDER,ST_ORDER);

	return;
}

//上轮按钮
VOID CGameClientView::OnBnClickedLastTurn()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_LAST_TURN_CARD,0,0);

	return;
}

//时间消息
VOID CGameClientView::OnTimer(UINT nIDEvent)
{
	//爆炸动画
	if (nIDEvent==IDI_BOMB_EFFECT)
	{
		//停止判断
		if (m_bBombEffect==false)
		{
			KillTimer(IDI_BOMB_EFFECT);
			return;
		}

		//设置变量
		if ((m_cbBombFrameIndex+1)>=BOMB_EFFECT_COUNT)
		{
			//删除时间
			KillTimer(IDI_BOMB_EFFECT);

			//设置变量
			m_bBombEffect=false;
			m_cbBombFrameIndex=0;
		}
		else m_cbBombFrameIndex++;

		//更新界面
		UpdateGameView(NULL);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//创建函数
INT CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//变量定义
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	//控制按钮
	m_btStart.Create(NULL,WS_CHILD,rcCreate,this,IDC_START);
	m_btStustee.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_TRUSTEE);
	m_btLastTurn.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_LAST_TURN);
	m_btSort.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_SORT_CARD);
	m_btSortCardColor.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_SORT_CARD_COLOR);
	m_btSortCardCount.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_SORT_CARD_COUNT);
	m_btSortCardOrder.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_SORT_CARD_ORDER);
	//m_btStusteeControl.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_TRUSTEE_CONTROL);
	//m_btStusteeControl.EnableWindow(FALSE);

	//游戏按钮
	m_btOutCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_OUT_CARD);
	m_btPassCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_PASS_CARD);
	//m_btOutPrompt.Create(NULL,WS_CHILD,rcCreate,this,IDC_OUT_PROMPT);

	//控制按钮
	m_btStart.SetButtonImage(IDB_BT_START,hResInstance,false);
	m_btLastTurn.SetButtonImage(IDB_BT_LAST_TURN,hResInstance,false);
	m_btStustee.SetButtonImage(IDB_BT_START_TRUSTEE,hResInstance,false);
	m_btSort.SetButtonImage(IDB_BT_SORT,hResInstance,false);
	m_btSortCardColor.SetButtonImage(IDB_BT_SORT_CARD_COLOR,hResInstance,false);
	m_btSortCardCount.SetButtonImage(IDB_BT_SORT_CARD_COUNT,hResInstance,false);
	m_btSortCardOrder.SetButtonImage(IDB_BT_SORT_CARD_ORDER,hResInstance,false);

	//m_btStusteeControl.SetButtonImage(IDB_START_TRUSTEE,hInstance,false);

	//游戏按钮
	m_btOutCard.SetButtonImage(IDB_BT_OUT_CARD,hResInstance,false);
	m_btPassCard.SetButtonImage(IDB_BT_PASS_CARD,hResInstance,false);
	//m_btOutPrompt.SetButtonImage(IDB_BT_OUT_PROMPT,hResInstance,false);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//用户扑克
		if (i==MYSELF_VIEW_ID)
		{
			m_HandCardControl[i].SetDirection(true);
			m_HandCardControl[i].SetCardDistance(DEF_X_DISTANCE,DEF_Y_DISTANCE,DEF_SHOOT_DISTANCE);
		}

		//玩家扑克
		if (i==0)
		{
			m_HandCardControl[i].SetCardMode(true);
			m_HandCardControl[i].SetDirection(true);
			m_HandCardControl[i].SetCardDistance(DEF_X_DISTANCE_SMALL,DEF_Y_DISTANCE_SMALL,0);
		}

		//玩家扑克
		if ((i==1)||(i==3))
		{
			m_HandCardControl[i].SetCardMode(true);
			m_HandCardControl[i].SetDirection(false);
			m_HandCardControl[i].SetCardDistance(DEF_X_DISTANCE_SMALL,DEF_Y_DISTANCE_SMALL,0);
		}

		//出牌扑克
		m_UserCardControl[i].SetDisplayItem(true);
		m_UserCardControl[i].SetCardDistance(DEF_X_DISTANCE,0,0);
	}

	//建立提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btStart,TEXT("开始游戏"));

	return 0;
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//扑克测试
	if (m_HandCardControl[MYSELF_VIEW_ID].OnEventSetCursor(MousePoint)==true) return TRUE;

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//鼠标消息
VOID CGameClientView::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	//变量定义
	WORD wHoverCardItem=INVALID_ITEM;

	//移动判断
	if (m_wHoverCardItem!=INVALID_ITEM)
	{
		//获取光标
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);

		//扑克大小
		CSize ControlSize;
		m_HandCardControl[MYSELF_VIEW_ID].GetControlSize(ControlSize);

		//扑克位置
		CPoint OriginPoint;
		m_HandCardControl[MYSELF_VIEW_ID].GetOriginPoint(OriginPoint);

		//横行调整
		if (MousePoint.x<OriginPoint.x) MousePoint.x=OriginPoint.x;
		if (MousePoint.x>(OriginPoint.x+ControlSize.cx)) MousePoint.x=(OriginPoint.x+ControlSize.cx);

		//获取索引
		MousePoint.y=OriginPoint.y+DEF_SHOOT_DISTANCE;
		wHoverCardItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(MousePoint);

		//移动变量
		if (wHoverCardItem!=m_wHoverCardItem) m_bMoveMouse=true;
	}

	//交换扑克
	if ((m_bSwitchCard==true)&&(wHoverCardItem!=m_wHoverCardItem))
	{
		//设置扑克
		m_HandCardControl[MYSELF_VIEW_ID].MoveCardItem(wHoverCardItem);

		//更新界面
		UpdateGameView(NULL);
	}

	//选择扑克
	if ((m_bSelectCard==true)&&(wHoverCardItem!=m_wHoverCardItem))
	{
		//设置扑克
		if (wHoverCardItem>m_wMouseDownItem)
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetSelectIndex(m_wMouseDownItem,wHoverCardItem);
		}
		else
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetSelectIndex(wHoverCardItem,m_wMouseDownItem);
		}

		//更新界面
		UpdateGameView(NULL);
	}

	//设置变量
	m_wHoverCardItem=wHoverCardItem;

	return;
}

//鼠标消息
VOID CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//状态判断
	if (m_HandCardControl[MYSELF_VIEW_ID].GetPositively()==true)
	{
		//区域判断
		if (m_HandCardControl[MYSELF_VIEW_ID].GetCardFromPoint(Point)==NULL)
		{
			//获取扑克
			BYTE cbCardData[MAX_COUNT];
			WORD wCardCount=m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbCardData,CountArray(cbCardData));

			//更新界面
			if (wCardCount>=0)
			{
				//设置扑克
				m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

				//发送消息
				AfxGetMainWnd()->SendMessage(IDM_LEFT_HIT_CARD,0,0);

				//更新界面
				UpdateGameView(NULL);
			}
		}
		else
		{
			//出牌消息
			if (m_btOutCard.IsWindowVisible()&&m_btOutCard.IsWindowEnabled())
			{
				AfxGetMainWnd()->SendMessage(IDM_OUT_CARD,0,0);
			}

		}
	}

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);

	m_nLButtonUpCount++;
	if ( m_nLButtonUpCount == 2 )
	{
		return __super::OnLButtonUp(nFlags, Point);
	}

	//默认处理
	if (m_bMoveMouse==false)
	{
		//获取扑克
		tagCardItem * pCardItem=NULL;
		WORD wMouseDownItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(Point);
		if (wMouseDownItem==m_wMouseDownItem) pCardItem=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromPoint(Point);

		//设置扑克
		if (pCardItem!=NULL)
		{
			//设置扑克
			pCardItem->bShoot=!pCardItem->bShoot;

			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_LEFT_HIT_CARD,0,0);

			//更新界面
			UpdateGameView(NULL);
		}
	}
		//选择处理
	if ((m_bSelectCard==true)&&(m_bMoveMouse==true))
	{
		//设置扑克
		if (m_wHoverCardItem>m_wMouseDownItem)
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetShootIndex(m_wMouseDownItem,m_wHoverCardItem);
		}
		else
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetShootIndex(m_wHoverCardItem,m_wMouseDownItem);
		}

		//发送消息
		AfxGetMainWnd()->SendMessage(IDM_LEFT_HIT_CARD,0,0);

		//更新界面
		UpdateGameView(NULL);
	}

	//交换处理
	if ((m_bSwitchCard==true)&&(m_bMoveMouse==true))
	{
		//设置扑克
		m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

		//发送消息
		AfxGetMainWnd()->SendMessage(IDM_LEFT_HIT_CARD,0,0);
		AfxGetMainWnd()->SendMessage(IDM_SORT_HAND_CARD,ST_CUSTOM,ST_CUSTOM);

		//更新界面
		UpdateGameView(NULL);
	}

	//释放鼠标
	ReleaseCapture();

	//设置变量
	m_bMoveMouse=false;
	m_bSwitchCard=false;
	m_bSelectCard=false;

	//设置索引
	m_wMouseDownItem=INVALID_ITEM;
	m_wHoverCardItem=INVALID_ITEM;
	
	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);
	m_nLButtonUpCount=0;

	//状态判断
	if (m_HandCardControl[MYSELF_VIEW_ID].GetPositively()==true)
	{
		//获取扑克
		m_wMouseDownItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(Point);

		//设置扑克
		if (m_wMouseDownItem!=INVALID_ITEM)
		{
			//获取扑克
			tagCardItem * pCardItem=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(m_wMouseDownItem);

			//设置变量
			m_bMoveMouse=false;
			m_wHoverCardItem=m_wMouseDownItem;

			//操作变量
			if (pCardItem->bShoot==false) 
			{
				m_bSelectCard=true;
			}
			else
			{
				m_bSwitchCard=true;
			}

			//设置鼠标
			SetCapture();
		}
	}

	return;
}

//鼠标消息
VOID CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags, Point);

	//设置扑克
	if (m_HandCardControl[MYSELF_VIEW_ID].GetCardFromPoint(Point)==NULL)
	{
		//获取扑克
		BYTE cbCardData[MAX_COUNT];
		WORD wCardCount=m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbCardData,CountArray(cbCardData));

		//更新界面
		if (wCardCount>=0)
		{
			//禁止按钮
			m_btOutCard.EnableWindow(FALSE);

			//设置扑克
			m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

			//更新界面
			UpdateGameView(NULL);
		}
	}
	else
	{
		//状态判断
		if (m_HandCardControl[MYSELF_VIEW_ID].GetPositively()==true) m_wMouseDownItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(Point);
		
		//获取扑克
		tagCardItem * pCardItem=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(m_wMouseDownItem);
		BYTE bSelectCard=pCardItem->cbCardData;
		pCardItem->bShoot=false;
		
		//查找附近牌
		WORD wStartIndex=0;
		WORD wEndIndex=0;
		for(int i=m_wMouseDownItem;i>=0;i--)
		{
			if((m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(i)->cbCardData&0x0F)==(bSelectCard&0x0F)) wStartIndex=i;
			else	break;
		}
		for(int i=m_wMouseDownItem;i<m_HandCardControl[MYSELF_VIEW_ID].GetCardCount();i++)
		{
			if((m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(i)->cbCardData&0x0F)==(bSelectCard&0x0F))		wEndIndex=i;
			else	break;
		}

		//弹起增加牌
		m_HandCardControl[MYSELF_VIEW_ID].SetShootIndex(wStartIndex,wEndIndex);

		AfxGetMainWnd()->SendMessage(IDM_LEFT_HIT_CARD,0,0);

		UpdateGameView(NULL);
	}

	return;
}
void CGameClientView::SetAutoUser(bool bAutoUser[4])
{
	CopyMemory(m_bAutoPlayer,bAutoUser,sizeof(m_bAutoPlayer));
		
	//更新界面
	UpdateGameView(NULL);
	return;
}
//////////////////////////////////////////////////////////////////////////
