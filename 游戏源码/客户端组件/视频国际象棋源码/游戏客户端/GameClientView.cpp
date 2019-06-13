#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识 
#define IDC_START					100									//开始按钮
#define IDC_REGRET					101									//悔棋按钮
#define IDC_PEACE					102									//求和按钮
#define IDC_GIVEUP					103									//认输按钮
#define IDC_PRESERVE				104									//保存按钮
#define IDC_STUDY					105									//研究按钮

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_REGRET, OnRegret)
	ON_BN_CLICKED(IDC_PEACE, OnPeace)
	ON_BN_CLICKED(IDC_GIVEUP, OnGiveUp)
	ON_BN_CLICKED(IDC_PRESERVE, OnPreserve)
	ON_BN_CLICKED(IDC_STUDY, OnStudy)
	ON_MESSAGE(WM_HIT_CHESS_BORAD, OnHitChessBorad)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView() : CGameFrameView(true,24)
{
	//设置变量
	m_wGameClock=0;
	m_wBlackUser=INVALID_CHAIR;
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));
	
	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageClock.SetLoadInfo(IDB_CLOCK,hInstance);
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageFlag.SetLoadInfo(IDB_CHESS_FLAG,hInstance);
	m_ImageDecorate.SetLoadInfo(IDB_DECORATE,hInstance);
	m_ImageUserInfo.SetLoadInfo(IDB_USER_INFO,hInstance);
	m_ImageButtonBack.SetLoadInfo(IDB_BUTTON_BACK,hInstance);

	//获取大小
	CImageHandle UserInfoHandle(&m_ImageUserInfo);
	CImageHandle ButtonBackHandle(&m_ImageButtonBack);
	m_UserInfoSize.cx=m_ImageUserInfo.GetWidth();
	m_UserInfoSize.cy=m_ImageUserInfo.GetHeight();
	m_ButtonBackSize.cx=m_ImageButtonBack.GetWidth();
	m_ButtonBackSize.cy=m_ImageButtonBack.GetHeight();


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
	CRect CreateRect(0,0,0,0);
	m_ChessSelect.Create(IDD_CHESS_SELECT,this);
	m_GameScoreWnd.Create(NULL,NULL,WS_CHILD,CreateRect,this,10);
	m_ChessBorad.Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CreateRect,this,11);

	//创建按钮
	m_btStudy.Create(NULL,WS_CHILD|WS_VISIBLE,CreateRect,this,IDC_STUDY);
	m_btStart.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_START);
	m_btPeace.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_PEACE);
	m_btRegret.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_REGRET);
	m_btGiveUp.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_GIVEUP);
	m_btPreserve.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,CreateRect,this,IDC_PRESERVE);

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_START,hInstance,false);
	m_btPeace.SetButtonImage(IDB_PEACE,hInstance,false);
	m_btStudy.SetButtonImage(IDB_STUDY,hInstance,false);
	m_btGiveUp.SetButtonImage(IDB_GIVEUP,hInstance,false);
	m_btRegret.SetButtonImage(IDB_REGRET,hInstance,false);
	m_btPreserve.SetButtonImage(IDB_PRESERVE,hInstance,false);

	//请求控件
	m_PeaceRequest.InitRequest(IDM_PEACE_ANSWER,15,TEXT("对家请求 [ 和棋 ] 你是否同意？"));
	m_RegretRequest.InitRequest(IDM_REGRET_ANSWER,15,TEXT("对家请求 [ 悔棋 ] 你是否同意？"));

#ifdef VIDEO_GAME
	//创建视频
	for (WORD i=0;i<GAME_PLAYER;i++)
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

//重置界面
void CGameClientView::ResetGameView()
{
	//变量控制
	m_wGameClock=0;
	m_wBlackUser=INVALID_CHAIR;
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));

	//按钮控制
	m_btStart.EnableWindow(FALSE);
	m_btRegret.EnableWindow(FALSE);
	m_btPeace.EnableWindow(FALSE);
	m_btGiveUp.EnableWindow(FALSE);
	m_btPreserve.EnableWindow(FALSE);

	//棋盘控制
	m_ChessBorad.CleanChess();
	m_ChessBorad.SetPositively(false);
	m_ChessBorad.SetSelectChess(255,255);
	m_ChessBorad.SetChessFrame(255,255,255,255);

	//控件控制
	m_ChessSelect.ShowWindow(SW_HIDE);
	m_GameScoreWnd.ShowWindow(SW_HIDE);
	if (m_PeaceRequest.GetSafeHwnd()) m_PeaceRequest.DestroyWindow();
	if (m_RegretRequest.GetSafeHwnd()) m_RegretRequest.DestroyWindow();

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//位置变量
	int nXPos=m_nXBorder+5;
	int nYPos=(nHeight-m_UserInfoSize.cy)/2-30;

	//用户数据
	m_ptName[0].x=nXPos+18;
	m_ptName[0].y=nYPos+62;
	m_ptFace[0].x=nXPos+18;
	m_ptFace[0].y=nYPos+24;
	m_ptReady[0].x=nXPos+66;
	m_ptReady[0].y=nYPos-23;
	m_ptTimer[0].x=nXPos+65;
	m_ptTimer[0].y=nYPos+99;
	m_ChessFlagPoint[0].x=nXPos+85;
	m_ChessFlagPoint[0].y=nYPos+27;

	//用户数据
	m_ptFace[1].x=nXPos+18;
	m_ptFace[1].y=nYPos+194;
	m_ptName[1].x=nXPos+18;
	m_ptName[1].y=nYPos+233;
	m_ptReady[1].x=nXPos+66;
	m_ptReady[1].y=nYPos+375;
	m_ptTimer[1].x=nXPos+65;
	m_ptTimer[1].y=nYPos+271;
	m_ChessFlagPoint[1].x=nXPos+85;
	m_ChessFlagPoint[1].y=nYPos+197;

	//调整棋盘
	const CSize & BoradSize=m_ChessBorad.GetChessBoradSize();
	nYPos=(nHeight-BoradSize.cy)/2+10;
	nXPos=m_nXBorder+m_UserInfoSize.cx+(nWidth-m_UserInfoSize.cx-BoradSize.cx-2*m_nXBorder)/2;
	if ((nXPos+BoradSize.cx+m_nXBorder)>nWidth) nXPos=nWidth-BoradSize.cx-m_nXBorder;
	m_ChessBorad.MoveWindow(nXPos,nYPos,BoradSize.cx,BoradSize.cy);

	//调整成绩
	const CSize & ScoreSize=m_GameScoreWnd.GetGameScoreSize();
	nXPos=m_nXBorder+m_UserInfoSize.cx+(nWidth-m_UserInfoSize.cx-ScoreSize.cx-2*m_nXBorder)/2;
	m_GameScoreWnd.MoveWindow(nXPos,m_nYBorder+5,ScoreSize.cx,ScoreSize.cy);

	//调整按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(6);
	m_btStart.GetWindowRect(&rcButton);
	nYPos=nHeight-m_nYBorder-m_ButtonBackSize.cy-5;
	nXPos=m_UserInfoSize.cx+(nWidth-m_UserInfoSize.cx-m_ButtonBackSize.cx)/2;
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btStart,NULL,nXPos+25,nYPos+8,0,0,uFlags);
	DeferWindowPos(hDwp,m_btRegret,NULL,nXPos+rcButton.Width()+25,nYPos+8,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPeace,NULL,nXPos+rcButton.Width()*2+25,nYPos+8,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUp,NULL,nXPos+rcButton.Width()*3+25,nYPos+8,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPreserve,NULL,nXPos+rcButton.Width()*4+25,nYPos+8,0,0,uFlags);
	DeferWindowPos(hDwp,m_btStudy,NULL,nXPos+rcButton.Width()*5+25,nYPos+8,0,0,uFlags);


#ifdef VIDEO_GAME
	//视频窗口
	DeferWindowPos(hDwp,m_DlgVideoService[0],NULL,5,2,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_DlgVideoService[1],NULL,5,nHeight/2+270,0,0,uFlags|SWP_NOSIZE);
#endif


	EndDeferWindowPos(hDwp);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBack,enMode_Spread);

	//装饰位图
	CImageHandle DecorateHandle(&m_ImageDecorate);
	m_ImageDecorate.BitBlt(pDC->m_hDC,m_nXBorder+10,m_nYBorder+5);

	//用户区域
	CImageHandle UserInfoHandle(&m_ImageUserInfo);
	int nXPos=m_nXBorder+5;
	int nYPos=(nHeight-m_UserInfoSize.cy)/2-30;
	m_ImageUserInfo.BitBlt(pDC->m_hDC,nXPos,nYPos);

	//棋子标志
	if (m_wBlackUser!=INVALID_CHAIR)
	{
		CImageHandle FlagHandle(&m_ImageFlag);
		m_ImageFlag.BitBlt(pDC->m_hDC,m_ChessFlagPoint[0].x,m_ChessFlagPoint[0].y,m_ImageFlag.GetWidth()/2,m_ImageFlag.GetHeight(),
			(m_wBlackUser==0)?0:m_ImageFlag.GetWidth()/2,0);
		m_ImageFlag.BitBlt(pDC->m_hDC,m_ChessFlagPoint[1].x,m_ChessFlagPoint[1].y,m_ImageFlag.GetWidth()/2,m_ImageFlag.GetHeight(),
			(m_wBlackUser==1)?0:m_ImageFlag.GetWidth()/2,0);
	}

	//绘画用户
	WORD wUserTimer=0;
	pDC->SetTextColor(RGB(255,255,255));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		CRect rcName;
		const tagUserData * pUserData=GetUserInfo(i);
		if (pUserData!=NULL)
		{
			//用户区域
			rcName.top=m_ptName[i].y;
			rcName.left=m_ptName[i].x;
			rcName.right=m_ptName[i].x+102;
			rcName.bottom=m_ptName[i].y+12;
			pDC->DrawText(pUserData->szName,lstrlen(pUserData->szName),&rcName,DT_SINGLELINE|DT_END_ELLIPSIS);

			//用户时间
			DrawUserClock(pDC,m_ptTimer[i].x,m_ptTimer[i].y,m_wGameClock);
			DrawUserClock(pDC,m_ptTimer[i].x,m_ptTimer[i].y+27,m_wLeftClock[i]);

			wUserTimer = GetUserTimer(i);
			if ( 0 < wUserTimer )
				DrawUserTimer( pDC, m_ptReady[i].x, m_ptReady[i].y, wUserTimer );

			//其他信息
						if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
		}
	}

	//按钮底图
	CImageHandle ButtonBackHandle(&m_ImageButtonBack);
	nYPos=nHeight-m_nYBorder-m_ButtonBackSize.cy-5;
	nXPos=m_UserInfoSize.cx+(nWidth-m_UserInfoSize.cx-m_ButtonBackSize.cx)/2;
	m_ImageButtonBack.BitBlt(pDC->m_hDC,nXPos,nYPos);

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

//设置时间
void CGameClientView::SetGameClock(WORD wGameClock)
{
	//设置变量
	m_wGameClock=wGameClock;

	//更新界面
	UpdateUserInfoArea();

	return;
}

//设置时间
void CGameClientView::SetUserClock(WORD wViewChairID, WORD wUserClock)
{
	//效验参数
	ASSERT(wViewChairID<GAME_PLAYER);
	if (wViewChairID>=GAME_PLAYER) return;

	//设置时间
	m_wLeftClock[wViewChairID]=wUserClock;

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
	rcUserArea.left=m_nXBorder+5;
	rcUserArea.top=(rcClient.Height()-m_UserInfoSize.cy)/2-30;
	rcUserArea.right=rcUserArea.left+m_UserInfoSize.cx;
	rcUserArea.bottom=rcUserArea.top+m_UserInfoSize.cy;

	//更新界面
	UpdateGameView(&rcUserArea);

	return;
}

//用户时钟
void CGameClientView::DrawUserClock(CDC * pDC, int nXPos, int nYPos, WORD wClockCount)
{
	//加载资源
	CImageHandle ClockImageHandle(&m_ImageClock);

	//变量定义
	WORD wMinute=wClockCount/60;
	WORD wSecond=wClockCount%60;
	int nImageWidth=m_ImageClock.GetWidth()/12;
	int nImageHeight=m_ImageClock.GetHeight();
	
	//绘画分钟
	m_ImageClock.AlphaDrawImage(pDC,nXPos,nYPos,nImageWidth,nImageHeight,wMinute/10*nImageWidth,0,RGB(255,0,255));
	m_ImageClock.AlphaDrawImage(pDC,nXPos+nImageWidth+3,nYPos,nImageWidth,nImageHeight,wMinute%10*nImageWidth,0,RGB(255,0,255));

	//绘画冒号
	m_ImageClock.AlphaDrawImage(pDC,nXPos+(nImageWidth+3)*2,nYPos,nImageWidth,nImageHeight,10*nImageWidth,0,RGB(255,0,255));

	//绘画秒钟
	m_ImageClock.AlphaDrawImage(pDC,nXPos+(nImageWidth+3)*3,nYPos,nImageWidth,nImageHeight,wSecond/10*nImageWidth,0,RGB(255,0,255));
	m_ImageClock.AlphaDrawImage(pDC,nXPos+(nImageWidth+3)*4,nYPos,nImageWidth,nImageHeight,wSecond%10*nImageWidth,0,RGB(255,0,255));

	return;
}

//开始按钮
void CGameClientView::OnStart()
{
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
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
