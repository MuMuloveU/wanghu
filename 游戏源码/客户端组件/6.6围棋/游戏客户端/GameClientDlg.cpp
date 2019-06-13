#include "Stdafx.h"
#include "GameClient.h"
#include "GameOption.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define IDI_GAME_TIMER				100									//游戏定时器
#define IDI_START_GAME				101									//开始定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_COUNT,OnCount)
	ON_MESSAGE(IDM_PASS,OnPass)
	ON_MESSAGE(IDM_PEACE,OnPeace)
	ON_MESSAGE(IDM_STUDY,OnStudy)
	ON_MESSAGE(IDM_REGRET,OnRegret)
	ON_MESSAGE(IDM_GIVEUP,OnGiveUp)
	ON_MESSAGE(IDM_PRESERVE,OnPreserve)
	ON_MESSAGE(IDM_PEACE_ANSWER,OnPeaceAnswer)
	ON_MESSAGE(IDM_REGRET_ANSWER,OnRegretAnswer)
	ON_MESSAGE(WM_HIT_CHESS_BORAD, OnHitChessBorad)
	ON_MESSAGE(IDM_AGREE_GAME_RULE, OnAgreeGameRule)
	ON_MESSAGE(IDM_CHESS_COUNT_RESPONSION, OnChessCountResponsion)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//规则变量
	memset(&m_GameRuleInfo,0,sizeof(m_GameRuleInfo));

	//配置变量
	m_bDoubleMouse=false;
	m_bShowChessID=false;
	m_bAutoSaveManual=false;

	//游戏辅助
	m_ShamView=false;
	m_bAgreeRule=false;
	m_bCustomize=false;
	m_bGameStart=false;
	m_bLimitTime=false;
	m_bReqChessCount=false;
	m_bResponsionChessCount=false;

	//游戏变量
	m_wBlackUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_StatusInfo,0,sizeof(m_StatusInfo));

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
	SetWindowText(TEXT("围棋游戏"));

	//读取配置
	m_bDoubleMouse=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DoubleMouse"),FALSE)?true:false;
	m_bShowChessID=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("ShowChessID"),FALSE)?true:false;
	m_bAutoSaveManual=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("AutoSaveManual"),FALSE)?true:false;

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
	memset(&m_GameRuleInfo,0,sizeof(m_GameRuleInfo));

	//游戏辅助
	m_ShamView=false;
	m_bAgreeRule=false;
	m_bCustomize=false;
	m_bGameStart=false;
	m_bLimitTime=false;
	m_bReqChessCount=false;
	m_bResponsionChessCount=false;

	//游戏变量
	m_wBlackUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_StatusInfo,0,sizeof(m_StatusInfo));

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
	GameOption.m_bAutoSaveManual=m_bAutoSaveManual;

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置变量
		m_bDoubleMouse=GameOption.m_bDoubleMouse;
		m_bShowChessID=GameOption.m_bShowChessID;
		m_bAutoSaveManual=GameOption.m_bAutoSaveManual;

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DoubleMouse"),m_bDoubleMouse);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("ShowChessID"),m_bShowChessID);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("AutoSaveManual"),m_bAutoSaveManual);

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
	case SUB_S_PASS_CHESS:		//放弃下棋
		{
			return OnSubPassChess(pBuffer,wDataSize);
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
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_CHESS_MANUAL:	//游戏棋谱
		{
			return OnSubChessManual(pBuffer,wDataSize);
		}
	case SUB_S_REQ_SET_RULE:	//请求设置
		{
			return OnSubSetRule(pBuffer,wDataSize);
		}
	case SUB_S_REQ_AGREE_RULE:	//请求同意
		{
			return OnSubAgreeRule(pBuffer,wDataSize);
		}
	case SUB_S_REQ_CHESS_COUNT:	//请求点目
		{
			return OnSubReqChessCount(pBuffer,wDataSize);
		}
	case SUB_S_UNAGREE_CHESS_COUNT:	//点目应答
		{
			return OnSubChessCountResponsion(pBuffer,wDataSize);
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

			//设置变量
			m_wBlackUser=pStatusFree->wBlackUser;
			m_wCurrentUser=pStatusFree->wBlackUser;
			m_GameRuleInfo=pStatusFree->GameRuleInfo;
			m_bCustomize=((IsLookonMode()==false)&&(m_wBlackUser==GetMeChairID()));

			//设置界面
			m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

			//设置规则
			bool bShowGameRule=((IsLookonMode()==false)&&((m_wBlackUser==GetMeChairID())||(pStatusFree->cbGameRule==TRUE)));
			if (bShowGameRule==true)
			{
				//规则设置
				m_GameClientView.m_GameRule.m_GameRuleInfo=m_GameRuleInfo;

				//显示窗口
				if (m_GameClientView.m_GameRule.GetSafeHwnd()==NULL)
				{
					m_GameClientView.m_GameRule.Create(IDD_GAME_RULE,&m_GameClientView);
				}
				else m_GameClientView.m_GameRule.UpdateControlStatus();

				//设置控件
				bool bEnableControl=(m_wBlackUser==GetMeChairID());
				m_GameClientView.m_GameRule.ControlEnable(bEnableControl);
				m_GameClientView.m_GameRule.ShowWindow(SW_SHOW);
			}
			else InsertSystemString(TEXT("正等待“黑方”玩家设置游戏规则"));

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
			m_bGameStart=true;
			m_wBlackUser=pStatusPlay->wBlackUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_GameRuleInfo=pStatusPlay->GameRuleInfo;
			m_bLimitTime=((m_GameRuleInfo.wRuleStepTime>0)||(m_GameRuleInfo.wRuleLimitTime>0));

			//设置界面
			m_GameClientView.m_btPreserve.EnableWindow(TRUE);
			m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

			//棋盘界面
			m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_cbChessBorad,m_GameLogic.m_wIdentifier);

			//设置框架
			const tagChessManual * pChessManual=m_GameLogic.GetLastChessManual();
			if (pChessManual!=NULL) m_GameClientView.m_ChessBorad.SetChessFocus(pChessManual->cbXPos,pChessManual->cbYPos);

			//用户信息
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				BYTE cbColor=(i==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
				m_StatusInfo[i].wUseTimeCount=pStatusPlay->wUseClock[i];
				m_StatusInfo[i].wLimitTimeCount=pStatusPlay->wLeftClock[i];
				m_StatusInfo[i].wStepCount=m_GameLogic.GetStepCount(cbColor);
				m_StatusInfo[i].wTakeChessCount=m_GameLogic.GetTakeChessCount(cbColor);
				m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_StatusInfo[i]);
			}

			//玩家设置
			if (IsLookonMode()==false)
			{
				//变量定义
				WORD wMeChairID=GetMeChairID();
				BOOL bEnablePeace=((pStatusPlay->cbBegStatus[wMeChairID]&GU_WAIT_PEACE)==0);
				WORD wStepCount=m_GameLogic.GetStepCount((GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);

				//按钮设置
				m_GameClientView.m_btGiveUp.EnableWindow(TRUE);
				m_GameClientView.m_btPeace.EnableWindow(bEnablePeace);
				if ((wStepCount>0)&&(m_GameRuleInfo.cbRegretFlag==TRUE))
				{
					BOOL bEnableRegret=((pStatusPlay->cbBegStatus[wMeChairID]&GU_WAIT_REGRET)==0);
					m_GameClientView.m_btRegret.EnableWindow(bEnableRegret);
				}

				//点目判断
				WORD wBlackStepCount=m_GameLogic.GetStepCount(BLACK_CHESS);
				if (wBlackStepCount==1) m_GameClientView.m_btCount.EnableWindow(TRUE);
				m_bReqChessCount=((pStatusPlay->cbBegStatus[wMeChairID]&GU_WAIT_CHESS_COUNT)!=0);
				m_bResponsionChessCount=((pStatusPlay->cbBegStatus[(wMeChairID+1)%GAME_PLAYER]&GU_WAIT_CHESS_COUNT)!=0);

				//变量定义
				WORD wEnemyUser=(wMeChairID+1)%GAME_PLAYER;
				bool bPeaceReq=((pStatusPlay->cbBegStatus[wEnemyUser]&GU_WAIT_PEACE)!=0);
				bool bRegretReq=((pStatusPlay->cbBegStatus[wEnemyUser]&GU_WAIT_REGRET)!=0);

				//请求设置
				if (bPeaceReq==true) m_GameClientView.ShowPeaceRequest();
				if (bRegretReq==true) m_GameClientView.ShowRegretRequest();

				//点目设置
				if (m_bResponsionChessCount==true)
				{
					//获取点目
					tagCountResult CountResult;
					memset(&CountResult,0,sizeof(CountResult));
					m_GameLogic.GetChessCountInfo(CountResult);

					//设置界面
					float fBlackCount=(float)CountResult.wChessCount[0]-((float)CountResult.wCommomCount)/(float)2.0;
					float fWhiteCount=(float)CountResult.wChessCount[1]-((float)CountResult.wCommomCount)/(float)2.0;
					m_GameClientView.m_ChessBorad.SetChessCountInfo(CountResult.cbCountInfo);
					m_GameClientView.m_ChessCountInfo.SetChessCountInfo(fBlackCount,fWhiteCount,CountResult.wCommomCount);

					//显示窗口
					m_GameClientView.m_ChessCountInfo.Create(IDD_CHESS_COUNT,&m_GameClientView);
					m_GameClientView.m_ChessCountInfo.ShowWindow(SW_SHOW);
				}

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
	m_wBlackUser=pGameStart->wBlackUser;
	m_wCurrentUser=pGameStart->wBlackUser;
	m_GameRuleInfo=pGameStart->GameRuleInfo;

	//辅助变量
	m_ShamView=false;
	m_bGameStart=true;
	m_bCustomize=false;
	m_bAgreeRule=false;
	memset(m_StatusInfo,0,sizeof(m_StatusInfo));
	m_bLimitTime=((m_GameRuleInfo.wRuleStepTime>0)||(m_GameRuleInfo.wRuleLimitTime>0));

	//重置逻辑
	m_GameLogic.InitGameLogic();

	//设置信息
	WORD wLimitTimeCount=m_GameRuleInfo.wRuleLimitTime;
	if (wLimitTimeCount==0) wLimitTimeCount=m_GameRuleInfo.wRuleStepTime;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_StatusInfo[i].wLimitTimeCount=wLimitTimeCount;
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_StatusInfo[i]);
	}

	//设置界面
	m_GameClientView.m_btPreserve.EnableWindow(TRUE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));
	if (IsLookonMode()==true) m_GameClientView.m_ChessBorad.CleanChess();

	//全部玩家
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btPeace.EnableWindow(TRUE);
		m_GameClientView.m_btGiveUp.EnableWindow(TRUE);
		m_GameClientView.m_btStart.EnableWindow(FALSE);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btPass.EnableWindow(TRUE);
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
	m_wCurrentUser=pPlaceChess->wCurrentUser;

	//设置界面
	m_ShamView=false;
	WORD wMeChairID=GetMeChairID();
	if ((IsLookonMode()==true)||(pPlaceChess->wPlaceUser!=wMeChairID))
	{
		//变量定义
		WORD wPlaceUser=pPlaceChess->wPlaceUser;
		BYTE cbColor=(wPlaceUser==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;

		//放置棋子
		WORD wDeadChess[361],wDeadChessCount=0;
		WORD wIdentifier=m_GameLogic.PlaceChess(pPlaceChess->cbXPos,pPlaceChess->cbYPos,cbColor,wDeadChess,wDeadChessCount);

		//点目判断
		WORD wBlackStepCount=m_GameLogic.GetStepCount(BLACK_CHESS);
		if (wBlackStepCount==1) m_GameClientView.m_btCount.EnableWindow(TRUE);

		//设置棋盘
		m_GameClientView.m_ChessBorad.SetChess(pPlaceChess->cbXPos,pPlaceChess->cbYPos,cbColor,wIdentifier);
		m_GameClientView.m_ChessBorad.SetChessFocus(pPlaceChess->cbXPos,pPlaceChess->cbYPos);
		if (wDeadChessCount>0) m_GameClientView.m_ChessBorad.CleanChess(wDeadChess,wDeadChessCount);

		//播放声音
		if (wDeadChessCount>0)
		{
			if (wMeChairID==wPlaceUser) PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_EAT"));
			else PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_DEAD"));
		}
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("PLACE_CHESS"));
	}

	//用户信息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		BYTE cbColor=(i==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
		m_StatusInfo[i].wUseTimeCount=pPlaceChess->wUseClock[i];
		m_StatusInfo[i].wLimitTimeCount=pPlaceChess->wLeftClock[i];
		m_StatusInfo[i].wStepCount=m_GameLogic.GetStepCount(cbColor);
		m_StatusInfo[i].wTakeChessCount=m_GameLogic.GetTakeChessCount(cbColor);
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_StatusInfo[i]);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pPlaceChess->wCurrentUser==wMeChairID))
	{
		ActiveGameFrame();
		m_GameClientView.m_btPass.EnableWindow(TRUE);
		m_GameClientView.m_ChessBorad.SetSelectMode(SM_NULL_CHESS);
	}

	return true;
}

//放弃下棋
bool CGameClientDlg::OnSubPassChess(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PassChess));
	if (wDataSize!=sizeof(CMD_S_PassChess)) return false;

	//消息处理
	CMD_S_PassChess * pPassChess=(CMD_S_PassChess *)pBuffer;
	m_wCurrentUser=pPassChess->wCurrentUser;

	//播放声音
	WORD wMeChairID=GetMeChairID();
	if ((IsLookonMode()==true)||(pPassChess->wPassUser!=wMeChairID))
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CHESS"));
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pPassChess->wCurrentUser==wMeChairID))
	{
		ActiveGameFrame();
		m_GameClientView.m_btPass.EnableWindow(TRUE);
		m_GameClientView.m_ChessBorad.SetSelectMode(SM_NULL_CHESS);
	}

	//用户信息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_StatusInfo[i].wUseTimeCount=pPassChess->wUseClock[i];
		m_StatusInfo[i].wLimitTimeCount=pPassChess->wLeftClock[i];
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_StatusInfo[i]);
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

	//对家反对
	if (pRegretFaile->cbFaileReason==FR_PLAYER_OPPOSE)
	{
		WORD wMeChairID=GetMeChairID();
		InsertSystemString(TEXT("对家不同意你 [ 悔棋 ]"));
		m_GameClientView.m_btRegret.EnableWindow((m_StatusInfo[wMeChairID].wStepCount>0)?TRUE:FALSE);

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

	//调整次数
	if (m_ShamView==true)
	{
		m_ShamView=false;
		pRegretResult->wRegretCount++;
	}

	//设置棋盘
	m_GameLogic.RegretChess(pRegretResult->wRegretCount);
	m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_cbChessBorad,m_GameLogic.m_wIdentifier);

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
		m_GameClientView.m_btPass.EnableWindow(TRUE);
		m_GameClientView.m_ChessBorad.SetSelectMode(SM_NULL_CHESS);
	}

	//用户信息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		BYTE cbColor=(i==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
		m_StatusInfo[i].wUseTimeCount=pRegretResult->wUseClock[i];
		m_StatusInfo[i].wLimitTimeCount=pRegretResult->wLeftClock[i];
		m_StatusInfo[i].wStepCount=m_GameLogic.GetStepCount(cbColor);
		m_StatusInfo[i].wTakeChessCount=m_GameLogic.GetTakeChessCount(cbColor);
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_StatusInfo[i]);
	}

	//点目判断
	WORD wBlackStepCount=m_GameLogic.GetStepCount(BLACK_CHESS);
	if (wBlackStepCount==0)
	{
		m_GameClientView.m_btCount.EnableWindow(FALSE);
		if (m_GameClientView.m_ChessCountInfo.GetSafeHwnd()!=NULL) m_GameClientView.m_ChessCountInfo.DestroyWindow();
	}

	//悔棋控制
	if (IsLookonMode()==false)
	{
		WORD wStepCount=m_GameLogic.GetStepCount((GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);
		if ((wStepCount==0)&&(m_GameRuleInfo.cbRegretFlag==TRUE)) m_GameClientView.m_btRegret.EnableWindow(FALSE);
		if ((wStepCount>0)&&(m_GameRuleInfo.cbRegretFlag==TRUE)&&(pRegretResult->wRegretUser==wMeChairID)) m_GameClientView.m_btRegret.EnableWindow(TRUE);
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

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//设置变量
	m_bGameStart=false;
	m_wCurrentUser=INVALID_CHAIR;

	//设置界面
	m_GameClientView.m_btPass.EnableWindow(FALSE);
	m_GameClientView.m_btPeace.EnableWindow(FALSE);
	m_GameClientView.m_btRegret.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_ChessBorad.CleanCountInfo();
	m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);
	if (m_GameClientView.m_ChessCountInfo.GetSafeHwnd()!=NULL) m_GameClientView.m_ChessCountInfo.DestroyWindow();

	//播放声音
	if (pGameEnd->wWinUser!=INVALID_CHAIR)
	{
		if (pGameEnd->lUserScore[GetMeChairID()]>=0) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));

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

		//设置时间
		SetGameTimer(GetMeChairID(), IDI_START_GAME, 30) ;
	}

	//删除定时器
	KillTimer(IDI_GAME_TIMER);

	//保存棋谱
	if (m_bAutoSaveManual==true)
	{
		//获取时间
		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);

		//获取目录
		TCHAR szPath[MAX_PATH]=TEXT("");
		GetCurrentDirectory(sizeof(szPath),szPath);

		//创建文件
		TCHAR szFileName[MAX_PATH];
		_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\围棋棋谱_%04ld_%02ld_%02ld_%02ld_%02ld_%02ld.WQM"),
			szPath,SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

		//保存棋谱
		if (SaveChessManual(szFileName)==false)	InsertSystemString(TEXT("围棋游戏“棋谱记录”保存失败"));
		else InsertSystemString(TEXT("围棋游戏“棋谱记录”保存成功"));
	}
	else
	{
		static bool bNotify=false;
		if (bNotify==false)
		{
			bNotify=true;
			InsertSystemString(TEXT("游戏结束了，若需要保存棋谱，请点击 [ 保存 ] 按钮保存棋谱记录"));
		}
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
	WORD wDeadChess[361],wDeadChessCount=0;
	WORD wManualCount=wDataSize/sizeof(tagChessManual);
	for (WORD wIndex=0;wIndex<wManualCount;wIndex++) 
	{
		tagChessManual * pChessManual=(((tagChessManual *)pBuffer)+wIndex);
		m_GameLogic.PlaceChess(pChessManual->cbXPos,pChessManual->cbYPos,pChessManual->cbColor,wDeadChess,wDeadChessCount);
	}

	return true;
}

//请求设置
bool CGameClientDlg::OnSubSetRule(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Req_SetRult));
	if (wDataSize!=sizeof(CMD_S_Req_SetRult)) return false;

	//消息处理
	CMD_S_Req_SetRult * pSetRult=(CMD_S_Req_SetRult *)pBuffer;
	m_GameRuleInfo=pSetRult->GameRuleInfo;

	//设置界面
	if (IsLookonMode()==false)
	{
		//设置变量
		m_bCustomize=true;
		m_bAgreeRule=false;
		m_wBlackUser=GetMeChairID();
		m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

		//设置界面
		if (m_GameClientView.m_btStart.IsWindowEnabled()==FALSE)
		{
			//规则设置
			m_GameClientView.m_GameRule.m_GameRuleInfo=m_GameRuleInfo;

			//显示窗口
			if (m_GameClientView.m_GameRule.GetSafeHwnd()==NULL)
			{
				m_GameClientView.m_GameRule.Create(IDD_GAME_RULE,&m_GameClientView);
			}
			else m_GameClientView.m_GameRule.UpdateControlStatus();

			//设置控件
			m_GameClientView.m_GameRule.ControlEnable(true);
			m_GameClientView.m_GameRule.ShowWindow(SW_SHOW);

			//激活界面
			ActiveGameFrame();
		}
	}

	return true;
}

//请求同意
bool CGameClientDlg::OnSubAgreeRule(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Req_AgreeRult));
	if (wDataSize!=sizeof(CMD_S_Req_AgreeRult)) return false;

	//消息处理
	CMD_S_Req_AgreeRult * pAgreeRult=(CMD_S_Req_AgreeRult *)pBuffer;
	m_GameRuleInfo=pAgreeRult->GameRuleInfo;

	//设置界面
	if (IsLookonMode()==false)
	{
		//设置变量
		m_bAgreeRule=true;
		m_bCustomize=false;

		//设置界面
		if (m_GameClientView.m_btStart.IsWindowEnabled()==FALSE)
		{
			//规则设置
			m_GameClientView.m_GameRule.m_GameRuleInfo=m_GameRuleInfo;

			//显示窗口
			if (m_GameClientView.m_GameRule.GetSafeHwnd()==NULL)
			{
				m_GameClientView.m_GameRule.Create(IDD_GAME_RULE,&m_GameClientView);
			}
			else m_GameClientView.m_GameRule.UpdateControlStatus();

			//设置控件
			m_GameClientView.m_GameRule.ControlEnable(false);
			m_GameClientView.m_GameRule.ShowWindow(SW_SHOW);

			//激活界面
			ActiveGameFrame();
		}
	}

	return true;
}

//请求点目
bool CGameClientDlg::OnSubReqChessCount(const void * pBuffer, WORD wDataSize)
{
	//设置变量
	m_bResponsionChessCount=true;

	//获取点目
	tagCountResult CountResult;
	memset(&CountResult,0,sizeof(CountResult));
	m_GameLogic.GetChessCountInfo(CountResult);

	//计算结果
	float fBlackCount=(float)CountResult.wChessCount[0]-((float)CountResult.wCommomCount)/(float)2.0;
	float fWhiteCount=(float)CountResult.wChessCount[1]-((float)CountResult.wCommomCount)/(float)2.0;
	m_GameClientView.m_ChessBorad.SetChessCountInfo(CountResult.cbCountInfo);
	m_GameClientView.m_ChessCountInfo.SetChessCountInfo(fBlackCount,fWhiteCount,CountResult.wCommomCount);

	//显示窗口
	if (m_GameClientView.m_ChessCountInfo.GetSafeHwnd()==NULL)
	{
		m_GameClientView.m_ChessCountInfo.Create(IDD_CHESS_COUNT,&m_GameClientView);
	}
	m_GameClientView.m_ChessCountInfo.ShowWindow(SW_SHOW);

	//玩家设置
	ActiveGameFrame();
	InsertSystemString(TEXT("对方玩家请求“点目”"));

	return true;
}

//点目应答
bool CGameClientDlg::OnSubChessCountResponsion(const void * pBuffer, WORD wDataSize)
{
	//设置变量
	m_bReqChessCount=false;

	//玩家设置
	InsertSystemString(TEXT("对方玩家不同意你的“点目”请求"));

	return true;
}

//保存棋谱
bool CGameClientDlg::SaveChessManual(LPCTSTR pszFileName)
{
	//打开文件
	CFile FileManual;
	if (FileManual.Open(pszFileName,CFile::modeWrite|CFile::modeCreate)==FALSE) return false;

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

	return true;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	//倒数定时器
	if ((nIDEvent==IDI_GAME_TIMER)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		//设置时间
		if ((m_bLimitTime==true)&&(m_StatusInfo[m_wCurrentUser].wLimitTimeCount!=0xFFFF))
		{
			//递减时间
			m_StatusInfo[m_wCurrentUser].wLimitTimeCount--;

			//时间切换
			if (m_StatusInfo[m_wCurrentUser].wLimitTimeCount==0xFFFF)
			{
				if (m_GameRuleInfo.wRuleTimeOutCount>0)
				{
					//设置变量
					m_GameRuleInfo.wRuleTimeOutCount--;
					m_StatusInfo[m_wCurrentUser].wLimitTimeCount=m_GameRuleInfo.wRuleStepTime;

					//玩家设置
					if ((GetMeChairID()==m_wCurrentUser)&&(IsLookonMode()==false))
					{
						//发送消息
						SendData(SUB_C_TIME_OUT);

						//提示消息
						TCHAR szMessage[128];
						_snprintf(szMessage,sizeof(szMessage),TEXT("下棋超时了，还有 %ld 次 %ld 秒的超时机会"),
							m_GameRuleInfo.wRuleTimeOutCount+1,m_GameRuleInfo.wRuleStepTime);
						InsertSystemString(szMessage);
					}
				}
				else 
				{
					//删除定时器
					KillTimer(IDI_GAME_TIMER);
					m_StatusInfo[m_wCurrentUser].wLimitTimeCount=0;

					//玩家设置
					if ((GetMeChairID()==m_wCurrentUser)&&(IsLookonMode()==false))
					{
						//发送消息
						SendData(SUB_C_TIME_OUT);

						//设置界面
						m_GameClientView.m_btPeace.EnableWindow(FALSE);
						m_GameClientView.m_btRegret.EnableWindow(FALSE);
						m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
						m_GameClientView.m_ChessBorad.CleanCountInfo();
						m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);
						if (m_GameClientView.m_ChessCountInfo.GetSafeHwnd()!=NULL) m_GameClientView.m_ChessCountInfo.DestroyWindow();

						//提示消息
						InsertSystemString(TEXT("下棋超时，你被判输"));
						PlayGameSound(AfxGetInstanceHandle(),"TIME_OUT");
					}
				}
			}

			//播放声音
			if ((m_StatusInfo[m_wCurrentUser].wLimitTimeCount<=5)&&(GetMeChairID()==m_wCurrentUser)&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}
		}
		
		//设置视图
		m_StatusInfo[m_wCurrentUser].wUseTimeCount++;
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(m_wCurrentUser),m_StatusInfo[m_wCurrentUser]);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//游戏变量
	m_wBlackUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_StatusInfo,0,sizeof(m_StatusInfo));

	//重置逻辑
	m_GameLogic.InitGameLogic();

	//设置界面
	m_GameClientView.m_ChessBorad.CleanChess();
	m_GameClientView.m_btStart.EnableWindow(FALSE);
	m_GameClientView.m_btCount.EnableWindow(FALSE);
	m_GameClientView.m_btPreserve.EnableWindow(FALSE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserStatusInfo(0,m_StatusInfo[0]);
	m_GameClientView.SetUserStatusInfo(1,m_StatusInfo[1]);

	//定制规则
	if ((m_bCustomize==true)||(m_bAgreeRule==true))
	{
		//规则设置
		m_GameClientView.m_GameRule.m_GameRuleInfo=m_GameRuleInfo;

		//显示窗口
		if (m_GameClientView.m_GameRule.GetSafeHwnd()==NULL)
		{
			m_GameClientView.m_GameRule.Create(IDD_GAME_RULE,&m_GameClientView);
		}
		else m_GameClientView.m_GameRule.UpdateControlStatus();

		//设置控件
		m_GameClientView.m_GameRule.ShowWindow(SW_SHOW);
		m_GameClientView.m_GameRule.ControlEnable(m_bCustomize);
	}
	else InsertSystemString(TEXT("正等待“黑方”玩家设置游戏规则"));

	//关闭时间
	KillGameTimer(IDI_START_GAME) ;

	return 0;
}

//点目按钮
LRESULT CGameClientDlg::OnCount(WPARAM wParam, LPARAM lParam)
{
	//点目判断
	if (m_GameClientView.m_ChessBorad.IsChessCount()==false)
	{
		//获取点目
		tagCountResult CountResult;
		memset(&CountResult,0,sizeof(CountResult));
		m_GameLogic.GetChessCountInfo(CountResult);

		//设置界面
		m_GameClientView.m_ChessBorad.SetChessCountInfo(CountResult.cbCountInfo);

		//点目统计
		if ((IsLookonMode()==false)&&(m_bGameStart==true))
		{
			//计算结果
			float fBlackCount=(float)CountResult.wChessCount[0]-((float)CountResult.wCommomCount)/(float)2.0;
			float fWhiteCount=(float)CountResult.wChessCount[1]-((float)CountResult.wCommomCount)/(float)2.0;
			m_GameClientView.m_ChessCountInfo.SetChessCountInfo(fBlackCount,fWhiteCount,CountResult.wCommomCount);

			//显示窗口
			if (m_GameClientView.m_ChessCountInfo.GetSafeHwnd()==NULL)
			{
				m_GameClientView.m_ChessCountInfo.Create(IDD_CHESS_COUNT,&m_GameClientView);
			}
			m_GameClientView.m_ChessCountInfo.ShowWindow(SW_SHOW);
		}
	}
	else m_GameClientView.m_ChessBorad.CleanCountInfo();

	return 0;
}

//通过按钮
LRESULT CGameClientDlg::OnPass(WPARAM wParam, LPARAM lParam)
{
	//发送消息
	SendData(SUB_C_PASS_CHESS);

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;

	//界面设置
	m_GameClientView.m_btPass.EnableWindow(FALSE);
	m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CHESS"));

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
	m_GameClientView.m_btRegret.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_ChessBorad.CleanCountInfo();
	m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);
	if (m_GameClientView.m_ChessCountInfo.GetSafeHwnd()!=NULL) m_GameClientView.m_ChessCountInfo.DestroyWindow();

	return 0;
}

//保存按钮
LRESULT CGameClientDlg::OnPreserve(WPARAM wParam, LPARAM lParam)
{
	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//选择文件
	LPCTSTR pszFilter=TEXT("围棋棋谱文件 （*.WQM）|*.WQM||");
	CFileDialog DlgSaveManual(FALSE,TEXT("WQM"),NULL,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter);
	DlgSaveManual.m_pOFN->lpstrInitialDir=szPath;
	if (DlgSaveManual.DoModal()!=IDOK) return 0;

	//保存棋谱
	if (SaveChessManual(DlgSaveManual.GetPathName())==false)	
	{
		InsertSystemString(TEXT("围棋游戏“棋谱记录”保存失败"));
	}
	else InsertSystemString(TEXT("围棋游戏“棋谱记录”保存成功"));

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
		WORD wDeadChess[361],wDeadChessCount=0;
		WORD wIdentifier=m_GameLogic.PlaceChess(cbXPos,cbYPos,cbColor,wDeadChess,wDeadChessCount);
		if ((wIdentifier==0)||(wIdentifier==0xFFFF))
		{
			InsertSystemString(TEXT("这点是死穴或者禁着点，不能在此点放棋"));
			PlayGameSound(AfxGetInstanceHandle(),TEXT("INVALID_PLACE"));
			return 0;
		}

		//设置变量
		m_ShamView=true;
		m_wCurrentUser=INVALID_CHAIR;

		//用户信息
		m_StatusInfo[wMeChairID].wStepCount=m_GameLogic.GetStepCount(cbColor);
		m_StatusInfo[wMeChairID].wTakeChessCount=m_GameLogic.GetTakeChessCount(cbColor);
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(wMeChairID),m_StatusInfo[wMeChairID]);

		//界面设置
		m_GameClientView.m_btPass.EnableWindow(FALSE);
		m_GameClientView.m_ChessBorad.SetSelectMode(SM_NO_SELECT);
		m_GameClientView.m_ChessBorad.SetChess((BYTE)wParam,(BYTE)lParam,cbColor,wIdentifier);
		m_GameClientView.m_ChessBorad.SetChessFocus((BYTE)wParam,(BYTE)lParam);
		if (wDeadChessCount>0) m_GameClientView.m_ChessBorad.CleanChess(wDeadChess,wDeadChessCount);

		//悔棋判断
		WORD wStepCount=m_GameLogic.GetStepCount(cbColor);
		if ((wStepCount==1)&&(m_GameRuleInfo.cbRegretFlag==TRUE)) m_GameClientView.m_btRegret.EnableWindow(TRUE);

		//点目判断
		WORD wBlackStepCount=m_GameLogic.GetStepCount(BLACK_CHESS);
		if (wBlackStepCount==1) m_GameClientView.m_btCount.EnableWindow(TRUE);

		//播放声音
		if (wDeadChessCount==0)	PlayGameSound(AfxGetInstanceHandle(),TEXT("PLACE_CHESS"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_EAT"));

		//发送消息
		CMD_C_PlaceChess PlaceChess;
		PlaceChess.cbXPos=(BYTE)wParam;
		PlaceChess.cbYPos=(BYTE)lParam;
		SendData(SUB_C_PLACE_CHESS,&PlaceChess,sizeof(PlaceChess));
	}

	return 0;
}

//同意规则
LRESULT CGameClientDlg::OnAgreeGameRule(WPARAM wParam, LPARAM lParam)
{
	//规则变量
	m_GameRuleInfo=m_GameClientView.m_GameRule.m_GameRuleInfo;

	//发送同意
	if (m_bCustomize==true)
	{
		//构造数据
		CMD_C_CustomizeRult CustomizeRult;
		CustomizeRult.GameRuleInfo=m_GameRuleInfo;

		//发送消息
		SendUserReady(&CustomizeRult,sizeof(CustomizeRult));
	}
	else SendUserReady(NULL,0);

	return 0;
}

//点目应答
LRESULT CGameClientDlg::OnChessCountResponsion(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	m_GameClientView.m_ChessBorad.CleanCountInfo();

	//同意点目
	if ((wParam==TRUE)&&(m_bReqChessCount==false))
	{
		//设置变量
		m_bReqChessCount=true;

		//发送消息
		SendData(SUB_C_REQ_CHESS_COUNT);

		//提示消息
		InsertSystemString(TEXT("正在对方玩家响应“点目”请求"));
	}

	//拒绝点目
	if ((wParam==FALSE)&&(m_bResponsionChessCount==true))
	{
		//设置变量
		m_bResponsionChessCount=false;

		//发送消息
		SendData(SUB_C_UNAGREE_CHESS_COUNT);

		//提示消息
		InsertSystemString(TEXT("你拒绝了对方的“点目”请求"));
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
