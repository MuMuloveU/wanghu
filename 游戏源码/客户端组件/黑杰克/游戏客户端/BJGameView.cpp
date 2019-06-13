#include "StdAfx.h"
#include "Resource.h"
#include "BJGameView.h"
#include "BJGameDlg.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识 
#define IDC_BEGIN					    101				//开始按钮
#define IDC_ADD_GOLD					102				//加注按钮
#define IDC_GIVE_UP						103				//放弃按钮
#define IDC_GET_CARD					104		    	//要牌按钮
#define IDC_STOP_CARD				    105				//停牌按钮
#define IDC_DOUBLE_CARD				    106				//双倍按钮
#define IDC_CUT_CARD				    107				//分牌按钮

//分牌标识
#define NO_CUT				            0				//没有分牌
#define TOP_CARD				        1				//左手分牌
#define BOTTOM_CARD				        2				//右手分牌

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBJGameView, CGameFrameView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BEGIN, OnStart)
	ON_BN_CLICKED(IDC_ADD_GOLD, OnAddGold)
	ON_BN_CLICKED(IDC_GIVE_UP, OnGiveUp)
	ON_BN_CLICKED(IDC_GET_CARD, OnGetCard)
	ON_BN_CLICKED(IDC_STOP_CARD, OnStopCard)
	ON_BN_CLICKED(IDC_DOUBLE_CARD, OnDoubleCard)
	ON_BN_CLICKED(IDC_CUT_CARD, OnCutCard)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////

//构造函数
CBJGameView::CBJGameView(void) : CGameFrameView(true,24)
{
	//金币变量
	m_lMaxGold   = 0L;
	m_lBasicGold = 0L;

	//界面相关
	m_wBankerStation = 255;
	m_cbCutCardMark  = NO_CUT;
	memset(m_cbTopCardSum, 0, sizeof(m_cbTopCardSum));
	memset(m_cbBottomCardSum, 0, sizeof(m_cbBottomCardSum));
	memset(m_cbPlayStatus, 0, sizeof(m_cbPlayStatus));

	//加载资源
	HINSTANCE hInstance = AfxGetInstanceHandle();	
	m_ImageBack.SetLoadInfo(IDB_BACKGROUD,hInstance);
	m_ImageTitle.SetLoadInfo(IDB_TITLE,hInstance);
	m_ImageBanker.SetLoadInfo(IDB_BANKER,hInstance);
	m_ImageArrowHead.SetLoadInfo(IDB_ARROW_HEAD,hInstance);
	m_ImageCenter.SetLoadInfo(IDB_Center,hInstance);
	
	m_ImageAddGoldMark.SetLoadInfo(IDB_ADDGOLD_MARK,hInstance);
	m_ImageGetCardMark.SetLoadInfo(IDB_GETCARD_MARK,hInstance);
	m_ImageStopMark.SetLoadInfo(IDB_STOP_MARK,hInstance);
	m_ImageBrustMark.SetLoadInfo(IDB_BRUST_MARK,hInstance);

	//获取大小
		
	//m_ImageSize.SetSize(m_ImageBack.GetWidth(),m_ImageBack.GetHeight());
	return;
}

//析构函数
CBJGameView::~CBJGameView(void)
{
}

//建立消息
int CBJGameView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1) return -1;

	//创建控件
	CRect CreateRect(0,0,0,0);
	m_GoldControl.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS,CreateRect,this,8,NULL);
	m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,CreateRect,this,9);
	for (int i = 0; i < GAME_PLAYER * 2; i++)
	{
		m_CardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,10+i);
	}

	//创建按钮
	m_btGiveUp.Create(NULL,WS_CHILD|WS_CHILD,CreateRect,this,IDC_GIVE_UP);
	m_btAddGold.Create(NULL,WS_CHILD|WS_CHILD,CreateRect,this,IDC_ADD_GOLD);
	m_btBegin.Create(NULL,WS_CHILD|WS_CHILD,CreateRect,this,IDC_BEGIN);	
	m_btGetCard.Create(NULL,WS_CHILD|WS_CHILD,CreateRect,this,IDC_GET_CARD);
	m_btStop.Create(NULL,WS_CHILD|WS_CHILD,CreateRect,this,IDC_STOP_CARD);
	m_btDouble.Create(NULL,WS_CHILD|WS_CHILD,CreateRect,this,IDC_DOUBLE_CARD);
	m_btCutCard.Create(NULL,WS_CHILD|WS_CHILD,CreateRect,this,IDC_CUT_CARD);

	//加载位图
	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_btAddGold.SetButtonImage(IDB_BT_ADD_GOLD,hInstance,false);
	m_btBegin.SetButtonImage(IDB_BT_BEGIN,hInstance,false);
	m_btGiveUp.SetButtonImage(IDB_BT_GIVE_UP,hInstance,false);
	m_btGetCard.SetButtonImage(IDB_BT_GET_CARD,hInstance,false);
	m_btStop.SetButtonImage(IDB_BT_STOP_CARD,hInstance,false);
	m_btDouble.SetButtonImage(IDB_BT_DOUBLE,hInstance,false);
	m_btCutCard.SetButtonImage(IDB_BT_CUT_CARD,hInstance,false);

	return 0;
}


//重置界面
void CBJGameView::ResetGameView()
{	
	//金币变量
	m_lMaxGold   = 0L;
	m_lBasicGold = 0L;

	//界面相关
	m_wBankerStation = 255;
	m_cbCutCardMark  = NO_CUT;
	memset(m_cbTopCardSum, 0, sizeof(m_cbTopCardSum));
	memset(m_cbBottomCardSum, 0, sizeof(m_cbBottomCardSum));
	memset(m_cbPlayStatus, 0, sizeof(m_cbPlayStatus));	


	//设置控件
	m_ScoreView.ShowWindow(SW_HIDE);
	m_GoldControl.ShowWindow(SW_HIDE);

	m_btBegin.ShowWindow(SW_HIDE);
	m_btGiveUp.ShowWindow(SW_HIDE);
	m_btGetCard.ShowWindow(SW_HIDE);
	m_btStop.ShowWindow(SW_HIDE);
	m_btDouble.ShowWindow(SW_HIDE);
	m_ScoreView.ShowWindow(SW_HIDE);
	m_GoldControl.ShowWindow(SW_HIDE);

	for (int i=0;i<CountArray(m_GoldView);i++) m_GoldView[i].SetGold(0);
	for (int i=0;i<CountArray(m_CardControl);i++) m_CardControl[i].SetCardData(NULL,0);

	return;
}

//调整控件
void CBJGameView::RectifyGameView(int nWidth, int nHeight)
{
	m_Rct_Play.left		= m_nXBorder;
	m_Rct_Play.top		= m_nYBorder;
	m_Rct_Play.right	= nWidth  - m_nXBorder;
	m_Rct_Play.bottom	= nHeight - m_nYBorder;

	int nXCenter = nWidth / 2;
	int nYCenter = nHeight / 2;	
	int temp     = 100;

	//设置坐标
	m_ptFace[0].x    = nXCenter - m_nXFace;
	m_ptFace[0].y    = m_Rct_Play.top + 15;
	m_ptName[0].x    = m_ptFace[0].x + m_nXFace + 5;
	m_ptName[0].y    = m_ptFace[0].y+10;	
	m_ptReady[0].x   = m_ptFace[0].x - 5;
	m_ptReady[0].y   = m_ptFace[0].y + 20;
	m_ptMark[0].x    = m_ptFace[0].x - 40;
	m_ptMark[0].y    = m_ptFace[0].y+10;
	m_ptCardSum[0].x = m_ptFace[0].x - 50;
	m_ptCardSum[0].y = m_ptFace[0].y + 50;

	m_ptFace[1].x    = m_Rct_Play.right - m_nXFace - 5;
	m_ptFace[1].y    = nYCenter;	
	m_ptName[1].x    = m_Rct_Play.right - 5;
	m_ptName[1].y    = m_ptFace[1].y + m_nXFace + 5 ;
	m_ptReady[1].x   = m_ptFace[1].x + m_nXFace / 2;
	m_ptReady[1].y   = m_ptFace[1].y - 10;
	m_ptMark[1].x    = m_ptFace[1].x;
	m_ptMark[1].y    = m_ptFace[1].y - 40;
	m_ptCardSum[1].x = m_Rct_Play.right - 120;
	m_ptCardSum[1].y = m_ptFace[1].y - 80;

	m_ptFace[2].x    = nXCenter - 100;
	m_ptFace[2].y    = m_Rct_Play.bottom - 75;
	m_ptName[2].x    = m_ptFace[2].x + m_nXFace + 5;
	m_ptName[2].y    = m_ptFace[2].y;
	m_ptReady[2].x   = m_ptFace[2].x + m_nXFace / 2;
	m_ptReady[2].y   = m_ptFace[2].y - 10;
	m_ptMark[2].x    = m_ptFace[2].x;
	m_ptMark[2].y    = m_ptFace[2].y - 40;
	m_ptCardSum[2].x = nWidth/2 - 15;
	m_ptCardSum[2].y = nHeight/2 + 220;
	
	m_ptFace[3].x    = m_Rct_Play.left + 30;
	m_ptFace[3].y    = nYCenter;
	m_ptName[3].x    = m_ptFace[3].x;
	m_ptName[3].y    = m_ptFace[3].y  + m_nXFace + 5;
	m_ptReady[3].x   = m_ptFace[3].x + m_nXFace / 2;
	m_ptReady[3].y   = m_ptFace[3].y - 10;
	m_ptMark[3].x    = m_ptFace[3].x;
	m_ptMark[3].y    = m_ptFace[3].y - 40;
	m_ptCardSum[3].x = m_Rct_Play.left + 100;
	m_ptCardSum[3].y = m_ptFace[3].y - 80;
	
	//筹码控件
	int nXCenterPos = nWidth/2;
	int nYCenterPos = nHeight/2;

	if (GetSystemMetrics(SM_CXSCREEN) >= 1024)
	{
		m_GoldStation[0].SetPoint(nXCenterPos-16,nYCenterPos-115);
		m_GoldStation[1].SetPoint(nXCenterPos+41,nYCenterPos-105);
		m_GoldStation[2].SetPoint(nXCenterPos+103,nYCenterPos-50);
		m_GoldStation[3].SetPoint(nXCenterPos+102,nYCenterPos+32);
		m_GoldStation[4].SetPoint(nXCenterPos-15,nYCenterPos+85);
		m_GoldStation[5].SetPoint(nXCenterPos-37,nYCenterPos+100);
		m_GoldStation[6].SetPoint(nXCenterPos-97,nYCenterPos-50);
		m_GoldStation[7].SetPoint(nXCenterPos-99,nYCenterPos-45);
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
	HDWP hDwp = BeginDeferWindowPos(5);
	const UINT uFlags = SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	
	DeferWindowPos(hDwp,m_btBegin,NULL,nWidth/2-30,nHeight/2+150,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUp,NULL,nWidth-m_nXBorder-190,nHeight-m_nYBorder-50,0,0,uFlags);     
	DeferWindowPos(hDwp,m_btAddGold,NULL,nWidth-m_nXBorder-130,nHeight-m_nYBorder-90,0,0,uFlags);    

	DeferWindowPos(hDwp,m_btCutCard,NULL,nWidth-m_nXBorder-75,nHeight-m_nYBorder-170,0,0,uFlags);  
	DeferWindowPos(hDwp,m_btStop,NULL,nWidth-m_nXBorder-95,nHeight-m_nYBorder-80,0,0,uFlags);
	DeferWindowPos(hDwp,m_btDouble,NULL,nWidth-m_nXBorder-87,nHeight-m_nYBorder-110,0,0,uFlags);  
	DeferWindowPos(hDwp,m_btGetCard,NULL,nWidth-m_nXBorder-95,nHeight-m_nYBorder-140,0,0,uFlags);  

	EndDeferWindowPos(hDwp);
	
	//成绩窗口
	CRect rcScoreView;
	m_ScoreView.GetWindowRect(&rcScoreView);
	m_ScoreView.MoveWindow((nWidth-rcScoreView.Width())/2,(nHeight-rcScoreView.Height())/2-10,rcScoreView.Width(),rcScoreView.Height());

	//加注窗口
	m_GoldControl.SetBasicPoint(nWidth-25-m_nXBorder,nHeight-100-m_nYBorder);

	//扑克控件
	m_CardControl[0].SetBasicStation(CPoint(nWidth/2,62+m_nYBorder),enXCenter,enYTop);
	m_CardControl[1].SetBasicStation(CPoint(nWidth-m_nXBorder-70,nHeight/2-10),enXRight,enYCenter);
	m_CardControl[2].SetBasicStation(CPoint(nWidth/2,nHeight-50-m_nYBorder),enXLeft,enYBottom);
	m_CardControl[3].SetBasicStation(CPoint(m_nXBorder+90,nHeight/2-10),enXLeft,enYCenter);
	m_CardControl[4].SetBasicStation(CPoint(nWidth/2,67+m_nYBorder),enXCenter,enYTop);
	m_CardControl[5].SetBasicStation(CPoint(nWidth-m_nXBorder-70,nHeight/2+25),enXRight,enYCenter);
	m_CardControl[6].SetBasicStation(CPoint(nWidth/2,nHeight-5-m_nYBorder),enXLeft,enYBottom);
	m_CardControl[7].SetBasicStation(CPoint(m_nXBorder+60,nHeight/2+25),enXLeft,enYCenter);

    return;
}

//绘画界面
void CBJGameView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//变量定义
	CImageHandle HandleTitle(&m_ImageTitle);
	CImageHandle HandleBanker(&m_ImageBanker);
	CImageHandle HandleArrowHead(&m_ImageArrowHead);
	CImageHandle HandleAddGoldMark(&m_ImageAddGoldMark);
	CImageHandle HandleGetCardMark(&m_ImageGetCardMark);
	CImageHandle HandleStopMark(&m_ImageStopMark);
	CImageHandle HandleBrustMark(&m_ImageBrustMark);
	CImageHandle HandleBack(&m_ImageBack);
	CImageHandle HandleCenter(&m_ImageCenter);

	//绘画背景
	pDC->FillSolidRect(0, 0, nWidth, nHeight, RGB(0,0,0));
	DrawViewImage(pDC, m_ImageBack, enMode_Spread);
	DrawViewImage(pDC, m_ImageCenter, enMode_Centent);
	m_ImageTitle.BitBlt(pDC->m_hDC,m_nXBorder+20,m_nYBorder+10);

	//绘画用户
	TCHAR szBuffer[64];
	WORD  wUserTimer = 0;
	
	pDC->SetTextColor(RGB(255,255,0));
	
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//变量定义
		const tagUserData * pUserData = GetUserInfo(i);
		
		if (pUserData != NULL)
		{
			//用户名字
			pDC->SetTextAlign((i == 1) ? (TA_RIGHT|TA_TOP) : (TA_LEFT|TA_TOP));
			pDC->TextOut(m_ptName[i].x, m_ptName[i].y, pUserData->szName, lstrlen(pUserData->szName));

			//用户金币
			_snprintf(szBuffer, sizeof(szBuffer), TEXT("§%ld"), pUserData->lScore);
			pDC->TextOut(m_ptName[i].x, m_ptName[i].y + 16, szBuffer, lstrlen(szBuffer));

			//其他信息
			wUserTimer = GetUserTimer(i);
			if (wUserTimer != 0)
			{
				DrawUserTimer(pDC, nWidth - 80, 50, wUserTimer);
			}
			
			if (pUserData->cbUserStatus == US_READY)
			{
				DrawUserReady(pDC, m_ptReady[i].x, m_ptReady[i].y);
			}
			
			DrawUserFace(pDC,pUserData, m_ptFace[i].x, m_ptFace[i].y, pUserData->cbUserStatus == US_OFFLINE);
		}
	}	

	//游戏信息
	const UINT uDrawFormat = DT_END_ELLIPSIS|DT_CENTER|DT_SINGLELINE|DT_VCENTER;

	pDC->SetTextAlign(TA_LEFT|TA_TOP);
	pDC->SetTextColor(RGB(255,255,255));

	if (m_lMaxGold != 0) 
	{
		_snprintf(szBuffer, sizeof(szBuffer), TEXT("§%ld"), m_lMaxGold);
		DrawText(pDC->GetSafeHdc(), szBuffer, lstrlen(szBuffer), CRect(86,23,162,38), uDrawFormat);
	}
	if (m_lBasicGold != 0) 
	{
		_snprintf(szBuffer, sizeof(szBuffer), TEXT("§%ld"), m_lBasicGold);
		DrawText(pDC->GetSafeHdc(), szBuffer, lstrlen(szBuffer), CRect(86,41,162,53), uDrawFormat);
	}
	

//#ifdef DEBUG_GAME
//	for (WORD i = 0; i < GAME_PLAYER; i++)
//	{
//		{
//			//用户名字
//			pDC->SetTextAlign((i == 1) ? (TA_RIGHT|TA_TOP) : (TA_LEFT|TA_TOP));
//			pDC->TextOut(m_ptName[i].x, m_ptName[i].y, "游戏玩家");
//
//			//用户金币
//			pDC->TextOut(m_ptName[i].x, m_ptName[i].y + 16, "§1000");
//
//			//其他信息
//			DrawUserTimer(pDC, nWidth - 80, 50, 5);
//			DrawUserReady(pDC, m_ptReady[i].x, m_ptReady[i].y);
//
//			DrawUserFace(pDC,5, m_ptFace[i].x, m_ptFace[i].y,true);
//		}
//	}	
//	//最大最小注
//	_snprintf(szBuffer, sizeof(szBuffer), TEXT("§%ld"), m_lMaxGold);
//	DrawText(pDC->GetSafeHdc(), szBuffer, lstrlen(szBuffer), CRect(86,30,162,45), uDrawFormat);
//
//	_snprintf(szBuffer, sizeof(szBuffer), TEXT("§%ld"), m_lBasicGold);
//	DrawText(pDC->GetSafeHdc(), szBuffer, lstrlen(szBuffer), CRect(86,48,162,60), uDrawFormat);
//#endif
	//绘画筹码
	LONG lTotalGold=0L;
	for (DWORD i = 0; i < CountArray(m_GoldView); i++)
	{
		lTotalGold += m_GoldView[i].GetGold();
		m_GoldView[i].DrawGoldView(pDC,m_GoldStation[i].x,m_GoldStation[i].y);
	}

	//总注信息
	if (lTotalGold != 0L)
	{
		CFont ViewFont;
		ViewFont.CreateFont(-24,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("楷体_GB2312"));
		CFont * pOldFont=pDC->SelectObject(&ViewFont);
		pDC->SetTextColor(RGB(255,255,0));
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),lTotalGold);
		pDC->TextOut(nWidth/2-20,nHeight/2+110,szBuffer,lstrlen(szBuffer));
		pDC->SelectObject(pOldFont);
		ViewFont.DeleteObject();
	}

	//扑克总值
	pDC->SetTextColor(RGB(255,255,0));
	CFont ViewFont;
	ViewFont.CreateFont(-20,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("楷体_GB2312"));
	CFont * pOldFont=pDC->SelectObject(&ViewFont);
	pDC->SetTextColor(RGB(255,255,0));
	pDC->SetTextAlign(TA_CENTER|TA_TOP);

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//变量定义
		const tagUserData * pUserData = GetUserInfo(i);

		if (pUserData != NULL)
		{
			if(m_cbTopCardSum[i] !=0)
			{
				_snprintf(szBuffer, sizeof(szBuffer), TEXT("%d"), m_cbTopCardSum[i]);
				
				pDC->TextOut(m_ptCardSum[i].x, m_ptCardSum[i].y, szBuffer, lstrlen(szBuffer));
			}
			if(m_cbBottomCardSum[i] !=0)
			{
				_snprintf(szBuffer, sizeof(szBuffer), TEXT("%d"), m_cbBottomCardSum[i]);
				pDC->TextOut(nWidth/2-15, nHeight/2+250, szBuffer, lstrlen(szBuffer));
			}
		}
	}

	pDC->SelectObject(pOldFont);
	ViewFont.DeleteObject();	

	//分牌标识
	if(m_cbCutCardMark == TOP_CARD)
	{
		m_ImageArrowHead.AlphaDrawImage(pDC,nWidth/2-80,nHeight-140,RGB(5,136,30));
	}
	if(m_cbCutCardMark == BOTTOM_CARD)
	{
		m_ImageArrowHead.AlphaDrawImage(pDC,nWidth/2-80,nHeight-80,RGB(5,136,30));
	}
	
	//玩家状态
	
	HINSTANCE hInstance = AfxGetInstanceHandle();

	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(i == 2) continue;
		switch(m_cbPlayStatus[i])
		{
		case 0:
			{
				break;
			}
			//用户加注
		case 1:
			{				
				m_ImageAddGoldMark.AlphaDrawImage(pDC,m_ptMark[i].x,m_ptMark[i].y,RGB(255,0,255));
				break;
			}
			//用户要牌
		case 2:
			{				
				m_ImageGetCardMark.AlphaDrawImage(pDC,m_ptMark[i].x,m_ptMark[i].y,RGB(255,0,255));
				break;
			}
			//用户停牌
		case 3:
			{				
				m_ImageStopMark.AlphaDrawImage(pDC,m_ptMark[i].x,m_ptMark[i].y,RGB(255,0,255));
				break;
			}
			//用户爆牌
		case 4:
			{				
				m_ImageBrustMark.AlphaDrawImage(pDC,m_ptMark[i].x,m_ptMark[i].y,RGB(255,0,255));
				break;
			}
		}		
	}

	//庄家标识	
	if ((m_wBankerStation != 255))
	{
		m_ImageBanker.AlphaDrawImage(pDC,m_ptMark[m_wBankerStation].x,m_ptMark[m_wBankerStation].y,RGB(255,0,255));
	}
	else m_wBankerStation = 255;

	return;
}

//开始按钮
void CBJGameView::OnStart()
{		
	memset(m_cbTopCardSum, 0, sizeof(m_cbTopCardSum));
	memset(m_cbBottomCardSum, 0, sizeof(m_cbBottomCardSum));

	CWnd * pWnd = ::AfxGetMainWnd();
	if(!pWnd)
	{
		return;	
	}
	pWnd->SendMessage(IDM_GAME_START,0,0);
}
//下注按钮
void CBJGameView::OnAddGold()
{
	CWnd * pWnd = ::AfxGetMainWnd();
	if(!pWnd)
	{
		return;	
	}
	pWnd->SendMessage(IDM_ADD_GOLD,0,0);
}
//取消按钮
void CBJGameView::OnGiveUp()
{
	CWnd * pWnd = ::AfxGetMainWnd();
	/*if(!pWnd)
	{
		return;	
	}*/
	pWnd->SendMessage(IDM_GIVE_UP,0,0);
}

//要牌按钮
void CBJGameView::OnGetCard()
{
	CWnd * pWnd = ::AfxGetMainWnd();
	if(!pWnd)
	{
		return;	
	}
	pWnd->SendMessage(IDM_GET_CARD,0,0);
}

//停牌按钮
void CBJGameView::OnStopCard()
{
	CWnd * pWnd = ::AfxGetMainWnd();
	if(!pWnd)
	{
		return;	
	}
	pWnd->SendMessage(IDM_STOP_CARD,0,0);
}

//双倍按钮
void CBJGameView::OnDoubleCard()
{
	CWnd * pWnd = ::AfxGetMainWnd();
	if(!pWnd)
	{
		return;	
	}
	pWnd->SendMessage(IDM_DOUBLE_CARD,0,0);
}

//分牌按钮
void CBJGameView::OnCutCard()
{
	CWnd * pWnd = ::AfxGetMainWnd();
	if(!pWnd)
	{
		return;	
	}
	pWnd->SendMessage(IDM_CUT_CARD,0,0);
}

//金币标题
void CBJGameView::SetGoldTitleInfo(DWORD lMaxGold, DWORD lBasicGold)
{
	//设置变量
	m_lMaxGold   = lMaxGold;
	m_lBasicGold = lBasicGold;

	//更新视图
	UpdateGameView(NULL);

	return;
}

//用户筹码
void CBJGameView::SetUserGoldInfo(WORD wViewChairID, bool bTableGold, DWORD dwGold)
{
	//参数判断
	WORD wGoldPos = wViewChairID * 2 + ((bTableGold == true) ? 1 : 0);
	
	if (wGoldPos >= CountArray(m_GoldStation)) return;

	//设置变量
	DWORD m_dwAllGold = 0L;

	m_GoldView[wGoldPos].SetGold(dwGold);

	for (int i = 0; i < CountArray(m_GoldStation); i++) 
	{
		m_dwAllGold += m_GoldView[i].GetGold();
	}
	//更新视图
	UpdateGameView(NULL);

	return;
}
//////////////////////////////////////////////////////////////////////////





