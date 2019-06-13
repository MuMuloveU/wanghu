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
	m_wFirstUser=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));

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
//用户坐下
bool __cdecl CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false) m_HistoryScore.OnEventUserEnter(wChairID);

	return true;
}

//用户起立
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false) m_HistoryScore.OnEventUserLeave(wChairID);

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
	m_wCurrentUser = rand() % GAME_PLAYER;
	m_wFirstUser=m_wCurrentUser;

	//发送扑克
	CMD_S_SendAllCard SendAllCard ;
	SendAllCard.wCurrentUser = m_wCurrentUser ;

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
			ZeroMemory( &GameEnd, sizeof( GameEnd ) );

			//剩余扑克
			BYTE bCardPos = 0;
			for ( WORD i = 0; i < m_wPlayerCount; i++ )
			{
				GameEnd.bCardCount[ i ] = m_bCardCount[ i ];
				CopyMemory( &GameEnd.bCardData[ bCardPos ], m_bHandCardData[ i ], m_bCardCount[ i ] * sizeof( BYTE ) );
				bCardPos += m_bCardCount[ i ];
			}

			//变量定义
			LONG lCellScore = m_pGameServiceOption->lCellScore;
			bool bLandWin = ( m_bCardCount[ m_wBankerUser ] == 0 ) ? true : false;

			//春天判断
			if ( wChairID == m_wBankerUser )
			{
				WORD wUser1 = ( m_wBankerUser + 1 ) % GAME_PLAYER;
				WORD wUser2 = ( m_wBankerUser + 2 ) % GAME_PLAYER;
				if ( ( m_bOutCardCount[ wUser1 ] == 0 ) && ( m_bOutCardCount[ wUser2 ] == 0 ) ) m_wBombTime *= 2;
			}
			else
			{
				if ( m_bOutCardCount[ m_wBankerUser ] == 1 ) m_wBombTime *= 2;
			}

			//炸弹限制
			m_wBombTime = __min( m_wBombTime, 16 );
			
			//游戏积分
			LONG lScore=0;
			LONG lRevenue=0;
			enScoreKind ScoreKind;

			//统计积分
			for ( WORD i = 0; i < m_wPlayerCount; i++ )
			{

				lScore=0;
				lRevenue=0;

				//统计积分
				if ( i == m_wBankerUser )
				{
					lScore = m_wBombTime * m_bLandScore * lCellScore * ( ( bLandWin == true ) ? 2 : -2 );
					GameEnd.lGameScore[ i ] = m_wBombTime * m_bLandScore * lCellScore * ( ( bLandWin == true ) ? 2 : -2 );
				}
				else 
				{
					lScore = m_wBombTime * m_bLandScore * lCellScore * ( ( bLandWin == true ) ? -1 : 1 );
					GameEnd.lGameScore[ i ] = m_wBombTime * m_bLandScore * lCellScore * ( ( bLandWin == true ) ? -1 : 1 );
				}

				//胜利类型
				ScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;

				//计算税收
				if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
				{
					if (GameEnd.lGameScore[i]>=100L)
					{
						//计算税收
						GameEnd.lGameTax+= (GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/100L);
						lRevenue = (GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/100L);

						//积分调整
						lScore=lScore-lRevenue;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue;
					}
				}
			
				//修改分数
				m_pITableFrame->WriteUserScore(i,lScore,lRevenue,ScoreKind);

				//历史积分
				m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);
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

			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//炸弹限制
			m_wBombTime=__min(m_wBombTime,16);
			m_bLandScore=__max(m_bLandScore,1);

			//修改积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//游戏积分
				LONG lScore=0;
				LONG lRevenue=0;
				enScoreKind ScoreKind=enScoreKind_Draw;


				//构造变量
				if (i==wChairID)
				{
					ScoreKind=enScoreKind_Flee;
					lScore	 =-m_wBombTime*m_bLandScore*m_pGameServiceOption->lCellScore*3;
					GameEnd.lGameScore[i]=-m_wBombTime*m_bLandScore*m_pGameServiceOption->lCellScore*3;
				}
				else if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
				{
					//统计积分
					//ScoreKind=enScoreKind_Win;
					//lScore=(m_wBombTime*m_bLandScore*m_pGameServiceOption->lCellScore*4)/2;
					//GameEnd.lGameScore[i]=(m_wBombTime*m_bLandScore*m_pGameServiceOption->lCellScore*4)/2;

					////计算税收
					//if (lScore>=100L)
					//{
					//	//计算税收
					//	GameEnd.lGameTax+=GameEnd.lGameScore[i]*m_pGameServiceOption->cbRevenue/100L;
					//	lRevenue = GameEnd.lGameScore[i]*m_pGameServiceOption->cbRevenue/100L;

					//	//积分调整
					//	lScore=lScore-lRevenue ;
					//	GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue ;
					//}
				}

				//写入积分
				m_pITableFrame->WriteUserScore(i,lScore,lRevenue,ScoreKind);
				//历史积分
				m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

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
			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusFree.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

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
			CopyMemory(StatusScore.bScoreInfo,m_bScoreInfo,sizeof(m_bScoreInfo));
	//		CopyMemory(StatusScore.bCardData,m_bHandCardData[wChiarID],m_bCardCount[wChiarID]);
			CopyMemory(StatusScore.bCardData,m_bHandCardData, sizeof(m_bHandCardData)*sizeof(BYTE));
			CopyMemory( StatusScore.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusScore.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusScore.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
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
			
			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusPlay.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

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

			CMD_C_UserTrustee *pUserTrustee = ( CMD_C_UserTrustee * )pDataBuffer;

			m_bUserTrustee[ pUserTrustee->wUserChairID ] = pUserTrustee->bTrustee;

			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_C_TRUSTEE, (void*)pDataBuffer,wDataSize);
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_C_TRUSTEE, (void*)pDataBuffer,wDataSize);

			return true;
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
	if (wChairID!=m_wCurrentUser) return false;

	//效验参数
	if (((bLandScore>3)&&(bLandScore!=255))||(bLandScore<=m_bLandScore)) return false;

	//设置变量
	if (bLandScore!=255)
	{
		m_bLandScore=bLandScore;
		m_wBankerUser=m_wCurrentUser;
	}
	m_bScoreInfo[wChairID]=bLandScore;

	//开始判断
	if ((m_bLandScore==3)||(m_wFirstUser==(wChairID+1)%m_wPlayerCount))
	{
		//重新发牌
		if (m_bLandScore==0) 
		{
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
			m_wCurrentUser = rand() % GAME_PLAYER;
			m_wFirstUser=m_wCurrentUser;

			//发送扑克
			CMD_S_SendAllCard SendAllCard ;
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

		//设置变量
		if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wFirstUser;

		//设置状态
		m_pITableFrame->SetGameStatus(GS_WK_PLAYING);

		//发送底牌
		m_bCardCount[m_wBankerUser]=20;
		CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
		m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);

		//出牌信息
		m_bTurnCardCount=0;
		m_wTurnWiner=m_wBankerUser;
		m_wCurrentUser=m_wBankerUser;

		//发送消息
		CMD_S_GameStart GameStart;
		GameStart.wLandUser=m_wBankerUser;
		GameStart.bLandScore=m_bLandScore;
		GameStart.wCurrentUser=m_wCurrentUser;
		CopyMemory(GameStart.bBackCard,m_bBackCard,sizeof(m_bBackCard));
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

		return true;
	}

	//设置用户
	m_wCurrentUser=(wChairID+1)%m_wPlayerCount;

	//发送消息
	CMD_S_LandScore LandScore;
	LandScore.bLandUser=wChairID;
	LandScore.bLandScore=bLandScore;
	LandScore.wCurrentUser=m_wCurrentUser;
	LandScore.bCurrentScore=m_bLandScore;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) return false;

	//排序扑克
	m_GameLogic.SortCardList(bCardData, bCardCount, ST_ORDER);

	//类型判断
	BYTE bCardType=m_GameLogic.GetCardType(bCardData,bCardCount);
	if (bCardType==CT_ERROR) return false;

	//更随出牌
	if (m_bTurnCardCount==0) m_bTurnCardCount=bCardCount;
	else if (m_GameLogic.CompareCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bCardCount)==false) return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID])==false) return false;
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

//////////////////////////////////////////////////////////////////////////
