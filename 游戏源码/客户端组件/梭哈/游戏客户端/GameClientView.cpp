#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientDlg.h"
#include <MMSystem.h>
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识 
#define IDC_START						100								//开始按钮
#define IDC_GIVE_UP						102								//放弃按钮
#define IDC_FOLLOW						103								//跟注按钮
#define IDC_SHOW_HAND					104								//梭哈按钮
#define IDC_ADD_TIMES1					105								//查分按钮
#define IDC_ADD_TIMES2					106								//查分按钮
#define IDC_ADD_TIMES3					107								//查分按钮

//定时器标识
#define IDI_SEND_CARD					107								//发牌定时器

//发牌定义
#define SEND_STEP_COUNT					6								//步数
#define SPEED_SEND_CARD					10								//发牌速度

#define MAX_COUNT						5

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_FOLLOW, OnFollow)
	ON_BN_CLICKED(IDC_GIVE_UP, OnGiveUp)
	ON_BN_CLICKED(IDC_ADD_TIMES1, OnAddTimes1)
	ON_BN_CLICKED(IDC_ADD_TIMES2, OnAddTimes2)
	ON_BN_CLICKED(IDC_ADD_TIMES3, OnAddTimes3)
	ON_BN_CLICKED(IDC_SHOW_HAND, OnShowHand)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//设置变量
	m_lMaxGold=0L;
	m_lBasicGold=0L;
	m_bShowScoreControl = false;

	//动画变量
	m_SendCardPos.SetPoint(0,0);
	m_SendCardCurPos = m_SendCardPos;
	m_nStepCount = SEND_STEP_COUNT;
	m_nXStep = 0;
	m_nYStep = 0;

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageCardBack.SetLoadInfo(IDB_CARD_BACK,hInstance);
	m_ImageTitle.SetLoadInfo(IDB_TITLE,hInstance);
	m_ImageViewFill.SetLoadInfo(IDB_VIEW_FILL,hInstance);
	m_ImageViewCenter.SetLoadInfo(IDB_BACK_GROUND,hInstance);
	m_ImageScoreControlBack.SetLoadInfo( IDB_SCORE_CONTROL_BACK,hInstance );
	m_ImageScoreTable.SetLoadInfo( IDB_SCORE_TABLE,hInstance );
	m_PngBkTopLeft.LoadImage( hInstance,TEXT("BK_TOPLEFT") );
	m_PngBkTopRight.LoadImage( hInstance,TEXT("BK_TOPRIGHT") );
	m_PngBkBottomLeft.LoadImage( hInstance,TEXT("BK_BOTTOMLEFT") );
	m_PngBkBottomRight.LoadImage( hInstance,TEXT("BK_BOTTOMRIGHT") );

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
	m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,CreateRect,this,9);
	for (int i=0;i<4;i++) m_CardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CreateRect,this,10+i);

	//创建按钮
	m_btStart.Create(NULL,WS_CHILD,CreateRect,this,IDC_START);
	m_btFollow.Create(NULL,WS_CHILD,CreateRect,this,IDC_FOLLOW);
	m_btGiveUp.Create(NULL,WS_CHILD,CreateRect,this,IDC_GIVE_UP);
	m_btShowHand.Create(NULL,WS_CHILD,CreateRect,this,IDC_SHOW_HAND);
	m_btAddTimes1.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_ADD_TIMES1);
	m_btAddTimes2.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_ADD_TIMES2);
	m_btAddTimes3.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_ADD_TIMES3);

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_RGN_START, hInstance, false);
	m_btFollow.SetButtonImage(IDB_RGN_FOLLOW, hInstance, false);
	m_btGiveUp.SetButtonImage(IDB_RGN_GIVEUP, hInstance, false);
	m_btShowHand.SetButtonImage(IDB_RGN_SHOWHAND, hInstance, false);
	m_btAddTimes1.SetButtonImage(IDB_RGN_ADD_GOLD, hInstance, false);
	m_btAddTimes2.SetButtonImage(IDB_RGN_ADD_GOLD, hInstance, false);
	m_btAddTimes3.SetButtonImage(IDB_RGN_ADD_GOLD, hInstance, false);

	//test
// 	SetGoldTitleInfo( 0,123456789 );
// 	m_btStart.ShowWindow( SW_SHOW );
// 	ShowScoreControl( true );
// 	BYTE byCard[] = {
// 		0x01,0x09,0x0a,0x0b,0x0c
// 	};
// 	for( WORD i = 0; i < GAME_PLAYER; i++ )
// 	{
// 		m_CardControl[i].SetCardData( byCard,5 );
// 		//SetUserGoldInfo( i,false,1 );
// 		//SetUserGoldInfo( i,true,456 );
// 	}
// 	m_CardControl[2].AllowPositively( true );
// 	BYTE cbCardData[] = {0x01,0x08,0x09,0x0A,0x0B};
// 	m_CardControl[2].SetCardData( cbCardData,5 );
// 	m_GoldView[0].SetGold( 123456789L );
// 	m_GoldView[1].SetGold( 123456789L );
// 	SetGoldTitleInfo( 1234567L,123456789L );
// 	m_btFollow.ShowWindow( SW_SHOW );
// 	m_btGiveUp.ShowWindow( SW_SHOW );
// 	m_btShowHand.ShowWindow( SW_SHOW );
// 	m_btAddTimes1.ShowWindow( SW_SHOW );
// 	m_btAddTimes2.ShowWindow( SW_SHOW );
// 	m_btAddTimes3.ShowWindow( SW_SHOW );
// 	ShowScoreControl( true );
// 	for( WORD i = 0; i < GAME_PLAYER; i++ )
// 	{
// 		m_GoldView[i*2].SetGoldIndex(2);
// 		m_GoldView[i*2].SetGold(12345678L);
// 		m_GoldView[i*2+1].SetGold(654321L);
// 	}
	//end test

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
void CGameClientView::OnAddTimes1()
{
	AfxGetMainWnd()->SendMessage(IDM_ADD_GOLD,1,0);
	return;
}

//加注按钮
void CGameClientView::OnAddTimes2()
{
	AfxGetMainWnd()->SendMessage(IDM_ADD_GOLD,2,0);
	return;
}

//加注按钮
void CGameClientView::OnAddTimes3()
{
	AfxGetMainWnd()->SendMessage(IDM_ADD_GOLD,3,0);
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
	m_bShowScoreControl = false;

	//动画变量
	m_SendCardCurPos = m_SendCardPos;
	m_nStepCount = 0;
	m_nXStep = 0;
	m_nYStep = 0;
	m_SendCardItemArray.RemoveAll();

	//设置控件
	m_btStart.ShowWindow(SW_HIDE);
	m_btFollow.ShowWindow(SW_HIDE);
	m_btGiveUp.ShowWindow(SW_HIDE);
	m_btShowHand.ShowWindow(SW_HIDE);
	m_ScoreView.ShowWindow(SW_HIDE);

	//设置筹码
	for (int i=0;i<CountArray(m_GoldView);i++) m_GoldView[i].SetGold(0L);

	//设置扑克
	for (int i=0;i<CountArray(m_CardControl);i++)
	{
		m_CardControl[i].SetCardData(NULL,0);
		m_CardControl[i].ShowFirstCard(false);
		m_CardControl[i].AllowPositively(false);
	}

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//设置坐标
	m_ptFace[0].x=nWidth/2;
	m_ptFace[0].y=5+m_nYBorder;
	m_ptName[0].x=nWidth/2+m_nXFace+5;
	m_ptName[0].y=5+m_nYBorder;
	m_ptTimer[0].x=nWidth/2-m_nXFace/2-m_nXTimer/2-5;
	m_ptTimer[0].y=m_nYBorder+m_nYTimer/2+2;
	m_ptReady[0].x=nWidth/2-m_nXFace-15;
	m_ptReady[0].y=2+m_nYBorder+m_nYTimer/2;

	m_ptFace[1].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptFace[1].y=nHeight/2-m_nYFace-58;
	m_ptName[1].x=nWidth-m_nXBorder-m_nXFace-10;
	m_ptName[1].y=nHeight/2-m_nYFace-58;
	m_ptTimer[1].x=nWidth-m_nXBorder-m_nXTimer/2-2;
	m_ptTimer[1].y=nHeight/2-m_nYFace-83;
	m_ptReady[1].x=nWidth-(m_nXBorder+48);
	m_ptReady[1].y=nHeight/2-m_nYFace-83;

	m_ptFace[2].x=nWidth/2;
	m_ptFace[2].y=nHeight-m_nYBorder-m_nYFace-5;
	m_ptName[2].x=nWidth/2+m_nYFace+5;
	m_ptName[2].y=nHeight-m_nYBorder-m_nYFace-5;
	m_ptTimer[2].x=nWidth/2-m_nXFace/2-m_nXTimer/2-5;
	m_ptTimer[2].y=nHeight-m_nYBorder-m_nYTimer/2-2;
	m_ptReady[2].x=nWidth/2 + 10;
	m_ptReady[2].y=nHeight-217-m_nYBorder;

	m_ptFace[3].x=m_nXBorder+5;
	m_ptFace[3].y=nHeight/2-m_nYFace-58;
	m_ptName[3].x=m_nXBorder+m_nXFace+10;
	m_ptName[3].y=nHeight/2-m_nYFace-58;
	m_ptTimer[3].x=m_nXBorder+m_nXTimer/2;
	m_ptTimer[3].y=nHeight/2-m_nYFace-83;
	m_ptReady[3].x=m_nXBorder+45;
	m_ptReady[3].y=nHeight/2-m_nYFace-83;

	//筹码控件
	int nXCenterPos=nWidth/2;
	int nYCenterPos=nHeight/2;
	m_GoldStation[0].SetPoint(nXCenterPos,nYCenterPos-90);
	m_GoldStation[1].SetPoint(nXCenterPos+79,nYCenterPos-90);
	m_GoldStation[2].SetPoint(nXCenterPos+91,nYCenterPos-3);
	m_GoldStation[3].SetPoint(nXCenterPos+91,nYCenterPos+65);
	m_GoldStation[4].SetPoint(nXCenterPos,nYCenterPos+82);
	m_GoldStation[5].SetPoint(nXCenterPos-79,nYCenterPos+82);
	m_GoldStation[6].SetPoint(nXCenterPos-90,nYCenterPos-5);
	m_GoldStation[7].SetPoint(nXCenterPos-90,nYCenterPos-71);

	//下注框位置
// 	m_ptScoreControl.SetPoint(nWidth/2+200,nHeight/2+200);
	m_ptScoreControl.SetPoint(nWidth+200,nHeight+200);
	INT nXControl = m_ptScoreControl.x+6;
	INT nYControl = m_ptScoreControl.y+7;

	//按钮控件
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	
	//开始按钮
	CRect rcStart;
	m_btStart.GetWindowRect(&rcStart);
// 	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcStart.Width())/2+10,nHeight/2+156,0,0,uFlags);
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcStart.Width())/2+50,nHeight-50,0,0,uFlags);

	//控制按钮
	m_btFollow.GetWindowRect(&rcButton);
// 	DeferWindowPos(hDwp,m_btFollow,NULL,nXControl,nYControl,0,0,uFlags);
// 	DeferWindowPos(hDwp,m_btAddTimes2,NULL,nXControl,nYControl+rcButton.Height()+5,0,0,uFlags);
// 	DeferWindowPos(hDwp,m_btAddTimes1,NULL,nXControl,nYControl+rcButton.Height()*2+10,0,0,uFlags);
// 	DeferWindowPos(hDwp,m_btShowHand,NULL,nXControl+rcButton.Width()+5,nYControl,0,0,uFlags);
// 	DeferWindowPos(hDwp,m_btAddTimes3,NULL,nXControl+rcButton.Width()+5,nYControl+rcButton.Height()+5,0,0,uFlags);
// 	DeferWindowPos(hDwp,m_btGiveUp,NULL,nXControl+rcButton.Width()+5,nYControl+rcButton.Height()*2+10,0,0,uFlags);

	DeferWindowPos(hDwp,m_btGiveUp,NULL,10,nHeight-50,0,0,uFlags);
	DeferWindowPos(hDwp,m_btFollow,NULL,nWidth-rcButton.Width()-10,nHeight-50,0,0,uFlags);
	DeferWindowPos(hDwp,m_btShowHand,NULL,nWidth-rcButton.Width()-10,nHeight-50-100,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAddTimes1,NULL,nWidth-rcButton.Width()-130,nHeight-50-100,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAddTimes2,NULL,nWidth-rcButton.Width()-130,nHeight-50-50,0,0,uFlags);
	DeferWindowPos(hDwp,m_btAddTimes3,NULL,nWidth-rcButton.Width()-130,nHeight-50,0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDwp);

	//成绩窗口
	CRect rcScoreView;
	m_ScoreView.GetWindowRect(&rcScoreView);
	m_ScoreView.MoveWindow((nWidth-rcScoreView.Width())/2,(nHeight-rcScoreView.Height())/2-10,rcScoreView.Width(),rcScoreView.Height());

	//扑克控件
	m_CardControl[0].SetBasicStation(CPoint(nWidth/2,52+m_nYBorder),enXCenter,enYTop);
	m_CardControl[1].SetBasicStation(CPoint(nWidth-m_nXBorder-5,nHeight/2),enXRight,enYCenter);
	m_CardControl[2].SetBasicStation(CPoint(nWidth/2,nHeight-55-m_nYBorder),enXCenter,enYBottom);
	m_CardControl[3].SetBasicStation(CPoint(m_nXBorder+5,nHeight/2),enXLeft,enYCenter);

	//发牌起始位置
	m_SendCardPos.SetPoint(nWidth/2-40,nHeight/2-52);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	CImageHandle HandleTitle(&m_ImageTitle);
	DrawViewImage(pDC,m_ImageViewFill,enMode_Centent);
	//DrawViewImage(pDC,m_ImageViewCenter,enMode_Centent);
	m_ImageTitle.BitBlt(pDC->m_hDC,m_nXBorder+80,m_nYBorder);
	//四角
	m_PngBkTopLeft.DrawImage( pDC,0,0 );
	m_PngBkTopRight.DrawImage( pDC,nWidth-m_PngBkTopRight.GetWidth(),0 );
	m_PngBkBottomLeft.DrawImage( pDC,0,nHeight-m_PngBkBottomLeft.GetHeight() );
	m_PngBkBottomRight.DrawImage( pDC,nWidth-m_PngBkBottomRight.GetWidth(),nHeight-m_PngBkBottomRight.GetHeight() );

	//
	CImageHandle HandleScoreTable(&m_ImageScoreTable);
	m_ImageScoreTable.AlphaDrawImage( pDC,nWidth/2-m_ImageScoreTable.GetWidth()/2,nHeight/2-m_ImageScoreTable.GetHeight()/2,
		RGB(255,0,255) );

	//绘画用户
	WORD wUserTimer=0;
	TCHAR szBuffer[64];
	pDC->SetTextColor(RGB(255,255,0));
	for (WORD i=0;i<4;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);
		
		////////////////////////////////////////////////////////
		//tagUserData *pUserData = new tagUserData;
		//strcpy( pUserData->szName, "dkfjdkfj" );
		//pUserData->cbUserStatus = US_READY ;
		//wUserTimer = 50;
		//////////////////////////////////////////////////////////
		

		if (pUserData!=NULL)
		{
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

	//绘画筹码
	LONG lTotalGold=0L;
	for (DWORD i=0;i<CountArray(m_GoldView);i++)
	{
		lTotalGold+=m_GoldView[i].GetGold();
		m_GoldView[i].DrawGoldView(pDC,m_GoldStation[i].x,m_GoldStation[i].y);
	}

	//游戏信息
	pDC->SetTextAlign(TA_LEFT|TA_TOP);
	pDC->SetTextColor(RGB(250,250,250));
	const UINT uDrawFormat=DT_END_ELLIPSIS|DT_CENTER|DT_SINGLELINE|DT_VCENTER;

	//游戏总注
	if (lTotalGold!=0L)
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),lTotalGold);
		DrawText(pDC->m_hDC,szBuffer,lstrlen(szBuffer),CRect(m_nXBorder+80+80,m_nYBorder+48,m_nXBorder+140+80,m_nYBorder+60),uDrawFormat);
		//m_TotalGoldView.SetGold( lTotalGold );
		//m_TotalGoldView.DrawGoldView(pDC,nWidth/2-1,nHeight/2+2);
		//创建字体
		CFont ViewFont;
		ViewFont.CreateFont(-12,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("宋体"));
		CFont * pOldFont=pDC->SelectObject(&ViewFont);
		
		DrawText( pDC->m_hDC,szBuffer,lstrlen(szBuffer),CRect(nWidth/2-40,nHeight/2-10,nWidth/2+40,nHeight/2+10),uDrawFormat );
		//清理资源
		pDC->SelectObject(pOldFont);
	}

	//最大下注
	if (m_lMaxGold!=0)
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lMaxGold);
		DrawText(pDC->m_hDC,szBuffer,lstrlen(szBuffer),CRect(m_nXBorder+80+80,m_nYBorder+10,m_nXBorder+140+80,m_nYBorder+22),uDrawFormat);
	}

	//基础下注
	if (m_lBasicGold!=0)
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lBasicGold);
		DrawText(pDC->m_hDC,szBuffer,lstrlen(szBuffer),CRect(m_nXBorder+80+80,m_nYBorder+29,m_nXBorder+140+80,m_nYBorder+41),uDrawFormat);
	}

	//显示下注控制背景
	if( m_bShowScoreControl )
	{
		CImageHandle HandleScoreControl(&m_ImageScoreControlBack);
		m_ImageScoreControlBack.AlphaDrawImage( pDC,m_ptScoreControl.x,m_ptScoreControl.y,RGB(255,0,255) );
	}

	//绘画扑克
	if (m_SendCardItemArray.GetCount()>0)
	{
		//变量定义
		CImageHandle HandleCard(&m_ImageCardBack);
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

		//绘画扑克
		m_ImageCardBack.AlphaDrawImage(pDC,m_SendCardCurPos.x,m_SendCardCurPos.y,RGB(255,0,255));
	}

	return;
}

//金币标题
void CGameClientView::SetGoldTitleInfo(DWORD lMaxGold, DWORD lBasicGold)
{
	//设置变量
	m_lMaxGold=lMaxGold;
	m_lBasicGold=lBasicGold;

	//更新按钮文字
	TCHAR szNum[16];
	_snprintf(szNum,CountArray(szNum),TEXT("%ld"),m_lBasicGold);
	m_btAddTimes1.SetWindowText(szNum);
	_snprintf(szNum,CountArray(szNum),TEXT("%ld"),m_lBasicGold*2);
 	m_btAddTimes2.SetWindowText(szNum);
	_snprintf(szNum,CountArray(szNum),TEXT("%ld"),m_lBasicGold*3);
 	m_btAddTimes3.SetWindowText(szNum);

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

//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
	//发送处理
	if (nIDEvent==IDI_SEND_CARD)
	{
		//设置位置
		m_SendCardCurPos.x += m_nXStep;
		m_SendCardCurPos.y += m_nYStep;


		//停止判断
		if ( --m_nStepCount == 0 )
		{
			//获取扑克
			BYTE cbCardData[MAX_COUNT];
			WORD wChairID=m_SendCardItemArray[0].wChairID;
			BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

			//设置扑克
			if (cbCardCount<MAX_COUNT)
			{
				cbCardData[cbCardCount++]=m_SendCardItemArray[0].cbCardData;
				m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
			}

			//删除动画
			m_SendCardItemArray.RemoveAt(0);

			//继续动画
			if (m_SendCardItemArray.GetCount()>0)
			{
				//
				wChairID = m_SendCardItemArray[0].wChairID;
				m_nStepCount = SEND_STEP_COUNT;
				m_SendCardCurPos = m_SendCardPos;
				m_nXStep = (m_CardControl[wChairID].GetTailPos().x-m_SendCardPos.x)/m_nStepCount;
				m_nYStep = (m_CardControl[wChairID].GetTailPos().y-m_SendCardPos.y)/m_nStepCount;

				//播放声音
				PlaySound(TEXT("GET_CARD"), AfxGetInstanceHandle(), SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);

				//更新界面
				UpdateGameView(NULL);

				return;
			}
			else 
			{
				//完成处理
				KillTimer(IDI_SEND_CARD);
				AfxGetMainWnd()->PostMessage(IDM_SEND_CARD_FINISH,0,0);

				//更新界面
				UpdateGameView(NULL);

				return;
			}
		}

		//更新界面
		UpdateGameView(NULL);

		return;

	}
	__super::OnTimer(nIDEvent);
}

//动画判断
bool CGameClientView::IsDispatchCard()
{
	return m_SendCardItemArray.GetCount()>0;
}

//完成发牌
void CGameClientView::FinishDispatchCard()
{
	//完成判断
	if (m_SendCardItemArray.GetCount()==0) return;

	//完成动画
	for (INT_PTR i=0;i<m_SendCardItemArray.GetCount();i++)
	{
		//获取数据
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[i];

		//获取扑克
		BYTE cbCardData[MAX_COUNT];
		WORD wChairID=pSendCardItem->wChairID;
		BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

		//设置扑克
		if (cbCardCount<MAX_COUNT)
		{
			cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
			m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
		}
	}

	//删除处理
	KillTimer(IDI_SEND_CARD);
	m_SendCardItemArray.RemoveAll();

	//更新界面
	UpdateGameView(NULL);

	return;
}

//发送扑克
void CGameClientView::DispatchUserCard(WORD wChairID, BYTE cbCardData)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//设置变量
	tagSendCardItem SendCardItem;
	SendCardItem.wChairID=wChairID;
	SendCardItem.cbCardData=cbCardData;
	m_SendCardItemArray.Add(SendCardItem);

	//启动动画
	if (m_SendCardItemArray.GetCount()==1) 
	{
		//设置定时器
		SetTimer(IDI_SEND_CARD,SPEED_SEND_CARD,NULL);

		//
		m_nStepCount = SEND_STEP_COUNT;
		m_SendCardCurPos = m_SendCardPos;
		m_nXStep = (m_CardControl[wChairID].GetTailPos().x-m_SendCardPos.x)/m_nStepCount;
		m_nYStep = (m_CardControl[wChairID].GetTailPos().y-m_SendCardPos.y)/m_nStepCount;

		//播放声音
		PlaySound(TEXT("GET_CARD2"), AfxGetInstanceHandle(), SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);

		UpdateGameView(NULL);

	}

	return;
}

//
VOID CGameClientView::ShowScoreControl( bool bShow )
{
	int nCmdShow = bShow?SW_SHOW:SW_HIDE;
	//设置界面
	m_btFollow.ShowWindow(nCmdShow);
	m_btGiveUp.ShowWindow(nCmdShow);
	m_btShowHand.ShowWindow(nCmdShow);
	m_btAddTimes1.ShowWindow(nCmdShow);
	m_btAddTimes2.ShowWindow(nCmdShow);
	m_btAddTimes3.ShowWindow(nCmdShow);

	m_bShowScoreControl = bShow;
	UpdateGameView( NULL );
}

//////////////////////////////////////////////////////////////////////////
