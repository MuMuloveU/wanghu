#include "StdAfx.h"
#include "Resource.h"
#include "GameClientDlg.h"
#include "GameClient.h"
#include "GameClientView.h"
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识 
#define IDC_PLACEMENT				100									//布局按钮
#define IDC_SAVEPLACEMENT			102									//保存按钮
#define IDC_LOADPLACEMENT			103									//载入按钮
#define IDC_RESETGAME				104									//重新游戏
#define IDC_LOAD_HISTORY			105									//载入棋局
#define IDC_PREV_STEP				106									//上一步棋
#define IDC_NEXT_STEP				107									//下一步棋
#define IDC_TURN_CHESS				108									//旋转棋盘
#define IDC_LOSE					109									//认输按钮
#define IDC_GIVEUP					110									//放弃按钮
#define IDC_PEACE					111									//和棋按钮




//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_PLACEMENT,OnHitPlacement)
	ON_BN_CLICKED(IDC_RESETGAME , OnHitResetGame)
	ON_BN_CLICKED(IDC_SAVEPLACEMENT , OnSavePlacement)
	ON_BN_CLICKED(IDC_LOADPLACEMENT , OnHitLoadPlacement)
	ON_BN_CLICKED(IDC_LOAD_HISTORY,OnHitLoadHistory)
	ON_BN_CLICKED(IDC_PREV_STEP,OnHitPrevStep)
	ON_BN_CLICKED(IDC_NEXT_STEP,OnHitNextStep)
	ON_BN_CLICKED(IDC_TURN_CHESS,OnHitTurn)
	ON_BN_CLICKED(IDC_LOSE,OnHitLose)
	ON_BN_CLICKED(IDC_GIVEUP,OnHitGiveUp)
	ON_BN_CLICKED(IDC_PEACE,OnHitPeace)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//设置变量
	m_bFrameX = 255 ;
	m_bFrameY = 255 ;


	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_lCurrentStepCount = 0 ;

	m_ChessBoard.SetParent(this) ;

	//获取大小

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
	CSize size = m_ChessBoard.GetChessBoradSize() ;
	m_ChessBoard.CleanChess() ;

	//m_ScoreView.Create(IDD_GAME_SCORE,this);
	//m_ScoreView.ShowWindow(SW_HIDE);

	//创建按钮

	CRect CreateRect(0,0,0,0);
	m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,CreateRect,this,IDD_GAME_SCORE);
	m_ScoreView.ShowWindow(SW_HIDE);
	m_btPlacement.Create("布局完成",WS_CHILD,CreateRect,this,IDC_PLACEMENT);
	m_btSavePlacement.Create("保存布局",WS_CHILD,CreateRect,this,IDC_SAVEPLACEMENT);
	m_btLoadPlacement.Create("载入布局",WS_CHILD,CreateRect,this,IDC_LOADPLACEMENT);
	m_btResetGame.Create("重新游戏",WS_CHILD,CreateRect,this,IDC_RESETGAME);
	m_btLoadHistory.Create(TEXT("装载棋局"),WS_CHILD|BS_OWNERDRAW,CreateRect,this,IDC_LOAD_HISTORY);
	m_btPrevStep.Create(TEXT("上一步棋"),WS_CHILD|BS_OWNERDRAW,CreateRect,this,IDC_PREV_STEP);	
	m_btNextStep.Create(TEXT("下一步棋"),WS_CHILD|BS_OWNERDRAW,CreateRect,this,IDC_NEXT_STEP);	
	m_btTurn.Create(TEXT("旋转棋盘"),WS_CHILD|BS_OWNERDRAW,CreateRect,this,IDC_TURN_CHESS);		
	m_btLose.Create(TEXT("本盘认输"),WS_CHILD|BS_OWNERDRAW,CRect(0,0,60,20),this,IDC_LOSE);
	m_btGiveUp.Create(TEXT("放弃一手"),WS_CHILD|BS_OWNERDRAW,CRect(0,0,60,20),this,IDC_GIVEUP);
	m_btPeace.Create(TEXT("请求和棋"),WS_CHILD|BS_OWNERDRAW,CRect(0,0,60,20),this,IDC_PEACE);



	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();

	m_btPlacement.SetButtonImage(IDB_BT_BUTTON,hInstance,false);
	m_btSavePlacement.SetButtonImage(IDB_BT_BUTTON,hInstance,false);
	m_btLoadPlacement.SetButtonImage(IDB_BT_BUTTON,hInstance,false);
	m_btResetGame.SetButtonImage(IDB_BT_BUTTON,hInstance,false);
	m_btLoadHistory.SetButtonImage(IDB_BT_BUTTON,hInstance,false);
	m_btPrevStep.SetButtonImage(IDB_BT_BUTTON,hInstance,false);
	m_btNextStep.SetButtonImage(IDB_BT_BUTTON,hInstance,false);
	m_btTurn.SetButtonImage(IDB_BT_BUTTON,hInstance,false);
	m_btLose.SetButtonImage(IDB_BT_BUTTON,hInstance,false);
	m_btGiveUp.SetButtonImage(IDB_BT_BUTTON,hInstance,false);
	m_btPeace.SetButtonImage(IDB_BT_BUTTON,hInstance,false);


	m_btPlacement.ShowWindow(SW_SHOW) ;
	m_btPlacement.EnableWindow(TRUE) ;
	m_btSavePlacement.ShowWindow(SW_SHOW) ;
	m_btSavePlacement.EnableWindow(TRUE) ;
	m_btLoadPlacement.ShowWindow(SW_SHOW) ;
	m_btLoadPlacement.EnableWindow(TRUE) ;

	m_btLoadHistory.ShowWindow(SW_SHOW) ;
	m_btLoadHistory.EnableWindow(TRUE) ;

	m_btPrevStep.ShowWindow(SW_SHOW) ;
	m_btPrevStep.EnableWindow(FALSE) ;
	m_btNextStep.ShowWindow(SW_SHOW) ;
	m_btNextStep.EnableWindow(FALSE) ;
	m_btTurn.ShowWindow(SW_SHOW) ;
	m_btTurn.EnableWindow(FALSE) ;

	m_bDeasilOrder = (AfxGetApp()->GetProfileInt(TEXT("GameOption") , TEXT("DeasilOrder") , FALSE)) ? true : false ;

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

//重置界面
void CGameClientView::ResetGameView()
{
	//棋盘控制
	m_ChessBoard.CleanChess();

	//控件控制
	if (m_PeaceRequest.GetSafeHwnd()) m_PeaceRequest.DestroyWindow();

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{

	RECT btRect;
	m_btPlacement.GetClientRect(&btRect);
	CSize boardSize = m_ChessBoard.GetChessBoradSize() ;

	//按钮位置
	m_btPlacement.MoveWindow(nWidth-btRect.right-(nWidth-boardSize.cx)/2-103,nHeight-3*btRect.bottom-80,btRect.right,btRect.bottom);
	m_btSavePlacement.MoveWindow(nWidth-btRect.right-(nWidth-boardSize.cx)/2-103,nHeight-2*btRect.bottom-80,btRect.right,btRect.bottom);
	m_btLoadPlacement.MoveWindow(nWidth-btRect.right-(nWidth-boardSize.cx)/2-103,nHeight-btRect.bottom-80,btRect.right,btRect.bottom);
	m_btResetGame.MoveWindow(nWidth-btRect.right-(nWidth-boardSize.cx)/2-103,nHeight-3*btRect.bottom-80,btRect.right,btRect.bottom);

	m_btLoadHistory.MoveWindow(nWidth-2*btRect.right-(nWidth-boardSize.cx)/2-105,nHeight-4*btRect.bottom-80,btRect.right,btRect.bottom);
	m_btPrevStep.MoveWindow(nWidth-2*btRect.right-(nWidth-boardSize.cx)/2-105,nHeight-3*btRect.bottom-80,btRect.right,btRect.bottom);
	m_btNextStep.MoveWindow(nWidth-2*btRect.right-(nWidth-boardSize.cx)/2-105,nHeight-2*btRect.bottom-80,btRect.right,btRect.bottom);
	m_btTurn.MoveWindow(nWidth-2*btRect.right-(nWidth-boardSize.cx)/2-105,nHeight-btRect.bottom-80,btRect.right,btRect.bottom);

	m_btLose.MoveWindow(nWidth-2*btRect.right-(nWidth-boardSize.cx)/2-105,nHeight-3*btRect.bottom-80,btRect.right,btRect.bottom);
	m_btGiveUp.MoveWindow(nWidth-2*btRect.right-(nWidth-boardSize.cx)/2-105,nHeight-2*btRect.bottom-80,btRect.right,btRect.bottom);
	m_btPeace.MoveWindow(nWidth-2*btRect.right-(nWidth-boardSize.cx)/2-105,nHeight-btRect.bottom-80,btRect.right,btRect.bottom);

			//移动控件
	HDWP hDwp=BeginDeferWindowPos(42);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//视频窗口
#ifdef VIDEO_GAME

	CRect rcAVDlg;
	m_DlgVedioService[0].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVedioService[1],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),nHeight/2+90,0,0,uFlags);
	DeferWindowPos(hDwp,m_DlgVedioService[3],NULL,m_nXBorder+5,nHeight/10+10,0,0,uFlags);
	DeferWindowPos(hDwp,m_DlgVedioService[0],NULL,nWidth-m_nXBorder-5-rcAVDlg.Width(),2,0,0,uFlags);
	m_DlgVedioService[2].GetWindowRect(&rcAVDlg);
	DeferWindowPos(hDwp,m_DlgVedioService[2],NULL,m_nXBorder+5,nHeight-m_nYBorder-3-rcAVDlg.Height(),0,0,uFlags);
#endif

	//移动控件
	EndDeferWindowPos(hDwp);

	//棋盘偏移
	m_ChessBoard.SetExcursionX((nWidth-boardSize.cx)/2) ;
	m_ChessBoard.SetExcursionY((nHeight-boardSize.cy)/2) ;


	m_ptFace[0].x=nWidth/2-NODE_WIDTH*4;
	m_ptFace[0].y=20;
	m_ptName[0].x=nWidth/2-NODE_WIDTH*4+40;
	m_ptName[0].y=20;
	m_ptTimer[0].x=nWidth/2-NODE_WIDTH*4-35;
	m_ptTimer[0].y=28;
	m_ptReady[0].x=nWidth/2-NODE_WIDTH*4-20;
	m_ptReady[0].y=35;

	m_ptFace[2].x=nWidth/2+NODE_WIDTH*3;
	m_ptFace[2].y=nHeight-60;
	m_ptName[2].x=nWidth/2+NODE_WIDTH*3+35;
	m_ptName[2].y=nHeight-60;
	m_ptTimer[2].x=nWidth/2+NODE_WIDTH*3;
	m_ptTimer[2].y=nHeight-85;
	m_ptReady[2].x=nWidth/2+NODE_WIDTH*3+13;
	m_ptReady[2].y=nHeight-75;

	//逆时针
	if(false==m_bDeasilOrder)
	{

		m_ptFace[1].x=20;
		m_ptFace[1].y=nHeight/2+3*NODE_HEIGTH+10;
		m_ptName[1].x=60;
		m_ptName[1].y=nHeight/2+3*NODE_HEIGTH+10;
		m_ptTimer[1].x=23;
		m_ptTimer[1].y=nHeight/2+3*NODE_HEIGTH-23+10;
		m_ptReady[1].x=37;
		m_ptReady[1].y=nHeight/2+3*NODE_HEIGTH-15+10;

		m_ptFace[3].x=nWidth-52;
		m_ptFace[3].y=nHeight/2-4*NODE_HEIGTH+10;
		m_ptName[3].x=nWidth-58;
		m_ptName[3].y=nHeight/2-4*NODE_HEIGTH+10;
		m_ptTimer[3].x=nWidth-50;
		m_ptTimer[3].y=nHeight/2-4*NODE_HEIGTH-22+10;
		m_ptReady[3].x=nWidth-37;
		m_ptReady[3].y=nHeight/2-4*NODE_HEIGTH-10+10;
	}
	//顺时针
	else
	{
		m_ptFace[3].x=20;
		m_ptFace[3].y=nHeight/2+3*NODE_HEIGTH+10;
		m_ptName[3].x=60;
		m_ptName[3].y=nHeight/2+3*NODE_HEIGTH+10;
		m_ptTimer[3].x=23;
		m_ptTimer[3].y=nHeight/2+3*NODE_HEIGTH-23+10;
		m_ptReady[3].x=37;
		m_ptReady[3].y=nHeight/2+3*NODE_HEIGTH-15+10;

		m_ptFace[1].x=nWidth-52;
		m_ptFace[1].y=nHeight/2-4*NODE_HEIGTH+10;
		m_ptName[1].x=nWidth-58;
		m_ptName[1].y=nHeight/2-4*NODE_HEIGTH+10;
		m_ptTimer[1].x=nWidth-50;
		m_ptTimer[1].y=nHeight/2-4*NODE_HEIGTH-22+10;
		m_ptReady[1].x=nWidth-37;
		m_ptReady[1].y=nHeight/2-4*NODE_HEIGTH-10+10;
	}

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageBack,enMode_Spread);

	//绘画棋盘
	m_ChessBoard.DrawBoard(pDC) ;

	//绘画边框
	if(255!=m_bFrameX && 255!=m_bFrameY)
	{
		LONG lXPos = m_ChessBoard.GetChessXPos(m_bFrameX) , 
			 lYPos = m_ChessBoard.GetChessYPos(m_bFrameY) ;

		pDC->Draw3dRect(lXPos-2+3,lYPos-2+3,CHESS_WIDTH+4,CHESS_HEIGTH+4,RGB(225,0,0),RGB(100,0,0));
		pDC->Draw3dRect(lXPos-1+3,lYPos-1+3,CHESS_WIDTH+2,CHESS_HEIGTH+2,RGB(225,0,0),RGB(100,0,0));
		pDC->Draw3dRect(lXPos+2+3,lYPos+2+3,CHESS_WIDTH-4,CHESS_HEIGTH-4,RGB(255,0,0),RGB(200,0,0));
	}

	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer=GetUserTimer(i);
		const tagUserData * pUserData=GetUserInfo(i);



		//////////////////////////////////////////////////////////////////////////
		//测试代码
/*
		tagUserData UserData ;
		memset(&UserData , 0 , sizeof(UserData)) ;
		UserData.wChairID = i ;
		UserData.wFaceID  = i ;
		UserData.cbUserStatus=US_READY ;
		CString str ;
		str.Format("%d" , i) ;
		CopyMemory(UserData.szName , str.GetBuffer(str.GetLength()) , str.GetLength()) ;
		pUserData = &UserData ;
		wUserTimer = i+1 ;
		m_bDeasilOrder = true ;
		RectifyGameView(nWidth, nHeight) ;

*/

		//////////////////////////////////////////////////////////////////////////
		//绘画用户
		if (pUserData!=NULL)
		{
			if(false==m_bDeasilOrder)
			{
				if(3==i) pDC->SetTextAlign(TA_RIGHT);
				else if(1==i) pDC->SetTextAlign(TA_LEFT);
				else pDC->SetTextAlign(TA_TOP|TA_LEFT);
			}
			else
			{
				if(1==i) pDC->SetTextAlign(TA_RIGHT);
				else if(3==i) pDC->SetTextAlign(TA_LEFT);
				else pDC->SetTextAlign(TA_TOP|TA_LEFT);
			}
			DrawTextString(pDC,pUserData->szName,RGB(240,240,240),RGB(50,50,50),m_ptName[i].x,m_ptName[i].y);

			//举手
			if (pUserData->cbUserStatus==US_READY)
			{
				DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			}
			//其他信息
						if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
		}
	}

	if(m_lCurrentStepCount>0)
	{
		CFont font;
		VERIFY(font.CreateFont(
			20,                        // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			ANSI_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			"Arial"));                 // lpszFacename
		COLORREF defColor = pDC->SetTextColor(RGB(255,255,255)) ;

		CFont* def_font = pDC->SelectObject(&font);
		CString strMsg ;
		strMsg.Format("当前步数为: %ld" , m_lCurrentStepCount) ;
		pDC->TextOut(m_nXBorder+5 , m_nYBorder+5, strMsg, strMsg.GetLength());
		pDC->SelectObject(def_font);
		pDC->SetTextColor(defColor) ;

		font.DeleteObject();
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



//////////////////////////////////////////////////////////////////////////

void CGameClientView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CGameFrameView::OnLButtonUp(nFlags, point);

	BYTE bXPos, bYPos ;

	m_ChessBoard.SwitchMousePos(point , bXPos , bYPos) ;

	CString str ;
	str.Format("(%d,%d)" , bXPos , bYPos) ;

//	AfxMessageBox(str) ;

	//点击棋盘
	if(255!=bXPos && 255!=bYPos)
		GetParent()->SendMessage(WM_HIT_CHESS_BORAD , bXPos , bYPos) ;

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

void CGameClientView::OnHitPlacement()
{
	GetParent()->SendMessage(IDM_PLACEMENT , 0 , 0) ;
}

void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
	BYTE bXPos, bYPos ;

	m_ChessBoard.SwitchMousePos(point , bXPos , bYPos) ;

	//点击棋盘
	if(255!=bXPos && 255!=bYPos)
		GetParent()->SendMessage(WM_CHANGE_CURSOR , bXPos , bYPos) ;


	CGameFrameView::OnMouseMove(nFlags, point);
}

//绘画边框
void CGameClientView::DrawChessFrame(BYTE bXPos , BYTE bYPos)
{
	m_bFrameX = bXPos ;
	m_bFrameY = bYPos ;

	UpdateGameView(NULL) ;
}

//得到边框
void CGameClientView::GetChessFrame(BYTE &bXPos , BYTE &bYPos)
{
	bXPos = m_bFrameX ;
	bYPos = m_bFrameY ;
}
//重新游戏
void CGameClientView::OnHitResetGame() 
{
	GetParent()->SendMessage(WM_RESET_GAME , 0 , 0) ;
}

//保存布局
void CGameClientView::OnSavePlacement()
{
	GetParent()->SendMessage(WM_SAVE_PLACEMENT , 0 , 0) ;
}

//装载布局
void CGameClientView::OnHitLoadPlacement() 
{
    GetParent()->SendMessage(WM_LOAD_PLACEMENT , 0 , 0) ;
}

//载入棋局
void CGameClientView::OnHitLoadHistory()
{
	GetParent()->SendMessage(IDM_LOAD_HISTORY,0,0);
}

//上一步棋			
void CGameClientView::OnHitPrevStep()
{
	GetParent()->SendMessage(IDM_PREV_STEP,0,0);
}

//下一步棋				
void CGameClientView::OnHitNextStep()
{
	GetParent()->SendMessage(IDM_NEXT_STEP,0,0);
}

//旋转棋盘				
void CGameClientView::OnHitTurn()
{
	GetParent()->SendMessage(IDM_TURN_CHESS,0,0);
}

//走棋顺序
void CGameClientView::SetUserOrder(bool bDeasilOrder)
{
	m_bDeasilOrder = bDeasilOrder ;


	//设置界面
	CRect rcClient;
	GetClientRect(&rcClient);

	RectifyGameView(rcClient.Width(),rcClient.Height());


	//更新界面
	UpdateGameView(NULL) ;
}

//走棋顺序
bool CGameClientView::IsDeasilOrder(bool bDeasilOrder)
{
	return bDeasilOrder ;
}

//认输按钮
void CGameClientView::OnHitLose()
{
	if(MessageBox("你是否确定要认输？","提示",MB_YESNO)==IDYES)
	{
		GetParent()->SendMessage(IDM_LOSE,0,0);
	}
}

//放弃按钮
void CGameClientView::OnHitGiveUp()
{
	if(MessageBox("你是否确定要放弃一手？","提示",MB_YESNO)==IDYES)
	{
		GetParent()->SendMessage(IDM_GIVEUP,0,0);
	}
}

//请求和棋
void CGameClientView::OnHitPeace()
{
	if(MessageBox("你是否确定要和棋？","提示",MB_YESNO)==IDYES)
	{
		GetParent()->SendMessage(IDM_PEACE,0,0);
	}
}

void CGameClientView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CGameFrameView::OnRButtonDown(nFlags, point);
	BYTE bXPos, bYPos ;

	m_ChessBoard.SwitchMousePos(point , bXPos , bYPos) ;

	CGameClientDlg *pParent = (CGameClientDlg *)GetParent() ;
	//非法判断
	if(CHESS_NULL == pParent->m_Chess[bXPos][bYPos].bChessID || CHESS_BACK!=pParent->m_Chess[bXPos][bYPos].bChessMode || CHESS_COLOR_NO==pParent->m_Chess[bXPos][bYPos].bColor)
        return ;

	//点击棋盘
	if(255!=bXPos && 255!=bYPos)
	{
		//标志图案
		m_ChessBoard.ShowFlagImage(point , true) ;
		m_ChessBoard.SetFlaChessPos(bXPos , bYPos) ;
	}

}

//设置步数 	
void CGameClientView::SetStepCount(LONG lStepCount)
{
	m_lCurrentStepCount = lStepCount ;
	//更新界面
	UpdateGameView(NULL) ;
}