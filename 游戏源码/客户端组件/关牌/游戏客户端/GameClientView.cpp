#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识

#define IDC_START						100								//开始按钮
#define IDC_OUT_CARD					101								//出牌按钮
#define IDC_PASS_CARD					102								//PASS按钮
#define IDC_OUT_PROMPT					103								//提示按钮
#define IDC_TRUSTEE_CONTROL				104								//托管控制

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_PASS_CARD, OnPassCard)
	ON_BN_CLICKED(IDC_OUT_PROMPT, OnOutPrompt)
	ON_BN_CLICKED(IDC_TRUSTEE_CONTROL, OnStusteeControl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//拖管控制
void CGameClientView::OnStusteeControl()
{
	AfxGetMainWnd()->SendMessage(IDM_TRUSTEE_CONTROL,0,0);
	return;
}

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//游戏变量
	m_lCellScore=0;
	m_cbBombCount=0;

	//状态变量
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bCardCount,0,sizeof(m_bCardCount));

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBomb.SetLoadInfo(IDB_BOMB,hInstance);
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageFill.SetLoadInfo(IDB_VIEW_FILL,hInstance);
	m_ImageUserPass.SetLoadInfo(IDB_USER_PASS,hInstance);

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
	for (WORD i=0;i<2;i++)
	{
		m_UserCardControl[i].SetDirection(true);
		m_UserCardControl[i].SetDisplayFlag(true);
		m_UserCardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,20+i);
	}

	//用户扑克
	m_HandCardControl[0].SetCardSpace(20,19,0);
	m_HandCardControl[1].SetSinkWindow(AfxGetMainWnd());
	m_HandCardControl[0].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,40);
	m_HandCardControl[1].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,41);

	//创建按钮
	m_btStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_START);
	m_btOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OUT_CARD);
	m_btPassCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_PASS_CARD);
	m_btOutPrompt.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OUT_PROMPT);
	m_btStusteeControl.Create(TEXT(""),WS_CHILD|WS_DISABLED|WS_VISIBLE|WS_CLIPSIBLINGS,CreateRect,this,IDC_TRUSTEE_CONTROL);
	m_btStusteeControl.EnableWindow(FALSE);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_START,hInstance,false);
	m_btOutCard.SetButtonImage(IDB_OUT_CARD,hInstance,false);
	m_btPassCard.SetButtonImage(IDB_PASS,hInstance,false);
	m_btOutPrompt.SetButtonImage(IDB_OUT_PROMPT,hInstance,false);
	m_btStusteeControl.SetButtonImage(IDB_START_TRUSTEE,hInstance,false);

#ifdef VIDEO_GAME
	//创建视频
	for (WORD i=0;i<2;i++)
	{
		//创建视频
		m_DlgVideoService[i].Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,CreateRect,this,200+i);
		m_DlgVideoService[i].InitVideoService(i==1,i==1);

		//设置视频
		g_VideoServiceManager.SetVideoServiceControl(i,&m_DlgVideoService[i]);
	}
#endif

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
	m_HandCardControl[1].ShootAllCard(false);

	return;
}

//重置界面
void CGameClientView::ResetGameView()
{
	//游戏变量
	m_lCellScore=0L;
	m_cbBombCount=0;

	//状态变量
	memset(m_bPass,0,sizeof(m_bPass));
	memset(m_bCardCount,0,sizeof(m_bCardCount));

	//隐藏控件
	m_ScoreView.ShowWindow(SW_HIDE);

	//隐藏按钮
	m_btStart.ShowWindow(SW_HIDE);
	m_btOutCard.ShowWindow(SW_HIDE);
	m_btPassCard.ShowWindow(SW_HIDE);
	m_btOutPrompt.ShowWindow(SW_HIDE);

	//禁用控件
	m_btOutCard.EnableWindow(FALSE);
	m_btPassCard.EnableWindow(FALSE);

	//扑克控件
	m_HandCardControl[0].SetCardData(NULL,0);
	m_HandCardControl[1].SetCardData(NULL,0);
	m_HandCardControl[0].SetPositively(false);
	m_HandCardControl[1].SetPositively(false);
	m_HandCardControl[1].SetDisplayFlag(false);
	for (WORD i=0;i<GAME_PLAYER;i++) m_UserCardControl[i].SetCardData(NULL,0);

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	m_ptFace[0].x=nWidth/2-m_nXFace/2;
	m_ptFace[0].y=m_nYBorder+5;
	m_ptName[0].x=nWidth/2+5+m_nXFace/2;
	m_ptName[0].y=m_nYBorder+5;
	m_ptTimer[0].x=nWidth/2-m_nXFace/2-m_nXTimer/2-5;
	m_ptTimer[0].y=m_nYBorder+m_nYTimer/2+5;
	m_ptReady[0].x=nWidth/2-m_nXTimer-m_nXFace/2-5;
	m_ptReady[0].y=m_nYBorder+m_nYTimer/2+10;
	m_PointUserPass[0].x=nWidth/2-21;
	m_PointUserPass[0].y=m_nYBorder+m_nYFace+210;
	m_PointUserBomb[0].x=nWidth/2-m_nXFace/2-150;
	m_PointUserBomb[0].y=m_nYBorder+m_nYFace+3;
	m_ptAuto[0].x=nWidth/2+m_nXFace+m_nXTimer+15;
	m_ptAuto[0].y=m_nYBorder+5;

	m_ptFace[1].x=nWidth/2-m_nXFace/2;
	m_ptFace[1].y=nHeight-m_nYBorder-m_nYFace-5;
	m_ptName[1].x=nWidth/2+5+m_nXFace/2;
	m_ptName[1].y=nHeight-m_nYBorder-m_nYFace-2;
	m_ptTimer[1].x=nWidth/2-m_nXFace/2-m_nXTimer/2-5;
	m_ptTimer[1].y=nHeight-m_nYTimer+20;
	m_ptReady[1].x=nWidth/2;
	m_ptReady[1].y=nHeight-m_nYBorder-m_nYTimer/2-130;
	m_PointUserPass[1].x=nWidth/2-21;
	m_PointUserPass[1].y=nHeight-m_nYBorder-m_nYFace-210;
	m_PointUserBomb[1].x=nWidth/2-m_nXFace/2-150;
	m_PointUserBomb[1].y=nHeight-m_nYBorder-m_nYFace-3;
	m_ptAuto[1].x=nWidth/2+m_nXFace+m_nXTimer+15;
	m_ptAuto[1].y=nHeight-m_nYBorder-m_nYFace-10;

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nHeight-223-m_nYBorder,0,0,uFlags);

	m_btOutCard.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btOutCard,NULL,nWidth/2-rcButton.Width()*3/2-10,nHeight-223-m_nYBorder,0,0,uFlags);
//	DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth/2-rcButton.Width()/2,nHeight-193-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPassCard,NULL,nWidth/2+rcButton.Width()/2+10,nHeight-223-m_nYBorder,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOutPrompt,NULL,nWidth,nHeight,0,0,uFlags);
	m_btStusteeControl.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStusteeControl,NULL,nWidth-m_nXBorder-(rcButton.Width()+5)*1,nHeight-m_nYBorder-5-rcButton.Height(),0,0,uFlags);

#ifdef VIDEO_GAME
	//视频窗口
	CRect rcAVDlg;
	m_DlgVideoService[0].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVideoService[0],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),m_nYBorder+5,0,0,uFlags);

	m_DlgVideoService[1].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVideoService[1],NULL,m_nXBorder+5,nHeight-m_nYBorder-3-rcAVDlg.Height(),0,0,uFlags);
#endif

	EndDeferWindowPos(hDwp);

	//移动扑克
	m_HandCardControl[0].SetBenchmarkPos(nWidth/2,m_nYBorder+m_nYFace+30,enXCenter,enYTop);
	m_HandCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-m_nYFace-m_nYBorder-30,enXCenter,enYBottom);

	//用户扑克
	m_UserCardControl[0].SetBenchmarkPos(nWidth/2,m_nYBorder+180,enXCenter,enYTop);
	m_UserCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-228-m_nYBorder,enXCenter,enYBottom);

	//积分视图
	CRect rcControl;
	m_ScoreView.GetWindowRect(&rcControl);
	m_ScoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,nHeight/2-80,0,0,SWP_NOZORDER|SWP_NOSIZE);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageFill,enMode_Spread);
	DrawViewImage(pDC,m_ImageBack,enMode_Centent);

	//绘画用户
	TCHAR szBuffer[32];
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer=GetUserTimer(i);
		const tagUserData * pUserData=GetUserInfo(i);

		///////////////////////////////////////////////
		//test
		//WORD wUserTimer = 30;
		//tagUserData *pUserData = new tagUserData;
		//lstrcpy(pUserData->szName,TEXT("测试玩家"));
		//pUserData->lScore = 10000L;
		//pUserData->cbUserStatus = US_PLAY;
		//m_bPass[i]=true;
		//m_bCardCount[i]=30;
		///////////////////////////////////////////////

		//绘画用户
		if (pUserData!=NULL)
		{
			//用户名字
			pDC->SetTextAlign(TA_LEFT|TA_TOP);
			pDC->SetTextColor((wUserTimer>0)?RGB(250,250,250):RGB(220,220,220));
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName,lstrlen(pUserData->szName));

			//剩余扑克
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("（%ld 张）"),m_bCardCount[i]);
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y+18,szBuffer,lstrlen(szBuffer));

			//其他信息
			if (wUserTimer!=0) DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
		}
			//托管标志
			if(m_bAutoPlayer[i]==true)
			{
				CPngImage ImageScore;
				ImageScore.LoadImage(AfxGetInstanceHandle(),TEXT("TRUSTEE"));
				ImageScore.DrawImage(pDC,m_ptAuto[i].x,m_ptAuto[i].y);
			}
	}

	//炸弹倍数
	if (m_cbBombCount>1)
	{
		//炸弹标志
		CImageHandle ImageHandle(&m_ImageBomb);
		m_ImageBomb.AlphaDrawImage(pDC,m_nXBorder+5,m_nYBorder+m_ImageBomb.GetHeight()+5,RGB(255,0,255));

		//设置 DC
		pDC->SetTextColor(RGB(250,250,250));
		pDC->SetTextAlign(TA_LEFT|TA_TOP);

		//炸弹倍数
		TCHAR szBuffer[64]=TEXT("");
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("炸弹数目：%d"),m_cbBombCount);
		pDC->TextOut(m_nXBorder+m_ImageBomb.GetWidth()+10,m_nYBorder+20,szBuffer,lstrlen(szBuffer));
	}

	//放弃标志
	CImageHandle ImageHnadleUserPass(&m_ImageUserPass);
	for (WORD i=0;i<2;i++)
	{
		if (m_bPass[i]==true) 
		{
			CSize sizeImage;
			sizeImage.SetSize(m_ImageUserPass.GetWidth(),m_ImageUserPass.GetHeight());
			m_ImageUserPass.AlphaDrawImage(pDC,m_PointUserPass[i].x-sizeImage.cx/2,m_PointUserPass[i].y-sizeImage.cy/2,RGB(255,0,255));
		}
	}

	//单元积分
	if (m_lCellScore>0L)
	{
		TCHAR szBuffer[128]=TEXT("");
		_snprintf(szBuffer,CountArray(szBuffer),TEXT("游戏底注：%ld"),m_lCellScore);
		pDC->SetTextColor(RGB(250,250,250));
		pDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
		pDC->TextOut(m_nXBorder+5,nHeight-5-m_nYBorder,szBuffer,lstrlen(szBuffer));
	}

	return;
}

//炸弹数目
void CGameClientView::SetBombCount(BYTE cbBombCount)
{
	//设置变量
	m_cbBombCount=cbBombCount;

	//刷新界面
	UpdateGameView(NULL);

	return;
}

//基础分数
void CGameClientView::SetCellScore(LONG lCellScore)
{
	//设置变量
	m_lCellScore=lCellScore;

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
void CGameClientView::SetAutoUser(bool bAutoUser[2])
{
	CopyMemory(m_bAutoPlayer,bAutoUser,sizeof(m_bAutoPlayer));
		
	//更新界面
	UpdateGameView(NULL);
	return;
}
//////////////////////////////////////////////////////////////////////////

