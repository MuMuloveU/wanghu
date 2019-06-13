#include "Stdafx.h"
#include "GameClient.h"
#include "SaveChessDlg.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define IDI_GAME_TIMER				100									//游戏定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_PLACEMENT,OnHitPlacement)
	ON_MESSAGE(WM_HIT_CHESS_BORAD , OnHitChessBoard)
	ON_MESSAGE(WM_CHANGE_CURSOR , OnChangeCursor)
	ON_MESSAGE(WM_RESET_GAME , OnHitResetGame)
	ON_MESSAGE(WM_SAVE_PLACEMENT , OnHitSavePlacement)
	ON_MESSAGE(WM_LOAD_PLACEMENT , OnHitLoadPlacement)
	ON_MESSAGE(IDM_PREV_STEP,OnHitPrevStep)
	ON_MESSAGE(IDM_NEXT_STEP,OnHitNextStep)
	ON_MESSAGE(IDM_TURN_CHESS,OnHitTurn)
	ON_MESSAGE(IDM_LOAD_HISTORY,OnHitLoadHistory)
	ON_MESSAGE(IDM_LOSE,OnHitLose)
	ON_MESSAGE(IDM_GIVEUP,OnHitGiveUp)
	ON_MESSAGE(IDM_PEACE,OnHitPeace)
							
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//配置变量

	//游戏变量
	m_bChessColor=CHESS_COLOR_NO;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_Chess , sizeof(m_Chess)) ;
	m_bFinishPlacement=false ;
	m_bGameSart=false ;
	m_bPlayerCount=0 ;
	m_lCurrentStep=0 ; 
	m_bDeasilOrder = false ;
	m_lCurrentStepCount=0 ;

	//辅助变量
	m_bXSourcePos=255;
	m_bYSourcePos=255;
	m_bXTargetPos=255;
	m_bYTargetPos=255;
	m_bChooseChess=false ;

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
	//设置标题
	SetWindowText(TEXT("视频四国军棋"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置
	m_bDeasilOrder = (AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder") , false)) ? true : false ;
	m_GameClientView.SetUserOrder(m_bDeasilOrder) ;

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//辅助变量
	m_bXSourcePos=255;
	m_bYSourcePos=255;
	m_bXTargetPos=255;
	m_bYTargetPos=255;
	m_lCurrentStep=0 ; 

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;
	m_bChooseChess=false ;
	m_bGameSart=false ;
	m_bPlayerCount=0 ;
	
	for(BYTE x=0 ; x<17 ; ++x)
		for(BYTE y=0 ; y<17 ; ++y)
		{
			//自己棋子
			if(x>=6 && x<=10 && y>=11 && y<=16)
				continue ;
			m_Chess[x][y].bChessID   = CHESS_NULL ;
			m_Chess[x][y].bColor     = CHESS_COLOR_NO ;
			m_Chess[x][y].bChessMode = CHESS_NO ;
		}

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound = IsEnableSound();
	GameOption.m_bDeasilOrder = m_bDeasilOrder ;

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{

		//设置控件
		EnableSound(GameOption.m_bEnableSound);
		m_GameClientView.SetUserOrder(GameOption.m_bDeasilOrder);

		if(m_bDeasilOrder!=GameOption.m_bDeasilOrder)
		{
			m_bDeasilOrder = GameOption.m_bDeasilOrder ;

			//置换棋子
			tagChessItem tagChess ; 
			for(BYTE x=0 ; x<8 ; ++x)
				for(BYTE y=0 ; y<17 ; ++y)
				{
					tagChess		 = m_Chess[x][y] ;
					m_Chess[x][y]	 = m_Chess[16-x][y] ;
					m_Chess[16-x][y] = tagChess ;
				}

			//更新界面
			m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;
			m_GameClientView.SetUserOrder(m_bDeasilOrder) ;
			BYTE bXPos , bYPos ;
			m_GameClientView.GetChessFrame(bXPos , bYPos) ;
			m_GameClientView.DrawChessFrame(16-bXPos , bYPos) ;
			//绘画箭头
			if(m_GameLogic.m_ptPathArray.GetCount()>0)
			{
				for(int n=0 ; n<m_GameLogic.m_ptPathArray.GetCount() ; ++n)
				{
					m_GameLogic.m_ptPathArray[n].x = 16 - m_GameLogic.m_ptPathArray[n].x ;
				}
				m_GameClientView.m_ChessBoard.SetPath(&(m_GameLogic.m_ptPathArray)) ;
			}

			//标志图案
			m_GameClientView.m_ChessBoard.ReverseFlagImage() ;
		}

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("EnableSound"),GameOption.m_bEnableSound);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),GameOption.m_bDeasilOrder?TRUE:FALSE);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_GAME_TIMER:			//游戏时间
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) SendData(SUB_C_GAME_TIME_OUT , NULL , 0);
			
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	}
	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_FINISH_PLACEMENT://完成布局
		{
			return OnSubFinishiPlacement(pBuffer, wDataSize) ;
		}
	case SUB_S_MOVE_CHESS:		//移动棋子
		{
			return OnSubMoveChess(pBuffer, wDataSize) ;
		}
	case SUB_S_ONE_LOSE:		//玩家没棋
		{
			return OnSubLoseOne(pBuffer, wDataSize) ;
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}	
	case SUB_S_GAME_TIME_OUT:	//玩家超时
		{
			CMD_S_GameTimeOut *pGameTimeOut=(CMD_S_GameTimeOut *)pBuffer ;
			m_wCurrentUser = pGameTimeOut->wCurrentUser ;
			//设置定时器
			SetGameTimer(m_wCurrentUser , IDI_GAME_TIMER , 30) ;

			//设置按钮
			if(GetMeChairID()==m_wCurrentUser)
			{
				m_GameClientView.m_btLose.ShowWindow(SW_SHOW) ;
				m_GameClientView.m_btLose.EnableWindow(TRUE) ;
				m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW) ;
				m_GameClientView.m_btGiveUp.EnableWindow(TRUE) ;
				m_GameClientView.m_btPeace.ShowWindow(SW_SHOW) ;
				m_GameClientView.m_btPeace.EnableWindow(TRUE) ;
			}
			else
			{
				m_GameClientView.m_btLose.ShowWindow(SW_HIDE) ;
				m_GameClientView.m_btLose.EnableWindow(FALSE) ;
				m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE) ;
				m_GameClientView.m_btGiveUp.EnableWindow(FALSE) ;
				m_GameClientView.m_btPeace.ShowWindow(SW_HIDE) ;
				m_GameClientView.m_btPeace.EnableWindow(FALSE) ;
			}

			return true ;
		}
	case SUB_S_CHESS_MANUAL:	//游戏棋谱
		{
			return OnSubChessManual(pBuffer,wDataSize);
		}
	case SUB_S_PLACEMENT:		//初始布局
		{
			return OnSubPlacement(pBuffer, wDataSize) ;
		}
	case SUB_S_REQUEST_PEACE:	//请求和棋
		{
			CMD_S_RuquestPeace *pRuquestPeace = (CMD_S_RuquestPeace *) pBuffer ;
			const tagUserData *pUserData = GetUserData(pRuquestPeace->wRequestUser) ;
			CString strMsg = CString(pUserData->szName) + CString("请求和棋，你是否同意？") ;
			CMD_C_PeaceAnswer PeaceAnswer ;
			memset(&PeaceAnswer , 0 , sizeof(PeaceAnswer)) ;
			if(MessageBox(strMsg,"提示",MB_YESNO)==IDYES)
			{
				PeaceAnswer.bAgree = true ;				
			}
			else
				PeaceAnswer.bAgree = false ;
			SendData(SUB_C_PEACE_ANSWER , &PeaceAnswer , sizeof(PeaceAnswer)) ;

			return true ;
		}
	case SUB_S_PEACE_FAIL:	//和棋失败
		{
			MessageBox("和棋失败，请继续走棋！" , "提示" , MB_ICONASTERISK) ;

			return true ;
		}
	case SUB_S_USER_UP:
		{
			CMD_S_UserUp *pUserUp = (CMD_S_UserUp*)pBuffer ;

			//保存棋子
			BYTE bClientX = 255 , 
				bClientY = 255 ;
			for(BYTE x=0 ; x<5 ; ++x)
				for(BYTE y=0 ; y<6 ; ++y)
				{
					//实际坐标
					BYTE bCurrentColor = pUserUp->wUser+1 ;
					BYTE bRealClientX = 255 ,
						bRealClientY = 255 ;

					switch(bCurrentColor) 
					{
					case CHESS_COLOR_GREEN:	//绿色棋子
						bRealClientX = 10-x ;
						bRealClientY = 5-y ;
						break;

					case CHESS_COLOR_BLACK:	//黑色棋子
						bRealClientX = 11+y ;
						bRealClientY = 10-x ;
						break;

					case CHESS_COLOR_BLUE:	//蓝色棋子
						bRealClientX = 6+x ;
						bRealClientY = 11+y ;
						break;

					case CHESS_COLOR_RED:	//红色棋子
						bRealClientX = 5-y ;
						bRealClientY = 6+x ;
						break;

					default:
						AfxMessageBox("ClientToServer:错误棋子") ;
						bClientX = 255 ;
						bClientY = 255 ;
					}

					ServerToClient(m_bChessColor , bRealClientX , bRealClientY , bClientX , bClientY) ;

					if(false==m_bDeasilOrder)
					{
						bClientX = 16 - bClientX ;
						bClientY = bClientY ;
					}

					ASSERT(255!=bClientX && 255 !=bClientY) ;
					if(255==bClientX || 255==bClientY) return false ;

					m_Chess[bClientX][bClientY].bColor	   = CHESS_COLOR_NO ;
					m_Chess[bClientX][bClientY].bChessMode = CHESS_NO ;
					m_Chess[bClientX][bClientY].bChessID   = CHESS_NULL ;
				}

				//更新界面
				m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;

				return true ;
		}

	}

	return false;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//数据校验
	ASSERT(sizeof(CMD_S_GameStart)==wDataSize) ;
	if(sizeof(CMD_S_GameStart)!=wDataSize) return false ;

	CMD_S_GameStart *pGameStart = (CMD_S_GameStart*)pBuffer ;

	m_lCurrentStepCount=0 ;

	//游戏人数
	m_bPlayerCount=0 ;
	for(WORD wChairID = 0 ; wChairID <GAME_PLAYER ; ++wChairID)
	{
		const tagUserData *pUserData = GetUserData(wChairID) ;
		if(NULL!=pUserData) ++m_bPlayerCount ;
	}

	m_wCurrentUser = pGameStart->wCurrentUser ;
	m_bChooseChess = false ;
	//保存布局
	for(BYTE x=0 ; x<17 ; ++x)
		for(BYTE y=0 ; y<17 ; ++y)
		{
			//坐标转换
			BYTE bServerX = 255 ,
				 bServerY = 255 ;				

			ClientToServer(m_bChessColor , x , y , bServerX , bServerY) ;

			//顺时针走
			if(true==m_bDeasilOrder)
			{
				m_HistoryRecord.m_Chess[bServerX][bServerY].bChessID	= m_Chess[x][y].bChessID ;
				m_HistoryRecord.m_Chess[bServerX][bServerY].bColor		= m_Chess[x][y].bColor ;
				m_HistoryRecord.m_Chess[bServerX][bServerY].bChessMode  = m_Chess[x][y].bChessMode ;
			}
			else
			{
				m_HistoryRecord.m_Chess[bServerX][bServerY].bChessID	= m_Chess[16-x][y].bChessID ;
				m_HistoryRecord.m_Chess[bServerX][bServerY].bColor		= m_Chess[16-x][y].bColor ;
				m_HistoryRecord.m_Chess[bServerX][bServerY].bChessMode  = m_Chess[16-x][y].bChessMode ;
			}
		}

#ifdef _DEBUG
	TRACE("OnSubGameStart:\n") ;
	for(BYTE x=0 ; x<17 ; ++x)
		for(BYTE y=0 ; y<17 ; ++y)
			TRACE("(%d , %d):%s\n" , x , y , m_GameLogic.IDToName(m_HistoryRecord.m_Chess[x][y].bChessID)) ;
#endif

	m_HistoryRecord.m_Player = m_bPlayerCount ;
	m_lCurrentStep=0 ; 

	m_bGameSart=true ;

	//擦除边框
	m_GameClientView.DrawChessFrame(255 , 255) ;
	//擦除路线
	m_GameClientView.m_ChessBoard.SetPath(NULL) ;
	
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	SetGameTimer(m_wCurrentUser , IDI_GAME_TIMER , 30) ;

	//游戏状态
	SetGameStatus(GS_PLAYING) ;

	//设置按钮
	if(GetMeChairID()==m_wCurrentUser)
	{
		m_GameClientView.m_btLose.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btLose.EnableWindow(TRUE) ;
		m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btGiveUp.EnableWindow(TRUE) ;
		m_GameClientView.m_btPeace.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btPeace.EnableWindow(TRUE) ;
	}
	else
	{
		m_GameClientView.m_btLose.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_btLose.EnableWindow(FALSE) ;
		m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_btGiveUp.EnableWindow(FALSE) ;
		m_GameClientView.m_btPeace.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_btPeace.EnableWindow(FALSE) ;
	}

	m_lCurrentStepCount=0 ; 
	m_GameClientView.SetStepCount(0) ;

	return true;
}


//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_FREE:		//空闲状态

		m_bChessColor = GetMeChairID()+1 ;

		//已经布局
		if(true==m_bFinishPlacement)
		{
			OnHitPlacement(0,0) ;
			m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;
			return true ;
		}

		//初始棋盘
		InitCurrentUserChess() ;
		m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;

		return true ;
		
	case GS_PLAYING:		//游戏状态
		{
			//读取配置
			m_bDeasilOrder = (AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder") , false)) ? true : false ;
			m_GameClientView.SetUserOrder(m_bDeasilOrder) ;
			m_bChessColor = GetMeChairID() + 1 ;

			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
			m_wCurrentUser = pStatusPlay->wCurrentUser ;

			m_wCurrentUser = pStatusPlay->wCurrentUser ;

			BYTE bClientX , bClientY ;
			for(BYTE x=0 ; x<17 ; ++x)
				for(BYTE y=0 ; y<17 ; ++y)
				{
					ServerToClient(m_bChessColor , x , y , bClientX , bClientY) ;

					//逆时针走
					if(false==m_bDeasilOrder)
					{
						bClientX = 16 - bClientX ;
					}
					m_Chess[bClientX][bClientY].bChessID   = pStatusPlay->Chess[x][y].bChessID ;
					m_Chess[bClientX][bClientY].bColor	   = pStatusPlay->Chess[x][y].bColor ;

					if(CHESS_NULL != m_Chess[bClientX][bClientY].bChessID && CHESS_COLOR_NO != m_Chess[bClientX][bClientY].bColor)
						m_Chess[bClientX][bClientY].bChessMode = CHESS_FORCE ;
					else
						m_Chess[bClientX][bClientY].bChessMode = CHESS_NO ;

					if(m_Chess[bClientX][bClientY].bColor == m_bChessColor && CHESS_NULL!=m_Chess[bClientX][bClientY].bChessID)
					{
						if(false==IsLookonMode())
							m_Chess[bClientX][bClientY].bChessMode = CHESS_FORCE ;
						else
							m_Chess[bClientX][bClientY].bChessMode = CHESS_BACK ;
					}
					else if(m_Chess[bClientX][bClientY].bColor != m_bChessColor && CHESS_NULL!=m_Chess[bClientX][bClientY].bChessID)
						m_Chess[bClientX][bClientY].bChessMode = CHESS_BACK ;
					

				}
			//设置界面
			m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;
			m_lCurrentStepCount = pStatusPlay->lStepCount ;
			m_GameClientView.SetStepCount(m_lCurrentStepCount) ;
			
			SetGameTimer(m_wCurrentUser , IDI_GAME_TIMER , 30) ;

			m_bFinishPlacement = true ;					
			m_bGameSart = true ;						
			//游戏人数
			m_bPlayerCount=0 ;
			for(WORD wChairID = 0 ; wChairID <GAME_PLAYER ; ++wChairID)
			{
				const tagUserData *pUserData = GetUserData(wChairID) ;
				if(NULL!=pUserData) ++m_bPlayerCount ;
			}			

			//按钮设置
			m_GameClientView.m_btPlacement.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btPlacement.EnableWindow(FALSE) ;
			m_GameClientView.m_btSavePlacement.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btSavePlacement.EnableWindow(FALSE) ;
			m_GameClientView.m_btLoadPlacement.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btLoadPlacement.EnableWindow(FALSE) ;
			m_GameClientView.m_btResetGame.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btResetGame.EnableWindow(FALSE) ;

			m_GameClientView.m_btLoadHistory.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btLoadHistory.EnableWindow(FALSE) ;
			m_GameClientView.m_btPrevStep.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btPrevStep.EnableWindow(FALSE) ;
			m_GameClientView.m_btNextStep.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btNextStep.EnableWindow(FALSE) ;
			m_GameClientView.m_btTurn.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btTurn.EnableWindow(FALSE) ;

			return true;
		}
	}

	return false;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
}


//初始棋子
void CGameClientDlg::InitCurrentUserChess() 
{
   	m_bChessColor = GetMeChairID() + 1 ;

#ifdef _DEBUG
	CString strMsg ;
	strMsg.Format("User %ld , color %d" , GetMeChairID() , m_bChessColor) ;
//	InsertSystemString(strMsg) ;
#endif

	for(int x=6 ; x<11 ; x++)
	{
		for(int y=11 ; y<17 ; y++)
		{
			//军营棋子
			if(true==m_GameLogic.IsBarback(x , y))
			{
				m_Chess[x][y].bColor	 = CHESS_COLOR_NO ;
				m_Chess[x][y].bChessMode = CHESS_NO ;
				m_Chess[x][y].bChessID   = CHESS_NULL ;
			}
			else
			{
				m_Chess[x][y].bColor	=GetMeChairID()+1 ;
				m_Chess[x][y].bChessMode=CHESS_FORCE ;
			}
		}
	}

	m_Chess[6][11].bChessID	 = CHESS_LIAN_ZHANG;			
	m_Chess[7][11].bChessID	 = CHESS_SI_LING;			
	m_Chess[8][11].bChessID	 = CHESS_JUN_ZHANG;			
	m_Chess[9][11].bChessID	 = CHESS_GONG_BING;			
	m_Chess[10][11].bChessID = CHESS_SHI_ZHANG;			
	m_Chess[6][12].bChessID	 = CHESS_YING_ZHANG;			
	m_Chess[8][12].bChessID	 = CHESS_LV_ZHANG;			
	m_Chess[10][12].bChessID = CHESS_TUAN_ZHANG;		
	m_Chess[6][13].bChessID	 = CHESS_TUAN_ZHANG;			
	m_Chess[7][13].bChessID	 = CHESS_ZHA_DAN;			
	m_Chess[9][13].bChessID	 = CHESS_ZHA_DAN;			
	m_Chess[10][13].bChessID = CHESS_LIAN_ZHANG;		
	m_Chess[6][14].bChessID	 = CHESS_YING_ZHANG;			
	m_Chess[8][14].bChessID	 = CHESS_LIAN_ZHANG;			
	m_Chess[10][14].bChessID = CHESS_SHI_ZHANG;			
	m_Chess[6][15].bChessID	 = CHESS_PAI_ZHANG;			
	m_Chess[7][15].bChessID	 = CHESS_DI_LEI;				
	m_Chess[8][15].bChessID	 = CHESS_GONG_BING;			
	m_Chess[9][15].bChessID	 = CHESS_DI_LEI;				
	m_Chess[10][15].bChessID = CHESS_GONG_BING;			

	m_Chess[6][16].bChessID	 = CHESS_LV_ZHANG;			
	m_Chess[7][16].bChessID	 = CHESS_JUN_QI;				
	m_Chess[8][16].bChessID	 = CHESS_DI_LEI;				
	m_Chess[9][16].bChessID	 = CHESS_PAI_ZHANG;			
	m_Chess[10][16].bChessID = CHESS_PAI_ZHANG;		

	//旁观用户
	if(true==IsLookonMode())
	{
		for(int x=0;x<11;x++)
		{
			for(int y=11;y<17;y++)
			{	
				if(false==m_GameLogic.IsBarback(x , y))	
                    m_Chess[x][y].bChessMode=CHESS_BACK ;
			}
		}
	}
}

//布局完成
LRESULT	CGameClientDlg::OnHitPlacement(WPARAM wparam, LPARAM lparam)
{
	UNREFERENCED_PARAMETER(wparam) ;
	UNREFERENCED_PARAMETER(lparam) ;

	//发送布局
	CMD_C_FinishPlacement FinishPlacement ;
	ZeroMemory(&FinishPlacement , sizeof(FinishPlacement)) ;
	FinishPlacement.bColor = m_bChessColor ;

	//设置界面
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreView.ResetScore() ;

	m_lCurrentStepCount=0 ; 
	m_GameClientView.SetStepCount(0) ;

	//数据校验
	ASSERT(m_bChessColor==(GetMeChairID()+1)) ;
	if(m_bChessColor!=GetMeChairID()+1) return false ;

	for(BYTE x=6 ; x<11 ; ++x)
		for(BYTE y=11 ; y<17 ; ++y)
		{
			FinishPlacement.bChessID[x-6][y-11] = m_Chess[x][y].bChessID ;
		}

	//逆时针走
	if(false==m_bDeasilOrder)
	{
		BYTE bChess ;
		for(BYTE x=0 ; x<2 ; ++x)
			for(BYTE y=0 ; y<6 ; ++y)
			{
				bChess = FinishPlacement.bChessID[x][y] ;
				FinishPlacement.bChessID[x][y] = FinishPlacement.bChessID[4-x][y] ;
				FinishPlacement.bChessID[4-x][y] = bChess ;
			}
	}

	SendData(SUB_C_FINISH_PLACEMENT,&FinishPlacement,sizeof(FinishPlacement));
	SendUserReady(NULL,0);

	//设置变量
	m_bFinishPlacement=true ;

	//按钮设置
	m_GameClientView.m_btPlacement.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btPlacement.EnableWindow(FALSE) ;
	m_GameClientView.m_btSavePlacement.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btSavePlacement.EnableWindow(FALSE) ;
	m_GameClientView.m_btLoadPlacement.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btLoadPlacement.EnableWindow(FALSE) ;
	m_GameClientView.m_btResetGame.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btResetGame.EnableWindow(FALSE) ;

	m_GameClientView.m_btLoadHistory.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btLoadHistory.EnableWindow(FALSE) ;
	m_GameClientView.m_btPrevStep.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btPrevStep.EnableWindow(FALSE) ;
	m_GameClientView.m_btNextStep.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btNextStep.EnableWindow(FALSE) ;
	m_GameClientView.m_btTurn.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btTurn.EnableWindow(FALSE) ;

	return true ;	
}

//点击棋盘
LRESULT CGameClientDlg::OnHitChessBoard(WPARAM wParam , LPARAM lParam) 
{

	//旁观判断
	if(true==IsLookonMode()) return 0 ;


	BYTE bXPos = (BYTE)wParam ,
		 bYPos = (BYTE)lParam ;
	ASSERT(bXPos>=0 && bXPos<17 && bYPos>=0 && bYPos<17) ;
	if(bXPos<0 || bXPos>=17 || bYPos<0 || bYPos>=17) return false ;
	WORD wChairID = GetMeChairID() ;


	//布局棋子
	if(bXPos>=6 && bXPos<=10 && bYPos>=11 && bYPos<=16 && !m_GameLogic.IsBarback(bXPos , bYPos) && false==m_bFinishPlacement)
	{
		if(false==m_bChooseChess)
		{
			m_bXSourcePos=bXPos;
			m_bYSourcePos=bYPos;

			//绘画边框
			m_GameClientView.DrawChessFrame(m_bXSourcePos , m_bYSourcePos) ;
		}
		else
		{
			m_bXTargetPos=bXPos;
			m_bYTargetPos=bYPos;

			//擦除边框
			m_GameClientView.DrawChessFrame(255 , 255) ;
		}

		if(true==m_bChooseChess && m_Chess[m_bXSourcePos][m_bYSourcePos].bColor==m_bChessColor && m_Chess[m_bXTargetPos][m_bYTargetPos].bColor==m_bChessColor)
		{
			if(true==m_GameLogic.SwapChess(m_Chess , m_bXSourcePos , m_bYSourcePos , m_bXTargetPos , m_bYTargetPos , true))
				m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;
		}

		//更改变量
		m_bChooseChess=!m_bChooseChess ;

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("SELECT_CHESS"));

	}
	//走棋判断
	else if(true==m_bFinishPlacement && true==m_bGameSart && false==m_bChooseChess && GetMeChairID()==m_wCurrentUser && true==m_GameLogic.AbleMove(m_bChessColor , m_Chess , bXPos , bYPos))
	{

		m_bXSourcePos=bXPos;
		m_bYSourcePos=bYPos;

		//绘画边框
		m_GameClientView.DrawChessFrame(m_bXSourcePos , m_bYSourcePos) ;

		//更改变量
		m_bChooseChess=!m_bChooseChess ;

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("SELECT_CHESS"));

	}
	//更改选棋
	else if(true==m_bFinishPlacement && true==m_bGameSart && true==m_bChooseChess && GetMeChairID()==m_wCurrentUser && true==m_GameLogic.AbleMove(m_bChessColor , m_Chess , bXPos , bYPos))
	{
		m_bXSourcePos=bXPos;
		m_bYSourcePos=bYPos;

		//绘画边框
		m_GameClientView.DrawChessFrame(m_bXSourcePos , m_bYSourcePos) ;

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("SELECT_CHESS"));
	}
	else if(true==m_bFinishPlacement && true==m_bGameSart && true==m_bChooseChess && GetMeChairID()==m_wCurrentUser && true==m_GameLogic.MoveChess(m_bPlayerCount ,  m_Chess , m_bXSourcePos , m_bYSourcePos  , bXPos , bYPos , true))
	{
		if(CHESS_JUN_QI==m_Chess[bXPos][bYPos].bChessID)
			if(false==m_GameLogic.CanCarry(m_Chess[m_bXSourcePos][m_bYSourcePos].bChessID , m_bChessColor , m_Chess))
			{
				InsertShtickString("请用最小的棋子来扛旗！" , RGB(255,0,0) , true);
				return false ;
			}
		m_bXTargetPos=bXPos;
		m_bYTargetPos=bYPos;

		//绘画箭头
		m_GameClientView.m_ChessBoard.SetPath(&(m_GameLogic.m_ptPathArray)) ;


		//播放声音
		LONGLONG bCount = m_GameLogic.m_ptPathArray.GetCount() ;
		for (BYTE i = 0 ; i<bCount ; ++i)
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_MOVE"));
		}
		//绘画边框
		m_GameClientView.DrawChessFrame(m_bXTargetPos , m_bYTargetPos) ;

		//更改变量
		m_bChooseChess=!m_bChooseChess ;

		//发送数据
		CMD_C_MoveChess SendMoveChess ;
		memset(&SendMoveChess , 0 , sizeof(SendMoveChess)) ;
		SendMoveChess.bXSourcePos = m_bXSourcePos ;
		SendMoveChess.bYSourcePos = m_bYSourcePos ;
		SendMoveChess.bXTargetPos = m_bXTargetPos ;
		SendMoveChess.bYTargetPos = m_bYTargetPos ;

		//逆时针走
		if(false==m_bDeasilOrder)
		{			
				SendMoveChess.bXSourcePos = 16 - SendMoveChess.bXSourcePos ;
				SendMoveChess.bYSourcePos = SendMoveChess.bYSourcePos ;

				SendMoveChess.bXTargetPos = 16 - SendMoveChess.bXTargetPos ;
				SendMoveChess.bYTargetPos = SendMoveChess.bYTargetPos ;
		}

		SendData(SUB_C_MOVE_CHESS,&SendMoveChess,sizeof(SendMoveChess));
	}

	return true ;
}

//改变鼠标
LRESULT CGameClientDlg::OnChangeCursor(WPARAM wParam , LPARAM lParam) 
{
	//旁观判断
	if(true==IsLookonMode())
	{
		return 0 ;
	}

	BYTE bXPos = (BYTE)wParam ,
		 bYPos = (BYTE)lParam ;
	ASSERT(bXPos>=0 && bXPos<17 && bYPos>=0 && bYPos<17) ;
	if(bXPos<0 || bXPos>=17 || bYPos<0 || bYPos>=17) 
		return false ;

	//布局棋子
	if(bXPos>=6 && bXPos<=10 && bYPos>=11 && bYPos<=16 && !m_GameLogic.IsBarback(bXPos , bYPos) && false==m_bFinishPlacement)
	{
		HCURSOR hCursor;
		hCursor=AfxGetApp()->LoadCursor(IDC_HAND_CURSOR);
		SetCursor(hCursor);

	}
	//走棋判断
	else if(true==m_bFinishPlacement && true==m_bGameSart && false==m_bChooseChess && GetMeChairID()==m_wCurrentUser && true==m_GameLogic.AbleMove(m_bChessColor , m_Chess , bXPos , bYPos))
	{
		HCURSOR hCursor;
		hCursor=AfxGetApp()->LoadCursor(IDC_HAND_CURSOR);
		SetCursor(hCursor);

	}
	else if(true==m_bFinishPlacement && true==m_bGameSart && true==m_bChooseChess && GetMeChairID()==m_wCurrentUser && true==m_GameLogic.MoveChess(m_bPlayerCount ,  m_Chess , m_bXSourcePos , m_bYSourcePos  , bXPos , bYPos , false))
	{
		HCURSOR hCursor;
		hCursor=AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);
		SetCursor(hCursor);
	}
	else if(true==m_bChooseChess)
	{
		HCURSOR hCursor;
		hCursor=AfxGetApp()->LoadCursor(IDC_FORBID_MOVE_CURSOR);
		SetCursor(hCursor);
	}
	else
	{
		HCURSOR hCursor;
		hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW) ;
		SetCursor(hCursor);
	}

	return true ;
}

//完成布局
bool CGameClientDlg::OnSubFinishiPlacement(const void * pDataBuffer, WORD wDataSize) 
{
	//数据校验
	ASSERT(sizeof(CMD_S_FinishPlacement)==wDataSize) ;
	if(sizeof(CMD_S_FinishPlacement)!=wDataSize) return false ;

	CMD_S_FinishPlacement *pFinishPlacement = (CMD_S_FinishPlacement*)pDataBuffer ;


	//保存棋子
	BYTE bClientX = 255 , 
		 bClientY = 255 ;
	WORD wChairID = SwitchViewChairID(pFinishPlacement->wCurrentUser) ;
	for(BYTE x=0 ; x<5 ; ++x)
		for(BYTE y=0 ; y<6 ; ++y)
		{
			//实际坐标
			BYTE bCurrentColor = pFinishPlacement->wCurrentUser+1 ;
			BYTE bRealClientX = 255 ,
				 bRealClientY = 255 ;

			switch(bCurrentColor) {
			case CHESS_COLOR_GREEN:	//绿色棋子
				bRealClientX = 10-x ;
				bRealClientY = 5-y ;
				break;

			case CHESS_COLOR_BLACK:	//黑色棋子
				bRealClientX = 11+y ;
				bRealClientY = 10-x ;
				break;

			case CHESS_COLOR_BLUE:	//蓝色棋子
				bRealClientX = 6+x ;
				bRealClientY = 11+y ;
				break;

			case CHESS_COLOR_RED:	//红色棋子
				bRealClientX = 5-y ;
				bRealClientY = 6+x ;
				break;

			default:
				AfxMessageBox("ClientToServer:错误棋子") ;
				bClientX = 255 ;
				bClientY = 255 ;
			}

            ServerToClient(m_bChessColor , bRealClientX , bRealClientY , bClientX , bClientY) ;

			if(false==m_bDeasilOrder)
			{
				bClientX = 16 - bClientX ;
				bClientY = bClientY ;
			}
			
			ASSERT(255!=bClientX && 255 !=bClientY) ;
			if(255==bClientX || 255==bClientY) return false ;

			//军营棋子
			if(true==m_GameLogic.IsBarback(bClientX , bClientY))
			{
				m_Chess[bClientX][bClientY].bColor	   = CHESS_COLOR_NO ;
				m_Chess[bClientX][bClientY].bChessMode = CHESS_NO ;
				m_Chess[bClientX][bClientY].bChessID   = CHESS_NULL ;
			}
			else
			{
				m_Chess[bClientX][bClientY].bChessID = pFinishPlacement->bChessID[x][y] ;
				if(GetMeChairID()==pFinishPlacement->wCurrentUser)
					m_Chess[bClientX][bClientY].bChessMode = CHESS_FORCE ;
				else
                	m_Chess[bClientX][bClientY].bChessMode =  CHESS_BACK;// CHESS_FORCE ;
				m_Chess[bClientX][bClientY].bColor = pFinishPlacement->bColor ;
			}
		}

	//更新界面
	m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;

	return true ;
}


//移动棋子
bool CGameClientDlg::OnSubMoveChess(const void * pDataBuffer, WORD wDataSize)
{
	//数据校验
	ASSERT(sizeof(CMD_S_MoveChess)==wDataSize) ;
	if(sizeof(CMD_S_MoveChess)!=wDataSize) return false ;

	CMD_S_MoveChess *pMoveChess = (CMD_S_MoveChess*)pDataBuffer ;

	m_wCurrentUser = pMoveChess->wCurrentUser ;


	//坐标转换 
	BYTE bSourceX = 255 ,
		 bSourceY = 255 ,
		 bTargetX = 255 ,
		 bTargetY = 255 ;

	ServerToClient(m_bChessColor , pMoveChess->bXSourcePos , pMoveChess->bYSourcePos , bSourceX , bSourceY) ;
	ServerToClient(m_bChessColor , pMoveChess->bXTargetPos , pMoveChess->bYTargetPos , bTargetX , bTargetY) ;
	ASSERT(255!=bSourceX && 255!=bSourceY && 255!=bTargetX && 255!=bTargetY) ;

	if(false==m_bDeasilOrder)
	{
		bSourceX = 16 - bSourceX ;
		bSourceY = bSourceY ;
	
		bTargetX = 16 - bTargetX ;
		bTargetY = bTargetY ;
	
	}

	//比较棋子
	enCompareResult CompareResult = m_GameLogic.CompareChess(m_Chess , bSourceX , bSourceY , bTargetX , bTargetY) ;
	//绘画箭头
	if(enError!=CompareResult && GetMeChairID()!=pMoveChess->wMoveUser) 
	{
		m_GameLogic.MoveChess(m_bPlayerCount , m_Chess , bSourceX , bSourceY , bTargetX , bTargetY , true) ;
		m_GameClientView.m_ChessBoard.SetPath(&(m_GameLogic.m_ptPathArray)) ;

		//播放声音
		LONGLONG bCount = m_GameLogic.m_ptPathArray.GetCount() ;
		for(BYTE i = 0 ; i<bCount ; ++i)
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_MOVE"));
		}
		//绘画边框
		m_GameClientView.DrawChessFrame(bTargetX , bTargetY) ;
	}	

	switch(CompareResult)
	{
	case enSmall:
		//显示军棋
		if(CHESS_SI_LING == m_Chess[bSourceX][bSourceY].bChessID)
		{
            ShowJunQi(m_Chess[bSourceX][bSourceY].bColor) ;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_JUN_QI"));
		}

		m_Chess[bSourceX][bSourceY].bChessID   = CHESS_NULL ;
		m_Chess[bSourceX][bSourceY].bColor	   = CHESS_COLOR_NO ;
		m_Chess[bSourceX][bSourceY].bChessMode = CHESS_NO ;

		//设置标志
		m_GameClientView.m_ChessBoard.MoveFlagImage(bSourceX , bSourceY , 255 , 255 , false) ;

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("AO"));
		break;

	case enEqual:
		//播放声音
		if(CHESS_ZHA_DAN==m_Chess[bSourceX][bSourceY].bChessID || CHESS_ZHA_DAN==m_Chess[bTargetX][bTargetY].bChessID)
			PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB"));
		else
			PlayGameSound(AfxGetInstanceHandle(),TEXT("LOSE_ONE_CHESS"));	

		//显示军棋
		if(CHESS_SI_LING == m_Chess[bSourceX][bSourceY].bChessID)
		{
			ShowJunQi(m_Chess[bSourceX][bSourceY].bColor) ;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_JUN_QI"));
		}
		//显示军棋
		if(CHESS_SI_LING == m_Chess[bTargetX][bTargetY].bChessID)
		{
			ShowJunQi(m_Chess[bTargetX][bTargetY].bColor) ;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_JUN_QI"));
		}

	
		m_Chess[bSourceX][bSourceY].bChessID   = CHESS_NULL ;
		m_Chess[bSourceX][bSourceY].bColor     = CHESS_COLOR_NO ;
		m_Chess[bSourceX][bSourceY].bChessMode = CHESS_NO ;

		m_Chess[bTargetX][bTargetY].bChessID   = CHESS_NULL ;
		m_Chess[bTargetX][bTargetY].bColor     = CHESS_COLOR_NO ;
		m_Chess[bTargetX][bTargetY].bChessMode = CHESS_NO ;

		//设置标志
		m_GameClientView.m_ChessBoard.MoveFlagImage(bSourceX , bSourceY , 255 , 255 , false) ;
		m_GameClientView.m_ChessBoard.MoveFlagImage(bTargetX , bTargetY , 255 , 255 , false) ;
		
		break;

	case enLarge:
		//播放声音
		if(CHESS_NULL!=m_Chess[bTargetX][bTargetY].bChessID)
            PlayGameSound(AfxGetInstanceHandle(),TEXT("WIN_ONE_CHESS"));
		
		m_Chess[bTargetX][bTargetY].bChessID   = m_Chess[bSourceX][bSourceY].bChessID ; 
		m_Chess[bTargetX][bTargetY].bColor     = m_Chess[bSourceX][bSourceY].bColor ;   
		m_Chess[bTargetX][bTargetY].bChessMode = m_Chess[bSourceX][bSourceY].bChessMode;

		m_Chess[bSourceX][bSourceY].bChessID   = CHESS_NULL ;
		m_Chess[bSourceX][bSourceY].bColor	   = CHESS_COLOR_NO ;
		m_Chess[bSourceX][bSourceY].bChessMode = CHESS_NO ;

		//设置标志
		m_GameClientView.m_ChessBoard.MoveFlagImage(bSourceX , bSourceY , bTargetX , bTargetY , true) ;

		break;

	case enError:
		AfxMessageBox("OnSubMoveChess:错误棋子！") ;
		return false ;

	default:
		return false ;
	}

	//更新界面
	m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;

	//记录历史

	SaveChessStruct ChessStruct ;
	ChessStruct.bSourceX = pMoveChess->bXSourcePos ;
	ChessStruct.bSourceY = pMoveChess->bYSourcePos ;
	ChessStruct.bTargetX = pMoveChess->bXTargetPos ;
	ChessStruct.bTargetY = pMoveChess->bYTargetPos ;
	m_HistoryRecord.AddToList(ChessStruct) ;

	//设置定时器
	KillGameTimer(IDI_GAME_TIMER) ;
	SetGameTimer(pMoveChess->wCurrentUser , IDI_GAME_TIMER , 30) ;

	//递增步数
	++m_lCurrentStepCount ;
	//设置步数
	m_GameClientView.SetStepCount(m_lCurrentStepCount) ;

	//设置按钮
	if(GetMeChairID()==pMoveChess->wCurrentUser)
	{
		m_GameClientView.m_btLose.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btLose.EnableWindow(TRUE) ;
		m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btGiveUp.EnableWindow(TRUE) ;
		m_GameClientView.m_btPeace.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btPeace.EnableWindow(TRUE) ;
	}
	else
	{
		m_GameClientView.m_btLose.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_btLose.EnableWindow(FALSE) ;
		m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_btGiveUp.EnableWindow(FALSE) ;
		m_GameClientView.m_btPeace.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_btPeace.EnableWindow(FALSE) ;
	}


	return true ;
}

//玩家没棋
bool CGameClientDlg::OnSubLoseOne(const void * pDataBuffer, WORD wDataSize)
{
	//数据校验
	ASSERT(sizeof(CMD_S_OneLose)==wDataSize) ;
	if(sizeof(CMD_S_OneLose)!=wDataSize) return false ;
	
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("TWO_DIE"));

	CMD_S_OneLose *pOneLose = (CMD_S_OneLose*)pDataBuffer ;
	m_wCurrentUser = pOneLose->wCurrentUser ;
	WORD wLoseUser = pOneLose->wLoseUser ;
	BYTE bChessColor = wLoseUser+1 ;
	for(BYTE x=0 ; x<17 ; ++x)
		for(BYTE y=0 ; y<17 ; ++y)
			if(bChessColor==m_Chess[x][y].bColor)
			{
				m_Chess[x][y].bChessID	 = CHESS_NULL ;
				m_Chess[x][y].bChessMode = CHESS_NO ;
				m_Chess[x][y].bColor	 = CHESS_COLOR_NO ;
			}
	//更新界面
	m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;
	
	return true ;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pDataBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pDataBuffer;

	KillGameTimer(IDI_GAME_TIMER) ;

	//设置状态
	SetGameStatus(GS_FREE);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));

	//设置积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);

		if(NULL!=pUserData)
			m_GameClientView.m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lUserScore[i]);
		else
			m_GameClientView.m_ScoreView.SetGameScore(i,"",0);
	}

	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	m_GameClientView.m_ScoreView.CenterWindow(&m_GameClientView) ;

	//设置按钮

	if(false==IsLookonMode())
	{
		m_GameClientView.m_btResetGame.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btResetGame.EnableWindow(TRUE) ;

		m_GameClientView.m_btLoadHistory.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btLoadHistory.EnableWindow(TRUE) ;
		m_GameClientView.m_btPrevStep.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btPrevStep.EnableWindow(FALSE) ;
		m_GameClientView.m_btNextStep.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btNextStep.EnableWindow(FALSE) ;
		m_GameClientView.m_btTurn.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btTurn.EnableWindow(FALSE) ;
	}

	m_GameClientView.m_btLose.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btLose.EnableWindow(FALSE) ;
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE) ;
	m_GameClientView.m_btPeace.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btPeace.EnableWindow(FALSE) ;

	//设置变量
	ZeroMemory(m_Chess , sizeof(m_Chess)) ;

	//保存棋局
	ShowSave() ;

	return true ;
}

//重新游戏
LRESULT CGameClientDlg::OnHitResetGame(WPARAM wParam , LPARAM lParam) 
{
	//设置界面
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreView.ResetScore() ;
	//擦除边框
	m_GameClientView.DrawChessFrame(255 , 255) ;
	//擦除箭头
	m_GameClientView.m_ChessBoard.SetPath(NULL) ;

	//设置按钮
	m_GameClientView.m_btPlacement.ShowWindow(SW_SHOW) ;
	m_GameClientView.m_btPlacement.EnableWindow(TRUE) ;
	m_GameClientView.m_btSavePlacement.ShowWindow(SW_SHOW) ;
	m_GameClientView.m_btSavePlacement.EnableWindow(TRUE) ;
	m_GameClientView.m_btLoadPlacement.ShowWindow(SW_SHOW) ;
	m_GameClientView.m_btLoadPlacement.EnableWindow(TRUE) ;
	m_GameClientView.m_btResetGame.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btResetGame.EnableWindow(FALSE) ;

	m_GameClientView.m_btLoadHistory.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btLoadHistory.EnableWindow(FALSE) ;
	m_GameClientView.m_btPrevStep.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btPrevStep.EnableWindow(FALSE) ;
	m_GameClientView.m_btNextStep.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btNextStep.EnableWindow(FALSE) ;
	m_GameClientView.m_btTurn.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btTurn.EnableWindow(FALSE) ;

	m_GameClientView.m_btLose.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btLose.EnableWindow(FALSE) ;
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE) ;
	m_GameClientView.m_btPeace.ShowWindow(SW_HIDE) ;
	m_GameClientView.m_btPeace.EnableWindow(FALSE) ;

	//初始棋盘
	memset(m_Chess , 0 , sizeof(m_Chess)) ;
	InitCurrentUserChess() ;
	m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;

	//棋子颜色
	m_bChessColor = GetMeChairID() + 1 ;

	//设置变量
	m_bFinishPlacement = false ;
	m_bChooseChess = false ;
	m_bGameSart = false ;

	return true ;
}

//保存布局
LRESULT CGameClientDlg::OnHitSavePlacement(WPARAM wParam , LPARAM lParam) 
{
	UNREFERENCED_PARAMETER(wParam) ;
	UNREFERENCED_PARAMETER(lParam) ;

	CFileDialog FileDlg(false,"jqp",0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"军棋布局文件(*.jqp)|*.jqp||",this);
	if(FileDlg.DoModal()==IDOK)
	{
		CFile file;
		if(file.Open(FileDlg.GetPathName(),CFile::modeCreate|CFile::modeReadWrite|CFile::typeBinary))
		{
			BYTE bXPos ;
			for(BYTE i=6;i<11;i++)
			{
				for(BYTE j=11;j<17;j++)
				{
					//逆时针走
					if(false==m_bDeasilOrder)
						bXPos = 16 - i ;
					else
						bXPos = i ;

					file.Write(&(m_Chess[bXPos][j].bChessID),sizeof(BYTE));
				}
			}
			file.Close();
		}
	}
	return true ;
}

//装载布局	
LRESULT CGameClientDlg::OnHitLoadPlacement(WPARAM wParam , LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam) ;
	UNREFERENCED_PARAMETER(lParam) ;

	//设置界面
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE) ;


	CFileDialog FileDlg(true,"jqp","*.jqp",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"军棋布局文件(*.jqp)|*.jqp||",this);
	if(FileDlg.DoModal()==IDOK)
	{
		//预先初始 
		InitCurrentUserChess();

		CFile file;
		if(file.Open(FileDlg.GetPathName(),CFile::modeRead|CFile::typeBinary))
		{
			BYTE bXPos ;
			for(int i=6;i<11;i++)
			{
				for(int j=11;j<17;j++)
				{
					//逆时针走
					if(false==m_bDeasilOrder)
						bXPos = 16 - i ;
					else
						bXPos = i ;
					file.Read(&(m_Chess[bXPos][j].bChessID),sizeof(BYTE));
				}
			}
			file.Close();
			//合法校验
			BOOL HeFa=true;
			BYTE Chess40=0 , Chess39=0 , Chess38=0 , Chess37=0 , Chess36=0 , Chess35=0 , Chess34=0 , Chess33=0 , Chess32=0 , Chess31=0 , Chess30=0 , Chess29=0 ;
			for(int i=11;i<17;i++)
			{
				for(int j=6;j<11;j++)
				{
					if(i==11 && (m_Chess[j][i].bChessID==CHESS_ZHA_DAN || 
						m_Chess[j][i].bChessID==CHESS_DI_LEI))//第一排不能有炸弹，地雷
					{
						HeFa=false;
					}
					if(i!=16 && m_Chess[j][i].bChessID==CHESS_JUN_QI)//军旗只能放在最后一排
					{
						HeFa=false;
					}

					if(m_Chess[j][i].bChessID==CHESS_SI_LING	)	Chess40++;
					if(m_Chess[j][i].bChessID==CHESS_JUN_ZHANG	)	Chess39++;
					if(m_Chess[j][i].bChessID==CHESS_SHI_ZHANG	)	Chess38++;
					if(m_Chess[j][i].bChessID==CHESS_LV_ZHANG	)	Chess37++;
					if(m_Chess[j][i].bChessID==CHESS_TUAN_ZHANG)	Chess36++;
					if(m_Chess[j][i].bChessID==CHESS_YING_ZHANG)	Chess35++;	
					if(m_Chess[j][i].bChessID==CHESS_LIAN_ZHANG)	Chess34++;	
					if(m_Chess[j][i].bChessID==CHESS_PAI_ZHANG	)	Chess33++;
					if(m_Chess[j][i].bChessID==CHESS_GONG_BING	)	Chess32++;
					if(m_Chess[j][i].bChessID==CHESS_ZHA_DAN	)	Chess31++;
					if(m_Chess[j][i].bChessID==CHESS_DI_LEI	)		Chess30++;	
					if(m_Chess[j][i].bChessID==CHESS_JUN_QI	)		Chess29++;	
				}									   
			}	

			if(	m_Chess[7][12].bChessID!=0 || m_Chess[7][12].bChessID!=0 ||
				m_Chess[9][12].bChessID!=0 || m_Chess[8][13].bChessID!=0 || 
				m_Chess[7][14].bChessID!=0 || m_Chess[9][14].bChessID!=0)	//军营里不能放棋子
			{
				HeFa=false;
			}
			if(m_Chess[7][16].bChessID!=CHESS_JUN_QI && m_Chess[7][16].bChessID!=CHESS_JUN_QI)//军棋不在大本营里
			{
				HeFa=false;
			}
			if(!(Chess40==1 && Chess39==1 && Chess38==2 && Chess37==2 && 
				Chess36==2 && Chess35==2 && Chess34==3 && Chess33==3 && 
				Chess32==3 && Chess31==2 && Chess30==3 && Chess29==1))
			{
				HeFa=false;
			}
			//恢复布局
			if(!HeFa)
			{
				InitCurrentUserChess();
				AfxMessageBox("载入布局失败!!!");
			}
		}

		m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;
	}
	return 0;
}

//显示军棋
void CGameClientDlg::ShowJunQi(BYTE bChessColor) 
{
	if(m_Chess[7][0].bColor==bChessColor && CHESS_JUN_QI==m_Chess[7][0].bChessID)
		m_Chess[7][0].bChessMode = CHESS_FORCE ;

	else if(m_Chess[7][16].bColor==bChessColor && CHESS_JUN_QI==m_Chess[7][16].bChessID)
		m_Chess[7][16].bChessMode = CHESS_FORCE ;

	else if(m_Chess[9][0].bColor==bChessColor && CHESS_JUN_QI==m_Chess[9][0].bChessID)
		m_Chess[9][0].bChessMode = CHESS_FORCE ;

	else if(m_Chess[9][16].bColor==bChessColor && CHESS_JUN_QI==m_Chess[9][16].bChessID)
		m_Chess[9][16].bChessMode = CHESS_FORCE ;

	else if(m_Chess[0][7].bColor==bChessColor && CHESS_JUN_QI==m_Chess[0][7].bChessID)
		m_Chess[0][7].bChessMode = CHESS_FORCE ;

	else if(m_Chess[0][9].bColor==bChessColor && CHESS_JUN_QI==m_Chess[0][9].bChessID)
		m_Chess[0][9].bChessMode = CHESS_FORCE ;

	else if(m_Chess[16][7].bColor==bChessColor && CHESS_JUN_QI==m_Chess[16][7].bChessID)
		m_Chess[16][7].bChessMode = CHESS_FORCE ;

	else if(m_Chess[16][9].bColor==bChessColor && CHESS_JUN_QI==m_Chess[16][9].bChessID)
		m_Chess[16][9].bChessMode = CHESS_FORCE ;

	else 
		AfxMessageBox("ShowJunQi:错误棋子");
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

注意：m_Chess和SaveChessStruct里的坐标，保存时是按照服务器坐标保存的，
	 

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

//载入棋局
LRESULT CGameClientDlg::OnHitLoadHistory(WPARAM wParam , LPARAM lParam)
{
	//设置界面
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE) ;
	//擦除边框
	m_GameClientView.DrawChessFrame(255 , 255) ;
	m_GameClientView.m_ChessBoard.SetPath(NULL) ;

	//初始记录
	m_HistoryRecord.Init() ;
	
	CFileDialog FileDlg(true,"junqi","*.junqi",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"军棋存盘文件(*.junqi)|*.junqi||",this);
	if(FileDlg.DoModal()==IDOK)
	{
		CFile file;

		//坐标转换 
		BYTE bClientX = 255 ,
			 bClientY = 255 ;

		if(file.Open(FileDlg.GetPathName(),CFile::modeRead|CFile::typeBinary))
		{
			LONGLONG lOffset=strlen("军棋存盘文件");
			file.Seek(lOffset,CFile::begin);

			file.Read(&m_HistoryRecord.m_Player , sizeof(m_HistoryRecord.m_Player)) ;

			file.Read(&m_HistoryRecord.m_Chess,sizeof(m_HistoryRecord.m_Chess));

			for(BYTE x=0 ; x<17 ; x++)
			{
				for(BYTE y=0 ; y<17 ; y++)
				{
					BYTE bTmpX = x ,
						 bTmpY = y ;
					
					//坐标转换 
					ServerToClient(m_bChessColor , bTmpX , bTmpY , bClientX , bClientY) ;
			
					ASSERT(255!=bClientX && 255!=bClientY) ;

					//逆时针走
					if(false==m_bDeasilOrder)
					{
						bClientX = 16 - bClientX ;
					}

					m_Chess[bClientX][bClientY].bChessID   = m_HistoryRecord.m_Chess[x][y].bChessID ;
					m_Chess[bClientX][bClientY].bColor     = m_HistoryRecord.m_Chess[x][y].bColor ;
					if(CHESS_NULL==m_Chess[bClientX][bClientY].bChessID)
						m_Chess[bClientX][bClientY].bChessMode = CHESS_NO ;
					else
                        m_Chess[bClientX][bClientY].bChessMode = CHESS_FORCE ;//m_HistoryRecord.m_Chess[x][y].bChessMode ;
				}
			}

			SaveChessStruct SaveChess;
			int num=0;
			num=file.Read(&SaveChess,sizeof(SaveChess));
			while(num!=0)
			{
				m_HistoryRecord.AddToList(SaveChess);
				num=file.Read(&SaveChess,sizeof(SaveChess));
			}

			file.Close();

			//按钮设置
			m_GameClientView.m_btPlacement.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btPlacement.EnableWindow(FALSE) ;
			m_GameClientView.m_btSavePlacement.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btSavePlacement.EnableWindow(FALSE) ; 
			m_GameClientView.m_btLoadPlacement.ShowWindow(SW_HIDE) ;
			m_GameClientView.m_btLoadPlacement.EnableWindow(FALSE) ;
			m_GameClientView.m_btResetGame.ShowWindow(SW_SHOW) ;
			m_GameClientView.m_btResetGame.EnableWindow(TRUE) ;

			//初始变量
			m_lCurrentStep=0 ; 
			//更新界面
			m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;

		}
		//设置按钮
		m_GameClientView.m_btPrevStep.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btPrevStep.EnableWindow(TRUE) ;
		m_GameClientView.m_btNextStep.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btNextStep.EnableWindow(TRUE) ;
		m_GameClientView.m_btTurn.ShowWindow(SW_SHOW) ;
		m_GameClientView.m_btTurn.EnableWindow(TRUE) ;
	}

	return 0 ;
}
//上一步棋
LRESULT CGameClientDlg::OnHitPrevStep(WPARAM wParam , LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam) ;
	UNREFERENCED_PARAMETER(lParam) ;

	--m_lCurrentStep ;
	if(m_lCurrentStep<0)
	{
		m_lCurrentStep = 0 ;
		MessageBox("已经到尽头了！" , "四国军棋"  , MB_OK|MB_ICONASTERISK | MB_ICONINFORMATION) ;
		return 0 ;
	}
	
	//重置变量
	CopyMemory(m_Chess , m_HistoryRecord.m_Chess , sizeof(m_Chess)) ;
	SaveChessStruct SaveChess;

	//坐标转换 
	BYTE bClientX = 255 ,
		bClientY = 255 ;
	for(int x=0 ; x<17 ; x++)
	{
		for(int y=0 ; y<17 ; y++)
		{
			//坐标转换 
			ServerToClient(m_bChessColor , x , y , bClientX , bClientY) ;

			ASSERT(255!=bClientX && 255!=bClientY) ;

			//逆时针走
			if(false==m_bDeasilOrder)
			{
				bClientX = 16 - bClientX ;
				bClientY = bClientY ;
			}
			m_Chess[bClientX][bClientY].bChessID   = m_HistoryRecord.m_Chess[x][y].bChessID ;
			m_Chess[bClientX][bClientY].bColor     = m_HistoryRecord.m_Chess[x][y].bColor ;
			if(CHESS_NULL==m_Chess[bClientX][bClientY].bChessID)
				m_Chess[bClientX][bClientY].bChessMode = CHESS_NO ;
			else
				m_Chess[bClientX][bClientY].bChessMode = CHESS_FORCE ;//m_HistoryRecord.m_Chess[x][y].bChessMode ;
		}
	}

	for(LONG lStep=0 ; lStep< m_lCurrentStep ; ++lStep)
	{
		
		if(0==lStep)
		{
			if(false==m_HistoryRecord.GetFirstFromList(SaveChess))
			{
				MessageBox("已经到尽头了！" , "四国军棋"  , MB_OK|MB_ICONASTERISK | MB_ICONINFORMATION) ;
				return 0 ;
			}
		}
		else if(false==m_HistoryRecord.GetNextFromList(SaveChess))
		{
			MessageBox("已经到尽头了！" , "四国军棋"  , MB_OK|MB_ICONASTERISK | MB_ICONINFORMATION) ;
			return 0 ;
		}

		BYTE  bSourceX = 255 ,
			 bSourceY = 255 ,
			 bTargetX = 255 ,
			 bTargetY = 255 ;

		ServerToClient(m_bChessColor , SaveChess.bSourceX , SaveChess.bSourceY , bSourceX , bSourceY) ;
		ServerToClient(m_bChessColor , SaveChess.bTargetX , SaveChess.bTargetY , bTargetX , bTargetY) ;

		//逆时针走
		if(false==m_bDeasilOrder)
		{
			bSourceX = 16 - bSourceX ;
			bTargetX = 16 - bTargetX ;
		}

		//比较棋子
		enCompareResult CompareResult = m_GameLogic.CompareChess(m_Chess , bSourceX , bSourceY , bTargetX , bTargetY) ;
		//绘画箭头
		if(enError!=CompareResult) 
		{
			m_GameLogic.MoveChess(m_bPlayerCount , m_Chess , bSourceX , bSourceY , bTargetX , bTargetY , true) ;
			m_GameClientView.m_ChessBoard.SetPath(&(m_GameLogic.m_ptPathArray)) ;

			//播放声音
			LONGLONG bCount = m_GameLogic.m_ptPathArray.GetCount() ;
			for(BYTE i = 0 ; i<bCount ; ++i)
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_MOVE"));
			}
			//绘画边框
			m_GameClientView.DrawChessFrame(bTargetX , bTargetY) ;
		}	

		switch(CompareResult)
		{
		case enSmall:
			//显示军棋
			if(CHESS_SI_LING == m_Chess[bSourceX][bSourceY].bChessID)
			{
				ShowJunQi(m_Chess[bSourceX][bSourceY].bColor) ;
				PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_JUN_QI"));
			}

			m_Chess[bSourceX][bSourceY].bChessID   = CHESS_NULL ;
			m_Chess[bSourceX][bSourceY].bColor	   = CHESS_COLOR_NO ;
			m_Chess[bSourceX][bSourceY].bChessMode = CHESS_NO ;

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("AO"));
			break;

		case enEqual:
			//播放声音
			if(CHESS_ZHA_DAN==m_Chess[bSourceX][bSourceY].bChessID || CHESS_ZHA_DAN==m_Chess[bTargetX][bTargetY].bChessID)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB"));
			else
				PlayGameSound(AfxGetInstanceHandle(),TEXT("LOSE_ONE_CHESS"));	

			//显示军棋
			if(CHESS_SI_LING == m_Chess[bSourceX][bSourceY].bChessID)
			{
				ShowJunQi(m_Chess[bSourceX][bSourceY].bColor) ;
				PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_JUN_QI"));
			}
			//显示军棋
			if(CHESS_SI_LING == m_Chess[bTargetX][bTargetY].bChessID)
			{
				ShowJunQi(m_Chess[bTargetX][bTargetY].bColor) ;
				PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_JUN_QI"));
			}


			m_Chess[bSourceX][bSourceY].bChessID   = CHESS_NULL ;
			m_Chess[bSourceX][bSourceY].bColor     = CHESS_COLOR_NO ;
			m_Chess[bSourceX][bSourceY].bChessMode = CHESS_NO ;

			m_Chess[bTargetX][bTargetY].bChessID   = CHESS_NULL ;
			m_Chess[bTargetX][bTargetY].bColor     = CHESS_COLOR_NO ;
			m_Chess[bTargetX][bTargetY].bChessMode = CHESS_NO ;

			break;

		case enLarge:
			//播放声音
			if(CHESS_NULL!=m_Chess[bTargetX][bTargetY].bChessID)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("WIN_ONE_CHESS"));

			m_Chess[bTargetX][bTargetY].bChessID   = m_Chess[bSourceX][bSourceY].bChessID ; 
			m_Chess[bTargetX][bTargetY].bColor     = m_Chess[bSourceX][bSourceY].bColor ;   
			m_Chess[bTargetX][bTargetY].bChessMode = m_Chess[bSourceX][bSourceY].bChessMode;

			m_Chess[bSourceX][bSourceY].bChessID   = CHESS_NULL ;
			m_Chess[bSourceX][bSourceY].bColor	   = CHESS_COLOR_NO ;
			m_Chess[bSourceX][bSourceY].bChessMode = CHESS_NO ;

			break;

		case enError:
			AfxMessageBox("OnHitPrevStep:错误棋子！") ;
			return false ;

		default:
			return false ;
		}		
	}
	//更新界面
	m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;



	return 0 ;
}
//下一步棋
LRESULT CGameClientDlg::OnHitNextStep(WPARAM wParam , LPARAM lParam) 
{
	UNREFERENCED_PARAMETER(wParam) ;
	UNREFERENCED_PARAMETER(lParam) ;

	m_lCurrentStep++ ;
	SaveChessStruct SaveChess;
	if(1==m_lCurrentStep)
	{
		if(false==m_HistoryRecord.GetFirstFromList(SaveChess))
		{
			MessageBox("已经到尽头了！" , "四国军棋"  , MB_OK|MB_ICONASTERISK | MB_ICONINFORMATION) ;
			m_lCurrentStep-- ;
			return 0 ;
		}
	}
	else if(false==m_HistoryRecord.GetNextFromList(SaveChess))
	{
		MessageBox("已经到尽头了！" , "四国军棋"  , MB_OK|MB_ICONASTERISK | MB_ICONINFORMATION) ;
		m_lCurrentStep-- ;
		return 0 ;
	}

	BYTE  bSourceX = 255 ,
		  bSourceY = 255 ,
		  bTargetX = 255 ,
		  bTargetY = 255 ;

	ServerToClient(m_bChessColor , SaveChess.bSourceX , SaveChess.bSourceY , bSourceX , bSourceY) ;
	ServerToClient(m_bChessColor , SaveChess.bTargetX , SaveChess.bTargetY , bTargetX , bTargetY) ;

	//逆时针走
	if(false==m_bDeasilOrder)
	{
		bSourceX = 16 - bSourceX ;
		bTargetX = 16 - bTargetX ;
	}

	//棋子颜色
	BYTE bSourceColor = m_Chess[bSourceX][bSourceY].bColor ,
		bTargetColor = m_Chess[bTargetX][bTargetY].bColor ;

	//比较棋子
	enCompareResult CompareResult = m_GameLogic.CompareChess(m_Chess , bSourceX , bSourceY , bTargetX , bTargetY) ;

	bool bActive[GAME_PLAYER] ;
	WORD wLoseUser = INVALID_CHAIR ;
	for(WORD wID = 0 ; wID < GAME_PLAYER ; ++wID)
		bActive[wID] = true ;

	if(enError!=CompareResult && CHESS_JUN_QI==m_Chess[bTargetX][bTargetY].bChessID && 
		true==m_GameLogic.CanCarry(m_Chess[bSourceX][bSourceY].bChessID , m_Chess[bSourceX][bSourceY].bColor , m_Chess))
	{
		wLoseUser = bTargetColor - 1 ;
		bActive[wLoseUser] = false ;
	}


	//绘画箭头
	if(enError!=CompareResult) 
	{
		m_GameLogic.MoveChess(m_bPlayerCount , m_Chess , bSourceX , bSourceY , bTargetX , bTargetY , true) ;
		m_GameClientView.m_ChessBoard.SetPath(&(m_GameLogic.m_ptPathArray)) ;

		//播放声音
		LONGLONG bCount = m_GameLogic.m_ptPathArray.GetCount() ;
		for(BYTE i = 0 ; i<bCount ; ++i)
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_MOVE"));
		}
		//绘画边框
		m_GameClientView.DrawChessFrame(bTargetX , bTargetY) ;
	}	

	switch(CompareResult)
	{
	case enSmall:
		//显示军棋
		if(CHESS_SI_LING == m_Chess[bSourceX][bSourceY].bChessID)
		{
			ShowJunQi(m_Chess[bSourceX][bSourceY].bColor) ;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_JUN_QI"));
		}

		m_Chess[bSourceX][bSourceY].bChessID   = CHESS_NULL ;
		m_Chess[bSourceX][bSourceY].bColor	   = CHESS_COLOR_NO ;
		m_Chess[bSourceX][bSourceY].bChessMode = CHESS_NO ;

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("AO"));
		break;

	case enEqual:
		//播放声音
		if(CHESS_ZHA_DAN==m_Chess[bSourceX][bSourceY].bChessID || CHESS_ZHA_DAN==m_Chess[bTargetX][bTargetY].bChessID)
			PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB"));
		else
			PlayGameSound(AfxGetInstanceHandle(),TEXT("LOSE_ONE_CHESS"));	

		//显示军棋
		if(CHESS_SI_LING == m_Chess[bSourceX][bSourceY].bChessID)
		{
			ShowJunQi(m_Chess[bSourceX][bSourceY].bColor) ;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_JUN_QI"));
		}
		//显示军棋
		if(CHESS_SI_LING == m_Chess[bTargetX][bTargetY].bChessID)
		{
			ShowJunQi(m_Chess[bTargetX][bTargetY].bColor) ;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_JUN_QI"));
		}


		m_Chess[bSourceX][bSourceY].bChessID   = CHESS_NULL ;
		m_Chess[bSourceX][bSourceY].bColor     = CHESS_COLOR_NO ;
		m_Chess[bSourceX][bSourceY].bChessMode = CHESS_NO ;

		m_Chess[bTargetX][bTargetY].bChessID   = CHESS_NULL ;
		m_Chess[bTargetX][bTargetY].bColor     = CHESS_COLOR_NO ;
		m_Chess[bTargetX][bTargetY].bChessMode = CHESS_NO ;

		break;

	case enLarge:
		//播放声音
		if(CHESS_NULL!=m_Chess[bTargetX][bTargetY].bChessID)
			PlayGameSound(AfxGetInstanceHandle(),TEXT("WIN_ONE_CHESS"));

		m_Chess[bTargetX][bTargetY].bChessID   = m_Chess[bSourceX][bSourceY].bChessID ; 
		m_Chess[bTargetX][bTargetY].bColor     = m_Chess[bSourceX][bSourceY].bColor ;   
		m_Chess[bTargetX][bTargetY].bChessMode = m_Chess[bSourceX][bSourceY].bChessMode;

		m_Chess[bSourceX][bSourceY].bChessID   = CHESS_NULL ;
		m_Chess[bSourceX][bSourceY].bColor	   = CHESS_COLOR_NO ;
		m_Chess[bSourceX][bSourceY].bChessMode = CHESS_NO ;

		break;

	case enError:
		AfxMessageBox("OnHitNextStep:错误棋子！") ;
		return false ;

	default:
		return false ;
	}

	//////////////////////////////////////////////////////////////////////////

	//判断死亡
	if(true==m_GameLogic.NoChess(bSourceColor , m_Chess))
	{		
		wLoseUser = bSourceColor-1 ;
		bActive[wLoseUser] = false ;
	}
	else if(true==m_GameLogic.NoChess(bTargetColor , m_Chess))
	{
		if(CHESS_COLOR_NO!=bTargetColor)
		{
			wLoseUser = bTargetColor-1 ;
			bActive[wLoseUser] = false ;
		}
	}

	//玩家没棋
	if(INVALID_CHAIR!=wLoseUser && false==bActive[wLoseUser])
	{
		BYTE bChessColor = wLoseUser+1 ;
		for(BYTE x=0 ; x<17 ; ++x)
			for(BYTE y=0 ; y<17 ; ++y)
				if(bChessColor==m_Chess[x][y].bColor)
				{
					m_Chess[x][y].bChessID	 = CHESS_NULL ;
					m_Chess[x][y].bChessMode = CHESS_NO ;
					m_Chess[x][y].bColor	 = CHESS_COLOR_NO ;
				}
	}


	//////////////////////////////////////////////////////////////////////////
	

	//更新界面
	m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;

	return 0 ;
}
//旋转棋盘				
LRESULT CGameClientDlg::OnHitTurn(WPARAM wParam , LPARAM lParam) 
{
	UNREFERENCED_PARAMETER(wParam) ;
	UNREFERENCED_PARAMETER(lParam) ;

	if(0==m_HistoryRecord.GetStepCount())
	{
		MessageBox("请先装载正确的棋谱！" , "四国军棋"  , MB_OK|MB_ICONASTERISK | MB_ICONINFORMATION) ;
		return 0 ;
	}

	//转换颜色
	m_bChessColor = (m_bChessColor+1)%GAME_PLAYER ;
	if(CHESS_COLOR_NO==m_bChessColor)
		m_bChessColor = CHESS_COLOR_RED ;

	m_lCurrentStep = 0 ;

	//坐标转换 
	BYTE bClientX = 255 ,
		bClientY = 255 ;
	for(int x=0 ; x<17 ; x++)
	{
		for(int y=0 ; y<17 ; y++)
		{
			//坐标转换 
			ServerToClient(m_bChessColor , x , y , bClientX , bClientY) ;

			//逆时针走
			if(false==m_bDeasilOrder)
			{
				bClientX = 16 - bClientX ;
			}

			ASSERT(255!=bClientX && 255!=bClientY) ;

			m_Chess[bClientX][bClientY].bChessID   = m_HistoryRecord.m_Chess[x][y].bChessID ;
			m_Chess[bClientX][bClientY].bColor     = m_HistoryRecord.m_Chess[x][y].bColor ;
			m_Chess[bClientX][bClientY].bChessMode = CHESS_FORCE ;//m_HistoryRecord.m_Chess[x][y].bChessMode ;
		}
	}

	//设置界面
	m_GameClientView.m_ChessBoard.SetChess(m_Chess) ;
	return 0 ;
}

//保存棋局
void CGameClientDlg::ShowSave() 
{
	CSaveChessDlg dlg(this);
	if(dlg.DoModal()==IDOK)
	{
		CFileDialog FileDlg(false,"junqi",0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"军棋存盘文件(*.junqi)|*.junqi||",this);
		if(FileDlg.DoModal()==IDOK)
		{
			CFile file;
			if(file.Open(FileDlg.GetPathName(),CFile::modeCreate|CFile::modeReadWrite|CFile::typeBinary))
			{
				file.Write("军棋存盘文件",(UINT)strlen("军棋存盘文件"));
				//保存人数
				file.Write(&m_HistoryRecord.m_Player , sizeof(m_HistoryRecord.m_Player)) ;
				//保存布阵
				file.Write(&m_HistoryRecord.m_Chess,sizeof(m_HistoryRecord.m_Chess));
#ifdef _DEBUG
				TRACE("ShowSave:\n") ;
				for(BYTE x=0 ; x<17 ; ++x)
					for(BYTE y=0 ; y<17 ; ++y)
						TRACE("(%d , %d):%s\n" , x , y , m_GameLogic.IDToName(m_HistoryRecord.m_Chess[x][y].bChessID)) ;
#endif
				//保存走棋
				POSITION pos = m_HistoryRecord.m_list.GetHeadPosition();
				for (int i=0;i < m_HistoryRecord.m_list.GetCount();i++)
				{
					SaveChessStruct SaveChess = m_HistoryRecord.m_list.GetNext(pos) ;
					file.Write(&SaveChess , sizeof(SaveChessStruct));
					TRACE("(x%d , y%d) : (x%d , y%d)" , SaveChess.bSourceX , SaveChess.bSourceY , SaveChess.bTargetX , SaveChess.bTargetY) ;
				}
				file.Close();
			}
		}
	}
}

//坐标转换
void CGameClientDlg::ClientToServer(const BYTE bChessColor , const BYTE bRealClientX , const BYTE bRealClientY , BYTE &bServerX , BYTE &bServerY) 
{
	switch(bChessColor) 
	{
	case CHESS_COLOR_GREEN:	//绿色棋子
		bServerX = 16-bRealClientX ;
		bServerY = 16-bRealClientY ;
		break;

	case CHESS_COLOR_BLACK:	//黑色棋子
		bServerX = bRealClientY ;
		bServerY = 16-bRealClientX ;
		break;

	case CHESS_COLOR_BLUE:	//蓝色棋子
		bServerX = bRealClientX ;
		bServerY = bRealClientY ;
		break;

	case CHESS_COLOR_RED:	//红色棋子
		bServerX = 16-bRealClientY ;
		bServerY = bRealClientX ;
		break;

	default:
		AfxMessageBox("ClientToServer:错误棋子") ;
		bServerX = 255 ;
		bServerY = 255 ;
	}
}

//坐标转换
void CGameClientDlg::ServerToClient(const BYTE bChessColor , const BYTE bServerX , const BYTE bServerY , BYTE &bClientX , BYTE &bClientY) 
{
	switch(bChessColor) 
	{
	case CHESS_COLOR_GREEN:	//绿色棋子
		bClientX = 16-bServerX ;
		bClientY = 16-bServerY ;
		break;

	case CHESS_COLOR_BLACK:	//黑色棋子
		bClientX = 16-bServerY ;
		bClientY = bServerX ;
		break;

	case CHESS_COLOR_BLUE:	//蓝色棋子
		bClientX = bServerX ;
		bClientY = bServerY ;
		break;

	case CHESS_COLOR_RED:	//红色棋子
		bClientX = bServerY ;
		bClientY = 16-bServerX;
		break;

	default:
		AfxMessageBox("ClientToServer:错误棋子") ;
		bClientX = 255 ;
		bClientY = 255 ;
	}

}

//游戏棋谱
bool CGameClientDlg::OnSubChessManual(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(SaveChessStruct)==0);
	if (wDataSize%sizeof(SaveChessStruct)!=0) return false;

	//设置棋谱
	WORD wManualCount=wDataSize/sizeof(SaveChessStruct);
	SaveChessStruct * pChessManual=(SaveChessStruct *)pBuffer;
	
	for(LONG lCount=0 ; lCount < wManualCount ; ++lCount)
	{
		m_HistoryRecord.AddToList(pChessManual[lCount]) ;
	}

	return true;
}

//初始布局
bool CGameClientDlg::OnSubPlacement(const void * pDataBuffer, WORD wDataSize) 
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
	if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

	CMD_S_StatusPlay *pStatusPlay = (CMD_S_StatusPlay *)pDataBuffer ;

    m_HistoryRecord.SavePlacement(pStatusPlay->Chess) ;
	m_wCurrentUser = pStatusPlay->wCurrentUser ;

	return true ;
}


//玩家认输
LRESULT CGameClientDlg::OnHitLose(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam) ;
	UNREFERENCED_PARAMETER(lParam) ;

	SendData(SUB_C_USER_SURRENDER , NULL , 0) ;
	
	return 0 ;
}
//放弃走棋
LRESULT CGameClientDlg::OnHitGiveUp(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam) ;
	UNREFERENCED_PARAMETER(lParam) ;

	SendData(SUB_C_GAME_TIME_OUT , NULL , 0);
	KillGameTimer(IDI_GAME_TIMER) ;
	
	return 0 ;
}
//请求和棋
LRESULT CGameClientDlg::OnHitPeace(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam) ;
	UNREFERENCED_PARAMETER(lParam) ;

	SendData(SUB_C_REQUEST_PEACE , NULL , 0) ;

	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
