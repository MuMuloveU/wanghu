#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//按钮标识
#define IDC_START						100								//开始按钮
#define IDC_EXIT						101								//离开按钮
#define IDC_ADD							102								//加注按钮
#define IDC_GIVEUP						103								//放弃按钮
#define IDC_SHOWHAND					104								//梭哈按钮
#define IDC_FOLLOW						105								//跟注按钮
#define IDC_PASS						106								//让牌按钮
#define IDC_AUTO_START					107								//自动按钮
#define IDC_SIT_DOWN					108								//坐下按钮
//IDC_SIT_DOWN 	108+GAME_PLAYER之内不能用
#define IDC_OPEN_CARD					117								//开牌按钮

//定时器标识
//#define IDI_SEND_CARD					100								//发牌定时器
#define IDI_GAME_END					101								//结束定时器
#define IDI_USER_ACTION					102								//动画定时器

//定时器时间
#define TIME_USER_ACTION				60								//动画定时器

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageViewCenter.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageHuman.SetLoadInfo(IDB_HUMAN,hInstance);
	m_ImageSmallCard.SetLoadInfo(IDB_SMALL_CARD,hInstance);
	m_ImageCard.SetLoadInfo(IDB_CARD,hInstance);
	m_ImageD.SetLoadInfo(IDB_D,hInstance);
	m_ImageCardMask.SetLoadInfo(IDB_CARD_MASK,hInstance);
	m_ImageTitleInfo.SetLoadInfo(IDB_TITLE,hInstance);
	m_ImageArrowhead.SetLoadInfo(IDB_TIME_ARROWHEAD,hInstance);
	m_ImageUserFrame.SetLoadInfo(IDB_FRAME,hInstance);

	//位置变量
	ZeroMemory(m_wUserPost,sizeof(m_wUserPost));
	ZeroMemory(m_ptSmallCard,sizeof(m_ptSmallCard));
	ZeroMemory(m_ptCard,sizeof(m_ptCard));
	ZeroMemory(m_ptJetton,sizeof(m_ptJetton));
	m_ptCenterCard.SetPoint(0,0);
	m_ptCenterJetton.SetPoint(0,0);

	//动画变量
	for(int i=0;i<GAME_PLAYER+1;i++) m_CardStatus[i].RemoveAll();
	ZeroMemory(m_JettonStatus,sizeof(m_JettonStatus));
	m_bGameEnd = false;
	m_bJettonAction = false;
	m_bCardAction = false;
	m_bCenterCount = 0;

	//数据变量
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));
	m_lCenterScore = 0L;
	m_lTurnLessScore = 0L;
	m_lTurnMaxScore = 0L;
	m_wMeChairID = INVALID_CHAIR;
	m_bShowUser = FALSE;

	//状态变量
	m_bReset = true;
	m_wDUser = INVALID_CHAIR;
	m_wMyIsLookOn = INVALID_CHAIR;
	ZeroMemory(m_wUserGender,sizeof(m_wUserGender));

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
	CRect rcCreate(0,0,0,0);
	m_ScoreView.Create(IDD_GAME_SCORE,this);
	m_Prompt.Create(IDD_DIALOG2,this);
	m_GoldControl.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS,rcCreate,this,8);

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_START);
	m_btExit.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_EXIT);
	m_btAdd.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_ADD);
	m_btGiveUp.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_GIVEUP);
	m_btShowHand.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_SHOWHAND);
	m_btFollow.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_FOLLOW);
	m_btPassCard.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_PASS);
	m_btAutoStart.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_AUTO_START);
	m_btOpenCard.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_OPEN_CARD);
	for(int i=0;i<GAME_PLAYER;i++)
		m_btSitDown[i].Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_SIT_DOWN+i);

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_BT_START,hInstance,false);
	m_btExit.SetButtonImage(IDB_BT_EXIT,hInstance,false);
	m_btAdd.SetButtonImage(IDB_BT_ADD,hInstance,false);
	m_btGiveUp.SetButtonImage(IDB_BT_GIVEUP,hInstance,false);
	m_btShowHand.SetButtonImage(IDB_BT_SHOWHAND,hInstance,false);
	m_btFollow.SetButtonImage(IDB_BT_FOLLOW,hInstance,false);
	m_btPassCard.SetButtonImage(IDB_BT_PASS_CARD,hInstance,false);
	m_btAutoStart.SetButtonImage(IDB_AUTO_START_FALSE,hInstance,false);
	m_btOpenCard.SetButtonImage(IDB_BT_OPEN_CARD,hInstance,false);
	for(int i=0;i<GAME_PLAYER;i++)
		m_btSitDown[i].SetButtonImage(IDB_BT_SITDOWN,hInstance,false);
	m_CenterCardControl.SetDisplayItem(true);

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{	
		m_CardControl[i].SetDisplayItem(true);
		m_SmallCardControl[i].SetCardDistance(5,0,0);
	}
	m_CenterCardControl.SetCardDistance(57+10,0,0);

	return 0;
}

//重置界面
void CGameClientView::ResetGameView()
{
	if(!m_bReset)return;

	//状态变量
	m_wDUser = INVALID_CHAIR;
	m_wMyIsLookOn =INVALID_CHAIR;
	m_lTurnLessScore = 0L;
	m_lTurnMaxScore = 0L;

	//隐藏控件
	m_ScoreView.ShowWindow(SW_HIDE);
	m_Prompt.ShowWindow(SW_HIDE);
	m_GoldControl.ShowWindow(SW_HIDE);
	m_btStart.ShowWindow(SW_HIDE);
	m_btExit.ShowWindow(SW_HIDE);
	m_btGiveUp.ShowWindow(SW_HIDE);
	m_btShowHand.ShowWindow(SW_HIDE);
	m_btFollow.ShowWindow(SW_HIDE);

	//动画变量
	ZeroMemory(m_wUserPost,sizeof(m_wUserPost));
	for(int i=0;i<GAME_PLAYER+1;i++) m_CardStatus[i].RemoveAll();
	ZeroMemory(m_JettonStatus,sizeof(m_JettonStatus));
	m_bGameEnd = false;
	m_bJettonAction = false;
	m_bCardAction = false;
	m_bCenterCount = 0;

	//删除定时器
	//KillTimer(IDI_SEND_CARD);
	KillTimer(IDI_USER_ACTION);
	KillTimer(IDI_GAME_END);

	//数据变量
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//隐藏控件
		m_btSitDown[i].ShowWindow(SW_HIDE);
		m_CardControl[i].SetCardData(NULL,0);
		m_SmallCardControl[i].SetCardData(NULL,0);
		m_CardControl[i].SetDisplayItem(true);
		m_SmallCardControl[i].SetDisplayItem(false);
		m_CardControl[i].SetPositively(false);
		m_SmallCardControl[i].SetPositively(false);
	}

	m_bShowUser = FALSE;
	m_wMeChairID = INVALID_CHAIR;
	m_lCenterScore = 0L;
	m_lTurnLessScore = 0L;
	m_lTurnMaxScore = 0L;
	m_CenterCardControl.SetCardData(NULL,0);
	m_CenterCardControl.SetDisplayItem(true);
	m_CenterCardControl.SetPositively(false);
	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//人物位置
	m_ptHuman[0].x = nWidth/2 + 60;
	m_ptHuman[0].y = nHeight/2-259;
	m_ptHuman[1].x = nWidth/2 + 225;
	m_ptHuman[1].y = nHeight/2-182;	
	m_ptHuman[2].x = nWidth/2 +201;
	m_ptHuman[2].y = nHeight/2+29;	
	m_ptHuman[3].x = nWidth/2 +46;
	m_ptHuman[3].y = nHeight/2+111;	
	m_ptHuman[4].x = nWidth/2 -173;
	m_ptHuman[4].y = nHeight/2 +110;	
	m_ptHuman[5].x = nWidth/2 -344;
	m_ptHuman[5].y = nHeight/2 + 28;	
	m_ptHuman[6].x = nWidth/2 - 358;
	m_ptHuman[6].y = nHeight/2 - 182;	
	m_ptHuman[7].x = nWidth/2 - 201;
	m_ptHuman[7].y = nHeight/2-265;

	//人物位置
	m_ptWomen[0].x = nWidth/2 + 63;
	m_ptWomen[0].y = nHeight/2-263;
	m_ptWomen[1].x = nWidth/2 + 228;
	m_ptWomen[1].y = nHeight/2-186;	
	m_ptWomen[2].x = nWidth/2 +226;
	m_ptWomen[2].y = nHeight/2+33;	
	m_ptWomen[3].x = nWidth/2 +42;
	m_ptWomen[3].y = nHeight/2+106;	
	m_ptWomen[4].x = nWidth/2 -170;
	m_ptWomen[4].y = nHeight/2 +106;	
	m_ptWomen[5].x = nWidth/2 -358;
	m_ptWomen[5].y = nHeight/2 + 33;	
	m_ptWomen[6].x = nWidth/2 - 355;
	m_ptWomen[6].y = nHeight/2 - 186;	
	m_ptWomen[7].x = nWidth/2 - 193;
	m_ptWomen[7].y = nHeight/2-263;

	//名字位置	
	m_ptName[0].x = nWidth/2+74;
	m_ptName[0].y = nHeight/2-279-10;
	m_ptName[1].x = nWidth/2+256;
	m_ptName[1].y = nHeight/2-220-10;	
	m_ptName[2].x = nWidth/2+256;
	m_ptName[2].y = nHeight/2+163-10;	
	m_ptName[3].x = nWidth/2+76;
	m_ptName[3].y = nHeight/2+231-10;	
	m_ptName[4].x = nWidth/2-176;
	m_ptName[4].y = nHeight/2+231-10;
	m_ptName[5].x = nWidth/2-363;
	m_ptName[5].y = nHeight/2+162-10;	
	m_ptName[6].x = nWidth/2-363;
	m_ptName[6].y = nHeight/2-221-10;	
	m_ptName[7].x = nWidth/2-179;
	m_ptName[7].y = nHeight/2-279-10;

	//举手位置 
	m_ptReady[0].x = nWidth/2 + 130;
	m_ptReady[0].y = nHeight/2-120;
	m_ptReady[1].x = nWidth/2 + 220;
	m_ptReady[1].y = nHeight/2-70;	
	m_ptReady[2].x = nWidth/2 +200;
	m_ptReady[2].y = nHeight/2+30;	
	m_ptReady[3].x = nWidth/2 +100;
	m_ptReady[3].y = nHeight/2+90;	
	m_ptReady[4].x = nWidth/2 -110;
	m_ptReady[4].y = nHeight/2 +90;	
	m_ptReady[5].x = nWidth/2 -200;
	m_ptReady[5].y = nHeight/2 + 30;	
	m_ptReady[6].x = nWidth/2 - 220;
	m_ptReady[6].y = nHeight/2 - 70;	
	m_ptReady[7].x = nWidth/2 - 130;
	m_ptReady[7].y = nHeight/2-120;
	
	//m_ptReady[0].x = nWidth/2 + 130;
	//m_ptReady[0].y = nHeight/2-220;
	//m_ptReady[1].x = nWidth/2 + 300;
	//m_ptReady[1].y = nHeight/2-160;	
	//m_ptReady[2].x = nWidth/2 +290;
	//m_ptReady[2].y = nHeight/2+70;	
	//m_ptReady[3].x = nWidth/2 +100;
	//m_ptReady[3].y = nHeight/2+150;	
	//m_ptReady[4].x = nWidth/2 -110;
	//m_ptReady[4].y = nHeight/2 +150;	
	//m_ptReady[5].x = nWidth/2 -285;
	//m_ptReady[5].y = nHeight/2 + 70;	
	//m_ptReady[6].x = nWidth/2 - 300;
	//m_ptReady[6].y = nHeight/2 - 160;	
	//m_ptReady[7].x = nWidth/2 - 130;
	//m_ptReady[7].y = nHeight/2-220;

	//时间位置
	//m_ptTimer[0].x = nWidth/2 + 160;
	//m_ptTimer[0].y = nHeight/2-250;
	//m_ptTimer[1].x = nWidth/2 + 200;
	//m_ptTimer[1].y = nHeight/2-180;	
	//m_ptTimer[2].x = nWidth/2 +260;
	//m_ptTimer[2].y = nHeight/2+180;	
	//m_ptTimer[3].x = nWidth/2 +180;
	//m_ptTimer[3].y = nHeight/2+170;	
	//m_ptTimer[4].x = nWidth/2 -200;
	//m_ptTimer[4].y = nHeight/2 +170;	
	//m_ptTimer[5].x = nWidth/2 -280;
	//m_ptTimer[5].y = nHeight/2 + 180;	
	//m_ptTimer[6].x = nWidth/2 - 260;
	//m_ptTimer[6].y = nHeight/2 - 180;	
	//m_ptTimer[7].x = nWidth/2 - 210;
	//m_ptTimer[7].y = nHeight/2-250;

	//时间位置
	m_ptTimer[0].x = nWidth/2 + 60;
	m_ptTimer[0].y = nHeight/2-170;
	m_ptTimer[1].x = nWidth/2 + 210;
	m_ptTimer[1].y = nHeight/2-145;	
	m_ptTimer[2].x = nWidth/2 +325;
	m_ptTimer[2].y = nHeight/2+35;	
	m_ptTimer[3].x = nWidth/2 +180;
	m_ptTimer[3].y = nHeight/2+190;	
	m_ptTimer[4].x = nWidth/2 -40;
	m_ptTimer[4].y = nHeight/2 +190;	
	m_ptTimer[5].x = nWidth/2 -225;
	m_ptTimer[5].y = nHeight/2 + 170;	
	m_ptTimer[6].x = nWidth/2 - 335;
	m_ptTimer[6].y = nHeight/2 - 30;	
	m_ptTimer[7].x = nWidth/2 - 210;
	m_ptTimer[7].y = nHeight/2-160;
	for(INT i=0;i<GAME_PLAYER;i++)
	{
		m_ptTimer[i].y -=10; 
	}

	//小牌位置
	m_ptSmallCard[0].x = nWidth/2 + 90;
	m_ptSmallCard[0].y = nHeight/2 - 175+60;
	m_ptSmallCard[1].x = nWidth/2 + 210;
	m_ptSmallCard[1].y = nHeight/2-120+60;	
	m_ptSmallCard[2].x = nWidth/2 +200;
	m_ptSmallCard[2].y = nHeight/2-15+60;	
	m_ptSmallCard[3].x = nWidth/2 +65;
	m_ptSmallCard[3].y = nHeight/2+35+60;	
	m_ptSmallCard[4].x = nWidth/2 -100;
	m_ptSmallCard[4].y = nHeight/2 +35+60;	
	m_ptSmallCard[5].x = nWidth/2 -230;
	m_ptSmallCard[5].y = nHeight/2 - 15+60;	
	m_ptSmallCard[6].x = nWidth/2 - 235;
	m_ptSmallCard[6].y = nHeight/2 - 120+60;	
	m_ptSmallCard[7].x = nWidth/2 - 110;
	m_ptSmallCard[7].y = nHeight/2-175+60;
	for(INT i=0;i<GAME_PLAYER;i++)m_ptSmallCard[i].y -=10; 
	
	//小扑克视图
	m_SmallCardControl[0].SetBenchmarkPos(m_ptSmallCard[0],enXLeft,enYCenter);
	m_SmallCardControl[1].SetBenchmarkPos(m_ptSmallCard[1],enXLeft,enYCenter);
	m_SmallCardControl[2].SetBenchmarkPos(m_ptSmallCard[2],enXLeft,enYCenter);
	m_SmallCardControl[3].SetBenchmarkPos(m_ptSmallCard[3],enXLeft,enYCenter);
	m_SmallCardControl[4].SetBenchmarkPos(m_ptSmallCard[4],enXLeft,enYCenter);
	m_SmallCardControl[5].SetBenchmarkPos(m_ptSmallCard[5],enXLeft,enYCenter);
	m_SmallCardControl[6].SetBenchmarkPos(m_ptSmallCard[6],enXLeft,enYCenter);
	m_SmallCardControl[7].SetBenchmarkPos(m_ptSmallCard[7],enXLeft,enYCenter);

	//扑克位置
	m_ptCard[0].x = nWidth/2 + 90;
	m_ptCard[0].y = nHeight/2-240;
	m_ptCard[1].x = nWidth/2 + 270;
	m_ptCard[1].y = nHeight/2-160;	
	m_ptCard[2].x = nWidth/2 +260;
	m_ptCard[2].y = nHeight/2+60;	
	m_ptCard[3].x = nWidth/2 +70;
	m_ptCard[3].y = nHeight/2+130;	
	m_ptCard[4].x = nWidth/2 -140;
	m_ptCard[4].y = nHeight/2 +130;	
	m_ptCard[5].x = nWidth/2 -340;
	m_ptCard[5].y = nHeight/2 + 60;	
	m_ptCard[6].x = nWidth/2 - 347;
	m_ptCard[6].y = nHeight/2 -160;	
	m_ptCard[7].x = nWidth/2 - 170;
	m_ptCard[7].y = nHeight/2-240;
	for(INT i=0;i<GAME_PLAYER;i++)m_ptCard[i].y -=14; 

	//扑克视图
	m_CardControl[0].SetBenchmarkPos(m_ptCard[0],enXLeft,enYTop);
	m_CardControl[1].SetBenchmarkPos(m_ptCard[1],enXLeft,enYTop);
	m_CardControl[2].SetBenchmarkPos(m_ptCard[2],enXLeft,enYTop);
	m_CardControl[3].SetBenchmarkPos(m_ptCard[3],enXLeft,enYTop);
	m_CardControl[4].SetBenchmarkPos(m_ptCard[4],enXLeft,enYTop);
	m_CardControl[5].SetBenchmarkPos(m_ptCard[5],enXLeft,enYTop);
	m_CardControl[6].SetBenchmarkPos(m_ptCard[6],enXLeft,enYTop);
	m_CardControl[7].SetBenchmarkPos(m_ptCard[7],enXLeft,enYTop);

	//筹码位置
	m_ptJetton[0].x = nWidth/2 + 60;
	m_ptJetton[0].y = nHeight/2-80;
	m_ptJetton[1].x = nWidth/2 +145;
	m_ptJetton[1].y = nHeight/2-66;
	m_ptJetton[2].x = nWidth/2 +175;//170
	m_ptJetton[2].y = nHeight/2+68;	
	m_ptJetton[3].x = nWidth/2 +44;
	m_ptJetton[3].y = nHeight/2+107;
	m_ptJetton[4].x = nWidth/2 -44;
	m_ptJetton[4].y = nHeight/2 +107;
	m_ptJetton[5].x = nWidth/2 -170;
	m_ptJetton[5].y = nHeight/2 + 68;	
	m_ptJetton[6].x = nWidth/2 - 140-10;
	m_ptJetton[6].y = nHeight/2 - 66;	
	m_ptJetton[7].x = nWidth/2 - 60;
	m_ptJetton[7].y = nHeight/2-80;
	for(INT i=0;i<GAME_PLAYER;i++)m_ptJetton[i].y -=13; 

	//发牌位置
	m_SendCardPos.SetPoint(nWidth/2-5,nHeight/2-145-10);

	//中心扑克位置
	m_ptCenterCard.SetPoint(nWidth/2-165,nHeight/2-4-14);

	//中心扑克
	m_CenterCardControl.SetBenchmarkPos(m_ptCenterCard,enXLeft,enYCenter);

	//中心筹码位置
	m_ptCenterJetton.SetPoint(nWidth/2,nHeight/2+45-10);

	//D位置
	m_ptD[0].x = nWidth/2 -12;
	m_ptD[0].y = nHeight/2 -100;
	m_ptD[1].x = nWidth/2 + 155;
	m_ptD[1].y = nHeight/2-84;	
	m_ptD[2].x = nWidth/2 +185;
	m_ptD[2].y = nHeight/2-20;	
	m_ptD[3].x = nWidth/2+140;
	m_ptD[3].y = nHeight/2 +50;	
	m_ptD[4].x = nWidth/2 -12;
	m_ptD[4].y = nHeight/2 +65;	
	m_ptD[5].x = nWidth/2 -160;
	m_ptD[5].y = nHeight/2 + 50;	
	m_ptD[6].x = nWidth/2 - 210;
	m_ptD[6].y = nHeight/2 - 20;	
	m_ptD[7].x = nWidth/2 -184;
	m_ptD[7].y = nHeight/2 -84;
	for(INT i=0;i<GAME_PLAYER;i++)m_ptD[i].y -=10; 

	//发牌结束位置
	m_SendEndingPos[0].SetPoint(nWidth/2+85,nHeight/2-100);
	m_SendEndingPos[1].SetPoint(nWidth/2+235,nHeight/2-120);
	m_SendEndingPos[2].SetPoint(nWidth/2+210,nHeight/2+140);
	m_SendEndingPos[3].SetPoint(nWidth/2+80,nHeight/2+90);
	m_SendEndingPos[4].SetPoint(nWidth/2-70,nHeight/2+90);
	m_SendEndingPos[5].SetPoint(nWidth/2-160,nHeight/2+140);
	m_SendEndingPos[6].SetPoint(nWidth/2-260,nHeight/2-120);
	m_SendEndingPos[7].SetPoint(nWidth/2-92,nHeight/2-240);
	for(INT i=0;i<GAME_PLAYER;i++)m_SendEndingPos[i].y -=10; 

	//坐下按钮位置
	m_ptSitDown[0].SetPoint(nWidth/2+95,nHeight/2-210);
	m_ptSitDown[1].SetPoint(nWidth/2+270,nHeight/2-120);
	m_ptSitDown[2].SetPoint(nWidth/2+270,nHeight/2+120);
	m_ptSitDown[3].SetPoint(nWidth/2+75,nHeight/2+200);
	m_ptSitDown[4].SetPoint(nWidth/2-140,nHeight/2+200);
	m_ptSitDown[5].SetPoint(nWidth/2-340,nHeight/2+120);
	m_ptSitDown[6].SetPoint(nWidth/2-340,nHeight/2-120);
	m_ptSitDown[7].SetPoint(nWidth/2-165,nHeight/2-210);
	for(INT i=0;i<GAME_PLAYER;i++)m_ptSitDown[i].y -=10; 

	//按钮控件
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//开始按钮
	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStart,NULL,nWidth -rcButton.Width()-m_nXBorder*2,nHeight/2+240 -rcButton.Height()/2,0,0,uFlags);

	//离开按钮
	DeferWindowPos(hDwp,m_btExit,NULL,nWidth -rcButton.Width()-m_nXBorder*2,nHeight/2+298-rcButton.Height()/2,0,0,uFlags);

	//自动开始按钮
	DeferWindowPos(hDwp,m_btAutoStart,NULL,m_nXBorder*2,nHeight/2+300,0,0,uFlags);

	//放弃按钮
	DeferWindowPos(hDwp,m_btGiveUp,NULL,nWidth/2 +88,nHeight/2+283,0,0,uFlags);

	//梭哈按钮
	DeferWindowPos(hDwp,m_btShowHand,NULL,nWidth/2 +4 ,nHeight/2+283,0,0,uFlags);

	//加注按钮
	DeferWindowPos(hDwp,m_btAdd,NULL,nWidth/2-79 ,nHeight/2+283,0,0,uFlags);

	//开牌按钮
	DeferWindowPos(hDwp,m_btOpenCard,NULL,nWidth/2-35,nHeight/2+283,0,0,uFlags);

	//跟注按钮
	DeferWindowPos(hDwp,m_btFollow,NULL,nWidth/2-163 ,nHeight/2+283,0,0,uFlags);

	//让牌按钮
	DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2-163 ,nHeight/2+283,0,0,uFlags);

	//坐下按钮
	for(int i=0;i<GAME_PLAYER;i++)
		DeferWindowPos(hDwp,m_btSitDown[i],NULL,m_ptSitDown[i].x ,m_ptSitDown[i].y,0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDwp);

	//积分视图
	//CRect rcControl;
	////m_ScoreView.GetWindowRect(&rcControl);
	////m_ScoreView.SetWindowPos(NULL,10,nHeight/2+235,/*nWidth/2-rcControl.Width()/2,nHeight/2+56,*/0,0,SWP_NOZORDER|SWP_NOSIZE);

	//提示弹起视图
	m_Prompt.SetWindowPos(NULL,nWidth/2+395,nHeight/2-200,0,0,SWP_NOZORDER|SWP_NOSIZE);

	//加注窗口
	m_GoldControl.SetBasicPoint(nWidth/2+100,nHeight/2+100);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageViewCenter,enMode_Centent);

	//人物句柄
	CImageHandle ImageHumanHandle(&m_ImageHuman);

	//获取大小
	int nItemWidth=m_ImageHuman.GetWidth()/8;
	int nItemHeight=m_ImageHuman.GetHeight()/2;

	//扑克句柄
	CImageHandle ImageCardHandle(&m_ImageCard);

	//扑克大小
	int nCardWidth=m_ImageCard.GetWidth()/13;
	int nCardHeight=m_ImageCard.GetHeight()/5;

	//D句柄
	CImageHandle ImageDHandle(&m_ImageD);

	//D大小
	int nDWidth = m_ImageD.GetWidth();
	int nDHeight = m_ImageD.GetHeight();

	//扑克掩图
	CImageHandle ImageCardMaskHandle(&m_ImageCardMask);

	//掩图大小
	int nCardMaskWidth = m_ImageCardMask.GetWidth();
	int nCardMaskHeight = m_ImageCardMask.GetHeight();

	//绘画用户
	if(!m_bReset)
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//绘制人物
			if(m_wUserGender[i] == 2)//男
			{
				m_ImageHuman.AlphaDrawImage(pDC,m_ptHuman[i].x,m_ptHuman[i].y,nItemWidth,nItemHeight,nItemWidth*i,0,RGB(255,0,255));
			}
			else if(m_wUserGender[i] == 1)//女
			{
				m_ImageHuman.AlphaDrawImage(pDC,m_ptWomen[i].x,m_ptWomen[i].y,nItemWidth,nItemHeight,nItemWidth*i,nItemHeight,RGB(255,0,255));
			}
		}
	}

	//绘画用户
	if(m_bShowUser==FALSE)return ;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer=GetUserTimer(i);
		const tagUserData * pUserData=GetUserInfo(i);

#ifdef _DEBUG
		//tagUserData Obj;
		//Obj.cbGender = 0;
		//_sntprintf( Obj.szName,sizeof(Obj.szName),TEXT("用户的名字"));
		//Obj.lScore = 10000;
		//Obj.cbUserStatus = US_READY;
		//m_wUserPost[i] = i;
		//pUserData = &Obj;
		//wUserTimer = 10;
#endif

		//绘画用户
		if (pUserData!=NULL)
		{
			WORD wUserId = m_wUserPost[i];
			pDC->SetTextColor((wUserTimer>0)?RGB(0,250,0):RGB(0,220,0));

			//绘制人物
			if(pUserData->cbGender == 0)
				m_ImageHuman.AlphaDrawImage(pDC,m_ptHuman[wUserId].x,m_ptHuman[wUserId].y,nItemWidth,nItemHeight,nItemWidth*wUserId,0,RGB(255,0,255));
			else
				m_ImageHuman.AlphaDrawImage(pDC,m_ptWomen[wUserId].x,m_ptWomen[wUserId].y,nItemWidth,nItemHeight,nItemWidth*wUserId,nItemHeight,RGB(255,0,255));

			//绘制举手
			if (pUserData->cbUserStatus==US_READY)
				DrawUserReady(pDC,m_ptReady[wUserId].x,m_ptReady[wUserId].y);

			//绘制时间
			if (wUserTimer>0)
			{
				DrawUserTimer(pDC,m_ptTimer[wUserId].x,m_ptTimer[wUserId].y,wUserTimer);
				if(wUserTimer%2==0)
				{
					CImageHandle ImageArrowheadHandle(&m_ImageArrowhead);
					int wPost = (wUserId+2)%8;
					int iAddY=0,iAddX=0;
					if(wUserId==0 || wUserId==1)
					{
						iAddY=25;
						//iAddX=10;
					}
					else if(wUserId==2)
					{
						iAddX=-40;
					}
					else if(wUserId==3||wUserId==4)
					{
						wPost = 6;
						iAddY=-45;
						iAddX=-10;
					}
					else if(wUserId==5)
					{
						iAddY=-45;
						//iAddX=10;
					}
					else if(wUserId==6)
					{
						iAddY=5;
						iAddX=35;
					}
					else if(wUserId==7)
					{
						iAddY=25;
						iAddX=10;
					}
					m_ImageArrowhead.AlphaDrawImage(pDC,m_ptTimer[wUserId].x+iAddX,m_ptTimer[wUserId].y+iAddY,
						m_ImageArrowhead.GetWidth()/8,m_ImageArrowhead.GetHeight(),
						m_ImageArrowhead.GetWidth()/8*wPost,0,RGB(255,0,255));
				}
			}

			//绘制D
			if(m_wDUser != INVALID_CHAIR)
			{
				m_ImageD.AlphaDrawImage(pDC,m_ptD[m_wDUser].x,m_ptD[m_wDUser].y,nDWidth,nDHeight,0,0,RGB(255,0,255));
			}

			//绘制小扑克
			if (pUserData->cbUserStatus!=US_READY)
				m_SmallCardControl[wUserId].DrawCardControl(pDC);

			m_CardControl[wUserId].DrawCardControl(pDC,m_bGameEnd);

			if(m_lTableScore[wUserId]>0)
			{
				m_GoldView[wUserId].DrawGoldView(pDC,m_ptJetton[wUserId].x,m_ptJetton[wUserId].y,true);
			}
		}

		//用户扑克
		m_CardControl[i].DrawCardControl(pDC,m_bGameEnd);

		//中心扑克
		m_CenterCardControl.DrawCardControl(pDC,m_bGameEnd);
	}

	//绘制中心筹码
	if(m_lCenterScore >0)
	{
		m_CenterGoldView.DrawGoldView(pDC,m_ptCenterJetton.x,m_ptCenterJetton.y,true,true,(BYTE)m_wDUser);
	}

	//绘画扑克
	for (WORD i=0;i<GAME_PLAYER+1;i++)
	{
		////绘制小扑克
		//if(i<GAME_PLAYER)
		//{
		//	m_SmallCardControl[i].DrawCardControl(pDC);
		//}

		//筹码移动
		if(m_MoveGoldView[i].GetGold()>0 && i<GAME_PLAYER)
		{
			m_MoveGoldView[i].DrawGoldView(pDC,m_JettonStatus[i].ptCourse.x,m_JettonStatus[i].ptCourse.y,true);
		}

		//小扑克
		if(m_CardStatus[i].GetCount()==0)continue;
		tagCardStatus * pCardStatus = &(m_CardStatus[i].GetAt(0));
		if(pCardStatus->wMoveCount>0)
		{
			//小扑克
			m_SmallCardControl[i].DrawOneCard(pDC,0,pCardStatus->ptCourse.x,pCardStatus->ptCourse.y);			
		}
	}

	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserInfo(i);
		//绘画用户
		if (pUserData!=NULL)
		{
			WORD wUserId = m_wUserPost[i];

			//绘画框架
			CImageHandle ImageFrameHandle(&m_ImageUserFrame);
			m_ImageUserFrame.AlphaDrawImage(pDC,m_ptName[wUserId].x,m_ptName[wUserId].y,
				m_ImageUserFrame.GetWidth()/8,m_ImageUserFrame.GetHeight(),
				m_ImageUserFrame.GetWidth()/8*wUserId,0,RGB(255,0,255));

			CFont ViewFont;
			ViewFont.CreateFont(-15,0,0,0,400,0,0,0,134,3,2,1,1,TEXT("Arial"));
			CFont *pOldFont=pDC->SelectObject(&ViewFont);

			CSize rcSize(103,30);
			CRect rcName(m_ptName[wUserId],rcSize);
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255,255,255));
			pDC->DrawText(pUserData->szName,lstrlen(pUserData->szName),&rcName,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//用户金币
			CRect rcName1(m_ptName[wUserId].x,m_ptName[wUserId].y+20,m_ptName[wUserId].x+108,m_ptName[wUserId].y+50);
			pDC->SetTextColor(RGB(255,255,4));
			TCHAR szBuffer[128]=TEXT("");
			LONG lLeaveScore=pUserData->lScore - ((m_lTotalScore[wUserId]>0)?m_lTotalScore[wUserId]:0L);
			if(lLeaveScore<0)lLeaveScore=pUserData->lScore;

			//加千数点
			LONG lTempCount = lLeaveScore;
			GetGlodString(lTempCount,szBuffer);

			pDC->DrawText(szBuffer,lstrlen(szBuffer),&rcName1,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
			pDC->SelectObject(pOldFont);
			ViewFont.DeleteObject();	
		}
	}

	//坐下按钮
	if(m_wMyIsLookOn!=INVALID_CHAIR)
	{
		//查找空位
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			const tagUserData * pUserData=GetUserInfo(i);
			WORD wUserId = m_wUserPost[i];
			//坐下按钮
			if (pUserData==NULL)
			{
				m_btSitDown[wUserId].ShowWindow(SW_SHOW);
			}
			else
			{
				m_btSitDown[wUserId].ShowWindow(SW_HIDE);
			}
		}
	}

	return;
}

//时间消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
	//消息处理
	switch (nIDEvent)
	{
	case IDI_GAME_END:		//动画变量
		{
			m_bGameEnd = !m_bGameEnd;
			UpdateGameView(NULL);

			return;
		}
	case IDI_USER_ACTION:	//筹码动作
		{
			bool bKillTime = true;

			//筹码移动
			if(m_bJettonAction && MoveJetton()) bKillTime = false;

			//筹码移动
			if(m_bCardAction && MoveCard()) bKillTime = false;

			//删除定时器
			if(bKillTime)
			{
				m_bJettonAction = false;
				m_bCardAction = false;
				KillTimer(IDI_USER_ACTION);
			}

			return;
		}
	case IDI_GAME_OVER:		//游戏结束
		{
			if(!m_bJettonAction && !m_bCardAction)
			{
				KillTimer(IDI_GAME_OVER);				
				AfxGetMainWnd()->SendMessage(IDM_GAME_OVER,0,0);
			}

			return;
		}
	case IDI_NO_SCORE:		//不够钱提示框
		{
			BYTE bCount = (BYTE)m_CenterCardControl.GetCardCount();
			if(bCount==MAX_CENTERCOUNT)
			{
				KillTimer(IDI_NO_SCORE);
				m_Prompt.ShowWindow(SW_SHOW);
			}

			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//坐下信息
	if(LOWORD(wParam)>=IDC_SIT_DOWN && LOWORD(wParam)<=(IDC_SIT_DOWN+GAME_PLAYER-1))
	{
		//发送消息
		AfxGetMainWnd()->SendMessage(IDM_SIT_DOWN,LOWORD(wParam)-IDC_SIT_DOWN,0);
		return TRUE;
	}

	// TODO: Add your specialized code here and/or call the base class
	switch (LOWORD(wParam))
	{
	case IDC_START:				//开始按钮
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_START,0,0);
			return TRUE;
		}
	case IDC_EXIT:				//离开按纽
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_EXIT,0,0);
			return TRUE;		
		}
	case IDC_GIVEUP:
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_GIVEUP,0,0);
			return TRUE;
		}
	case IDC_ADD:
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_ADD_SCORE,0,0);
			return TRUE;
		}
	case IDC_PASS:
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_PASS_CARD,0,0);
			return TRUE;
		}
	case IDC_FOLLOW:
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_FOLLOW,0,0);
			return TRUE;
		}
	case IDC_SHOWHAND:
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_SHOWHAND,0,0);
			return TRUE;
		}
	case IDC_AUTO_START:
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_AUTO_START,0,0);
			return TRUE;
		}
	case IDC_OPEN_CARD:
		{
			//发送消息
			AfxGetMainWnd()->SendMessage(IDM_OPEN_CARD,0,0);
			return TRUE;
		}

	}

	return __super::OnCommand(wParam, lParam);
}

void CGameClientView::SetShowUserView(BYTE bShow)
{
	m_bShowUser=bShow;

	//更新界面
	UpdateGameView(NULL);

	return ;
}

//设置位置
void CGameClientView::SetMyLookOn(WORD wIsLookOnId)
{
	m_wMyIsLookOn=wIsLookOnId;

	//更新界面
	UpdateGameView(NULL);

	return;
}

void CGameClientView::SetDFlag(WORD wDUser)
{
	//设置数据
	m_wDUser = wDUser;

	//更新界面
	UpdateGameView(NULL);
	return;
}

//设置下注
void CGameClientView::SetUserTableScore(WORD wChairID, LONG lTableScore)
{
	//设置数据
	if (wChairID!=INVALID_CHAIR) 
	{
		m_lTableScore[wChairID]=lTableScore;
		m_GoldView[wChairID].SetGold(lTableScore);
	}
	else
	{
		ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
		for (WORD i =0;i<GAME_PLAYER;i++)
		{
			m_GoldView[i].SetGold(0L);
		}
	}

	//更新界面
	UpdateGameView(NULL);

	return;
}

//设置总筹码
void CGameClientView::SetTotalScore(WORD wChairID, LONG lTotalScore)
{
	ASSERT(wChairID>=0 && wChairID <GAME_PLAYER);
	m_lTotalScore[wChairID] = lTotalScore;

	//更新界面
	UpdateGameView(NULL);
	return;

}

//筹码信息
void CGameClientView::SetTitleInfo(LONG lTurnLessScore, LONG lTurnMaxScore)
{
	//设置变量
	m_lTurnLessScore = lTurnLessScore;
	m_lTurnMaxScore = lTurnMaxScore;

	//更新界面
	UpdateGameView(NULL);

	return ;
}

//中心筹码
void CGameClientView::SetCenterScore(LONG lCenterScore)
{
	//设置数据
	m_lCenterScore = lCenterScore;
	m_CenterGoldView.SetGold(lCenterScore);

	//更新变量
	UpdateGameView(NULL);

	return;

}

//设置结束
void CGameClientView::SetGameEndStart()
{
	SetTimer(IDI_GAME_END,400,NULL);
	return;
}

void CGameClientView::SetGameEndEnd()
{
	KillTimer(IDI_GAME_END);
	return;
}

//绘制动画, 0: 底注动画, 1: 加注动画, 2: 加注合并到底注, 3: 赢家收筹码
void CGameClientView::DrawMoveAnte( WORD wViewChairID, int iMoveType,LONG lTableScore)
{
	//动画步数
	int nAnimeStep = 60;		

	m_JettonStatus[wViewChairID].wMoveIndex = 0;
	m_JettonStatus[wViewChairID].iMoveType = iMoveType;
	m_MoveGoldView[wViewChairID].SetGold(lTableScore);

	switch( iMoveType )
	{
	case AA_BASEFROM_TO_BASEDEST:	// 底注筹码下注
		m_JettonStatus[wViewChairID].ptFrom = m_ptCard[wViewChairID];
		m_JettonStatus[wViewChairID].ptDest = m_ptJetton[wViewChairID];
		m_JettonStatus[wViewChairID].ptCourse= m_ptCard[wViewChairID];
		m_JettonStatus[wViewChairID].lGold =lTableScore;
		break;

	case AA_CENTER_TO_BASEFROM:		// 中间筹码移至下注
		m_JettonStatus[wViewChairID].ptFrom = m_ptCenterJetton;
		m_JettonStatus[wViewChairID].ptDest = m_ptJetton[wViewChairID];
		m_JettonStatus[wViewChairID].ptCourse =m_ptCenterJetton;
		m_JettonStatus[wViewChairID].lGold =lTableScore;
		break;

	case AA_BASEDEST_TO_CENTER:		// 加注合并到中间
		m_JettonStatus[wViewChairID].ptFrom = m_ptJetton[wViewChairID];
		m_JettonStatus[wViewChairID].ptDest = m_ptCenterJetton;
		m_JettonStatus[wViewChairID].ptCourse= m_ptJetton[wViewChairID];
		m_JettonStatus[wViewChairID].lGold =lTableScore;
		break;

	default:
		ASSERT(FALSE);
	}

	//位移计算
	INT nXCount=abs(m_JettonStatus[wViewChairID].ptDest.x-m_JettonStatus[wViewChairID].ptFrom.x)/nAnimeStep+rand()%8;
	INT nYCount=abs(m_JettonStatus[wViewChairID].ptDest.y-m_JettonStatus[wViewChairID].ptFrom.y)/nAnimeStep+rand()%8;
	m_JettonStatus[wViewChairID].wMoveCount = __max(1,__max(nXCount,nYCount));;

	//设置时间
	if(!m_bJettonAction)
	{
		m_bJettonAction = true;
		SetTimer(IDI_USER_ACTION,TIME_USER_ACTION,NULL);
	}

	return;
}

//移动筹码
bool CGameClientView::MoveJetton()
{
	bool bAllClean = true;

	//设置变量
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		//移动步数
		if(m_JettonStatus[i].wMoveIndex<m_JettonStatus[i].wMoveCount)
		{
			bAllClean = false;
			m_JettonStatus[i].wMoveIndex++;
			WORD wMoveIndex = m_JettonStatus[i].wMoveIndex;
			WORD wMoveCount = m_JettonStatus[i].wMoveCount;
			m_JettonStatus[i].ptCourse.x =m_JettonStatus[i].ptFrom.x + (m_JettonStatus[i].ptDest.x-m_JettonStatus[i].ptFrom.x)*wMoveIndex/wMoveCount;
			m_JettonStatus[i].ptCourse.y =m_JettonStatus[i].ptFrom.y + (m_JettonStatus[i].ptDest.y-m_JettonStatus[i].ptFrom.y)*wMoveIndex/wMoveCount;
		}
		else if(m_JettonStatus[i].wMoveCount>0)
		{
			//筹码处理
			switch( m_JettonStatus[i].iMoveType )
			{
			case AA_BASEFROM_TO_BASEDEST:	// 底注筹码下注
				m_lTableScore[i] += m_JettonStatus[i].lGold ;
				m_GoldView[i].SetGold(m_lTableScore[i]);
				break;

			case AA_BASEDEST_TO_CENTER:		// 加注合并到中间
				m_lCenterScore += m_JettonStatus[i].lGold;
				m_CenterGoldView.SetGold(m_lCenterScore);
				break;

			case AA_CENTER_TO_BASEFROM:		// 中间筹码移至下注
				m_CenterGoldView.SetGold(0);
				break;

			default:
				ASSERT(FALSE);
			}

			//清理信息
			m_MoveGoldView[i].SetGold(0);
			ZeroMemory(&m_JettonStatus[i],sizeof(m_JettonStatus[i]));
		}
	}

	if(bAllClean)
	{
		m_bJettonAction = false;
	}

	//更新界面
	UpdateGameView(NULL);

	return !bAllClean;
}

//移动扑克
void CGameClientView::DrawMoveCard( WORD wViewChairID,int iMoveType,BYTE bCard)
{
	//动画步数
	int nAnimeStep = 45;

	tagCardStatus CardStatus;
	ZeroMemory(&CardStatus,sizeof(CardStatus));

	CardStatus.wMoveIndex = 0;
	CardStatus.iMoveType = iMoveType;
	CardStatus.bCard = bCard;

	switch( iMoveType )
	{
	case TO_USERCARD:				//用户扑克
		CardStatus.ptFrom = m_SendCardPos;
		m_SmallCardControl[wViewChairID].GetOriginPoint(CardStatus.ptDest);
		CardStatus.ptCourse= m_SendCardPos;
		break;

	case TO_GIVEUP_CARD:			//回收扑克
		m_SmallCardControl[wViewChairID].GetOriginPoint(CardStatus.ptFrom);
		CardStatus.ptDest = m_SendCardPos;
		CardStatus.ptCourse =CardStatus.ptFrom;
		break;

	case TO_CENTER_CARD:			//中心扑克
		{
			m_bCenterCount++;
			CImageHandle obj(&m_ImageCard);
			CSize size(m_ImageCard.GetWidth()/13,m_ImageCard.GetHeight()/5);
			INT iTemp = size.cx*(m_bCenterCount-1)+25;

			CardStatus.ptDest.x = iTemp+m_ptCenterCard.x;
			CardStatus.ptDest.y = m_ptCenterCard.y;
			CardStatus.ptFrom = m_SendCardPos;
			CardStatus.ptCourse = m_SendCardPos;

			//声音效果
			CGameClientDlg * pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
			pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
		}
		break;

	default:
		ASSERT(FALSE);
	}

	//位移计算
	INT nXCount=abs(CardStatus.ptDest.x-CardStatus.ptFrom.x)/nAnimeStep+rand()%4;
	INT nYCount=abs(CardStatus.ptDest.y-CardStatus.ptFrom.y)/nAnimeStep+rand()%4;
	CardStatus.wMoveCount = __max(1,__max(nXCount,nYCount));

	//加载数据
	m_CardStatus[wViewChairID].Add(CardStatus);

	//设置时间
	if(!m_bJettonAction)
	{
		m_bJettonAction=true;
		SetTimer(IDI_USER_ACTION,TIME_USER_ACTION,NULL);
	}
	if(!m_bCardAction)m_bCardAction=true;
}

//移动扑克
bool CGameClientView::MoveCard()
{
	bool bAllClean = true;

	//设置变量
	for(BYTE i=0;i<GAME_PLAYER+1;i++)
	{
		if(m_CardStatus[i].GetCount()==0)continue;

		tagCardStatus * pCardStatus = &(m_CardStatus[i].GetAt(0));

		//移动步数
		if(pCardStatus->wMoveIndex<pCardStatus->wMoveCount)
		{
			bAllClean = false;
			pCardStatus->wMoveIndex++;
			WORD wMoveIndex = pCardStatus->wMoveIndex;
			WORD wMoveCount = pCardStatus->wMoveCount;
			pCardStatus->ptCourse.x =pCardStatus->ptFrom.x + (pCardStatus->ptDest.x-pCardStatus->ptFrom.x)*wMoveIndex/wMoveCount;
			pCardStatus->ptCourse.y =pCardStatus->ptFrom.y + (pCardStatus->ptDest.y-pCardStatus->ptFrom.y)*wMoveIndex/wMoveCount;
		}
		else if(pCardStatus->wMoveCount>0)
		{
			//扑克处理
			switch( pCardStatus->iMoveType )
			{
			case TO_USERCARD:			//用户扑克
				{
					//扑克数目
					BYTE bCount = (BYTE)m_SmallCardControl[i].GetCardCount();
					ASSERT(bCount<2);

					//数据变量
					BYTE bTempCard[MAX_COUNT];
					ZeroMemory(bTempCard,sizeof(bTempCard));

					//设置控件
					if(pCardStatus->bCard>0) 
					{
						m_CardControl[i].GetCardData(bTempCard,bCount);
						bTempCard[bCount] = pCardStatus->bCard;
						bCount++;
						m_CardControl[i].SetCardData(bTempCard,bCount);
					}
					bCount = (BYTE)m_SmallCardControl[i].GetCardCount()+1;
					m_SmallCardControl[i].SetCardData(bCount);

					//清理信息
					m_CardStatus[i].RemoveAt(0);

					//控件处理
					if(m_SmallCardControl[m_wMeChairID].GetCardCount()==2 && m_btGiveUp.IsWindowVisible()==FALSE)
					{
						AfxGetMainWnd()->PostMessage(IDM_SEND_FINISH,0,0);
					}

					//声音效果
					if(m_CardStatus[i].GetCount()>0)
					{
						CGameClientDlg * pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
						pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
					}
				}

				break;

			case TO_GIVEUP_CARD:		//回收扑克

				//清理信息
				m_CardStatus[i].RemoveAt(0);

				break;

			case TO_CENTER_CARD:		//中心扑克
				{
					//扑克数目
					BYTE bCount = (BYTE)m_CenterCardControl.GetCardCount();
					ASSERT(bCount<=MAX_CENTERCOUNT);

					//
					BYTE bTempCard[MAX_CENTERCOUNT];
					ZeroMemory(bTempCard,sizeof(bTempCard));

					//
					ASSERT(pCardStatus->bCard>0);

					if(bCount>0)m_CenterCardControl.GetCardData(bTempCard,bCount);
					bTempCard[bCount] = pCardStatus->bCard;
					bCount++;
					m_CenterCardControl.SetCardData(bTempCard,bCount);

					//控件处理
					if(bCount>=3)
					{
						AfxGetMainWnd()->PostMessage(IDM_SEND_FINISH,0,0);
					}

					//清理信息
					m_CardStatus[i].RemoveAt(0);

					//声音效果
					if(m_CardStatus[i].GetCount()>0)
					{
						CGameClientDlg * pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
						pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
					}
				}

				break;
			}


			if(m_CardStatus[i].GetCount()>0)bAllClean=false;
			//ZeroMemory(pCardStatus,sizeof(tagCardStatus));
		}
	}

	if(bAllClean)
	{
		m_bCardAction = false;
	}

	//更新界面
	UpdateGameView(NULL);

	return !bAllClean;
}

//整性变字符
LPCTSTR CGameClientView::GetGlodString(LONG lGold, TCHAR szString[])
{
	LONG lTemp=(LONG)lGold; 
	if(lGold<0L) return szString;

	//处理小数点
	DWORD dwCharBit=0L;
	//lGold+=0.001;
	//if(lGold-lTemp>0)
	//{
	//	lTemp = (LONG)((lGold-lTemp)*100);

	//	bool bZero=(lTemp<10)?true:false;

	//	//转换字符
	//	do
	//	{
	//		szString[dwCharBit++]=(TCHAR)(lTemp%10+TEXT('0'));
	//		lTemp/=10;
	//	} while (lTemp>0L);

	//	//加0位
	//	if(bZero)szString[dwCharBit++]=TEXT('0');

	//	szString[dwCharBit++]=TEXT('.');
	//}

	//转换字符
	lTemp = (LONG)lGold; 
	DWORD dwNumBit=0L;
	do
	{
		dwNumBit++;
		szString[dwCharBit++]=(TCHAR)(lTemp%10+TEXT('0'));
		if (dwNumBit%3==0) szString[dwCharBit++]=TEXT(',');
		lTemp/=10;
	} while (lTemp>0L);

	//调整字符
	if (szString[dwCharBit-1]==TEXT(',')) szString[dwCharBit-1]=0;
	szString[dwCharBit]=0;

	//尾头交换
	_tcsrev(szString);

	return szString;
}

//物理位置
void CGameClientView::SetUserPost(WORD wPhysicsPost,WORD wViewPost)
{
	//设置变量
	m_wUserPost[wViewPost] = wPhysicsPost;

	return ;
}
//////////////////////////////////////////////////////////////////////////