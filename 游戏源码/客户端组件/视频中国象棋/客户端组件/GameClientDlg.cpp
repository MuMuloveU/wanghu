#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_GAME_TIME				100									//游戏定时器
#define IDI_START_GAME				200									//开始定时器

//游戏时间
#define TIME_START_GAME				90									//开始定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)

	//系统消息
	ON_WM_TIMER()

	//控制按钮
	ON_MESSAGE(IDM_START,OnMessageStart)
	ON_MESSAGE(IDM_PEACE,OnMessagePeace)
	ON_MESSAGE(IDM_REGRET,OnMessageRegret)
	ON_MESSAGE(IDM_GIVEUP,OnMessageGiveUp)
	ON_MESSAGE(IDM_MANUAL,OnMessageManual)
	ON_MESSAGE(IDM_PRESERVE,OnMessagePreserve)

	//控件消息
	ON_MESSAGE(WM_HIT_CHESS_BORAD, OnHitChessBorad)
	ON_MESSAGE(IDM_GAME_RULE_AGREE, OnGameRuleAgree)
	ON_MESSAGE(IDM_GAME_RULE_REJECT, OnGameRuleReject)
	ON_MESSAGE(IDM_ORIENTATION_MANUAL, OnOrientationManual)
	ON_MESSAGE(IDM_USER_REQUEST_RESPOND, OnUserRequestRespond)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg(): CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_cbChessColor=CHESS_BLACK;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//选择信息
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;

	//辅助变量
	m_bDrawTimeMode=false;
	m_bRequestAgree=false;
	m_bRequestCustomize=false;
	ZeroMemory(m_szUserAccounts,sizeof(m_szUserAccounts));

	//规则变量
	m_wRuleRoundID=0L;
	ZeroMemory(&m_GameRuleInfo,sizeof(m_GameRuleInfo));

	//状态变量
	m_wViewStepCount=0;
	ZeroMemory(&m_UserStatusInfo,sizeof(m_UserStatusInfo));

	//辅助变量
	ZeroMemory(m_ChessItemView,sizeof(m_ChessItemView));
	ZeroMemory(m_ChessBoradView,sizeof(m_ChessBoradView));

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
	SetWindowText(TEXT("中国象棋  --  Ver：6.0.1.0"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	////内核初始
	//if (__super::InitGameEngine()==false) return false;

	//设置逻辑
	m_GameLogic.ResetChessBorad();

	return true;
}

//重置引擎
void CGameClientDlg::ResetGameFrame()
{
	//内核重置
	//if (__super::ResetGameEngine()==false) return false;

	//删除时间
	KillTimer(IDI_GAME_TIME);
	KillGameTimer(IDI_START_GAME);

	//游戏变量
	m_cbChessColor=CHESS_BLACK;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//选择信息
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;

	//辅助变量
	m_bDrawTimeMode=false;
	m_bRequestAgree=false;
	m_bRequestCustomize=false;
	ZeroMemory(m_szUserAccounts,sizeof(m_szUserAccounts));

	//游戏逻辑
	m_GameLogic.ResetChessBorad();

	//规则变量
	m_wRuleRoundID=0L;
	ZeroMemory(&m_GameRuleInfo,sizeof(m_GameRuleInfo));

	//状态变量
	m_wViewStepCount=0;
	ZeroMemory(&m_UserStatusInfo,sizeof(m_UserStatusInfo));

	//辅助变量
	ZeroMemory(m_ChessItemView,sizeof(m_ChessItemView));
	ZeroMemory(m_ChessBoradView,sizeof(m_ChessBoradView));

	return ;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bAllowLookon=IsAllowUserLookon();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置配置
		EnableSound(GameOption.m_bEnableSound);
		AllowUserLookon(0,GameOption.m_bAllowLookon);
	}

	return;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//游戏开始
		{
			//自动处理
			if (nElapse==0)
			{
				//离开游戏
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) AfxGetMainWnd()->PostMessage(WM_CLOSE);

				return true;
			}

			//超时警告
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			//更新界面
			m_GameClientView.UpdateGameView(NULL);

			return true;
		}
	}

	return false;
}

//网络消息
bool  CGameClientDlg::OnGameMessage(WORD wSubCmdID,const VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_MOVE_CHESS:		//移动棋子
		{
			return OnSubMoveChess(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_REGRET_FAILE:	//悔棋失败
		{
			return OnSubRegretFaile(pData,wDataSize);
		}
	case SUB_S_REGRET_RESULT:	//悔棋结果
		{
			return OnSubRegretResult(pData,wDataSize);
		}
	case SUB_S_REGRET_REQUEST:	//悔棋请求
		{
			return OnSubRegretRequest(pData,wDataSize);
		}
	case SUB_S_PEACE_RESPOND:	//和棋响应
		{
			return OnSubPeaceRespond(pData,wDataSize);
		}
	case SUB_S_PEACE_REQUEST:	//和棋请求
		{
			return OnSubPeaceRequest(pData,wDataSize);
		}
	case SUB_S_REQUEST_RULE:	//请求设置
		{
			return OnSubRequestRule(pData,wDataSize);
		}
	case SUB_S_REQUEST_AGREE:	//请求同意
		{
			return OnSubRequestAgree(pData,wDataSize);
		}
	case SUB_S_CHESS_MANUAL:	//游戏棋谱
		{
			return OnSubChessManual(pData,wDataSize);
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏场景
bool  CGameClientDlg::OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther,const VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//变量定义
			WORD wMeChairID=GetMeChairID();
			bool bPlayerMode=(IsLookonMode()==false);

			//设置变量
			m_wBankerUser=pStatusFree->wBankerUser;
			m_wCurrentUser=pStatusFree->wBankerUser;
			m_GameRuleInfo=pStatusFree->GameRuleInfo;

			//辅助变量
			m_bRequestAgree=((m_wBankerUser!=wMeChairID)&&(pStatusFree->cbGameRule==TRUE));
			m_bRequestCustomize=((m_wBankerUser==wMeChairID)&&(pStatusFree->cbGameRule==FALSE));

			//设置界面
			m_GameClientView.SetGameRuleInfo(m_GameRuleInfo);
			m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

			//设置规则
			if ((bPlayerMode==true)&&((m_bRequestAgree==true)||(m_bRequestCustomize==true)))
			{
				//规则设置
				m_GameClientView.m_DlgRuleOption.SetGameRuleInfo(m_GameRuleInfo,m_bRequestCustomize,true);

				//窗口窗口
				CDlgRuleOption * pDlgRuleOption=&m_GameClientView.m_DlgRuleOption;
				if (pDlgRuleOption->m_hWnd==NULL) pDlgRuleOption->Create(IDD_RULE_OPTION,&m_GameClientView);

				//显示窗口
				pDlgRuleOption->ShowWindow(SW_SHOW);
				pDlgRuleOption->SetForegroundWindow();
			}
			else
			{
				//提示消息
				if (bPlayerMode==false)
				{
					m_GameClientView.m_ChessBorad.SetWaitGameRule(true);
					InsertSystemString(TEXT("正等待玩家设置游戏规则"));
				}
				else
				{
					m_GameClientView.m_ChessBorad.SetWaitGameRule(true);
					InsertSystemString(TEXT("正等待“红方”玩家设置游戏规则"));
				}

				//更新界面
				m_GameClientView.UpdateGameView(NULL);
			}

			return true;
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//变量定义
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//变量定义
			WORD wMeChairID=GetMeChairID();
			bool bPlayerMode=(IsLookonMode()==false);

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbChessColor=(m_wBankerUser==wMeChairID)?CHESS_BLACK:CHESS_WHITE;

			//规则变量
			m_GameRuleInfo=pStatusPlay->GameRuleInfo;
			m_bDrawTimeMode=(pStatusPlay->wLeaveDrawTime[m_wCurrentUser]>0);

			//设置界面
			m_GameClientView.m_btPreserve.EnableWindow(TRUE);
			m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));
			m_GameClientView.SetCurrentUser(SwitchViewChairID(m_wCurrentUser));

			//棋盘界面
			m_GameClientView.m_ChessBorad.SetChessColor(m_cbChessColor);
			m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);

			//设置棋谱
			UpdateManualControl((WORD)m_GameLogic.m_ChessManualArray.GetCount());

			//设置路线
			tagChessManual * pChessManual=m_GameLogic.GetLastChessManual();
			if (pChessManual!=NULL)
			{
				BYTE cbXPosStart=pChessManual->cbXSourceChessPos;
				BYTE cbYPosStart=pChessManual->cbYSourceChessPos;
				BYTE cbXPosTerminal=pChessManual->cbXTargetChessPos;
				BYTE cbYPosTerminal=pChessManual->cbYTargetChessPos;
				m_GameClientView.SetMoveChessTrace(cbXPosStart,cbYPosStart,cbXPosTerminal,cbYPosTerminal);
			}

			//用户信息
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_UserStatusInfo[i].wDrawTimeCount=pStatusPlay->wLeaveDrawTime[i];
				m_UserStatusInfo[i].wStepTimeCount=pStatusPlay->GameRuleInfo.wRuleStepTime;
				m_UserStatusInfo[i].wSecondTimeCount=pStatusPlay->GameRuleInfo.wRuleSecondTime;
				m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_UserStatusInfo[i]);
			}

			//保存用户
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				const tagUserData * pUserData=GetUserData(i);
				//IClientUserItem * pIClientUserItem=GetTableUserItem(i);
				lstrcpyn(m_szUserAccounts[i],pUserData->szName,CountArray(m_szUserAccounts[i]));
			}

			//玩家设置
			if (bPlayerMode==true)
			{
				//变量定义
				BYTE cbMyselfStatus=pStatusPlay->cbRequestStatus[wMeChairID];
				BYTE cbEnemyStatus=pStatusPlay->cbRequestStatus[(wMeChairID+1)%GAME_PLAYER];
				WORD wStepCount=m_GameLogic.GetStepCount((wMeChairID==m_wBankerUser)?CHESS_BLACK:CHESS_WHITE);

				//按钮设置
				m_GameClientView.m_btGiveUp.EnableWindow(TRUE);
				m_GameClientView.m_btPeace.EnableWindow(((cbMyselfStatus&GU_WAIT_PEACE)==0)?TRUE:FALSE);
				m_GameClientView.m_btRegret.EnableWindow(((cbMyselfStatus&GU_WAIT_REGRET)==0)?TRUE:FALSE);

				//悔棋按钮
				if ((wStepCount>0)&&(m_GameRuleInfo.cbRegretFlag==TRUE))
				{
					bool bAllowRegret=((cbMyselfStatus&GU_WAIT_REGRET)==0);
					m_GameClientView.m_btRegret.EnableWindow((bAllowRegret==true)?TRUE:FALSE);
				}

				//和棋请求
				if (cbEnemyStatus&GU_WAIT_PEACE)
				{
					//创建窗口
					CDlgUserRequest * pDlgUserRequest=&m_GameClientView.m_UserRequestPeace;
					if (pDlgUserRequest->m_hWnd==NULL) pDlgUserRequest->Create(IDD_USER_REQUEST,&m_GameClientView);

					//显示窗口
					pDlgUserRequest->ShowWindow(SW_SHOW);
					pDlgUserRequest->SetForegroundWindow();
				}

				//悔棋请求
				if (cbEnemyStatus&GU_WAIT_REGRET) 
				{
					//创建窗口
					CDlgUserRequest * pDlgUserRequest=&m_GameClientView.m_UserRequestPeace;
					if (pDlgUserRequest->m_hWnd==NULL) pDlgUserRequest->Create(IDD_USER_REQUEST,&m_GameClientView);

					//显示窗口
					pDlgUserRequest->ShowWindow(SW_SHOW);
					pDlgUserRequest->SetForegroundWindow();
				}

				//操作设置
				if (wMeChairID==m_wCurrentUser) m_GameClientView.m_ChessBorad.SetPositively(true);
			}

			//设置时间
			SetTimer(IDI_GAME_TIME,1000,NULL);

			//更新界面
			m_GameClientView.UpdateGameView(NULL);

			return true;
		}
	}

	return false;
}


//更新按钮
bool CGameClientDlg::UpdateManualControl(WORD wViewStepCount)
{
	//设置变量
	m_wViewStepCount=wViewStepCount;
	WORD wManualCount=(WORD)m_GameLogic.m_ChessManualArray.GetCount();

	//设置控件
	m_GameClientView.m_btManualHead.EnableWindow(m_wViewStepCount>0);
	m_GameClientView.m_btManualLast.EnableWindow(m_wViewStepCount>0);
	m_GameClientView.m_btManualNext.EnableWindow(m_wViewStepCount<wManualCount);
	m_GameClientView.m_btManualTail.EnableWindow(m_wViewStepCount<wManualCount);

	//棋谱列表
	m_GameClientView.m_ChessBorad.SetManualView(m_wViewStepCount<wManualCount);
	m_GameClientView.m_ManualList.SetCurSel((m_wViewStepCount>0)?m_wViewStepCount-1:LB_ERR);

	return true;
}

//执行走棋
bool CGameClientDlg::PerformMoveChess(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//获取棋子
	tagChessItem * pChessItem=NULL;
	pChessItem=m_GameLogic.GetChessItem(cbXTargetPos,cbYTargetPos);

	//移动棋子
	TCHAR szManualName[32]=TEXT("");
	tagMoveChessResult MoveChessResult;
	m_GameLogic.MoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos,MoveChessResult,szManualName);

	//走棋界面
	m_GameClientView.m_ManualList.AddString(szManualName);
	m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);
	m_GameClientView.SetMoveChessTrace(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos);

	//更新棋谱
	UpdateManualControl((WORD)m_GameLogic.m_ChessManualArray.GetCount());

	//播放声音
	if (MoveChessResult.bMinatoryKing==false)
	{
		if (pChessItem!=NULL) PlayGameSound(AfxGetInstanceHandle(),TEXT("EAT_CHESS"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("MOVE_CHESS"));
	}
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("GENERAL")); 

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//旁观设置
	if(IsLookonMode())
	{
		m_GameClientView.m_ChessBorad.SetWaitGameRule(false);
	}

	//游戏变量
	m_wBankerUser=pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wBankerUser;
	m_GameRuleInfo=pGameStart->GameRuleInfo;
	m_cbChessColor=(m_wBankerUser==GetMeChairID())?CHESS_BLACK:CHESS_WHITE;

	//选择信息
	m_cbXSourcePos=INVALID_POS;
	m_cbYSourcePos=INVALID_POS;

	//辅助变量
	m_bRequestAgree=false;
	m_bRequestCustomize=false;
	m_bDrawTimeMode=(m_GameRuleInfo.wRuleStepTime>0);

	//保存用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		lstrcpyn(m_szUserAccounts[i],pUserData->szName,CountArray(m_szUserAccounts[i]));
	}

	//状态变量
	m_wViewStepCount=0;
	ZeroMemory(&m_UserStatusInfo,sizeof(m_UserStatusInfo));

	//设置棋盘
	m_GameLogic.ResetChessBorad();
	m_GameClientView.m_ChessBorad.SetChessColor(m_cbChessColor);
	m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);
	m_GameClientView.SetMoveChessTrace(INVALID_POS,INVALID_POS,INVALID_POS,INVALID_POS);

	//棋谱控件
	UpdateManualControl(0);
	m_GameClientView.m_ManualList.ResetContent();

	//视图控件
	m_GameClientView.m_btPreserve.EnableWindow(TRUE);
	m_GameClientView.m_ScoreControl.RestorationData();

	//设置界面
	m_GameClientView.SetGameRuleInfo(m_GameRuleInfo);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));
	m_GameClientView.SetCurrentUser(SwitchViewChairID(m_wCurrentUser));

	//用户状态
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_UserStatusInfo[i].wDrawTimeCount=m_GameRuleInfo.wRuleDrawTime;
		m_UserStatusInfo[i].wStepTimeCount=m_GameRuleInfo.wRuleStepTime;
		m_UserStatusInfo[i].wSecondTimeCount=m_GameRuleInfo.wRuleSecondTime;
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_UserStatusInfo[i]);
	}

	//按钮设置
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
		m_GameClientView.m_ChessBorad.SetPositively(true);
	}

	//设置定时器
	SetTimer(IDI_GAME_TIME,1000,NULL);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//移动棋子
bool CGameClientDlg::OnSubMoveChess(const VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_MoveChess));
	if (wDataSize!=sizeof(CMD_S_MoveChess)) return false;

	//消息处理
	CMD_S_MoveChess * pMoveChess=(CMD_S_MoveChess *)pData;

	//清理窗口
	CDlgHint * pDlgUserHint=&m_GameClientView.m_UserHint;
	if (pDlgUserHint->m_hWnd!=NULL) pDlgUserHint->DestroyWindow();

	//设置用户
	m_wCurrentUser=pMoveChess->wCurrentUser;
	m_GameClientView.SetCurrentUser(SwitchViewChairID(m_wCurrentUser));

	//时间模式
	if (m_wCurrentUser!=INVALID_CHAIR) m_bDrawTimeMode=(m_UserStatusInfo[m_wCurrentUser].wDrawTimeCount>0);

	//移动棋子
	if ((IsLookonMode()==true)||(pMoveChess->wActionUser!=GetMeChairID()))
	{
		//变量定义
		WORD wActionUser=pMoveChess->wActionUser;
		WORD wStepTimeCount=__min(m_GameRuleInfo.wRuleStepTime,pMoveChess->wLeaveDrawTime);

		//走棋界面
		BYTE cbXSourcePos=pMoveChess->cbXSourcePos;
		BYTE cbYSourcePos=pMoveChess->cbYSourcePos;
		BYTE cbXTargetPos=pMoveChess->cbXTargetPos;
		BYTE cbYTargetPos=pMoveChess->cbYTargetPos;
		PerformMoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos);

		//设置时间
		m_UserStatusInfo[wActionUser].wStepTimeCount=wStepTimeCount;
		m_UserStatusInfo[wActionUser].wDrawTimeCount=pMoveChess->wLeaveDrawTime;
		m_UserStatusInfo[wActionUser].wSecondTimeCount=m_GameRuleInfo.wRuleSecondTime;
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(wActionUser),m_UserStatusInfo[wActionUser]);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		//设置变量
		m_cbXSourcePos=255;
		m_cbYSourcePos=255;

		//设置界面
		ActiveGameFrame();
		m_GameClientView.m_ChessBorad.SetPositively(true);
		m_GameClientView.SelectChessItem(m_cbXSourcePos,m_cbYSourcePos);
	}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//悔棋失败
bool CGameClientDlg::OnSubRegretFaile(const VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RegretFaile));
	if (wDataSize!=sizeof(CMD_S_RegretFaile)) return false;

	//消息处理
	CMD_S_RegretFaile * pRegretFaile=(CMD_S_RegretFaile *)pData;

	//对家反对
	if (pRegretFaile->cbFaileReason==FR_PLAYER_OPPOSE)
	{
		m_GameClientView.m_btRegret.EnableWindow(TRUE);
		InsertSystemString(TEXT("对家拒绝了您的“悔棋”请求"));

		return true;
	}

	return true;
}

//悔棋结果
bool CGameClientDlg::OnSubRegretResult(const VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RegretResult));
	if (wDataSize!=sizeof(CMD_S_RegretResult)) return false;

	//变量定义
	CMD_S_RegretResult * pRegretResult=(CMD_S_RegretResult *)pData;

	//设置变量
	m_wCurrentUser=pRegretResult->wCurrentUser;
	m_bDrawTimeMode=(m_UserStatusInfo[m_wCurrentUser].wDrawTimeCount>0);

	//设置用户
	m_GameClientView.SetCurrentUser(SwitchViewChairID(m_wCurrentUser));

	//悔棋逻辑
	m_GameLogic.RegretChess(pRegretResult->wTargetStep);

	//设置棋盘
	m_GameClientView.m_ChessBorad.SetManualView(false);
	m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);
	m_GameClientView.m_ChessBorad.SelectChessItem(INVALID_POS,INVALID_POS);

	//设置框架
	if (m_GameLogic.m_ChessManualArray.GetCount()>0) 
	{
		tagChessManual * pChessManual=m_GameLogic.GetLastChessManual();
		m_GameClientView.SetMoveChessTrace(pChessManual->cbXSourceChessPos,pChessManual->cbYSourceChessPos,
			pChessManual->cbXTargetChessPos,pChessManual->cbYTargetChessPos);
	}
	else m_GameClientView.m_ChessBorad.SetMoveChessTrace(INVALID_POS,INVALID_POS,INVALID_POS,INVALID_POS);

	//更新棋谱
	do
	{
		//获取变量
		INT nListCount=m_GameClientView.m_ManualList.GetCount();

		//删除棋谱
		if (nListCount>pRegretResult->wTargetStep) 
		{
			m_GameClientView.m_ManualList.DeleteString(nListCount-1);
			continue;
		}

		//更新棋谱
		UpdateManualControl(pRegretResult->wTargetStep);

		break;

	} while (true);

	//设置时间
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置时间
		m_UserStatusInfo[i].wDrawTimeCount=pRegretResult->wLeaveDrawTime[i];
		m_UserStatusInfo[i].wSecondTimeCount=m_GameRuleInfo.wRuleSecondTime;
		m_UserStatusInfo[i].wStepTimeCount=__min(pRegretResult->wLeaveDrawTime[i],m_GameRuleInfo.wRuleStepTime);

		//设置界面
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_UserStatusInfo[i]);
	}

	//玩家设置
	WORD wMeChairID=GetMeChairID();
	if ((IsLookonMode()==false)&&(pRegretResult->wCurrentUser==wMeChairID))
	{
		//设置变量
		m_cbXSourcePos=INVALID_POS;
		m_cbYSourcePos=INVALID_POS;

		//设置界面
		ActiveGameFrame();
		m_GameClientView.m_ChessBorad.SetPositively(true);
		m_GameClientView.SelectChessItem(INVALID_POS,INVALID_POS);
	}

	//按钮控制
	if (IsLookonMode()==false)
	{
		WORD wStepCount=m_GameLogic.GetStepCount(m_cbChessColor);
		if (wStepCount==0) m_GameClientView.m_btRegret.EnableWindow(FALSE);
		if ((wStepCount>0)&&(pRegretResult->wRegretUser==wMeChairID)) m_GameClientView.m_btRegret.EnableWindow(TRUE);
	}

	//更新视图
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//悔棋请求
bool CGameClientDlg::OnSubRegretRequest(const VOID * pData, WORD wDataSize)
{
	//创建窗口
	CDlgUserRequest * pDlgUserRequest=&m_GameClientView.m_UserRequestRegret;
	if (pDlgUserRequest->m_hWnd==NULL) pDlgUserRequest->Create(IDD_USER_REQUEST,&m_GameClientView);

	//显示窗口
	pDlgUserRequest->ShowWindow(SW_SHOW);
	pDlgUserRequest->SetForegroundWindow();

	return true;
}

//和棋响应
bool CGameClientDlg::OnSubPeaceRespond(const VOID * pData, WORD wDataSize)
{
	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(TRUE);

	//提示消息
	InsertSystemString(TEXT("对家拒绝了您的“和棋”请求"));

	return true;
}

//和棋请求
bool CGameClientDlg::OnSubPeaceRequest(const VOID * pData, WORD wDataSize)
{
	//创建窗口
	CDlgUserRequest * pDlgUserRequest=&m_GameClientView.m_UserRequestPeace;
	if (pDlgUserRequest->m_hWnd==NULL) pDlgUserRequest->Create(IDD_USER_REQUEST,&m_GameClientView);

	//显示窗口
	pDlgUserRequest->ShowWindow(SW_SHOW);
	pDlgUserRequest->SetForegroundWindow();

	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//清理窗口
	CDlgHint * pDlgUserHint=&m_GameClientView.m_UserHint;
	if (pDlgUserHint->m_hWnd!=NULL) pDlgUserHint->DestroyWindow();

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

	//选择变量
	m_cbXSourcePos=INVALID_POS;
	m_cbYSourcePos=INVALID_POS;

	//设置变量
	m_bRequestAgree=false;
	m_bRequestCustomize=false;
	m_wCurrentUser=INVALID_CHAIR;

	//变量定义
	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

	//结束原因
	ScoreInfo.wMeChiarID = GetMeChairID();
	ScoreInfo.cbReason=pGameEnd->cbReason;
	if(m_GameLogic.GetStepCount(CHESS_WHITE)<5)ScoreInfo.cbReason=REASON_TEN_STEP;
	else if(m_GameLogic.GetStepCount(CHESS_WHITE)<10)ScoreInfo.cbReason=REASON_TWENTY_STEP;

	//成绩用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		const tagUserData * pUserData=GetUserData(i);
		if (pUserData==NULL) continue;

		//设置成绩
		ScoreInfo.lGameScore[i]=pGameEnd->lGameScore[i];

		//计算胜率
		//if(ScoreInfo.cbReason<REASON_TEN_STEP)
		//{
		//	FLOAT fPlayCount = FLOAT(pUserData->lFleeCount+pUserData->lWinCount+pUserData->lDrawCount+pUserData->lLostCount)+1;
		//	FLOAT fWinCount = FLOAT(pUserData->lWinCount);
		//	if(pGameEnd->lGameScore[i]>=0 && pGameEnd->cbReason<REASON_AGREE_DRAW)fWinCount++;
		//	ScoreInfo.fWinRate[i] = fWinCount/fPlayCount*100;
		//}
		//else 
		{
			FLOAT fPlayCount = FLOAT(pUserData->lWinCount+pUserData->lDrawCount+pUserData->lLostCount+pUserData->lFleeCount);
			FLOAT fWinCount = FLOAT(pUserData->lWinCount);
			if((INT)fPlayCount==0) ScoreInfo.fWinRate[i] = 0.0f;
			else ScoreInfo.fWinRate[i] = fWinCount/fPlayCount*100;
		}
		//lstrcpyn(ScoreInfo.szLevelName[i],m_GameClientView.GetLevelDescribe(pIClientUserItem),CountArray(ScoreInfo.szLevelName[i]));

		//设置名字
		//CW2CT strAccounts(pUserData->szName);
		lstrcpyn(ScoreInfo.szUserName[i],(LPCTSTR)pUserData->szName,CountArray(ScoreInfo.szUserName[i]));
	}

	//成绩界面
	m_GameClientView.m_ScoreControl.SetScoreInfo(ScoreInfo);

	//播放声音
	LONGLONG lMeScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOSE"));
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));

	//设置界面
	if (IsLookonMode()==false)
	{
		//设置界面
		ActiveGameFrame();
		m_GameClientView.m_btStart.EnableWindow(TRUE);
		m_GameClientView.m_btPeace.EnableWindow(FALSE);
		m_GameClientView.m_btRegret.EnableWindow(FALSE);
		m_GameClientView.m_btGiveUp.EnableWindow(FALSE);

		//开始时间
		SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);

		//设置棋盘
		m_GameClientView.m_ChessBorad.SetPositively(false);
		m_GameClientView.SetMoveChessTrace(INVALID_POS,INVALID_POS,INVALID_POS,INVALID_POS);

		//关闭窗口
		if (m_GameClientView.m_UserRequestPeace.m_hWnd!=NULL) m_GameClientView.m_UserRequestPeace.DestroyWindow();
		if (m_GameClientView.m_UserRequestRegret.m_hWnd!=NULL) m_GameClientView.m_UserRequestRegret.DestroyWindow();
	}

	//设置界面
	m_GameClientView.SetCurrentUser(INVALID_CHAIR);

	//删除定时器
	KillTimer(IDI_GAME_TIME);

	//保存棋谱
/*	if (m_bAutoSaveManual==true)
	{
		//获取时间
		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);

		//获取目录
		TCHAR szPath[MAX_PATH]=TEXT("");
		GetCurrentDirectory(sizeof(szPath),szPath);

		//创建文件
		TCHAR szFileName[MAX_PATH];
		_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\中国象棋棋谱_%04ld_%02ld_%02ld_%02ld_%02ld_%02ld.CCM"),
			szPath,SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

		//保存棋谱
		if (SaveChessManual(szFileName)==false)	InsertSystemString(TEXT("中国象棋游戏“棋谱记录”保存失败"));
		else InsertSystemString(TEXT("中国象棋游戏“棋谱记录”保存成功"));
	}
	else
	{
		static bool bNotify=false;
		if (bNotify==false)
		{
			bNotify=true;
			InsertSystemString(TEXT("游戏结束了，若需要保存棋谱，请点击 [ 保存 ] 按钮保存棋谱记录"));
		}
	}*/

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//请求设置
bool CGameClientDlg::OnSubRequestRule(const VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RequestRule));
	if (wDataSize!=sizeof(CMD_S_RequestRule)) return false;

	//变量定义
	CMD_S_RequestRule * pRequestRule=(CMD_S_RequestRule *)pData;

	//设置变量
	m_wRuleRoundID=pRequestRule->wRuleRoundID;
	m_GameRuleInfo=pRequestRule->GameRuleInfo;

	//设置界面
	if (IsLookonMode()==false)
	{
		//设置变量
		m_bRequestAgree=false;
		m_bRequestCustomize=true;
		m_wBankerUser=GetMeChairID();

		//设置界面
		if (m_GameClientView.m_btStart.IsWindowEnabled()==FALSE)
		{
			//设置界面
			m_GameClientView.SetGameRuleInfo(m_GameRuleInfo);
			m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

			//设置控件
			m_GameClientView.m_ChessBorad.SetWaitGameRule(false);
			m_GameClientView.m_DlgRuleOption.SetGameRuleInfo(m_GameRuleInfo,m_bRequestCustomize,true);

			//窗口窗口
			CDlgRuleOption * pDlgRuleOption=&m_GameClientView.m_DlgRuleOption;
			if (pDlgRuleOption->m_hWnd==NULL) pDlgRuleOption->Create(IDD_RULE_OPTION,&m_GameClientView);

			//显示窗口
			pDlgRuleOption->ShowWindow(SW_SHOW);
			pDlgRuleOption->SetForegroundWindow();

			//更新界面
			m_GameClientView.UpdateGameView(NULL);
		}
	}

	return true;
}

//请求同意
bool CGameClientDlg::OnSubRequestAgree(const VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RequestRule));
	if (wDataSize!=sizeof(CMD_S_RequestRule)) return false;

	//变量定义
	CMD_S_RequestRule * pRequestRule=(CMD_S_RequestRule *)pData;

	//设置变量
	m_wRuleRoundID=pRequestRule->wRuleRoundID;
	m_GameRuleInfo=pRequestRule->GameRuleInfo;

	//设置界面
	if (IsLookonMode()==false)
	{
		//设置变量
		m_bRequestAgree=true;
		m_bRequestCustomize=false;
		m_wBankerUser=(GetMeChairID()+1)%GAME_PLAYER;

		//设置界面
		if (m_GameClientView.m_btStart.IsWindowEnabled()==FALSE)
		{
			//设置界面
			m_GameClientView.SetGameRuleInfo(m_GameRuleInfo);
			m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

			//设置控件
			m_GameClientView.m_ChessBorad.SetWaitGameRule(false);
			m_GameClientView.m_DlgRuleOption.SetGameRuleInfo(m_GameRuleInfo,m_bRequestCustomize,true);

			//窗口窗口
			CDlgRuleOption * pDlgRuleOption=&m_GameClientView.m_DlgRuleOption;
			if (pDlgRuleOption->m_hWnd==NULL) pDlgRuleOption->Create(IDD_RULE_OPTION,&m_GameClientView);

			//显示窗口
			pDlgRuleOption->ShowWindow(SW_SHOW);
			pDlgRuleOption->SetForegroundWindow();

			//更新界面
			m_GameClientView.UpdateGameView(NULL);
		}
	}

	return true;
}

//游戏棋谱
bool CGameClientDlg::OnSubChessManual(const VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagChessManual)==0);
	if (wDataSize%sizeof(tagChessManual)!=0) return false;

	//变量定义
	WORD wManualCount=wDataSize/sizeof(tagChessManual);
	tagChessManual * pChessManual=(tagChessManual *)pData;

	//设置棋谱
	for (WORD i=0;i<wManualCount;i++) 
	{
		//变量定义
		tagChessManual * pChessManualItem=(pChessManual+i);
		BYTE cbXSourcePos=pChessManualItem->cbXSourceChessPos;
		BYTE cbYSourcePos=pChessManualItem->cbYSourceChessPos;
		BYTE cbXTargetPos=pChessManualItem->cbXTargetChessPos;
		BYTE cbYTargetPos=pChessManualItem->cbYTargetChessPos;

		//设置棋子
		TCHAR szManualName[32]=TEXT("");
		tagMoveChessResult MoveChessResult;
		m_GameLogic.MoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos,MoveChessResult,szManualName);

		//棋谱界面
		m_GameClientView.m_ManualList.AddString(szManualName);
	}

	return true;
}

//开始消息
LRESULT CGameClientDlg::OnMessageStart(WPARAM wParam, LPARAM lParam)
{
	//选择信息
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;

	//游戏变量
	m_cbChessColor=CHESS_BLACK;
	m_wCurrentUser=INVALID_CHAIR;

	//状态变量
	m_wViewStepCount=0;
	ZeroMemory(&m_UserStatusInfo,sizeof(m_UserStatusInfo));

	//删除时间
	KillGameTimer(IDI_START_GAME);

	//设置棋盘
	m_GameLogic.ResetChessBorad();
	m_GameClientView.m_ChessBorad.CleanChessItem();
	m_GameClientView.m_ChessBorad.SetPositively(false);

	//棋谱控件
	UpdateManualControl(0);
	m_GameClientView.m_ManualList.ResetContent();

	//视图界面
	m_GameClientView.m_btStart.EnableWindow(FALSE);
	m_GameClientView.m_btPreserve.EnableWindow(FALSE);
	m_GameClientView.m_ScoreControl.RestorationData();

	//界面设置
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.SetUserStatusInfo(i,m_UserStatusInfo[i]);

	//定制规则
	if ((m_bRequestCustomize==true)||(m_bRequestAgree==true))
	{
		//界面设置
		m_GameClientView.SetGameRuleInfo(m_GameRuleInfo);
		m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));
		m_GameClientView.m_DlgRuleOption.SetGameRuleInfo(m_GameRuleInfo,m_bRequestCustomize,true);

		//窗口窗口
		CDlgRuleOption * pDlgRuleOption=&m_GameClientView.m_DlgRuleOption;
		if (pDlgRuleOption->m_hWnd==NULL) pDlgRuleOption->Create(IDD_RULE_OPTION,&m_GameClientView);

		//显示窗口
		pDlgRuleOption->ShowWindow(SW_SHOW);
		pDlgRuleOption->SetForegroundWindow();
	}
	else
	{
		//界面设置
		m_wBankerUser=(GetMeChairID()+1)%GAME_PLAYER;
		m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

		//提示消息
		//ASSERT(m_pIStringMessage!=NULL);
		m_GameClientView.m_ChessBorad.SetWaitGameRule(true);
		InsertSystemString(TEXT("正等待“红方”玩家设置游戏规则"));
	}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return 0;
}

//求和消息
LRESULT CGameClientDlg::OnMessagePeace(WPARAM wParam, LPARAM lParam)
{
	//发送消息
	SendData(SUB_C_PEACE_REQUEST);

	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(FALSE);
	InsertSystemString(TEXT("正在等待对家回应 [ 和棋 ] 请求..."));

	return 0;
}

//悔棋消息		 
LRESULT CGameClientDlg::OnMessageRegret(WPARAM wParam, LPARAM lParam)
{
	//发送消息
	SendData(SUB_C_REGRET_REQUEST);

	//设置界面
	m_GameClientView.m_btRegret.EnableWindow(FALSE);
	InsertSystemString(TEXT("正在等待对家回应 [ 悔棋 ] 请求..."));

	return 0;
}

//认输消息		 
LRESULT CGameClientDlg::OnMessageGiveUp(WPARAM wParam, LPARAM lParam)
{
	//删除时间
	KillTimer(IDI_GAME_TIME);

	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(FALSE);
	m_GameClientView.m_btRegret.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_ChessBorad.SetPositively(false);

	//变量定义
	CMD_C_ConcludeRequest ConcludeRequest;
	ZeroMemory(&ConcludeRequest,sizeof(ConcludeRequest));

	//发送消息
	ConcludeRequest.cbReason=REASON_GIVE_UP;
	SendData(SUB_C_CONCLUDE_REQUEST,&ConcludeRequest,sizeof(ConcludeRequest));

	return 0;
}

//棋谱消息
LRESULT CGameClientDlg::OnMessageManual(WPARAM wParam, LPARAM lParam)
{
	//创建窗口
	if (m_GameClientView.m_DlgChessManual.m_hWnd==NULL)
	{
		m_GameClientView.m_DlgChessManual.Create(IDD_CHESS_MANUAL,GetDesktopWindow());
	}

	//显示窗口
	m_GameClientView.m_DlgChessManual.ShowWindow(SW_SHOW);
	m_GameClientView.m_DlgChessManual.SetForegroundWindow();

	return 0;
}

//保存消息		 
LRESULT CGameClientDlg::OnMessagePreserve(WPARAM wParam, LPARAM lParam)
{
	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	//获取目录
	TCHAR szFilePath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(CountArray(szFilePath),szFilePath);

	//构造名字
	TCHAR szManualName[MAX_PATH]=TEXT("");
	_sntprintf(szManualName,CountArray(szManualName),TEXT("%s_%s_%02d_%02d.CML"),(m_szUserAccounts[0]),
		(m_szUserAccounts[1]),SystemTime.wMonth,SystemTime.wDay);

	//选择文件
	LPCTSTR pszFilter=TEXT("中国象棋棋谱 （*.CML）|*.CML||");
	CFileDialog DlgSaveManual(FALSE,TEXT("CML"),szManualName,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter);
	DlgSaveManual.m_pOFN->lpstrInitialDir=szFilePath;

	//选择文件
	if (DlgSaveManual.DoModal()==IDOK)
	{
		//棋谱文件
		CChessManualFile ChessManualFile;
		ChessManualFile.SetBankerUser(m_wBankerUser);
		ChessManualFile.SetUserAccounts(0,m_szUserAccounts[0]);
		ChessManualFile.SetUserAccounts(1,m_szUserAccounts[1]);

		//保存棋谱
		if (ChessManualFile.SaveChessManual(DlgSaveManual.GetPathName(),m_GameLogic.m_ChessManualArray)==false)
		{
			//ASSERT(m_pIStringMessage!=NULL);
			/*if (m_pIStringMessage!=NULL) */InsertSystemString(TEXT("中国象棋“棋谱”保存失败"));
		}
	}

	return 0;
}

//点击棋盘
LRESULT CGameClientDlg::OnHitChessBorad(WPARAM wParam, LPARAM lParam)
{
	if (m_wCurrentUser==GetMeChairID())
	{
		//变量定义
		BYTE cbXTargetPos=(BYTE)wParam;
		BYTE cbYTargetPos=(BYTE)lParam;
		tagChessItem * pChessItem=m_GameLogic.GetChessItem(cbXTargetPos,cbYTargetPos);

		//取消判断
		if ((m_cbXSourcePos==cbXTargetPos)&&(m_cbYSourcePos==cbYTargetPos))
		{
			//设置棋盘
			m_cbXSourcePos=INVALID_POS;
			m_cbYSourcePos=INVALID_POS;
			m_GameClientView.SelectChessItem(INVALID_POS,INVALID_POS);

			return 0;
		}

		//选择判断
		if ((pChessItem!=NULL)&&(pChessItem->cbColor==m_cbChessColor))
		{
			//设置棋盘
			m_cbXSourcePos=cbXTargetPos;
			m_cbYSourcePos=cbYTargetPos;
			m_GameClientView.SelectChessItem(m_cbXSourcePos,m_cbYSourcePos);

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SELECT_CHESS"));

			return 0;
		}

		//走棋判断
		bool bWalkLegality=false;
		if ((m_cbXSourcePos!=INVALID_POS)&&(m_cbYSourcePos!=INVALID_POS))
		{
			//走棋判断
			tagChessItem * pSourceChessItem=m_GameLogic.GetChessItem(m_cbXSourcePos,m_cbYSourcePos);
			bWalkLegality=m_GameLogic.IsWalkLegality(pSourceChessItem,cbXTargetPos,cbYTargetPos);

			if(bWalkLegality)
			{
				//判断死棋
				if(m_GameLogic.IsLoseChess(pSourceChessItem->cbColor)==false)
				{
					//自杀棋步
					if(m_GameLogic.IsKillGeneral(m_cbXSourcePos,m_cbYSourcePos,cbXTargetPos,cbYTargetPos))
					{
						//创建窗口
						CDlgHint * pDlgUserHint=&m_GameClientView.m_UserHint;
						if (pDlgUserHint->m_hWnd==NULL) pDlgUserHint->Create(IDD_USER_HINT,&m_GameClientView);

						//显示窗口
						pDlgUserHint->InitTimerCount();
						pDlgUserHint->ShowWindow(SW_SHOW);
						pDlgUserHint->SetForegroundWindow();

						bWalkLegality = false;
					}
				}
			}
		}

		//走棋判断
		if (bWalkLegality==true)
		{
			//设置变量
			m_wCurrentUser=INVALID_CHAIR;

			//变量定义
			WORD wActionUser=GetMeChairID();
			WORD wStepTimeCount=__min(m_GameRuleInfo.wRuleStepTime,m_UserStatusInfo[wActionUser].wDrawTimeCount);

			//设置时间
			m_UserStatusInfo[wActionUser].wStepTimeCount=wStepTimeCount;
			m_UserStatusInfo[wActionUser].wSecondTimeCount=m_GameRuleInfo.wRuleSecondTime;
			m_GameClientView.SetUserStatusInfo(SwitchViewChairID(wActionUser),m_UserStatusInfo[wActionUser]);

			//移动棋子
			m_GameClientView.m_ChessBorad.SetPositively(false);
			PerformMoveChess(m_cbXSourcePos,m_cbYSourcePos,cbXTargetPos,cbYTargetPos);

			//悔棋设置
			WORD wStepCount=m_GameLogic.GetStepCount((GetMeChairID()==m_wBankerUser)?CHESS_BLACK:CHESS_WHITE);
			if (wStepCount==1) m_GameClientView.m_btRegret.EnableWindow(TRUE);

			//变量定义
			CMD_C_MoveChess MoveChess;
			ZeroMemory(&MoveChess,sizeof(MoveChess));

			//设置变量
			MoveChess.cbXTargetPos=cbXTargetPos;
			MoveChess.cbYTargetPos=cbYTargetPos;
			MoveChess.cbXSourcePos=m_cbXSourcePos;
			MoveChess.cbYSourcePos=m_cbYSourcePos;
			MoveChess.wUsedTimeCount=m_GameRuleInfo.wRuleDrawTime-m_UserStatusInfo[GetMeChairID()].wDrawTimeCount;

			//发送消息
			SendData(SUB_C_MOVE_CHESS,&MoveChess,sizeof(MoveChess));

			//设置变量
			m_cbXSourcePos=INVALID_POS;
			m_cbYSourcePos=INVALID_POS;
			m_GameClientView.SelectChessItem(INVALID_POS,INVALID_POS);

			return 0;
		}
		else
		{
			//取消选择
			m_cbXSourcePos=INVALID_POS;
			m_cbYSourcePos=INVALID_POS;
			m_GameClientView.SelectChessItem(m_cbXSourcePos,m_cbYSourcePos);

			return 0;
		}
	}

	return 0;
}

//同意规则
LRESULT CGameClientDlg::OnGameRuleAgree(WPARAM wParam, LPARAM lParam)
{
	//获取规则
	m_GameClientView.m_DlgRuleOption.GetGameRuleInfo(m_GameRuleInfo);

	//发送同意
	if (m_bRequestCustomize==true)
	{
		//设置界面
		m_GameClientView.SetGameRuleInfo(m_GameRuleInfo);

		//发送消息
		CMD_C_CustomizeRult CustomizeRult;
		CustomizeRult.GameRuleInfo=m_GameRuleInfo;
		SendUserReady(&CustomizeRult,sizeof(CustomizeRult));
	}
	else SendUserReady(NULL,0);

	return 0;
}

//拒绝规则
LRESULT CGameClientDlg::OnGameRuleReject(WPARAM wParam, LPARAM lParam)
{
	//离开游戏
	AfxGetMainWnd()->PostMessage(WM_CLOSE);

	return 0;
}

//棋谱定位
LRESULT CGameClientDlg::OnOrientationManual(WPARAM wParam, LPARAM lParam)
{
	//显示棋谱
	if (m_wViewStepCount!=wParam)
	{
		//设置变量
		m_cbXSourcePos=INVALID_POS;
		m_cbYSourcePos=INVALID_POS;

		//棋谱状态
		WORD wStepCount=(WORD)wParam;
		ZeroMemory(m_ChessItemView,sizeof(m_ChessItemView));
		ZeroMemory(m_ChessBoradView,sizeof(m_ChessBoradView));
		m_GameLogic.GetHistoryStatus(m_ChessItemView,m_ChessBoradView,wStepCount);

		//设置界面
		UpdateManualControl(wStepCount);
		m_GameClientView.SelectChessItem(INVALID_POS,INVALID_POS);
		m_GameClientView.m_ChessBorad.SetChessBorad(m_ChessBoradView);

		//设置框架
		if (wStepCount>0)
		{
			tagChessManual * pChessManual=&m_GameLogic.m_ChessManualArray[wStepCount-1];
			m_GameClientView.SetMoveChessTrace(pChessManual->cbXSourceChessPos,pChessManual->cbYSourceChessPos,
				pChessManual->cbXTargetChessPos,pChessManual->cbYTargetChessPos);
		}
		else
		{
			m_GameClientView.SetMoveChessTrace(INVALID_POS,INVALID_POS,INVALID_POS,INVALID_POS);
		}

		//更新界面
		m_GameClientView.UpdateGameView(NULL);
	}

	return 0;
}

//同意请求
LRESULT CGameClientDlg::OnUserRequestRespond(WPARAM wParam, LPARAM lParam)
{
	//同意处理
	switch (wParam)
	{
	case REQUEST_PEACE:		//和棋回应
		{
			//变量定义
			CMD_C_PeaceRespond PeaceRespond;
			ZeroMemory(&PeaceRespond,sizeof(PeaceRespond));

			//设置变量
			PeaceRespond.cbApprove=(BYTE)lParam;

			//发送消息
			SendData(SUB_C_PEACE_RESPOND,&PeaceRespond,sizeof(PeaceRespond));

			break;
		}
	case REQUEST_REGRET:	//悔棋回应
		{
			//变量定义
			CMD_C_RegretRespond RegretRespond;
			ZeroMemory(&RegretRespond,sizeof(RegretRespond));

			//设置变量
			RegretRespond.cbApprove=(BYTE)lParam;

			//发送消息
			SendData(SUB_C_REGRET_RESPOND,&RegretRespond,sizeof(RegretRespond));

			break;
		}
	}

	return 0;
}

//时间消息
VOID CGameClientDlg::OnTimer(UINT nIDEvent)
{
	//游戏时间
	if ((nIDEvent==IDI_GAME_TIME)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		//变量定义
		bool bWarnning=false;
		bool bDeductTime=false;
		tagStatusInfo * pStatusInfo=&m_UserStatusInfo[m_wCurrentUser];

		//局时模式
		if (/*(bDeductTime==false)&&*/(m_bDrawTimeMode==true)&&(pStatusInfo->wStepTimeCount>0))
		{
			//设置时间
			pStatusInfo->wStepTimeCount--;
			pStatusInfo->wDrawTimeCount--;

			//设置标志
			bDeductTime=true;
			bWarnning=(pStatusInfo->wStepTimeCount<=10);

			//时间切换
			if (pStatusInfo->wDrawTimeCount==0)
			{
				m_bDrawTimeMode=false;
				pStatusInfo->wSecondTimeCount=m_GameRuleInfo.wRuleSecondTime;
			}
		}

		//读秒模式
		if ((bDeductTime==false)&&(m_bDrawTimeMode==false)&&(pStatusInfo->wSecondTimeCount>0))
		{
			//设置标志
			bDeductTime=true;
			bWarnning=(pStatusInfo->wSecondTimeCount<=10);

			//设置时间
			pStatusInfo->wSecondTimeCount--;
		}

		//设置界面
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		m_GameClientView.SetUserStatusInfo(wViewChairID,m_UserStatusInfo[m_wCurrentUser]);

		//警告声音
		if ((bWarnning==true)&&(IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
		}

		//发送超时
		if ((IsLookonMode()==false)&&(bDeductTime==false)&&(m_wCurrentUser==GetMeChairID()))
		{
			//设置界面
			m_GameClientView.m_btPeace.EnableWindow(FALSE);
			m_GameClientView.m_btRegret.EnableWindow(FALSE);
			m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
			m_GameClientView.m_ChessBorad.SetPositively(false);

			//变量定义
			CMD_C_ConcludeRequest ConcludeRequest;
			ZeroMemory(&ConcludeRequest,sizeof(ConcludeRequest));

			//发送消息
			ConcludeRequest.cbReason=REASON_OVER_TIME;
			SendData(SUB_C_CONCLUDE_REQUEST,&ConcludeRequest,sizeof(ConcludeRequest));
		}

		return;
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
