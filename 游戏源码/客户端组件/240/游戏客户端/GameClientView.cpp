#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include ".\gameclientview.h"

///////////////////////////////
#define IDC_PUBLIC_CARD				110				//亮牌
#define IDC_CONTINUE_CARD			111				//续牌
#define IDC_DING_CARD				112				//顶牌
#define IDC_NIAN_CARD				113				//拈牌
#define IDC_GIVE_UP					114				//放弃等待
#define IDC_CHANGE_CARD				115				//换牌
#define IDC_CONTINUE_GAME			116				//继续游戏


#define IDC_GAME_BEGIN				120				//开始按钮
#define IDC_OUT_CARD				121				//出牌按钮
#define IDC_HINT                    119             //提示按钮
#define IDC_GIVE_BACK				122				//留底按钮
#define	IDC_GIVE_CARD				123				//回牌按钮

#define IDC_LAST_CARD				125				//上轮按钮
#define IDC_BACK_CARD				126				//底牌按钮
#define IDC_AI   					128				//AI
#define	IDC_QUIT_					129				//退出


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_GAME_BEGIN,OnHitBegin)

	
	ON_BN_CLICKED(IDC_GIVE_BACK,OnHitBackCard)
	ON_BN_CLICKED(IDC_OUT_CARD,OnHitOutCard)
	ON_BN_CLICKED(IDC_HINT,OnHint)

	ON_BN_CLICKED(IDC_LAST_CARD, OnHitLastTurn)
	ON_BN_CLICKED(IDC_BACK_CARD, OnHitLookBack)
	ON_BN_CLICKED(IDC_QUIT_, OnQuit_)
	ON_BN_CLICKED(IDC_AI, OnAI)
//	ON_WM_RBUTTONDOWN()
//ON_WM_RBUTTONUP()
//ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	m_bSunShi = false;
	m_wPublicChair=INVALID_CHAIR;
	m_wViewPublicChair=INVALID_CHAIR;
	m_bPublicCard =0xF0;

	memset(m_nCurScore ,0,sizeof(m_nCurScore));
	m_wBankerChair=INVALID_CHAIR;
	m_wViewBankerChair=INVALID_CHAIR;
	m_Timer_Scale=0.00;

	LoadResource();
	return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
}

//加载资源
void	CGameClientView::LoadResource()
{
	HINSTANCE	hInstance=AfxGetInstanceHandle();
	m_ImageBack.LoadFromResource(hInstance,IDB_BACK);

	m_ImageBackCenter.LoadFromResource(hInstance,IDB_BACK_CENTER);

	m_ImageFullTimer.LoadFromResource(hInstance,IDB_TIMER_FULL);
	m_ImageEmptyTimer.LoadFromResource(hInstance,IDB_TIMER_EMPTY);

	m_ImageNT.LoadFromResource(hInstance,IDB_NT);
	m_ImageNTLogo.LoadFromResource(hInstance,IDB_NT_LOGO);

	m_ImageScore.LoadFromResource(hInstance,IDB_GET_SCORE);
	m_ImagePublicCard.LoadFromResource(hInstance,IDB_PUBLIC_CARD);

	m_ImageFlower.LoadFromResource(hInstance,IDB_BK_FLOWER);
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	RECT rc;
	memset(&rc,0,sizeof(rc));

	m_NtList.Create(IDD_NT_LIST,this);
	if(!m_ScoreWnd.Create(NULL, 
		                   NULL, 
							WS_CHILD|WS_CLIPSIBLINGS|WS_TILED, 
							rc, 
							this, 
							56))
	{
		return -1;
	}
	m_ScoreWnd.ShowWindow(SW_HIDE);

	m_dlgHintInfo.Create(IDD_HINT_INFO,this);

	m_btBegin.Create(TEXT(""),WS_CHILD,rc,this,IDC_GAME_BEGIN);
	m_btOutCard.Create(TEXT(""),WS_CHILD,rc,this,IDC_OUT_CARD);
	m_btHint.Create(TEXT(""),WS_CHILD,rc,this,IDC_HINT);

	m_btLastTurn.Create(TEXT(""),WS_CHILD,rc,this,IDC_LAST_CARD);

	m_btAI.Create(TEXT(""),WS_CHILD,rc,this,IDC_AI);

	HINSTANCE	hInstance=AfxGetInstanceHandle();

	m_btHint.SetButtonImage(IDB_BT_HINT,hInstance,false);
	m_btBegin.SetButtonImage(IDB_BT_START,hInstance,false);
	m_btOutCard.SetButtonImage(IDB_BT_OUT_CARD,hInstance,false);


	m_btLastTurn.SetButtonImage(IDB_BT_LAST_ROUND,hInstance,false);
	//建立客户扑克
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_OutCard[i].Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rc,this,20+i,NULL);
		m_OutCard[i].SetShowCard(true);
	}
	m_MeCard.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rc,this,25,NULL);
	m_MeCard.SetShowCard(true);
	m_MeCard.SetUseMouse(true);
	m_MeCard.SetCardParameter(14,0,10);


	m_PublicCard.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rc,this,27,NULL);
	m_PublicCard.SetShowCard(true);
	m_PublicCard.SetShowTen(true);

	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_Score_Card[i].Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,rc,this,30+i,NULL);
		m_Score_Card[i].SetShowCard(true);
	}

	return 0;
}


//隐藏按钮
void	CGameClientView::HideAllButton()
{
	CWnd *pAllButton[] ={&m_NtList,&m_dlgHintInfo,&m_btBegin,&m_btOutCard,&m_btHint};      
	for(int i=0;i<sizeof(pAllButton)/sizeof(pAllButton[0]);i++)
	{
		pAllButton[i]->ShowWindow(SW_HIDE);
	}
}
//重置界面
void CGameClientView::ResetGameView()
{

	m_btAI.SetButtonImage(IDB_BT_AI,AfxGetInstanceHandle(),false);
	
	m_wPublicChair=INVALID_CHAIR;
	m_wViewPublicChair=INVALID_CHAIR;
	m_bPublicCard =0xF0;
	m_wBankerChair=INVALID_CHAIR;
	m_wViewBankerChair=INVALID_CHAIR;

	HideAllButton();
	//所有扑克还原
	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_OutCard[i].SetCard(NULL,NULL,0);
	}

	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_nCurScore[i] =0;
		m_Score_Card[i].SetCard(NULL,NULL,0);
	}

	m_MeCard.SetCard(NULL,NULL,0);
	m_PublicCard.SetCard(NULL,NULL,0);
	m_ScoreWnd.ShowWindow(SW_HIDE);

	BOOL	bNT[6]={0};
	m_NtList.UpdateNtList(bNT);
	return;
}

//调整控件
void CGameClientView::RectifyGameView(int cx, int cy)
{
	if ((cx==0)||(cy==0)) return;
	

	m_PlayRect.left=m_nXBorder;
	m_PlayRect.right=cx-m_nXBorder;
	m_PlayRect.top=m_nYBorder+60;
	m_PlayRect.bottom=cy-m_nYBorder-60;



	BYTE bTempSpace=10;
	m_Rct_DymTimer.left		=m_PlayRect.left+bTempSpace+55;
	m_Rct_DymTimer.right	=m_Rct_DymTimer.left+m_ImageEmptyTimer.GetWidth();
	m_Rct_DymTimer.top		=m_PlayRect.top+bTempSpace-52;
	m_Rct_DymTimer.bottom	=m_Rct_DymTimer.top+m_ImageEmptyTimer.GetHeight();

	int iMoveUpDistance =0;

	POINT ptCardPoint[4];

	m_Rct_Info.bottom= m_PlayRect.bottom+20;
	m_Rct_Info.top   = m_Rct_Info.bottom;
	m_Rct_Info.left  = m_PlayRect.left;
	m_Rct_Info.right = m_Rct_Info.left+170;


	//以头像为中心展开
	m_ptFace[0].x = m_PlayRect.Width()/2;
	m_ptFace[0].y = m_PlayRect.top -40;
	m_ptName[0].x = m_ptFace[0].x+m_nXFace/2;
	m_ptName[0].y = m_ptFace[0].y +m_nYFace+10;
	
	m_ptTimer[0].x = m_ptFace[0].x -m_nXTimer -30;
	m_ptTimer[0].y = m_ptFace[0].y +5;
	m_ptReady[0].x = m_ptTimer[0].x+40;
	m_ptReady[0].y = m_ptTimer[0].y-25;
	m_PtNt[0].x   = m_ptFace[0].x +m_nXFace + 100;
	m_PtNt[0].y   = m_ptFace[0].y +m_nYFace -m_ImageNT.GetHeight();
	
	ptCardPoint[0].x=m_PlayRect.Width()/2+m_nXFace;
	ptCardPoint[0].y=m_ptFace[0].y +m_nYFace+HEIGHT_CARD_SMALL+10;
	m_PtScoreCard[0].x = m_ptFace[0].x + m_nXFace+15;
	m_PtScoreCard[0].y =  m_ptName[0].y;
	
	if(m_bSunShi ==false)
	{
		m_ptFace[1].x =  m_PlayRect.right -m_nXFace-5;
		m_ptFace[1].y = m_PlayRect.bottom/2-50;
		m_ptName[1].x = m_ptFace[1].x +m_nXFace;
		m_ptName[1].y = m_ptFace[1].y +m_nYFace+23;
		m_ptTimer[1].x = m_ptFace[1].x ;
		m_ptTimer[1].y =  m_ptFace[1].y +m_nYFace+90;
		m_ptReady[1].x = m_ptFace[1].x-25;
		m_ptReady[1].y = m_ptFace[1].y+25;
		m_PtNt[1].x =  m_ptFace[1].x +m_nXFace-m_ImageNT.GetWidth();
		m_PtNt[1].y = m_ptFace[1].y  -m_ImageNT.GetHeight()-5;
		m_PtScoreCard[1].x = m_ptFace[1].x+m_nXFace-10;
		m_PtScoreCard[1].y =m_ptFace[1].y +m_nYFace+65;// m_ptTimer[1].y+55;
		ptCardPoint[1].x = m_ptFace[1].x -20;
		ptCardPoint[1].y = m_ptFace[1].y ;
	}
	else
	{
		m_ptFace[3].x =  m_PlayRect.right -m_nXFace-5;
		m_ptFace[3].y = m_PlayRect.bottom/2-50;
		m_ptName[3].x = m_ptFace[3].x +m_nXFace;
		m_ptName[3].y = m_ptFace[3].y +m_nYFace+23;
		m_ptTimer[3].x = m_ptFace[3].x ;
		m_ptTimer[3].y = m_ptFace[3].y +m_nYFace+38;
		m_ptReady[3].x = m_ptFace[3].x-25;
		m_ptReady[3].y = m_ptFace[3].y+25;
		m_PtNt[3].x =  m_ptFace[3].x +m_nXFace-m_ImageNT.GetWidth();
		m_PtNt[3].y = m_ptFace[3].y  -m_ImageNT.GetHeight()-5;
		m_PtScoreCard[3].x = m_ptFace[3].x+m_nXFace-10;
		m_PtScoreCard[3].y = m_ptTimer[3].y+55;
		ptCardPoint[3].x = m_ptFace[3].x -20;
		ptCardPoint[3].y = m_ptFace[3].y ;
	}

	POINT	*pPTArray[]={m_PtNt,m_PtNtLogo,m_ptFace,m_ptName,m_ptTimer,m_ptReady};
	for(int i=0;i<sizeof(pPTArray)/sizeof(pPTArray[0]);i++)
	{
		pPTArray[i][2].x=pPTArray[i][0].x;
		pPTArray[i][2].y=pPTArray[i][0].y +m_PlayRect.bottom-m_nYFace-m_nYBorder-20;
	}
	m_PtScoreCard[2].x=m_PtScoreCard[0].x;
	m_PtScoreCard[2].y=m_ptFace[2].y+m_nYFace-HEIGHT_CARD_SMALL;
	ptCardPoint[2].x=ptCardPoint[0].x;
	ptCardPoint[2].y=m_ptFace[2].y-HEIGHT_CARD-58;

	if(m_bSunShi ==false)
	{
		m_ptFace[3].x =  m_PlayRect.left +30;
		m_ptFace[3].y = m_ptFace[1].y;
		m_ptName[3].x = m_ptFace[3].x ;
		m_ptName[3].y = m_ptName[1].y;
		m_ptTimer[3].x = m_ptFace[3].x ;
		m_ptTimer[3].y = m_ptTimer[1].y;
		m_PtNt[3].x =  m_ptFace[3].x ;
		m_PtNt[3].y = m_PtNt[1].y;
		m_ptReady[3].x = m_ptFace[3].x-25;
		m_ptReady[3].y = m_ptReady[1].y ;
		ptCardPoint[3].x = m_ptFace[3].x +20+m_nXFace;
		ptCardPoint[3].y = ptCardPoint[1].y;
		m_PtScoreCard[3].x = m_ptFace[3].x+7;
		m_PtScoreCard[3].y = m_PtScoreCard[1].y;
	}
	else
	{
		m_ptFace[1].x =  m_PlayRect.left +5;
		m_ptFace[1].y = m_ptFace[3].y;
		m_ptName[1].x = m_ptFace[1].x ;
		m_ptName[1].y = m_ptName[3].y;
		m_ptTimer[1].x = m_ptFace[1].x ;
		m_ptTimer[1].y = m_ptTimer[3].y;
		m_PtNt[1].x =  m_ptFace[1].x ;
		m_PtNt[1].y = m_PtNt[3].y;
		m_ptReady[1].x = m_ptFace[1].x-25;
		m_ptReady[1].y = m_ptReady[3].y ;
		ptCardPoint[1].x = m_ptFace[1].x +20+m_nXFace;
		ptCardPoint[1].y = ptCardPoint[3].y;
		m_PtScoreCard[1].x = m_ptFace[1].x+7;
		m_PtScoreCard[1].y = m_PtScoreCard[3].y;
	}


	m_PtPublicCard.x=m_PlayRect.left+12;
	m_PtPublicCard.y=m_ptFace[2].y+m_nYFace+8-m_ImagePublicCard.GetHeight();

	m_PublicCard.SetBasePoint(CPoint(m_PtPublicCard.x+m_ImagePublicCard.GetWidth()/2,m_PtPublicCard.y+m_ImagePublicCard.GetHeight()/2),
		CSmallGameCard::XShowMode::SWX_MODE_CENTER,CSmallGameCard::YShowMode::SWY_MODE_CENTER);

	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(i%2==0)
		{
			m_Score_Card[i].SetBasePoint(CPoint(m_PtScoreCard[i].x,m_PtScoreCard[i].y),CSmallGameCard::XShowMode::SWX_MODE_LEFT,CSmallGameCard::YShowMode::SWY_MODE_TOP);
		}
		else
		{
			if(m_bSunShi==false)
			{
				if(i==1)
				{
					m_Score_Card[i].SetBasePoint(CPoint(m_PtScoreCard[i].x,m_PtScoreCard[i].y),CSmallGameCard::XShowMode::SWX_MODE_RIGHT,CSmallGameCard::YShowMode::SWY_MODE_TOP);
				}
				else
				{
					m_Score_Card[i].SetBasePoint(CPoint(m_PtScoreCard[i].x,m_PtScoreCard[i].y),CSmallGameCard::XShowMode::SWX_MODE_LEFT,CSmallGameCard::YShowMode::SWY_MODE_TOP);
				}
			}
			else
			{
				if(i==3)
				{
					m_Score_Card[i].SetBasePoint(CPoint(m_PtScoreCard[i].x,m_PtScoreCard[i].y),CSmallGameCard::XShowMode::SWX_MODE_RIGHT,CSmallGameCard::YShowMode::SWY_MODE_TOP);
				}
				else
				{
					m_Score_Card[i].SetBasePoint(CPoint(m_PtScoreCard[i].x,m_PtScoreCard[i].y),CSmallGameCard::XShowMode::SWX_MODE_LEFT,CSmallGameCard::YShowMode::SWY_MODE_TOP);
				}
			}
		}
	}

	//移动扑克
	m_OutCard[0].SetBasePoint(CPoint(ptCardPoint[0].x,ptCardPoint[0].y),
		CGameCard::XShowMode::SWX_MODE_CENTER,CGameCard::YShowMode::SWY_MODE_TOP);

	if(m_bSunShi ==false)
	{
		m_OutCard[1].SetBasePoint(CPoint(ptCardPoint[1].x,ptCardPoint[1].y),
			CGameCard::XShowMode::SWX_MODE_RIGHT,CGameCard::YShowMode::SWY_MODE_CENTER);
		m_OutCard[3].SetBasePoint(CPoint(ptCardPoint[3].x,ptCardPoint[3].y),
			CGameCard::XShowMode::SWX_MODE_LEFT,CGameCard::YShowMode::SWY_MODE_CENTER);
	}
	else
	{
		m_OutCard[3].SetBasePoint(CPoint(ptCardPoint[3].x,ptCardPoint[3].y),
			CGameCard::XShowMode::SWX_MODE_RIGHT,CGameCard::YShowMode::SWY_MODE_CENTER);
		m_OutCard[1].SetBasePoint(CPoint(ptCardPoint[1].x,ptCardPoint[1].y),
			CGameCard::XShowMode::SWX_MODE_LEFT,CGameCard::YShowMode::SWY_MODE_CENTER);
	}


	m_OutCard[2].SetBasePoint(CPoint(ptCardPoint[2].x,ptCardPoint[2].y),
		CGameCard::XShowMode::SWX_MODE_CENTER,CGameCard::YShowMode::SWY_MODE_BOTTOM);

	m_MeCard.SetBasePoint(CPoint(cx/2,m_ptFace[2].y-17),
		CGameCardEx::XShowMode::SWX_MODE_CENTER,CGameCardEx::YShowMode::SWY_MODE_BOTTOM);




	BYTE btSpace =18;
	//移动反牌视图
	CRect ContronRect;
	m_NtList.GetWindowRect(&ContronRect);
	m_NtList.MoveWindow((cx-ContronRect.Width())/2+160,ptCardPoint[2].y-45,ContronRect.Width(),ContronRect.Height(),FALSE);

	m_dlgHintInfo.GetWindowRect(&ContronRect);
	m_dlgHintInfo.MoveWindow(cx/2-ContronRect.Width()/2,cy/2-30-ContronRect.Height()/2,ContronRect.Width(),ContronRect.Height(),FALSE);

	m_ScoreWnd.GetWindowRect(&ContronRect);
	m_ScoreWnd.MoveWindow(cx/2-ContronRect.Width()/2, cy/3-45,ContronRect.Width(),ContronRect.Height(),FALSE);
	//移动控件
	HDWP hDwp=BeginDeferWindowPos(20);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btBegin,NULL,cx/2-20,ptCardPoint[2].y+5,0,0,uFlags);
	CRect btRect;
	m_btOutCard.GetClientRect(&btRect);
	DeferWindowPos(hDwp,m_btOutCard,NULL,cx/2-btRect.Width()-20,ptCardPoint[2].y+5,0,0,uFlags);
	DeferWindowPos(hDwp,m_btHint,NULL,cx/2+btRect.Width()-20,ptCardPoint[2].y+5,0,0,uFlags);

	iMoveUpDistance +=8;
	btSpace=8;
	DeferWindowPos(hDwp,m_btLastTurn,NULL,m_PlayRect.right-btRect.Width()-20,m_ptFace[2].y,0,0,uFlags);
	m_btLastTurn.GetClientRect(&btRect);
	//DeferWindowPos(hDwp,m_btAI,NULL,m_PlayRect.right-btRect.Width()-20,m_ptFace[2].y-50,0,0,uFlags);
	EndDeferWindowPos(hDwp);
	return;
}


//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画界面
	pDC->FillSolidRect(0,0,nWidth,nHeight,RGB(255,255,0));

	//画信息背景

	//画游戏背景
	int iBkgViewHeight=m_ImageBack.GetHeight();
	int iBkgViewWidth=m_ImageBack.GetWidth();

	for(int i=0;i<m_PlayRect.Width()/iBkgViewWidth+1;i++)
	{
		for(int j=0;j<(m_PlayRect.Height())/iBkgViewHeight+2;j++)
		{
			m_ImageBack.BitBlt(pDC->m_hDC,
				i*iBkgViewWidth,j*iBkgViewHeight,
				//	m_PlayRect.top+j*iBkgViewHeight,
				SRCCOPY);
		}
	}

	//画中心标志图
	m_ImageBackCenter.BitBlt(pDC->m_hDC,nWidth/2-m_ImageBackCenter.GetWidth()/2,nHeight/2-m_ImageBackCenter.GetHeight()/2-50);


	
	const int flower_width = m_ImageFlower.GetWidth()/4;
	const int flower_height = m_ImageFlower.GetHeight();

	m_ImageFlower.BitBlt(pDC->m_hDC,m_nXBorder,m_nYBorder,flower_width,flower_height,0,0);
	m_ImageFlower.BitBlt(pDC->m_hDC,nWidth - m_nXBorder-flower_width,m_nYBorder,flower_width,flower_height,1*flower_width,0);
	m_ImageFlower.BitBlt(pDC->m_hDC,m_nXBorder,nHeight-m_nYBorder-flower_height,flower_width,flower_height,2*flower_width,0);
	m_ImageFlower.BitBlt(pDC->m_hDC,nWidth - m_nXBorder-flower_width,nHeight-m_nYBorder-flower_height,flower_width,flower_height,3*flower_width,0);

	//画时间条
	m_ImageEmptyTimer.AlphaDrawImage(pDC,m_Rct_DymTimer.left,m_Rct_DymTimer.top,RGB(255,255,255));


	const int height_dymtimer= m_ImageEmptyTimer.GetHeight();
	const int width_left	= 47;
	const int width_time_bar= 125;

	const int width = width_left + (int)(width_time_bar * m_Timer_Scale);

	m_ImageFullTimer.AlphaDrawImage(pDC,m_Rct_DymTimer.left,m_Rct_DymTimer.top,	width,height_dymtimer,0,0,RGB(255,255,255));


	int temp=m_PublicCard.GetCard(NULL,NULL);
	if(temp)
	{
		m_ImagePublicCard.BitBlt(pDC->m_hDC,m_PtPublicCard.x,m_PtPublicCard.y);
	}
	CString str;
	//绘画用户
	pDC->SetTextColor(RGB(255,255,255));
	for(int i=0; i<GAME_PLAYER; i++)
	{
		//变量定义
		const tagUserData *pUserData = GetUserInfo(i);
		if(pUserData)//如果椅子ID为i的椅子上坐了一个人。。。则画
		{
			if(m_nCurScore[i])
			{
				str.Format("%d",m_nCurScore[i]);
				pDC->SetTextAlign(TA_LEFT);
				if((i!=1 &&m_bSunShi==false) || (i!=3 &&m_bSunShi ==true))
				{
					m_ImageScore.BitBlt(pDC->m_hDC,m_PtScoreCard[i].x-10,m_PtScoreCard[i].y-m_ImageScore.GetHeight()-2);
					pDC->TextOut(m_PtScoreCard[i].x+25,m_PtScoreCard[i].y-20,str);
				}
				else
				{
					m_ImageScore.BitBlt(pDC->m_hDC,m_PtScoreCard[i].x-(m_ImageScore.GetWidth()-10),m_PtScoreCard[i].y-m_ImageScore.GetHeight()-2);
					pDC->TextOut(m_PtScoreCard[i].x-(m_ImageScore.GetWidth()-10)+35,m_PtScoreCard[i].y-20,str);
				}
			}
			//用户名
			if( (i!=1 &&m_bSunShi==false) || (i!=3 &&m_bSunShi ==true))
			{
				pDC->SetTextAlign(TA_LEFT);
				if(i%2)
				{
					pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName,lstrlen(pUserData->szName));
				}
				else
				{
					if(i==0)
					{
						CRect rect_temp;
						rect_temp.left		= m_ptName[i].x-20;
						rect_temp.top		= m_ptName[i].y;
						rect_temp.right		= rect_temp.left + 50;
						rect_temp.bottom	= rect_temp.top + 15 ;
						pDC->DrawText(pUserData->szName, &rect_temp, DT_LEFT|DT_SINGLELINE|DT_BOTTOM|DT_END_ELLIPSIS);
					}
					else
					{
						pDC->SetTextAlign(TA_CENTER);
						pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName);
					}
				}
			}
			else
			{
				//右边的改为右对齐
				pDC->SetTextAlign(TA_RIGHT);
				pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName,lstrlen(pUserData->szName));
			}


			//记时器
			WORD wUserTimer = this->GetUserTimer(i);
			if(wUserTimer) 
			{
				this->DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			}

			//举起的手
			if(pUserData->cbUserStatus==US_READY) 
			{
				this->DrawUserReady(pDC,m_ptReady[i].x+50,	m_ptReady[i].y+50);
			}
			if(i%2)
			{
				//头像
				this->DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y+15,pUserData->cbUserStatus==US_OFFLINE);
			}
			else
			{
				//头像
				this->DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
			}
		}
	}

	pDC->SetTextColor(RGB(0,0,0));

	//画庄家位置
	if(m_wBankerChair!=INVALID_CHAIR)
	{
		m_ImageNT.AlphaDrawImage(pDC,m_PtNt[m_wViewBankerChair].x,m_PtNt[m_wViewBankerChair].y,RGB(0,0,255));
	}
	//不显示亮主(不显示亮主位置)
	//if(m_wPublicChair!=INVALID_CHAIR && m_bPublicCard!=0xF0)
	if(0)
	{ 
		BYTE bPos = m_bPublicCard >>4;
		BYTE bHeight=m_ImageNTLogo.GetHeight();
		BYTE bWidth=m_ImageNTLogo.GetWidth()/6;
		m_ImageNTLogo.AlphaDrawImage(pDC,m_PtNtLogo[m_wViewPublicChair].x,m_PtNtLogo[m_wViewPublicChair].y,bWidth,bHeight,bPos*bWidth,0,RGB(255,255,255));
	}
	//画提示信息
	return;
}

/*
//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画界面
	pDC->FillSolidRect(0,0,nWidth,nHeight,RGB(255,255,0));


	//画游戏背景
	int iBkgViewHeight=m_ImageBack.GetHeight();
	int iBkgViewWidth=m_ImageBack.GetWidth();

	for(int i=0;i<m_PlayRect.Width()/iBkgViewWidth+1;i++)
	{
		for(int j=0;j<(m_PlayRect.Height())/iBkgViewHeight+2;j++)
		{
			m_ImageBack.BitBlt(pDC->m_hDC,
				i*iBkgViewWidth,
				//m_PlayRect.top+
				j*iBkgViewHeight,
				SRCCOPY);
		}
	}

	//画时间条
	m_ImageEmptyTimer.AlphaDrawImage(pDC, 
		m_Rct_DymTimer.left,
		m_Rct_DymTimer.top, 
		RGB(255,255,255));


	const int height_dymtimer= m_ImageEmptyTimer.GetHeight();
	const int width_left	= 47;
	const int width_time_bar= 125;

	const int width = width_left + (int)(width_time_bar * m_Timer_Scale);

	m_ImageFullTimer.AlphaDrawImage(pDC,
		m_Rct_DymTimer.left,
		m_Rct_DymTimer.top,
		width,
		height_dymtimer,
		0,
		0,
		RGB(255,255,255));

	TCHAR name[4][32]={0};
	for(int i=0;i<GAME_PLAYER;i++)
	{
		for(int j=0;j<20;j++)
		{
			name[i][j] =75+j+i;
		}
	}

	//绘画用户
	pDC->SetTextColor(RGB(255,255,255));
	for(int i=0; i<GAME_PLAYER; i++)
	{
		//变量定义
		//	if(pUserData)//如果椅子ID为i的椅子上坐了一个人。。。则画
		{
			//用户名
			if( (i!=1 &&m_bSunShi==false) || (i!=3 &&m_bSunShi ==true))
			{
				pDC->SetTextAlign(TA_LEFT);
				if(i%2)
				{
					pDC->TextOut(m_ptName[i].x,
						m_ptName[i].y,
						name[i],
						lstrlen(name[i]));
				}
				else
				{
					CRect rect_temp;
					rect_temp.left		= m_ptName[i].x;
					rect_temp.top		= m_ptName[i].y;
					rect_temp.right		= rect_temp.left + 60;
					rect_temp.bottom	= rect_temp.top + 10 ;
					pDC->DrawText(name[i], &rect_temp, DT_LEFT|DT_SINGLELINE|DT_TOP|DT_END_ELLIPSIS);
				}
			}
			else
			{
				//右边的改为右对齐
				pDC->SetTextAlign(TA_RIGHT);
				pDC->TextOut(m_ptName[i].x,
					m_ptName[i].y,
					name[i],
					strlen(name[i]));
			}


			//记时器
			WORD wUserTimer = 12;//this->GetUserTimer(i);
			if(wUserTimer) 
			{
				this->DrawUserTimer(pDC,
					m_ptTimer[i].x,
					m_ptTimer[i].y,
					wUserTimer);
			}

			//举起的手
			//	if(pUserData->cbUserStatus==US_READY) 
			{
				this->DrawUserReady(pDC,
					m_ptReady[i].x,
					m_ptReady[i].y);
			}

			//头像
			this->DrawUserFace(pDC,
				13,
				m_ptFace[i].x,
				m_ptFace[i].y,
				false);
		}
	}
	pDC->SetTextColor(RGB(0,0,0));


	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_wBankerChair =i;
		m_wViewBankerChair=i;
		//画庄家位置
		if(m_wBankerChair!=INVALID_CHAIR)
		{
			m_ImageNT.AlphaDrawImage(pDC,m_PtNt[m_wViewBankerChair].x,m_PtNt[m_wViewBankerChair].y,RGB(0,0,255));
		}
	}
	m_bPublicCard =0x20;
	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_wPublicChair =i;
		//画亮主
		if(m_wPublicChair!=INVALID_CHAIR && m_bPublicCard!=0xF0)
		{ 
			BYTE bPos = m_bPublicCard >>4;
			BYTE bHeight=m_ImageNTLogo.GetHeight();
			BYTE bWidth=m_ImageNTLogo.GetWidth()/6;
			//		m_ImageNTLogo.AlphaDrawImage(pDC,m_PtNtLogo[m_wPublicChair].x,m_PtNtLogo[m_wPublicChair].y,bWidth,bHeight,bPos*bWidth,0,RGB(255,255,255));
		}
	}
	BYTE cards[]={0x4e,0x4f};
	m_MeCard.SetCard(cards,NULL,sizeof(cards));


	//m_MeBackCard.SetShowCard(false);
	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_Score_Card[i].SetCard(cards,NULL,sizeof(cards));
		m_OutCard[i].SetCard(cards,NULL,sizeof(cards));
	//	m_Score_Card[i].SetCard(cards,NULL,sizeof(cards));
	}
	m_PublicCard.SetCard(cards,NULL,sizeof(cards));

	memset(m_nCurScore,55,16);
	m_wBankerChair =0;
	m_wPublicChair =INVALID_CHAIR;

	
	//画提示信息
	CSkinButton *pAllButton[] ={&m_btBegin,&m_btOutCard,&m_btHint,&m_btLastTurn};
	for(int i=0;i<sizeof(pAllButton)/sizeof(pAllButton[0]);i++)
	{
		pAllButton[i]->ShowWindow(SW_SHOW);
	}
	m_NtList.ShowWindow(SW_SHOW);
	CString str ="撒反对撒发射点. ";
	m_dlgHintInfo.SetShowInfo(str);
	m_dlgHintInfo.ShowWindow(SW_SHOW);
	BOOL bIs[5]={1,1,1,0,0};
	m_NtList.UpdateNtList(bIs);

	m_btOutCard.ShowWindow(SW_SHOW);
	m_btHint.ShowWindow(SW_SHOW);

	m_ImagePublicCard.BitBlt(pDC->m_hDC,m_PtPublicCard.x,m_PtPublicCard.y);
//	CString str;
	pDC->SetTextColor(RGB(255,255,255));
	pDC->SetTextAlign(TA_LEFT);
	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_nCurScore[i]=200;
		str.Format("%d",m_nCurScore[i]);
		if(i!=1)
		{
			m_ImageScore.BitBlt(pDC->m_hDC,m_PtScoreCard[i].x-10,m_PtScoreCard[i].y-m_ImageScore.GetHeight()-2);
			pDC->TextOut(m_PtScoreCard[i].x+25,m_PtScoreCard[i].y-20,str);
		}
		else
		{
			
			m_ImageScore.BitBlt(pDC->m_hDC,m_PtScoreCard[i].x-(m_ImageScore.GetWidth()-10),m_PtScoreCard[i].y-m_ImageScore.GetHeight()-2);
			pDC->TextOut(m_PtScoreCard[i].x-(m_ImageScore.GetWidth()-10)+35,m_PtScoreCard[i].y-20,str);
		}
		str.Format("得分：%d",m_nCurScore[i]);
	//	if(i%2==0)
		{
	//		pDC->SetTextAlign(TA_LEFT);
	//		pDC->TextOut(m_PtScoreCard[i].x+5,m_PtScoreCard[i].y-20,str);
		}
//		else
		{
			if(i==221)
			{
				pDC->SetTextAlign(TA_RIGHT);
				pDC->TextOut(m_PtScoreCard[i].x,m_PtScoreCard[i].y+5,str);
			}
			else if(i==22)
			{
				pDC->SetTextAlign(TA_LEFT);
				pDC->TextOut(m_PtScoreCard[i].x,m_PtScoreCard[i].y+5,str);
			}
		}

	}
	m_ScoreWnd.ShowWindow(SW_SHOW);
	m_ScoreWnd.UpdateWindow();
	return;
}
*/









void CGameClientView::OnHitBegin()
{
	AfxGetMainWnd()->SendMessage(IDM_BEGIN,0,0);
}

void CGameClientView::OnHitBackCard()
{
	AfxGetMainWnd()->SendMessage(IDM_GIVE_BACK,0,0);
}

void CGameClientView::OnHitOutCard()
{
	AfxGetMainWnd()->SendMessage(IDM_OUT_CARD,0,0);
}
void CGameClientView::OnHint()
{
	AfxGetMainWnd()->SendMessage(IDM_HINT,0,0);
}
void CGameClientView::OnHitLastTurn()
{
	AfxGetMainWnd()->SendMessage(IDM_LOOK_LAST,0,0);
}
void CGameClientView::OnHitLookBack()
{
	AfxGetMainWnd()->SendMessage(IDM_BACK_CARD,0,0);
}
void CGameClientView::OnQuit_()
{
	if(AfxMessageBox(TEXT("你确定要退出游戏吗？"),MB_OKCANCEL) ==IDOK)
	{
		AfxGetMainWnd()->PostMessage(WM_QUIT,0,0);
	}
}
void CGameClientView::OnAI()
{
	AfxGetMainWnd()->SendMessage(IDM_AI,0,0);
}


void CGameClientView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	AfxGetMainWnd()->SendMessage(IDM_RIGHT_HITCARD);
	CGameFrameView::OnRButtonUp(nFlags, point);
}

void CGameClientView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	AfxGetMainWnd()->SendMessage(IDM_REVERT_CARD);
	CGameFrameView::OnLButtonDown(nFlags, point);
}
