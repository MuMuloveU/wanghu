#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//宏定义
#define	GOOD_CARD_PERCENT				4									//好牌概率

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wBombTime=1;
	m_bLandScore=0;
	m_wFirstUser=rand()%GAME_PLAYER;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));

	//疯狂变量
	m_bBrightCard=false;
	m_bCallScorePhase=CSD_NORMAL;
	ZeroMemory( m_bGiveUpBanker, sizeof( m_bGiveUpBanker ) );
	ZeroMemory( m_bBrightStart, sizeof( m_bBrightStart ) );
	m_cbBrightCardTime = 1;
	m_cbDoubleCount=0;
	ZeroMemory( m_bDoubleUser, sizeof( m_bDoubleUser ) );
	m_bAllowDoubleScore = false;
	ZeroMemory(m_bUserBrightCard, sizeof(m_bUserBrightCard));
	m_cbBrightCardCount=0;

	//运行信息
	m_bTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

	//扑克信息
	memset(m_bBackCard,0,sizeof(m_bBackCard));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bOutCardCount,0,sizeof(m_bOutCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

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

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}

//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	//游戏变量
	m_wBombTime=1;
	m_bLandScore=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));

	//疯狂变量
	m_bBrightCard=false;
	m_bCallScorePhase=CSD_NORMAL;
	ZeroMemory( m_bGiveUpBanker, sizeof( m_bGiveUpBanker ) );
	ZeroMemory( m_bBrightStart, sizeof( m_bBrightStart ) );
	m_cbBrightCardTime = 1;
	m_cbDoubleCount=0;
	ZeroMemory( m_bDoubleUser, sizeof( m_bDoubleUser ) );
	ZeroMemory(m_bUserBrightCard, sizeof(m_bUserBrightCard));
	m_cbBrightCardCount=0;

	//运行信息
	m_bTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

	//扑克信息
	memset(m_bBackCard,0,sizeof(m_bBackCard));
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bOutCardCount,0,sizeof(m_bOutCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

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
	//疯狂变量
	ZeroMemory( m_bGiveUpBanker, sizeof( m_bGiveUpBanker ) );

	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_SCORE);

	BYTE cbSendGoodCard = rand () % GOOD_CARD_PERCENT ;
	BYTE cbAndroidUserCount = 0 ;
	WORD wAndroidUser[ GAME_PLAYER ] = {0};

	//机器人个数
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( wChairID ) ;
		if ( pServerUserItem->IsAndroidUser() )
		{
			wAndroidUser[ cbAndroidUserCount ] = ( wChairID ) ;
			++cbAndroidUserCount ;
		}
	}

	//明牌次数
	m_cbBrightCardCount = cbAndroidUserCount;

	WORD wHaveGoodCardAndroidUser = INVALID_CHAIR ;
	if ( 0 < cbAndroidUserCount ) wHaveGoodCardAndroidUser = wAndroidUser[ rand() % cbAndroidUserCount ] ;

	//发送好牌
	if ( 1 == cbSendGoodCard && wHaveGoodCardAndroidUser != INVALID_CHAIR )
	{
		//混乱扑克
		BYTE bRandCard[54];
		m_GameLogic.RandCardList( bRandCard, sizeof( bRandCard ) / sizeof( bRandCard[ 0 ] ) );

		BYTE cbGoodCard[ NORMAL_COUNT ] ;
		m_GameLogic.GetGoodCardData( cbGoodCard ) ;

		//抽取好牌
		m_GameLogic.RemoveGoodCardData( cbGoodCard, NORMAL_COUNT, bRandCard, FULL_COUNT ) ;

		//分发扑克
		CopyMemory( m_bHandCardData[ wHaveGoodCardAndroidUser ], cbGoodCard, NORMAL_COUNT ) ;
		m_bCardCount[ wHaveGoodCardAndroidUser ] = 17;
		for ( WORD i = 0, j = 0; i < m_wPlayerCount; i++ )
		{
			if ( i != wHaveGoodCardAndroidUser )
			{
				m_bCardCount[ i ] = 17;
				CopyMemory( &m_bHandCardData[ i ], &bRandCard[ j * 17 ], sizeof( BYTE ) * 17 );
				++j ;
			}
		}
		CopyMemory( m_bBackCard, &bRandCard[ 34 ], sizeof( m_bBackCard ) );
		m_GameLogic.SortCardList( m_bBackCard, 3, ST_ORDER );
	}
	else
	{
		//混乱扑克
		BYTE bRandCard[ 54 ];

		m_GameLogic.RandCardList( bRandCard, sizeof( bRandCard ) / sizeof( bRandCard[ 0 ] ) );

		//分发扑克
		for ( WORD i = 0; i < m_wPlayerCount; i++ )
		{
			m_bCardCount[ i ] = 17;
			CopyMemory( &m_bHandCardData[ i ], &bRandCard[ i * m_bCardCount[ i ] ], sizeof( BYTE ) * m_bCardCount[ i ] );
		}
		CopyMemory( m_bBackCard, &bRandCard[ 51 ], sizeof( m_bBackCard ) );
		m_GameLogic.SortCardList( m_bBackCard, 3, ST_ORDER );
	}

	//设置用户
	if ( m_wCurrentUser == INVALID_CHAIR ) m_wCurrentUser = m_wFirstUser;

	//发送扑克
	CMD_S_SendAllCard SendAllCard ;
	SendAllCard.wCurrentUser = m_wCurrentUser ;

	//亮牌信息
	CopyMemory(SendAllCard.bUserBrightCard, m_bUserBrightCard, sizeof(SendAllCard.bUserBrightCard));

	//手上扑克
	CopyMemory( SendAllCard.bCardData, m_bHandCardData, sizeof( m_bHandCardData ) ) ;
	//底牌扑克
	CopyMemory( SendAllCard.bBackCardData, m_bBackCard, 3 ) ;
	for ( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		m_pITableFrame->SendTableData( i, SUB_S_SEND_CARD, &SendAllCard, sizeof( SendAllCard ) );
		m_pITableFrame->SendLookonData( i, SUB_S_SEND_CARD, &SendAllCard, sizeof( SendAllCard ) );
	}

	//排列扑克
	for ( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		m_GameLogic.SortCardList( m_bHandCardData[ i ], m_bCardCount[ i ], ST_ORDER );
	}

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd( WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason )
{
	switch ( cbReason )
	{
	case GER_DISMISS:		//游戏解散
		{
			//效验参数
			ASSERT( pIServerUserItem != NULL );
			ASSERT( wChairID < m_wPlayerCount );

			//构造数据
			CMD_S_GameEnd GameEnd;
			memset( &GameEnd, 0, sizeof( GameEnd ) );

			//剩余扑克
			BYTE bCardPos = 0;
			for ( WORD i = 0; i < m_wPlayerCount; i++ )
			{
				GameEnd.bCardCount[ i ] = m_bCardCount[ i ];
				CopyMemory( &GameEnd.bCardData[ bCardPos ], m_bHandCardData[ i ], m_bCardCount[ i ] * sizeof( BYTE ) );
				bCardPos += m_bCardCount[ i ];
			}

			//发送信息
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof( GameEnd ) );
			m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof( GameEnd ) );

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//变量定义
			LONG lCellScore=m_pGameServiceOption->lCellScore;
			bool bLandWin=(m_bCardCount[m_wBankerUser]==0)?true:false;

			//翻倍计算
			WORD wDrawTimes=(WORD)m_pGameServiceOption->lCellScore;
			for (WORD i=1;i<m_bLandScore;i++) wDrawTimes*=2;

			//明牌倍数
			if ( m_cbBrightCardTime != 0 ) wDrawTimes *= m_cbBrightCardTime;

			//春天判断
			if ( wChairID == m_wBankerUser )
			{
				WORD wUser1 = ( m_wBankerUser + 1 ) % GAME_PLAYER;
				WORD wUser2 = ( m_wBankerUser + 2 ) % GAME_PLAYER;
				if ( ( m_bOutCardCount[ wUser1 ] == 0 ) && ( m_bOutCardCount[ wUser2 ] == 0 ) ) wDrawTimes *= 2;
			}
			else
			{
				if ( m_bOutCardCount[ m_wBankerUser ] == 1 ) wDrawTimes *= 2;
			}

			//计算积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (i==m_wBankerUser) GameEnd.lGameScore[i]=m_wBombTime*wDrawTimes*((bLandWin==true)?2:-2);
				else GameEnd.lGameScore[i]=m_wBombTime*wDrawTimes*((bLandWin==true)?-1:1);
			}

			//积分平衡
			if ( m_pGameServiceOption->wServerType&GAME_GENRE_GOLD )
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (GameEnd.lGameScore[i]<0L)
					{
						IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
						if ((pIServerUserItem->GetUserScore()->lScore+GameEnd.lGameScore[i])<0L)
						{
							//金币调整
							if (i==m_wBankerUser)
							{
								for (WORD j=0;j<m_wPlayerCount;j++)
								{
									if (j!=m_wBankerUser) 
									{
										GameEnd.lGameScore[j]=pIServerUserItem->GetUserScore()->lScore/2;
									}
								}
							}
							else
							{
								GameEnd.lGameScore[m_wBankerUser]-=(-GameEnd.lGameScore[i]-pIServerUserItem->GetUserScore()->lScore);
							}

							//结果调整
							GameEnd.lGameScore[i]=-pIServerUserItem->GetUserScore()->lScore;
						}
					}
				}
			}

			//统计积分
			LONG lScore=0;
			LONG lRevenue=0;
			enScoreKind ScoreKind;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//计算税收
				lRevenue=0;
				if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD && GameEnd.lGameScore[i]>=100L)
				{
					GameEnd.lGameTax+=GameEnd.lGameScore[i]/50L;
					GameEnd.lGameScore[i]=GameEnd.lGameScore[i]*49L/50L;
					lRevenue = GameEnd.lGameScore[i]/50L;
				}
				lScore = GameEnd.lGameScore[i];
				ScoreKind = lScore > 0 ? enScoreKind_Win : enScoreKind_Lost;
				m_pITableFrame->WriteUserScore(i, lScore, lRevenue, ScoreKind );
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//切换用户
			m_wFirstUser=wChairID;

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//翻倍计算
			WORD wDrawTimes=(WORD)m_pGameServiceOption->lCellScore;
			for (WORD i=1;i<m_bLandScore;i++) wDrawTimes*=2;

			//炸弹计算
			for ( WORD wUserID = 0; wUserID < GAME_PLAYER; ++wUserID )
			{
				//分析扑克
				tagAnalyseResult AnalyseResult ;
				ZeroMemory(&AnalyseResult, sizeof( AnalyseResult ) );
				m_GameLogic.AnalysebCardData(m_bHandCardData[wUserID], m_bCardCount[wUserID], AnalyseResult) ;

				//炸弹翻倍
				for (WORD i=1;i<AnalyseResult.cbFourCount;i++) wDrawTimes*=2;

				//天炸判断
				if ( m_bHandCardData[wUserID][0] == 0x4F && m_bHandCardData[wUserID][1] == 0x4E ) wDrawTimes *= 2;
			}

			//明牌倍数
			if ( m_cbBrightCardTime != 0 ) wDrawTimes *= m_cbBrightCardTime;

			//已出炸弹
			wDrawTimes *= m_wBombTime;

			//倍数判断
			if ( wDrawTimes < 10 )
			{
				if ( wChairID == m_wBankerUser ) wDrawTimes = 20;
				else wDrawTimes = 10;
			}
			else
			{
				if ( wChairID == m_wBankerUser ) wDrawTimes *= 4;
				else wDrawTimes *= 2;
			}

			//构造数据
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			GameEnd.lGameScore[wChairID] = -wDrawTimes;

			//金币房间
			if ( m_pGameServiceOption->wServerType&GAME_GENRE_GOLD )
			{	
				//金币平衡
				IServerUserItem *pDrawServerUserItem = m_pITableFrame->GetServerUserItem( wChairID );
				if ( pDrawServerUserItem->GetUserScore()->lScore + GameEnd.lGameScore[wChairID] < 0 ) 
					GameEnd.lGameScore[wChairID] = - pDrawServerUserItem->GetUserScore()->lScore;

				//计算积分
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if ( i != wChairID ) GameEnd.lGameScore[i] = (-GameEnd.lGameScore[wChairID])/2;
				}

				//统计积分
				LONG lScore=0;
				LONG lRevenue=0;
				enScoreKind ScoreKind;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//计算税收
					lRevenue=0;
					if ( GameEnd.lGameScore[i]>=100L)
					{
						GameEnd.lGameTax+=GameEnd.lGameScore[i]/50L;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]*49L/50L;
						lRevenue = GameEnd.lGameScore[i]/50L;
					}
					lScore = GameEnd.lGameScore[i];
					ScoreKind = lScore > 0 ? enScoreKind_Win : enScoreKind_Lost;
					m_pITableFrame->WriteUserScore(i, lScore, lRevenue, ScoreKind );
				}
			}

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			m_pITableFrame->WriteUserScore(wChairID,GameEnd.lGameScore[wChairID],0,enScoreKind_Draw);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_WK_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//设置变量
			StatusFree.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory( StatusFree.bBrightStart, m_bBrightStart, sizeof( StatusFree.bBrightStart ) );

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_WK_SCORE:		//叫分状态
		{
			//构造数据
			CMD_S_StatusScore StatusScore;
			memset(&StatusScore,0,sizeof(StatusScore));

			//设置变量
			StatusScore.bLandScore=m_bLandScore;
			StatusScore.wCurrentUser=m_wCurrentUser;
			StatusScore.lBaseScore=m_pGameServiceOption->lCellScore;
			StatusScore.bCallScorePhase=m_bCallScorePhase;
			CopyMemory(StatusScore.bScoreInfo,m_bScoreInfo,sizeof(m_bScoreInfo));
	//		CopyMemory(StatusScore.bCardData,m_bHandCardData[wChiarID],m_bCardCount[wChiarID]);
			CopyMemory(StatusScore.bCardData,m_bHandCardData, sizeof(m_bHandCardData)*sizeof(BYTE));
			CopyMemory( StatusScore.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			StatusScore.bBrightTime = m_cbBrightCardTime;
			CopyMemory( StatusScore.bUserBrightCard, m_bUserBrightCard, sizeof( StatusScore.bUserBrightCard ) );

			//明牌判断
			if ( m_bCallScorePhase == CSD_BRIGHTCARD && m_wCurrentUser!=wChiarID && m_bUserBrightCard[wChiarID]==false ) m_cbBrightCardCount++;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_WK_DOUBLE_SCORE:	//加倍状态
		{
			//构造消息
			CMD_S_StatusDoubleScore StatusDoubleScore;
			ZeroMemory( &StatusDoubleScore, sizeof( StatusDoubleScore ) );
			StatusDoubleScore.bAllowDouble = m_bAllowDoubleScore;
			CopyMemory(StatusDoubleScore.bBackCard, m_bBackCard, sizeof(m_bBackCard ) );
			CopyMemory(StatusDoubleScore.bCardCount, m_bCardCount, sizeof(m_bCardCount ) );
			CopyMemory(StatusDoubleScore.bCardData, m_bHandCardData, sizeof(StatusDoubleScore.bCardData ) );
			StatusDoubleScore.bLandScore = m_bLandScore;
			CopyMemory(StatusDoubleScore.bUserTrustee, m_bUserTrustee, sizeof(StatusDoubleScore.bUserTrustee ) );	
			StatusDoubleScore.lBaseScore = m_pGameServiceOption->lCellScore;
			StatusDoubleScore.wLandUser = m_wBankerUser;
			CopyMemory(StatusDoubleScore.bDoubleUser, m_bDoubleUser, sizeof(StatusDoubleScore.bDoubleUser ) );	
			CopyMemory( StatusDoubleScore.bUserBrightCard, m_bUserBrightCard, sizeof( StatusDoubleScore.bUserBrightCard ) );


			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusDoubleScore,sizeof(StatusDoubleScore));
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//设置变量
			StatusPlay.wBombTime=m_wBombTime;
			StatusPlay.wLandUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.wLastOutUser=m_wTurnWiner;
			StatusPlay.bLandScore=m_bLandScore;
			StatusPlay.bTurnCardCount=m_bTurnCardCount;
			StatusPlay.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.bBackCard,m_bBackCard,sizeof(m_bBackCard));
			CopyMemory(StatusPlay.bCardCount,m_bCardCount,sizeof(m_bCardCount));
			CopyMemory(StatusPlay.bTurnCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
	//		CopyMemory(StatusPlay.bCardData,m_bHandCardData[wChiarID],sizeof(BYTE)*m_bCardCount[wChiarID]);
			CopyMemory(StatusPlay.bCardData,m_bHandCardData,sizeof(BYTE)*sizeof(m_bHandCardData));
			CopyMemory( StatusPlay.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			StatusPlay.bBrightTime = m_cbBrightCardTime;
			CopyMemory( StatusPlay.bUserBrightCard, m_bUserBrightCard, sizeof( StatusPlay.bUserBrightCard ) );

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	return false;
}

//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_LAND_SCORE:	//用户叫分
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_LandScore));
			if (wDataSize!=sizeof(CMD_C_LandScore)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_LandScore * pLandScore=(CMD_C_LandScore *)pDataBuffer;
			return OnUserLandScore(pUserData->wChairID,pLandScore->bLandScore);
		}
	case SUB_C_OUT_CART:		//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->bCardData);

			//效验数据
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserOutCard(pUserData->wChairID,pOutCard->bCardData,pOutCard->bCardCount);
		}
	case SUB_C_PASS_CARD:
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserPassCard(pUserData->wChairID);
		}
	case SUB_C_TRUSTEE:			//玩家托管
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_UserTrustee));
			if (wDataSize != sizeof(CMD_C_UserTrustee)) return false;

			//变量定义
			CMD_C_UserTrustee *pUserTrustee = ( CMD_C_UserTrustee * )pDataBuffer;

			//消息处理
			return OnUserTrustee(pUserTrustee->wUserChairID, pUserTrustee->bTrustee);			
		}
	case SUB_C_BRIGHT_START:	//明牌开始
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			
			//消息处理
			return OnBrightStart( pUserData->wChairID, true );
		}
	case SUB_C_BRIGHT:		//玩家明牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_BrightCard));
			if (wDataSize != sizeof(CMD_C_BrightCard)) return false;			

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			
			//变量定义
			CMD_C_BrightCard *pBrightCard = ( CMD_C_BrightCard * )pDataBuffer;

			//消息处理
			return OnBrightCard( pUserData->wChairID, pBrightCard->cbBrightCardTime);
		}
	case SUB_C_DOUBLE_SCORE:	//玩家加倍
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_DoubleScore));
			if (wDataSize != sizeof(CMD_C_DoubleScore)) return false;			

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			
			//变量定义
			CMD_C_DoubleScore *pDoubleScore = ( CMD_C_DoubleScore * )pDataBuffer;

			//消息处理
			return OnUserDoubleScore( pUserData->wChairID, pDoubleScore->bDoubleScore);
		}
	}

	return true;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//叫分事件
bool CTableFrameSink::OnUserLandScore(WORD wChairID, BYTE bLandScore)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_SCORE) return true;

	//变量定义
	bool bContinue=false;
	BYTE cbPreCallScorePhase = m_bCallScorePhase;

	//叫分处理
	if (m_bCallScorePhase==CSD_NORMAL)
	{
		//效验参数
		ASSERT(((bLandScore<=3)||(bLandScore==255))&&(bLandScore>m_bLandScore));
		if (((bLandScore>3)&&(bLandScore!=255))||(bLandScore<=m_bLandScore)) return false;

		//设置变量
		if (bLandScore!=255)
		{
		//	m_bLandScore=bLandScore;
			m_bLandScore=1;
			m_wBankerUser=m_wCurrentUser;
		}
		else
		{
			bContinue = true;
			m_bGiveUpBanker[wChairID] = true;

			//切换玩家
			m_wCurrentUser=(wChairID+1)%m_wPlayerCount;

			//重新发牌
			if ( m_bGiveUpBanker[m_wCurrentUser] )
			{
				//重新发牌
				if ( m_wBankerUser == INVALID_CHAIR )
				{
					//疯狂变量
					m_bBrightCard=false;
					m_bCallScorePhase=CSD_NORMAL;
					ZeroMemory( m_bGiveUpBanker, sizeof( m_bGiveUpBanker ) );

					BYTE cbSendGoodCard = rand() % GOOD_CARD_PERCENT ;
					BYTE cbAndroidUserCount=0 ;
					WORD wAndroidUser[GAME_PLAYER]={0};

					//重置变量
					ZeroMemory(m_bScoreInfo, sizeof(m_bScoreInfo)) ;

					//机器人个数
					for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
					{
						IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID) ;
						if(pServerUserItem->IsAndroidUser())
						{
							wAndroidUser[cbAndroidUserCount] = (wChairID) ;
							++cbAndroidUserCount ;

						}
					}

					WORD wHaveGoodCardAndroidUser = INVALID_CHAIR ;
					if(cbAndroidUserCount>0) wHaveGoodCardAndroidUser = wAndroidUser[rand()%cbAndroidUserCount] ;

					//发送好牌
					if( 0 == cbSendGoodCard && wHaveGoodCardAndroidUser!=INVALID_CHAIR)
					{
						//混乱扑克
						BYTE bRandCard[54];
						m_GameLogic.RandCardList(bRandCard,sizeof(bRandCard)/sizeof(bRandCard[0]));

						BYTE cbGoodCard[NORMAL_COUNT] ;
						m_GameLogic.GetGoodCardData(cbGoodCard) ;

						//抽取好牌
						m_GameLogic.RemoveGoodCardData(cbGoodCard, NORMAL_COUNT, bRandCard, FULL_COUNT ) ;

						//分发扑克
						CopyMemory(m_bHandCardData[wHaveGoodCardAndroidUser], cbGoodCard, NORMAL_COUNT) ;
						m_bCardCount[wHaveGoodCardAndroidUser]=17;
						for (WORD i=0, j=0;i<m_wPlayerCount;i++)
						{
							if(i!=wHaveGoodCardAndroidUser)
							{
								m_bCardCount[i]=17;
								CopyMemory(&m_bHandCardData[i],&bRandCard[j*17],sizeof(BYTE)*17);
								++j ;
							}
						}
						CopyMemory(m_bBackCard,&bRandCard[34],sizeof(m_bBackCard));
						m_GameLogic.SortCardList(m_bBackCard,3,ST_ORDER);
					}
					else
					{
						//混乱扑克
						BYTE bRandCard[54];
						m_GameLogic.RandCardList(bRandCard,sizeof(bRandCard)/sizeof(bRandCard[0]));

						//分发扑克
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							m_bCardCount[i]=17;
							CopyMemory(&m_bHandCardData[i],&bRandCard[i*m_bCardCount[i]],sizeof(BYTE)*m_bCardCount[i]);
						}
						CopyMemory(m_bBackCard,&bRandCard[51],sizeof(m_bBackCard));
						m_GameLogic.SortCardList(m_bBackCard,3,ST_ORDER);
					}

					//设置用户
					m_wCurrentUser=m_wFirstUser;

					//发送扑克
					CMD_S_SendAllCard SendAllCard ;
					ZeroMemory(&SendAllCard, sizeof(SendAllCard));
					SendAllCard.wCurrentUser = m_wCurrentUser ;

					//手上扑克
					CopyMemory(SendAllCard.bCardData, m_bHandCardData, sizeof(m_bHandCardData)) ;
					//底牌扑克
					CopyMemory(SendAllCard.bBackCardData, m_bBackCard, 3) ;
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendAllCard,sizeof(SendAllCard));
						m_pITableFrame->SendLookonData(i,SUB_S_SEND_CARD,&SendAllCard,sizeof(SendAllCard));
					}

					//排列扑克
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						m_GameLogic.SortCardList(m_bHandCardData[i],m_bCardCount[i],ST_ORDER);
					}

					return true;
				}
				else
				{
					//设置玩家
					m_wCurrentUser = m_wBankerUser;

					//切换状态
					m_bCallScorePhase=CSD_GAMESTART;

					//设置变量
					bContinue = false;
				}
			}
		}

		m_bScoreInfo[wChairID]=bLandScore;

		//状态切换
		if (m_bLandScore==1)
		{
			bContinue=true;
			m_wFirstUser=wChairID;
			m_bCallScorePhase=CSD_SNATCHLAND;

			//切换玩家
			m_wCurrentUser = INVALID_CHAIR;
			WORD wUser = (wChairID+1)%GAME_PLAYER;
			while (  wUser != wChairID ) 
			{
				if ( m_bGiveUpBanker[wUser] == false )
				{
					m_wCurrentUser = wUser;
					break;
				}
				wUser = (wUser+1)%GAME_PLAYER ;
			}

			if ( m_wCurrentUser == INVALID_CHAIR )
			{
				m_wCurrentUser = wChairID;
				bContinue = false;
			}
		}
	}
	else if (m_bCallScorePhase==CSD_SNATCHLAND)
	{
		//效验参数
		ASSERT((bLandScore==4)||(bLandScore==255));
		if ((bLandScore!=4)&&(bLandScore!=255)) return false;

		//设置变量
		if (bLandScore==4)
		{
			m_bLandScore++;
			m_wBankerUser=wChairID;
		}

		//状态切换
		if (m_wFirstUser==wChairID)
		{
			bContinue=true;
			m_wFirstUser=m_wBankerUser;
			m_wCurrentUser = m_wBankerUser;

			//明牌判断
			if ( m_cbBrightCardTime > 2 && m_bUserBrightCard[m_wCurrentUser] )
			{
				m_bCallScorePhase = CSD_GAMESTART;
				bContinue = false;
			}
			else
				m_bCallScorePhase=CSD_BRIGHTCARD;
		}
		else bContinue=true;

		//切换玩家
		if ( m_bCallScorePhase == CSD_SNATCHLAND )
		{
			m_wCurrentUser = INVALID_CHAIR;
			WORD wUser = (wChairID+1)%GAME_PLAYER;
			while (  wUser != wChairID && wUser != (m_wFirstUser+1)%GAME_PLAYER ) 
			{
				if ( m_bGiveUpBanker[wUser] == false )
				{
					m_wCurrentUser = wUser;
					break;
				}
				wUser = (wUser+1)%GAME_PLAYER ;
			}

			if ( m_wCurrentUser == INVALID_CHAIR || ( m_wFirstUser == m_wBankerUser && m_wFirstUser == m_wCurrentUser ))
			{
				m_wCurrentUser = m_wBankerUser;
				bContinue=true;
				m_wFirstUser=m_wBankerUser;

				//明牌判断
				bool bAllUserBright = true;
				for ( WORD wBrightIdx = 0; wBrightIdx < GAME_PLAYER; ++wBrightIdx )
					if ( ! m_bUserBrightCard[wBrightIdx] ) bAllUserBright = false;

				//明牌判断
				if ( m_cbBrightCardTime > 2 && bAllUserBright )
				{
					m_bCallScorePhase = CSD_GAMESTART;
					bContinue = false;
				}
				else
					m_bCallScorePhase=CSD_BRIGHTCARD;
				}
		}
	}
	else if (m_bCallScorePhase==CSD_BRIGHTCARD)
	{
		////效验参数
		//ASSERT((bLandScore==5)||(bLandScore==255));
		//if ((bLandScore!=5)&&(bLandScore!=255)) return false;

		////增加次数
		//m_cbBrightCardCount++;
		//if ( m_cbBrightCardCount == GAME_PLAYER )
		//{
		//	bContinue = false;
		//	m_bCallScorePhase = CSD_BRIGHTCARD;
		//}
		//else 
		//{
		//	bContinue = true;
		//	m_bCallScorePhase=CSD_GAMESTART;
		//}

		////设置变量
		//m_bBrightCard=(bLandScore==5)?true:false;
		//if ( bLandScore == 5 )
		//{
		//	m_bLandScore++;
		//	m_bUserBrightCard[wChairID] = true;
		//}
		bContinue = true;
	}

	//开始判断
	if ((bContinue==false)&&(m_bLandScore>=1 || m_wBankerUser != INVALID_CHAIR))
	{
		//切换状态
		m_bCallScorePhase = CSD_BRIGHTCARD;
	}


	//明牌状态
	if ( m_bCallScorePhase == CSD_BRIGHTCARD )
	{
		//明牌个数
		for ( WORD wBrightIdx = 0; wBrightIdx < GAME_PLAYER; ++wBrightIdx ) 
			if ( m_bUserBrightCard[wBrightIdx] ) ++m_cbBrightCardCount;
;
	}

	//开始判断
	if ( m_cbBrightCardCount == GAME_PLAYER  && CSD_BRIGHTCARD ==m_bCallScorePhase )
	{
		//设置状态
		m_pITableFrame->SetGameStatus(GS_WK_DOUBLE_SCORE);

		//获取路径
		CHAR szDirectory[128];
		GetCurrentDirectory(sizeof( szDirectory ), szDirectory );

		//文件名称
		CHAR szProfileName[128];
		sprintf(szProfileName, TEXT("%s\\LandCrazy.ini"), szDirectory);

		//读取配置
		LONG lDoubleLimit = GetPrivateProfileInt(TEXT("DoubleScore"), TEXT("Score"), 10000, szProfileName );

		bool bAllowDouble = true;
		for ( WORD wUserID = 0; wUserID < GAME_PLAYER; ++wUserID )
		{
			//获取玩家
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem( wUserID );
			if ( pIServerUserItem == NULL ) continue;

			//条件判断
			LONG lUserScore = pIServerUserItem->GetUserScore()->lScore;
			if ( lUserScore < lDoubleLimit )
			{
				bAllowDouble = false;
				break;
			}
		}
		m_bAllowDoubleScore = bAllowDouble;

		//发送底牌
		m_bCardCount[m_wBankerUser]=20;
		CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
		m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);

		//设置消息
		CMD_S_DoubleScore DoubleScore;
		ZeroMemory( &DoubleScore, sizeof( DoubleScore ) );
		DoubleScore.bAllowDouble = bAllowDouble;
		DoubleScore.wLandUser = m_wBankerUser;
		DoubleScore.bCurrentScore = m_bLandScore;
		CopyMemory(DoubleScore.bBackCard,m_bBackCard,sizeof(m_bBackCard));

		//发送消息
		m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_DOUBLE_SCORE, &DoubleScore, sizeof( DoubleScore ) );

		//设置变量
		if (m_bLandScore==0) m_bLandScore=1;
		if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wFirstUser;

		return true;
	}

	//发送消息
	CMD_S_LandScore LandScore;
	ZeroMemory( &LandScore, sizeof( LandScore ) );

	LandScore.bLandUser=wChairID;
	LandScore.bLandScore=bLandScore;
	LandScore.wCurrentUser=m_wCurrentUser;
	LandScore.bCurrentScore=m_bLandScore;
	LandScore.bCallScorePhase=m_bCallScorePhase;
	LandScore.bPreCallScorePhase = cbPreCallScorePhase;
	CopyMemory(LandScore.bBrightCardUser, m_bUserBrightCard, sizeof( LandScore.bBrightCardUser ) );
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) 
		return false;

	//排序扑克
	m_GameLogic.SortCardList(bCardData,bCardCount,ST_ORDER);

	//类型判断
	BYTE bCardType=m_GameLogic.GetCardType(bCardData,bCardCount);
	if (bCardType==CT_ERROR) 
		return false;

	//更随出牌
	if (m_bTurnCardCount==0) m_bTurnCardCount=bCardCount;
	else if (m_GameLogic.CompareCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bCardCount)==false) 
		return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID])==false) 
		return false;
	m_bCardCount[wChairID]-=bCardCount;

	//出牌记录
	m_bTurnCardCount=bCardCount;
	m_bOutCardCount[wChairID]++;
	CopyMemory(m_bTurnCardData,bCardData,sizeof(BYTE)*bCardCount);

	//炸弹判断
	if ((bCardType==CT_BOMB_CARD)||(bCardType==CT_MISSILE_CARD)) m_wBombTime*=2;
	
	//切换用户
	m_wTurnWiner=wChairID;
	if (m_bCardCount[wChairID]!=0)
	{
		if (bCardType!=CT_MISSILE_CARD) m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	}
	else m_wCurrentUser=INVALID_CHAIR;

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.bCardCount=bCardCount;
	OutCard.wOutCardUser=wChairID;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));

	//发送数据
	WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//出牌最大
	if (bCardType==CT_MISSILE_CARD) m_bTurnCardCount=0;

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameEnd(wChairID,NULL,GER_NORMAL);

	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if ((wChairID!=m_wCurrentUser)||(m_bTurnCardCount==0)) return false;

	//设置变量
	m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	if (m_wCurrentUser==m_wTurnWiner) m_bTurnCardCount=0;

	//发送数据
	CMD_S_PassCard PassCard;
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bNewTurn=(m_bTurnCardCount==0)?TRUE:FALSE;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if ( ! bLookonUser )
	{
		//设置变量
		m_bBrightStart[wChairID] = false;
		m_bUserBrightCard[wChairID] = false;
		m_cbBrightCardTime=1;
		for ( WORD wBrightUser = 0; wBrightUser < GAME_PLAYER; ++wBrightUser )
			if ( m_bBrightStart[wBrightUser] ) m_cbBrightCardTime = BRIGHT_START_TIME;

		//发送消息
		OnBrightStart( wChairID, false );
	}

	return true;
}

//玩家托管
bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	m_bUserTrustee[ wChairID ] = bTrustee;

	//变量定义
	CMD_C_UserTrustee UserTrustee;
	ZeroMemory( &UserTrustee, sizeof( UserTrustee ) );
	UserTrustee.wUserChairID = wChairID;
	UserTrustee.bTrustee = bTrustee;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_C_TRUSTEE, &UserTrustee,sizeof(UserTrustee));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_C_TRUSTEE, &UserTrustee,sizeof(UserTrustee));

	return true;
}

//明牌开始
bool CTableFrameSink::OnBrightStart( WORD wChairID , bool bBright )
{
	//设置变量
	m_bBrightStart[wChairID] = bBright;
	m_bUserBrightCard[wChairID] = bBright;

	if ( bBright ) m_cbBrightCardTime=BRIGHT_START_TIME;

	//设置玩家
	if ( bBright && m_wCurrentUser == INVALID_CHAIR )
	{
		m_wCurrentUser = wChairID;
		m_wBankerUser = wChairID;
	}

	//设置消息
	CMD_S_BrightStart BrightStart;
	ZeroMemory( &BrightStart, sizeof( BrightStart ) );
	BrightStart.wBrightUser = wChairID;
	BrightStart.bBright = bBright;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BRIGHT_START,&BrightStart,sizeof(BrightStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BRIGHT_START,&BrightStart,sizeof(BrightStart));
	return true;
}

//玩家明牌
bool  CTableFrameSink::OnBrightCard( WORD wChairID, BYTE cbBrightCardTime)
{
	//发送消息
	if ( 0 < cbBrightCardTime && cbBrightCardTime != 255 ) 
	{
		CHAR szMsg[128] ;
		sprintf(szMsg, TEXT("[ %s ] 玩家选择了明牌，积分翻%d倍"), m_pITableFrame->GetServerUserItem( wChairID )->GetAccounts(), cbBrightCardTime==5?1:cbBrightCardTime );

		SendGameMessage( INVALID_CHAIR, szMsg );
	}

	//发牌明牌
	if ( m_bCallScorePhase != CSD_BRIGHTCARD )
	{
		ASSERT( cbBrightCardTime == BRIGHT_CARD_TIME );
		if ( cbBrightCardTime != BRIGHT_CARD_TIME ) return true;

		//倍数判断
		m_cbBrightCardTime = m_cbBrightCardTime > cbBrightCardTime ? m_cbBrightCardTime : cbBrightCardTime;

		//设置变量
		m_bUserBrightCard[wChairID] = cbBrightCardTime > 0 ? true:false;

		//设置消息
		CMD_S_BrightCard BrightCard;
		ZeroMemory( &BrightCard, sizeof( BrightCard ) );
		BrightCard.wBrightUser = wChairID;
		BrightCard.cbBrightTime = cbBrightCardTime;
		BrightCard.cbCurrenBrightTime = m_cbBrightCardTime;
		BrightCard.cbCallScorePhase = CSD_NORMAL;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BRIGHT_CARD,&BrightCard,sizeof(BrightCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BRIGHT_CARD,&BrightCard,sizeof(BrightCard));
	}
	else
	{
		//效验参数
		ASSERT((cbBrightCardTime==5)||(cbBrightCardTime==255));
		if ((cbBrightCardTime!=5)&&(cbBrightCardTime!=255)) return false;

		//明牌判断
		if ( m_bUserBrightCard[wChairID] ) return true;

		//增加次数
		m_cbBrightCardCount++;
		if ( m_cbBrightCardCount == GAME_PLAYER )
		{
			m_bCallScorePhase = CSD_GAMESTART;
		}
		else 
		{
			m_bCallScorePhase = CSD_BRIGHTCARD;
		}

		//设置变量
		if ( cbBrightCardTime == 5 )
		{
			m_bLandScore++;
			m_bUserBrightCard[wChairID] = true;
		}

		//设置消息
		CMD_S_BrightCard BrightCard;
		ZeroMemory( &BrightCard, sizeof( BrightCard ) );
		BrightCard.wBrightUser = wChairID;
		BrightCard.cbBrightTime = cbBrightCardTime;
		BrightCard.cbCurrenBrightTime = m_cbBrightCardTime;
		BrightCard.cbCallScorePhase = CSD_BRIGHTCARD;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BRIGHT_CARD,&BrightCard,sizeof(BrightCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BRIGHT_CARD,&BrightCard,sizeof(BrightCard));

		//结束判断
		if ( m_bCallScorePhase==CSD_GAMESTART )
		{
			//设置状态
			m_pITableFrame->SetGameStatus(GS_WK_DOUBLE_SCORE);

			//获取路径
			CHAR szDirectory[128];
			GetCurrentDirectory(sizeof( szDirectory ), szDirectory );

			//文件名称
			CHAR szProfileName[128];
			sprintf(szProfileName, TEXT("%s\\LandCrazy.ini"), szDirectory);

			//读取配置
			LONG lDoubleLimit = GetPrivateProfileInt(TEXT("DoubleScore"), TEXT("Score"), 10000, szProfileName );

			bool bAllowDouble = true;
			for ( WORD wUserID = 0; wUserID < GAME_PLAYER; ++wUserID )
			{
				//获取玩家
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem( wUserID );
				if ( pIServerUserItem == NULL ) continue;

				//条件判断
				LONG lUserScore = pIServerUserItem->GetUserScore()->lScore;
				if ( lUserScore < lDoubleLimit )
				{
					bAllowDouble = false;
					break;
				}
			}
			m_bAllowDoubleScore = bAllowDouble;

			//发送底牌
			m_bCardCount[m_wBankerUser]=20;
			CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
			m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);

			//设置消息
			CMD_S_DoubleScore DoubleScore;
			ZeroMemory( &DoubleScore, sizeof( DoubleScore ) );
			DoubleScore.bAllowDouble = bAllowDouble;
			DoubleScore.wLandUser = m_wBankerUser;
			DoubleScore.bCurrentScore = m_bLandScore;
			CopyMemory(DoubleScore.bBackCard,m_bBackCard,sizeof(m_bBackCard));

			//发送消息
			m_pITableFrame->SendTableData( INVALID_CHAIR, SUB_S_DOUBLE_SCORE, &DoubleScore, sizeof( DoubleScore ) );

			//设置变量
			if (m_bLandScore==0) m_bLandScore=1;
			if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wFirstUser;

			return true;
		}

	}

	return true;
}

//玩家加倍
bool CTableFrameSink::OnUserDoubleScore( WORD wChairID, bool bDoubleScore)
{
	//加倍判断
	if ( bDoubleScore ) m_bLandScore++;

	//发送消息
	if ( bDoubleScore ) 
	{
		CHAR szMsg[128] ;
		sprintf(szMsg, TEXT("[ %s ] 玩家选择了翻倍，积分翻1倍"), m_pITableFrame->GetServerUserItem( wChairID )->GetAccounts() );

		SendGameMessage( INVALID_CHAIR, szMsg );
	}

	//设置变量
	m_bDoubleUser[wChairID] = true;

	//构造消息
	CMD_S_UserDouble UserDouble;
	ZeroMemory( &UserDouble, sizeof( UserDouble ) );
	UserDouble.bCurrentScore = m_bLandScore;
	UserDouble.bDoubleScore = bDoubleScore;
	UserDouble.wDoubleUser = wChairID;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_DOUBLE,&UserDouble,sizeof(UserDouble));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_DOUBLE,&UserDouble,sizeof(UserDouble));


	//统计次数
	m_cbDoubleCount++;

	//结束判断
	if ( m_cbDoubleCount == GAME_PLAYER )
	{
		//设置状态
		m_pITableFrame->SetGameStatus(GS_WK_PLAYING);	

		//出牌信息
		m_bTurnCardCount=0;
		m_wTurnWiner=m_wBankerUser;
		m_wCurrentUser=m_wBankerUser;

		//发送消息
		CMD_S_GameStart GameStart;
		GameStart.wLandUser=m_wBankerUser;
		GameStart.bLandScore=m_bLandScore;
		GameStart.bBrightCard=m_bBrightCard;
		GameStart.wCurrentUser=m_wCurrentUser;
		CopyMemory(GameStart.bBackCardData, m_bBackCard, 3);
		CopyMemory(GameStart.bUserBrightCard, m_bUserBrightCard, sizeof(GameStart.bUserBrightCard));
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	return true;
}

//发送消息
void CTableFrameSink::SendGameMessage( WORD wChairID, CHAR szMsg[] )
{
	if ( wChairID == INVALID_CHAIR )
	{
		//游戏玩家
		for ( WORD wUserIdx = 0; wUserIdx < GAME_PLAYER; ++wUserIdx )
		{
			IServerUserItem *pUserItem = m_pITableFrame->GetServerUserItem( wUserIdx );
			m_pITableFrame->SendGameMessage( pUserItem, szMsg, SMT_INFO );
		}

		//旁观玩家
		WORD wLookonIdx = 0;
		IServerUserItem *pLookonUserItem = m_pITableFrame->EnumLookonUserItem(wLookonIdx);
		while( pLookonUserItem != NULL )
		{
			wLookonIdx++;
			m_pITableFrame->SendGameMessage( pLookonUserItem, szMsg, SMT_INFO );
			pLookonUserItem = m_pITableFrame->EnumLookonUserItem(wLookonIdx);
		}
		
	}
	else
	{
		IServerUserItem *pUserItem = m_pITableFrame->GetServerUserItem( wChairID );
		m_pITableFrame->SendGameMessage( pUserItem, szMsg, SMT_INFO );
	}
}
//////////////////////////////////////////////////////////////////////////
