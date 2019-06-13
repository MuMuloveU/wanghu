#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include "Math.h"

//////////////////////////////////////////////////////////////////////////
//按钮标识 

#define IDC_START						100								//开始按钮
#define IDC_HAIDI						101								//海底按钮
#define IDC_NOHAIDI						102								//放弃按钮
#define IDC_CONSIGN_ENTER				103								//托管按钮
#define IDC_CONSIGN_CANCEL				104								//取消托管

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_CONSIGN_ENTER, OnConsignEnter)
	ON_BN_CLICKED(IDC_CONSIGN_CANCEL, OnConsignCancel)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(IDataCtrl* pDataCtrl)
:	CGameFrameView(true,16),
	m_pDataCtrl(pDataCtrl),
	m_HandCardControl(pDataCtrl),
	m_GameScoreWnd(pDataCtrl),
	m_ControlWnd(pDataCtrl)
{
	//游戏属性
	m_bOutCard=false;
	m_bWaitOther=false;
	m_wBankerUser=INVALID_CHAIR;

	//用户状态
	m_cbCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_bUserAction,sizeof(m_bUserAction));

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageWait.SetLoadInfo(IDB_WAIT_TIP,hInstance);
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageUserFlag.SetLoadInfo(IDB_USER_FLAG,hInstance);
	m_ImageOutCard.SetLoadInfo(IDB_OUT_CARD_TIP,hInstance);
	m_ImageUserAction.SetLoadInfo(IDB_USER_ACTION,hInstance);
	m_ImageActionBack.SetLoadInfo(IDB_ACTION_BACK,hInstance);
	m_ImageMoneyLeft.SetLoadInfo(IDB_MONEY_LEFT,hInstance);
	m_ImageTipInfo.SetLoadInfo(IDB_TIP_INFO, hInstance);

	//设置数据指针
	for(int i=0; i<4; i++)
	{
		m_TableCard[i].SetDataCtrl(m_pDataCtrl);
	}
	for(int i=0; i<4; i++)
	{
		m_HeapCard[i].SetDataCtrl(m_pDataCtrl);
	}


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
		m_WeaveCard[i][0].SetDisplayItem(true);
		m_WeaveCard[i][1].SetDisplayItem(true);
		m_WeaveCard[i][2].SetDisplayItem(true);
		m_WeaveCard[i][3].SetDisplayItem(true);
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
	CRect CreateRect(0,0,0,0);
	m_GameScoreWnd.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,CreateRect,this,100);
	m_ControlWnd.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,CreateRect,this,10,NULL);

	//创建控件
	m_btStart.Create(NULL,WS_CHILD,CreateRect,this,IDC_START);
	m_btStart.SetButtonImage(IDB_BT_START,AfxGetInstanceHandle(),false);
	m_btConsignEnter.Create(NULL,WS_CHILD,CreateRect,this,IDC_CONSIGN_ENTER);
	m_btConsignEnter.SetButtonImage(IDB_CONSIGN_ENTER,AfxGetInstanceHandle(),false);
	m_btConsignCancel.Create(NULL,WS_CHILD,CreateRect,this,IDC_CONSIGN_CANCEL);
	m_btConsignCancel.SetButtonImage(IDB_CONSIGN_CANCEL,AfxGetInstanceHandle(),false);
	m_btConsignCancel.EnableWindow(FALSE);	

	return 0;
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
	if (bRePaint==true) UpdateGameView(NULL);

	//光标控制
	if (bHandle==false) __super::OnSetCursor(pWnd,nHitTest,uMessage);

	return TRUE;
}

//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//获取扑克
	BYTE cbHoverCard=m_HandCardControl.GetHoverCard();
	//王牌不能打出
	if(cbHoverCard == CARD_KING_DATA_MASK)
	{
		return;
	}
	if (cbHoverCard!=0) AfxGetMainWnd()->PostMessage(IDM_OUT_CARD,cbHoverCard,cbHoverCard);

	return;
}

//重置界面
void CGameClientView::ResetGameView()
{
	//游戏属性
	m_bOutCard=false;
	m_bWaitOther=false;
	m_wBankerUser=INVALID_CHAIR;

	//用户状态
	m_cbCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_bUserAction,sizeof(m_bUserAction));

	//界面设置
	m_btStart.ShowWindow(SW_HIDE);
	m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameScoreWnd.ShowWindow(SW_HIDE);

	//扑克设置
	m_UserCard[0].SetCardData(0,false);
	m_UserCard[1].SetCardData(0,false);
	m_UserCard[2].SetCardData(0,false);
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

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//设置坐标
	m_ptFace[0].x=nWidth/2-m_nXFace/2;
	m_ptFace[0].y=m_nYBorder+5;
	m_ptName[0].x=nWidth/2+5+m_nXFace/2;
	m_ptName[0].y=m_nYBorder+5;
	m_ptTimer[0].x=nWidth/2-m_nXFace-10;
	m_ptTimer[0].y=m_nYBorder+m_nYTimer/2+5;
	m_ptReady[0].x=nWidth/2-m_nXFace/2;
	m_ptReady[0].y=m_nYBorder+m_nYTimer/2+150;
	m_UserFlagPos[0].x=nWidth/2-m_nXFace-m_nXTimer*2-30;
	m_UserFlagPos[0].y=5+m_nYBorder;

	m_ptFace[1].x=m_nXBorder+5;
	m_ptFace[1].y=nHeight/2-m_nYFace;
	m_ptName[1].x=m_nXBorder+10;
	m_ptName[1].y=nHeight/2+3;
	m_ptTimer[1].x=m_nXBorder+m_nXTimer/2+5;
	m_ptTimer[1].y=nHeight/2-m_nYFace-5-m_nYTimer/2;
	m_ptReady[1].x=m_nXBorder+150+m_nXTimer;
	m_ptReady[1].y=nHeight/2-m_nYFace/2;
	m_UserFlagPos[1].x=nWidth-m_nXBorder-25;
	m_UserFlagPos[1].y=nHeight/2-m_nYFace-80-m_nYTimer;

	m_ptFace[2].x=nWidth/2-m_nXFace/2;
	m_ptFace[2].y=nHeight-m_nYBorder-m_nYFace-5;
	m_ptName[2].x=nWidth/2+5+m_nXFace/2;
	m_ptName[2].y=nHeight-m_nYBorder-m_nYFace-2;
	m_ptTimer[2].x=nWidth/2-m_nXFace-10;
	m_ptTimer[2].y=nHeight-m_nYBorder-m_nYTimer/2-5;
	m_ptReady[2].x=nWidth/2-m_nXFace/2;
	m_ptReady[2].y=nHeight-m_nYBorder-m_nYTimer/2-150;
	m_UserFlagPos[2].x=nWidth/2-m_nXFace/2-m_nXTimer*2-30;
	m_UserFlagPos[2].y=nHeight-m_nYBorder-m_nYTimer/2-5;

	m_ptFace[3].x=nWidth-m_nXBorder-m_nXFace-5;
	m_ptFace[3].y=nHeight/2-m_nYFace;
	m_ptName[3].x=nWidth-m_nXBorder-5;
	m_ptName[3].y=nHeight/2+3;
	m_ptTimer[3].x=nWidth-m_nXBorder-m_nXTimer/2-5;
	m_ptTimer[3].y=nHeight/2-m_nYFace-m_nYTimer/2-5;
	m_ptReady[3].x=nWidth-m_nXBorder-m_nXTimer-150;
	m_ptReady[3].y=nHeight/2-m_nYFace/2;
	m_UserFlagPos[3].x=m_nXBorder+5;
	m_UserFlagPos[3].y=nHeight/2-m_nYFace-80-m_nYTimer;

	//用户扑克
	m_UserCard[0].SetControlPoint(nWidth/2-200,m_nYBorder+m_nYFace+25);
	m_UserCard[1].SetControlPoint(nWidth-m_nXBorder-m_nXFace-49,nHeight/2-255);
	m_UserCard[2].SetControlPoint(m_nXBorder+m_nXFace+30,nHeight/2+177);
	m_HandCardControl.SetBenchmarkPos(nWidth/2,nHeight-m_nYFace-m_nYBorder-25,enXCenter,enYBottom);

	//桌面扑克
	m_TableCard[0].SetControlPoint(nWidth/2-179,m_nYBorder+m_nYFace+25);
	m_TableCard[1].SetControlPoint(nWidth-m_nXBorder-m_nXFace-30,nHeight/2-197);
	m_TableCard[2].SetControlPoint(nWidth/2+185,nHeight-m_nYFace-m_nYBorder-25);
	m_TableCard[3].SetControlPoint(m_nXBorder+m_nXFace+30,nHeight/2+97);

	//堆积扑克
	int nXCenter=nWidth/2;
	int nYCenter=nHeight/2-40;
	m_HeapCard[0].SetControlPoint(nXCenter-98,nYCenter-108);
	m_HeapCard[1].SetControlPoint(nXCenter+101,nYCenter-68);
	m_HeapCard[2].SetControlPoint(nXCenter-192,nYCenter+73);
	m_HeapCard[3].SetControlPoint(nXCenter-123,nYCenter-172);

	//丢弃扑克
	m_DiscardCard[0].SetControlPoint(nXCenter-98,nYCenter-110);
	m_DiscardCard[1].SetControlPoint(nXCenter+125,nYCenter-68);
	m_DiscardCard[2].SetControlPoint(nXCenter+71,nYCenter+112);
	m_DiscardCard[3].SetControlPoint(nXCenter-126,nYCenter+59);

	//组合扑克
	m_WeaveCard[0][0].SetControlPoint(nWidth/2+145,m_nYBorder+m_nYFace+52);
	m_WeaveCard[0][1].SetControlPoint(nWidth/2+55,m_nYBorder+m_nYFace+52);
	m_WeaveCard[0][2].SetControlPoint(nWidth/2-35,m_nYBorder+m_nYFace+52);
	m_WeaveCard[0][3].SetControlPoint(nWidth/2-125,m_nYBorder+m_nYFace+52);

	//组合扑克
	m_WeaveCard[1][0].SetControlPoint(nWidth-m_nXBorder-m_nXFace-83,nHeight/2+100);
	m_WeaveCard[1][1].SetControlPoint(nWidth-m_nXBorder-m_nXFace-83,nHeight/2+20);
	m_WeaveCard[1][2].SetControlPoint(nWidth-m_nXBorder-m_nXFace-83,nHeight/2-60);
	m_WeaveCard[1][3].SetControlPoint(nWidth-m_nXBorder-m_nXFace-83,nHeight/2-140);

	//组合扑克
	m_WeaveCard[2][0].SetControlPoint(nWidth/2-190,nHeight-m_nYFace-m_nYBorder-73);
	m_WeaveCard[2][1].SetControlPoint(nWidth/2-100,nHeight-m_nYFace-m_nYBorder-73);
	m_WeaveCard[2][2].SetControlPoint(nWidth/2-10,nHeight-m_nYFace-m_nYBorder-73);
	m_WeaveCard[2][3].SetControlPoint(nWidth/2+80,nHeight-m_nYFace-m_nYBorder-73);

	//组合扑克
	m_WeaveCard[3][0].SetControlPoint(m_nXBorder+m_nXFace+82,nHeight/2-200);
	m_WeaveCard[3][1].SetControlPoint(m_nXBorder+m_nXFace+82,nHeight/2-120);
	m_WeaveCard[3][2].SetControlPoint(m_nXBorder+m_nXFace+82,nHeight/2-40);
	m_WeaveCard[3][3].SetControlPoint(m_nXBorder+m_nXFace+82,nHeight/2+40);

	//控制窗口
	m_ControlWnd.SetBenchmarkPos(nWidth/2+298,nHeight-m_nYBorder-120);

	//调整成绩
	CRect rcScore;
	m_GameScoreWnd.GetWindowRect(&rcScore);
	m_GameScoreWnd.MoveWindow((nWidth-rcScore.Width())/2,(nHeight-rcScore.Height())/2-30,rcScore.Width(),rcScore.Height());

	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(3);
	m_btStart.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,nHeight-155-m_nYBorder,0,0,uFlags);
	m_btConsignEnter.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btConsignEnter,NULL,(nWidth-rcButton.Width()) - 15,nHeight-m_nYBorder-rcButton.Height()*2 - 5,0,0,uFlags);
	m_btConsignCancel.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btConsignCancel,NULL,(nWidth-rcButton.Width()) - 15,nHeight-m_nYBorder-rcButton.Height(),0,0,uFlags);

	EndDeferWindowPos(hDwp);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBack,enMode_Spread);
	//剩余财富
	_DrawScoreLeft(pDC, nWidth, nHeight);
	//画提示信息
	_DrawTipInfo(pDC, nWidth, nHeight);

	//绘画用户	
	pDC->SetTextColor(RGB(255,255,0));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);

		//test
		//WORD wUserTimer = 30;
		//tagUserData *pUserData = new tagUserData();
		//lstrcpy(pUserData->szName,TEXT("SLDJLSK"));
		//pUserData->cbUserStatus = US_READY;
		//pUserData->lScore = 1000l;

		if (pUserData!=NULL)
		{
			//用户名字
			pDC->SetTextAlign((i==3)?TA_RIGHT:TA_LEFT);
			pDC->TextOut(m_ptName[i].x,m_ptName[i].y,pUserData->szName,lstrlen(pUserData->szName));

			//其他信息
			WORD wUserTimer=GetUserTimer(i);
			if (wUserTimer!=0) 
			{
				DrawUserTimer(pDC,m_ptTimer[i].x,m_ptTimer[i].y,wUserTimer);
			}
			if (m_pDataCtrl && m_pDataCtrl->IsUserReady(pUserData->wChairID)) 
			{
				DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			}
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
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
			m_ImageUserFlag.BitBlt(pDC->m_hDC,m_UserFlagPos[i].x,m_UserFlagPos[i].y,nImageWidth,nImageHeight,nImageWidth*wIndex,0);
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
		m_ImageOutCard.AlphaDrawImage(pDC,(nWidth-m_ImageOutCard.GetWidth())/2,nHeight-150,RGB(255,0,255));
	}

	//等待提示
	if (m_bWaitOther==true)
	{
		CImageHandle HandleWait(&m_ImageWait);
		m_ImageWait.AlphaDrawImage(pDC,(nWidth-m_ImageWait.GetWidth())/2,nHeight-150,RGB(255,0,255));
	}

	//用户状态
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if ((m_wOutCardUser==i)||(m_bUserAction[i]!=0))
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

			//动作背景
			CImageHandle ImageHandle(&m_ImageActionBack);
			m_ImageActionBack.AlphaDrawImage(pDC,nXPos,nYPos,m_ImageActionBack.GetWidth()/4,m_ImageActionBack.GetHeight(),
				i*m_ImageActionBack.GetWidth()/4,0,RGB(255,0,255));

			//绘画动作
			if (m_bUserAction[i]!=0)
			{
				//变量定义
				int nXImagePos=0;
				CImageHandle ImageHandle(&m_ImageUserAction);
				int nItemWidth=m_ImageUserAction.GetWidth()/7;

				//计算位置
				if (m_bUserAction[i]&ACK_PENG) nXImagePos=nItemWidth;
				else if (m_bUserAction[i]&ACK_GANG) nXImagePos=nItemWidth*3;
				else if (m_bUserAction[i]&ACK_CHIHU) nXImagePos=nItemWidth*4;

				//绘画动作
				int nItemHeight=m_ImageUserAction.GetHeight();
				m_ImageUserAction.BitBlt(pDC->m_hDC,nXPos+13,nYPos+15,nItemWidth,nItemHeight,nXImagePos,0);
			}
			else
			{
				//绘画扑克
				g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,m_cbCardData,nXPos+18,nYPos+15);
			}
		}
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
void CGameClientView::SetUserAction(WORD wViewChairID, BYTE bUserAction)
{
	//设置变量
	if (wViewChairID<GAME_PLAYER) 
	{
		m_bUserAction[wViewChairID]=bUserAction;
	}
	else 
	{
		ZeroMemory(m_bUserAction,sizeof(m_bUserAction));
	}

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

//托管按钮
void CGameClientView::OnConsignEnter()
{
	AfxGetMainWnd()->SendMessage(IDM_CONSIGN_CHANGED,1,0);
	return;
}

//取消托管
void CGameClientView::OnConsignCancel()
{
	AfxGetMainWnd()->SendMessage(IDM_CONSIGN_CHANGED,0,0);
	return;
}

//托管信息
void CGameClientView::SetConsign(bool bEnter)
{
	if(bEnter)
	{
		m_btConsignEnter.EnableWindow(FALSE);
		m_btConsignCancel.EnableWindow(TRUE);
	}
	else
	{
		m_btConsignEnter.EnableWindow(TRUE);
		m_btConsignCancel.EnableWindow(FALSE);
	}
}

//画剩余财富
void CGameClientView::_DrawScoreLeft(CDC * pDC, int nWidth, int nHeight)
{
#ifdef MJ_DESPERATE
	CImageHandle hImgMoneyLeft(&m_ImageMoneyLeft);
	hImgMoneyLeft->Draw(pDC->GetSafeHdc(), m_ptFace[2].x-hImgMoneyLeft->GetWidth()-65, nHeight - hImgMoneyLeft->GetHeight()-m_nYBorder);
	CPoint ptText[4];
	ptText[0] = CPoint(m_ptName[0].x, m_ptName[0].y + 20);
	ptText[1] = CPoint(m_ptName[1].x, m_ptName[1].y + 20);
	ptText[2] = CPoint(m_ptFace[2].x-hImgMoneyLeft->GetWidth()/2-50, nHeight-hImgMoneyLeft->GetHeight()-m_nYBorder+11);
	ptText[3] = CPoint(m_ptName[3].x, m_ptName[3].y + 20);

	CString strTmp;
	for(int i=0; i<4; i++)
	{
		const tagUserData* pUserData = GetUserInfo(i);
		if(pUserData != 0)
		{
			strTmp.Format("%d", m_pDataCtrl->GetScoreLeft(pUserData->wChairID));
			pDC->SetTextAlign((i==1)?(TA_RIGHT|TA_TOP):(TA_LEFT|TA_TOP));
			pDC->SetTextColor((i==2)?(RGB(0,0,0)):(RGB(255,255,0)));
			pDC->TextOut(ptText[i].x, ptText[i].y, strTmp);				
		}
	}

#endif	

}

//画提示信息
void CGameClientView::_DrawTipInfo(CDC * pDC, int nWidth, int nHeight)
{
	if(m_wBankerUser == INVALID_CHAIR)
	{
		return;
	}

	CImageHandle hTipInfo(&m_ImageTipInfo);

	int iX = (nWidth-hTipInfo->GetWidth())/2;
	int iY = (nHeight-hTipInfo->GetHeight())/2-35;
	hTipInfo->Draw(pDC->GetSafeHdc(), iX, iY);

	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetTextAlign(TA_LEFT);
	//画财神
	iX += 5;
	iY += 23;
	g_CardResource.m_ImageUserBottom.DrawCardItem(pDC,m_pDataCtrl->GetKingCardData(), iX, iY);
	//画庄家
	iX += 105;
	iY += 15;
	const tagUserData * pUserData=GetUserInfo(m_wBankerUser);
	if(pUserData != 0)
	{
		pDC->TextOut(iX, iY, pUserData->szName);
	}
	//画底注
	iY += 27;
	CString szTmp;
	szTmp.Format("%d", m_pDataCtrl->GetCellScore());
	pDC->TextOut(iX, iY, szTmp);

}


// 说明(映射方式MM_LOMETRIC下)：
//
//     在矩形框中水平或垂直显示多行文字
//
// 参数：
//       pDC: 绘制DC
//
//  szString: 绘制的字符串
//
//    lpRect: 绘制的矩形范围
//
//     lMode: 排列方式，0:水平方式; 1:垂直方式 
//
//     lHori: 水平对齐方式, 0:左对齐; 1:居中; 2:右对齐; 3：自定义
//
//     lVert: 垂直对齐方式, 0:顶对齐; 1:居中; 2:底对齐; 3：自定义
///////////////////////////////////////////////////////

CRect CGameClientView::_DrawTitleInRect(CDC *pDC, CString szString, LPRECT lpRect, long lMode, long lHori, long lVert)
{
	CRect rcInner(lpRect);

	if(rcInner.Width() ==0)
		return rcInner;

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int tmpWidth=tm.tmAveCharWidth, tmpHeight=tm.tmHeight;

	//---------------------------------------------------------------------------------------------
	//功能：根据新、老矩形，重新计算行数，使文字多行显示,jingzhou xu
	//---------------------------------------------------------------------------------------------
	//一行中最大字符数
	int nMaxLineChar = abs(lpRect->right - lpRect->left) / tmpWidth; 
	if(nMaxLineChar < 2)               //应该至少能显示一个汉字
		return rcInner;

	//记录当前行的宽度
	short theLineLength=0; 
	//记录当前行中汉字字节数，以防止将一半汉字分为两行
	unsigned short halfChinese=0;

	for(int i=0; i<=szString.GetLength()-1; i++)
	{
		if(((unsigned char)szString.GetAt(i) == 0x0d) && ((unsigned char)szString.GetAt(i+1) == 0x0a))
			theLineLength=0;

		// 在此加入"||"字符为换行标志字符，输入时可根据此字符串来自动换行
		if(((unsigned char)szString.GetAt(i) == '|') && ((unsigned char)szString.GetAt(i+1) == '|'))
		{
			szString.SetAt(i,(unsigned char)0x0d);
			szString.SetAt(i+1,(unsigned char)0x0a);
		}

		//大于0xa1的字节为汉字字节
		if((unsigned char)szString.GetAt(i) >= 0xA1)
			halfChinese++;
		theLineLength++;

		//如果行宽大于每行最大宽度，进行特殊处理
		if(theLineLength > nMaxLineChar)
		{
			//防止将一个汉字分为两行，回溯
			if(!(halfChinese%2) && (unsigned char)szString.GetAt(i) >= 0xA1)
			{
				szString.Insert(i-1,(unsigned char)0x0a);
				szString.Insert(i-1,(unsigned char)0x0d);
				//注：此处不加一跳过，是由于它是在i-1处添加，只需跳到<i+1>处，故只需在循环处加一次既可。
			}
			else
			{
				szString.Insert(i,(unsigned char)0x0a);
				szString.Insert(i,(unsigned char)0x0d);

				i++;       //跳过新增的换行符，应跳到<i+2>处(循环中加一次，故这里只加一次)
			}

			theLineLength = 0;
			halfChinese=0;
		}
	}

	if(lMode==0)        //水平排列 
	{
		rcInner.left+=tmpWidth;
		rcInner.right-=tmpWidth;
		rcInner.top-=tmpWidth;
		rcInner.bottom+=tmpWidth;
	}
	if(lMode==1)        //垂直排列
	{
		rcInner.left+=tmpWidth;
		rcInner.right=rcInner.left+tmpWidth;
		rcInner.top-=tmpWidth;
		rcInner.bottom+=tmpWidth;
	}

	//重新计算矩形边界范围
	pDC->DrawText(szString, rcInner,DT_WORDBREAK|DT_LEFT|DT_CALCRECT);

	switch(lHori)
	{
	case 0:
		break;
	case 1:
		{
			long xOutCent=(lpRect->right+lpRect->left)/2;
			long xInnCent=(rcInner.right+rcInner.left)/2;
			rcInner.left+=(xOutCent-xInnCent);
			rcInner.right+=(xOutCent-xInnCent);
		}
		break;
	case 2:
		{
			long lInWidth=rcInner.right-rcInner.left;
			rcInner.right=lpRect->right-tmpWidth;
			rcInner.left=rcInner.right-lInWidth;
		}
		break;
	default:
		break;
	}

	switch(lVert)
	{
	case 0:
		break;
	case 1:
		{
			long yOutCent=(lpRect->bottom+lpRect->top)/2;
			long yInnCent=(rcInner.bottom+rcInner.top)/2;
			rcInner.top-=(yInnCent-yOutCent);
			rcInner.bottom-=(yInnCent-yOutCent);
		}
		break;
	case 2:
		{
			long lInHeigh=rcInner.top-rcInner.bottom;
			rcInner.bottom=lpRect->bottom+tmpWidth;
			rcInner.top=rcInner.bottom+lInHeigh;
		}
		break;
	default:
		break;
	}

	if(rcInner.bottom < lpRect->bottom)
		rcInner.bottom = lpRect->bottom;
	if(rcInner.top > lpRect->top)
		rcInner.top = lpRect->top;

	//---------------------------------------------------------------------------------------------

	if(lHori==0)
		pDC->DrawText(szString, rcInner, DT_WORDBREAK|DT_LEFT);
	else if(lHori==1)
		pDC->DrawText(szString, rcInner, DT_WORDBREAK|DT_CENTER);
	else if(lHori==2)
		pDC->DrawText(szString, rcInner, DT_WORDBREAK|DT_RIGHT);

	return rcInner;


}
//////////////////////////////////////////////////////////////////////////
