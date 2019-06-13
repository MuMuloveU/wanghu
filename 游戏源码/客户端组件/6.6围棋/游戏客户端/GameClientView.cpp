#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识 
#define IDC_START					100									//开始按钮
#define IDC_COUNT					101									//点目按钮
#define IDC_PASS					102									//通过按钮
#define IDC_REGRET					103									//悔棋按钮
#define IDC_PEACE					104									//求和按钮
#define IDC_GIVEUP					105									//认输按钮
#define IDC_PRESERVE				106									//保存按钮
#define IDC_STUDY					107									//研究按钮

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_COUNT, OnCount)
	ON_BN_CLICKED(IDC_PASS, OnPass)
	ON_BN_CLICKED(IDC_REGRET, OnRegret)
	ON_BN_CLICKED(IDC_PEACE, OnPeace)
	ON_BN_CLICKED(IDC_GIVEUP, OnGiveUp)
	ON_BN_CLICKED(IDC_PRESERVE, OnPreserve)
	ON_BN_CLICKED(IDC_STUDY, OnStudy)
	ON_MESSAGE(WM_HIT_CHESS_BORAD, OnHitChessBorad)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//游戏变量
	m_wBlackUser=INVALID_CHAIR;
	memset(m_StatusInfo,0,sizeof(m_StatusInfo));

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageUserBlack.SetLoadInfo(IDB_USER_INFO_BLACK,hInstance);
	m_ImageUserWhite.SetLoadInfo(IDB_USER_INFO_WHITE,hInstance);

	//获取大小
	CImageHandle UserInfoHandle(&m_ImageUserBlack);
	m_UserInfoSize.cx=m_ImageUserBlack.GetWidth();
	m_UserInfoSize.cy=m_ImageUserBlack.GetHeight();

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
	m_GameScoreWnd.Create(IDD_GAME_SCORE,this);
	m_ChessBorad.Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CreateRect,this,11);

	//创建按钮
	m_btStudy.Create(NULL,WS_CHILD|WS_VISIBLE,CreateRect,this,IDC_STUDY);
	m_btCount.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_COUNT);
	m_btPass.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_PASS);
	m_btStart.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_START);
	m_btPeace.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_PEACE);
	m_btRegret.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_REGRET);
	m_btGiveUp.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_GIVEUP);
	m_btPreserve.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_PRESERVE);

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_START,hInstance,false);
	m_btCount.SetButtonImage(IDB_COUNT,hInstance,false);
	m_btPass.SetButtonImage(IDB_PASS,hInstance,false);
	m_btPeace.SetButtonImage(IDB_PEACE,hInstance,false);
	m_btStudy.SetButtonImage(IDB_STUDY,hInstance,false);
	m_btGiveUp.SetButtonImage(IDB_GIVEUP,hInstance,false);
	m_btRegret.SetButtonImage(IDB_REGRET,hInstance,false);
	m_btPreserve.SetButtonImage(IDB_PRESERVE,hInstance,false);

	//请求控件
	m_PeaceRequest.InitRequest(IDM_PEACE_ANSWER,15,TEXT("对家请求 [ 和棋 ] 你是否同意？"));
	m_RegretRequest.InitRequest(IDM_REGRET_ANSWER,15,TEXT("对家请求 [ 悔棋 ] 你是否同意？"));

	return 0;
}

//重置界面
void CGameClientView::ResetGameView()
{
	//游戏变量
	m_wBlackUser=INVALID_CHAIR;
	memset(m_StatusInfo,0,sizeof(m_StatusInfo));

	//按钮控制
	m_btStart.EnableWindow(FALSE);
	m_btRegret.EnableWindow(FALSE);
	m_btCount.EnableWindow(FALSE);
	m_btPeace.EnableWindow(FALSE);
	m_btGiveUp.EnableWindow(FALSE);
	m_btPreserve.EnableWindow(FALSE);

	//棋盘控制
	m_ChessBorad.CleanChess();
	m_ChessBorad.SetSelectMode(SM_NO_SELECT);

	//控件控制
	m_GameScoreWnd.ShowWindow(SW_HIDE);
	if (m_GameRule.GetSafeHwnd()) m_GameRule.DestroyWindow();
	if (m_PeaceRequest.GetSafeHwnd()) m_PeaceRequest.DestroyWindow();
	if (m_RegretRequest.GetSafeHwnd()) m_RegretRequest.DestroyWindow();
	if (m_ChessCountInfo.GetSafeHwnd()) m_ChessCountInfo.DestroyWindow();

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//位置变量
	int nXPos=0,nYPos=0;
	const CSize & BoradSize=m_ChessBorad.GetChessBoradSize();

	//移动用户
	int nControlSpace=(nHeight-BoradSize.cy-m_UserInfoSize.cy-2*m_nYBorder)/3;
	nYPos=nControlSpace*2+BoradSize.cy+m_nYBorder;

	//用户数据
	int nYReady = 200;
	m_ptName[0].x=nWidth/2-189;
	m_ptName[0].y=nYPos+10;
	m_ptReady[0].x=nXPos+66;
	m_ptReady[0].y=nYReady;
	m_ptTimer[0].x=nWidth/2-267;
	m_ptTimer[0].y=nYPos+32;

	//用户数据
	m_ptName[1].x=nWidth/2+85;
	m_ptName[1].y=nYPos+10;
	m_ptReady[1].x=nXPos+66;
	m_ptReady[1].y=nHeight-nYReady;
	m_ptTimer[1].x=nWidth/2+76;
	m_ptTimer[1].y=nYPos+32;

	//调整棋盘
	nXPos=(nWidth-BoradSize.cx)/2;
	nYPos=m_nYBorder+25;
	m_ChessBorad.MoveWindow(nXPos,nYPos,BoradSize.cx,BoradSize.cy);

	//调整成绩
	CRect rcScore;
	m_GameScoreWnd.GetWindowRect(&rcScore);
	m_GameScoreWnd.MoveWindow((nWidth-rcScore.Width())/2,(nHeight-rcScore.Height())/2-30,rcScore.Width(),rcScore.Height());

	//调整按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(8);
	m_btStart.GetWindowRect(&rcButton);
	nYPos=((BoradSize.cy-rcButton.Height()*8))/2+25+m_nYBorder;
	nXPos=nWidth-((nWidth-BoradSize.cx)/2-m_nXBorder-rcButton.Width())/2-rcButton.Width()-m_nXBorder;
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btStart,NULL,nXPos,nYPos,0,0,uFlags);
	DeferWindowPos(hDwp,m_btCount,NULL,nXPos,nYPos+rcButton.Height(),0,0,uFlags);
	DeferWindowPos(hDwp,m_btPass,NULL,nXPos,nYPos+rcButton.Height()*2,0,0,uFlags);
	DeferWindowPos(hDwp,m_btRegret,NULL,nXPos,nYPos+rcButton.Height()*3,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPeace,NULL,nXPos,nYPos+rcButton.Height()*4,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUp,NULL,nXPos,nYPos+rcButton.Height()*5,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPreserve,NULL,nXPos,nYPos+rcButton.Height()*6,0,0,uFlags);
	DeferWindowPos(hDwp,m_btStudy,NULL,nXPos,nYPos+rcButton.Height()*7,0,0,uFlags);
	EndDeferWindowPos(hDwp);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBack,enMode_ElongGate);

	//用户信息
	CImageHandle HandleBlack(&m_ImageUserBlack);
	CImageHandle HandleWhite(&m_ImageUserWhite);
	const CSize & BoradSize=m_ChessBorad.GetChessBoradSize();
	int nControlSpace=(nHeight-BoradSize.cy-m_UserInfoSize.cy-2*m_nYBorder)/3;
	int nYPos=nControlSpace*2+BoradSize.cy+m_nYBorder;
	m_ImageUserWhite.BitBlt(pDC->m_hDC,nWidth/2+20,nYPos);
	m_ImageUserBlack.BitBlt(pDC->m_hDC,nWidth/2-m_ImageUserBlack.GetWidth()-20,nYPos);

	//绘画用户
	WORD wUserTimer=0;
	TCHAR szBuffer[64];
	pDC->SetTextColor(RGB(10,10,10));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=GetUserInfo(i);
		int nIndex=(m_wBlackUser==0)?i:((i+1)%GAME_PLAYER);

		//用户信息
		if (pUserData!=NULL)
		{
			pDC->TextOut(m_ptName[nIndex].x,m_ptName[nIndex].y,pUserData->szName,lstrlen(pUserData->szName));
			if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);

			wUserTimer = GetUserTimer(i);
			if ( 0 < wUserTimer )
				DrawUserTimer( pDC, m_ptReady[i].x, m_ptReady[i].y, wUserTimer );
		}

		//限时信息
		WORD wLimitTimeCount=m_StatusInfo[nIndex].wLimitTimeCount;
		pDC->SetTextColor(((wLimitTimeCount<=15)&&(wLimitTimeCount!=0))?RGB(250,250,250):RGB(10,10,10));
		DrawUserClock(pDC,m_ptTimer[i].x,m_ptTimer[i].y,m_StatusInfo[nIndex].wLimitTimeCount);


		//用时信息
		pDC->SetTextColor(RGB(10,10,10));
		DrawUserClock(pDC,m_ptTimer[i].x+180,m_ptTimer[i].y,m_StatusInfo[nIndex].wUseTimeCount);

		//提子信息
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),m_StatusInfo[nIndex].wTakeChessCount);
		pDC->TextOut(m_ptTimer[i].x,m_ptTimer[i].y+25,szBuffer,lstrlen(szBuffer));

		//目数信息
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),m_StatusInfo[nIndex].wStepCount);
		pDC->TextOut(m_ptTimer[i].x+97,m_ptTimer[i].y+25,szBuffer,lstrlen(szBuffer));
	}

	return;
}

//和棋请求
void CGameClientView::ShowPeaceRequest()
{
	if (m_PeaceRequest.m_hWnd==NULL) 
	{
		m_PeaceRequest.Create(IDD_USER_REQ,this);
		m_PeaceRequest.ShowWindow(SW_SHOW);
	}

	return;
}

//悔棋请求
void CGameClientView::ShowRegretRequest()
{
	if (m_RegretRequest.m_hWnd==NULL) 
	{
		m_RegretRequest.Create(IDD_USER_REQ,this);
		m_RegretRequest.ShowWindow(SW_SHOW);
	}

	return;
}

//设置黑棋
void CGameClientView::SetBlackUser(WORD wBlackUser)
{
	if (m_wBlackUser!=wBlackUser)
	{
		//设置变量
		m_wBlackUser=wBlackUser;

		//更新界面
		UpdateUserInfoArea();
	}

	return;
}

//设置信息
void CGameClientView::SetUserStatusInfo(WORD wViewChairID, tagStatusInfo & StatusInfo)
{
	//设置变量
	m_StatusInfo[wViewChairID]=StatusInfo;

	//更新界面
	UpdateUserInfoArea();

	return;
}

//更新状态
void CGameClientView::UpdateUserInfoArea()
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//设置变量
	CRect rcUserArea;
	rcUserArea.left=m_nXBorder;
	rcUserArea.right=rcClient.Width()-m_nXBorder;
	rcUserArea.bottom=rcClient.Height()-m_nYBorder;
	rcUserArea.top=rcClient.Height()/2+m_ChessBorad.GetChessBoradSize().cy/2-m_UserInfoSize.cy/2+13;

	//更新界面
	UpdateGameView(&rcUserArea);

	return;
}

//用户时钟
void CGameClientView::DrawUserClock(CDC * pDC, int nXPos, int nYPos, WORD wClockCount)
{
	//变量定义
	WORD wHour=wClockCount/3600;
	WORD wSecond=wClockCount%60;
	WORD wMinute=(wClockCount-wHour*3600)/60;

	//构造字符
	TCHAR szTimeDesc[64];
	_snprintf(szTimeDesc,sizeof(szTimeDesc),TEXT("%01d:%02d:%02d"),wHour,wMinute,wSecond);

	//绘画时钟
	pDC->TextOut(nXPos,nYPos,szTimeDesc,lstrlen(szTimeDesc));

	return;
}

//开始按钮
void CGameClientView::OnStart()
{
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	return;
}

//点目按钮
void CGameClientView::OnCount()
{
	AfxGetMainWnd()->SendMessage(IDM_COUNT,0,0);
	return;
}

//通过按钮
void CGameClientView::OnPass()
{
	AfxGetMainWnd()->SendMessage(IDM_PASS,0,0);
	return;
}

//悔棋按钮
void CGameClientView::OnRegret()
{
	AfxGetMainWnd()->SendMessage(IDM_REGRET,0,0);
	return;
}

//求和按钮
void CGameClientView::OnPeace()
{
	AfxGetMainWnd()->SendMessage(IDM_PEACE,0,0);
	return;
}

//认输按钮
void CGameClientView::OnGiveUp()
{
	AfxGetMainWnd()->SendMessage(IDM_GIVEUP,0,0);
	return;
}

//保存按钮
void CGameClientView::OnPreserve()
{
	AfxGetMainWnd()->SendMessage(IDM_PRESERVE,0,0);
	return;
}

//研究按钮
void CGameClientView::OnStudy()
{
	AfxGetMainWnd()->SendMessage(IDM_STUDY,0,0);
	return;
}

//点击棋盘
LRESULT CGameClientView::OnHitChessBorad(WPARAM wParam, LPARAM lParam)
{
	AfxGetMainWnd()->SendMessage(WM_HIT_CHESS_BORAD,wParam,lParam);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
