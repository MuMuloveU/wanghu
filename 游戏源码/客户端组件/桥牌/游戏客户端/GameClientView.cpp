#include "StdAfx.h"
#include "Resource.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识

//游戏按钮
#define IDC_START						100								//开始按钮
#define IDC_OUT_CARD					101								//出牌按钮
#define IDC_OUT_PROMPT					102								//提示按钮
#define IDC_SEND_CONCEAL				103								//留底按钮

#define IDC_CALL_SCORE_OK				107								//叫分确定按钮
#define IDC_GIVE_UP_SCORE				109								//放弃按钮

//功能按钮
#define IDC_CONCEAL_CARD				200								//查看底牌
#define IDC_REQUEST_LEAVE				201								//请求离开
#define IDC_LAST_TURN_CARD				202								//上轮扑克
#define IDC_TRUSTEE_CONTROL				203								//托管控制

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_OUT_PROMPT, OnOutPrompt)
	ON_BN_CLICKED(IDC_SEND_CONCEAL, OnSendConceal)
	ON_BN_CLICKED(IDC_CONCEAL_CARD, OnConcealCard)
	ON_BN_CLICKED(IDC_REQUEST_LEAVE, OnRuquestLeave)
	ON_BN_CLICKED(IDC_LAST_TURN_CARD, OnLastTurnCard)
	ON_BN_CLICKED(IDC_TRUSTEE_CONTROL, OnStusteeControl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView() : CGameFrameView()
{
	//显示变量
	m_bDeasilOrder=true;
	m_bLastTurnCard=false;

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_cbJuKuang=VALUE_ERROR;
	m_cbMainColor=VALUE_ERROR;
	m_cbMainValue=VALUE_ERROR;

	//叫牌信息
	m_cbValueOrder[0]=VALUE_ERROR;
	m_cbValueOrder[1]=VALUE_ERROR;
	m_wCallCardUser=INVALID_CHAIR;
	m_bMeChair=INVALID_CHAIR;

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBack.SetLoadInfo(IDB_LOGO,hInstance);
	m_ImageBackGround.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageColorFlag.SetLoadInfo(IDB_COLOR_FLAG,hInstance);
	m_ImageLastTurn.SetLoadInfo(IDB_LAST_TURN_TIP,hInstance);
	m_ImageUserFlag.SetLoadInfo(IDB_USER_FLAG,hInstance);
	m_ImageStationTitle.SetLoadInfo(IDB_STATION_TITLE,hInstance);
	

	//加载资源
	//CImageHandle ImageHandleStationBack(&m_ImageStationBack);
	//CImageHandle ImageHandleBackGround(&m_ImageBackGround);

	//获取大小
	m_SizeStation.cx=0;
	m_SizeStation.cy=0;

	return;
}

//析构函数
CGameClientView::~CGameClientView()
{
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建控件
	CRect rcCreate(0,0,0,0);
	m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,10);
	m_CallScore.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,13);
	m_CallScoreInfo.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,14);

	//创建扑克
	for (WORD i=0;i<4;i++)
	{
		m_UserCardControl[i].SetDirection(true);
		m_UserCardControl[i].SetDisplayFlag(true);
		m_UserCardControl[i].SetCardSpace(DEF_CARD_H_SPACE,DEF_CARD_V_SPACE,0);
		m_UserCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,20+i);
	}

	BYTE bNum=0;

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		for (WORD j=0;j<GAME_PLAYER;j++)
		{
			bNum++;
			m_HandOutCardControl[i][j].SetDirection(true);
			m_HandOutCardControl[i][j].SetDisplayFlag(true);
			m_HandOutCardControl[i][j].SetCardSpace(DEF_CARD_H_SPACE,DEF_CARD_V_SPACE,0);
			m_HandOutCardControl[i][j].Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,500+bNum);
		}
	}
	
	m_CardScore.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcCreate,this,50);
	m_HandCardControl.Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,60);

	//设置扑克
	m_CardScore.SetDisplayFlag(true);
	m_CardScore.SetCardSpace(DEF_CARD_H_SPACE,0,0);
	m_HandCardControl.SetSinkWindow(AfxGetMainWnd());

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_START);
	m_btOutCard.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_OUT_CARD);
	m_btOutPrompt.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_OUT_PROMPT);

	//功能按钮
	m_btLastTurnCard.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_LAST_TURN_CARD);
	m_btStusteeControl.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_TRUSTEE_CONTROL);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_START,hInstance,false);
	m_btOutCard.SetButtonImage(IDB_OUT_CARD,hInstance,false);
	m_btOutPrompt.SetButtonImage(IDB_OUT_PROMPT,hInstance,false);
	
	//功能按钮
	m_btLastTurnCard.SetButtonImage(IDB_LAST_TURN_CARD,hInstance,false);
	m_btStusteeControl.SetButtonImage(IDB_START_TRUSTEE,hInstance,false);

	//建立提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btLastTurnCard,TEXT("显示上一轮出牌"));
	m_ToolTipCtrl.AddTool(&m_btStusteeControl,TEXT("开始（停止）托管"));

	//读取配置
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;

#ifdef VIDEO_GAME
	//创建视频
	for (WORD i=0;i<4;i++)
	{
		//创建视频
		m_DlgVedioService[i].Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,200+i);
		m_DlgVedioService[i].InitVideoService(i==2,i==2);

		//设置视频
		g_VedioServiceManager.SetVideoServiceControl(i,&m_DlgVedioService[i]);
	}
#endif


	return 0;
}

//消息解释
BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
{
	m_ToolTipCtrl.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//重置界面
void CGameClientView::ResetGameView()
{
	//显示变量
	m_bLastTurnCard=false;

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_cbJuKuang=VALUE_ERROR;
	m_cbMainColor=VALUE_ERROR;
	m_cbMainValue=VALUE_ERROR;


	//叫牌信息
	m_cbValueOrder[0]=VALUE_ERROR;
	m_cbValueOrder[1]=VALUE_ERROR;
	m_wCallCardUser=INVALID_CHAIR;
	m_bMeChair=INVALID_CHAIR;

	//隐藏控件
	m_btStart.ShowWindow(SW_HIDE);
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btOutPrompt.ShowWindow(SW_HIDE);

	//禁用按钮
	m_btLastTurnCard.EnableWindow(FALSE);
	m_btStusteeControl.EnableWindow(FALSE);
	m_btStusteeControl.SetButtonImage(IDB_START_TRUSTEE,AfxGetInstanceHandle(),false);

	//设置扑克
	m_CardScore.SetCardData(NULL,0);
	m_HandCardControl.SetCardData(NULL,0);
	m_HandCardControl.SetPositively(false);
	m_HandCardControl.SetDisplayFlag(false);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		for (WORD j=0;j<GAME_PLAYER;j++)
		{
			m_HandOutCardControl[i][j].SetCardData(NULL,0);
		}
	}
	for (WORD i=0;i<CountArray(m_UserCardControl);i++) m_UserCardControl[i].SetCardData(NULL,0);

	//控件组件
	m_ScoreView.ShowWindow(SW_HIDE);
	m_CallScore.ShowWindow(SW_HIDE);
	m_CallScoreInfo.ShowWindow(SW_HIDE);

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//变量定义
	int nViewHeight=nHeight-m_SizeStation.cy-m_nYBorder;

	//设置坐标
	if (m_bDeasilOrder==false)
	{
		m_ptFace[1].x=nWidth-m_nXBorder-m_nXFace-5;
		m_ptFace[1].y=nHeight/2-m_nYFace-50;
		m_ptName[1].x=nWidth-m_nXBorder-40;
		m_ptName[1].y=nHeight/2-45;
		m_ptTimer[1].x=nWidth-m_nXBorder-m_nXFace-10;
		m_ptTimer[1].y=nHeight/2-m_nYFace-55-m_nYTimer;
		m_ptReady[1].x=nWidth-m_nXBorder-m_nXTimer-5;
		m_ptReady[1].y=nHeight/2-m_nYFace-55-m_nYTimer/2;
		m_PointBanker[1].x=nWidth-m_nXBorder-40;
		m_PointBanker[1].y=nHeight/2-m_nYFace-83-m_nYTimer;


		m_ptFace[3].x=m_nXBorder+30;
		m_ptFace[3].y=nHeight/2-m_nYFace-50;
		m_ptName[3].x=m_nXBorder+60;
		m_ptName[3].y=nHeight/2-45;
		m_ptTimer[3].x=m_nXBorder+30;
		m_ptTimer[3].y=nHeight/2-m_nYFace-55-m_nYTimer;
		m_ptReady[3].x=m_nXBorder+30+m_nXTimer;
		m_ptReady[3].y=nHeight/2-m_nYFace-55-m_nYTimer/2;
		m_PointBanker[3].x=m_nXBorder+30;
		m_PointBanker[3].y=nHeight/2-m_nYFace-83-m_nYTimer;
	}
	else
	{
		m_ptFace[3].x=nWidth-m_nXBorder-m_nXFace-5;
		m_ptFace[3].y=nHeight/2-m_nYFace-50;
		m_ptName[3].x=nWidth-m_nXBorder-40;
		m_ptName[3].y=nHeight/2-45;
		m_ptTimer[3].x=nWidth-m_nXBorder-m_nXFace-10;
		m_ptTimer[3].y=nHeight/2-m_nYFace-55-m_nYTimer;
		m_ptReady[3].x=nWidth-m_nXBorder-m_nXTimer-5;
		m_ptReady[3].y=nHeight/2-m_nYFace-55-m_nYTimer/2;
		m_PointBanker[3].x=nWidth-m_nXBorder-40;
		m_PointBanker[3].y=nHeight/2-m_nYFace-83-m_nYTimer;

		m_ptFace[1].x=m_nXBorder+30;
		m_ptFace[1].y=nHeight/2-m_nYFace-50;
		m_ptName[1].x=m_nXBorder+60;
		m_ptName[1].y=nHeight/2-45;
		m_ptTimer[1].x=m_nXBorder+30;
		m_ptTimer[1].y=nHeight/2-m_nYFace-55-m_nYTimer;
		m_ptReady[1].x=m_nXBorder+30+m_nXTimer;
		m_ptReady[1].y=nHeight/2-m_nYFace-55-m_nYTimer/2;
		m_PointBanker[1].x=m_nXBorder+30;
		m_PointBanker[1].y=nHeight/2-m_nYFace-83-m_nYTimer;
	}



	//设置坐标
	m_ptFace[0].x=nWidth/2-m_nXFace;
	m_ptFace[0].y=20+m_nYBorder;
	m_ptName[0].x=nWidth/2+5;
	m_ptName[0].y=20+m_nYBorder;
	m_ptTimer[0].x=nWidth/2-m_nXFace-m_nXTimer*2-5;
	m_ptTimer[0].y=20+m_nYBorder;
	m_ptReady[0].x=nWidth/2-m_nXFace-m_nXTimer-5;
	m_ptReady[0].y=20+m_nYBorder+m_nYTimer/2;
	m_PointBanker[0].x=nWidth/2-m_nXFace-m_nXTimer*2-32;
	m_PointBanker[0].y=25+m_nYBorder;

	
	m_ptFace[2].x=nWidth/2-m_nXFace/2;
	m_ptFace[2].y=nHeight-90;
	m_ptName[2].x=nWidth/2+5+m_nXFace/2;
	m_ptName[2].y=nHeight-80;
	m_ptTimer[2].x=nWidth/2-m_nXFace/2-m_nXTimer*2-5;
	m_ptTimer[2].y=nHeight-90;
	m_ptReady[2].x=nWidth/2-m_nXTimer-m_nXFace/2-5;
	m_ptReady[2].y=nHeight-90;
	m_PointBanker[2].x=nWidth/2-m_nXFace/2-m_nXTimer*2-32;
	m_PointBanker[2].y=nHeight-90;


	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//开始按钮
	m_btStart.GetWindowRect(&rcButton);
	//DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nViewHeight-225,0,0,uFlags);
	DeferWindowPos(hDwp,m_btStart,NULL,nWidth/2,nHeight-230,0,0,uFlags);

	//出牌按钮
	m_btOutCard.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2+10,nViewHeight-225,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth/2-rcButton.Width()-10,nViewHeight-225,0,0,uFlags);

	//功能按钮
	m_btStusteeControl.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStusteeControl,NULL,nWidth-m_nXBorder-(rcButton.Width()+5)*3,nViewHeight-50-rcButton.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btLastTurnCard,NULL,nWidth-m_nXBorder-(rcButton.Width()+5)*4,nViewHeight-50-rcButton.Height(),0,0,uFlags);
	
	//视频窗口
#ifdef VIDEO_GAME

	CRect rcAVDlg;
	m_DlgVedioService[0].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVedioService[1],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),nHeight/2+90,0,0,uFlags);
	DeferWindowPos(hDwp,m_DlgVedioService[3],NULL,m_nXBorder+5,nHeight/9+30,0,0,uFlags);
	DeferWindowPos(hDwp,m_DlgVedioService[0],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),5,0,0,uFlags);
	m_DlgVedioService[2].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVedioService[2],NULL,m_nXBorder+5,nHeight-m_nYBorder-3-rcAVDlg.Height(),0,0,uFlags);
#endif

	//移动控件
	EndDeferWindowPos(hDwp);

	//移动扑克
	//m_CardScore.SetBenchmarkPos(m_nXBorder+100,m_nYBorder-15,enXLeft,enYTop);  //分数牌
	m_UserCardControl[0].SetBenchmarkPos(nWidth/2,m_nYBorder+m_nYFace+130,enXCenter,enYTop);
	m_UserCardControl[2].SetBenchmarkPos(nWidth/2,nViewHeight-223-m_nYBorder,enXCenter,enYBottom);
	

	//m_HandOutCardControl[0].SetBenchmarkPos(nWidth/2-200,nViewHeight-233-m_nYBorder,enXCenter,enYBottom);
	//m_HandOutCardControl[3].SetBenchmarkPos(nWidth/2,nViewHeight-150-m_nYBorder,enXCenter,enYBottom);
	//明手扑克
	m_HandOutCardControl[0][0].SetBenchmarkPos(nWidth/2+10,m_nYBorder+m_nYFace+15,enXCenter,enYTop);
	m_HandOutCardControl[0][1].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-60,nViewHeight/2-45,enXCenter,enYCenter);
	m_HandOutCardControl[0][2].SetBenchmarkPos(nWidth/2-35,nViewHeight-m_nYFace-m_nYBorder-75,enXCenter,enYBottom);
	m_HandOutCardControl[0][3].SetBenchmarkPos(m_nXBorder+m_nXFace+90,nViewHeight/2-45,enXCenter,enYCenter);

	//明手扑克
	m_HandOutCardControl[1][0].SetBenchmarkPos(nWidth/2+10,m_nYBorder+m_nYFace+15,enXCenter,enYTop);
	m_HandOutCardControl[1][1].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-60,nViewHeight/2-45,enXCenter,enYCenter);
	m_HandOutCardControl[1][2].SetBenchmarkPos(nWidth/2-35,nViewHeight-m_nYFace-m_nYBorder-75,enXCenter,enYBottom);
	m_HandOutCardControl[1][3].SetBenchmarkPos(m_nXBorder+m_nXFace+90,nViewHeight/2-45,enXCenter,enYCenter);

	
	//明手扑克
	m_HandOutCardControl[2][0].SetBenchmarkPos(nWidth/2+10,m_nYBorder+m_nYFace+15,enXCenter,enYTop);
	m_HandOutCardControl[2][1].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-60,nViewHeight/2-45,enXCenter,enYCenter);
	m_HandOutCardControl[2][2].SetBenchmarkPos(nWidth/2-35,nViewHeight-m_nYFace-m_nYBorder-75,enXCenter,enYBottom);
	m_HandOutCardControl[2][3].SetBenchmarkPos(m_nXBorder+m_nXFace+90,nViewHeight/2-45,enXCenter,enYCenter);

	//明手扑克
	m_HandOutCardControl[3][0].SetBenchmarkPos(nWidth/2+10,m_nYBorder+m_nYFace+15,enXCenter,enYTop);
	m_HandOutCardControl[3][1].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-60,nViewHeight/2-45,enXCenter,enYCenter);
	m_HandOutCardControl[3][2].SetBenchmarkPos(nWidth/2-35,nViewHeight-m_nYFace-m_nYBorder-75,enXCenter,enYBottom);
	m_HandOutCardControl[3][3].SetBenchmarkPos(m_nXBorder+m_nXFace+90,nViewHeight/2-45,enXCenter,enYCenter);
	

	m_HandCardControl.SetBenchmarkPos(nWidth/2,nViewHeight-m_nYFace-m_nYBorder-50,enXCenter,enYBottom);  //手上牌

	//用户扑克
	//if (m_bDeasilOrder==true)
	//{
		m_UserCardControl[1].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-120,nViewHeight/2-65,enXRight,enYCenter);
		m_UserCardControl[3].SetBenchmarkPos(m_nXBorder+m_nXFace+140,nViewHeight/2-65,enXLeft,enYCenter);

	//	m_HandOutCardControl[1].SetBenchmarkPos(nWidth-m_nXBorder-m_nXFace-50,nHeight/2-50,enXRight,enYCenter);
	//	m_HandOutCardControl[2].SetBenchmarkPos(m_nXBorder+m_nXFace+50,nHeight/2-55,enXLeft,enYCenter);
	//}
	//else 
	//{
		//m_UserCardControl[3].SetBenchmarkPos(nWidth-m_nXFace-m_nXBorder-120,nViewHeight/2-65,enXRight,enYCenter);
		//m_UserCardControl[1].SetBenchmarkPos(m_nXBorder+m_nXFace+140,nViewHeight/2-65,enXLeft,enYCenter);

	//	m_HandOutCardControl[2].SetBenchmarkPos(nWidth-m_nXBorder-m_nXFace-50,nHeight/2-50,enXRight,enYCenter);
	//	m_HandOutCardControl[1].SetBenchmarkPos(m_nXBorder+m_nXFace+50,nHeight/2-55,enXLeft,enYCenter);
	//}


	//积分视图
	CRect rcScore;
	m_ScoreView.GetWindowRect(&rcScore);
	m_ScoreView.SetWindowPos(NULL,(nWidth-rcScore.Width())/2,nViewHeight/2-160,0,0,SWP_NOZORDER|SWP_NOSIZE);

	
	//叫分窗口
	CRect rcCallScore;
	m_CallScore.GetWindowRect(&rcCallScore);
	m_CallScore.SetWindowPos(NULL,(nWidth-rcCallScore.Width())/2+120,nViewHeight/2-160,0,0,SWP_NOZORDER|SWP_NOSIZE);


	//叫分历史信息窗口
	CRect rcCallScoreInfo;
	m_CallScoreInfo.GetWindowRect(&rcCallScoreInfo);
	m_CallScoreInfo.SetWindowPos(NULL,(nWidth-rcCallScoreInfo.Width())/2-120,nViewHeight/2-160,0,0,SWP_NOZORDER|SWP_NOSIZE);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBackGround,enMode_Spread);
	DrawViewImage(pDC,m_ImageBack,enMode_Centent);
	//CImageHandle m_ImageBackBack(&m_ImageBack);
	//SetStretchBltMode(pDC->m_hDC,STRETCH_HALFTONE);
	//m_ImageBack.StretchBlt(pDC->m_hDC,CRect(0,0,nWidth,nHeight),CRect(0,0,m_ImageBack.GetWidth()/2,m_ImageBack.GetHeight()/2));
	
	
	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer=GetUserTimer(i);
		const tagUserData * pUserData=GetUserInfo(i);

		//绘画用户
		if (pUserData!=NULL)
		{
			//用户名字
			pDC->SetTextAlign(((i==1)&&(m_bDeasilOrder==true))||((i==3)&&(m_bDeasilOrder==false))?TA_RIGHT:TA_LEFT);
			DrawTextString(pDC,pUserData->szName,RGB(240,240,240),RGB(50,50,50),m_ptName[i].x,m_ptName[i].y);

			//其他信息
			if (wUserTimer!=0) DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
		}
	}

	//状态界面
	CImageHandle ImageHandleStationTitle(&m_ImageStationTitle);
	m_ImageStationTitle.AlphaDrawImage(pDC,m_nXBorder+25,m_nYBorder+15,RGB(255,0,255));

	//设置局况
	if (m_cbJuKuang!=VALUE_ERROR)
	{
		TCHAR szJuKuang[16]=TEXT("");
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->SetTextColor(RGB(240,240,240));
		switch (m_cbJuKuang)
		{
			//双 无
		case 1:
			{
				_snprintf(szJuKuang,CountArray(szJuKuang),TEXT("-"));
				break;
			}
			//南 北
		case 2:
			{
				_snprintf(szJuKuang,CountArray(szJuKuang),TEXT("NS"));
				break;
			}
			//东 西
		case 3:
			{
				_snprintf(szJuKuang,CountArray(szJuKuang),TEXT("EW"));
				break;
			}
			//双 有
		case 4:
			{
				_snprintf(szJuKuang,CountArray(szJuKuang),TEXT("B"));
				break;
			}
		}
		DrawTextString(pDC,szJuKuang,RGB(240,240,240),RGB(50,50,50),m_nXBorder+85,m_nYBorder+20);
	}



	//双方得分
	TCHAR szBankerValueOrder[16]=TEXT("");
	pDC->SetTextAlign(TA_LEFT|TA_TOP);
	pDC->SetTextColor(RGB(240,240,240));
	_snprintf(szBankerValueOrder,CountArray(szBankerValueOrder),TEXT("%ld"),m_cbValueOrder[0]);
	DrawTextString(pDC,szBankerValueOrder,RGB(240,240,240),RGB(50,50,50),m_nXBorder+72,m_nYBorder+88);

	TCHAR szOtherValueOrder[16]=TEXT("");
	pDC->SetTextAlign(TA_LEFT|TA_TOP);
	pDC->SetTextColor(RGB(240,240,240));
	_snprintf(szOtherValueOrder,CountArray(szOtherValueOrder),TEXT("：%ld"),m_cbValueOrder[1]);
	DrawTextString(pDC,szOtherValueOrder,RGB(240,240,240),RGB(50,50,50),m_nXBorder+78,m_nYBorder+88);
	
	//定约信息
	if (m_cbMainColor!=COLOR_ERROR||m_cbMainValue!=VALUE_ERROR)
	{
		//加载资源
		CImageHandle HandleColorFlag(&m_ImageColorFlag);
		int nColorImageWidth=m_ImageColorFlag.GetWidth()/6;
		int nColorImageHeight=m_ImageColorFlag.GetHeight();

		//绘画标志
		int nXPos=m_nXBorder+87;
		int nYPos=m_nYBorder+40;
		m_ImageColorFlag.AlphaDrawImage(pDC,nXPos,nYPos,nColorImageWidth,nColorImageHeight,nColorImageWidth*(m_cbMainColor>>4),0,RGB(255,0,255));
		m_ImageColorFlag.AlphaDrawImage(pDC,nXPos-17,nYPos+25,nColorImageWidth,nColorImageHeight,nColorImageWidth*(m_cbMainColor>>4),0,RGB(255,0,255));
		

		TCHAR szMainValue[16]=TEXT("");
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->SetTextColor(RGB(240,240,240));
		_snprintf(szMainValue,CountArray(szMainValue),TEXT("%ld"),m_cbMainValue);
		DrawTextString(pDC,szMainValue,RGB(240,240,240),RGB(50,50,50),nXPos-15,nYPos+3);
	}

	//用户标志
	if (m_bMeChair!=INVALID_CHAIR)
	{
		//加载位图（固定状态）
		CImageHandle ImageHandle(&m_ImageUserFlag);
		int nImageWidth=m_ImageUserFlag.GetWidth()/4;
		int nImageHeight=m_ImageUserFlag.GetHeight();
		
		switch(m_bMeChair)
		{
		case 0:
			{
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[2].x,m_PointBanker[2].y,nImageWidth,nImageHeight,nImageWidth*3,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[0].x,m_PointBanker[0].y,nImageWidth,nImageHeight,nImageWidth*1,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[1].x,m_PointBanker[1].y,nImageWidth,nImageHeight,nImageWidth*2,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[3].x,m_PointBanker[3].y,nImageWidth,nImageHeight,nImageWidth*0,0);
				break;
			}
		case 1:
			{
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[2].x,m_PointBanker[2].y,nImageWidth,nImageHeight,nImageWidth*0,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[0].x,m_PointBanker[0].y,nImageWidth,nImageHeight,nImageWidth*2,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[1].x,m_PointBanker[1].y,nImageWidth,nImageHeight,nImageWidth*3,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[3].x,m_PointBanker[3].y,nImageWidth,nImageHeight,nImageWidth*1,0);
				break;
			}
		case 2:
			{
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[2].x,m_PointBanker[2].y,nImageWidth,nImageHeight,nImageWidth*1,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[0].x,m_PointBanker[0].y,nImageWidth,nImageHeight,nImageWidth*3,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[1].x,m_PointBanker[1].y,nImageWidth,nImageHeight,nImageWidth*0,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[3].x,m_PointBanker[3].y,nImageWidth,nImageHeight,nImageWidth*2,0);
				break;
			}
		case 3:
			{
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[2].x,m_PointBanker[2].y,nImageWidth,nImageHeight,nImageWidth*2,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[0].x,m_PointBanker[0].y,nImageWidth,nImageHeight,nImageWidth*0,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[1].x,m_PointBanker[1].y,nImageWidth,nImageHeight,nImageWidth*1,0);
				m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[3].x,m_PointBanker[3].y,nImageWidth,nImageHeight,nImageWidth*3,0);
				break;
			}
		}

		/*
		//绘画标志(根据庄家来画状态)
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			WORD wIndex=((i+GAME_PLAYER)-m_wBankerUser)%GAME_PLAYER;
			m_ImageUserFlag.BitBlt(pDC->m_hDC,m_PointBanker[i].x,m_PointBanker[i].y,nImageWidth,nImageHeight,nImageWidth*wIndex,0);
		}
		*/
	}
	//上轮标志
	if (m_bLastTurnCard==true)
	{
		//加载资源
		CImageHandle HandleLastTurn(&m_ImageLastTurn);

		//绘画标志
		int nImageWidth=m_ImageLastTurn.GetWidth();
		int nImageHegith=m_ImageLastTurn.GetHeight();
		m_ImageLastTurn.AlphaDrawImage(pDC,(nWidth-nImageWidth)/2,(nHeight-m_SizeStation.cy-nImageHegith)/2-65,RGB(255,0,255));
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

//自己位置
void CGameClientView::SetMeChair(WORD wMeChair)
{
	//设置用户
	if (wMeChair!=m_bMeChair)
	{
		//设置变量
		m_bMeChair=wMeChair;

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


//设置上轮
void CGameClientView::SetLastTurnCard(bool bLastTurnCard)
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


//设置定约分
void CGameClientView::SetValueOrder(BYTE cbValueBanker, BYTE cbValueOther)
{
	//设置变量
	m_cbValueOrder[0]=cbValueBanker;
	m_cbValueOrder[1]=cbValueOther;

	//更新界面
	UpdateGameView(NULL);

	return;
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


//开始按钮
void CGameClientView::OnStart()
{
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	
	return;
}

//出牌按钮
void CGameClientView::OnOutCard()
{
	AfxGetMainWnd()->SendMessage(IDM_OUT_CARD,1,1);
	return;
}

//提示按钮
void CGameClientView::OnOutPrompt()
{
	AfxGetMainWnd()->SendMessage(IDM_OUT_PROMPT,0,0);
	return;
}

//留底按钮
void CGameClientView::OnSendConceal()
{
	AfxGetMainWnd()->SendMessage(IDM_SEND_CONCEAL,0,0);
	return;
}

//查看底牌
void CGameClientView::OnConcealCard()
{
	AfxGetMainWnd()->SendMessage(IDM_CONCEAL_CARD,0,0);
	return;
}

//请求离开
void CGameClientView::OnRuquestLeave()
{
	AfxGetMainWnd()->SendMessage(IDM_REQUEST_LEAVE,0,0);
	return;
}

//上轮扑克
void CGameClientView::OnLastTurnCard()
{
	AfxGetMainWnd()->SendMessage(IDM_LAST_TURN_CARD,0,0);
	return;
}

//拖管控制
void CGameClientView::OnStusteeControl()
{
	AfxGetMainWnd()->SendMessage(IDM_TRUSTEE_CONTROL,0,0);
	return;
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


//设置局况
void  CGameClientView::SetJuKuang(int nJuKuang)
{

	if(m_cbJuKuang!=nJuKuang)
	{
		//设置变量
		m_cbJuKuang=nJuKuang;
	
		//更新界面
		UpdateGameView(NULL);
	}

	return;
}


//设置定约
void  CGameClientView::SetDingYue(BYTE cbMainValue,BYTE cbMainColor)
{

	if (m_cbMainValue!=cbMainValue||m_cbMainColor!=cbMainColor)
	{
		//设置变量
		m_cbMainValue=cbMainValue;
		m_cbMainColor=cbMainColor;
		
		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
