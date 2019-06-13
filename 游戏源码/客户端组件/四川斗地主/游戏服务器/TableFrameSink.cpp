#include "StdAfx.h"
#include "TableFrameSink.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//宏定义
#define	GOOD_CARD_PERCENT				4									//好牌概率

#define	MAX_CALL_TIMES					2									//翻倍叫数

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wBombTime=1;
	m_bMingCard=0;
	m_bBackCatchTag = 0;
	m_bHaveSendCard = 0;
	m_bLookCardTag = 0;
	m_bCurrentState = 0;
	m_wOutBombCount = 0;
	m_wFirstUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_bScoreTimes,0,sizeof(m_bScoreTimes));
	memset(m_bCallScoreTimes,0,sizeof(m_bCallScoreTimes));
	memset(m_bCallScoreTag,0,sizeof(m_bCallScoreTag));
	memset(m_bCallBanker,0,sizeof(m_bCallBanker));

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
	m_bMingCard=0;
	m_bBackCatchTag = 0;
	m_bHaveSendCard = 0;
	m_bLookCardTag = 0;
	m_bCurrentState = 0;
	m_wOutBombCount = 0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_bScoreTimes,0,sizeof(m_bScoreTimes));
	memset(m_bCallScoreTimes,0,sizeof(m_bCallScoreTimes));
	memset(m_bCallScoreTag,0,sizeof(m_bCallScoreTag));
	memset(m_bCallBanker,0,sizeof(m_bCallBanker));

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

	//扑克变量
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

	//随机叫庄
	/*if(m_wFirstUser==INVALID_CHAIR)*/m_wFirstUser = rand()%m_wPlayerCount;
	m_wCurrentUser = m_wFirstUser;
	m_bMingCard = m_bHandCardData[m_wCurrentUser][(rand()%17)];

	//构造数据
	CMD_S_CallBanker CallBanker;
	ZeroMemory(&CallBanker,sizeof(CallBanker));
	CallBanker.wCurrentUser = m_wCurrentUser;
	CallBanker.bMingCard = m_bMingCard;

	//发送数据
	for ( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		m_pITableFrame->SendTableData( i, SUB_S_CHOICE_LOOK, &CallBanker, sizeof( CallBanker ) );
		m_pITableFrame->SendLookonData( i, SUB_S_CHOICE_LOOK, &CallBanker, sizeof( CallBanker ) );
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
			//ASSERT( pIServerUserItem != NULL );
			//ASSERT( wChairID < m_wPlayerCount );

			//构造数据
			CMD_S_GameEnd GameEnd;
			memset( &GameEnd, 0, sizeof( GameEnd ) );
			CopyMemory(GameEnd.bBackCard,m_bBackCard,sizeof(m_bBackCard));

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
			CopyMemory(GameEnd.bBackCard,m_bBackCard,sizeof(m_bBackCard));

			//剩余扑克
			BYTE bCardPos = 0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//变量定义
			LONG lCellScore=m_pGameServiceOption->lCellScore;
			bool bLandWin =(m_bCardCount[m_wBankerUser]==0)?true:false;

			//春天判断
			WORD wBombTime=1;
			if(wChairID==m_wBankerUser)
			{
				WORD wUser1=(m_wBankerUser+1)%GAME_PLAYER;
				WORD wUser2=(m_wBankerUser+2)%GAME_PLAYER;
				if((m_bOutCardCount[wUser1]==0)&&(m_bOutCardCount[wUser2]==0))wBombTime*=2;
			}
			else
			{
				if(m_bOutCardCount[m_wBankerUser]==1)wBombTime*=2;
			}

			//炸弹限制
			//wBombTime= __min( wBombTime, 16 );

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
					WORD wUser1=(m_wBankerUser+1)%m_wPlayerCount,wUser2=(m_wBankerUser+2)%m_wPlayerCount;
					lScore=(m_bScoreTimes[wUser1]+m_bScoreTimes[wUser2])*wBombTime*lCellScore*((bLandWin==true)?1:-1);
					GameEnd.lGameScore[i]=lScore;
				}
				else 
				{
					lScore = m_bScoreTimes[i] * wBombTime * lCellScore * ( ( bLandWin == true ) ? -1 : 1 );
					GameEnd.lGameScore[i] = lScore;
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
				m_pITableFrame->WriteUserScore(i,GameEnd.lGameScore[i],lRevenue,ScoreKind);
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
			CopyMemory(GameEnd.bBackCard,m_bBackCard,sizeof(m_bBackCard));

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//炸弹限制
			//WORD wBombTime = 1;
			//wBombTime=__min(wBombTime,16);

			//最少赔率
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				m_bScoreTimes[i] = __max(m_bScoreTimes[i],1);
			}

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
					WORD wUser1=(i+1)%m_wPlayerCount;
					WORD wUser2=(i+2)%m_wPlayerCount;
					lScore=(m_bScoreTimes[wUser1]+m_bScoreTimes[wUser2])*m_pGameServiceOption->lCellScore*(-2);
					GameEnd.lGameScore[i]=lScore;
				}
				else if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
				{
					//统计积分
					ScoreKind=enScoreKind_Win;
					lScore=m_bScoreTimes[i]*m_pGameServiceOption->lCellScore*2;
					GameEnd.lGameScore[i]=lScore;

					//计算税收
					if (lScore>=100L)
					{
						//计算税收
						GameEnd.lGameTax+=GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/100L;
						lRevenue = GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/100L;

						//积分调整
						lScore=lScore-lRevenue ;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue ;
					}
				}

				//写入积分
				m_pITableFrame->WriteUserScore(i,lScore,lRevenue,ScoreKind);
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

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_WK_SCORE:		//叫分状态
		{
			//构造数据
			CMD_S_StatusScore StatusScore;
			memset(&StatusScore,0,sizeof(StatusScore));

			//设置变量
			StatusScore.bBackCatchTag = m_bBackCatchTag;
			StatusScore.wBankerUser = m_wBankerUser;
			StatusScore.wFirstUser = m_wFirstUser;
			StatusScore.bMingCard = m_bMingCard;
			StatusScore.wCurrentUser=m_wCurrentUser;
			StatusScore.bCurrentState = m_bCurrentState;
			StatusScore.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusScore.bCallScoreTimes,m_bCallScoreTimes,sizeof(m_bCallScoreTimes));
			CopyMemory( StatusScore.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			if(m_bBackCatchTag!=FALSE || m_bLookCardTag!=FALSE)
			{
				CopyMemory(StatusScore.bCardData,m_bHandCardData[wChiarID], MAX_COUNT*sizeof(BYTE));
				if(m_bLookCardTag==TRUE)CopyMemory(StatusScore.bBackCard,m_bBackCard, sizeof(m_bBackCard)*sizeof(BYTE));
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
			StatusPlay.lBaseScore=m_pGameServiceOption->lCellScore;
			StatusPlay.wFirstUser=m_wFirstUser;
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.wLastOutUser=m_wTurnWiner;
			StatusPlay.wOutBombCount=m_wOutBombCount;
			StatusPlay.bMingCard=m_bMingCard;
			StatusPlay.bTurnCardCount=m_bTurnCardCount;
			CopyMemory(StatusPlay.bCallScoreTimes,m_bCallScoreTimes,sizeof(m_bCallScoreTimes));
			CopyMemory(StatusPlay.bScoreTimes,m_bScoreTimes,sizeof(m_bScoreTimes));
			CopyMemory(StatusPlay.bCardCount,m_bCardCount,sizeof(m_bCardCount));
			CopyMemory(StatusPlay.bTurnCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
			CopyMemory(StatusPlay.bCardData,m_bHandCardData[wChiarID],sizeof(BYTE)*m_bCardCount[wChiarID]);
			CopyMemory( StatusPlay.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			if(wChiarID==m_wBankerUser || m_bLookCardTag==TRUE)
				CopyMemory(StatusPlay.bBackCard,m_bBackCard,sizeof(m_bBackCard));

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

			//用户效验
			ASSERT(m_wCurrentUser==pUserData->wChairID);
			if(m_wCurrentUser!=pUserData->wChairID)return false;

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

//发送信息
bool CTableFrameSink::SendUserInfoData(WORD wLandUser,BYTE bLandScore,WORD wCurrentUser,BYTE bCurrentState,WORD wBankerUser)
{
	//当前状态
	m_bCurrentState = bCurrentState;

	//构造数据
	CMD_S_LandScore LandScore;
	ZeroMemory(&LandScore,sizeof(LandScore));

	//转换类型
	BYTE bTemp = bLandScore;
	if(bTemp==DOUBLE_SCORE)
	{
		if(m_bCallScoreTimes[wLandUser]==1)bTemp=CALL_SCORE;
		else bTemp=CALL_TWO_SCORE;
	}
	else if(bTemp==0)bTemp=PASS_ACTION;

	//设置参数
	LandScore.bLandUser=wLandUser;
	LandScore.bLandScore=bTemp;
	LandScore.wCurrentUser=wCurrentUser;
	LandScore.bCurrentState=bCurrentState;
	LandScore.wBankerUser=wBankerUser;
	LandScore.bUserCallTimes=m_bCallScoreTimes[wLandUser];

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LAND_SCORE,&LandScore,sizeof(LandScore));

	return true;
}

//发送扑克
bool CTableFrameSink::SendUserCard()
{
	//从未发送
	if(m_bHaveSendCard==FALSE)
	{
		//设置变量
		m_bHaveSendCard = TRUE;

		//发送扑克
		CMD_S_SendAllCard SendAllCard ;
		ZeroMemory(&SendAllCard,sizeof(SendAllCard));

		//手上扑克
		CopyMemory( SendAllCard.bCardData, m_bHandCardData, sizeof( m_bHandCardData ) ) ;

		//底牌扑克
		ASSERT(m_bLookCardTag!=m_bBackCatchTag);
		if(m_bLookCardTag==TRUE)CopyMemory(SendAllCard.bBackCardData,m_bBackCard,3);

		for ( WORD i = 0; i < m_wPlayerCount; i++ )
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(i);
			if ( pServerUserItem == NULL ) continue;
			if ( pServerUserItem->IsAndroidUser() ) CopyMemory(SendAllCard.bBackCardData,m_bBackCard,3);
			else if ( m_bLookCardTag!=TRUE ) ZeroMemory(SendAllCard.bBackCardData, sizeof(SendAllCard.bBackCardData));

			m_pITableFrame->SendTableData( i, SUB_S_SEND_CARD, &SendAllCard, sizeof( SendAllCard ) );
			m_pITableFrame->SendLookonData( i, SUB_S_SEND_CARD, &SendAllCard, sizeof( SendAllCard ) );
		}

		//排列扑克
		for ( WORD i = 0; i < m_wPlayerCount; i++ )
		{
			m_GameLogic.SortCardList( m_bHandCardData[ i ], m_bCardCount[ i ], ST_ORDER );
		}
	}

	return true;
}

//开始出牌
bool CTableFrameSink::UserStartOutCard()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_WK_PLAYING);

	//设置扑克
	if(m_bCardCount[m_wBankerUser]<20)
	{
		//发送底牌
		//ASSERT(m_bLookCardTag==FALSE);
		m_bCardCount[m_wBankerUser]=20;
		CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
		m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);
	}

	//出牌信息
	m_bTurnCardCount=0;
	m_wTurnWiner=m_wBankerUser;
	m_wCurrentUser=m_wBankerUser;

	//构造消息
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	GameStart.bLandScore=0;
	GameStart.wCurrentUser=m_wCurrentUser;

	//用户倍数
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_wBankerUser==i)continue;
		WORD wScoreTimes = 0;
		wScoreTimes+=__min(m_bCallScoreTimes[m_wBankerUser],m_bCallScoreTimes[i]);
		wScoreTimes+=m_bCallScoreTimes[i];

		//闲家倍数
		GameStart.wUserMultiple[i]=m_wBombTime*((WORD)pow(2,(INT)wScoreTimes));
		m_bScoreTimes[i]=(BYTE)GameStart.wUserMultiple[i];
	}

	//庄家倍数
	WORD wUser1 = (m_wBankerUser+1)%GAME_PLAYER;
	WORD wUser2 = (m_wBankerUser+2)%GAME_PLAYER;
	m_bScoreTimes[m_wBankerUser] = __max(m_bScoreTimes[wUser1],m_bScoreTimes[wUser2]);
	GameStart.wUserMultiple[m_wBankerUser] = m_bScoreTimes[m_wBankerUser];

	//发送信息
	for (WORD i=0; i<m_wPlayerCount;i++ )
	{
		if(i==m_wBankerUser && m_bBackCatchTag==TRUE)CopyMemory(GameStart.bBackCard,m_bBackCard,sizeof(m_bBackCard));
		else ZeroMemory(GameStart.bBackCard,sizeof(GameStart.bBackCard));
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	return true;
}

//叫分事件
bool CTableFrameSink::OnUserLandScore(WORD wChairID, BYTE bLandScore)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_SCORE) return true;
	if (wChairID!=m_wCurrentUser) return false;

	//首叫状态
	if(m_bBackCatchTag==FALSE && m_bLookCardTag==FALSE)
	{
		//效验参数
		ASSERT(bLandScore==BACK_CATCH || bLandScore==LOOK_CARD);
		if(bLandScore!=BACK_CATCH && bLandScore!=LOOK_CARD)return false;

		//选择看牌
		if(bLandScore==LOOK_CARD)
		{
			//设置变量
			m_bLookCardTag=TRUE;

			//发送扑克
			SendUserCard();
		}
		//选择闷抓
		else
		{
			//设置变量
			m_bBackCatchTag=TRUE;

			//确定庄家
			m_wBankerUser = wChairID;

			//积分翻倍
			m_wBombTime*=2;

			//下位叫分
			m_wCurrentUser=(wChairID+1)%m_wPlayerCount;
		}

		//继续询问
		BYTE bCurrentState=((bLandScore==LOOK_CARD)?CALL_BANKER:CALL_SCORE);
		WORD wBankerUser=((bLandScore==LOOK_CARD)?INVALID_CHAIR:m_wBankerUser);
		SendUserInfoData(wChairID,bLandScore,m_wCurrentUser,bCurrentState,wBankerUser);
	}
	//叫庄状态
	else if(m_wBankerUser==INVALID_CHAIR)
	{
		//效验参数
		ASSERT(bLandScore==MING_CATCH || bLandScore==NULL);
		if(bLandScore!=MING_CATCH && bLandScore!=NULL)return false;

		//用户状态
		ASSERT(m_bCallBanker[wChairID] == FALSE);
		if(m_bCallBanker[wChairID] != FALSE)return false;

		//设置变量
		if(bLandScore==MING_CATCH)m_bCallBanker[wChairID] = 2*TRUE;
		else m_bCallBanker[wChairID] = TRUE;

		//判断次数
		bool bFullTimes = false;
		for(WORD i=0;i<m_wPlayerCount;i++)if(m_bCallBanker[i] == FALSE)break;
		if(i==m_wPlayerCount)bFullTimes=true;

		//判断开始
		if(bFullTimes || bLandScore==MING_CATCH)
		{
			//确定庄家
			if(bLandScore==MING_CATCH)
			{
				m_wBankerUser = wChairID;

				//开始出牌
				if(bFullTimes)
				{
					SendUserInfoData(wChairID,bLandScore,INVALID_CHAIR,0,m_wBankerUser);
					UserStartOutCard();
				}
				//询问叫分
				else 
				{
					//插入底牌
					m_bCardCount[m_wBankerUser]=20;
					CopyMemory(&m_bHandCardData[m_wBankerUser][17],m_bBackCard,sizeof(m_bBackCard));
					m_GameLogic.SortCardList(m_bHandCardData[m_wBankerUser],m_bCardCount[m_wBankerUser],ST_ORDER);

					//开始叫分+客户端插入底牌
					m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
					BYTE bCurrentState = 0x10+CALL_SCORE;
					SendUserInfoData(wChairID,bLandScore,m_wCurrentUser,bCurrentState,m_wBankerUser);
				}
			}
			//游戏结束
			else OnEventGameEnd(INVALID_CHAIR,NULL,GER_DISMISS);

			return true;
		}

		//询问下位
		m_wCurrentUser = (m_wCurrentUser+1)%m_wPlayerCount;
		ASSERT(m_bCallBanker[m_wCurrentUser] != TRUE);

		//继续询问
		SendUserInfoData(wChairID,bLandScore,m_wCurrentUser,CALL_BANKER,INVALID_CHAIR);
	}
	//叫分状态
	else 
	{
		//效验参数
		ASSERT(m_bCallScoreTimes[wChairID]<MAX_CALL_TIMES);
		ASSERT(bLandScore==DOUBLE_SCORE || bLandScore==0);
		if(m_bCallScoreTimes[wChairID]>=MAX_CALL_TIMES)return false;
		if(bLandScore!=DOUBLE_SCORE && bLandScore!=0)return false;

		//设置变量
		if(bLandScore==0)m_bCallScoreTag[wChairID] = TRUE;
		else m_bCallScoreTimes[wChairID]++;
		if(m_bCallScoreTimes[wChairID]==MAX_CALL_TIMES)m_bCallScoreTag[wChairID] = TRUE;

		//判断开始
		bool bStartOut=(m_bCallScoreTag[m_wBankerUser]==TRUE)?true:false;
		if(!bStartOut && bLandScore==0 && m_wBankerUser!=wChairID)
		{
			WORD wUser1 = m_wPlayerCount-m_wBankerUser-wChairID;
			if(m_bCallScoreTag[wUser1]==TRUE && m_bCallScoreTimes[wUser1]<MAX_CALL_TIMES)
				bStartOut=true;
			WORD wUser2 = (m_wBankerUser+1)%m_wPlayerCount;
			if(!bStartOut && m_bLookCardTag==TRUE && wUser2==wChairID && m_bCallScoreTimes[wUser2]==0 && m_bCallBanker[wUser1]==TRUE)
				bStartOut=true;
		}

		//开始出牌
		if(bStartOut)
		{
			if(bLandScore==DOUBLE_SCORE)
			{
				SendUserInfoData(wChairID,bLandScore,INVALID_CHAIR,0,INVALID_CHAIR);
			}

			//发送扑克
			SendUserCard();

			//开始出牌
			UserStartOutCard();

			return true;
		}

		//下位玩家
		WORD wNextUser = INVALID_CHAIR;
		for(WORD i=1;i<m_wPlayerCount;i++)
		{
			wNextUser = (wChairID+i)%m_wPlayerCount;
			if(m_bCallScoreTag[wNextUser]!=TRUE)break;
		}

		//当前用户
		ASSERT(wNextUser<m_wPlayerCount && m_bCallScoreTimes[wNextUser]<MAX_CALL_TIMES);
		m_wCurrentUser = wNextUser;

		//继续询问
		BYTE bCallScoreState=((m_bCallScoreTimes[m_wCurrentUser]==0)?CALL_SCORE:CALL_TWO_SCORE);
		SendUserInfoData(wChairID,bLandScore,m_wCurrentUser,bCallScoreState,INVALID_CHAIR);
	}

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) return false;

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
	if ((bCardType==CT_BOMB_CARD)||(bCardType==CT_MISSILE_CARD)) 
	{
		for(WORD i=0;i<m_wPlayerCount;i++)m_bScoreTimes[i]*=2;
		m_wOutBombCount++;
	}
	
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
