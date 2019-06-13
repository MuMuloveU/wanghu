#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FREE					100									//空闲时间
#define IDI_GAME					101									//游戏时间
#define IDI_END_PLACE_JETTON		102									//游戏时间
#define IDI_DISPATCH_CARD			301									//发牌时间
#define IDI_SHOW_GAME_RESULT		302									//显示结果

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_PLACE_JETTON,OnPlaceJetton)
	ON_MESSAGE(IDM_APPLY_BANKER, OnApplyBanker)
	ON_MESSAGE(IDM_UPDATE_CONTROL, OnUpdateControl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//下注信息
	m_lMeMaxScore=0L;
	m_lMeOddScore=0L;
	m_lMeEvenScore=0L;

	m_lAreaLimitScore=0L;	

	//历史信息
	m_wDrawCount=1;
	m_lMeResultCount=0;

	//状态变量
	m_bPlaying = false;

	//扑克信息
	ZeroMemory(m_cbDice,sizeof(m_cbDice));

	//庄家信息
	m_lApplyBankerCondition = 100000000;
	m_wCurrentBanker = INVALID_CHAIR;
	m_bMeApplyBanker = false;

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
	SetWindowText(TEXT("单双游戏"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//下注信息
	m_lMeMaxScore=0L;
	m_lMeOddScore=0L;
	m_lMeEvenScore=0L;
	
	m_lAreaLimitScore=0L;	
    
	//历史信息
	m_wDrawCount=1;
	m_lMeResultCount=0;

	//状态变量
	m_bPlaying = false;
	m_bMeApplyBanker = false;

	//庄家信息
	m_lApplyBankerCondition = 100000000;
	m_wCurrentBanker = INVALID_CHAIR;

	//扑克信息
	ZeroMemory(m_cbDice,sizeof(m_cbDice));

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	if ((nTimerID==IDI_GAME)&&(nElapse==0))
	{
		//设置光标
		m_GameClientView.SetCurrentJetton(0L);

		//禁止按钮
		m_GameClientView.m_btJetton100.EnableWindow(FALSE);
		m_GameClientView.m_btJetton1000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton10000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton100000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton500.EnableWindow(FALSE);
		m_GameClientView.m_btJetton50000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton500000.EnableWindow(FALSE);

		//庄家按钮
		m_GameClientView.m_btApplyBanker.EnableWindow( FALSE );
		m_GameClientView.m_btCancelBanker.EnableWindow( FALSE );

		//播放声音
		if ( IsEnableSound() ) PlayGameSound(AfxGetInstanceHandle(),TEXT("STOP_JETTON"));
	}

	return true;
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
	case SUB_S_PLACE_JETTON:	//用户加注
		{
			return OnSubPlaceJetton(pBuffer,wDataSize);
		}
	case SUB_S_APPLY_BANKER:	//申请做庄
		{
			return OnUserApplyBanker( pBuffer, wDataSize );
		}
	case SUB_S_CHANGE_BANKER:	//切换庄家
		{
			return OnChangeBanker( pBuffer, wDataSize );
		}
	case SUB_S_CHANGE_USER_SCORE://更新积分
		{
			return OnChangeUserScore( pBuffer, wDataSize );
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_SEND_RECORD:		//游戏记录
		{
			return OnSubGameRecord(pBuffer,wDataSize);
		}
	case SUB_S_GAME_SCORE:		//游戏积分
		{
			return OnSubGameScore(pBuffer, wDataSize);
		}
	case SUB_S_END_PLACE_JETTON://下注结束
		{
			return OnSubEndPlaceJetton(pBuffer, wDataSize);
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_FREE:			//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//消息处理
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//庄家变量
			m_lApplyBankerCondition = pStatusFree->lApplyBankerCondition;			

			//设置位置
			WORD wMeChairID=GetMeChairID();
			m_GameClientView.SetMeChairID(SwitchViewChairID(wMeChairID));
			m_GameClientView.SetHistoryScore(m_wDrawCount,m_lMeResultCount);

			//玩家下注
			m_GameClientView.SetMeMaxScore(pStatusFree->lMeMaxScore);
			m_GameClientView.SetAreaLimitScore( pStatusFree->lAreaLimitScore );

			m_GameClientView.SetMeOddScore(pStatusFree->lMeOddScore);
			m_GameClientView.SetMeEvenScore(pStatusFree->lMeEvenScore);

			m_wCurrentBanker = pStatusFree->wCurrentBankerChairID;

			//设置变量
			m_lMeMaxScore= pStatusFree->lMeMaxScore ;
			m_lMeOddScore=pStatusFree->lMeOddScore;
			m_lMeEvenScore=pStatusFree->lMeEvenScore;

			//庄家信息
			if ( pStatusFree->wCurrentBankerChairID == INVALID_CHAIR )
				m_GameClientView.SetBankerInfo( INVALID_CHAIR, pStatusFree->cbBankerTime, pStatusFree->lCurrentBankerScore );
			else
				m_GameClientView.SetBankerInfo( SwitchViewChairID( pStatusFree->wCurrentBankerChairID ), pStatusFree->cbBankerTime, pStatusFree->lCurrentBankerScore );
			m_GameClientView.SetBankerTreasure(pStatusFree->lBankerTreasure);

			//下注界面
			m_GameClientView.PlaceUserJetton(ID_ODD,pStatusFree->lOddcore);
			m_GameClientView.PlaceUserJetton(ID_EVEN,pStatusFree->lEvenScore);
			m_GameClientView.ShowCloseDicebox(true);

			//更新控制
			UpdateButtonContron();

			//播放声音
			if ( IsEnableSound() ) PlayGameSound(AfxGetInstanceHandle(),TEXT("PLACE_JETTON"));

			//设置时间
			SetGameTimer(GetMeChairID(),IDI_FREE,pStatusFree->cbTimeLeave);

			return true;
		}
	case GS_PLAYING:		//游戏状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//消息处理
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//庄家变量
			m_lApplyBankerCondition = pStatusPlay->lApplyBankerCondition;

			//设置变量
			m_lMeMaxScore=pStatusPlay->lMeMaxScore ;
			m_lMeOddScore=pStatusPlay->lMeOddScore;
			m_lMeEvenScore=pStatusPlay->lMeEvenScore;

			//设置位置
			WORD wMeChairID=GetMeChairID();
			m_GameClientView.SetMeChairID(SwitchViewChairID(wMeChairID));
			m_GameClientView.SetHistoryScore(m_wDrawCount,m_lMeResultCount);

			//玩家下注
			m_GameClientView.SetMeMaxScore(pStatusPlay->lMeMaxScore);	
			m_GameClientView.SetAreaLimitScore( pStatusPlay->lAreaLimitScore );

			m_GameClientView.SetMeOddScore(pStatusPlay->lMeOddScore);
			m_GameClientView.SetMeEvenScore(pStatusPlay->lMeEvenScore);

			//庄家信息
			m_wCurrentBanker = pStatusPlay->wCurrentBankerChairID;
			if ( pStatusPlay->wCurrentBankerChairID == INVALID_CHAIR )
				m_GameClientView.SetBankerInfo( INVALID_CHAIR, pStatusPlay->cbBankerTime, pStatusPlay->lCurrentBankerScore );
			else
				m_GameClientView.SetBankerInfo( SwitchViewChairID( pStatusPlay->wCurrentBankerChairID ), pStatusPlay->cbBankerTime, pStatusPlay->lCurrentBankerScore );
			m_GameClientView.SetBankerTreasure(pStatusPlay->lBankerTreasure);


			//下注界面
			m_GameClientView.PlaceUserJetton(ID_ODD,pStatusPlay->lOddcore);
			m_GameClientView.PlaceUserJetton(ID_EVEN,pStatusPlay->lEvenScore);

			//播放声音
			if ( IsEnableSound() ) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

			//设置色盅
			m_GameClientView.SetDice(pStatusPlay->cbDice);

			//禁用按钮
			m_GameClientView.m_btApplyBanker.EnableWindow( FALSE );
			m_GameClientView.m_btCancelBanker.EnableWindow( FALSE );

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

	//设置状态
	SetGameStatus(GS_WK_GAME);
	SetGameTimer(GetMeChairID(), IDI_GAME, pGameStart->cbGameTime);
	m_GameClientView.SetTimeIndex(1);

	//设置状态
	m_GameClientView.SetCartoon(INDEX_SHAKE_DICEBOX, true);

	//更新控制
	UpdateButtonContron();

	////环境设置
	//if ( IsEnableSound() ) PlayGameSound(AfxGetInstanceHandle(),TEXT("SHAKE_DICE"));

	return true;
}

//用户加注
bool CGameClientDlg::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJetton));
	if (wDataSize!=sizeof(CMD_S_PlaceJetton)) return false;

	//消息处理
	CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;

	//播放声音
	if ( IsEnableSound() ) PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));

	//加注界面
	m_GameClientView.PlaceUserJetton(pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);

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

	//设置状态
	SetGameStatus(GS_FREE);
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_SHOW_GAME_RESULT);

	//设置变量
	m_lMeOddScore=0L;
	m_lMeEvenScore=0L;

	//更新成绩
	for ( WORD wUserIndex = 0; wUserIndex < MAX_CHAIR; ++wUserIndex )
	{
		tagUserData const *pUserData = GetUserData(wUserIndex);

		if ( pUserData == NULL ) continue;
		tagApplyUser ApplyUser ;

		//更新信息
		ApplyUser.lUserScore = pUserData->lScore;
		ApplyUser.strUserName = pUserData->szName;
		m_GameClientView.m_ApplyUser.UpdateUser( ApplyUser );
	}

	enOperateResult OperateResult = enOperateResult_NULL;
	if ( 0 < m_GameClientView.m_lMeCurGameScore ) OperateResult = enOperateResult_Win;
	else if ( m_GameClientView.m_lMeCurGameScore < 0 ) OperateResult = enOperateResult_Lost;
	else OperateResult = enOperateResult_NULL;

	m_GameClientView.SetGameHistory(OperateResult, m_cbDice[0]+m_cbDice[1]);

	//庄家金币
	m_GameClientView.SetBankerTreasure(pGameEnd->lBankerTreasure);

	//状态变量
	m_bPlaying = false;

	//玩家下注
	m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
	m_GameClientView.SetMeOddScore(m_lMeOddScore);
	m_GameClientView.SetMeEvenScore(m_lMeEvenScore);

	//设置界面
	m_GameClientView.CleanUserJetton();
	m_GameClientView.SetWinnerSide(0xFF);

	KillTimer(IDI_SHOW_GAME_RESULT);
	m_GameClientView.SetShowGameFlag(false);

	m_GameClientView.SetHistoryScore(m_wDrawCount,m_lMeResultCount);

	//庄家信息
	if ( m_wCurrentBanker != INVALID_CHAIR )
		m_GameClientView.SetBankerInfo(SwitchViewChairID(m_wCurrentBanker), pGameEnd->nBankerTime, pGameEnd->lBankerTotalScore);


	//更新控制
	UpdateButtonContron();

	//播放声音
	if ( IsEnableSound() ) PlayGameSound(AfxGetInstanceHandle(),TEXT("PLACE_JETTON"));

	//设置时间
	SetGameTimer(GetMeChairID(),IDI_FREE,pGameEnd->cbTimeLeave);
	m_GameClientView.SetTimeIndex(0);

	return true;
}

//游戏积分
bool CGameClientDlg::OnSubGameScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameScore));
	if (wDataSize!=sizeof(CMD_S_GameScore)) return false;

	//消息处理
	CMD_S_GameScore * pGameScore=(CMD_S_GameScore *)pBuffer;

	//历史成绩
	m_wDrawCount++;
	m_lMeResultCount+=pGameScore->lMeGameScore;

	//设置成绩
	m_GameClientView.SetGameScore(pGameScore->lMeGameScore, pGameScore->lMeReturnScore, pGameScore->lBankerScore);

	m_GameClientView.SetMeOddScore(pGameScore->lMeOddScore);
	m_GameClientView.SetMeEvenScore(pGameScore->lMeEvenScore);

	//设置变量
	m_lMeOddScore=pGameScore->lMeOddScore;
	m_lMeEvenScore=pGameScore->lMeEvenScore;

	return true;
}

//下注结束
bool CGameClientDlg::OnSubEndPlaceJetton(const void * pBuffer, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize == sizeof(CMD_S_EndPlaceJetton));
	if ( wDataSize != sizeof(CMD_S_EndPlaceJetton)) return false;

	CMD_S_EndPlaceJetton *pEndPlaceJetton = (CMD_S_EndPlaceJetton *)pBuffer;

	//设置色子
	m_GameClientView.SetDice(pEndPlaceJetton->cbDice);
	CopyMemory(m_cbDice, pEndPlaceJetton->cbDice, sizeof(m_cbDice));

	//设置动画
	m_GameClientView.ShowCloseDicebox(false);
	m_GameClientView.SetCartoon(INDEX_OPEN_DICEBOX, true);

	//设置时间
	SetGameTimer(GetMeChairID(), IDI_END_PLACE_JETTON, pEndPlaceJetton->cbGameTime);
	m_GameClientView.SetTimeIndex(2);

	return true;
}

//更新控制
void CGameClientDlg::UpdateButtonContron()
{
	if ((IsLookonMode()==false)&&(GetGameStatus()==GS_WK_GAME) && m_wCurrentBanker != GetMeChairID() && m_wCurrentBanker != INVALID_CHAIR &&
		m_GameClientView.IsPlaceJettonStatus()==true)
	{
		//计算积分
		LONG lCurrentJetton=m_GameClientView.GetCurrentJetton();
		LONG lLeaveScore=m_lMeMaxScore-m_lMeOddScore-m_lMeEvenScore;

		//设置光标
		if (lCurrentJetton>lLeaveScore)
		{
			if (lLeaveScore>=500000L) m_GameClientView.SetCurrentJetton(500000L);
			else if (lLeaveScore>=100000L) m_GameClientView.SetCurrentJetton(100000L);
			else if (lLeaveScore>=50000L) m_GameClientView.SetCurrentJetton(50000L);
			else if (lLeaveScore>=10000L) m_GameClientView.SetCurrentJetton(10000L);
			else if (lLeaveScore>=1000L) m_GameClientView.SetCurrentJetton(1000L);
			else if (lLeaveScore>=500L) m_GameClientView.SetCurrentJetton(500L);
			else if (lLeaveScore>=100L) m_GameClientView.SetCurrentJetton(100L);
			else m_GameClientView.SetCurrentJetton(0L);
		}

		//控制按钮
		m_GameClientView.m_btJetton100.EnableWindow((lLeaveScore>=100)?TRUE:FALSE);
		m_GameClientView.m_btJetton500.EnableWindow((lLeaveScore>=500)?TRUE:FALSE);
		m_GameClientView.m_btJetton1000.EnableWindow((lLeaveScore>=1000)?TRUE:FALSE);
		m_GameClientView.m_btJetton10000.EnableWindow((lLeaveScore>=10000)?TRUE:FALSE);
		m_GameClientView.m_btJetton50000.EnableWindow((lLeaveScore>=50000)?TRUE:FALSE);		
		m_GameClientView.m_btJetton100000.EnableWindow((lLeaveScore>=100000)?TRUE:FALSE);
		m_GameClientView.m_btJetton500000.EnableWindow((lLeaveScore>=500000)?TRUE:FALSE);
	}
	else
	{
		//设置光标
		m_GameClientView.SetCurrentJetton(0L);

		//禁止按钮
		m_GameClientView.m_btJetton100.EnableWindow(FALSE);
		m_GameClientView.m_btJetton1000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton10000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton100000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton500.EnableWindow(FALSE);
		m_GameClientView.m_btJetton50000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton500000.EnableWindow(FALSE);
	}

	//申请按钮
	if ( ! IsLookonMode() )
	{
		//状态判断
		if ( GetGameStatus()==GS_WK_FREE ) 
		{
			m_GameClientView.m_btCancelBanker.EnableWindow(TRUE);
			m_GameClientView.m_btApplyBanker.EnableWindow(TRUE);
		}
		else
		{
			m_GameClientView.m_btCancelBanker.EnableWindow(FALSE);
			m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);
		}

		//显示判断
		const tagUserData *pMeUserData = GetUserData( GetMeChairID() );
		if ( m_bMeApplyBanker )
		{
			m_GameClientView.m_btCancelBanker.ShowWindow(SW_SHOW);
			m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);

			bool bMeBanker = pMeUserData->wChairID == m_wCurrentBanker ? true : false;
			if ( bMeBanker ) m_GameClientView.m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_BANKER,AfxGetInstanceHandle(),false);
			else m_GameClientView.m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false);

		}
		else if ( m_lApplyBankerCondition <= pMeUserData->lScore )
		{
			m_GameClientView.m_btCancelBanker.ShowWindow(SW_HIDE);
			m_GameClientView.m_btApplyBanker.ShowWindow(SW_SHOW);
		}
	}

	return;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	__super::OnTimer(nIDEvent);
}

//加注消息
LRESULT CGameClientDlg::OnPlaceJetton(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	BYTE cbJettonArea=(BYTE)wParam;
	LONG lJettonScore=(LONG)lParam;

	//庄家判断
	if ( GetMeChairID() == m_wCurrentBanker ) return true;
	if ( m_wCurrentBanker == INVALID_CHAIR ) 
	{
		UpdateButtonContron();
		return true;
	}

	//设置变量
	switch (cbJettonArea)
	{
		case ID_ODD:
			{
				m_lMeOddScore += lJettonScore;
				m_GameClientView.SetMeOddScore(m_lMeOddScore);
				break;
			}
		case ID_EVEN:
			{
				m_lMeEvenScore += lJettonScore;
				m_GameClientView.SetMeEvenScore(m_lMeEvenScore);
				break;
			}
		}

	//变量定义
	CMD_C_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

	//构造变量
	PlaceJetton.cbJettonArea=cbJettonArea;
	PlaceJetton.lJettonScore=lJettonScore;

	//发送消息
	SendData(SUB_C_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

	//更新按钮
	UpdateButtonContron();

	//设置状态
	m_bPlaying = true;

	return 0;
}

//申请消息
LRESULT CGameClientDlg::OnApplyBanker(WPARAM wParam, LPARAM lParam)
{
	//合法判断
	tagUserData const *pMeUserData = GetUserData( GetMeChairID() );
	if ( pMeUserData->lScore < m_lApplyBankerCondition ) return true;

	//旁观判断
	if ( IsLookonMode() ) return true;

	//转换变量
	bool bApplyBanker = wParam != 0 ? true : false;

	//当前判断
	if ( m_wCurrentBanker == GetMeChairID() && bApplyBanker ) return true;

	CMD_C_ApplyBanker ApplyBanker;

	//赋值变量
	ApplyBanker.bApplyBanker = bApplyBanker;

	//发送消息
	SendData( SUB_C_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );

	//设置按钮
	if ( m_wCurrentBanker == GetMeChairID() && !bApplyBanker )
	{
		m_GameClientView.m_btCancelBanker.EnableWindow(FALSE);
	}

	return true;
}

//更新消息
LRESULT CGameClientDlg::OnUpdateControl(WPARAM wParam, LPARAM lParam)
{
	//更新控制
	UpdateButtonContron();

	return true;
}

//申请做庄
bool CGameClientDlg::OnUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) return false;

	//消息处理
	CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;

	//插入玩家
	if ( pApplyBanker->bApplyBanker )
	{
		tagApplyUser ApplyUser;
		ApplyUser.strUserName = pApplyBanker->szAccount;
		ApplyUser.lUserScore = pApplyBanker->lScore;

		//插入判断
		bool bInsertApplyUser = true;

		if ( m_wCurrentBanker != INVALID_CHAIR )
		{
			tagUserData const *pBankerUserData = GetUserData(m_wCurrentBanker);
			if ( pBankerUserData != NULL && 0==lstrcmp(pBankerUserData->szName, pApplyBanker->szAccount))
				bInsertApplyUser = false;
		}

		if ( bInsertApplyUser == true ) m_GameClientView.m_ApplyUser.InserUser( ApplyUser );

		//更换按钮
		tagUserData const *pUserData = GetUserData( GetMeChairID() );
		if ( pUserData && !strcmp(pApplyBanker->szAccount, pUserData->szName ) )
		{
			m_bMeApplyBanker = true;
			UpdateButtonContron();
		}
	}
	else
	{
		tagApplyUser ApplyUser;
		ApplyUser.strUserName = pApplyBanker->szAccount;
		ApplyUser.lUserScore = pApplyBanker->lScore;
		m_GameClientView.m_ApplyUser.DeleteUser( ApplyUser );

		//更换按钮
		tagUserData const *pUserData = GetUserData( GetMeChairID() );
		if ( pUserData &&  !strcmp(pApplyBanker->szAccount, pUserData->szName ) )
		{
			m_bMeApplyBanker = false;
			
			UpdateButtonContron();
		}
	}

	return true;
}

//切换庄家
bool CGameClientDlg::OnChangeBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
	if (wDataSize!=sizeof(CMD_S_ChangeBanker)) return false;

	//消息处理
	CMD_S_ChangeBanker * pChangeBanker=(CMD_S_ChangeBanker *)pBuffer;

	//显示图片
	m_GameClientView.ShowChangeBanker( true );

	//自己判断
	if ( m_wCurrentBanker == GetMeChairID() && IsLookonMode() == false && pChangeBanker->wChairID != GetMeChairID() ) 
	{
		m_bMeApplyBanker = false;
	}

	m_wCurrentBanker = pChangeBanker->wChairID;

	//庄家信息
	if ( pChangeBanker->wChairID == INVALID_CHAIR )
	{
		m_GameClientView.SetBankerInfo( INVALID_CHAIR, pChangeBanker->cbBankerTime, pChangeBanker->lBankerScore );
	}
	else
	{
		m_GameClientView.SetBankerInfo( SwitchViewChairID( pChangeBanker->wChairID ), pChangeBanker->cbBankerTime, 0 );
	}
	m_GameClientView.SetBankerTreasure(pChangeBanker->lBankerTreasure);

	//删除玩家
	if ( m_wCurrentBanker != INVALID_CHAIR )
	{
		tagUserData const *pBankerUserData = GetUserData(m_wCurrentBanker);
		if ( pBankerUserData != NULL )
		{
			tagApplyUser ApplyUser;
			ApplyUser.strUserName = pBankerUserData->szName;
			m_GameClientView.m_ApplyUser.DeleteUser( ApplyUser );
		}
	}
	
	//更新界面
	UpdateButtonContron();

	return true;
}

//更新积分
bool CGameClientDlg::OnChangeUserScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ChangeUserScore));
	if (wDataSize!=sizeof(CMD_S_ChangeUserScore)) return false;

	//消息处理
	CMD_S_ChangeUserScore * pChangeUserScore=(CMD_S_ChangeUserScore *)pBuffer;

	tagUserData const *pUserData = GetUserData( pChangeUserScore->wChairID );

	if ( pUserData )
	{
		tagApplyUser ApplyUser ;

		ApplyUser.lUserScore = pChangeUserScore->lScore;
		ApplyUser.strUserName = pUserData->szName;
		m_GameClientView.m_ApplyUser.UpdateUser( ApplyUser );
	}

	//庄家信息
	if ( m_wCurrentBanker == pChangeUserScore->wCurrentBankerChairID )
	{
		m_GameClientView.SetBankerInfo( SwitchViewChairID( pChangeUserScore->wCurrentBankerChairID ), pChangeUserScore->cbBankerTime, pChangeUserScore->lCurrentBankerScore );

		//更新界面
		UpdateButtonContron();
	}

	return true;
}

//游戏记录
bool CGameClientDlg::OnSubGameRecord(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagServerGameRecord)==0);
	if (wDataSize%sizeof(tagServerGameRecord)!=0) return false;
	
	//结果变量
	tagServerGameRecord GameRecord;
	ZeroMemory(&GameRecord,sizeof(GameRecord));

	//设置记录
	WORD wRecordCount=wDataSize/sizeof(tagServerGameRecord);
	for (WORD wIndex=0;wIndex<wRecordCount;wIndex++) 
	{
		tagServerGameRecord * pServerGameRecord=(((tagServerGameRecord *)pBuffer)+wIndex);

		m_GameClientView.SetGameHistory(enOperateResult_NULL, pServerGameRecord->cbDicePoint);
	}

	return true;
}

//推断赢家
void CGameClientDlg::DeduceWinner(BYTE &cbWinner)
{
	bool bEven = (m_cbDice[0] + m_cbDice[1]) % 2 == 0 ? true : false;
	if ( bEven ) cbWinner = ID_EVEN;
	else cbWinner = ID_ODD;
}
//////////////////////////////////////////////////////////////////////////
