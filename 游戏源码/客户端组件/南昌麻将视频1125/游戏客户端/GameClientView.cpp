#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClient.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识 

#define IDC_START						100								//开始按钮
#define IDC_SCOREONE					101								//1分按钮
#define IDC_SCORETWO					102								//2分按钮
#define IDC_SCORENONE					103								//不买按钮
#define IDC_TRUSTEE_CONTROL				104								//托管控制
#define IDC_BATCH						105								//洗澡按钮	
#define IDC_SCORE						107								//查分按钮
#define IDC_SCORE_CLOSE					108								//关闭成绩

//动作标识
#define IDI_BOMB_EFFECT					101								//动作标识

#define IDI_BATCH_START					102								//洗澡动作

//动作数目
#define BOMB_EFFECT_COUNT				12								//动作数目
#define DISC_EFFECT_COUNT				8								//丢弃效果		
#define BATCH_EFFECT_COUNT				5								//动作数目

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_BATCH, OnBatch)
	ON_BN_CLICKED(IDC_TRUSTEE_CONTROL,OnStusteeControl)
	ON_BN_CLICKED(IDC_SCORE, OnBnClickedScore)
	ON_BN_CLICKED(IDC_SCORE_CLOSE,OnBnClickedCloseScore)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
	//标志变量
	m_bOutCard=false;
	m_bWaitOther=false;
	m_bHuangZhuang=false;
	m_bShowScore=false;

	//游戏属性
	m_lCellScore=0L;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//动作动画
	m_bBombEffect=false;
	m_cbBombFrameIndex=0;

	//洗澡动画
	m_bBatchEffect=false;
	m_cbBatchFrameIndex=0;

	//丢弃效果
	m_wDiscUser=INVALID_CHAIR;
	m_cbDiscFrameIndex=0;

	//历史积分
	memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
	memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));
	ZeroMemory(m_lUpDownScore,sizeof(m_lUpDownScore));
	m_cbUpDownKingCardData=0;
	m_cbFloor=0;

	//用户状态
	m_cbCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_wUserAction,sizeof(m_wUserAction));
	ZeroMemory(m_cbBatch,sizeof(m_cbBatch));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	FillMemory(m_cbKingCardIndex,sizeof(m_cbKingCardIndex),0xFF);

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageWait.SetLoadInfo(IDB_WAIT_TIP,hInstance);
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageUserFlag.SetLoadInfo(IDB_USER_FLAG,hInstance);
	m_ImageOutCard.SetLoadInfo(IDB_OUT_CARD_TIP,hInstance);
	m_ImageUserAction.SetLoadInfo(IDB_USER_ACTION,hInstance);
	m_ImageActionBack.SetLoadInfo(IDB_ACTION_BACK,hInstance);
	m_ImageHuangZhuang.SetLoadInfo(IDB_HUANG_ZHUANG,hInstance);
	m_ImageTrustee.LoadImage(hInstance,TEXT("TRUSTEE"));
	m_ImageActionAni.LoadImage(AfxGetInstanceHandle(),TEXT("ActionAni"));
	m_ImageDisc.LoadImage(AfxGetInstanceHandle(),TEXT("DISC"));
	m_ImageArrow.LoadImage(AfxGetInstanceHandle(),TEXT("ARROW"));
	m_ImageHistoryScore.SetLoadInfo(IDB_HISTORY_SCORE,hInstance);
	m_ImageCenter.SetLoadInfo(IDB_VIEW_CENTER,hInstance);
	m_ImageLeft.SetLoadInfo(IDB_VIEW_LEFT,hInstance);

	// 洗澡png
	m_PngBatch[0].LoadImage(hInstance,TEXT("BATCH_TOP"));
	m_PngBatch[1].LoadImage(hInstance,TEXT("BATCH_RIGHT"));
	m_PngBatch[2].LoadImage(hInstance,TEXT("BATCH_SELF"));
	m_PngBatch[3].LoadImage(hInstance,TEXT("BATCH_LEFT"));

	CImageHandle HistoryScoreHandle(&m_ImageHistoryScore);
	m_sizeHistory.SetSize(m_ImageHistoryScore.GetWidth(),m_ImageHistoryScore.GetHeight());


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

	//变量定义
	enDirection Direction[]={Direction_North,Direction_East,Direction_South,Direction_West};

	//设置控件
	for (WORD i=0;i<4;i++)
	{
		//用户扑克
		m_HeapCard[i].SetDirection(Direction[i]);
		m_TableCard[i].SetDirection(Direction[i]);
		m_DiscardCard[i].SetDirection(Direction[i]);

		//组合扑克
		m_WeaveCard[i][0].SetDisplayItem(1);
		m_WeaveCard[i][1].SetDisplayItem(1);
		m_WeaveCard[i][2].SetDisplayItem(1);
		m_WeaveCard[i][3].SetDisplayItem(1);
		m_WeaveCard[i][0].SetDirection(Direction[i]);
		m_WeaveCard[i][1].SetDirection(Direction[i]);
		m_WeaveCard[i][2].SetDirection(Direction[i]);
		m_WeaveCard[i][3].SetDirection(Direction[i]);
	}




	//设置控件
	m_UserCard[0].SetDirection(Direction_North);
	m_UserCard[1].SetDirection(Direction_East);
	m_UserCard[2].SetDirection(Direction_West);

	//创建控件
	CRect rcCreate(0,0,0,0);
	m_GameScoreWnd.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rcCreate,this,100);
	m_ControlWnd.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rcCreate,this,10,NULL);
	m_DrawSiceWnd.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rcCreate,this,10,NULL);
	m_DrawSiceWnd2.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rcCreate,this,10,NULL);

	//创建控件
	//开始按钮
	m_btStart.Create(NULL,WS_CHILD,rcCreate,this,IDC_START);
	m_btStart.SetButtonImage(IDB_BT_START,AfxGetInstanceHandle(),false);

	//洗澡按钮
	m_btBatch.Create(NULL,WS_CHILD|WS_CLIPCHILDREN,rcCreate,this,IDC_BATCH);
	m_btBatch.SetButtonImage(IDB_BT_BATCH,AfxGetInstanceHandle(),false);

	//托管按钮
	m_btStusteeControl.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_TRUSTEE_CONTROL);
	m_btStusteeControl.SetButtonImage(IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false);

	//看牌按钮
	m_btScore.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SCORE);
	m_btScore.SetButtonImage(IDB_BT_SHOW_SCORE,AfxGetInstanceHandle(),false);

	//看牌按钮
	m_btScoreClose.Create(NULL,WS_CHILD,rcCreate,this,IDC_SCORE_CLOSE);
	m_btScoreClose.SetButtonImage(IDB_BT_CLOSE_SCORE,AfxGetInstanceHandle(),false);

	////创建视频
	//for (WORD i=0;i<4;i++)
	//{
	//	//创建视频
	//	m_DlgVedioService[i].Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,300+i);
	//	m_DlgVedioService[i].InitVideoService(i==2,true);

	//	//设置视频
	//	g_VedioServiceManager.SetVideoServiceControl(i,&m_DlgVedioService[i]);
	//}

	SetTimer(IDI_DISC_EFFECT,250,NULL);
	return 0;
}

//重置界面
void CGameClientView::ResetGameView()
{
	//标志变量
	m_bOutCard=false;
	m_bWaitOther=false;
	m_bHuangZhuang=false;
	m_bShowScore=false;

	//游戏属性
	m_lCellScore=0L;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbBatch,sizeof(m_cbBatch));

   	//动作动画
	m_bBombEffect=false;
	m_cbBombFrameIndex=0;

	//洗澡动画
	m_bBatchEffect=false;
	m_cbBatchFrameIndex=0;

	//丢弃效果
	m_wDiscUser=INVALID_CHAIR;
	m_cbDiscFrameIndex=0;

	//历史积分
	memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
	memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));
	ZeroMemory(m_lUpDownScore,sizeof(m_lUpDownScore));
	m_cbUpDownKingCardData=0;
	m_cbFloor=0;


	//用户状态
	m_cbCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_wUserAction,sizeof(m_wUserAction));
	FillMemory(m_cbKingCardIndex,sizeof(m_cbKingCardIndex),0xFF);

	//界面设置
	m_btStart.ShowWindow(SW_HIDE);
	m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameScoreWnd.ShowWindow(SW_HIDE);
	m_DrawSiceWnd.ShowWindow(SW_HIDE);
	m_btBatch.ShowWindow(SW_HIDE);
	m_btScoreClose.ShowWindow(SW_HIDE);

	//禁用控件
	m_btStusteeControl.EnableWindow(FALSE);

	//扑克设置
	m_UserCard[0].SetCardData(0,false);
	m_UserCard[1].SetCardData(0,false);
	m_UserCard[2].SetCardData(0,false);
	m_HandCardControl.SetPositively(false);
	m_HandCardControl.SetDisplayItem(false);
	m_HandCardControl.SetCardData(NULL,0,0);

	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HeapCard[i].SetCardData(0,0,0);
		m_TableCard[i].SetCardData(NULL,0);
		m_DiscardCard[i].SetCardData(NULL,0);
		m_WeaveCard[i][0].SetCardData(NULL,0);
		m_WeaveCard[i][1].SetCardData(NULL,0);
		m_WeaveCard[i][2].SetCardData(NULL,0);
		m_WeaveCard[i][3].SetCardData(NULL,0);
	}

	//销毁定时器
	KillTimer(IDI_DISC_EFFECT);
	KillTimer(IDI_BOMB_EFFECT);
	KillTimer(IDI_BATCH_START);

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//设置坐标
	m_ptFace[0].x=nWidth/2-m_nXFace;
	m_ptFace[0].y=5+m_nYBorder;
	m_ptName[0].x=nWidth/2+5;
	m_ptName[0].y=10+m_nYBorder;
	m_ptTimer[0].x=nWidth/2-m_nXFace-m_nXTimer-2;
	m_ptTimer[0].y=17+m_nYBorder;
	m_ptReady[0].x=nWidth/2-m_nXFace-m_nXTimer*2-5;
	m_ptReady[0].y=5+m_nYBorder+m_nYTimer/2;
	m_UserFlagPos[0].x=nWidth/2-m_nXFace-m_nXTimer-32;
	m_UserFlagPos[0].y=5+m_nYBorder;
	//m_UserListenPos[0].x=nWidth/2;
	//m_UserListenPos[0].y=m_nYBorder+100;
	m_PointTrustee[0].x=nWidth/2-m_nXFace-20-m_nXFace/2;
	m_PointTrustee[0].y=5+m_nYBorder;


	m_ptFace[1].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptFace[1].y=nHeight/2-m_nYFace-50;
	m_ptName[1].x=nWidth-m_nXBorder-5;
	m_ptName[1].y=nHeight/2-45;
	m_ptTimer[1].x=nWidth-m_nXBorder-m_nXFace+5;
	m_ptTimer[1].y=nHeight/2-m_nYFace-27-m_nYTimer;
	m_ptReady[1].x=nWidth-m_nXBorder-m_nXTimer+3;
	m_ptReady[1].y=nHeight/2-m_nYFace-55-m_nYTimer/2+m_nYFace*2+40;
	m_UserFlagPos[1].x=nWidth-m_nXBorder-m_nXTimer+8;
	m_UserFlagPos[1].y=nHeight/2-m_nYFace-83-m_nYTimer;
	//m_UserListenPos[1].x=nWidth-m_nXBorder-93;
	//m_UserListenPos[1].y=nHeight/2-75;
	m_PointTrustee[1].x=nWidth-m_nXBorder-m_nXTimer+8;
	m_PointTrustee[1].y=nHeight/2-m_nYFace-50-35;

	m_ptFace[2].x=nWidth/2-m_nXFace;
	m_ptFace[2].y=nHeight-m_nYBorder-m_nYFace-5;
	m_ptName[2].x=nWidth/2+5+m_nXFace/2;
	m_ptName[2].y=nHeight-m_nYBorder-m_nYFace-5;
	m_ptTimer[2].x=nWidth/2-m_nXFace/2-m_nXTimer-2;
	m_ptTimer[2].y=nHeight-m_nYBorder-m_nYTimer-8+40;
	m_ptReady[2].x=nWidth/2-m_nXTimer*2-m_nXFace/2-5;
	m_ptReady[2].y=nHeight-m_nYBorder-m_nYTimer/2-10;
	m_UserFlagPos[2].x=nWidth/2-m_nXFace-m_nXTimer-32;
	m_UserFlagPos[2].y=nHeight-m_nYBorder-35;
	//m_UserListenPos[2].x=nWidth/2;
	//m_UserListenPos[2].y=nHeight-m_nYBorder-123;
	m_PointTrustee[2].x=nWidth/2-m_nXFace-20-m_nXFace/2;
	m_PointTrustee[2].y=nHeight-m_nYBorder-m_nYFace-5;

	m_ptFace[3].x=m_nXBorder+5;
	m_ptFace[3].y=nHeight/2-m_nYFace-50;
	m_ptName[3].x=m_nXBorder+5;
	m_ptName[3].y=nHeight/2-45;
	m_ptTimer[3].x=m_nXBorder+22;
	m_ptTimer[3].y=nHeight/2-m_nYFace-27-m_nYTimer;
	m_ptReady[3].x=m_nXBorder-2+m_nXTimer;
	m_ptReady[3].y=nHeight/2-m_nYFace-55-m_nYTimer/2+m_nYFace*2+40;
	m_UserFlagPos[3].x=m_nXBorder+8;
	m_UserFlagPos[3].y=nHeight/2-m_nYFace-83-m_nYTimer;
	//m_UserListenPos[3].x=m_nXBorder+91;
	//m_UserListenPos[3].y=nHeight/2-80;
	m_PointTrustee[3].x=m_nXBorder+5;
	m_PointTrustee[3].y=nHeight/2-m_nYFace-50-35;
	//用户扑克
	m_UserCard[0].SetControlPoint(nWidth/2-210,m_nYBorder+m_nYFace+10);
	m_UserCard[1].SetControlPoint(nWidth-m_nXBorder-m_nXFace-49-30-20,nHeight/2-255+50-25-10);
	m_UserCard[2].SetControlPoint(m_nXBorder+m_nXFace+30+30+20+10,nHeight/2+157+30+20);
	m_HandCardControl.SetBenchmarkPos(nWidth/2+5+10,nHeight-m_nYFace-m_nYBorder-5,enXCenter,enYBottom);

	//桌面扑克
	m_TableCard[0].SetControlPoint(nWidth/2-200,m_nYBorder+m_nYFace+10);
	m_TableCard[1].SetControlPoint(nWidth-m_nXBorder-m_nXFace-20-30-20,nHeight/2-197-20);
	m_TableCard[2].SetControlPoint(nWidth/2+185+80,nHeight-m_nYFace-m_nYBorder-15-20);
	m_TableCard[3].SetControlPoint(m_nXBorder+m_nXFace+10+30+20+13,nHeight/2+97+20+20+30);

	//堆积扑克
	int nXCenter=nWidth/2;
	int nYCenter=nHeight/2-40;
	m_HeapCard[0].SetControlPoint(nXCenter-225+8,nYCenter-225);
	m_HeapCard[1].SetControlPoint(nXCenter+228-20,nYCenter-155+15);
	m_HeapCard[2].SetControlPoint(nXCenter-225+8,nYCenter+213);
	m_HeapCard[3].SetControlPoint(nXCenter-258+20+10,nYCenter-155+15);

	//丢弃扑克
	//m_DiscardCard[0].SetControlPoint(nXCenter-103,nYCenter-100);
	m_DiscardCard[0].SetControlPoint(nXCenter-175+80,nYCenter-100);
	m_DiscardCard[1].SetControlPoint(nXCenter+103+50-10-20,nYCenter-63);
	//m_DiscardCard[2].SetControlPoint(nXCenter+100,nYCenter+112);
	m_DiscardCard[2].SetControlPoint(nXCenter+150-70,nYCenter+127);
	m_DiscardCard[3].SetControlPoint(nXCenter-137-55+10+20+10,nYCenter+90);

	//组合扑克
	m_WeaveCard[0][0].SetControlPoint(nWidth/2+175-70,m_nYBorder+m_nYFace+55+5+30);
	m_WeaveCard[0][1].SetControlPoint(nWidth/2+90-70,m_nYBorder+m_nYFace+55+5+30);
	m_WeaveCard[0][2].SetControlPoint(nWidth/2+5-70,m_nYBorder+m_nYFace+55+5+30);
	m_WeaveCard[0][3].SetControlPoint(nWidth/2-80-70,m_nYBorder+m_nYFace+55+5+30);

	//组合扑克
	m_WeaveCard[1][0].SetControlPoint(nWidth-m_nXBorder-m_nXFace-73-15-30-20,nHeight/2+130-50);
	m_WeaveCard[1][1].SetControlPoint(nWidth-m_nXBorder-m_nXFace-73-15-30-20,nHeight/2+50-50);
	m_WeaveCard[1][2].SetControlPoint(nWidth-m_nXBorder-m_nXFace-73-15-30-20,nHeight/2-30-50);
	m_WeaveCard[1][3].SetControlPoint(nWidth-m_nXBorder-m_nXFace-73-15-30-20,nHeight/2-110-50);

	//组合扑克
	m_WeaveCard[2][0].SetControlPoint(nWidth/2-200+20+5,nHeight-m_nYFace-m_nYBorder-10-100-3-15);
	m_WeaveCard[2][1].SetControlPoint(nWidth/2-85+20+5,nHeight-m_nYFace-m_nYBorder-10-100-3-15);
	m_WeaveCard[2][2].SetControlPoint(nWidth/2+30+20+5,nHeight-m_nYFace-m_nYBorder-10-100-3-15);
	m_WeaveCard[2][3].SetControlPoint(nWidth/2+145+20+5,nHeight-m_nYFace-m_nYBorder-10-100-3-15);

	//组合扑克
	m_WeaveCard[3][0].SetControlPoint(m_nXBorder+m_nXFace+70+15+30+20+10,nHeight/2-200+50+20);
	m_WeaveCard[3][1].SetControlPoint(m_nXBorder+m_nXFace+70+15+30+20+10,nHeight/2-120+50+20);
	m_WeaveCard[3][2].SetControlPoint(m_nXBorder+m_nXFace+70+15+30+20+10,nHeight/2-40+50+20);
	m_WeaveCard[3][3].SetControlPoint(m_nXBorder+m_nXFace+70+15+30+20+10,nHeight/2+40+50+20);

	//控制窗口
	m_ControlWnd.SetBenchmarkPos(nWidth/2+328,nHeight-m_nYBorder-120);

	//调整成绩
	CRect rcScore;
	m_GameScoreWnd.GetWindowRect(&rcScore);
	m_GameScoreWnd.MoveWindow((nWidth-rcScore.Width())/2,(nHeight-rcScore.Height())/2-30,rcScore.Width(),rcScore.Height());

	CRect rcSice;
	m_DrawSiceWnd.GetWindowRect(&rcSice);
	m_DrawSiceWnd.SetBenchmarkPos((nWidth-rcSice.Width())/2,(nHeight-rcSice.Height())/2-40);
	m_DrawSiceWnd.MoveWindow((nWidth-rcSice.Width())/2,(nHeight-rcSice.Height())/2-40,rcSice.Width(),rcSice.Height());

	m_DrawSiceWnd2.SetBenchmarkPos((nWidth-rcSice.Width())/2,(nHeight-rcSice.Height())/2-40);
	m_DrawSiceWnd2.MoveWindow((nWidth-rcSice.Width())/2,(nHeight-rcSice.Height())/2-40,rcSice.Width(),rcSice.Height());


	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	m_btStart.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//移动调整
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nHeight-155-m_nYBorder+30-10,0,0,uFlags);

	//移动调整
	DeferWindowPos(hDwp,m_btBatch,NULL,(nWidth-rcButton.Width())/2,nHeight-155-m_nYBorder+30,0,0,uFlags);

	
	//托管按钮
	DeferWindowPos(hDwp,m_btStusteeControl,NULL,nWidth-m_nXBorder-(rcButton.Width()+5),nHeight-m_nYBorder-5-rcButton.Height(),0,0,uFlags);
	
	//查分按钮
	m_btScore.GetWindowRect(&rcScore);
	DeferWindowPos(hDwp,m_btScore,NULL,nWidth-rcScore.Width()-m_nXBorder,0,0,0,uFlags);

	//关闭成绩
	DeferWindowPos(hDwp,m_btScoreClose,NULL,nWidth-rcScore.Width()-m_nXBorder,m_nYBorder+m_sizeHistory.cy,0,0,uFlags);

	////视频窗口
	//CRect rcAVDlg;
	//m_DlgVedioService[0].GetWindowRect(&rcAVDlg);
	//DeferWindowPos(hDwp,m_DlgVedioService[1],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),nHeight/2+30,0,0,uFlags);
	//DeferWindowPos(hDwp,m_DlgVedioService[3],NULL,m_nXBorder+5,nHeight/2+30,0,0,uFlags);
	//DeferWindowPos(hDwp,m_DlgVedioService[0],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),5,0,0,uFlags);
	//m_DlgVedioService[2].GetWindowRect(&rcAVDlg);
	//DeferWindowPos(hDwp,m_DlgVedioService[2],NULL,m_nXBorder+5,nHeight-m_nYBorder-3-rcAVDlg.Height(),0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDwp);



	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBack,enMode_Spread);
	DrawViewImage(pDC,m_ImageCenter,enMode_Centent);
	//加载位图
	CImageHandle ImageHandle(&m_ImageLeft);
	m_ImageLeft.BitBlt(pDC->GetSafeHdc(),m_nXBorder,m_nYBorder);

	//上下番
	if(m_cbUpDownKingCardData != 0)
	{
		g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,m_cbUpDownKingCardData,m_nXBorder+45,m_nYBorder+12);
	}

	
	//爬楼标志
	if (m_cbFloor!=0)
	{
		//创建资源
		CFont InfoFont;
		InfoFont.CreateFont(-36,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("黑体"));

		//设置 DC
		pDC->SetTextColor(RGB(201,229,133));
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		CFont * pOldFont=pDC->SelectObject(&InfoFont);
	
		TCHAR szFloor[32]=TEXT("");
		_sntprintf(szFloor,sizeof(szFloor),TEXT("%d"),m_cbFloor);

		//绘画信息
		pDC->TextOut(m_nXBorder+50+65,m_nYBorder+15,szFloor);

		//清理资源
		pDC->SelectObject(pOldFont);
		InfoFont.DeleteObject();


	}

	//绘画效果
	INT nItemHeight=m_PngBatch[1].GetHeight();
	INT nItemWidth=m_PngBatch[1].GetWidth()/BATCH_EFFECT_COUNT;


	//绘画用户
	pDC->SetTextColor(RGB(255,255,0));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);
		if (pUserData!=NULL)
		{
			//用户名字
			pDC->SetTextAlign((i==1)?(TA_RIGHT|TA_TOP):(TA_LEFT|TA_TOP));
			DrawTextString(pDC,pUserData->szName,RGB(255,255,255),RGB(0,0,0),m_ptName[i].x,m_ptName[i].y);

			//其他信息
			WORD wUserTimer=GetUserTimer(i);
			if ((wUserTimer!=0)&&(m_wCurrentUser!=INVALID_CHAIR))
			{
				DrawUserTimer(pDC,nWidth/2,nHeight/2-40,wUserTimer);
				if(m_wCurrentUser==0)
					m_ImageArrow.DrawImage(pDC,nWidth/2-15,nHeight/2-m_ImageArrow.GetHeight()*2,m_ImageArrow.GetWidth()/4,m_ImageArrow.GetHeight(),m_ImageArrow.GetWidth()/4*m_wCurrentUser,0);
				if(m_wCurrentUser==1)
					m_ImageArrow.DrawImage(pDC,nWidth/2+m_ImageArrow.GetWidth()/4,nHeight/2-15,m_ImageArrow.GetWidth()/4,m_ImageArrow.GetHeight(),m_ImageArrow.GetWidth()/4*m_wCurrentUser,0);
				if(m_wCurrentUser==2)
					m_ImageArrow.DrawImage(pDC,nWidth/2-15,nHeight/2+m_ImageArrow.GetHeight(),m_ImageArrow.GetWidth()/4,m_ImageArrow.GetHeight(),m_ImageArrow.GetWidth()/4*m_wCurrentUser,0);
				if(m_wCurrentUser==3)
					m_ImageArrow.DrawImage(pDC,nWidth/2-m_ImageArrow.GetWidth()/4*2,nHeight/2-15,m_ImageArrow.GetWidth()/4,m_ImageArrow.GetHeight(),m_ImageArrow.GetWidth()/4*m_wCurrentUser,0);
			}
			if((wUserTimer!=0)&&(m_wCurrentUser==INVALID_CHAIR))
			{
				DrawUserTimer(pDC,nWidth/2,nHeight/2-40,wUserTimer);
				if(i==0)
					m_ImageArrow.DrawImage(pDC,nWidth/2-15,nHeight/2-m_ImageArrow.GetHeight()*2-40,m_ImageArrow.GetWidth()/4,m_ImageArrow.GetHeight(),m_ImageArrow.GetWidth()/4*i,0);
				if(i==1)
					m_ImageArrow.DrawImage(pDC,nWidth/2+m_ImageArrow.GetWidth()/4,nHeight/2-15-40,m_ImageArrow.GetWidth()/4,m_ImageArrow.GetHeight(),m_ImageArrow.GetWidth()/4*i,0);
				if(i==2)
					m_ImageArrow.DrawImage(pDC,nWidth/2-15,nHeight/2+m_ImageArrow.GetHeight()-40,m_ImageArrow.GetWidth()/4,m_ImageArrow.GetHeight(),m_ImageArrow.GetWidth()/4*i,0);
				if(i==3)
					m_ImageArrow.DrawImage(pDC,nWidth/2-m_ImageArrow.GetWidth()/4*2,nHeight/2-15-40,m_ImageArrow.GetWidth()/4,m_ImageArrow.GetHeight(),m_ImageArrow.GetWidth()/4*i,0);

			}

			if (pUserData->cbUserStatus==US_READY) 
				DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);

			//托管
			if(m_bTrustee[i])
			{

				m_ImageTrustee.DrawImage(pDC,m_PointTrustee[i].x,m_PointTrustee[i].y);
			}
		}
	}

	//用户标志
	if (m_wBankerUser!=INVALID_CHAIR)
	{
		//加载位图
		CImageHandle ImageHandle(&m_ImageUserFlag);
		int nImageWidth=m_ImageUserFlag.GetWidth()/4;
		int nImageHeight=m_ImageUserFlag.GetHeight();

		//绘画标志
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			WORD wIndex=((i+GAME_PLAYER)-m_wBankerUser)%GAME_PLAYER;
			m_ImageUserFlag.AlphaDrawImage(pDC,m_UserFlagPos[i].x,m_UserFlagPos[i].y,nImageWidth,nImageHeight,nImageWidth*wIndex,0,RGB(255,0,255));
		}
	}
	//历史积分
	if (m_bShowScore==true)
	{
		//积分背景
		CImageHandle HandleHistoryScore(&m_ImageHistoryScore);
		m_ImageHistoryScore.BitBlt(pDC->m_hDC,nWidth-m_ImageHistoryScore.GetWidth()-m_nYBorder*4,0);

		//计算位置
		INT nYBenchmark=0;
		INT nXBenchmark=nWidth-m_ImageHistoryScore.GetWidth()-m_nYBorder*2;
		UINT nFormat=DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER;

		//绘画属性
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->SetTextColor(RGB(240,242,242));

		//绘画信息
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//获取用户
			const tagUserData * pUserData=GetUserInfo(i);
			if ((pUserData==NULL)) continue;

			//位置计算
			CRect rcAccounts(nXBenchmark+2,nYBenchmark+24+i*20,nXBenchmark+63,nYBenchmark+42+i*20);
			CRect rcUpDownScore(nXBenchmark+64,nYBenchmark+24+i*20,nXBenchmark+93,nYBenchmark+42+i*20);
			CRect rcTurnScore(nXBenchmark+94,nYBenchmark+24+i*20,nXBenchmark+147,nYBenchmark+42+i*20);
			CRect rcCollectScore(nXBenchmark+128,nYBenchmark+24+i*20,nXBenchmark+197,nYBenchmark+42+i*20);

			//构造信息
			TCHAR szUpDownScore[32]=TEXT(""),szTurnScore[32]=TEXT(""),szCollectScore[32]=TEXT("");
			_sntprintf(szTurnScore,sizeof(szTurnScore),TEXT("%ld"),m_lLastTurnScore[i]);
			_sntprintf(szCollectScore,sizeof(szCollectScore),TEXT("%ld"),m_lAllTurnScore[i]);
			_sntprintf(szUpDownScore,sizeof(szUpDownScore),TEXT("%ld"),m_lUpDownScore[i]);

			//绘画信息
			
			pDC->DrawText(pUserData->szName,lstrlen(pUserData->szName),&rcAccounts,nFormat);
			pDC->DrawText(szUpDownScore,lstrlen(szUpDownScore),&rcUpDownScore,nFormat);
			pDC->DrawText(szTurnScore,lstrlen(szTurnScore),&rcTurnScore,nFormat);
			pDC->DrawText(szCollectScore,lstrlen(szCollectScore),&rcCollectScore,nFormat);
		}
	}
	//桌面扑克
	for (WORD i=0;i<4;i++)
	{
		m_TableCard[i].DrawCardControl(pDC);
		m_DiscardCard[i].DrawCardControl(pDC);
		m_WeaveCard[i][0].DrawCardControl(pDC);
		m_WeaveCard[i][1].DrawCardControl(pDC);
		m_WeaveCard[i][2].DrawCardControl(pDC);
		m_WeaveCard[i][3].DrawCardControl(pDC);
	}

	m_WeaveCard[0][4].DrawCardControl(pDC);
	m_WeaveCard[1][4].DrawCardControl(pDC);
	m_WeaveCard[2][4].DrawCardControl(pDC);
	m_WeaveCard[3][4].DrawCardControl(pDC);

	//堆积扑克
	m_HeapCard[3].DrawCardControl(pDC);
	m_HeapCard[0].DrawCardControl(pDC);
	m_HeapCard[1].DrawCardControl(pDC);
	m_HeapCard[2].DrawCardControl(pDC);

	//用户扑克
	m_UserCard[0].DrawCardControl(pDC);
	m_UserCard[1].DrawCardControl(pDC);
	m_UserCard[2].DrawCardControl(pDC);
	m_HandCardControl.DrawCardControl(pDC);

	//出牌提示
	if (m_bOutCard==true)
	{
		CImageHandle HandleOutCard(&m_ImageOutCard);
		m_ImageOutCard.AlphaDrawImage(pDC,(nWidth-m_ImageOutCard.GetWidth())/2,nHeight-130,RGB(255,0,255));
	}

	//等待提示
	if (m_bWaitOther==true)
	{
		CImageHandle HandleWait(&m_ImageWait);
		m_ImageWait.AlphaDrawImage(pDC,(nWidth-m_ImageWait.GetWidth())/2,nHeight-130,RGB(255,0,255));
	}

	//荒庄标志
	if (m_bHuangZhuang==true)
	{
		CImageHandle HandleHuangZhuang(&m_ImageHuangZhuang);
		m_ImageHuangZhuang.AlphaDrawImage(pDC,(nWidth-m_ImageHuangZhuang.GetWidth())/2,nHeight/2-103,RGB(255,0,255));
	}



	//用户状态
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if ((m_wOutCardUser==i)||(m_wUserAction[i]!=0))
		{
			//计算位置
			int nXPos=0,nYPos=0;
			switch (i)
			{
			case 0:	//北向
				{
					nXPos=nWidth/2-32;
					nYPos=m_nYBorder+95;
					break;
				}
			case 1:	//东向
				{
					nXPos=nWidth-m_nXBorder-170;
					nYPos=nHeight/2-71;
					break;
				}
			case 2:	//南向
				{
					nXPos=nWidth/2-32;
					nYPos=nHeight-m_nYBorder-220;
					break;
				}
			case 3:	//西向
				{
					nXPos=m_nXBorder+115;
					nYPos=nHeight/2-71;
					break;
				}
			}

			//绘画动作
			if (m_wUserAction[i]!=WIK_NULL)
			{	

				//绘画动作
				if (m_bBombEffect==true)
				{

					//绘画效果
					INT nItemHeight=m_ImageActionAni.GetHeight()/6;
					INT nItemWidth=m_ImageActionAni.GetWidth()/BOMB_EFFECT_COUNT;

					//绘画动作
					int nYImagePos=0;
					if (m_wUserAction[i]&WIK_PENG) nYImagePos=nItemHeight;
					else if (m_wUserAction[i]&WIK_GANG) nYImagePos=nItemHeight*2;
				//	else if (m_cbUserAction[i]&WIK_LISTEN) nYImagePos=nItemHeight*3;
					else if (m_wUserAction[i]&WIK_CHI_HU) nYImagePos=nItemHeight*4;
					else nYImagePos=0;
					m_ImageActionAni.DrawImage(pDC,nXPos-nItemWidth/2+54,nYPos+62-nItemHeight/2,nItemWidth,nItemHeight,
						nItemWidth*(m_cbBombFrameIndex%BOMB_EFFECT_COUNT),nYImagePos,nItemWidth,nItemHeight);
				}
			}
			else
			{	
				//动作背景
				CImageHandle ImageHandle(&m_ImageActionBack);
				m_ImageActionBack.BlendDrawImage(pDC,nXPos,nYPos,m_ImageActionBack.GetWidth(),m_ImageActionBack.GetHeight(),
					0,0,RGB(255,0,255),180);


				//绘画扑克
				g_CardResource.m_ImageUserBottom.DrawCardItem(pDC,m_cbCardData,nXPos+15,nYPos+20);
			}

		}
	}
	//if(m_cbKingCardData!=0xFF)
	//{
	//	//绘画扑克
	//	g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,m_cbKingCardData,nWidth/2-25,nHeight/2-20);

	//}
	//丢弃效果
	if(m_wDiscUser!=INVALID_CHAIR)
	{

		CSize SizeDisc(m_ImageDisc.GetWidth()/DISC_EFFECT_COUNT,m_ImageDisc.GetHeight());
		CPoint pt=m_DiscardCard[m_wDiscUser].GetLastCardPosition();
		//绘画信息
		m_ImageDisc.DrawImage(pDC,pt.x,pt.y,SizeDisc.cx,SizeDisc.cy,
			m_cbDiscFrameIndex*SizeDisc.cx,0,SizeDisc.cx,SizeDisc.cy);

	}
	//洗澡效果
	if(m_bBatchEffect)
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbBatch[i]==1)
			{
				//计算位置
				int nXPos=0,nYPos=0;

				int nXCenter=nWidth/2;
				int nYCenter=nHeight/2-40;
				switch (i)
				{
				case 0:	//北向
					{
						nXPos=nXCenter-276+50;
						nYPos=nYCenter-200-30;
						break;
					}
				case 1:	//东向
					{
						nXPos=nXCenter+210;
						nYPos=nYCenter-150;
						break;
					}
				case 2:	//南向
					{
						nXPos=nXCenter-235;
						nYPos=nYCenter+213;
						break;
					}
				case 3:	//西向
					{
						nXPos=nXCenter-276+50;
						nYPos=nYCenter-145-5;
						break;
					}
				}

				//绘画效果
				INT nItemHeight=m_PngBatch[i].GetHeight();
				INT nItemWidth=m_PngBatch[i].GetWidth()/BATCH_EFFECT_COUNT;

				//绘画信息
				m_PngBatch[i].DrawImage(pDC,nXPos,nYPos,nItemWidth,nItemHeight,
					nItemWidth*(m_cbBatchFrameIndex),0,nItemWidth,nItemHeight);

			}
		}

	}
	return;
}

//基础积分
void CGameClientView::SetCellScore(LONG lCellScore)
{
	//设置扑克
	if (lCellScore!=m_lCellScore)
	{
		//设置变量
		m_lCellScore=lCellScore;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//海底扑克
void CGameClientView::SetHuangZhuang(bool bHuangZhuang)
{
	//设置扑克
	if (bHuangZhuang!=m_bHuangZhuang)
	{
		//设置变量
		m_bHuangZhuang=bHuangZhuang;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//庄家用户
void CGameClientView::SetBankerUser(WORD wBankerUser)
{
	//设置用户
	if (wBankerUser!=m_wBankerUser)
	{
		//设置变量
		m_wBankerUser=wBankerUser;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//状态标志
void CGameClientView::SetStatusFlag(bool bOutCard, bool bWaitOther)
{
	//设置变量
	m_bOutCard=bOutCard;
	m_bWaitOther=bWaitOther;

	//更新界面
	UpdateGameView(NULL);

	return;
}
//设置动作
bool CGameClientView::SetBombEffect(bool bBombEffect)
{
	if (bBombEffect==true)
	{
		//设置变量
		m_bBombEffect=true;
		m_cbBombFrameIndex=0;

		//启动时间
		SetTimer(IDI_BOMB_EFFECT,250,NULL);
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
//设置洗澡
bool CGameClientView::SetBatchEffect(bool bBatchEffect)
{
	if (bBatchEffect==true)
	{
		//设置变量
		m_bBatchEffect=true;
		m_cbBatchFrameIndex=0;
		for (BYTE i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbBatch[i]==1)
				m_HeapCard[i].SetCardData(0,0,0);
		}

		//启动时间
		SetTimer(IDI_BATCH_START,250,NULL);
	}
	else
	{
		//停止动画
		if (m_bBatchEffect==true)
		{
			//删除时间
			KillTimer(IDI_BATCH_START);

			//设置变量
			m_bBatchEffect=false;
			m_cbBatchFrameIndex=0;

			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbBatch[i]==1)
					m_HeapCard[i].SetCardData(0,0,HEAP_FULL_COUNT);
			}

			//更新界面
			UpdateGameView(NULL);
		}
	}

	return true;
}

//基础积分
void CGameClientView::SetKingCardIndex(BYTE cbKingCardIndex[2])
{
	//拷贝数据
	CopyMemory(m_cbKingCardIndex,cbKingCardIndex,sizeof(m_cbKingCardIndex));
    
	//更新界面
	UpdateGameView(NULL);


	return;
}


//出牌信息
void CGameClientView::SetOutCardInfo(WORD wViewChairID, BYTE cbCardData)
{
	//设置变量
	m_cbCardData=cbCardData;

	m_wOutCardUser=wViewChairID;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//动作信息
void CGameClientView::SetUserAction(WORD wViewChairID, WORD wUserAction)
{
	//设置变量
	if (wViewChairID<GAME_PLAYER)
	{
		m_wUserAction[wViewChairID]=wUserAction;
		SetBombEffect(true);
	}
	else 
	{
		ZeroMemory(m_wUserAction,sizeof(m_wUserAction));
		if(m_bBombEffect)
			SetBombEffect(false);
	}

	//更新界面
	UpdateGameView(NULL);


	return;
}


//丢弃用户
void CGameClientView::SetDiscUser(WORD wDiscUser)
{
	if(m_wDiscUser != wDiscUser)
	{
		//更新变量
		m_wDiscUser=wDiscUser;

		//更新界面
		UpdateGameView(NULL);
	}
	return;
}
//定时玩家
void CGameClientView::SetCurrentUser(WORD wCurrentUser)
{
	if (m_wCurrentUser != wCurrentUser)
	{
		//更新变量 
		m_wCurrentUser=wCurrentUser;

		//更新界面
		UpdateGameView(NULL);
	}
	return;
}
//设置托管
void CGameClientView::SetTrustee(WORD wTrusteeUser,bool bTrustee)
{
	//校验数据 
	ASSERT(wTrusteeUser>=0&&wTrusteeUser<GAME_PLAYER);

	if(m_bTrustee[wTrusteeUser] !=bTrustee)	
	{
		//设置数据
		m_bTrustee[wTrusteeUser]=bTrustee;

		//更新界面
		UpdateGameView(NULL);
	}
	return;

}

//洗澡结果
void CGameClientView::SetBatchResult(BYTE cbBatchResult[4])
{
	CopyMemory(m_cbBatch,cbBatchResult,sizeof(m_cbBatch));
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

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//点击测试
	bool bRePaint=false;
	bool bHandle=m_HandCardControl.OnEventSetCursor(MousePoint,bRePaint);

	//重画控制
	if (bRePaint==true)
		UpdateGameView(NULL);

	//光标控制
	if (bHandle==false)
		__super::OnSetCursor(pWnd,nHitTest,uMessage);

	return TRUE;
}

//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//获取扑克
	BYTE cbHoverCard=m_HandCardControl.GetHoverCard();


	if (cbHoverCard!=0) 
		AfxGetMainWnd()->PostMessage(IDM_OUT_CARD,cbHoverCard,cbHoverCard);

	return;
}

//开始按钮
void CGameClientView::OnStart()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);

	return;
}

//1分按钮
void CGameClientView::OnBatch()
{
	//发送消息
	AfxGetMainWnd()->SendMessage(IDM_BATCH,1,1);

	return;
}


//拖管控制
void CGameClientView::OnStusteeControl()
{
	AfxGetMainWnd()->SendMessage(IDM_TRUSTEE_CONTROL,0,0);
	return;
}
void CGameClientView::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//动作动画
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
	//动作动画
	if (nIDEvent==IDI_BATCH_START)
	{
		//停止判断
		if (m_bBatchEffect==false)
		{
			KillTimer(IDI_BATCH_START);
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbBatch[i]==1)
					m_HeapCard[i].SetCardData(0,0,HEAP_FULL_COUNT);
			}
			UpdateGameView(NULL);
			AfxGetMainWnd()->PostMessage(IDM_BATCH_END,0,0);
			return;
		}

		//设置变量
		if ((m_cbBatchFrameIndex+1)>=BATCH_EFFECT_COUNT)
		{
			//删除时间
			KillTimer(IDI_BOMB_EFFECT);

			//设置变量
			m_bBatchEffect=false;
			m_cbBatchFrameIndex=0;
		}
		else m_cbBatchFrameIndex++;

		//更新界面
		UpdateGameView(NULL);

		return;
	}
	if (nIDEvent==IDI_DISC_EFFECT)
	{
		//设置变量
		if ((m_cbDiscFrameIndex+1)>=DISC_EFFECT_COUNT)
		{
			m_cbDiscFrameIndex=0;
		}
		else m_cbDiscFrameIndex++;

		//更新界面
		UpdateGameView(NULL);

		return;

	}

	__super::OnTimer(nIDEvent);
}
//积分按钮
VOID CGameClientView::OnBnClickedScore()
{
	//设置变量
	m_bShowScore=!m_bShowScore;

	//隐藏按钮
	m_btScore.ShowWindow(SW_HIDE);

	//显示按钮
	m_btScoreClose.ShowWindow(SW_SHOW);

	//更新界面
	UpdateGameView(NULL);

	return;
}

VOID CGameClientView::OnBnClickedCloseScore()
{
	//设置变量
	m_bShowScore=!m_bShowScore;

	//隐藏按钮
	m_btScoreClose.ShowWindow(SW_HIDE);

	//显示按钮
	m_btScore.ShowWindow(SW_SHOW);

	//更新界面
	UpdateGameView(NULL);


}
//////////////////////////////////////////////////////////////////////////