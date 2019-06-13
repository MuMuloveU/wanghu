#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_PLACE_JETTON			100									//下注时间
#define IDI_DISPATCH_CARD			301									//发牌时间
#define IDI_SHOW_GAME_RESULT		302									//显示结果

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_PLACE_JETTON,OnPlaceJetton)
	ON_MESSAGE(IDM_APPLY_BANKER, OnApplyBanker)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//下注信息
	m_lMeMaxScore=0L;
	m_lMeTieScore=0L;
	m_lMeBankerScore=0L;
	m_lMePlayerScore=0L;
	m_lMeTieSamePointScore=0L;
	m_lMePlayerKingScore=0L;
	m_lMeBankerKingScore=0L;

	m_lAreaLimitScore=0L;	
	m_strDispatchCardTips = TEXT("");


	//历史信息
	m_wDrawCount=1;
	m_lMeResultCount=0;

	//状态变量
	m_bPlaying = false;

	//扑克信息
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbSendCount,sizeof(m_cbSendCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

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
	SetWindowText(TEXT("百家乐游戏"));

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
	m_lMeTieScore=0L;
	m_lMeBankerScore=0L;
	m_lMePlayerScore=0L;
	m_lMeTieSamePointScore=0L;
	m_lMePlayerKingScore=0L;
	m_lMeBankerKingScore=0L;
	
	m_lAreaLimitScore=0L;	
	m_strDispatchCardTips = TEXT("");
    
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
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbSendCount,sizeof(m_cbSendCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

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
	if ((nTimerID==IDI_PLACE_JETTON)&&(nElapse==0))
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
		PlayGameSound(AfxGetInstanceHandle(),TEXT("STOP_JETTON"));
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

			m_GameClientView.SetMeTieScore(pStatusFree->lMeTieScore);
			m_GameClientView.SetMeBankerScore(pStatusFree->lMeBankerScore);
			m_GameClientView.SetMePlayerScore(pStatusFree->lMePlayerScore);
			m_GameClientView.SetMeBankerKingScore(pStatusFree->lMeBankerKingScore);
			m_GameClientView.SetMePlayerKingScore(pStatusFree->lMePlayerKingScore);
			m_GameClientView.SetMeTieSamePointScore(pStatusFree->lMeTieKingScore);

			m_wCurrentBanker = pStatusFree->wCurrentBankerChairID;

			//设置变量
			m_lMeMaxScore= pStatusFree->lMeMaxScore ;
			m_lMeTieScore=pStatusFree->lMeTieScore;
			m_lMeBankerScore=pStatusFree->lMeBankerScore;
			m_lMePlayerScore=pStatusFree->lMePlayerScore;
			m_lMeTieSamePointScore = pStatusFree->lMeTieKingScore;
			m_lMeBankerKingScore = pStatusFree->lMeBankerKingScore;
			m_lMePlayerKingScore = pStatusFree->lMePlayerKingScore;

			//庄家信息
			if ( pStatusFree->wCurrentBankerChairID == INVALID_CHAIR )
				m_GameClientView.SetBankerInfo( INVALID_CHAIR, pStatusFree->cbBankerTime, pStatusFree->lBankerScore );
			else
				m_GameClientView.SetBankerInfo( SwitchViewChairID( pStatusFree->wCurrentBankerChairID ), pStatusFree->cbBankerTime, pStatusFree->lBankerScore );
			m_GameClientView.SetBankerTreasure(pStatusFree->lBankerTreasure);

			//下注界面
			m_GameClientView.PlaceUserJetton(ID_PING_JIA,pStatusFree->lTieScore);
			m_GameClientView.PlaceUserJetton(ID_TONG_DIAN_PING,pStatusFree->lTieSamePointScore);
			m_GameClientView.PlaceUserJetton(ID_XIAN_JIA,pStatusFree->lPlayerScore);
			m_GameClientView.PlaceUserJetton(ID_XIAN_TIAN_WANG,pStatusFree->lPlayerKingScore);
			m_GameClientView.PlaceUserJetton(ID_ZHUANG_JIA,pStatusFree->lBankerScore);
			m_GameClientView.PlaceUserJetton(ID_ZHUANG_TIAN_WANG,pStatusFree->lBankerKingScore);

			//更新控制
			UpdateButtonContron();

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("PLACE_JETTON"));

			//设置时间
			SetGameTimer(GetMeChairID(),IDI_PLACE_JETTON,pStatusFree->cbTimeLeave);

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
			m_lMeTieScore=pStatusPlay->lMeTieScore;
			m_lMeBankerScore=pStatusPlay->lMeBankerScore;
			m_lMePlayerScore=pStatusPlay->lMePlayerScore;
			m_lMeTieSamePointScore = pStatusPlay->lMeTieKingScore;
			m_lMeBankerKingScore = pStatusPlay->lMeBankerKingScore;
			m_lMePlayerKingScore = pStatusPlay->lMePlayerKingScore;

			//设置位置
			WORD wMeChairID=GetMeChairID();
			m_GameClientView.SetMeChairID(SwitchViewChairID(wMeChairID));
			m_GameClientView.SetHistoryScore(m_wDrawCount,m_lMeResultCount);

			//玩家下注
			m_GameClientView.SetMeMaxScore(pStatusPlay->lMeMaxScore);	
			m_GameClientView.SetAreaLimitScore( pStatusPlay->lAreaLimitScore );

			m_GameClientView.SetMeTieScore(pStatusPlay->lMeTieScore);
			m_GameClientView.SetMeBankerScore(pStatusPlay->lMeBankerScore);
			m_GameClientView.SetMePlayerScore(pStatusPlay->lMePlayerScore);
			m_GameClientView.SetMeBankerKingScore(pStatusPlay->lMeBankerKingScore);
			m_GameClientView.SetMePlayerKingScore(pStatusPlay->lMePlayerKingScore);
			m_GameClientView.SetMeTieSamePointScore(pStatusPlay->lMeTieKingScore);

			//庄家信息
			m_wCurrentBanker = pStatusPlay->wCurrentBankerChairID;
			if ( pStatusPlay->wCurrentBankerChairID == INVALID_CHAIR )
				m_GameClientView.SetBankerInfo( INVALID_CHAIR, pStatusPlay->cbBankerTime, pStatusPlay->lBankerScore );
			else
				m_GameClientView.SetBankerInfo( SwitchViewChairID( pStatusPlay->wCurrentBankerChairID ), pStatusPlay->cbBankerTime, pStatusPlay->lCurrentBankerScore );
			m_GameClientView.SetBankerTreasure(pStatusPlay->lBankerTreasure);


			//下注界面
			m_GameClientView.PlaceUserJetton(ID_PING_JIA,pStatusPlay->lTieScore);
			m_GameClientView.PlaceUserJetton(ID_TONG_DIAN_PING,pStatusPlay->lTieSamePointScore);
			m_GameClientView.PlaceUserJetton(ID_XIAN_JIA,pStatusPlay->lPlayerScore);
			m_GameClientView.PlaceUserJetton(ID_XIAN_TIAN_WANG,pStatusPlay->lPlayerKingScore);
			m_GameClientView.PlaceUserJetton(ID_ZHUANG_JIA,pStatusPlay->lBankerScore);
			m_GameClientView.PlaceUserJetton(ID_ZHUANG_TIAN_WANG,pStatusPlay->lBankerKingScore);

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

			//发送扑克
			DispatchUserCard(pStatusPlay->cbTableCardArray[INDEX_PLAYER],pStatusPlay->cbCardCount[INDEX_PLAYER],
				pStatusPlay->cbTableCardArray[INDEX_BANKER],pStatusPlay->cbCardCount[INDEX_BANKER]);

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
	SetGameStatus(GS_PLAYING);
	KillGameTimer(IDI_PLACE_JETTON);

	//更新控制
	UpdateButtonContron();

	//派发扑克
	DispatchUserCard(pGameStart->cbTableCardArray[INDEX_PLAYER],pGameStart->cbCardCount[INDEX_PLAYER],
		pGameStart->cbTableCardArray[INDEX_BANKER],pGameStart->cbCardCount[INDEX_BANKER]);

	//环境设置
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

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
	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));

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
	m_GameClientView.SetDispatchCardFalg(false);

	//设置变量
	m_lMeTieScore=0L;
	m_lMeBankerScore=0L;
	m_lMePlayerScore=0L;
	m_lMeTieSamePointScore = 0L;
	m_lMeBankerKingScore = 0L;
	m_lMePlayerKingScore = 0L;
	m_lMeMaxScore=pGameEnd->lMeMaxScore;

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

	BYTE cbPlayerPoint=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerPoint=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	enOperateResult OperateResult = enOperateResult_NULL;
	if ( 0 < m_GameClientView.m_lMeCurGameScore ) OperateResult = enOperateResult_Win;
	else if ( m_GameClientView.m_lMeCurGameScore < 0 ) OperateResult = enOperateResult_Lost;
	else OperateResult = enOperateResult_NULL;

	m_GameClientView.SetGameHistory(OperateResult, cbPlayerPoint, cbBankerPoint);

	//庄家金币
	m_GameClientView.SetBankerTreasure(pGameEnd->lBankerTreasure);

	//状态变量
	m_bPlaying = false;

	//玩家下注
	m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
	m_GameClientView.SetMeTieScore(m_lMeTieScore);
	m_GameClientView.SetMeBankerScore(m_lMeBankerScore);
	m_GameClientView.SetMePlayerScore(m_lMePlayerScore);
	m_GameClientView.SetMeBankerKingScore(m_lMeBankerKingScore);
	m_GameClientView.SetMePlayerKingScore(m_lMePlayerKingScore);
	m_GameClientView.SetMeTieSamePointScore(m_lMeTieSamePointScore);

	//设置界面
	m_GameClientView.CleanUserJetton();
	m_GameClientView.SetWinnerSide(NULL,0xFF,0xFF);
	m_GameClientView.m_CardControl[INDEX_PLAYER].SetCardData(NULL,0);
	m_GameClientView.m_CardControl[INDEX_BANKER].SetCardData(NULL,0);

	KillTimer(IDI_SHOW_GAME_RESULT);
	m_GameClientView.SetShowGameFlag(false);
	m_strDispatchCardTips = TEXT("");
    m_GameClientView.SetDispatchCardTips(TEXT(""));

	m_GameClientView.SetHistoryScore(m_wDrawCount,m_lMeResultCount);

	//庄家信息
	if ( m_wCurrentBanker != INVALID_CHAIR )
		m_GameClientView.SetBankerInfo(SwitchViewChairID(m_wCurrentBanker), pGameEnd->nBankerTime, pGameEnd->lBankerTotalScore);


	//更新控制
	UpdateButtonContron();

	//扑克信息
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbSendCount,sizeof(m_cbSendCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("PLACE_JETTON"));

	//设置时间
	SetGameTimer(GetMeChairID(),IDI_PLACE_JETTON,pGameEnd->cbTimeLeave);

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

	m_GameClientView.SetMeTieScore(pGameScore->lMeTieScore);
	m_GameClientView.SetMeBankerScore(pGameScore->lMeBankerScore);
	m_GameClientView.SetMePlayerScore(pGameScore->lMePlayerScore);
	m_GameClientView.SetMeBankerKingScore(pGameScore->lMeBankerKingScore);
	m_GameClientView.SetMePlayerKingScore(pGameScore->lMePlayerKingScore);
	m_GameClientView.SetMeTieSamePointScore(pGameScore->lMeTieKingScore);

	//设置变量
	m_lMeTieScore=pGameScore->lMeTieScore;
	m_lMeBankerScore=pGameScore->lMeBankerScore;
	m_lMePlayerScore=pGameScore->lMePlayerScore;
	m_lMeTieSamePointScore = pGameScore->lMeTieKingScore;
	m_lMeBankerKingScore = pGameScore->lMeBankerKingScore;
	m_lMePlayerKingScore = pGameScore->lMePlayerKingScore;

	return true;
}

//更新控制
void CGameClientDlg::UpdateButtonContron()
{
	if ((IsLookonMode()==false)&&(GetGameStatus()==GS_FREE) && m_wCurrentBanker != GetMeChairID() && m_wCurrentBanker != INVALID_CHAIR )
	{
		//计算积分
		LONG lCurrentJetton=m_GameClientView.GetCurrentJetton();
		LONG lLeaveScore=m_lMeMaxScore-m_lMeTieScore-m_lMeBankerScore-m_lMePlayerScore-m_lMeBankerKingScore-m_lMePlayerKingScore-
			m_lMeTieSamePointScore;

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
		if ( GetGameStatus()==GS_FREE ) 
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

//派发扑克
bool CGameClientDlg::DispatchUserCard(BYTE cbPlayerCard[], BYTE cbPlayerCount, BYTE cbBankerCard[], BYTE cbBankerCount)
{
	//效验参数
	ASSERT(cbPlayerCount<=CountArray(m_cbTableCardArray[INDEX_PLAYER]));
	ASSERT(cbBankerCount<=CountArray(m_cbTableCardArray[INDEX_BANKER]));

	//设置变量
	m_cbSendCount[INDEX_PLAYER]=0;
	m_cbSendCount[INDEX_BANKER]=0;
	m_cbCardCount[INDEX_PLAYER]=cbPlayerCount;
	m_cbCardCount[INDEX_BANKER]=cbBankerCount;
	CopyMemory(m_cbTableCardArray[INDEX_PLAYER],cbPlayerCard,sizeof(BYTE)*cbPlayerCount);
	CopyMemory(m_cbTableCardArray[INDEX_BANKER],cbBankerCard,sizeof(BYTE)*cbBankerCount);

	//设置界面
	m_GameClientView.m_CardControl[INDEX_PLAYER].SetCardData(NULL,0);
	m_GameClientView.m_CardControl[INDEX_BANKER].SetCardData(NULL,0);
	m_GameClientView.SetDispatchCardFalg(true);

	//设置定时器
	SetTimer(IDI_DISPATCH_CARD,1000,NULL);

	return true;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case IDI_DISPATCH_CARD:		//派发扑克
		{
			m_GameClientView.m_CardControl[0].ShowWindow(SW_SHOW);
			m_GameClientView.m_CardControl[1].ShowWindow(SW_SHOW);

			//发送扑克
			if ((m_cbSendCount[0]+m_cbSendCount[1])<4)
			{
				//定义索引
				WORD wIndex=(m_cbSendCount[0]+m_cbSendCount[1])%2;

				//设置扑克
				m_cbSendCount[wIndex]++;
				m_GameClientView.m_CardControl[wIndex].SetCardData(m_cbTableCardArray[wIndex],m_cbSendCount[wIndex]);

				//更新界面
				m_GameClientView.UpdateGameView(NULL);

				//播放声音
				PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

				return;
			}

			//闲三扑克
			if (m_cbSendCount[INDEX_PLAYER]<m_cbCardCount[INDEX_PLAYER])
			{
				//设置扑克
				m_cbSendCount[INDEX_PLAYER]++;
				m_GameClientView.m_CardControl[INDEX_PLAYER].SetCardData(m_cbTableCardArray[INDEX_PLAYER],m_cbSendCount[INDEX_PLAYER]);

				//播放声音
				PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

				//发牌提示
				SetDispatchCardTips();

				return;
			}

			//庄三扑克
			if (m_cbSendCount[INDEX_BANKER]<m_cbCardCount[INDEX_BANKER])
			{
				//设置扑克
				m_cbSendCount[INDEX_BANKER]++;
				m_GameClientView.m_CardControl[INDEX_BANKER].SetCardData(m_cbTableCardArray[INDEX_BANKER],m_cbSendCount[INDEX_BANKER]);

				//播放声音
				PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

				
				//发牌提示
				SetDispatchCardTips();

				return;
			}

			//获取牌点
			BYTE cbPlayerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
			BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

			//设置胜方
			BYTE cbWinnerSide=ID_PING_JIA, cbKingWinner=0;
			DeduceWinner(cbWinnerSide, cbKingWinner);

			//胜利字符
			TCHAR szGameResult[128]=TEXT("");
			LPCTSTR pszWinnerSide[]={TEXT("闲胜"),TEXT("平胜"),TEXT("庄胜")};
			int nIndex = 0;
			if (cbPlayerCount>cbBankerCount) nIndex = 0;
			else if (cbBankerCount>cbPlayerCount) nIndex = 2;
			else nIndex = 1;
			_sntprintf(szGameResult,CountArray(szGameResult),TEXT("闲：%d点，庄：%d点，%s"),cbPlayerCount,cbBankerCount, pszWinnerSide[nIndex]);

			m_strDispatchCardTips += szGameResult;
			m_GameClientView.SetDispatchCardTips(m_strDispatchCardTips);

			//设置界面
			m_GameClientView.SetWinnerSide(szGameResult,cbWinnerSide,cbKingWinner);
			m_GameClientView.SetDispatchCardFalg(false);

			//删除时间
			KillTimer(IDI_DISPATCH_CARD);

			//设置时间
			m_nShowResultTime = 15;
			SetTimer(IDI_SHOW_GAME_RESULT, 1000, NULL);
			m_GameClientView.SetShowGameFlag(true);
			m_GameClientView.m_CardControl[0].ShowWindow(SW_HIDE);
			m_GameClientView.m_CardControl[1].ShowWindow(SW_HIDE);

			//播放剩余
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));

			return;
		}
	case IDI_SHOW_GAME_RESULT:
		{
			m_nShowResultTime--;
			if ( m_nShowResultTime <= 0 )
			{
				KillTimer(IDI_SHOW_GAME_RESULT);
				m_GameClientView.SetShowGameFlag(false);
			}
			return;
		}
	}

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
		case ID_XIAN_JIA:
			{
				m_lMePlayerScore += lJettonScore;
				m_GameClientView.SetMePlayerScore(m_lMePlayerScore);
				break;
			}
		case ID_PING_JIA:
			{
				m_lMeTieScore += lJettonScore;
				m_GameClientView.SetMeTieScore(m_lMeTieScore);
				break;
			}
		case ID_ZHUANG_JIA:
			{
				m_lMeBankerScore += lJettonScore;
				m_GameClientView.SetMeBankerScore(m_lMeBankerScore);
				break;
			}
		case ID_TONG_DIAN_PING:
			{
				m_lMeTieSamePointScore += lJettonScore;
				m_GameClientView.SetMeTieSamePointScore(m_lMeTieSamePointScore);
				break;
			}
		case ID_XIAN_TIAN_WANG:
			{
				m_lMePlayerKingScore += lJettonScore;
				m_GameClientView.SetMePlayerKingScore(m_lMePlayerKingScore);
				break;
			}
		case ID_ZHUANG_TIAN_WANG:
			{
				m_lMeBankerKingScore += lJettonScore;
				m_GameClientView.SetMeBankerKingScore(m_lMeBankerKingScore);
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
	m_GameClientView.ShowChangeBanker( m_wCurrentBanker != pChangeBanker->wChairID );

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

//取消消息
void CGameClientDlg::OnCancel()
{
	tagUserData const *pMeUserData = GetUserData( GetMeChairID() );
	if ( pMeUserData != NULL && pMeUserData->cbUserStatus != US_PLAY && m_bPlaying != false )
	{
		int iRet=AfxMessageBox(TEXT("你正在游戏中，强行退出将被扣分，确实要强退吗？"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
		if (iRet!=IDYES) return;
	}

	__super::OnCancel();
}

//游戏记录
bool CGameClientDlg::OnSubGameRecord(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagServerGameRecord)==0);
	if (wDataSize%sizeof(tagServerGameRecord)!=0) return false;
	
	//结果变量
	tagGameRecord GameRecord;
	ZeroMemory(&GameRecord,sizeof(GameRecord));

	//设置记录
	WORD wRecordCount=wDataSize/sizeof(tagServerGameRecord);
	for (WORD wIndex=0;wIndex<wRecordCount;wIndex++) 
	{
		tagServerGameRecord * pServerGameRecord=(((tagServerGameRecord *)pBuffer)+wIndex);

		m_GameClientView.SetGameHistory(enOperateResult_NULL, pServerGameRecord->cbPlayerCount, pServerGameRecord->cbBankerCount);
	}

	return true;
}

//发牌提示
void CGameClientDlg::SetDispatchCardTips()
{
	//计算点数
	BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],2);
	BYTE cbPlayerTwoCardCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],2);

	//闲家补牌
	BYTE cbPlayerThirdCardValue = 0 ;	//第三张牌点数
	if(cbPlayerTwoCardCount<=5 && cbBankerCount<8)
	{
		//计算点数
		cbPlayerThirdCardValue = m_GameLogic.GetCardPip(m_cbTableCardArray[INDEX_PLAYER][2]);
		CString strTips;
		strTips.Format(TEXT("闲%d点，庄%d点，闲继续拿牌\n"), cbPlayerTwoCardCount, cbBankerCount);
		m_strDispatchCardTips = strTips;
		m_GameClientView.SetDispatchCardTips(m_strDispatchCardTips);
	}

	//庄家补牌
	CString strTips;
	if(cbPlayerTwoCardCount<8 && cbBankerCount<8 && m_cbSendCount[INDEX_BANKER] == 3)
	{
		switch(cbBankerCount)
		{
		case 0:
		case 1:
		case 2:
			break;

		case 3:
			if(m_cbCardCount[INDEX_PLAYER]==3 && cbPlayerThirdCardValue!=8)
			{
				strTips.Format(TEXT("闲第三张牌%d点，庄%d点，庄继续拿牌\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
				m_GameClientView.SetDispatchCardTips(m_strDispatchCardTips);
			}
			else if (m_cbCardCount[INDEX_PLAYER]==2) 
			{
				strTips.Format(TEXT("闲不补牌，庄%d点，庄继续拿牌\n"), cbBankerCount);
				m_strDispatchCardTips += strTips;
				m_GameClientView.SetDispatchCardTips(m_strDispatchCardTips);
			}			
			break;

		case 4:
			if(m_cbCardCount[INDEX_PLAYER]==3 && cbPlayerThirdCardValue!=1 && cbPlayerThirdCardValue!=8 && cbPlayerThirdCardValue!=9 && cbPlayerThirdCardValue!=0)
			{
				strTips.Format(TEXT("闲第三张牌%d点，庄%d点，庄继续拿牌\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
				m_GameClientView.SetDispatchCardTips(m_strDispatchCardTips);
			}
			else if ( m_cbCardCount[INDEX_PLAYER]==2) 
			{
				strTips.Format(TEXT("闲不补牌，庄%d点，庄继续拿牌\n"), cbBankerCount);
				m_strDispatchCardTips += strTips;
				m_GameClientView.SetDispatchCardTips(m_strDispatchCardTips);
			}
			break;

		case 5:
			if(m_cbCardCount[INDEX_PLAYER]==3 && cbPlayerThirdCardValue!=1 && cbPlayerThirdCardValue!=2 && cbPlayerThirdCardValue!=3  && cbPlayerThirdCardValue!=8 && cbPlayerThirdCardValue!=9 &&  cbPlayerThirdCardValue!=0)
			{
				strTips.Format(TEXT("闲第三张牌%d点，庄%d点，庄继续拿牌\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
				m_GameClientView.SetDispatchCardTips(m_strDispatchCardTips);
			}
			else if ( m_cbCardCount[INDEX_PLAYER]==2) 
			{
				strTips.Format(TEXT("闲不补牌，庄%d点，庄继续拿牌\n"), cbBankerCount);
				m_strDispatchCardTips += strTips;
				m_GameClientView.SetDispatchCardTips(m_strDispatchCardTips);
			}
			break;

		case 6:
			if(m_cbCardCount[INDEX_PLAYER]==3 && (cbPlayerThirdCardValue==6 || cbPlayerThirdCardValue==7))
			{
				strTips.Format(TEXT("闲第三张牌%d点，庄%d点，庄继续拿牌\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
				m_GameClientView.SetDispatchCardTips(m_strDispatchCardTips);
			}
			break;

			//不须补牌
		case 7:
		case 8:
		case 9:
			break;
		default:
			break;
		}
	}

	return ;
}


//推断赢家
void CGameClientDlg::DeduceWinner(BYTE &cbWinner, BYTE &cbKingWinner)
{
	cbWinner = 0;
	cbKingWinner = 0;

	//计算牌点
	BYTE cbPlayerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//胜利玩家
	if (cbPlayerCount==cbBankerCount)
	{
		cbWinner=ID_PING_JIA;

		//同点平判断
		bool bAllPointSame = false;
		if ( m_cbCardCount[INDEX_PLAYER] == m_cbCardCount[INDEX_BANKER] )
		{
			for (WORD wCardIndex = 0; wCardIndex < m_cbCardCount[INDEX_PLAYER]; ++wCardIndex )
			{
				BYTE cbBankerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][wCardIndex]);
				BYTE cbPlayerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][wCardIndex]);
				if ( cbBankerValue != cbPlayerValue ) break;
			}
			if ( wCardIndex == m_cbCardCount[INDEX_PLAYER] ) bAllPointSame = true;
		}
		if ( bAllPointSame ) cbKingWinner = ID_TONG_DIAN_PING;
	}
	else if (cbPlayerCount<cbBankerCount) 
	{
		cbWinner=ID_ZHUANG_JIA;

		//天王判断
		if ( cbBankerCount == 8 || cbBankerCount == 9 ) cbKingWinner = ID_ZHUANG_TIAN_WANG;
	}
	else 
	{
		cbWinner=ID_XIAN_JIA;

		//天王判断
		if ( cbPlayerCount == 8 || cbPlayerCount == 9 ) cbKingWinner = ID_XIAN_TIAN_WANG;
	}
}
//////////////////////////////////////////////////////////////////////////
