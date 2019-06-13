#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////
//按钮标识 

#define IDC_START						100								//开始按钮
#define IDC_ONE_SCORE					101								//1 分按钮
#define IDC_TWO_SCORE					102								//2 分按钮
#define IDC_THREE_SCORE					103								//3 分按钮
#define IDC_GIVE_UP_SCORE				104								//放弃按钮
#define IDC_OUT_CARD					105								//出牌按钮
#define IDC_PASS_CARD					106								//PASS按钮
#define IDC_AUTO_OUTCARD                107                             //提示按纽
#define IDC_AUTOPLAY_ON					108								//托管按钮
#define IDC_AUTOPLAY_OFF				109								//取消按钮
#define IDC_SORT_CARD			        110                             //托管按纽

#define IDC_SNATCH_LAND					111								//抢庄按钮
#define IDC_BRIGHT_CARD					112								//明牌按钮
#define IDC_BRIGHT_START				113								//明牌开始
#define IDC_DOUBLE_SCORE				114								//加倍消息
#define IDC_NOT_DOUBLE_SCORE			115								//加倍消息

//爆炸数目
#define BOMB_EFFECT_COUNT				6								//爆炸数目

//爆炸标识
#define IDI_BOMB_EFFECT					101								//爆炸标识
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_PASS_CARD, OnPassCard)
	ON_BN_CLICKED(IDC_ONE_SCORE, OnOneScore)
	ON_BN_CLICKED(IDC_TWO_SCORE, OnTwoScore)
	ON_BN_CLICKED(IDC_THREE_SCORE, OnThreeScore)
	ON_BN_CLICKED(IDC_AUTO_OUTCARD, OnAutoOutCard)
	ON_BN_CLICKED(IDC_GIVE_UP_SCORE, OnGiveUpScore)
	ON_BN_CLICKED(IDC_AUTOPLAY_ON, OnAutoPlayerOn)
	ON_BN_CLICKED(IDC_AUTOPLAY_OFF,OnAutoPlayerOff)
	ON_BN_CLICKED(IDC_SORT_CARD, OnBnClickedSortCard)
	ON_BN_CLICKED(IDC_SNATCH_LAND, OnSnatchLand)
	ON_BN_CLICKED(IDC_BRIGHT_CARD, OnBrightCard)
	ON_BN_CLICKED(IDC_BRIGHT_START, OnBrightStart)
	ON_BN_CLICKED(IDC_DOUBLE_SCORE, OnDoubleScore)
	ON_BN_CLICKED(IDC_NOT_DOUBLE_SCORE, OnNotDoubleScore)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//游戏变量
	m_lBaseScore=0;
	m_wBombTime=1;
	m_cbLandScore=0;
	m_wLandUser=INVALID_CHAIR;

	//状态变量
	m_bLandTitle=false;
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bScore,0,sizeof(m_bScore));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );
	for(BYTE i=0; i<GAME_PLAYER; ++i) m_bShowWarming[i] = false ;

	//疯狂变量
	m_cbCallScorePhase = CSD_NORMAL;
	ZeroMemory( m_cbUserPhase, sizeof( m_cbUserPhase ) );
	ZeroMemory( m_bBrightStart, sizeof( m_bBrightStart ) );
	m_cbBrightTime=0;
	m_bShowLandTime=false;
	ZeroMemory( m_bDoubleScore, sizeof( m_bDoubleScore ) );
        
	//爆炸动画
	m_bBombEffect=false;
	m_cbBombFrameIndex=0;

	//位置信息
	memset(m_ptScore,0,sizeof(m_ptScore));
	memset(m_ptLand,0,sizeof(m_ptLand));
	m_bDeasilOrder = false;

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBomb.SetLoadInfo(IDB_BOMB,hInstance);
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageCenter.SetLoadInfo(IDB_VIEW_CENTER,hInstance);
	m_ImageBombEffect.SetLoadInfo(IDB_BOMB_EFFECT,hInstance);

	//PNG资源
	m_ImageScore.LoadImage(hInstance, TEXT( "SCORE" ) );
	m_ImageLand.LoadImage(hInstance, TEXT( "BANKER" ) );
	m_ImageUserTrustee.LoadImage(hInstance,  TEXT("USER_TRUSTEE"));
	m_ImageOnecardWarming.LoadImage(hInstance, TEXT("ONE_CARD_WARMING"));
	m_ImageTwoCardWarming.LoadImage(hInstance, TEXT("TWO_CARD_WARMING"));
	m_ImageLandTimes.LoadImage(hInstance, TEXT("LAND_TIMES"));
    
	//计算位置
	m_LandSize.cx=m_ImageLand.GetWidth();
	m_LandSize.cy=m_ImageLand.GetHeight();

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

	//创建扑克
	for (WORD i=0;i<3;i++)
	{
		//用户扑克
		if ( i == 1 )
			m_UserCardControl[i].SetDirection(true);
		else
			m_UserCardControl[i].SetDirection(false);
			
		m_UserCardControl[i].SetDisplayFlag(true);	
		m_UserCardControl[i].SetCardSpace( 16, 20, 16 );

		m_UserCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,20+i);

		//用户扑克
		if (i!=1)
		{
			m_HandCardControl[i].SetCardSpace(0,18,0);
			m_HandCardControl[i].SetDirection(false);
			m_HandCardControl[i].SetDisplayFlag(false);
			m_HandCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,30+i);
		}
		else
		{
			m_HandCardControl[i].SetDirection(true);
			m_HandCardControl[i].SetDisplayFlag(false);
			m_HandCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,30+i);
		}
	}

	//设置扑克
	m_BackCardControl.SetCardSpace(85,0,0);
	m_BackCardControl.SetDisplayFlag(false);
	m_HandCardControl[1].SetSinkWindow(AfxGetMainWnd());
	m_BackCardControl.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,41);

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_START);
	m_btOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OUT_CARD);
	m_btPassCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_PASS_CARD);
	m_btOneScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_ONE_SCORE);
	m_btTwoScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_TWO_SCORE);
	m_btGiveUpScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_GIVE_UP_SCORE);
	m_btAutoOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_AUTO_OUTCARD);
	m_btThreeScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_THREE_SCORE);
	m_btAutoPlayOn.Create(TEXT(""), WS_CHILD ,CreateRect,this,IDC_AUTOPLAY_ON);
	m_btAutoPlayOff.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_AUTOPLAY_OFF);
	m_btSortCard.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,CreateRect,this,IDC_SORT_CARD);

	//扩展按钮
	m_btSnatchLand.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_SNATCH_LAND);
	m_btBrightCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_BRIGHT_CARD);	
	m_btBrightStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_BRIGHT_START);
	m_btDoubleScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_DOUBLE_SCORE);
	m_btNotDoubleScore.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_NOT_DOUBLE_SCORE);
    
	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_START,hInstance,false);
	m_btOutCard.SetButtonImage(IDB_OUT_CARD,hInstance,false);
	m_btPassCard.SetButtonImage(IDB_PASS,hInstance,false);
	m_btOneScore.SetButtonImage(IDB_ONE_SCORE,hInstance,false);
	m_btTwoScore.SetButtonImage(IDB_TWO_SCORE,hInstance,false);
	m_btGiveUpScore.SetButtonImage(IDB_GIVE_UP,hInstance,false);
	m_btAutoOutCard.SetButtonImage(IDB_AUTO_OUT_CARD,hInstance,false);
	m_btThreeScore.SetButtonImage(IDB_THREE_SCORE,hInstance,false);
	m_btAutoPlayOn.SetButtonImage  (IDB_AUTOPLAY_ON,hInstance,false);
	m_btAutoPlayOff.SetButtonImage (IDB_AUTOPLAY_OFF,hInstance,false);
	m_btSortCard.SetButtonImage(IDB_COUNT_SORT,hInstance,false);
	m_btSnatchLand.SetButtonImage(IDB_SNATCH_LAND,hInstance,false);
	m_btBrightCard.SetButtonImage(IDB_BRIGHT_CARD,hInstance,false);
	m_btBrightStart.SetButtonImage(IDB_BRIGHT_CARD_START, hInstance, false);
	m_btDoubleScore.SetButtonImage(IDB_DOUBLE_SCORE, hInstance, false);
	m_btNotDoubleScore.SetButtonImage(IDB_NOT_DOUBLE_SCORE, hInstance, false);
    
    m_btAutoPlayOn.ShowWindow(SW_SHOW);
    m_btAutoPlayOff.ShowWindow(SW_HIDE);

	//创建控件
	m_ScoreView.Create(IDD_GAME_SCORE,this);

	//隐藏扑克
	//m_HandCardControl[ 0 ].ShowWindow( SW_HIDE );
	//m_HandCardControl[ 2 ].ShowWindow( SW_HIDE );

#ifdef VIDEO_GAME

	//创建视频
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		//创建视频
		m_DlgVedioService[i].Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CreateRect,this,200+i);
		m_DlgVedioService[i].InitVideoService(i==1,i==1);

		//设置视频
		m_VedioServiceManager.SetVideoServiceControl(i,&m_DlgVedioService[i]);
	}	

#endif

	return 0;
}

//鼠标消息
void CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//收起扑克
		//设置扑克
	m_btOutCard.EnableWindow(FALSE);
	m_HandCardControl[1].ShootAllCard(false);

	return;
}

//鼠标双击
void CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);

	//设置扑克
	m_btOutCard.EnableWindow(FALSE);
	m_HandCardControl[1].ShootAllCard(false);

	return;
}

//重置界面
void CGameClientView::ResetGameView()
{
	//游戏变量
	m_lBaseScore=0;
	m_wBombTime=1;
	m_cbLandScore=0;
	m_wLandUser=INVALID_CHAIR;

	//疯狂变量
	ZeroMemory( m_cbUserPhase, sizeof( m_cbUserPhase ) );
	ZeroMemory( m_bBrightStart, sizeof( m_bBrightStart ) );
	m_cbBrightTime=0;
	m_bShowLandTime=false;
	ZeroMemory( m_bDoubleScore, sizeof( m_bDoubleScore ) );
                
	//爆炸动画
	m_bBombEffect=false;
	m_cbBombFrameIndex=0;

	//状态变量
	m_bLandTitle=false;
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bScore,0,sizeof(m_bScore));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );
	for(BYTE i=0; i<GAME_PLAYER; ++i) m_bShowWarming[i] = false ;
    
	//隐藏控件
	if(m_ScoreView.m_hWnd) m_ScoreView.ShowWindow(SW_HIDE);

	//隐藏按钮
	m_btStart.ShowWindow(SW_HIDE);
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btPassCard.ShowWindow(SW_HIDE);
	m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_btThreeScore.ShowWindow(SW_HIDE);
	m_btAutoPlayOn.ShowWindow( SW_SHOW );
	m_btAutoPlayOff.ShowWindow( SW_HIDE );

	m_btSnatchLand.ShowWindow(SW_HIDE);
	m_btBrightCard.ShowWindow(SW_HIDE);
	m_btBrightStart.ShowWindow(SW_HIDE);

	//禁用控件
	m_btOutCard.EnableWindow(FALSE);
	m_btPassCard.EnableWindow(FALSE);
	m_btAutoOutCard.EnableWindow(FALSE);
	m_btSortCard.EnableWindow(FALSE);
	m_btSortCard.SetButtonImage(IDB_COUNT_SORT,AfxGetInstanceHandle(),false);
	m_btGiveUpScore.SetButtonImage(IDB_GIVE_UP,AfxGetInstanceHandle(),false);

	//扑克控件
	m_BackCardControl.SetCardData(NULL,0);
	m_HandCardControl[1].SetPositively(false);
	m_HandCardControl[1].SetDisplayFlag(false);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_UserCardControl[i].SetCardData(NULL,0);
		m_HandCardControl[i].SetCardData(NULL,0);
	}

	//隐藏扑克
//	m_HandCardControl[ 0 ].ShowWindow( SW_HIDE );
//	m_HandCardControl[ 2 ].ShowWindow( SW_HIDE );

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{

	//顺时针出
	if ( m_bDeasilOrder ) {
		//设置坐标
		m_ptFace[2].x=m_nXBorder+5;
		m_ptFace[2].y=nHeight/2-m_nYFace;
		m_ptName[2].x=m_nXBorder+5;
		m_ptTimer[0].x=m_nXBorder+2+m_nXTimer/2;
		m_ptTimer[0].y=nHeight/2-m_nYFace-5-m_nYTimer/2;
		m_ptTimer[2].y=nHeight/2-m_nYFace-5-m_nYTimer;
		m_ptReady[2].x=m_nXBorder+5+m_nXTimer;
		m_ptReady[2].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
		m_ptScore[2].x=m_nXBorder+m_nXFace+147-10;
		m_ptScore[2].y=nHeight/2-m_nYFace-30;
		m_ptLand[2].x=m_nXBorder+5;
		m_ptLand[2].y=nHeight/2+40;
		m_ptWarmingCard[2].x = m_ptLand[2].x;
		m_ptWarmingCard[2].y = m_ptLand[2].y + 40;

		//设置坐标
		m_ptFace[0].x=nWidth-m_nXBorder-m_nXFace-5;
		m_ptFace[0].y=nHeight/2-m_nYFace;
		m_ptName[0].x=nWidth-m_nXBorder-5;
		m_ptName[0].y=nHeight/2+5;
		m_ptTimer[2].x=nWidth-m_nXBorder-m_nXTimer/2-2;
		m_ptTimer[2].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
		m_ptReady[0].x=nWidth-m_nXBorder-m_nXTimer-5;
		m_ptReady[0].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
		m_ptScore[0].x=nWidth-m_nXBorder-m_nXFace-190-7-m_ImageScore.GetWidth()/11;
		m_ptScore[0].y=nHeight/2-m_nYFace-30;
		m_ptLand[0].x=nWidth-m_nXBorder-m_LandSize.cx-5;
		m_ptLand[0].y=nHeight/2+40;
		m_ptWarmingCard[0].x = m_ptLand[0].x-40;
		m_ptWarmingCard[0].y =m_ptLand[0].y + 40;

		//用户扑克
		m_UserCardControl[0].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-137-100,nHeight/2-40,enXRight,enYCenter);
		m_UserCardControl[2].SetBenchmarkPos(m_nXBorder+m_nXFace+137+100,nHeight/2-40,enXLeft,enYCenter);
		m_HandCardControl[0].SetBenchmarkPos(nWidth-m_nXBorder-m_nXFace-57,nHeight/2-50,enXRight,enYCenter);
		m_HandCardControl[2].SetBenchmarkPos(m_nXBorder+m_nXFace+57,nHeight/2-55,enXLeft,enYCenter);
	}
	else {
		//设置坐标
		m_ptFace[0].x=m_nXBorder+5;
		m_ptFace[0].y=nHeight/2-m_nYFace;
		m_ptName[0].x=m_nXBorder+5;
		m_ptName[0].y=nHeight/2+5;
		m_ptTimer[0].x=m_nXBorder+2+m_nXTimer/2;
		m_ptTimer[0].y=nHeight/2-m_nYFace-5-m_nYTimer/2;
		m_ptReady[0].x=m_nXBorder+5+m_nXTimer;
		m_ptReady[0].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
		m_ptScore[0].x=m_nXBorder+m_nXFace+147-10;
		m_ptScore[0].y=nHeight/2-m_nYFace-30;
		m_ptLand[0].x=m_nXBorder+5;
		m_ptLand[0].y=nHeight/2+40;
		m_ptWarmingCard[0].x = m_ptLand[0].x;
		m_ptWarmingCard[0].y = m_ptLand[0].y + 40;

		//设置坐标
		m_ptFace[2].x=nWidth-m_nXBorder-m_nXFace-5;
		m_ptFace[2].y=nHeight/2-m_nYFace;
		m_ptName[2].x=nWidth-m_nXBorder-5;
		m_ptName[2].y=nHeight/2+5;
		m_ptTimer[2].x=nWidth-m_nXBorder-m_nXTimer/2-2;
		m_ptTimer[2].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
		m_ptReady[2].x=nWidth-m_nXBorder-m_nXTimer-5;
		m_ptReady[2].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
		m_ptScore[2].x=nWidth-m_nXBorder-m_nXFace-147+10-m_ImageScore.GetWidth()/11;
		m_ptScore[2].y=nHeight/2-m_nYFace-30;
		m_ptLand[2].x=nWidth-m_nXBorder-m_LandSize.cx-5;
		m_ptLand[2].y=nHeight/2+40;
		m_ptWarmingCard[2].x = m_ptLand[2].x-40;
		m_ptWarmingCard[2].y =m_ptLand[2].y + 40;

		//用户扑克
		m_UserCardControl[2].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-137-100,nHeight/2-40,enXRight,enYCenter);
		m_UserCardControl[0].SetBenchmarkPos(m_nXBorder+m_nXFace+137+100,nHeight/2-40,enXLeft,enYCenter);
		m_HandCardControl[2].SetBenchmarkPos(nWidth-m_nXBorder-m_nXFace-57,nHeight/2-50,enXRight,enYCenter);
		m_HandCardControl[0].SetBenchmarkPos(m_nXBorder+m_nXFace+57,nHeight/2-55,enXLeft,enYCenter);
	}
    
	////////////////////////////////////////////////////////////////////////////
	//BYTE cbCardData[ 20 ] = { 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01 };
	//
	//m_HandCardControl[ 1 ].SetCardData( cbCardData, sizeof( cbCardData ) ); 
	//m_HandCardControl[ 0 ].SetCardData( cbCardData, sizeof( cbCardData ) );
	//m_HandCardControl[ 2 ].SetCardData( cbCardData, sizeof( cbCardData ) );
	////m_UserCardControl[ 1 ].SetCardData( cbCardData, 8 ); 
	////m_UserCardControl[ 0 ].SetCardData( cbCardData, 8 );
	////m_UserCardControl[ 2 ].SetCardData( cbCardData, 8 );
	//for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID ) {
	//	m_bPass[ wChairID ] = true; 
	//	m_HandCardControl[ wChairID ].ShowWindow( SW_SHOW );
	//}
	//if ( m_ScoreView.m_hWnd == NULL ) m_ScoreView.Create(IDD_GAME_SCORE,this);
	//m_ScoreView.ShowWindow( SW_SHOW );
	//for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID ) {
	//	m_ScoreView.SetGameScore( wChairID, "dkfjdkfjdf", 00 );
	//}
	//////////////////////////////////////////////////////////////////////////////

#ifdef VIDEO_GAME
	CRect VDRect;
	m_DlgVedioService[ 0 ].GetWindowRect( &VDRect );
	if ( m_bDeasilOrder ) {
		m_DlgVedioService[ 2 ].MoveWindow( m_nXBorder + 5, m_ptTimer[ 0 ].y - VDRect.Height() - m_nYTimer, VDRect.Width(), VDRect.Height() );
		m_DlgVedioService[ 0 ].MoveWindow( nWidth - VDRect.Width() - m_nXBorder - 5, m_ptTimer[ 2 ].y - VDRect.Height() - m_nYTimer, VDRect.Width(), VDRect.Height() );

		m_HandCardControl[0].SetBenchmarkPos(nWidth-m_nXBorder-m_nXFace-147,nHeight/2-50,enXRight,enYCenter);
		m_HandCardControl[2].SetBenchmarkPos(m_nXBorder+m_nXFace+147,nHeight/2-55,enXLeft,enYCenter);

		m_ptScore[2].x=m_nXBorder+m_nXFace+147-10+100;
		m_ptScore[2].y=nHeight/2-m_nYFace-30;

		m_ptScore[0].x=nWidth-m_nXBorder-m_nXFace-190-37-120;
		m_ptScore[0].y=nHeight/2-m_nYFace-30;
	
		m_ptWarmingCard[0].x = nWidth-m_nXBorder-180;
		m_ptWarmingCard[0].y = m_ptName[0].y - 30 ;
		m_ptWarmingCard[2].x = m_nXBorder+100;
		m_ptWarmingCard[2].y = m_ptName[2].y - 30 ;
	}
	else {
		m_DlgVedioService[ 0 ].MoveWindow( m_nXBorder + 5, m_ptTimer[ 0 ].y - VDRect.Height() - m_nYTimer, VDRect.Width(), VDRect.Height() );
		m_DlgVedioService[ 2 ].MoveWindow( nWidth - VDRect.Width() - m_nXBorder - 5, m_ptTimer[ 2 ].y - VDRect.Height() - m_nYTimer, VDRect.Width(), VDRect.Height() );

		m_HandCardControl[2].SetBenchmarkPos(nWidth-m_nXBorder-m_nXFace-147,nHeight/2-50,enXRight,enYCenter);
		m_HandCardControl[0].SetBenchmarkPos(m_nXBorder+m_nXFace+147,nHeight/2-55,enXLeft,enYCenter);

		m_ptScore[0].x=m_nXBorder+m_nXFace+147-10+100;
		m_ptScore[0].y=nHeight/2-m_nYFace-30;

		m_ptScore[2].x=nWidth-m_nXBorder-m_nXFace-190-37-120;
		m_ptScore[2].y=nHeight/2-m_nYFace-30;

		m_ptWarmingCard[2].x = nWidth-m_nXBorder-180;
		m_ptWarmingCard[2].y = m_ptName[2].y - 30 ;
		m_ptWarmingCard[0].x = m_nXBorder+100;
		m_ptWarmingCard[0].y = m_ptName[0].y - 30 ;
	}

	m_DlgVedioService[ 1 ].GetWindowRect( &VDRect );
	m_DlgVedioService[ 1 ].MoveWindow( m_nXBorder + 5, nHeight - VDRect.Height() - m_nYBorder - 5, VDRect.Width(), VDRect.Height() );
#endif
    

	//设置坐标
	m_ptFace[1].x=nWidth/2-m_nXFace/2;
	m_ptFace[1].y=nHeight-m_nYBorder-m_nYFace-5;
	m_ptName[1].x=nWidth/2+5+m_nXFace/2;
	m_ptName[1].y=nHeight-m_nYBorder-m_nYFace-2;
	m_ptTimer[1].x=nWidth/2-m_nXFace/2-m_nXTimer/2-5;
	m_ptTimer[1].y=nHeight-m_nYBorder-m_nYTimer/2-2;
	m_ptReady[1].x=nWidth/2 + 10;
	m_ptReady[1].y=nHeight-217-m_nYBorder;
	m_ptScore[1].x=nWidth/2-21;
	m_ptScore[1].y=nHeight-m_nYBorder-m_nYFace-210;
	m_ptLand[1].x=nWidth/2-m_nXFace/2-m_LandSize.cx-m_nXTimer*2;
	m_ptLand[1].y=nHeight-m_nXBorder-m_LandSize.cy-5;

	m_ptBrightStart[0] = m_ptScore[0];
	m_ptBrightStart[1] = m_ptScore[1];
	m_ptBrightStart[2] = m_ptScore[2];
	m_ptBrightStart[1].y -= 35;
	m_ptBrightStart[1].x -= 20;

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	m_btStart.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2-rcButton.Width(),nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcButton.Width()*3/2-10,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAutoOutCard,NULL,nWidth/2-rcButton.Width()/2,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2+rcButton.Width()/2+10,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOneScore,NULL,nWidth/2-rcButton.Width()*2-12,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btTwoScore,NULL,nWidth/2-rcButton.Width()-2,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btThreeScore,NULL,nWidth/2-rcButton.Width()*2-12,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUpScore,NULL,nWidth/2+30+rcButton.Width()-10,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btSortCard,NULL,nWidth-m_nXBorder-rcButton.Width()-15,nHeight-rcButton.Height()*2-m_nYBorder-40,0,0,uFlags);

	//扩展按钮
	DeferWindowPos(hDwp,m_btSnatchLand,NULL,nWidth/2-rcButton.Width()*2-30,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btBrightCard,NULL,nWidth/2-rcButton.Width()*2-30,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btBrightStart,NULL,(nWidth-rcButton.Width())/2+10,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btDoubleScore,NULL,nWidth/2-rcButton.Width()*2-12,nHeight-217-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btNotDoubleScore,NULL,nWidth/2+30+rcButton.Width()-10,nHeight-217-m_nYBorder,0,0,uFlags);

	EndDeferWindowPos(hDwp);

	CRect rcBtAutoPlay;
	m_btAutoPlayOn.GetWindowRect( rcBtAutoPlay );
	m_btAutoPlayOn .MoveWindow( nWidth - m_nXBorder - 5 - rcBtAutoPlay.Width() ,     nHeight-50, rcBtAutoPlay.Width(), rcBtAutoPlay.Height(), FALSE);
    m_btAutoPlayOff.MoveWindow( nWidth - m_nXBorder - 5 - rcBtAutoPlay.Width(),     nHeight-50, rcBtAutoPlay.Width(), rcBtAutoPlay.Height(), FALSE);
	m_btSortCard.MoveWindow( nWidth - m_nXBorder - 5 - rcBtAutoPlay.Width(), nHeight-55-rcBtAutoPlay.Height(), rcBtAutoPlay.Width(), rcBtAutoPlay.Height(), FALSE);

	//移动扑克
	m_BackCardControl.SetBenchmarkPos(nWidth/2,m_nYBorder+10,enXCenter,enYTop);
	m_HandCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-m_nYFace-m_nYBorder-25,enXCenter,enYBottom);
	m_UserCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-223-m_nYBorder,enXCenter,enYBottom);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBack,enMode_Spread);
	DrawViewImage(pDC,m_ImageCenter,enMode_Centent);	

	//绘画用户
	TCHAR szBuffer[32];
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer=GetUserTimer(i);
		const tagUserData * pUserData=GetUserInfo(i);

		/////////////////////////////////////////////////////

		//tagUserData *pUserData = new tagUserData;
		//strcpy( (char*)pUserData->szName, "dkjfdkfjdkfj" );
		//pUserData->cbUserStatus = US_READY;
		//wUserTimer = 10;
		//m_ImageLand.DrawImage(pDC,m_ptLand[i].x,m_ptLand[i].y);
		//m_wBombTime = 2;
		//m_bShowWarming[i]=true;
		//m_bCardCount[i] = 2;
		//m_cbCallScorePhase=5;
		//m_bScore[i] = 3;
		//m_bPass[i]=false;

		//m_wLandUser = 2;
		//m_wBombTime=4;
		///////////////////////////////////////////////////////

		//绘画用户
		if (pUserData!=NULL)
		{
			//用户名字
			pDC->SetTextAlign( ( ( i == 2 && ! m_bDeasilOrder ) || ( i == 0 && m_bDeasilOrder ) ) ? TA_RIGHT : TA_LEFT );
			pDC->SetTextColor((wUserTimer>0)?RGB(250,250,250):RGB(220,220,220));
			DrawTextString( pDC, pUserData->szName, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), m_ptName[i].x,m_ptName[i].y );

			//剩余扑克
		//	if (i!=1)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("剩余：%d 张"),m_bCardCount[i]);
				DrawTextString( pDC, szBuffer, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), m_ptName[i].x,m_ptName[i].y+18 );
			}

			//其他信息
			if (wUserTimer!=0) DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);

			//报牌信息
			if(m_bShowWarming[i])
			{
				//创建资源
				CFont InfoFont;
				InfoFont.CreateFont(-40,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));

				//设置 DC
				pDC->SetTextColor(RGB(255,0,0));
				pDC->SetTextAlign(TA_CENTER|TA_TOP);
				CFont * pOldFont=pDC->SelectObject(&InfoFont);

				//绘画信息
				if ( i == 0 || i == 2)
				{
					if ( m_bCardCount[i] == 1)
						m_ImageOnecardWarming.DrawImage(pDC, m_ptWarmingCard[i].x,  m_ptWarmingCard[i].y);
					else if(m_bCardCount[i] == 2 )
						m_ImageTwoCardWarming.DrawImage(pDC, m_ptWarmingCard[i].x,  m_ptWarmingCard[i].y);
				}					


				//清理资源
				pDC->SelectObject(pOldFont);
				InfoFont.DeleteObject();			

			}

			//亮牌信息
			if ( m_bBrightStart[i] )
			{
				BYTE const SECTION_COUNT = 11; 
				INT nInfoWidth = m_ImageScore.GetWidth() / SECTION_COUNT;
				INT nInfoHeight = m_ImageScore.GetHeight();
				INT nXPos = nInfoWidth * 8 ;
				m_ImageScore.DrawImage(pDC, m_ptBrightStart[i].x, m_ptBrightStart[i].y, nInfoWidth, nInfoHeight, nXPos, 0 );
			}
		}
	}

	//显示倍数
	//if ( m_cbCallScorePhase >= CSD_SNATCHLAND && m_cbLandScore > 1 )
	//{
	//	//位置信息
	//	INT const LAND_TIMES_COUNT = 5;
	//	INT nXPos = 0;
	//	INT nNumWidth = m_ImageLandTimes.GetWidth() / LAND_TIMES_COUNT;
	//	INT nNumHeight = m_ImageLandTimes.GetHeight();
	//	if ( m_cbLandScore == 2 ) nXPos = 0;
	//	else if ( m_cbLandScore == 3 ) nXPos = nNumWidth;
	//	else if ( m_cbLandScore == 4 ) nXPos = 2 * nNumWidth;
	//	else if ( m_cbLandScore == 5 ) nXPos = 3 * nNumWidth;

	//	INT nSpace = 10;
	//	INT nXStartPos = ( nWidth - ( 2 * nNumWidth - nSpace ) ) / 2;
	//	INT nYStartPos = 250;

	//	//绘画图片
	//	m_ImageLandTimes.DrawImage( pDC, nXStartPos, nYStartPos, nNumWidth, nNumHeight, nNumWidth * 4, 0 );
	//	m_ImageLandTimes.DrawImage( pDC, nXStartPos+nNumWidth-nSpace, nYStartPos, nNumWidth, nNumHeight, nXPos, 0 );
	//}

	//叫分标志
	int nXImagePos,nImageWidth=m_ImageScore.GetWidth(),nImageHeight=m_ImageScore.GetHeight();
	for (WORD i=0;i<3;i++)
	{
		if ((m_bScore[i]!=0)||(m_bPass[i]==true) || m_bDoubleScore[i]==true)
		{
			BYTE const SECTION_COUNT = 11; 
			if ( m_bPass[i]==true ) nXImagePos=m_ImageScore.GetWidth()*4/SECTION_COUNT;
			else if ( m_bScore[i]==255 && m_cbUserPhase[i] == CSD_SNATCHLAND ) nXImagePos=m_ImageScore.GetWidth()*6/SECTION_COUNT;
			else if ( m_bScore[i]==255 && m_cbUserPhase[i] == CSD_BRIGHTCARD ) nXImagePos=m_ImageScore.GetWidth()*10/SECTION_COUNT;
			else if ( m_bScore[i]==255 ) nXImagePos=m_ImageScore.GetWidth()*3/SECTION_COUNT;
			else if (m_bScore[i]==4) nXImagePos=7*m_ImageScore.GetWidth()/SECTION_COUNT;
			else if ( m_bDoubleScore[i]==true ) nXImagePos=5*m_ImageScore.GetWidth()/SECTION_COUNT;
			else nXImagePos=9*m_ImageScore.GetWidth()/SECTION_COUNT;
			m_ImageScore.DrawImage(pDC,m_ptScore[i].x,m_ptScore[i].y,nImageWidth/SECTION_COUNT,nImageHeight,nXImagePos,0);
		}
	}

	//庄家位置
	if (m_wLandUser!=INVALID_CHAIR && m_cbCallScorePhase == CSD_GAMESTART)
	{
		//获取用户
		const tagUserData * pUserData=GetUserInfo(m_wLandUser);
		if (pUserData!=NULL)
		{
			//变量定义
			TCHAR szBuffer[64];
			INT nXStartPos = m_nXFace + m_nXBorder + 15;
			INT nYStartPos = m_nYBorder+10;
			INT nSpace = 20;

			//设置DC
			pDC->SetTextAlign( DT_LEFT );

			//绘画信息
			m_ImageLand.DrawImage(pDC,m_ptLand[m_wLandUser].x,m_ptLand[m_wLandUser].y);
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("地主：%s"),pUserData->szName);
			DrawTextString( pDC, szBuffer, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), nXStartPos,nYStartPos );

			//绘画头像
			DrawUserFace(pDC,pUserData,m_nXBorder+5,m_nYBorder+5,false);

			//叫牌分数
			LONG lScore=m_cbLandScore*m_lBaseScore;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("底分：%ld "),m_lBaseScore);
			DrawTextString( pDC, szBuffer, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), nXStartPos,nYStartPos + nSpace  );

			//翻倍计算
			WORD wDrawTimes=(WORD)m_lBaseScore;
			for (WORD i=1;i<m_cbLandScore;i++) wDrawTimes*=2;

			//总共倍数
			WORD wLandTime = 0;
			if ( m_cbBrightTime != 0 && wDrawTimes == 0 ) wLandTime = m_cbBrightTime;
			else if ( m_cbBrightTime == 0 && wDrawTimes != 0 ) wLandTime = wDrawTimes;
			else if ( m_cbBrightTime != 0 && wDrawTimes != 0 ) wLandTime = m_cbBrightTime * wDrawTimes;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("倍数：%ld"),wLandTime );
			DrawTextString( pDC, szBuffer, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), nXStartPos,nYStartPos + nSpace * 2 );

			//炸弹倍数
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("炸弹翻倍：%d"),m_wBombTime);
			DrawTextString( pDC, szBuffer, RGB( 255, 255, 255 ), RGB( 0, 0, 0 ), nXStartPos,nYStartPos + nSpace * 3);
		}
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

	//描述信息
	if (m_bLandTitle==true)
	{
		//创建资源
		CFont InfoFont;
		InfoFont.CreateFont(-36,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));

		//设置 DC
		pDC->SetTextColor(RGB(201,229,133));
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		CFont * pOldFont=pDC->SelectObject(&InfoFont);

		//绘画信息
		pDC->TextOut(nWidth/2,200,"正等待用户叫分");

		//清理资源
		pDC->SelectObject(pOldFont);
		InfoFont.DeleteObject();
	}

	//托管标识
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID ) {
		
		const tagUserData * pUserData=GetUserInfo( wChairID );
		if ( pUserData == NULL ) m_bUserTrustee[ wChairID ] = false;

		if ( m_bUserTrustee[ wChairID ] ) {
			if ( wChairID == 1 )
				m_ImageUserTrustee.DrawImage( pDC, m_ptLand[ wChairID ].x + 40, m_ptLand[ wChairID ].y - 1 );
			else if ( wChairID == 0 && !m_bDeasilOrder || wChairID == 2 && m_bDeasilOrder )
				m_ImageUserTrustee.DrawImage( pDC, m_ptFace[ wChairID ].x + m_nXFace + 8, m_ptFace[ wChairID ].y + m_nYFace -
				m_ImageUserTrustee.GetHeight() + 3 );
			else 
				m_ImageUserTrustee.DrawImage( pDC, m_ptFace[ wChairID ].x - m_nXFace - 8, m_ptFace[ wChairID ].y + m_nYFace -
				m_ImageUserTrustee.GetHeight() + 3 );
		}
	}

	//绘画倍数
	if ( m_bShowLandTime ) DrawLandTime(pDC, nWidth, nHeight);

	return;
}

//炸弹倍数
void CGameClientView::SetBombTime(WORD wBombTime)
{
	//设置变量
	m_wBombTime=wBombTime;

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
void CGameClientView::ShowLandTitle(bool bLandTitle)
{
	//设置变量
	m_bLandTitle=bLandTitle;

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
void CGameClientView::SetLandUser(WORD wChairID, BYTE bLandScore)
{
	//设置变量
	m_wLandUser=wChairID;
	m_cbLandScore=bLandScore;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//挖坑分数
void CGameClientView::SetLandScore(WORD wChairID, BYTE bLandScore, BYTE cbCallScorePhase, BYTE cbPreCallScorePhase)
{
	//设置变量
	if (wChairID!=INVALID_CHAIR)
	{
		m_bScore[wChairID]=bLandScore;
		m_cbCallScorePhase = cbCallScorePhase;
		m_cbUserPhase[wChairID] = cbPreCallScorePhase;
	}
	else 
	{
		memset(m_bScore,0,sizeof(m_bScore));
		m_cbCallScorePhase = cbCallScorePhase;
		ZeroMemory( m_cbUserPhase, sizeof( m_cbUserPhase ) );
	}	

	//更新界面
	UpdateGameView(NULL);

	return;
}

//开始按钮
void CGameClientView::OnStart()
{

//#ifdef VIDEO_GAME
//	//隐藏扑克
//	m_HandCardControl[ 0 ].ShowWindow( SW_HIDE );
//	m_HandCardControl[ 2 ].ShowWindow( SW_HIDE );
//#endif

	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	return;
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

//叫分按钮
void CGameClientView::OnOneScore()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,1,1);
	return;
}

//叫分按钮
void CGameClientView::OnTwoScore()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,2,2);
	return;
}

//放弃按钮
void CGameClientView::OnGiveUpScore()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,255,255);
	return;
}

//出牌提示
void CGameClientView::OnAutoOutCard()
{
	AfxGetMainWnd()->SendMessage(IDM_AUTO_OUTCARD,0,0);
	return;
}

//叫分按钮
void CGameClientView::OnThreeScore()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,3,3);
	return;
}
//////////////////////////////////////////////////////////////////////////

// 自动托管
void CGameClientView::OnAutoPlayerOn()
{
    m_btAutoPlayOn.ShowWindow(SW_HIDE);
    m_btAutoPlayOff.ShowWindow(SW_SHOW);
	AfxGetMainWnd()->SendMessage(IDM_LAND_AUTOPLAY,1,1);
    return;
}

// 取消托管
void CGameClientView::OnAutoPlayerOff()
{
    m_btAutoPlayOn.ShowWindow(SW_SHOW);
    m_btAutoPlayOff.ShowWindow(SW_HIDE);
	AfxGetMainWnd()->SendMessage(IDM_LAND_AUTOPLAY,0,0);
    return;
}

//设置顺序
void CGameClientView::SetUserOrder(bool bDeasilOrder) {

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


//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//绘画字体
	pDC->SetTextColor(crText);
	pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

	return;
}

//玩家托管
void CGameClientView::SetUserTrustee( WORD wChairID, bool bTrustee ) {

	//设置变量
	if ( INVALID_CHAIR == wChairID ) {
		for ( WORD wChairIdx = 0; wChairIdx < GAME_PLAYER; wChairIdx++ ) {
			m_bUserTrustee[ wChairIdx ] = bTrustee;
		}
	}
	else {
		m_bUserTrustee[ wChairID ] = bTrustee;
	}

	//更新界面
	UpdateGameView( NULL );
}

//排序按钮
VOID CGameClientView::OnBnClickedSortCard()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_SORT_HAND_CARD,0,0);

	return;
}

//叫分按钮
void CGameClientView::OnSnatchLand()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,4,4);
	return;
}

//叫分按钮
void CGameClientView::OnBrightCard()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,5,5);
	return;
}

//显示报牌
void CGameClientView::SetShowWarming(WORD wChairID, bool bShowWarming)
{
	if(INVALID_CHAIR==wChairID)
	{
		for(BYTE i=0; i<GAME_PLAYER; ++i) m_bShowWarming[i] = false ;
	}
	else m_bShowWarming[wChairID] = bShowWarming ;

	UpdateGameView(NULL) ;
}

//明牌开始
void CGameClientView::OnBrightStart()
{
	AfxGetMainWnd()->SendMessage(IDM_BRIGHT_START,0,0);
	return;
}

//明牌开始
void CGameClientView::SetBrightStart(WORD wChairID, bool bBrightStart) 
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bBrightStart[i]=bBrightStart;
	}
	else m_bBrightStart[wChairID]=bBrightStart;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//显示倍数
void CGameClientView::DrawLandTime(CDC *pDC, INT nViewWidth, INT nViewHeight)
{
	//图片信息
	INT nDigitWidth = m_ImageLandTimes.GetWidth() / 11;
	INT nDigitHeight = m_ImageLandTimes.GetHeight();

	//设置变量
	bool bBrightStart = false;
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		//存在判断
		tagUserData const *pUserData = GetUserInfo( wChairID );
		if ( pUserData == NULL )  m_bBrightStart[wChairID] = false;
	}
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
		if ( m_bBrightStart[wChairID] ) bBrightStart = true;

	//重置变量
	WORD wCardCount = m_bCardCount[0] + m_bCardCount[1] + m_bCardCount[2];
	if ( ! bBrightStart && wCardCount == 0 ) m_cbBrightTime = 0;

	//翻倍计算
	WORD wDrawTimes=(WORD)m_lBaseScore;
	for (WORD i=1;i<m_cbLandScore;i++) wDrawTimes*=2;

	//总共倍数
	WORD wLandTime = 0;
	if ( m_cbBrightTime != 0 && wDrawTimes == 0 ) wLandTime = m_cbBrightTime;
	else if ( m_cbBrightTime == 0 && wDrawTimes != 0 ) wLandTime = wDrawTimes;
	else if ( m_cbBrightTime != 0 && wDrawTimes != 0 ) wLandTime = m_cbBrightTime * wDrawTimes;

	//大小过滤
	if ( wLandTime <= m_lBaseScore ) return ;

	//数字个数
	INT nDigitCount = 0;
	INT nTmpTime = wLandTime;
	while( 1 <= nTmpTime )
	{
		nDigitCount++;
		nTmpTime /= 10;
	}

	if ( 1 <= nDigitCount )
	{
		//总共长度
		INT nTotalLen = ( nDigitCount +1 ) * nDigitWidth;

		//位置信息
		INT nXPos = 0; 
		INT nYPos = 0;

		nXPos = ( nViewWidth - nTotalLen ) / 2 ;
		nYPos = 150;

		//绘画乘号
		m_ImageLandTimes.DrawImage(pDC, nXPos, nYPos, nDigitWidth, nDigitHeight, 10 * nDigitWidth, 0);

		//开始绘画
		while( 0 < nDigitCount )
		{
			//计算位置
			nXPos += nDigitWidth;

			//计算倍数
			--nDigitCount;
			INT nMutiple = (INT)pow( 10, nDigitCount );

			//绘画数字
			INT nDrawDigit = (INT)(wLandTime / nMutiple) ;
			m_ImageLandTimes.DrawImage(pDC, nXPos, nYPos, nDigitWidth, nDigitHeight, nDrawDigit * nDigitWidth, 0);

			//计算变量
			wLandTime -= ( nMutiple * nDrawDigit );
		}
	}
}

//明牌倍数
void CGameClientView::SetBrightTime(BYTE cbBrightTime)
{
	//相等判断
	if ( m_cbBrightTime == cbBrightTime ) return ;

	//变量赋值
	m_cbBrightTime = cbBrightTime;

	//更新界面
	UpdateGameView(NULL);

	return;
}

	//加倍消息
void CGameClientView::OnDoubleScore()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_DOUBLE_SCORE, 0, 0);
}
//加倍消息
void CGameClientView::OnNotDoubleScore()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_DOUBLE_SCORE, 1, 1);
}

//显示倍数
void CGameClientView::ShowLandTime(bool bShowLandTime)
{
	//等同判断
	if ( m_bShowLandTime == bShowLandTime ) return ;

	//设置变量
	m_bShowLandTime = bShowLandTime;

	//更新界面
	UpdateGameView( NULL );
}

//玩家加倍
void CGameClientView::SetDoubleScoreUser( WORD wChairID, bool bDoubleScore )
{
	//设置变量
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) m_bDoubleScore[i]=bDoubleScore;
	}
	else m_bDoubleScore[wChairID]=bDoubleScore;

	//更新界面
	UpdateGameView(NULL);

}