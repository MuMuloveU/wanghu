#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识 
#define IDC_START						100								//开始按钮
#define IDC_ADD_GOLD					101								//加注按钮
#define IDC_GIVE_UP						102								//放弃按钮
#define IDC_FOLLOW						103								//跟注按钮
#define IDC_SHOW_HAND					104								//梭哈按钮

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_FOLLOW, OnFollow)
	ON_BN_CLICKED(IDC_GIVE_UP, OnGiveUp)
	ON_BN_CLICKED(IDC_ADD_GOLD, OnAddGold)
	ON_BN_CLICKED(IDC_SHOW_HAND, OnShowHand)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//设置变量
	m_lMaxGold=0L;
	m_lBasicGold=0L;

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	
	m_ImageBarMid.SetLoadInfo(IDB_BAR_MID,hInstance);
	m_ImageBarLeft.SetLoadInfo(IDB_BAR_LEFT,hInstance);
	m_ImageBarRight.SetLoadInfo(IDB_BAR_RIGHT,hInstance);
	m_ImageTitle.SetLoadInfo(IDB_TITLE,hInstance);
	m_ImageBackGround.SetLoadInfo(IDB_BACK_GROUND,hInstance);
	m_ImageHeadBg.SetLoadInfo(IDB_HEAD_BG,hInstance);
	
	if (GetSystemMetrics(SM_CXSCREEN)>=1024) 
	{	
		m_ImageGlodTable.SetLoadInfo(IDB_GLOD_TABLE,hInstance);
		m_ImageTotal.SetLoadInfo(IDB_TOTAL,hInstance);
	}else
	{
		m_ImageGlodTable.SetLoadInfo(IDB_GLOD_TABLE,hInstance);
		m_ImageTotal.SetLoadInfo(IDB_TOTAL_MINI,hInstance);
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
	m_GoldControl.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS,CreateRect,this,8,NULL);
	m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,CreateRect,this,9);
	for (int i=0;i<4;i++) m_CardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,10+i);

	//创建按钮
	m_btStart.Create(NULL,WS_CHILD,CreateRect,this,IDC_START);
	m_btFollow.Create(NULL,WS_CHILD,CreateRect,this,IDC_FOLLOW);
	m_btGiveUp.Create(NULL,WS_CHILD,CreateRect,this,IDC_GIVE_UP);
	m_btAddGold.Create(NULL,WS_CHILD,CreateRect,this,IDC_ADD_GOLD);
	m_btShowHand.Create(NULL,WS_CHILD,CreateRect,this,IDC_SHOW_HAND);

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_BT_STAR,hInstance,false);
	m_btFollow.SetButtonImage(IDB_BT_FOLLOW,hInstance,false);
	m_btGiveUp.SetButtonImage(IDB_BT_GIVE_UP,hInstance,false);
	m_btAddGold.SetButtonImage(IDB_BT_ADD_GOLD,hInstance,false);
	m_btShowHand.SetButtonImage(IDB_BT_SHOW_HAND,hInstance,false);

#ifdef VIDEO_GAME
	//创建视频
	for (WORD i=0;i<4;i++)
	{
		//创建视频
		m_DlgVedioService[i].Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CreateRect,this,200+i);
		m_DlgVedioService[i].InitVideoService(i==2,i==2);

		//设置视频
		g_VedioServiceManager.SetVideoServiceControl(i,&m_DlgVedioService[i]);
	}
#endif


	return 0;
}

//开始按钮
void CGameClientView::OnStart()
{
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	return;
}

//跟注按钮
void CGameClientView::OnFollow()
{
	AfxGetMainWnd()->SendMessage(IDM_FOLLOW,0,0);
	return;
}

//放弃按钮
void CGameClientView::OnGiveUp()
{
	AfxGetMainWnd()->SendMessage(IDM_GIVE_UP,0,0);
	return;
}

//加注按钮
void CGameClientView::OnAddGold()
{
	AfxGetMainWnd()->SendMessage(IDM_ADD_GOLD,0,0);
	return;
}

//梭哈按钮
void CGameClientView::OnShowHand()
{
	AfxGetMainWnd()->SendMessage(IDM_SHOWHAND,0,0);
	return;
}

//重置界面
void CGameClientView::ResetGameView()
{
	//设置变量
	m_lMaxGold=0L;
	m_lBasicGold=0L;

	//设置控件
	m_btStart.ShowWindow(SW_HIDE);
	m_btFollow.ShowWindow(SW_HIDE);
	m_btGiveUp.ShowWindow(SW_HIDE);
	m_btAddGold.ShowWindow(SW_HIDE);
	m_btShowHand.ShowWindow(SW_HIDE);
	m_ScoreView.ShowWindow(SW_HIDE);
	m_GoldControl.ShowWindow(SW_HIDE);
	for (int i=0;i<CountArray(m_GoldView);i++) m_GoldView[i].SetGold(0L);
	for (int i=0;i<CountArray(m_CardControl);i++) m_CardControl[i].SetCardData(NULL,0);

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//设置坐标
	m_ptFace[0].x=nWidth/2+75;
	m_ptFace[0].y=20+m_nYBorder;
	m_ptName[0].x=nWidth/2+75+m_nXFace;
	m_ptName[0].y=20+m_nYBorder;
	m_ptTimer[0].x=nWidth/2+75;
	m_ptTimer[0].y=20+m_nYBorder;
	m_ptReady[0].x=nWidth/2+55+m_nXTimer;
	m_ptReady[0].y=30+m_nYBorder;

	m_ptFace[1].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptFace[1].y=nHeight/2-m_nYFace-50;
	m_ptName[1].x=nWidth-m_nXBorder-m_nXFace-10;
	m_ptName[1].y=nHeight/2-m_nYFace-50;
	m_ptTimer[1].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptTimer[1].y=nHeight/2-m_nYFace-55-m_nYTimer;
	m_ptReady[1].x=nWidth-m_nXBorder-m_nXTimer-5;
	m_ptReady[1].y=nHeight/2-m_nYFace-55-m_nYTimer/2;

	m_ptFace[2].x=nWidth/2+72;
	m_ptFace[2].y=nHeight-m_nYBorder-m_nYFace-50;
	m_ptName[2].x=nWidth/2+77+m_nXFace;
	m_ptName[2].y=nHeight-m_nYBorder-m_nYFace-50;
	m_ptTimer[2].x=nWidth/2+72;
	m_ptTimer[2].y=nHeight-m_nYBorder-m_nYFace-55-m_nYTimer;
	m_ptReady[2].x=nWidth/2+72+m_nXTimer;
	m_ptReady[2].y=nHeight-m_nYBorder-m_nYFace-50-m_nYTimer/2;

	m_ptFace[3].x=m_nXBorder+30;
	m_ptFace[3].y=nHeight/2-m_nYFace-50;
	m_ptName[3].x=m_nXBorder+m_nXFace+40;
	m_ptName[3].y=nHeight/2-m_nYFace-50;
	m_ptTimer[3].x=m_nXBorder+30;
	m_ptTimer[3].y=nHeight/2-m_nYFace-55-m_nYTimer;
	m_ptReady[3].x=m_nXBorder+30+m_nXTimer;
	m_ptReady[3].y=nHeight/2-m_nYFace-55-m_nYTimer/2;

	//筹码控件
	int nXCenterPos=nWidth/2;
	int nYCenterPos=nHeight/2;
	int nXmove=52;
	int nYmove=145;
	if (GetSystemMetrics(SM_CXSCREEN)>=1024)
	{
		m_GoldStation[0].SetPoint(nXCenterPos-nXmove,nYCenterPos-nYmove);
		m_GoldStation[1].SetPoint(nXCenterPos+nXmove,nYCenterPos-nYmove);

		m_GoldStation[2].SetPoint(nXCenterPos+nYmove,nYCenterPos-nXmove);
		m_GoldStation[3].SetPoint(nXCenterPos+nYmove,nYCenterPos+nXmove);

		m_GoldStation[4].SetPoint(nXCenterPos+nXmove,nYCenterPos+nYmove);
		m_GoldStation[5].SetPoint(nXCenterPos-nXmove,nYCenterPos+nYmove);

		m_GoldStation[6].SetPoint(nXCenterPos-nYmove,nYCenterPos+nXmove);
		m_GoldStation[7].SetPoint(nXCenterPos-nYmove,nYCenterPos-nXmove);
	}
	else
	{
		m_GoldStation[0].SetPoint(nXCenterPos-32,nYCenterPos-92);
		m_GoldStation[1].SetPoint(nXCenterPos+28,nYCenterPos-92);
		m_GoldStation[2].SetPoint(nXCenterPos+96,nYCenterPos-27);
		m_GoldStation[3].SetPoint(nXCenterPos+96,nYCenterPos+33);
		m_GoldStation[4].SetPoint(nXCenterPos+28,nYCenterPos+93);
		m_GoldStation[5].SetPoint(nXCenterPos-32,nYCenterPos+93);
		m_GoldStation[6].SetPoint(nXCenterPos-94,nYCenterPos+33);
		m_GoldStation[7].SetPoint(nXCenterPos-94,nYCenterPos-27);
	}

	//按钮控件
	HDWP hDwp=BeginDeferWindowPos(5);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	//DeferWindowPos(hDwp,m_btStart,NULL,nWidth-m_nXBorder-110,nHeight-m_nYBorder-105,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btStart,NULL,nWidth/2-39,nHeight/2-39,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btAddGold,NULL,nWidth-m_nXBorder-75,nHeight-m_nYBorder-143,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btFollow,NULL,nWidth-m_nXBorder-85,nHeight-m_nYBorder-80,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btGiveUp,NULL,nWidth-m_nXBorder-43,nHeight-m_nYBorder-38,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btShowHand,NULL,nWidth-m_nXBorder-155,nHeight-m_nYBorder-71,0,0,uFlags);
	DeferWindowPos(hDwp,m_btStart,NULL,nWidth-m_nXBorder-100,nHeight-m_nYBorder-105,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAddGold,NULL,nWidth-m_nXBorder-65,nHeight-m_nYBorder-140,0,0,uFlags);
	DeferWindowPos(hDwp,m_btFollow,NULL,nWidth-m_nXBorder-65,nHeight-m_nYBorder-110,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUp,NULL,nWidth-m_nXBorder-65,nHeight-m_nYBorder-80,0,0,uFlags);
	//DeferWindowPos(hDwp,m_btShowHand,NULL,nWidth-m_nXBorder-155,nHeight-m_nYBorder-71,0,0,uFlags);

#ifdef VIDEO_GAME

	CRect rcAVDlg;
	m_DlgVedioService[0].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVedioService[1],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),nHeight/2+90,0,0,uFlags);
	DeferWindowPos(hDwp,m_DlgVedioService[3],NULL,m_nXBorder+5,nHeight/20+30,0,0,uFlags);
	DeferWindowPos(hDwp,m_DlgVedioService[0],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),5,0,0,uFlags);
	m_DlgVedioService[2].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVedioService[2],NULL,m_nXBorder+5,nHeight-m_nYBorder-3-rcAVDlg.Height(),0,0,uFlags);
#endif

	EndDeferWindowPos(hDwp);

	//成绩窗口
	CRect rcScoreView;
	m_ScoreView.GetWindowRect(&rcScoreView);
	m_ScoreView.MoveWindow((nWidth-rcScoreView.Width())/2,(nHeight-rcScoreView.Height())/2-10,rcScoreView.Width(),rcScoreView.Height());
	//m_ScoreView.MoveWindow	(		
	//							(nWidth-rcScoreView.Width())/2,(nHeight-rcScoreView.Height())/2-115,
	//							rcScoreView.Width(),rcScoreView.Height()						
	//							);

	//加注窗口
	m_GoldControl.SetBasicPoint(nWidth-80-m_nXBorder,nHeight-85-m_nYBorder);

	//扑克控件
	m_CardControl[0].SetBasicStation(CPoint(nWidth/2,22+m_nYBorder),enXCenter,enYTop);
	m_CardControl[1].SetBasicStation(CPoint(nWidth-m_nXBorder-5,nHeight/2),enXRight,enYCenter);
	m_CardControl[2].SetBasicStation(CPoint(nWidth/2,nHeight-40-m_nYBorder),enXCenter,enYBottom);
	m_CardControl[3].SetBasicStation(CPoint(m_nXBorder+30,nHeight/2),enXLeft,enYCenter);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//变量定义
	CImageHandle HandleTitle(&m_ImageTitle);
	//CImageHandle HandleBarMid(&m_ImageBarMid);
	CImageHandle HandleBarLeft(&m_ImageBarLeft);
	CImageHandle HandleBarRight(&m_ImageBarRight);
	CImageHandle HandleGlodTable(&m_ImageGlodTable);
	CImageHandle HandleHeadBg(&m_ImageHeadBg);
	CImageHandle HandleTotal(&m_ImageTotal);
	//绘画背景
	//DrawViewImage(pDC,m_ImageBackGround,enMode_Centent);

	DrawViewImage(pDC,m_ImageBackGround,enMode_Spread);
	DrawViewImage(pDC,m_ImageGlodTable,enMode_Centent);
	
	
	//m_ImageGlodTable.BitBlt(pDC->m_hDC,(nWidth-m_ImageGlodTable.GetWidth())/2,(nHeight-m_ImageGlodTable.GetHeight())/2);

	//控制区域
	//m_ImageBarLeft.BitBlt(pDC->m_hDC,m_nXBorder,nHeight-m_ImageBarLeft.GetHeight()-m_nYBorder);
	//m_ImageBarMid.StretchBlt(pDC->m_hDC,m_nXBorder+m_ImageBarLeft.GetWidth(),nHeight-m_ImageBarMid.GetHeight()-m_nYBorder,nWidth-2*m_nXBorder-m_ImageBarLeft.GetWidth()-m_ImageBarRight.GetWidth(),m_ImageBarMid.GetHeight());
	//m_ImageBarRight.BitBlt(pDC->m_hDC,nWidth-m_nXBorder-m_ImageBarRight.GetWidth(),/*m_ImageBarRight.GetHeight()+*/m_nYBorder);
	m_ImageBarRight.AlphaDrawImage(pDC,nWidth-m_nXBorder-m_ImageBarRight.GetWidth(),m_nYBorder,RGB(255,0,255));
	m_ImageBarLeft.AlphaDrawImage(pDC,m_nXBorder,nHeight-m_ImageBarLeft.GetHeight()-m_nYBorder,RGB(255,0,255));
	//左上角
	//m_ImageTitle.BitBlt(pDC->m_hDC,m_nXBorder,m_nYBorder);
	m_ImageTitle.AlphaDrawImage(pDC,m_nXBorder+20,m_nYBorder+10,RGB(255,0,255));

	m_ImageTotal.AlphaDrawImage(pDC,nWidth/2-m_ImageTotal.GetWidth()/2,nHeight/2-m_ImageTotal.GetHeight()/2,RGB(255,0,255));
	//绘画用户
	WORD wUserTimer=0;
	TCHAR szBuffer[64];

	pDC->SetTextColor(RGB(255,255,0));
	for (WORD i=0;i<4;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);
		if (pUserData!=NULL)
		{
			//m_ImageHeadBg.BitBlt(pDC->m_hDC,m_ptFace[i].x-2,m_ptFace[i].y-2);
			//用户名字
			pDC->SetTextAlign((i==1)?(TA_RIGHT|TA_TOP):(TA_LEFT|TA_TOP));
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName,lstrlen(pUserData->szName));

			//用户金币
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("￥%ld"),pUserData->lScore);
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y+16,szBuffer,lstrlen(szBuffer));

			//其他信息
			wUserTimer=GetUserTimer(i);
			if (wUserTimer!=0) DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);

		}
	}

	//游戏信息
	const UINT uDrawFormat=DT_END_ELLIPSIS|DT_CENTER|DT_SINGLELINE|DT_VCENTER;
	pDC->SetTextAlign(TA_LEFT|TA_TOP);
	pDC->SetTextColor(RGB(255,255,255));
	if (m_lMaxGold!=0) 
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("￥%ld"),m_lMaxGold);
		DrawText(pDC->GetSafeHdc(),szBuffer,lstrlen(szBuffer),CRect(91,25,167,40),uDrawFormat);
		//DrawText(pDC->GetSafeHdc(),szBuffer,lstrlen(szBuffer),CRect(86,28,162,43),uDrawFormat);
	}
	if (m_lBasicGold!=0) 
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("￥%ld"),m_lBasicGold);
		//DrawText(pDC->GetSafeHdc(),szBuffer,lstrlen(szBuffer),CRect(86,35,162,50),uDrawFormat);
		DrawText(pDC->GetSafeHdc(),szBuffer,lstrlen(szBuffer),CRect(91,47,167,62),uDrawFormat);
	}

	//绘画筹码
	LONG lTotalGold=0L;
	for (DWORD i=0;i<CountArray(m_GoldView);i++)
	{
		lTotalGold+=m_GoldView[i].GetGold();
		m_GoldView[i].DrawGoldView(pDC,m_GoldStation[i].x,m_GoldStation[i].y);
	}

	//总注信息
	if (lTotalGold!=0L)
	{
		CFont ViewFont;
		ViewFont.CreateFont(-24,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("楷体_GB2312"));
		CFont * pOldFont=pDC->SelectObject(&ViewFont);
		pDC->SetTextColor(RGB(0,0,250));
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),lTotalGold);
		pDC->TextOut(nWidth/2,nHeight/2-12,szBuffer,lstrlen(szBuffer));

		pDC->SetTextColor(RGB(255,0,0));
		pDC->TextOut(nWidth/2+1,nHeight/2-12+1,szBuffer,lstrlen(szBuffer));
		pDC->TextOut(nWidth/2+1,nHeight/2-12-1,szBuffer,lstrlen(szBuffer));
		pDC->TextOut(nWidth/2+1,nHeight/2-12+0,szBuffer,lstrlen(szBuffer));
		pDC->TextOut(nWidth/2-1,nHeight/2-12+1,szBuffer,lstrlen(szBuffer));
		pDC->TextOut(nWidth/2-1,nHeight/2-12-1,szBuffer,lstrlen(szBuffer));
		pDC->TextOut(nWidth/2-1,nHeight/2-12+0,szBuffer,lstrlen(szBuffer));
		pDC->TextOut(nWidth/2+0,nHeight/2-12+1,szBuffer,lstrlen(szBuffer));
		pDC->TextOut(nWidth/2+0,nHeight/2-12-1,szBuffer,lstrlen(szBuffer));

		pDC->SelectObject(pOldFont);
		ViewFont.DeleteObject();
	}
	return;
}

//金币标题
void CGameClientView::SetGoldTitleInfo(DWORD lMaxGold, DWORD lBasicGold)
{
	//设置变量
	m_lMaxGold=lMaxGold;
	m_lBasicGold=lBasicGold;

	//更新视图
	UpdateGameView(NULL);

	return;
}

//用户筹码
void CGameClientView::SetUserGoldInfo(WORD wViewChairID, bool bTableGold, DWORD dwGold)
{
	//参数判断
	WORD wGoldPos=wViewChairID*2+((bTableGold==true)?1:0);
	if (wGoldPos>=CountArray(m_GoldStation)) return;

	//设置变量
	DWORD m_dwAllGold=0L;
	m_GoldView[wGoldPos].SetGold(dwGold);
	for (int i=0;i<CountArray(m_GoldStation);i++) m_dwAllGold+=m_GoldView[i].GetGold();

	//更新视图
	UpdateGameView(NULL);

	return;
}

//////////////////////////////////////////////////////////////////////////
