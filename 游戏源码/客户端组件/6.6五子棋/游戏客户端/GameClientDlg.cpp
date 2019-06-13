#include "Stdafx.h"
#include "GameClient.h"
#include "GameOption.h"
#include "ChessManual.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define IDI_GAME_TIMER				100									//游戏定时器
#define IDI_START_GAME				101									//开始定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_PEACE,OnPeace)
	ON_MESSAGE(IDM_TRADE,OnTrade)
	ON_MESSAGE(IDM_STUDY,OnStudy)
	ON_MESSAGE(IDM_REGRET,OnRegret)
	ON_MESSAGE(IDM_GIVEUP,OnGiveUp)
	ON_MESSAGE(IDM_PRESERVE,OnPreserve)
	ON_MESSAGE(IDM_SHOW_ID,OnShowChessID)
	ON_MESSAGE(IDM_PEACE_ANSWER,OnPeaceAnswer)
	ON_MESSAGE(IDM_REGRET_ANSWER,OnRegretAnswer)
	ON_MESSAGE(WM_HIT_CHESS_BORAD, OnHitChessBorad)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//规则变量
	m_bRestrict=true;
	m_bTradeUser=true;
	m_bDoubleChess=true;

	//配置变量
	m_bDoubleChess=false;
	m_bShowChessID=false;

	//游戏变量
	m_ShamView=false;
	m_wBlackUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));

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
	TCHAR szTitle[128]=TEXT("");
	_snprintf(szTitle,sizeof(szTitle),TEXT("%s  --  Ver：6.0.1.0"),GAME_NAME);
	SetWindowText(szTitle);

	//读取配置
	m_bDoubleMouse=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DoubleMouse"),FALSE)?true:false;
	m_bShowChessID=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("ShowChessID"),FALSE)?true:false;

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//设置控件
	m_GameClientView.m_ChessBorad.ShowChessID(m_bShowChessID);
	m_GameClientView.m_ChessBorad.SetDoubleMouse(m_bDoubleMouse);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//规则变量
	m_bRestrict=true;
	m_bTradeUser=true;
	m_bDoubleChess=true;
	m_wBlackUser=INVALID_CHAIR;

	//游戏变量
	m_ShamView=false;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));

	//组件变量
	m_GameLogic.InitGameLogic();

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bDoubleMouse=m_bDoubleMouse;
	GameOption.m_bShowChessID=m_bShowChessID;
	GameOption.m_bEnableSound=IsEnableSound();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置变量
		m_bDoubleMouse=GameOption.m_bDoubleMouse;
		m_bShowChessID=GameOption.m_bShowChessID;

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DoubleMouse"),m_bDoubleMouse);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("ShowChessID"),m_bShowChessID);

		//功能设置
		EnableSound(GameOption.m_bEnableSound);
		m_GameClientView.m_ChessBorad.ShowChessID(m_bShowChessID);
		m_GameClientView.m_ChessBorad.SetDoubleMouse(m_bDoubleMouse);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	if ( nTimerID == IDI_START_GAME )
	{
		if (wChairID == GetMeChairID() && ! IsLookonMode())		//开始游戏
		{
			if (nElapse==0)
			{
				SendMessage(WM_CLOSE,0,0);
				return false;
			}
			if ((nElapse<=10)&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

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
	case SUB_S_PLACE_CHESS:		//放置棋子
		{
			return OnSubPlaceChess(pBuffer,wDataSize);
		}
	case SUB_S_REGRET_REQ:		//悔棋请求
		{
			return OnSubRegretReq(pBuffer,wDataSize);
		}
	case SUB_S_REGRET_FAILE:	//悔棋失败
		{
			return OnSubRegretFaile(pBuffer,wDataSize);
		}
	case SUB_S_REGRET_RESULT:	//悔棋结果
		{
			return OnSubRegretResult(pBuffer,wDataSize);
		}
	case SUB_S_PEACE_REQ:		//和棋请求
		{
			return OnSubPeaceReq(pBuffer,wDataSize);
		}
	case SUB_S_PEACE_ANSWER:	//和棋应答
		{
			return OnSubPeaceAnser(pBuffer,wDataSize);
		}
	case SUB_S_BLACK_TRADE:		//交换对家
		{
			return OnSubBlackTrade(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_CHESS_MANUAL:	//游戏棋谱
		{
			return OnSubChessManual(pBuffer,wDataSize);
		}
	}

	return false;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置界面
			m_wBlackUser=pStatusFree->wBlackUser;
			m_wCurrentUser=pStatusFree->wBlackUser;
			m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.EnableWindow(TRUE);
				m_GameClientView.m_btStart.SetFocus();

				//设置时间
				SetGameTimer(GetMeChairID(), IDI_START_GAME, 30) ;
			}

			return true;
		}
	case GS_PLAYING:	//游戏状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//消息处理
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置变量
			m_wBlackUser=pStatusPlay->wBlackUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_bRestrict=pStatusPlay->cbRestrict?true:false;
			m_bTradeUser=pStatusPlay->cbTradeUser?true:false;
			m_bDoubleChess=pStatusPlay->cbDoubleChess?true:false;
			
			//设置时间
			CopyMemory(m_wLeftClock,pStatusPlay->wLeftClock,sizeof(m_wLeftClock));
			m_GameClientView.SetGameClock(pStatusPlay->wGameClock);
			m_GameClientView.SetUserClock(SwitchViewChairID(0),m_wLeftClock[0]);
			m_GameClientView.SetUserClock(SwitchViewChairID(1),m_wLeftClock[1]);

			//设置界面
			m_GameClientView.m_btPreserve.EnableWindow(TRUE);
			m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

			//棋盘界面
			m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_cbChessBorad,m_GameLogic.m_cbIdentifier);

			//设置框架
			const tagChessManual * pChessManual=m_GameLogic.GetLastChessManual();
			if (pChessManual!=NULL) m_GameClientView.m_ChessBorad.SetChessFocus(pChessManual->cbXPos,pChessManual->cbYPos);

			//玩家设置
			if (IsLookonMode()==false)
			{
				//变量定义
				WORD wMeChairID=GetMeChairID();
				BOOL bEnablePeace=((pStatusPlay->cbBegStatus[wMeChairID]&GU_WAIT_PEACE)==0);
				BYTE cbStepCount=m_GameLogic.GetStepCount((GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);

				//按钮设置
				m_GameClientView.m_btGiveUp.EnableWindow(TRUE);
				m_GameClientView.m_btPeace.EnableWindow(bEnablePeace);
				if (cbStepCount>0)
				{
					BOOL bEnableRegret=((pStatusPlay->cbBegStatus[wMeChairID]&GU_WAIT_REGRET)==0);
					m_GameClientView.m_btRegret.EnableWindow(bEnableRegret);
				}

				//对换控制
				BYTE cbAllStepCount=m_GameLogic.m_cbStepCount[0]+m_GameLogic.m_cbStepCount[1];
				if ((m_bTradeUser==true)&&(wMeChairID!=m_wBlackUser)&&(cbAllStepCount==3))
				{
					m_GameClientView.m_btTrade.EnableWindow(TRUE);
				}

				//变量定义
				WORD wTowardsUser=(wMeChairID+1)%GAME_PLAYER;
				BOOL bPeaceReq=((pStatusPlay->cbBegStatus[wTowardsUser]&GU_WAIT_PEACE)!=0);
				BOOL bRegretReq=((pStatusPlay->cbBegStatus[wTowardsUser]&GU_WAIT_REGRET)!=0);

				//请求设置
				if (bPeaceReq==TRUE) m_GameClientView.ShowPeaceRequest();
				if (bRegretReq==TRUE) m_GameClientView.ShowRegretRequest();

				//操作设置
				if (wMeChairID==m_wCurrentUser) m_GameClientView.m_ChessBorad.SetSelectMode(SM_NULL_CHESS);
			}

			//设置定时器
			SetTimer(IDI_GAME_TIMER,1000,NULL);

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//游戏变量
	m_ShamView=false;
	m_wBlackUser=pGameStart->wBlackUser;
	m_wCurrentUser=pGameStart->wBlackUser;
	m_bRestrict=pGameStart->cbRestrict?true:false;
	m_bTradeUser=pGameStart->cbTradeUser?true:false;
	m_bDoubleChess=pGameStart->cbDoubleChess?true:false;

	//设置时间
	m_wLeftClock[0]=m_wLeftClock[1]=pGameStart->wGameClock;
	m_GameClientView.SetGameClock(pGameStart->wGameClock);
	m_GameClientView.SetUserClock(SwitchViewChairID(0),m_wLeftClock[0]);
	m_GameClientView.SetUserClock(SwitchViewChairID(1),m_wLeftClock[1]);

	//设置变量
	m_GameLogic.InitGameLogic();

	//设置界面
	m_GameClientView.m_btPreserve.EnableWindow(TRUE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));
	if (IsLookonMode()==true) m_GameClientView.m_ChessBorad.CleanChess();

	//全部玩家
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.EnableWindow(FALSE);
		m_GameClientView.m_btPeace.EnableWindow(TRUE);
		m_GameClientView.m_btGiveUp.EnableWindow(TRUE);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_ChessBorad.SetSelectMode(SM_NULL_CHESS);
	}

	//设置定时器
	SetTimer(IDI_GAME_TIMER,1000,NULL);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//用户放棋
bool CGameClientDlg::OnSubPlaceChess(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlaceChess));
	if (wDataSize!=sizeof(CMD_S_PlaceChess)) return false;

	//消息处理
	CMD_S_PlaceChess * pPlaceChess=(CMD_S_PlaceChess *)pBuffer;

	//游戏变量
	m_wCurrentUser=pPlaceChess->wCurrentUser;
	CopyMemory(m_wLeftClock,pPlaceChess->wLeftClock,sizeof(m_wLeftClock));

	//设置界面
	m_ShamView=false;
	WORD wMeChairID=GetMeChairID();
	if ((IsLookonMode()==true)||(pPlaceChess->wPlaceUser!=wMeChairID))
	{
		//变量定义
		WORD wPlaceUser=pPlaceChess->wPlaceUser;
		BYTE cbColor=(wPlaceUser==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;

		//放置棋子
		BYTE cbIdentifier=m_GameLogic.PlaceChess(pPlaceChess->cbXPos,pPlaceChess->cbYPos,cbColor);

		//设置棋盘
		m_GameClientView.m_ChessBorad.SetChess(pPlaceChess->cbXPos,pPlaceChess->cbYPos,cbColor,cbIdentifier);
		m_GameClientView.m_ChessBorad.SetChessFocus(pPlaceChess->cbXPos,pPlaceChess->cbYPos);

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("PLACE_CHESS"));
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pPlaceChess->wCurrentUser==wMeChairID))
	{
		//交换判断
		BYTE cbAllStepCount=m_GameLogic.m_cbStepCount[0]+m_GameLogic.m_cbStepCount[1];
		if ((m_bTradeUser==true)&&(cbAllStepCount==3)) m_GameClientView.m_btTrade.EnableWindow(TRUE);

		//界面设置
		ActiveGameFrame();
		m_GameClientView.m_ChessBorad.SetSelectMode(SM_NULL_CHESS);
	}

	return true;
}

//悔棋请求
bool CGameClientDlg::OnSubRegretReq(const void * pBuffer, WORD wDataSize)
{
	m_GameClientView.ShowRegretRequest();

	return true;
}

//悔棋失败
bool CGameClientDlg::OnSubRegretFaile(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RegretFaile));
	if (wDataSize!=sizeof(CMD_S_RegretFaile)) return false;

	//消息处理
	CMD_S_RegretFaile * pRegretFaile=(CMD_S_RegretFaile *)pBuffer;

	//次数限制
	if (pRegretFaile->cbFaileReason==FR_COUNT_LIMIT)
	{
		InsertSystemString(TEXT("由于你的 [ 悔棋 ] 请求次数过多，悔棋失败"));
		return true;
	}

	//对家反对
	if (pRegretFaile->cbFaileReason==FR_PLAYER_OPPOSE)
	{
		InsertSystemString(TEXT("对家不同意你 [ 悔棋 ]"));
		BYTE cbColor=(GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
		m_GameClientView.m_btRegret.EnableWindow(m_GameLogic.GetStepCount(cbColor)>0);
		return true;
	}

	return true;
}

//悔棋结果
bool CGameClientDlg::OnSubRegretResult(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RegretResult));
	if (wDataSize!=sizeof(CMD_S_RegretResult)) return false;

	//变量定义
	CMD_S_RegretResult * pRegretResult=(CMD_S_RegretResult *)pBuffer;
	CopyMemory(m_wLeftClock,pRegretResult->wLeftClock,sizeof(m_wLeftClock));

	//调整次数
	if (m_ShamView==true)
	{
		m_ShamView=false;
		pRegretResult->wRegretCount++;
	}

	//修改棋谱
	m_GameLogic.RegretChess(pRegretResult->wRegretCount);
	m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_cbChessBorad,m_GameLogic.m_cbIdentifier);

	//设置焦点
	const tagChessManual * pChessManual=m_GameLogic.GetLastChessManual();
	if (pChessManual==NULL) m_GameClientView.m_ChessBorad.SetChessFocus(255,255);
	else m_GameClientView.m_ChessBorad.SetChessFocus(pChessManual->cbXPos,pChessManual->cbYPos);

	//玩家设置
	WORD wMeChairID=GetMeChairID();
	m_wCurrentUser=pRegretResult->wCurrentUser;
	if ((IsLookonMode()==false)&&(pRegretResult->wCurrentUser==wMeChairID))
	{
		ActiveGameFrame();
		m_GameClientView.m_ChessBorad.SetSelectMode(SM_NULL_CHESS);
	}

	//悔棋控制
	if (IsLookonMode()==false)
	{
		BYTE cbStepCount=m_GameLogic.GetStepCount((GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);
		if (cbStepCount==0) m_GameClientView.m_btRegret.EnableWindow(FALSE);
		if ((cbStepCount>0)&&(pRegretResult->wRegretUser==wMeChairID)) m_GameClientView.m_btRegret.EnableWindow(TRUE);
	}

	return true;
}

//和棋请求
bool CGameClientDlg::OnSubPeaceReq(const void * pBuffer, WORD wDataSize)
{
	m_GameClientView.ShowPeaceRequest();

	return true;
}

//和棋应答
bool CGameClientDlg::OnSubPeaceAnser(const void * pBuffer, WORD wDataSize)
{
	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(TRUE);
	InsertSystemString(TEXT("对家不同意你 [ 和棋 ]"));

	return true;
}

//交换对家
bool CGameClientDlg::OnSubBlackTrade(const void * pBuffer, WORD wDataSize)
{
	//对换用户
	m_wCurrentUser=m_wBlackUser;
	m_wBlackUser=(m_wBlackUser+1)%GAME_PLAYER;

	//界面设置
	InsertSystemString(TEXT("[ 黑棋方 ] 与 [ 白棋方 ] 对换了"));
	m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

	//玩家设置
	if ((IsLookonMode()==false)&&(GetMeChairID()!=m_wBlackUser))
	{
		ActiveGameFrame();
		m_GameClientView.m_ChessBorad.SetSelectMode(SM_NULL_CHESS);
	}

	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;

	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(FALSE);
	m_GameClientView.m_btTrade.EnableWindow(FALSE);
	m_GameClientView.m_btRegret.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);

	//播放声音
	if (IsLookonMode()==false)
	{
		if (pGameEnd->wWinUser==GetMeChairID()) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));

	//成绩设置
	tagGameScoreInfo GameScoreInfo;
	memset(&GameScoreInfo,0,sizeof(GameScoreInfo));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wUserChairID=(m_wBlackUser+i)%GAME_PLAYER;
		const tagUserData * pUserData=GetUserData(wUserChairID);
		GameScoreInfo.lScore[i]=pGameEnd->lUserScore[wUserChairID];
		lstrcpyn(GameScoreInfo.szUserName[i],pUserData->szName,CountArray(GameScoreInfo.szUserName[i]));
	}
	m_GameClientView.m_GameScoreWnd.SetScoreInfo(GameScoreInfo);

	//设置界面
	if (IsLookonMode()==false)
	{
		ActiveGameFrame();
		m_GameClientView.m_btStart.EnableWindow(TRUE);
		m_GameClientView.m_btStart.SetFocus();
		if (m_GameClientView.m_PeaceRequest.GetSafeHwnd()!=NULL) m_GameClientView.m_PeaceRequest.DestroyWindow();
		if (m_GameClientView.m_RegretRequest.GetSafeHwnd()!=NULL) m_GameClientView.m_RegretRequest.DestroyWindow();
	}

	//删除定时器
	KillTimer(IDI_GAME_TIMER);

	//系统消息
	static bool bNotify=false;
	if (bNotify==false)
	{
		bNotify=true;
		InsertSystemString(TEXT("游戏结束了，若需要保存棋谱，请点击 [ 保存 ] 按钮保存棋谱记录"));
	}

	//设置定时器
	if(false==IsLookonMode())
	{
		SetGameTimer(GetMeChairID(), IDI_START_GAME, 30) ;
		m_nElapse = 30 ;
	}

	return true;
}

//游戏棋谱
bool CGameClientDlg::OnSubChessManual(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagChessManual)==0);
	if (wDataSize%sizeof(tagChessManual)!=0) return false;

	//放置棋子
	WORD wManualCount=wDataSize/sizeof(tagChessManual);
	for (WORD wIndex=0;wIndex<wManualCount;wIndex++) 
	{
		tagChessManual * pChessManual=(((tagChessManual *)pBuffer)+wIndex);
		m_GameLogic.PlaceChess(pChessManual->cbXPos,pChessManual->cbYPos,pChessManual->cbColor);
	}

	return true;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	//倒数定时器
	if ((nIDEvent==IDI_GAME_TIMER)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		//设置视图
		if (m_wLeftClock[m_wCurrentUser]>0)
		{
			m_wLeftClock[m_wCurrentUser]--;
			m_GameClientView.SetUserClock(SwitchViewChairID(m_wCurrentUser),m_wLeftClock[m_wCurrentUser]);
		}
		
		//超时判断
		if ((IsLookonMode()==false)&&(GetMeChairID()==m_wCurrentUser))
		{
			if (m_wLeftClock[m_wCurrentUser]==0)
			{
				//发送消息
				SendData(SUB_C_GIVEUP_REQ);

				//设置界面
				m_GameClientView.m_btPeace.EnableWindow(FALSE);
				m_GameClientView.m_btTrade.EnableWindow(FALSE);
				m_GameClientView.m_btRegret.EnableWindow(FALSE);
				m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
				m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);

				return;
			}
			else if (m_wLeftClock[m_wCurrentUser]<=30) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
		}

		return;
	}

	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//规则变量
	m_bRestrict=true;
	m_bTradeUser=true;
	m_bDoubleChess=true;

	//游戏变量
	m_wBlackUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//设置界面
	m_GameClientView.m_ChessBorad.CleanChess();
	m_GameClientView.m_btStart.EnableWindow(FALSE);
	m_GameClientView.m_btPreserve.EnableWindow(FALSE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);

	KillGameTimer(IDI_START_GAME) ;

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//悔棋按钮
LRESULT CGameClientDlg::OnRegret(WPARAM wParam, LPARAM lParam)
{
	//发送消息
	SendData(SUB_C_REGRET_REQ);

	//设置界面
	m_GameClientView.m_btRegret.EnableWindow(FALSE);
	InsertSystemString(TEXT("正在等待对家回应 [ 悔棋 ] 请求..."));

	return 0;
}

//求和按钮
LRESULT CGameClientDlg::OnPeace(WPARAM wParam, LPARAM lParam)
{
	//和棋确认
	int nResult=AfxMessageBox(TEXT("你确实要 [ 和棋 ] 吗？"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
	if (nResult!=IDYES) return 0;

	//状态判断
	if (m_wCurrentUser==INVALID_CHAIR) return 0;

	//发送消息
	SendData(SUB_C_PEACE_REQ);

	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(FALSE);
	InsertSystemString(TEXT("正在等待对家回应 [ 和棋 ] 请求..."));

	return 0;
}

//认输按钮
LRESULT CGameClientDlg::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	//认输确认
	int nResult=AfxMessageBox(TEXT("你确实要 [ 认输 ] 吗？"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
	if (nResult!=IDYES) return 0;

	//状态判断
	if (m_wCurrentUser==INVALID_CHAIR) return 0;

	//发送消息
	SendData(SUB_C_GIVEUP_REQ);

	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(FALSE);
	m_GameClientView.m_btTrade.EnableWindow(FALSE);
	m_GameClientView.m_btRegret.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);

	return 0;
}

//手数按钮
LRESULT CGameClientDlg::OnShowChessID(WPARAM wParam, LPARAM lParam)
{
	//设置控件
	m_bShowChessID=!m_bShowChessID;
	m_GameClientView.m_ChessBorad.ShowChessID(m_bShowChessID);

	//保存配置
	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("ShowChessID"),m_bShowChessID);

	//系统消息
	if (m_bShowChessID==true) InsertSystemString(TEXT("设置为显示棋子手数"));
	else InsertSystemString(TEXT("设置为隐藏棋子手数"));

	return 0;
}

//对换按钮
LRESULT CGameClientDlg::OnTrade(WPARAM wParam, LPARAM lParam)
{
	//交换确认
	int nResult=AfxMessageBox(TEXT("你确实要与黑棋 [ 交换 ] 吗？"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
	if (nResult!=IDYES) return 0;

	//状态判断
	if (m_wCurrentUser==INVALID_CHAIR) return 0;

	//发送消息
	SendData(SUB_C_TRADE_REQ);

	//设置界面
	m_GameClientView.m_btTrade.EnableWindow(FALSE);
	m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);

	return 0;
}

//保存按钮
LRESULT CGameClientDlg::OnPreserve(WPARAM wParam, LPARAM lParam)
{
	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//选择文件
	LPCTSTR pszFilter=TEXT("五子棋棋谱文件 （*.GBM）|*.GBM||");
	CFileDialog DlgSaveManual(FALSE,TEXT("GBM"),NULL,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter);
	DlgSaveManual.m_pOFN->lpstrInitialDir=szPath;
	if (DlgSaveManual.DoModal()!=IDOK) return 0;

	//打开文件
	CFile FileManual;
	if (FileManual.Open(DlgSaveManual.GetPathName(),CFile::modeWrite|CFile::modeCreate)==FALSE)
	{
		AfxMessageBox(TEXT("无法创建文件，棋谱保存失败！"),MB_ICONSTOP);
		return 0;
	}

	//文件头
	tagChessManualHead ChessManualHead;
	ChessManualHead.wVersion=VER_MANUAL;
	ChessManualHead.dwManualDate=(DWORD)time(NULL);
	ChessManualHead.dwManualCount=(DWORD)m_GameLogic.m_ChessManualInfo.GetCount();

	//写入文件头
	FileManual.SeekToBegin();
	FileManual.Write(&ChessManualHead,sizeof(ChessManualHead));

	//棋谱数据
	tagChessManual * pChessManual=NULL;
	for (DWORD i=0;i<ChessManualHead.dwManualCount;i++)
	{
		pChessManual=&m_GameLogic.m_ChessManualInfo[i];
		FileManual.Write(pChessManual,sizeof(tagChessManual));
	}

	//关闭文件
	FileManual.SetLength(sizeof(ChessManualHead)+ChessManualHead.dwManualCount*sizeof(tagChessManual));
	FileManual.Close();

	//系统消息
	InsertSystemString(TEXT("五子棋游戏“棋谱记录”保存成功"));

	return 0;
}

//研究按钮
LRESULT CGameClientDlg::OnStudy(WPARAM wParam, LPARAM lParam)
{
	//创建棋谱
	if (m_GameClientView.m_ChessManual.GetSafeHwnd()==NULL)
	{
		m_GameClientView.m_ChessManual.m_pDlgGameLogic=&m_GameLogic;
		m_GameClientView.m_ChessManual.Create(IDD_MANUAL,&m_GameClientView);
	}

	//显示棋谱
	m_GameClientView.m_ChessManual.CenterWindow(NULL);
	m_GameClientView.m_ChessManual.ShowWindow(SW_SHOW);

	return 0;
}

//和棋应答
LRESULT CGameClientDlg::OnPeaceAnswer(WPARAM wParam, LPARAM lParam)
{
	//发送消息
	CMD_C_PeaceAnswer PeaceAnswer;
	PeaceAnswer.cbApprove=BYTE(wParam);
	SendData(SUB_C_PEACE_ANSWER,&PeaceAnswer,sizeof(PeaceAnswer));

	return 0;
}

//悔棋应答
LRESULT CGameClientDlg::OnRegretAnswer(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	if (wParam==TRUE)
	{
		m_wCurrentUser=INVALID_CHAIR;
		m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);
	}

	//发送消息
	CMD_C_RegretAnswer ReqretAnswer;
	ReqretAnswer.cbApprove=BYTE(wParam);
	SendData(SUB_C_REGRET_ANSWER,&ReqretAnswer,sizeof(ReqretAnswer));

	return 0;
}

//点击棋盘
LRESULT CGameClientDlg::OnHitChessBorad(WPARAM wParam, LPARAM lParam)
{
	if (m_wCurrentUser==GetMeChairID())
	{
		//变量定义
		WORD wMeChairID=GetMeChairID();
		BYTE cbXPos=(BYTE)wParam,cbYPos=(BYTE)lParam;
		BYTE cbColor=(m_wBlackUser==wMeChairID)?BLACK_CHESS:WHITE_CHESS;

		//放置棋子
		BYTE cbIdentifier=m_GameLogic.PlaceChess(cbXPos,cbYPos,cbColor);
		if (cbIdentifier==0)
		{
			InsertSystemString(TEXT("这点是禁手点，不能在此点放棋"));
			PlayGameSound(AfxGetInstanceHandle(),TEXT("INVALID_PLACE"));
			return 0;
		}

		//设置变量
		m_ShamView=true;
		m_wCurrentUser=INVALID_CHAIR;

		//界面设置
		m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);
		m_GameClientView.m_ChessBorad.SetChess((BYTE)wParam,(BYTE)lParam,cbColor,cbIdentifier);
		m_GameClientView.m_ChessBorad.SetChessFocus((BYTE)wParam,(BYTE)lParam);

		//悔棋判断
		if (m_GameLogic.GetStepCount(cbColor)==1) m_GameClientView.m_btRegret.EnableWindow(TRUE);

		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("PLACE_CHESS"));

		//发送消息
		CMD_C_PlaceChess PlaceChess;
		PlaceChess.cbXPos=(BYTE)wParam;
		PlaceChess.cbYPos=(BYTE)lParam;
		SendData(SUB_C_PLACE_CHESS,&PlaceChess,sizeof(PlaceChess));
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
