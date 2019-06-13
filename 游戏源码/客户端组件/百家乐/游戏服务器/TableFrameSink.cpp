#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//常量定义
#define SEND_COUNT					8									//发送次数

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//下注时间
#define IDI_PLACE_JETTON			1									//下注时间

#ifdef _DEBUG
#define TIME_PLACE_JETTON			30									//下注时间
#else
#define TIME_PLACE_JETTON			25									//下注时间
#endif

//结束时间
#define IDI_GAME_END				2									//结束时间
#ifdef _DEBUG
#define TIME_GAME_END				13									//结束时间
#else
#define TIME_GAME_END				13									//结束时间
#endif

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;				//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_TimeControl;	//开始模式

//////////////////////////////////////////////////////////////////////////

//替代结构
struct tagReplaceCard
{
	BYTE							cbCardCount[2];						//扑克数目
    BYTE							cbTableCardArray[2][3];				//桌面扑克
};

//平家代替
tagReplaceCard CardTieWinCard[]=
{
	3,3,0x12,0x05,0x00,0x22,0x25,0x32,
	3,3,0x11,0x05,0x00,0x14,0x2B,0x32,
	3,3,0x1A,0x2B,0x3D,0x1C,0x3C,0x0B,
	3,3,0x27,0x2D,0x00,0x01,0x39,0x3D,
	3,3,0x3C,0x28,0x00,0x12,0x15,0x01,
};

//庄家代替
tagReplaceCard CardBankerWinCard[]=
{
	3,3,0x17,0x1A,0x05,0x36,0x0D,0x1C,
	3,3,0x17,0x03,0x3A,0x1C,0x1A,0x34,
	3,3,0x11,0x0D,0x1D,0x35,0x32,0x00,
	3,3,0x18,0x29,0x00,0x22,0x16,0x00,
	3,3,0x38,0x1A,0x00,0x25,0x04,0x00,
};

//闲家代替
tagReplaceCard CardPlayerWinCard[]=
{
	3,3,0x37,0x3D,0x00,0x31,0x3D,0x0D,
	3,3,0x16,0x24,0x08,0x03,0x3D,0x00,
	3,3,0x0B,0x39,0x00,0x09,0x35,0x00,
	3,3,0x2C,0x08,0x00,0x02,0x15,0x00,
	3,3,0x16,0x33,0x00,0x1A,0x27,0x00,
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//下注信息
	m_lTieScore=0L;
	m_lBankerScore=0L;
	m_lPlayerScore=0L;
	m_lTieSamePointScore=0L;
	m_lBankerKingScore=0L;
	m_lPlayerKingScore=0L;


	//库存变量
	m_lStockScore=0;
	m_lDesPer = 0 ;

	m_lFreezeScore=0L;

	//状态变量
	m_dwJettonTime=0L;

	//下注信息
	ZeroMemory(m_lUserTieScore,sizeof(m_lUserTieScore));
	ZeroMemory(m_lUserBankerScore,sizeof(m_lUserBankerScore));
	ZeroMemory(m_lUserPlayerScore,sizeof(m_lUserPlayerScore));
	ZeroMemory(m_lUserTieSamePointScore,sizeof(m_lUserTieSamePointScore));
	ZeroMemory(m_lUserBankerKingScore,sizeof(m_lUserBankerKingScore));
	ZeroMemory(m_lUserPlayerKingScore,sizeof(m_lUserPlayerKingScore));

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//扑克信息
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//庄家变量
	m_lApplyBankerCondition = 100000000;
	ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
	m_cbBankerTimer=0;
	m_lBankerWinScore = 0;
	m_bCancelBanker=false;
	
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pITableFrameControl=NULL;

	//游戏记录
	ZeroMemory(m_GameRecordArrary, sizeof(m_GameRecordArrary));
	m_nRecordFirst=0;	
	m_nRecordLast=0;	


	//读取配置
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//读取配置
	TCHAR szFileName[MAX_PATH];
	_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\BaccaratConfig.ini"),szPath);

	//读取分数
	m_lApplyBankerCondition = GetPrivateProfileInt( TEXT("BankerCondition"), TEXT("Score"), 10000000, szFileName );
	m_lAreaLimitScore = GetPrivateProfileInt( TEXT("BankerCondition"), TEXT("AreaLimitScore"), 1000000000, szFileName );

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void * __cdecl CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool __cdecl CTableFrameSink::InitTableFrameSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//控制接口
	m_pITableFrameControl=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameControl);
	if (m_pITableFrameControl==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//配置参数
	TCHAR   buffer[MAX_PATH];
	if (GetCurrentDirectory(sizeof(buffer), buffer))
	{
		CString strIniFileName ;

		CString szDir = buffer;
		if(szDir.Right(1) != "\\")	szDir += "\\";
		szDir += "BaccaratConfig.ini";
		strIniFileName= szDir ;

		if(!strIniFileName.IsEmpty())
		{
            //库存数目
			CString StockScore ;
			GetPrivateProfileString(TEXT("StockScore"), TEXT("StockScore"), NULL, StockScore.GetBuffer(64), 64, strIniFileName) ;
			StockScore.ReleaseBuffer() ;
			m_lStockScore = atol(LPCTSTR(StockScore)) ;

			//递减比率
			CString DesPer ;
			GetPrivateProfileString(TEXT("DesPer"), TEXT("DesPer"), NULL, DesPer.GetBuffer(64), 64, strIniFileName) ;
			DesPer.ReleaseBuffer() ;
			m_lDesPer = atol(LPCTSTR(DesPer)) ;
		}

		if(m_lDesPer<=0) m_lDesPer = 2 ;
		if(m_lStockScore<=0) m_lStockScore = 5000 ;

	}

	return true;
}

//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	//下注信息
	m_lTieScore=0L;
	m_lBankerScore=0L;
	m_lPlayerScore=0L;
	m_lTieSamePointScore=0L;
	m_lBankerKingScore=0L;
	m_lPlayerKingScore=0L;

	//下注信息
	ZeroMemory(m_lUserTieScore,sizeof(m_lUserTieScore));
	ZeroMemory(m_lUserBankerScore,sizeof(m_lUserBankerScore));
	ZeroMemory(m_lUserPlayerScore,sizeof(m_lUserPlayerScore));
	ZeroMemory(m_lUserTieSamePointScore,sizeof(m_lUserTieSamePointScore));
	ZeroMemory(m_lUserBankerKingScore,sizeof(m_lUserBankerKingScore));
	ZeroMemory(m_lUserPlayerKingScore,sizeof(m_lUserPlayerKingScore));

	//扑克信息
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	return;
}

//开始模式
enStartMode __cdecl CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//游戏状态
bool __cdecl CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	//发送扑克
	DispatchTableCard();

	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//构造数据
	CopyMemory(GameStart.cbCardCount,m_cbCardCount,sizeof(m_cbCardCount));
	CopyMemory(GameStart.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
	GameStart.lApplyBankerCondition = m_lApplyBankerCondition;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

	//计算分数
	CalculateScore();

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//结束消息
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			GameEnd.nBankerTime = m_cbBankerTimer;

			GameEnd.lBankerTotalScore = m_lBankerWinScore;

			//推断玩家
			DeduceWinner(GameEnd.cbWinner, GameEnd.cbKingWinner);			

			//写入积分
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) continue;

				//胜利类型
				enScoreKind ScoreKind=(m_lUserWinScore[wUserChairID]>0L)?enScoreKind_Win:enScoreKind_Lost;

				//写入积分
				if (m_lUserWinScore[wUserChairID]!=0L) m_pITableFrame->WriteUserScore(wUserChairID,m_lUserWinScore[wUserChairID], m_lUserRevenue[wUserChairID], ScoreKind);

				//庄家判断
				if ( m_CurrentBanker.dwUserID == pIServerUserItem->GetUserID() ) m_CurrentBanker.lUserScore = pIServerUserItem->GetUserScore()->lScore;
			}

			//发送积分
			GameEnd.cbTimeLeave=TIME_PLACE_JETTON;	
			if ( m_CurrentBanker.dwUserID != 0 ) GameEnd.lBankerTreasure = m_CurrentBanker.lUserScore;
			for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(wUserIndex);
				if ( pIServerUserItem == NULL ) continue;

				//发送消息					
				GameEnd.lMeMaxScore=pIServerUserItem->GetUserScore()->lScore;
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			//庄家判断
			if ( m_CurrentBanker.dwUserID != 0 )
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(m_CurrentBanker.wChairID);
				if (pIServerUserItem) 
				{
					//积分判断
					if ( pIServerUserItem->GetUserScore()->lScore < m_lApplyBankerCondition )
					{
						//玩家下庄
						OnUserApplyBanker( pIServerUserItem->GetUserData(), false );
					}
				}
			}

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户离开
		{
			//胜利类型
			enScoreKind ScoreKind=(m_lUserWinScore[wChairID]>0L)?enScoreKind_Win:enScoreKind_Lost;

			//写入积分
			if (m_lUserWinScore[wChairID]!=0L) m_pITableFrame->WriteUserScore(wChairID,m_lUserWinScore[wChairID], m_lUserRevenue[wChairID], ScoreKind);

			return true;
		}
	}

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_FREE:			//空闲状态
		{
			//发送记录
			WORD wBufferSize=0;
			BYTE cbBuffer[SOCKET_PACKET];
			int nIndex = m_nRecordFirst;
			while ( nIndex != m_nRecordLast )
			{
				if ((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
				{
					m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
					wBufferSize=0;
				}
				CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
				wBufferSize+=sizeof(tagServerGameRecord);

				nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
			}
			if (wBufferSize>0) m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);

			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//下注信息
			StatusFree.lTieScore=m_lTieScore;
			StatusFree.lBankerScore=m_lBankerScore;
			StatusFree.lPlayerScore=m_lPlayerScore;
			StatusFree.lTieSamePointScore = m_lTieSamePointScore;
			StatusFree.lBankerKingScore = m_lBankerKingScore;
			StatusFree.lPlayerKingScore = m_lPlayerKingScore;

			StatusFree.lAreaLimitScore = m_lAreaLimitScore;

			//庄家信息
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;

			//下注信息
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusFree.lMeTieScore=m_lUserTieScore[wChiarID];
				StatusFree.lMeBankerScore=m_lUserBankerScore[wChiarID];
				StatusFree.lMePlayerScore=m_lUserPlayerScore[wChiarID];
				StatusFree.lMeTieKingScore = m_lUserTieSamePointScore[wChiarID];
				StatusFree.lMeBankerKingScore = m_lUserBankerKingScore[wChiarID];
				StatusFree.lMePlayerKingScore = m_lUserPlayerKingScore[wChiarID];
				StatusFree.lMeMaxScore=pIServerUserItem->GetUserScore()->lScore;
			}
	
			if ( m_CurrentBanker.dwUserID != 0 ) 
			{
				StatusFree.cbBankerTime = m_cbBankerTimer;
				StatusFree.lCurrentBankerScore = m_lBankerWinScore;
				StatusFree.wCurrentBankerChairID = m_CurrentBanker.wChairID;
				StatusFree.lBankerTreasure = m_CurrentBanker.lUserScore;
			}
			else StatusFree.wCurrentBankerChairID = INVALID_CHAIR;
			

			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(TIME_PLACE_JETTON-__min(dwPassTime,TIME_PLACE_JETTON));

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
			
			//发送申请者
			SendApplyUser(  pIServerUserItem );

			return bSuccess;
		}
	case GS_PLAYING:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//下注信息
			StatusPlay.lTieScore=m_lTieScore;
			StatusPlay.lBankerScore=m_lBankerScore;
			StatusPlay.lPlayerScore=m_lPlayerScore;
			StatusPlay.lTieSamePointScore = m_lTieSamePointScore;
			StatusPlay.lBankerKingScore = m_lBankerKingScore;
			StatusPlay.lPlayerKingScore = m_lPlayerKingScore;

			StatusPlay.lAreaLimitScore = m_lAreaLimitScore;

			//庄家信息
			StatusPlay.lApplyBankerCondition = m_lApplyBankerCondition;

			//下注信息
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusPlay.lMeTieScore=m_lUserTieScore[wChiarID];
				StatusPlay.lMeBankerScore=m_lUserBankerScore[wChiarID];
				StatusPlay.lMePlayerScore=m_lUserPlayerScore[wChiarID];
				StatusPlay.lMeTieKingScore = m_lUserTieSamePointScore[wChiarID];
				StatusPlay.lMeBankerKingScore = m_lUserBankerKingScore[wChiarID];
				StatusPlay.lMePlayerKingScore = m_lUserPlayerKingScore[wChiarID];
				StatusPlay.lMeMaxScore=pIServerUserItem->GetUserScore()->lScore;
			}

			if ( m_CurrentBanker.dwUserID != 0 ) 
			{
				StatusPlay.cbBankerTime = m_cbBankerTimer;
				StatusPlay.lCurrentBankerScore = m_lBankerWinScore;
				StatusPlay.wCurrentBankerChairID = m_CurrentBanker.wChairID;
				StatusPlay.lBankerTreasure = m_CurrentBanker.lUserScore;
			}
			else StatusPlay.wCurrentBankerChairID = INVALID_CHAIR;

			//扑克信息
			CopyMemory(StatusPlay.cbCardCount,m_cbCardCount,sizeof(m_cbCardCount));
			CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));

			//发送申请者
			SendApplyUser( pIServerUserItem );

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	return false;
}

//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_PLACE_JETTON:		//下注时间
		{
			//变量定义
			LONGLONG lIncrease=LONGLONG(m_lStockScore*m_lDesPer/100.);

			//设置库存
			if (m_lTieScore+m_lBankerScore+m_lPlayerScore)
			{
				m_lStockScore-=lIncrease;
				m_lFreezeScore+=lIncrease;
			}

			//开始游戏
			m_pITableFrameControl->StartGame();

			//设置时间
			m_pITableFrame->SetGameTimer(IDI_GAME_END,TIME_GAME_END*1000,1,0L);

			return true;
		}
	case IDI_GAME_END:			//结束游戏
		{
			//结束游戏
			OnEventGameEnd(INVALID_CHAIR,NULL,GER_NORMAL);

			//下庄判断
			if ( m_bCancelBanker && m_CurrentBanker.dwUserID != 0 )
			{		
				//获取玩家
				IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( m_CurrentBanker.wChairID );

				//重置变量
				m_cbBankerTimer = 0;
				m_lBankerWinScore=0;
				ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
				m_bCancelBanker=false;
				
				//发送消息
				SendChangeBankerMsg();

				//删除庄家
				if ( pServerUserItem ) OnUserApplyBanker( pServerUserItem->GetUserData(), false );
			}

			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,TIME_PLACE_JETTON*1000,1,0L);

			//轮换庄家
			ChangeBanker();

			//庄家信息
			if ( m_CurrentBanker.dwUserID != 0 )
			{
				CMD_S_ChangeUserScore ChangeUserScore;
				ZeroMemory( &ChangeUserScore, sizeof( ChangeUserScore ) );
				ChangeUserScore.wCurrentBankerChairID = m_CurrentBanker.wChairID;
				ChangeUserScore.lCurrentBankerScore = m_lBankerWinScore;
				ChangeUserScore.cbBankerTime = m_cbBankerTimer;
				ChangeUserScore.lScore = m_CurrentBanker.lUserScore;
				ChangeUserScore.wChairID = m_CurrentBanker.wChairID;
				m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_CHANGE_USER_SCORE,&ChangeUserScore,sizeof(ChangeUserScore));
				m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_CHANGE_USER_SCORE,&ChangeUserScore,sizeof(ChangeUserScore));
			}

			//切换判断
			if ( m_cbBankerTimer == 0 )
			{
				//发送消息
				SendChangeBankerMsg();
			}

			return true;
		}
	}

	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			if (wDataSize!=sizeof(CMD_C_PlaceJetton)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_SIT) return true;

			//消息处理
			CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pDataBuffer;
			return OnUserPlaceJetton(pUserData->wChairID,pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
		}
	case SUB_C_APPLY_BANKER:		//申请做庄
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ApplyBanker));
			if (wDataSize!=sizeof(CMD_C_ApplyBanker)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_SIT) return true;

			//消息处理
			CMD_C_ApplyBanker * pApplyBanker=(CMD_C_ApplyBanker *)pDataBuffer;

			return OnUserApplyBanker( pUserData, pApplyBanker->bApplyBanker );	
		}
	}

	return false;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool __cdecl CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//设置时间
	if ((bLookonUser==false)&&(m_dwJettonTime==0L))
	{
		m_dwJettonTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,TIME_PLACE_JETTON,1,NULL);
	}

	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//记录成绩
	if (bLookonUser==false)
	{
		//状态判断
		if ( m_pITableFrame->GetGameStatus() != GS_PLAYING )
		{
			//变量定义
			LONG lScore=0;
			LONG lRevenue=0;
			enScoreKind ScoreKind;

			//库存统计
			m_lStockScore+=( m_lUserTieScore[wChairID] + m_lUserBankerScore[wChairID] + m_lUserPlayerScore[wChairID] + 
				m_lUserBankerKingScore[wChairID] + m_lUserPlayerKingScore[wChairID] + m_lUserTieSamePointScore[wChairID]);

			//统计成绩
			lScore=-( m_lUserTieScore[wChairID] + m_lUserBankerScore[wChairID] + m_lUserPlayerScore[wChairID] + 
				m_lUserBankerKingScore[wChairID] + m_lUserPlayerKingScore[wChairID] + m_lUserTieSamePointScore[wChairID]);

			//置零变量
			m_lUserTieScore[wChairID] = 0; 
			m_lUserBankerScore[wChairID] = 0;
			m_lUserPlayerScore[wChairID] = 0;
			m_lUserBankerKingScore[wChairID] = 0 ;
			m_lUserPlayerKingScore[wChairID] = 0;
			m_lUserTieSamePointScore[wChairID] = 0;

			//写入积分
			if (lScore!=0L)
			{
				//写入积分
				ScoreKind=enScoreKind_Flee;
				m_pITableFrame->WriteUserScore(pIServerUserItem, lScore,lRevenue, ScoreKind);
			}
		}

		//存在判断
		for ( INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArrary.GetCount(); ++nUserIdx )
		{
			tagApplyUserInfo ApplyUserInfo = m_ApplyUserArrary[ nUserIdx ];

			if ( ApplyUserInfo.dwUserID == pIServerUserItem->GetUserID() )
			{
				//删除玩家
				m_ApplyUserArrary.RemoveAt( nUserIdx );

				//构造变量
				CMD_S_ApplyBanker ApplyBanker;
				CopyMemory( ApplyBanker.szAccount, pIServerUserItem->GetAccounts(), sizeof( ApplyBanker.szAccount ) );
				ApplyBanker.lScore = pIServerUserItem->GetUserScore()->lScore;
				ApplyBanker.bApplyBanker = false;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );

				//当前判断
				if ( ApplyUserInfo.dwUserID == m_CurrentBanker.dwUserID )
				{
					//重置变量
					ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
					m_cbBankerTimer=0;
					m_lBankerWinScore=0;

					//发送消息
					SendChangeBankerMsg();
				}

			}
		}
	}

	return true;
}

//加注事件
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONG lJettonScore)
{
	//效验参数
	ASSERT((cbJettonArea<=ID_TONG_DIAN_PING)&&(lJettonScore>0L));
	if ((cbJettonArea>ID_TONG_DIAN_PING)||(lJettonScore<=0L)) return false;

	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_FREE);
	if (m_pITableFrame->GetGameStatus()!=GS_FREE) return true;

	//庄家判断
	if ( m_CurrentBanker.dwUserID != 0 && m_CurrentBanker.wChairID == wChairID ) return true;
	if ( m_CurrentBanker.dwUserID == 0 ) return true;

	//变量定义
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
	LONG lJettonCount=m_lUserTieScore[wChairID]+m_lUserBankerScore[wChairID]+m_lUserPlayerScore[wChairID]+
		m_lUserTieSamePointScore[wChairID] + m_lUserPlayerKingScore[wChairID] + m_lUserBankerKingScore[wChairID];

	LONG lUserScore = pIServerUserItem->GetUserScore()->lScore;

	if ( lUserScore < lJettonCount + lJettonScore ) return true;

	//合法验证
	if ( ID_XIAN_JIA == cbJettonArea )
	{
		if ( GetMaxPlayerScore(wChairID) < lJettonScore ) return true;

		//保存下注
		m_lPlayerScore += lJettonScore;
		m_lUserPlayerScore[wChairID] += lJettonScore;
	}
	else if ( ID_XIAN_TIAN_WANG == cbJettonArea )
	{
		if ( GetMaxPlayerKingScore(wChairID) <lJettonScore ) return true;

		//保存下注
		m_lPlayerKingScore += lJettonScore;
		m_lUserPlayerKingScore[wChairID] += lJettonScore;
	}
	else if ( ID_PING_JIA == cbJettonArea )
	{
		if ( GetMaxTieScore(wChairID) <lJettonScore ) return true;

		//保存下注
		m_lTieScore += lJettonScore;
		m_lUserTieScore[wChairID] += lJettonScore;
	}
	else if ( ID_TONG_DIAN_PING == cbJettonArea )
	{
		if ( GetMaxTieKingScore(wChairID) <lJettonScore ) return true;

		//保存下注
		m_lTieSamePointScore += lJettonScore;
		m_lUserTieSamePointScore[wChairID] += lJettonScore;
	}
	else if ( ID_ZHUANG_JIA == cbJettonArea )
	{
		if ( GetMaxBankerScore(wChairID) <lJettonScore ) return true;

		//保存下注
		m_lBankerScore += lJettonScore;
		m_lUserBankerScore[wChairID] += lJettonScore;
	}
	else if ( ID_ZHUANG_TIAN_WANG == cbJettonArea )
	{
		if ( GetMaxBankerKingScore(wChairID) <lJettonScore ) return true;

		//保存下注
		m_lBankerKingScore += lJettonScore;
		m_lUserBankerKingScore[wChairID] += lJettonScore;
	}
	else
	{
		ASSERT(FALSE);
		return true;
	}

	//变量定义
	CMD_S_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

	//构造变量
	PlaceJetton.wChairID=wChairID;
	PlaceJetton.cbJettonArea=cbJettonArea;
	PlaceJetton.lJettonScore=lJettonScore;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

	return true;
}

//发送扑克
bool CTableFrameSink::DispatchTableCard()
{
//#ifdef _DEBUG
//	//平家胜利
//	BYTE cbIndex=0;
//	CopyMemory(m_cbCardCount,CardTieWinCard[cbIndex].cbCardCount,sizeof(m_cbCardCount));
//	CopyMemory(m_cbTableCardArray,CardTieWinCard[cbIndex].cbTableCardArray,sizeof(m_cbTableCardArray));
//#else
//	m_GameLogic.RandCardList(m_cbTableCardArray[0],sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0][0]));
//#endif

	m_GameLogic.RandCardList(m_cbTableCardArray[0],sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0][0]));
	//首次发牌
	m_cbCardCount[INDEX_PLAYER]=2;
	m_cbCardCount[INDEX_BANKER]=2;
	

	//计算点数
	BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);
	BYTE cbPlayerTwoCardCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);

	//闲家补牌
	BYTE cbPlayerThirdCardValue = 0 ;	//第三张牌点数
	if(cbPlayerTwoCardCount<=5 && cbBankerCount<8)
	{
		//计算点数
		m_cbCardCount[INDEX_PLAYER]++;
		cbPlayerThirdCardValue = m_GameLogic.GetCardPip(m_cbTableCardArray[INDEX_PLAYER][2]);
	}

	//庄家补牌
	if(cbPlayerTwoCardCount<8 && cbBankerCount<8)
	{
		switch(cbBankerCount)
		{
		case 0:
		case 1:
		case 2:
			m_cbCardCount[INDEX_BANKER]++ ;
			break;

		case 3:
			if((m_cbCardCount[INDEX_PLAYER]==3 && cbPlayerThirdCardValue!=8) || m_cbCardCount[INDEX_PLAYER]==2) m_cbCardCount[INDEX_BANKER]++ ;
			break;

		case 4:
			if((m_cbCardCount[INDEX_PLAYER]==3 && cbPlayerThirdCardValue!=1 && cbPlayerThirdCardValue!=8 && cbPlayerThirdCardValue!=9 && cbPlayerThirdCardValue!=0) || m_cbCardCount[INDEX_PLAYER]==2) m_cbCardCount[INDEX_BANKER]++ ;
			break;

		case 5:
			if((m_cbCardCount[INDEX_PLAYER]==3 && cbPlayerThirdCardValue!=1 && cbPlayerThirdCardValue!=2 && cbPlayerThirdCardValue!=3  && cbPlayerThirdCardValue!=8 && cbPlayerThirdCardValue!=9 &&  cbPlayerThirdCardValue!=0) || m_cbCardCount[INDEX_PLAYER]==2) m_cbCardCount[INDEX_BANKER]++ ;
			break;

		case 6:
			if(m_cbCardCount[INDEX_PLAYER]==3 && (cbPlayerThirdCardValue==6 || cbPlayerThirdCardValue==7)) m_cbCardCount[INDEX_BANKER]++ ;
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

	return true;
}

//盈利判断
LONGLONG CTableFrameSink::AccountPayoffScore()
{
	//计算点数
	BYTE cbPlayerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//盈利计算
	LONGLONG lPayoffScore=0L;
	if (cbPlayerCount>cbBankerCount) lPayoffScore=m_lTieScore+m_lBankerScore-m_lPlayerScore;
	else if (cbPlayerCount<cbBankerCount) lPayoffScore=m_lTieScore+m_lPlayerScore-m_lBankerScore;
	else lPayoffScore=-m_lTieScore*8L;

	return lPayoffScore;
}

//申请庄家
bool CTableFrameSink::OnUserApplyBanker( tagServerUserData *pUserData, bool bApplyBanker )
{
	//合法判断
	if ( bApplyBanker && pUserData->UserScoreInfo.lScore < m_lApplyBankerCondition ) return true;

	//保存玩家
	if ( bApplyBanker )
	{
		//存在判断
		for ( INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArrary.GetCount(); ++nUserIdx )
		{
			tagApplyUserInfo ApplyUserInfo = m_ApplyUserArrary[ nUserIdx ];
			if ( ApplyUserInfo.dwUserID == pUserData->dwUserID ) return true;
		}

		//构造信息
		tagApplyUserInfo ApplyUserInfo;
		ApplyUserInfo.dwUserID = pUserData->dwUserID;
		ApplyUserInfo.lUserScore = pUserData->UserScoreInfo.lScore;
		ApplyUserInfo.wChairID = pUserData->wChairID;

		//插入玩家
		INT_PTR nUserCount = m_ApplyUserArrary.GetCount();
		m_ApplyUserArrary.InsertAt( nUserCount, ApplyUserInfo );

		//构造变量
		CMD_S_ApplyBanker ApplyBanker;
		CopyMemory( ApplyBanker.szAccount, pUserData->szAccounts, sizeof( ApplyBanker.szAccount ) );
		ApplyBanker.lScore = pUserData->UserScoreInfo.lScore+pUserData->lStorageScore;
		ApplyBanker.bApplyBanker = true;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );

		return true;
	}
	else	//取消申请
	{
		//存在判断
		for ( INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArrary.GetCount(); ++nUserIdx )
		{
			tagApplyUserInfo ApplyUserInfo = m_ApplyUserArrary[ nUserIdx ];

			if ( ApplyUserInfo.dwUserID == pUserData->dwUserID )
			{
				//当前判断
				if ( m_CurrentBanker.dwUserID == ApplyUserInfo.dwUserID )
				{
					m_bCancelBanker = true;
					continue;
				}

				//删除玩家
				m_ApplyUserArrary.RemoveAt( nUserIdx );

				//构造变量
				CMD_S_ApplyBanker ApplyBanker;
				CopyMemory( ApplyBanker.szAccount, pUserData->szAccounts, sizeof( ApplyBanker.szAccount ) );
				ApplyBanker.lScore = pUserData->UserScoreInfo.lScore;
				ApplyBanker.bApplyBanker = false;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );

				return true;
			}
		}

		return true;
	}

	return true;

}

//发送庄家
void CTableFrameSink::SendApplyUser( IServerUserItem *pRcvServerUserItem )
{
	for ( INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArrary.GetCount(); ++nUserIdx )
	{
		tagApplyUserInfo ApplyUserInfo = m_ApplyUserArrary[ nUserIdx ];

		//获取玩家
		IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( ApplyUserInfo.wChairID );
		if ( ! pServerUserItem ) continue;

		//构造变量
		CMD_S_ApplyBanker ApplyBanker;
		CopyMemory( ApplyBanker.szAccount, pServerUserItem->GetAccounts(), sizeof( ApplyBanker.szAccount ) );
		ApplyBanker.lScore = pServerUserItem->GetUserScore()->lScore;
		ApplyBanker.bApplyBanker = true;

		//发送消息
		m_pITableFrame->SendUserData( pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );
	}
}

//更换庄家
void CTableFrameSink::ChangeBanker()
{
	//读取配置
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//读取配置
	TCHAR szFileName[MAX_PATH];
	_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\BaccaratConfig.ini"),szPath);

	//读取分数
	m_lApplyBankerCondition = GetPrivateProfileInt( TEXT("BankerCondition"), TEXT("Score"), 10000000, szFileName );
	m_lAreaLimitScore = GetPrivateProfileInt( TEXT("BankerCondition"), TEXT("AreaLimitScore"), 1000000000, szFileName );

	//递增次数
	m_cbBankerTimer++;

	//轮庄判断
	if ( m_CurrentBanker.dwUserID != 0 )
	{
		//轮换判断
		bool bChangeBanker = false;
		IServerUserItem *pBankerUserItem = m_pITableFrame->GetServerUserItem(m_CurrentBanker.wChairID);
		LONG lBankerScore = pBankerUserItem->GetUserScore()->lScore+pBankerUserItem->GetUserData()->lStorageScore;
		for ( WORD wApplyUserIndex = 0; wApplyUserIndex < m_ApplyUserArrary.GetCount(); ++wApplyUserIndex )
		{
				tagApplyUserInfo &ApplyUserInfo = m_ApplyUserArrary[ wApplyUserIndex ];
				IServerUserItem *pApplyUserItem = m_pITableFrame->GetServerUserItem(ApplyUserInfo.wChairID);
				LONG lApplyUserScore = pApplyUserItem->GetUserScore()->lScore+pApplyUserItem->GetUserData()->lStorageScore;
				if ( lBankerScore < lApplyUserScore )
				{
					bChangeBanker = true;
					break;
				}
		}

		//次数判断
		if ( bChangeBanker && 20 <= m_cbBankerTimer )
		{
			m_cbBankerTimer = 0;
			m_lBankerWinScore=0;
			m_bCancelBanker = false;

			//客户端删除
			IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( m_CurrentBanker.wChairID );
			OnUserApplyBanker( pServerUserItem->GetUserData(), false ) ;

			//重新赋值
			ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
			while( ! m_ApplyUserArrary.IsEmpty() )
			{
				m_CurrentBanker = m_ApplyUserArrary[ 0 ];
				//合法判断
				if ( m_CurrentBanker.lUserScore < m_lApplyBankerCondition )
				{
					ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );

					//获取玩家
					IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( m_ApplyUserArrary[ 0 ].wChairID );
					if (  pServerUserItem  ) OnUserApplyBanker( pServerUserItem->GetUserData(), false );
				}
				else
					break;
			}
		}
	}
	else if ( 0 < m_ApplyUserArrary.GetCount() )
	{
		while( ! m_ApplyUserArrary.IsEmpty() )
		{
			m_CurrentBanker = m_ApplyUserArrary[ 0 ];
			//合法判断
			if ( m_CurrentBanker.lUserScore < m_lApplyBankerCondition )
			{
				ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
				//获取玩家
				IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( m_ApplyUserArrary[ 0 ].wChairID );
				if (  pServerUserItem  ) OnUserApplyBanker( pServerUserItem->GetUserData(), false );
			}
			else
				break;
		}
		m_cbBankerTimer = 0;
		m_lBankerWinScore=0;
		m_bCancelBanker = false;
	}
}

//轮换庄家
void CTableFrameSink::SendChangeBankerMsg()
{
	CMD_S_ChangeBanker ChangeBanker;

	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		IServerUserItem *pSeverUserItem = m_pITableFrame->GetServerUserItem( wChairID );
		if ( ! pSeverUserItem ) continue;

		ZeroMemory( &ChangeBanker, sizeof( ChangeBanker ) );
		if ( m_CurrentBanker.dwUserID != 0 )
		{
			ChangeBanker.wChairID = m_CurrentBanker.wChairID;
			ChangeBanker.lBankerTreasure = m_CurrentBanker.lUserScore;
		}
		else
		{
			ChangeBanker.wChairID = INVALID_CHAIR;
		}
		ChangeBanker.lBankerScore = m_CurrentBanker.lUserScore;
		ChangeBanker.cbBankerTime = m_cbBankerTimer;

		//我的下注
		m_pITableFrame->SendTableData( wChairID, SUB_S_CHANGE_BANKER, &ChangeBanker, sizeof( ChangeBanker ) );
		m_pITableFrame->SendLookonData( wChairID, SUB_S_CHANGE_BANKER, &ChangeBanker, sizeof( ChangeBanker ) );
	}
}

//用户断线
bool __cdecl CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) 
{
	return true;
}

//最大下注
LONG CTableFrameSink::GetMaxPlayerScore(WORD wChairID)
{
	//其他区域
	LONG lOtherAreaScore = m_lBankerScore+ m_lTieScore+ m_lTieSamePointScore+ m_lBankerKingScore;

	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	LONG lMaxPlayerScore = m_CurrentBanker.lUserScore+lOtherAreaScore;
	lMaxPlayerScore -= (m_lPlayerScore+m_lPlayerKingScore*2);

	//已下注额
	LONG lNowJetton = 0;
	lNowJetton += m_lUserPlayerScore[wChairID];
	lNowJetton += m_lUserPlayerKingScore[wChairID];
	lNowJetton += m_lUserBankerScore[wChairID];
	lNowJetton += m_lUserTieScore[wChairID];
	lNowJetton += m_lUserTieSamePointScore[wChairID];
	lNowJetton += m_lUserBankerKingScore[wChairID];

	//我的下注
	LONG lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	ASSERT(lMeLessScore >= 0 );
	lMeLessScore = max(lMeLessScore, 0);

	//最大下注
	LONG lMaxJetton = min(lMaxPlayerScore, lMeLessScore);
	LONG lAreaLimit = m_lAreaLimitScore - m_lPlayerScore;
	lMaxJetton = min(lMaxJetton, lAreaLimit);

	return lMaxJetton;
}

//最大下注
LONG CTableFrameSink::GetMaxPlayerKingScore(WORD wChairID)
{
	//其他区域
	LONG lOtherAreaScore = m_lBankerScore+ m_lTieScore+ m_lTieSamePointScore+ m_lBankerKingScore;

	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	LONG lMaxPlayerScore = m_CurrentBanker.lUserScore+lOtherAreaScore;
	lMaxPlayerScore -= (m_lPlayerScore+m_lPlayerKingScore*2);

	//已下注额
	LONG lNowJetton = 0;
	lNowJetton += m_lUserPlayerScore[wChairID];
	lNowJetton += m_lUserPlayerKingScore[wChairID];
	lNowJetton += m_lUserBankerScore[wChairID];
	lNowJetton += m_lUserTieScore[wChairID];
	lNowJetton += m_lUserTieSamePointScore[wChairID];
	lNowJetton += m_lUserBankerKingScore[wChairID];

	//我的下注
	LONG lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	ASSERT(lMeLessScore >= 0 );
	lMeLessScore = max(lMeLessScore, 0);

	//最大下注
	LONG lMaxJetton = min(lMaxPlayerScore/2, lMeLessScore);
	LONG lAreaLimit = m_lAreaLimitScore - m_lPlayerKingScore;
	lMaxJetton = min(lMaxJetton, lAreaLimit);

	return lMaxJetton;
}

//最大下注
LONG CTableFrameSink::GetMaxBankerScore(WORD wChairID)
{
	//其他区域
	LONG lOtherAreaScore = m_lPlayerScore + m_lPlayerKingScore + m_lTieSamePointScore+ m_lTieScore;

	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	LONG lMaxPlayerScore = lOtherAreaScore + m_CurrentBanker.lUserScore;
	lMaxPlayerScore -= (m_lBankerScore+m_lBankerKingScore*2);

	//已下注额
	LONG lNowJetton = 0;
	lNowJetton += m_lUserPlayerScore[wChairID];
	lNowJetton += m_lUserPlayerKingScore[wChairID];
	lNowJetton += m_lUserBankerScore[wChairID];
	lNowJetton += m_lUserTieScore[wChairID];
	lNowJetton += m_lUserTieSamePointScore[wChairID];
	lNowJetton += m_lUserBankerKingScore[wChairID];

	//我的下注
	LONG lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	ASSERT(lMeLessScore >= 0 );
	lMeLessScore = max(lMeLessScore, 0);

	//最大下注
	LONG lMaxJetton = min(lMaxPlayerScore, lMeLessScore);
	LONG lAreaLimit = m_lAreaLimitScore - m_lBankerScore;
	lMaxJetton = min(lMaxJetton, lAreaLimit);

	return lMaxJetton;
}

//最大下注
LONG CTableFrameSink::GetMaxBankerKingScore(WORD wChairID)
{
	//其他区域
	LONG lOtherAreaScore = m_lPlayerScore + m_lPlayerKingScore + m_lTieSamePointScore+ m_lTieScore;

	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	LONG lMaxPlayerScore = lOtherAreaScore + m_CurrentBanker.lUserScore;
	lMaxPlayerScore -= (m_lBankerScore+m_lBankerKingScore*2);

	//已下注额
	LONG lNowJetton = 0;
	lNowJetton += m_lUserPlayerScore[wChairID];
	lNowJetton += m_lUserPlayerKingScore[wChairID];
	lNowJetton += m_lUserBankerScore[wChairID];
	lNowJetton += m_lUserTieScore[wChairID];
	lNowJetton += m_lUserTieSamePointScore[wChairID];
	lNowJetton += m_lUserBankerKingScore[wChairID];

	//我的下注
	LONG lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	ASSERT(lMeLessScore >= 0 );
	lMeLessScore = max(lMeLessScore, 0);

	//最大下注
	LONG lMaxJetton = min(lMaxPlayerScore/2, lMeLessScore);
	LONG lAreaLimit = m_lAreaLimitScore - m_lBankerKingScore;
	lMaxJetton = min(lMaxJetton, lAreaLimit);

	return lMaxJetton;
}

//最大下注
LONG CTableFrameSink::GetMaxTieScore(WORD wChairID)
{
	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	LONG lMaxTieScore = m_CurrentBanker.lUserScore;
	lMaxTieScore -= (m_lTieScore * 8 + m_lTieSamePointScore * 33);

	//已下注额
	LONG lNowJetton = 0;
	lNowJetton += m_lUserPlayerScore[wChairID];
	lNowJetton += m_lUserPlayerKingScore[wChairID];
	lNowJetton += m_lUserBankerScore[wChairID];
	lNowJetton += m_lUserTieScore[wChairID];
	lNowJetton += m_lUserTieSamePointScore[wChairID];
	lNowJetton += m_lUserBankerKingScore[wChairID];

	//我的下注
	LONG lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	ASSERT(lMeLessScore >= 0 );
	lMeLessScore = max(lMeLessScore, 0);

	//最大下注
	LONG lMaxJetton = min(lMaxTieScore/8, lMeLessScore);
	LONG lAreaLimit = m_lAreaLimitScore - m_lTieScore;
	lMaxJetton = min(lMaxJetton, lAreaLimit);

	return lMaxJetton;
}

//最大下注
LONG CTableFrameSink::GetMaxTieKingScore(WORD wChairID)
{
	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	LONG lMaxTieScore = m_CurrentBanker.lUserScore;
	lMaxTieScore -= (m_lTieScore * 8 + m_lTieSamePointScore * 33);

	//已下注额
	LONG lNowJetton = 0;
	lNowJetton += m_lUserPlayerScore[wChairID];
	lNowJetton += m_lUserPlayerKingScore[wChairID];
	lNowJetton += m_lUserBankerScore[wChairID];
	lNowJetton += m_lUserTieScore[wChairID];
	lNowJetton += m_lUserTieSamePointScore[wChairID];
	lNowJetton += m_lUserBankerKingScore[wChairID];

	//我的下注
	LONG lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	ASSERT(lMeLessScore >= 0 );
	lMeLessScore = max(lMeLessScore, 0);

	//最大下注
	LONG lMaxJetton = min(lMaxTieScore/33, lMeLessScore);
	LONG lAreaLimit = m_lAreaLimitScore - m_lTieSamePointScore;
	lMaxJetton = min(lMaxJetton, lAreaLimit);

	return lMaxJetton;
}

//计算得分
void CTableFrameSink::CalculateScore()
{
	//结束消息
	CMD_S_GameScore GameScore;
	ZeroMemory(&GameScore,sizeof(GameScore));

	//变量定义
	LONG cbRevenue=m_pGameServiceOption->wRevenue;

	//计算牌点
	BYTE cbPlayerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//推断玩家
	DeduceWinner(GameScore.cbWinner, GameScore.cbKingWinner);

	//游戏记录
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.cbBankerCount = cbBankerCount;
	GameRecord.cbPlayerCount = cbPlayerCount;
	GameRecord.lBankerScore = m_lBankerScore;
	GameRecord.lPlayerScore = m_lPlayerScore;
	GameRecord.lTieScore = m_lTieScore;
	GameRecord.wWinner = GameScore.cbWinner;

	//移动下标
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst ) m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;

	//庄家总量
	LONG lBankerWinScore = 0;

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONG lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//玩家下注
	LONG *pUserScore[] = {NULL, m_lUserPlayerScore, m_lUserTieScore, m_lUserBankerScore, m_lUserPlayerKingScore, 
		m_lUserBankerKingScore, m_lUserTieSamePointScore};
	//区域倍率
	BYTE cbMultiple[] = {0, 1, 8, 1, 2, 2, 33};

	//计算积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//庄家判断
		if ( m_CurrentBanker.dwUserID != 0 && m_CurrentBanker.wChairID == i ) continue;

		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
		if (pIServerUserItem==NULL) continue;

		for ( WORD wAreaIndex = ID_XIAN_JIA; wAreaIndex <= ID_TONG_DIAN_PING; ++wAreaIndex )
		{
			if ( wAreaIndex == GameScore.cbWinner || wAreaIndex == GameScore.cbKingWinner ) 
			{
				m_lUserWinScore[i] += ( pUserScore[wAreaIndex][i] * cbMultiple[wAreaIndex] ) ;
				m_lUserReturnScore[i] += pUserScore[wAreaIndex][i] ;
				lBankerWinScore -= ( pUserScore[wAreaIndex][i] * cbMultiple[wAreaIndex] ) ;
			}
			else if ( GameScore.cbWinner == ID_PING_JIA )
			{
				m_lUserReturnScore[i] += pUserScore[wAreaIndex][i] ;
			}
			else
			{
				lUserLostScore[i] -= pUserScore[wAreaIndex][i] ;
				lBankerWinScore += pUserScore[wAreaIndex][i] ;
			}
		}

		//计算税收
		if ( 0 < m_lUserWinScore[i] )
		{
			m_lUserRevenue[i]  = m_lUserWinScore[i]*cbRevenue/100L;
			m_lUserWinScore[i] -= m_lUserRevenue[i];
		}

		//总的分数
		m_lUserWinScore[i] += lUserLostScore[i];
	}

	//庄家成绩
	if ( m_CurrentBanker.dwUserID != 0 )
	{
		WORD wBankerChairID = m_CurrentBanker.wChairID;
		m_lUserWinScore[wBankerChairID] = lBankerWinScore;

		//计算税收
		if ( 0 < m_lUserWinScore[wBankerChairID] )
		{
			m_lUserRevenue[wBankerChairID]  = m_lUserWinScore[wBankerChairID]*cbRevenue/100L;
			m_lUserWinScore[wBankerChairID] -= m_lUserRevenue[wBankerChairID];
			lBankerWinScore = m_lUserWinScore[wBankerChairID];
		}
		IServerUserItem *pBankerUserItem = m_pITableFrame->GetServerUserItem(wBankerChairID);

		//累计积分
		m_lBankerWinScore += lBankerWinScore;
	}

	//买注总量
	GameScore.lDrawTieScore=m_lTieScore;
	GameScore.lDrawBankerScore=m_lBankerScore;
	GameScore.lDrawPlayerScore=m_lPlayerScore;
	GameScore.lDrawTieSamPointScore = m_lTieSamePointScore;
	GameScore.lDrawBankerKingScore = m_lBankerKingScore;
	GameScore.lDrawPlayerKingScore = m_lPlayerKingScore;

	GameScore.lBankerScore = lBankerWinScore;

	//发送积分
	for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(wUserIndex);
		if ( pIServerUserItem == NULL ) continue;

		//我的下注
		GameScore.lMeTieScore=m_lUserTieScore[wUserIndex];
		GameScore.lMeBankerScore=m_lUserBankerScore[wUserIndex];
		GameScore.lMePlayerScore=m_lUserPlayerScore[wUserIndex];
		GameScore.lMeTieKingScore = m_lUserTieSamePointScore[wUserIndex];
		GameScore.lMeBankerKingScore = m_lUserBankerKingScore[wUserIndex];
		GameScore.lMePlayerKingScore = m_lUserPlayerKingScore[wUserIndex];

		//发送消息
		GameScore.lMeGameScore=m_lUserWinScore[wUserIndex];
		GameScore.lMeReturnScore = m_lUserReturnScore[wUserIndex];
		m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_SCORE,&GameScore,sizeof(GameScore));
		m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_SCORE,&GameScore,sizeof(GameScore));
	}
	return ;
}

//推断赢家
void CTableFrameSink::DeduceWinner(BYTE &cbWinner, BYTE &cbKingWinner)
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

