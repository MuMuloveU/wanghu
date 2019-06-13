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
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wChaUser=INVALID_CHAIR;
	m_bgou = false;
	m_bdian = false;
	//运行信息
	m_bTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

	//扑克信息
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bOutCardCount,0,sizeof(m_bOutCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	////////////////////////////////////////初始化添加的变量////////////////////////////////////////
	memset(m_bLiOrJue, 0, sizeof(m_bLiOrJue));
	m_cbLiSuperCount = 0;
	memset(m_bNotLiOrJue, 0, sizeof(m_bNotLiOrJue));
	m_cbNotJue = 0;
	m_cbMingCi = 0;
	memset(m_cbMiCi, 0, sizeof(m_cbMiCi));
	m_cbEndType = 0;
	memset(m_cbCiXu, -1, sizeof(m_cbCiXu));
	m_wLiGun = 256;
	m_wJueGun = 256;

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
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wChaUser=INVALID_CHAIR;
	m_bgou = false;
	m_bdian = false;
	//运行信息
	m_bTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bUserTrustee, 0, sizeof( m_bUserTrustee ) );

	//扑克信息
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bOutCardCount,0,sizeof(m_bOutCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

	//////////////////////////////添加变量的重置/////////////////////////////
	memset(m_bLiOrJue, 0, sizeof(m_bLiOrJue));
	m_cbLiSuperCount = 0;
	memset(m_bNotLiOrJue, 0, sizeof(m_bNotLiOrJue));
	m_cbNotJue = 0;
	m_cbMingCi = 0;
	memset(m_cbMiCi, 0, sizeof(m_cbMiCi));
	m_cbEndType = 0;
	memset(m_cbCiXu, -1, sizeof(m_cbCiXu));
	m_wLiGun = 256;
	m_wJueGun = 256;

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
	m_pITableFrame->SetGameStatus(GS_WK_PLAYING);

	BYTE cbSendGoodCard = rand () % GOOD_CARD_PERCENT ;
	BYTE cbAndroidUserCount = 0 ;
	WORD wAndroidUser[ GAME_PLAYER ] = {0};

	WORD wHaveGoodCardAndroidUser = INVALID_CHAIR ;
	{
		//混乱扑克
		BYTE bRandCard[ 54 ];
		m_GameLogic.RandCardList( bRandCard,sizeof( BYTE ) * 54 );

		int num = 0;
		//分发扑克
		for ( WORD i = 0; i < GAME_PLAYER; i++ )
		{	
			m_bCardCount[ i ] = 13;		
			CopyMemory( &m_bHandCardData[ i ], &bRandCard[ i * m_bCardCount[ i ] ], sizeof( BYTE ) * m_bCardCount[ i ] );
		}

		CopyMemory( &m_bHandCardData[0][13], &bRandCard[52], sizeof( BYTE ) );m_bCardCount[ 0 ]++;
		CopyMemory( &m_bHandCardData[1][13], &bRandCard[53], sizeof( BYTE ) );m_bCardCount[ 1 ]++;

	}

	///////////////////////////////////////////////////////////
	int index = -1;
	for (int i=0; i<GAME_PLAYER; ++i)
	{
		index = m_GameLogic.FindCard(m_bHandCardData[i], m_bCardCount[i], 0x23);
		if (-1 != index)
		{
			index = i;
			break;
		}
	}
	///////////////////////////////////////////////////////////

	//设置用户
	m_wCurrentUser = index;//当前玩家=//首叫用户

	//发送扑克
	CMD_S_SendAllCard SendAllCard ;
	SendAllCard.wCurrentUser = m_wCurrentUser ;

	//手上扑克
	CopyMemory( SendAllCard.bCardData, m_bHandCardData, sizeof( m_bHandCardData ) ) ;
	//扑克的数目
	CopyMemory(SendAllCard.cbCardNum,m_bCardCount, sizeof( m_bCardCount ));

	for ( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		m_pITableFrame->SendTableData( i, SUB_S_SEND_CARD, &SendAllCard, sizeof( SendAllCard ) );
		m_pITableFrame->SendLookonData( i, SUB_S_SEND_CARD, &SendAllCard, sizeof( SendAllCard ) );
	}

	//排列扑克
	for ( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		m_GameLogic.SortCardList( m_bHandCardData[ i ], m_bCardCount[ i ], ST_ORDER );//排列扑克
	}

	////发送消息
	//CMD_S_GameStart GameStart;
	//GameStart.wCurrentUser=index;

	//m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

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
				GameEnd.bCardCount[ i ] = m_bCardCount[ i ];//扑克数目
				CopyMemory( &GameEnd.bCardData[ bCardPos ], m_bHandCardData[ i ], m_bCardCount[ i ] * sizeof( BYTE ) );
				bCardPos += m_bCardCount[ i ];
			}

			//变量定义
			LONG lCellScore = m_pGameServiceOption->lCellScore;

			WORD Winer = 0;
			for(WORD i = 0; i < m_wPlayerCount; i++ )
			{
				bool a44  = false;
				bool king = false;

				a44  = m_GameLogic.A44(m_bHandCardData[i],m_bCardCount[i]);
				king = m_GameLogic.DoubleKong(m_bHandCardData[i],m_bCardCount[i]);

				if(i == wChairID) continue;

				if((m_bOutCardCount[i] == 0) || a44 || king)
					m_bCardCount[i]*=2;

				//GameEnd.lGameScore[ i ]=-m_bCardCount[i];
				Winer+=m_bCardCount[i];
			}
				
			//GameEnd.lGameScore[wChairID] = Winer;
			CountScore(GameEnd.lGameScore);

			//统计积分
			for ( WORD i = 0; i < m_wPlayerCount; i++ )
			{
				LONG lScore=0;
				LONG lRevenue=0;
				enScoreKind ScoreKind;


				//胜利类型
				ScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
				//计算税收
				if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
				{
					if (GameEnd.lGameScore[i]>=100L)
					{
						//计算税收
						GameEnd.lGameTax+=(GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/100L);
						lRevenue = (GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/100L);

						//积分调整
						lScore=lScore-lRevenue ;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue ;
					}
				}
				//修改分数
				m_pITableFrame->WriteUserScore(i,lScore,lRevenue,ScoreKind);

			}



			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

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

			//修改积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//变量定义
				LONG lScore=0;
				LONG lRevenue=0;
				enScoreKind ScoreKind=enScoreKind_Draw;

				//构造变量
				if (i==wChairID)
				{
					ScoreKind=enScoreKind_Flee;
					lScore	 =-m_pGameServiceOption->lCellScore*54;
					GameEnd.lGameScore[i]=-/*m_pGameServiceOption->lCellScore*54*/80;
				}
				else if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
				{
					//统计积分
					ScoreKind=enScoreKind_Win;
					lScore=(m_pGameServiceOption->lCellScore*54)/3;
					GameEnd.lGameScore[i]=(m_pGameServiceOption->lCellScore*54)/3;

					//计算税收
					if (lScore>=100L)
					{
						//计算税收
						GameEnd.lGameTax+=(GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/100L);
						lRevenue = (GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/100L);

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
	case GS_WK_PLAYING:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//设置变量
//			StatusPlay.wLandUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.wLastOutUser=m_wTurnWiner;
			StatusPlay.bTurnCardCount=m_bTurnCardCount;
//			StatusPlay.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.bCardCount,m_bCardCount,sizeof(m_bCardCount));
			CopyMemory(StatusPlay.bTurnCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
			CopyMemory(StatusPlay.bCardData,m_bHandCardData,sizeof(BYTE)*sizeof(m_bHandCardData));
			CopyMemory( StatusPlay.bUserTrustee, m_bUserTrustee, sizeof( m_bUserTrustee ) );
			StatusPlay.gou = m_bgou;
			if(m_bgou) StatusPlay.wCurrentUser=m_wChaUser;
			StatusPlay.dian= m_bdian;
			if(m_bdian) StatusPlay.wCurrentUser=m_wChaUser;
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
	/////////////////////////////////////////添加的消息处理///////////////////////////////////////
		//玩家不撅棍
	case SUB_C_NOT_JUE_GUN:
		{
			CMD_C_NotLiOrJue *pNotJue;
			pNotJue = (CMD_C_NotLiOrJue *)pDataBuffer;
			m_bNotLiOrJue[pNotJue->wNotLiOrJueChair] = true;

			m_cbNotJue++;
			if (m_cbNotJue == 2)
			{
				CMD_S_GameStart GameStart; 
				//for (BYTE i=0; i<GAME_PLAYER; i++)
				//{
				//	if (m_bLiOrJue[i] == false)
				//	{
				//		m_wCurrentUser = i;
				//		GameStart.wCurrentUser = m_wCurrentUser;
				//		break;
				//	}
				//}
				m_wCurrentUser = m_wLiGun;
				GameStart.wCurrentUser = m_wLiGun;

				for (BYTE j=0; j<GAME_PLAYER; j++)
					m_pITableFrame->SendTableData(j, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));


			}
			return true;
		}
		//立棍超时，全部不立棍
	case SUB_C_LI_GUN_SUPERTIME:
		{
			m_cbLiSuperCount++;
			if (m_cbLiSuperCount == 4)
			{
				//发送消息
				CMD_S_GameStart GameStart;
				GameStart.wCurrentUser=m_wCurrentUser;

				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			}

			return true;
		}
		//玩家发送立棍请求，找到他的对家，发送玩家立棍的消息，看对家是否同意
	case SUB_C_LI_GUN:
		{
			CMD_C_LiGun *pLiGun;
			pLiGun = (CMD_C_LiGun *)pDataBuffer;
			
			WORD wLiGunChair = pLiGun->wLiGunUserChairID;

			//得到请求立棍对家的座位号
			WORD wAnswerChair = ( wLiGunChair + 2) % GAME_PLAYER;
			CMD_S_LiGunRequest LiGunRequest;
			ZeroMemory(&LiGunRequest, sizeof(LiGunRequest));
			LiGunRequest.wRequest = wAnswerChair;

			//发送消息
			for (BYTE i=0; i<GAME_PLAYER; i++)
				m_pITableFrame->SendTableData(i, SUB_S_LI_GUN_REQUEST, &LiGunRequest, sizeof(LiGunRequest));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_LI_GUN_REQUEST, &LiGunRequest, sizeof(LiGunRequest));

			return true;

		}
	//立棍的回答
	case SUB_C_LI_GUN_ANSWER:
		{
			CMD_C_LiGunAnswer *pLiGunAnswer;
			pLiGunAnswer = (CMD_C_LiGunAnswer *)pDataBuffer;
			WORD wChairID = ((pLiGunAnswer->wAgreeChairID)+2)%GAME_PLAYER;
			BYTE cbLiOrJue = pLiGunAnswer->cbLiOrJue;

			//如果玩家同意了立棍或者撅棍
			if (pLiGunAnswer->cbAnswer == 1)
			{
				m_bLiOrJue[pLiGunAnswer->wAgreeChairID] = true;
				CMD_S_CanJueGun CanJueGun;
				ZeroMemory(&CanJueGun, sizeof(CanJueGun));
				BYTE temp = 0;
				//玩家同意了立棍的时候，向对方两个玩家发送撅棍消息，点亮他们的撅棍按钮
				if (cbLiOrJue == 1)
				{
					m_wLiGun = wChairID;
					//得到撅棍玩家的位置
					for (WORD i=0; i<GAME_PLAYER; i++)
					{
						if (i != wChairID && i != pLiGunAnswer->wAgreeChairID)
						{
							CanJueGun.wCanJueChair[temp] = i;
							temp++;
						}
					}
					for (WORD j=0; j<GAME_PLAYER; j++)
						m_pITableFrame->SendTableData(j, SUB_S_JUE_GUN, &CanJueGun, sizeof(CanJueGun));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_JUE_GUN, &CanJueGun, sizeof(CanJueGun));

				}
				//玩家同意了撅棍，发送游戏开始消息
				if (cbLiOrJue == 2)
				{
					m_wJueGun = wChairID;
					CMD_S_GameStart GameStart;
					ZeroMemory(&GameStart, sizeof(GameStart));
					//for (WORD i=0; i<GAME_PLAYER; i++)
					//{
					//	if (m_bLiOrJue[i] == false)
					//	{
					//		GameStart.wCurrentUser = i;
					//		m_wCurrentUser = i;
					//		break;
					//	}
					//}
					m_wCurrentUser = m_wJueGun;
					GameStart.wCurrentUser = m_wJueGun;
					//CString s;
					//s.Format("%d, %d, %d, %d, %d", m_wCurrentUser, m_bLiOrJue[0], m_bLiOrJue[1], m_bLiOrJue[2], m_bLiOrJue[3]);
					//AfxMessageBox(s);

					for (BYTE j=0; j<GAME_PLAYER; j++)
						m_pITableFrame->SendTableData(j, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

				}
			}
			//玩家不同意立棍，那么这两个人全部不能立棍、撅棍，看另外两家是不是立棍
			else
			{
				//不同意立棍的时候
				if (cbLiOrJue == 1)
				{
					//如果玩家不同意对方立棍，那么，如果另外两个玩家
					m_bLiOrJue[wChairID] = false;
					m_bLiOrJue[pLiGunAnswer->wAgreeChairID] = false;
					m_bNotLiOrJue[wChairID] = true;
					m_bNotLiOrJue[pLiGunAnswer->wAgreeChairID] = true;

					CMD_S_CanJueGun CanJueGun;
					ZeroMemory(&CanJueGun, sizeof(CanJueGun));
					BYTE temp = 0;

					//得到另外两个玩家的位置
					for (WORD i=0; i<GAME_PLAYER; i++)
					{
						if (i != wChairID && i != pLiGunAnswer->wAgreeChairID)
						{
							CanJueGun.wCanJueChair[temp] = i;
							temp++;
						}
					}

					//如果另外两个玩家没有声明不立棍或者不撅棍，那么向他们发送能够立棍的消息
					if (m_bNotLiOrJue[CanJueGun.wCanJueChair[0]] != true && m_bNotLiOrJue[CanJueGun.wCanJueChair[1]] != true)
					{
						for (WORD j=0; j<GAME_PLAYER; j++)
							m_pITableFrame->SendTableData(j, SUB_S_LI_GUN, &CanJueGun, sizeof(CanJueGun));
						m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_LI_GUN, &CanJueGun, sizeof(CanJueGun));
					}
					//否则的话，直接开始游戏
					else
					{
						CMD_S_GameStart GameStart;
						ZeroMemory(&GameStart, sizeof(GameStart));
						for (BYTE i=0; i<GAME_PLAYER; i++)
						{
							if (m_bLiOrJue[i] == false)
							{
								GameStart.wCurrentUser = i;
								m_wCurrentUser = i;
								break;
							}
						}

						for (BYTE j=0; j<GAME_PLAYER; j++)
							m_pITableFrame->SendTableData(j, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
						m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
					}
				}

				//不同意撅棍,开始游戏
				if (cbLiOrJue == 2)
				{
					CMD_S_GameStart GameStart;
					ZeroMemory(&GameStart, sizeof(GameStart));
					for (BYTE i=0; i<GAME_PLAYER; i++)
					{
						if (m_bLiOrJue[i] == false)
						{
							GameStart.wCurrentUser = i;
							m_wCurrentUser = i;
							break;
						}
					}

					for (BYTE j=0; j<GAME_PLAYER; j++)
						m_pITableFrame->SendTableData(j, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
				}
			}

			return true;
		}
	//不立棍
	case SUB_C_NOT_LI_GUN:
		{
			CMD_C_NotLiOrJue *pNotLiOrJue = (CMD_C_NotLiOrJue *)pDataBuffer;

			m_bNotLiOrJue[pNotLiOrJue->wNotLiOrJueChair] = true;

			BYTE cbCount = 0;
			for (BYTE i=0; i<GAME_PLAYER; i++)
			{
				if (m_bNotLiOrJue[i])
					cbCount++;
			}

			if (cbCount == GAME_PLAYER)
			{
				CMD_S_GameStart GameStart;
				ZeroMemory(&GameStart, sizeof(GameStart));
				GameStart.wCurrentUser = m_wCurrentUser;

				for (WORD j=0; j<GAME_PLAYER; j++)
					m_pITableFrame->SendTableData(j, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
			}

			return true;

		}
	case SUB_C_JUE_GUN:
		{
			CMD_C_LiGun *pJueGun = (CMD_C_LiGun *)pDataBuffer;
			//m_bLiOrJue[pJueGun->wLiGunUserChairID] = true;

			WORD wLiGunChair = pJueGun->wLiGunUserChairID;

			//得到请求立棍对家的座位号
			WORD wAnswerChair = ( wLiGunChair + 2) % GAME_PLAYER;
			CMD_S_LiGunRequest LiGunRequest;
			ZeroMemory(&LiGunRequest, sizeof(LiGunRequest));
			LiGunRequest.wRequest = wAnswerChair;

			//发送消息
			for (BYTE i=0; i<GAME_PLAYER; i++)
				m_pITableFrame->SendTableData(i, SUB_S_JUE_GUN_REQUEST, &LiGunRequest, sizeof(LiGunRequest));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_JUE_GUN_REQUEST, &LiGunRequest, sizeof(LiGunRequest));

			return true;

		}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

			//AfxMessageBox("p");
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserOutCard(pUserData->wChairID,pOutCard->bCardData,pOutCard->bCardCount);
		}
	case SUB_C_PASS_CARD:		//放弃出牌
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
	case SUB_C_GAME_CHA:		//差牌
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


			return OnUserChaCard(pUserData->wChairID,pOutCard->bCardData,pOutCard->bCardCount);
		}
	case SUB_C_GAME_NOCHA:		//放弃叉牌
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserPassChaCard(pUserData->wChairID);
		}
	case SUB_C_GAME_DIAN:		//点牌
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
			/////////////////////////////////////////////////////

			/////////////////////////////////////////////////////
			return OnUserDianCard(pUserData->wChairID,pOutCard->bCardData,pOutCard->bCardCount);;
		}
	case SUB_C_GAME_NODIAN:		//放弃点牌
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			/////////////////////////////////////////////////////

			////////////////////////////////////////////////////
			//消息处理
			return OnUserPassDianCard(pUserData->wChairID);
		}
	}

	return true;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;//游戏进行
	if (wChairID!=m_wCurrentUser) return false;//当前玩家

	//排序扑克
	m_GameLogic.SortCardList( bCardData, bCardCount, ST_ORDER );//大小排序

	//类型判断
	BYTE bCardType=m_GameLogic.GetCardType(bCardData,bCardCount);//获取类型
	if (bCardType==CT_ERROR) return false;

	//更随出牌
	if (m_bTurnCardCount==0) m_bTurnCardCount=bCardCount;//出牌数目
	else if (m_GameLogic.CompareCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bCardCount)==false) return false;//对比扑克

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID])==false) return false;
	m_bCardCount[wChairID]-=bCardCount;//剩余张数

	//出牌记录
	m_bTurnCardCount=bCardCount;
	m_bOutCardCount[wChairID]++;
	CopyMemory(m_bTurnCardData,bCardData,sizeof(BYTE)*bCardCount);//出牌列表
/////////////////一下分为正常,够的情况和点的情况

	/////////////////////////////////////确定玩家名次///////////////////////////////
	if (m_bCardCount[wChairID] == 0)
	{
		m_cbMiCi[m_cbMingCi] = wChairID;
		m_cbMingCi++;
		m_cbCiXu[wChairID] = m_cbMingCi;
	}
	////////////////////////////////////////////////////////////////////////////////
	//切换用户
	m_wTurnWiner=wChairID;//胜利玩家
	if (m_bCardCount[wChairID]!=0)//扑克数目
	{
		if (bCardType!=CT_A44_red || bCardType!=CT_A44_red_black)
		{
				m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
		}
			//////////////////////////////////////////////////////////////////////
		/////////////////////////////////计算叉牌的用户	
		int index = 0;
		if(bCardType == CT_SINGLE)
		{
			for(int i =0; i<GAME_PLAYER; i++)
			{
				if(wChairID == i) continue;
				//////////////////////////////////添加的代码////////////////////////////////
				if (m_bLiOrJue[i]==true) continue;
				//AfxMessageBox("OutCard--1");
				////////////////////////////////////////////////////////////////////////////
				index = m_GameLogic.FindCardNum(m_bHandCardData[i], m_bCardCount[i], bCardData[0]);
				if(index >= 2 && m_bLiOrJue[i]==false)
				{
					m_wChaUser = i;
					m_bgou = true;
					//构造数据
					CMD_S_OutCard OutCard;//用户出牌
					OutCard.bCardCount=bCardCount;
					OutCard.wOutCardUser=wChairID;
					OutCard.wCurrentUser=m_wChaUser;
					CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));

					//发送数据
					WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
					m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CHA,&OutCard,wSendSize);
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CHA,&OutCard,wSendSize);

					return true;

				}

			}

		}

	}
	//else 
	//	m_wCurrentUser=INVALID_CHAIR;	//当前玩家=无效椅子号
/////////////////////////////////////////////////

	//AfxMessageBox("OutCard--2");
	if (m_bCardCount[m_wCurrentUser]!=0 && m_bLiOrJue[m_wCurrentUser] == false)
	{
		m_wCurrentUser = m_wCurrentUser;
	}
	else
	{
		for (BYTE i=0; i<GAME_PLAYER; i++)
		{
			m_wCurrentUser = (m_wCurrentUser + 1)%GAME_PLAYER;

			if (m_bLiOrJue[m_wCurrentUser]!=true && m_bCardCount[m_wCurrentUser]!=0)
				break;
		}
	}

	//CString w;
	//w.Format("%d,%d", m_wCurrentUser, m_bLiOrJue[m_wCurrentUser]);
	//AfxMessageBox(w);
	//构造数据
	CMD_S_OutCard OutCard;//用户出牌
	OutCard.bCardCount=bCardCount;
	OutCard.wOutCardUser=wChairID;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));

	//发送数据
	WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//出牌最大
	if (bCardType==CT_A44_red) m_bTurnCardCount=0;//出牌数目

	//结束判断
	//if (m_wCurrentUser==INVALID_CHAIR) OnEventGameEnd(wChairID,NULL,GER_NORMAL);
	if ( IsGameOver() != 0) OnEventGameEnd(wChairID,NULL,GER_NORMAL);

	return true;
}

//用户放弃出牌
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;//游戏进行
	if ((wChairID!=m_wCurrentUser)||(m_bTurnCardCount==0)) return false;
/////////////////一下分为正常,够的情况和点的情况
	//////设置变量
	////for (BYTE i=0; i<GAME_PLAYER; i++)
	////{
	////	m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;//得到下个玩家
	////	if (m_bCardCount[m_wCurrentUser] != 0 && m_bLiOrJue[m_wCurrentUser] == false)
	////		break;
	////	//if (m_bLiOrJue[m_wCurrentUser]==false) break;
	////}
	////if (m_wCurrentUser==m_wTurnWiner) m_bTurnCardCount=0;//当前玩家==//胜利玩家
	//得到下一个玩家
	BYTE wTemp;
	wTemp = (m_wCurrentUser+1)%GAME_PLAYER;
	if (wTemp==m_wTurnWiner) m_bTurnCardCount=0;//当前玩家==//胜利玩家

	if (wTemp == m_wTurnWiner && m_bCardCount[wTemp]==0)
	{
		m_wCurrentUser = (wTemp + 2) % GAME_PLAYER;
		m_bTurnCardCount = 0;
	}
	else
	{
		for (BYTE i=0; i<GAME_PLAYER; i++)
		{
			m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;//得到下个玩家
			if (m_wCurrentUser==m_wTurnWiner) m_bTurnCardCount=0;//当前玩家==//胜利玩家
			if (m_bCardCount[m_wCurrentUser] != 0 && m_bLiOrJue[m_wCurrentUser] == false)
			break;
		}
		
	}
/////////////////////////////////////////////////
	//发送数据
	CMD_S_PassCard PassCard;
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bNewTurn=(m_bTurnCardCount==0)?TRUE:FALSE;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	return true;
}

//用户叉牌
bool CTableFrameSink::OnUserChaCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;//游戏进行
	if (wChairID!=m_wChaUser) return false;//当前玩家

	//排序扑克
	m_GameLogic.SortCardList( bCardData, bCardCount, ST_ORDER );//大小排序

	//类型判断
	BYTE bCardType=m_GameLogic.GetCardType(bCardData,bCardCount);//获取类型
	if (bCardType==CT_ERROR) return false;

	//更随出牌
	if (m_bTurnCardCount==0) m_bTurnCardCount=bCardCount;//出牌数目
	//else if (m_GameLogic.CompareCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bCardCount)==false) return false;//对比扑克

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID])==false) return false;
	m_bCardCount[wChairID]-=bCardCount;//剩余张数

	//出牌记录
	m_bTurnCardCount=bCardCount;//出牌数目
	m_bOutCardCount[wChairID]++;
	CopyMemory(m_bTurnCardData,bCardData,sizeof(BYTE)*bCardCount);//出牌列表
/////////////////一下分为正常,够的情况和点的情况
	//切换用户
	m_wTurnWiner=wChairID;//胜利玩家
	m_bgou = false;
	if (m_bCardCount[wChairID]!=0)//扑克数目
	{
		//if (bCardType!=CT_A44_red || bCardType!=CT_A44_red_black)
		//	m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
		/////////////////////////////////计算叉牌的用户	
		int index = -1;
		if(bCardType == CT_DOUBLE)
		{
			for(int i =0; i<GAME_PLAYER; i++)
			{
				if(wChairID == i) continue;//自己跳出
				if (m_bLiOrJue[i]) continue;

				index = m_GameLogic.FindCardNum(m_bHandCardData[i], m_bCardCount[i], bCardData[0]);
				/////////////////////////////////下面的条件中，添加了一个条件//////////////////////////
				if(index >= 1)
				{
					m_wChaUser = i;
					m_bdian = true;
					//构造数据
					CMD_S_OutCard OutCard;//用户出牌
					OutCard.bCardCount=bCardCount;
					OutCard.wOutCardUser=wChairID;
					OutCard.wCurrentUser=m_wChaUser;
					CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));

					//发送数据
					WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
					m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_DIAN,&OutCard,wSendSize);
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_DIAN,&OutCard,wSendSize);

					return true;

				}

			}

		}
		if(index == 0)
		{
			//构造数据
			CMD_S_OutCard OutCard;//用户出牌
			OutCard.bCardCount=bCardCount;
			OutCard.wOutCardUser=m_wChaUser;
			OutCard.wCurrentUser=m_wChaUser;
			CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));

			//发送数据
			WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_DIAN,&OutCard,wSendSize);
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_DIAN,&OutCard,wSendSize);

			m_wCurrentUser = m_wChaUser;//当前玩家==//胜利玩家
			 m_bTurnCardCount=0;
			/////////////////////////////////////////////////
			//发送数据
			CMD_S_PassCard PassCard;
			PassCard.wPassUser=m_wTurnWiner;
			PassCard.wCurrentUser=m_wTurnWiner;
			PassCard.bNewTurn=(m_bTurnCardCount==0)?TRUE:FALSE;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));


			return true;
		
		}
	}
	else 
		m_wCurrentUser=INVALID_CHAIR;	//当前玩家=无效椅子号
/////////////////////////////////////////////////
	//构造数据
	CMD_S_OutCard OutCard;//用户出牌
	OutCard.bCardCount=bCardCount;
	OutCard.wOutCardUser=wChairID;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));

	//发送数据
	WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//出牌最大
	if (bCardType==CT_A44_red) m_bTurnCardCount=0;//出牌数目

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameEnd(wChairID,NULL,GER_NORMAL);

	return true;
}

//用户放弃叉牌
bool CTableFrameSink::OnUserPassChaCard(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;//游戏进行
	if ((wChairID!=m_wChaUser)||(m_bTurnCardCount==0)) return false;
/////////////////一下分为正常,够的情况和点的情况
	//设置变量
	//m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;//得到下个玩家
	if (m_bCardCount[m_wCurrentUser] != 0 && m_bLiOrJue[m_wCurrentUser] == false)
	{
		m_wCurrentUser = m_wCurrentUser;
	}
	else 
	{
		for (BYTE i=0; i<GAME_PLAYER; i++)
		{
			m_wCurrentUser = (m_wCurrentUser + 1)%GAME_PLAYER;
			if (m_bCardCount[m_wCurrentUser] != 0 && m_bLiOrJue[m_wCurrentUser] == false)
				break;
		}
	}
	if (m_wCurrentUser==m_wTurnWiner) m_bTurnCardCount=0;
/////////////////////////////////////////////////
	m_bgou = false;
	//发送数据
	CMD_S_PassCard PassCard;
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bNewTurn=(m_bTurnCardCount==0)?TRUE:FALSE;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_NOCHA,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_NOCHA,&PassCard,sizeof(PassCard));

	return true;
}

//用户点牌
bool CTableFrameSink::OnUserDianCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;//游戏进行
	if (wChairID!=m_wChaUser) return false;//当前玩家

	//排序扑克
	m_GameLogic.SortCardList( bCardData, bCardCount, ST_ORDER );//大小排序

	//类型判断
	BYTE bCardType=m_GameLogic.GetCardType(bCardData,bCardCount);//获取类型
	if (bCardType==CT_ERROR) return false;

	//更随出牌
	if (m_bTurnCardCount==0) m_bTurnCardCount=bCardCount;//出牌数目
	//else if (m_GameLogic.CompareCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bCardCount)==false) return false;//对比扑克

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID])==false) return false;
	m_bCardCount[wChairID]-=bCardCount;//剩余张数
	m_bdian = false;
	//出牌记录
	m_bTurnCardCount=bCardCount;//出牌数目
	m_bOutCardCount[wChairID]++;
	CopyMemory(m_bTurnCardData,bCardData,sizeof(BYTE)*bCardCount);//出牌列表

	//切换用户
	m_wTurnWiner=wChairID;//胜利玩家
	if (m_bCardCount[wChairID]!=0)//扑克数目
	{
			CMD_S_OutCard OutCard;//用户出牌
			OutCard.bCardCount=bCardCount;
			OutCard.wOutCardUser=m_wTurnWiner;
			OutCard.wCurrentUser=m_wTurnWiner;
			CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));

			//发送数据
			WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_DIAN,&OutCard,wSendSize);
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_DIAN,&OutCard,wSendSize);


			 m_bTurnCardCount=0;//当前玩家==//胜利玩家
			/////////////////////////////////////////////////
			//发送数据
			CMD_S_PassCard PassCard;
			PassCard.wPassUser=m_wTurnWiner;
			PassCard.wCurrentUser=m_wTurnWiner;
			PassCard.bNewTurn=(m_bTurnCardCount==0)?TRUE:FALSE;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

            m_wCurrentUser = m_wTurnWiner;

			return true;
		
		//}
	}
	else 
		m_wCurrentUser=INVALID_CHAIR;	//当前玩家=无效椅子号
/////////////////////////////////////////////////
	//构造数据
	CMD_S_OutCard OutCard;//用户出牌
	OutCard.bCardCount=bCardCount;
	OutCard.wOutCardUser=wChairID;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));

	//发送数据
	WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//出牌最大
	if (bCardType==CT_A44_red) m_bTurnCardCount=0;//出牌数目

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameEnd(wChairID,NULL,GER_NORMAL);

	return true;
}
//用户放弃点牌
bool CTableFrameSink::OnUserPassDianCard(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;//游戏进行
	if ((wChairID!=m_wChaUser)||(m_bTurnCardCount==0)) return false;

	m_wCurrentUser = m_wTurnWiner;//当前玩家==//胜利玩家
	m_bTurnCardCount=0;

	//////////////////////////////////添加的计算下一个出牌玩家的代码/////////////////////////////////////
	if (m_bCardCount[m_wCurrentUser] != 0 && m_bLiOrJue[m_wCurrentUser] == false)
	{
		m_wCurrentUser = m_wCurrentUser;
	}
	else 
	{
		for (BYTE i=0; i<GAME_PLAYER; i++)
		{
			m_wCurrentUser = (m_wCurrentUser + 1)%GAME_PLAYER;
			if (m_bCardCount[m_wCurrentUser] != 0 && m_bLiOrJue[m_wCurrentUser] == false)
				break;
		}
	}

	/////////////////////////////////////////////////
	//发送数据
	m_bdian = false;
	CMD_S_PassCard PassCard;
	PassCard.wPassUser=m_wTurnWiner;
	PassCard.wCurrentUser=m_wTurnWiner;
	PassCard.bNewTurn=(m_bTurnCardCount==0)?TRUE:FALSE;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	return true;
}

/////////////////////////////////////////////添加的结束函数//////////////////////////////
BYTE CTableFrameSink::IsGameOver()
{
	//////////////首先分析没有立棍、撅棍的情况////////////////////////
	//当前两名为同一方人时……
	if ( (m_cbCiXu[0]+m_cbCiXu[2]==3) || (m_cbCiXu[1]+m_cbCiXu[3]==3) )
		m_cbEndType = 1;
	
	//当一方第一、第三名时……
	if ( (m_cbCiXu[0]+m_cbCiXu[2]==4) || (m_cbCiXu[1]+m_cbCiXu[3]==4) )
		m_cbEndType = 2;

	//当一方第二、第三名时……
	if ( (m_cbCiXu[0]+m_cbCiXu[2]==5) || (m_cbCiXu[1]+m_cbCiXu[3]==5) )
		m_cbEndType = 3;

	/////////////当有立棍、撅棍的情况时///////////////////////////////
	WORD wNotLiGun = 10;
	WORD wNotJueGun = 10;
	WORD wLiGun = 10;
	WORD wJueGun = 10;

	for (BYTE i=0; i<GAME_PLAYER; i++)
	{
		if (m_bLiOrJue[i]==true && (i==0 || i==2))
			wNotLiGun = i;
		if (m_bLiOrJue[i]==true && (i==1 || i==3))
			wNotJueGun = i;
	}

	if (wNotLiGun != 10) wLiGun = (wNotLiGun + 2)%GAME_PLAYER;
	if (wNotJueGun != 10) wJueGun = (wNotJueGun + 2)%GAME_PLAYER;

	//////////////////既有立棍，又有撅棍时
	if (wLiGun != 10 && wJueGun != 10)
	{
		if (m_cbCiXu[0] == wLiGun || m_cbCiXu[0] == wJueGun) m_cbEndType = 4;
	}

	/////////////////立棍、撅棍有一个人
	if (wLiGun != 10 || wJueGun != 10)
	{
		//立棍方赢了
		if (m_cbCiXu[0] == wLiGun || m_cbCiXu[0] == wJueGun) m_cbEndType = 5;
		//立棍方输了
		else
		{
			if (wLiGun != 0 && m_cbCiXu[0] != wLiGun && m_cbCiXu[0] != 255)
				m_cbEndType = 6;
			if (wJueGun != 0 && m_cbCiXu[0] != wJueGun && m_cbCiXu[0] != 255)
				m_cbEndType = 6;
		}
	}

	return m_cbEndType;
}


//算分函数
void CTableFrameSink::CountScore(LONG lScore[])
{
	switch (m_cbEndType)
	{
	case 1:
		{
			if (m_cbMiCi[0] == 0 || m_cbMiCi[0] == 2)
			{
				lScore[0] = 8;
				lScore[1] = -8;
				lScore[2] = 8;
				lScore[3] = -8;
			}
			else
			{
				lScore[0]=-8;
				lScore[1]=8;
				lScore[2]=-8;
				lScore[3]=8;
			}
			break;
		}
	case 2:
		{
			if (m_cbMiCi[0] == 0 || m_cbMiCi[0] == 2)
			{
				lScore[0]=4;
				lScore[1]=-4;
				lScore[2]=4;
				lScore[3]=-4;
			}
			else
			{
				lScore[0]=-4;
				lScore[1]=4;
				lScore[2]=-4;
				lScore[3]=4;
			}
			break;
		}
	case 3:
		{
			lScore[0]=0;
			lScore[1]=0;
			lScore[2]=0;
			lScore[3]=0;

			break;
		}
	case 4:
		{
			if (m_cbMiCi[0] == 0 || m_cbMiCi[0] == 2)
			{
				lScore[0]=16;
				lScore[1]=-16;
				lScore[2]=16;
				lScore[3]=-16;
			}
			else
			{
				lScore[0]=-16;
				lScore[1]=16;
				lScore[2]=-16;
				lScore[3]=16;
			}
			break;
		}
	case 5:
		{
			if (m_cbMiCi[0] == 0 || m_cbMiCi[0] == 2)
			{
				lScore[0]=16;
				lScore[1]=-16;
				lScore[2]=16;
				lScore[3]=-16;
			}
			else
			{
				lScore[0]=-16;
				lScore[1]=16;
				lScore[2]=-16;
				lScore[3]=16;
			}
			break;
		}
	case 6:
		{
			if (m_cbMiCi[0] == 0 || m_cbMiCi[0] == 2)
			{
				lScore[0]=16;
				lScore[1]=-16;
				lScore[2]=16;
				lScore[3]=-16;
			}
			else
			{
				lScore[0]=-16;
				lScore[1]=16;
				lScore[2]=-16;
				lScore[3]=16;
			}
			break;
		}
	}
}