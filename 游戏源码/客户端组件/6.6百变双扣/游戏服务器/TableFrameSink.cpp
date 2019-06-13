#include "StdAfx.h"
#include "TableFrameSink.h"
#include "cmath"
//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));
	ZeroMemory(m_cbGongxian,sizeof(m_cbGongxian));

	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

	//扑克信息
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));


	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
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
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));
	ZeroMemory(m_cbGongxian,sizeof(m_cbGongxian));

	//出牌信息
	m_cbTurnCardCount=0;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

	//扑克信息
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

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
	m_pITableFrame->SetGameStatus(GS_RF_PLAYING);

	//混乱扑克
	srand((unsigned)time(NULL));
	BYTE cbRandCard[FULL_COUNT];
	m_GameLogic.RandCardList(cbRandCard,CountArray(cbRandCard));

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_cbHandCardCount[i]=MAX_COUNT;
		CopyMemory(&m_cbHandCardData[i],&cbRandCard[i*m_cbHandCardCount[i]],sizeof(BYTE)*m_cbHandCardCount[i]);
	}
    
	//设置用户
	if(m_wBankerUser == INVALID_CHAIR)
		m_wBankerUser = rand()%4;
	m_wTurnWiner=m_wBankerUser;
	m_wCurrentUser=m_wBankerUser;

	//构造变量
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//设置变量
	GameStart.wCurrentUser=m_wCurrentUser;
    GameStart.lCellScore=m_pGameServiceOption->lCellScore;

	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//构造扑克
		ASSERT(CountArray(GameStart.cbCardData)>=m_cbHandCardCount[i]);
		CopyMemory(GameStart.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
		CopyMemory(GameStart.cbPartnerCardData,m_cbHandCardData[(i+2)%GAME_PLAYER],sizeof(BYTE)*m_cbHandCardCount[(i+2)%GAME_PLAYER]);

		GameStart.bTrustee[i]=m_bTrustee[i];
		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	//排列扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i],ST_ORDER);
	}

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_DISMISS:		//游戏解散
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//构造数据
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

			//胜利信息
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_cbHandCardCount[i]!=0)
				{
					m_wWinOrder[m_wWinCount++]=i;
				}
			}

			//剩余扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}

			//引用变量
			LONG lTotalTimes[4];
			LONG lMaxTimes[4];
			
			//计算贡献分和最大倍数
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				GetTotalTimes(m_cbGongxian,i,lTotalTimes[i],lMaxTimes[i]);
			}
            
			
			if (m_wWinOrder[0]==(m_wWinOrder[2]+2)%GAME_PLAYER) //单扣判断
			{
				//取赢家中最大的倍率
				LONG lBothMaxTimes=max(lMaxTimes[m_wWinOrder[0]],lMaxTimes[m_wWinOrder[2]]);
	
				//游戏积分
				ScoreInfo[m_wWinOrder[0]].lScore = 4*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[0]].ScoreKind = enScoreKind_Win;
				ScoreInfo[m_wWinOrder[2]].lScore = 4*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[2]].ScoreKind = enScoreKind_Win;
				ScoreInfo[m_wWinOrder[1]].lScore= -4*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[1]].ScoreKind = enScoreKind_Lost;
				ScoreInfo[m_wWinOrder[3]].lScore= -4*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[3]].ScoreKind = enScoreKind_Lost;
				
				//切换庄家
				m_wBankerUser=m_wWinOrder[0];

			}			
			else if (m_wWinOrder[0]==(m_wWinOrder[1]+2)%GAME_PLAYER)//双扣判断
			{
				//取赢家中最大的倍率
				LONG lBothMaxTimes=max(lMaxTimes[m_wWinOrder[0]],lMaxTimes[m_wWinOrder[1]]);

				//游戏积分
				ScoreInfo[m_wWinOrder[0]].lScore = 6*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[0]].ScoreKind = enScoreKind_Win;
				ScoreInfo[m_wWinOrder[1]].lScore = 6*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[1]].ScoreKind = enScoreKind_Win;
				ScoreInfo[m_wWinOrder[2]].lScore = -6*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[2]].ScoreKind = enScoreKind_Lost;
				ScoreInfo[m_wWinOrder[3]].lScore = -6*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[3]].ScoreKind = enScoreKind_Lost;
			
				//切换庄家
				m_wBankerUser=rand()%4;
			}			
			else //平扣判断
			{
				//取赢家中最大的倍率
				LONG lBothMaxTimes=max(lMaxTimes[m_wWinOrder[0]],lMaxTimes[m_wWinOrder[3]]);

				//游戏积分
				ScoreInfo[m_wWinOrder[0]].lScore =2*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[0]].ScoreKind = enScoreKind_Draw;
				ScoreInfo[m_wWinOrder[2]].lScore = -2*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[2]].ScoreKind = enScoreKind_Draw;
				ScoreInfo[m_wWinOrder[1]].lScore = -2*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[1]].ScoreKind = enScoreKind_Draw;
				ScoreInfo[m_wWinOrder[3]].lScore = 2*m_pGameServiceOption->lCellScore*lBothMaxTimes;
				ScoreInfo[m_wWinOrder[3]].ScoreKind = enScoreKind_Draw;

				//切换庄家
				m_wBankerUser=rand()%4;
			}
			//计算贡献分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				for (WORD j=0;j<GAME_PLAYER;j++)
				{
					if(i==j) continue;
					if(lTotalTimes[i]==0) continue;
					ScoreInfo[i].lScore +=m_pGameServiceOption->lCellScore*lTotalTimes[i];
					ScoreInfo[j].lScore -=m_pGameServiceOption->lCellScore*lTotalTimes[i];
				}
			}
			//游戏积分
			for (WORD i =0;i<GAME_PLAYER;i++)
			{
				GameEnd.lGameScore[i] = ScoreInfo[i].lScore;
			}

			//计算税收
			if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				//扣税变量
				BYTE wRevenue=m_pGameServiceOption->wRevenue;

				//计算税收
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (GameEnd.lGameScore[i]>0L)
					{
						//计算税收
						LONG lRevenue=GameEnd.lGameScore[i]*wRevenue/100L;

						//积分调整
						GameEnd.lGameTax+=lRevenue;
						ScoreInfo[i].lRevenue=lRevenue;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]-lRevenue;
					}
				}
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//统计积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//写入积分
				m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore,ScoreInfo[i].lRevenue,ScoreInfo[i].ScoreKind);
			}
			//结束游戏
			m_pITableFrame->ConcludeGame();


			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}
			//引用变量
			LONG lTotalTimes[4];
			LONG lMaxTimes[4];
			
			//计算贡献分和最大倍数
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				GetTotalTimes(m_cbGongxian,i,lTotalTimes[i],lMaxTimes[i]);
			}

			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//过滤自己
				if(i==wChairID) continue;
				GameEnd.lGameScore[wChairID] -= lTotalTimes[i]*m_pGameServiceOption->lCellScore;

			}
			if(GameEnd.lGameScore[wChairID]==0)
			{
				GameEnd.lGameScore[wChairID] -=m_pGameServiceOption->lCellScore*2*6 ;
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//变量定义
			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

			//修改积分
			ScoreInfo.ScoreKind=enScoreKind_Flee;
			ScoreInfo.lScore=GameEnd.lGameScore[wChairID];
			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo.lScore,0,ScoreInfo.ScoreKind);


			//结束游戏
			m_pITableFrame->ConcludeGame();

			//切换庄家
			m_wBankerUser=(m_wBankerUser+1)%GAME_PLAYER;

			return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_RF_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));
            
            
			//游戏变量
			StatusFree.wBankerUser=m_wBankerUser;
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_RF_PLAYING:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;
		
			StatusPlay.wBankerUser=m_wBankerUser;

			//胜利信息
			StatusPlay.wWinCount=m_wWinCount;
			CopyMemory(StatusPlay.wWinOrder,m_wWinOrder,sizeof(StatusPlay.wWinOrder));
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//出牌信息
			StatusPlay.wTurnWiner=m_wTurnWiner;
			StatusPlay.cbTurnCardCount=m_cbTurnCardCount;
			CopyMemory(StatusPlay.cbTurnCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));
			CopyMemory(StatusPlay.cbMagicCardData,m_cbMagicCardData,m_cbTurnCardCount*sizeof(BYTE));

			//扑克信息
			CopyMemory(StatusPlay.cbHandCardCount,m_cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(StatusPlay.cbHandCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*m_cbHandCardCount[wChairID]);
			CopyMemory(StatusPlay.cbPartnerCardData,m_cbHandCardData[(wChairID+2)%GAME_PLAYER],sizeof(BYTE)*m_cbHandCardCount[(wChairID+2)%GAME_PLAYER]);

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
	case SUB_C_OUT_CARD:		//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//效验数据
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0]))) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserOutCard(pUserData->wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
	case SUB_C_PASS_CARD:
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserPassCard(pUserData->wChairID);
		}
	case SUB_C_TRUSTEE:
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pDataBuffer;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();

			m_bTrustee[pUserData->wChairID]=pTrustee->bTrustee;
			CMD_S_Trustee Trustee;
			Trustee.bTrustee=pTrustee->bTrustee;
			Trustee.wChairID = pUserData->wChairID;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));

			return true;
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
	//庄家设置
	if ((bLookonUser==false)&&(m_wBankerUser==INVALID_CHAIR))
	{
		m_wBankerUser=pIServerUserItem->GetChairID();
	}


	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if (bLookonUser==false) m_wBankerUser=INVALID_CHAIR;


	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_RF_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) return false;


	//变幻扑克
	BYTE cbMagicCardData[MAX_COUNT];
	m_GameLogic.MagicCardData(bCardData,bCardCount,cbMagicCardData);

	//类型判断
	BYTE cbStarLevel;
	BYTE bCardType=m_GameLogic.GetCardType(cbMagicCardData,bCardCount,cbStarLevel);
	if (bCardType==CT_ERROR) return false;

	if(bCardType>=CT_BOMB)
	{
		m_cbGongxian[wChairID][cbStarLevel-4]++;
	}

	//出牌判断
	if (m_cbTurnCardCount!=0)
	{
		//对比扑克
		if (m_GameLogic.CompareCard(m_cbMagicCardData,cbMagicCardData,m_cbTurnCardCount,bCardCount)==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置变量
	m_cbTurnCardCount=bCardCount;
	m_cbHandCardCount[wChairID]-=bCardCount;
	CopyMemory(m_cbTurnCardData,bCardData,sizeof(BYTE)*bCardCount);
	CopyMemory(m_cbMagicCardData,cbMagicCardData,sizeof(BYTE)*bCardCount);

	//胜利判断
	if (m_cbHandCardCount[wChairID]==0) 
	{
		//设置用户
		m_wWinOrder[m_wWinCount++]=wChairID;

		//用户统计
		if (m_cbHandCardCount[(wChairID+2)%GAME_PLAYER]==0) 
			m_wCurrentUser=INVALID_CHAIR;
	}

	//切换用户
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//设置用户
		m_wTurnWiner=wChairID;
		m_wCurrentUser=(wChairID+1)%GAME_PLAYER;

		//用户调整
		while (m_cbHandCardCount[m_wCurrentUser]==0) 
			m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
	}

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbCardCount=bCardCount;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.cbCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

	//发送数据
	WORD wHeadSize=sizeof(OutCard)-sizeof(OutCard.cbCardData);
	WORD wSendSize=wHeadSize+OutCard.cbCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameEnd(wChairID,NULL,GER_NORMAL);

	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_RF_PLAYING) return true;
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return false;

	//用户切换
	do
	{
		//设置用户
		m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;

		//完成判断
		if (m_wCurrentUser==m_wTurnWiner)
		{
			//设置变量
			m_cbTurnCardCount=0;

			//接风处理
			if (m_cbHandCardCount[m_wTurnWiner]==0)
			{
				m_wCurrentUser=(m_wTurnWiner+2)%GAME_PLAYER;
			}
			break;
		}

		//继续处理
		if (m_cbHandCardCount[m_wCurrentUser]!=0) break;

	} while(true);

	//构造消息
	CMD_S_PassCard PassCard;
	PassCard.wPassCardUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.cbTurnOver=(m_cbTurnCardCount==0)?TRUE:FALSE;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	return true;
}

//计算贡献
VOID CTableFrameSink::GetTotalTimes(BYTE cbGongxian[GAME_PLAYER][15],WORD wChairID,LONG &lTotalTimes,LONG &lMaxTimes)
{
	BYTE cbTempGongxian[15];
	CopyMemory(cbTempGongxian,cbGongxian[wChairID],sizeof(cbTempGongxian));
	//3个四星级以上 3*4 算5
	if(cbTempGongxian[0]>=3)
	{
		ASSERT((0+cbTempGongxian[0]-1)<15);
		cbTempGongxian[0+cbTempGongxian[0]-2]++;
		cbTempGongxian[0]=0;
	}
	//2个五星级以上 
	if(cbTempGongxian[1]>=2)
	{
		ASSERT((1+cbTempGongxian[1]-1)<15);
		cbTempGongxian[1+cbTempGongxian[1]-1]++;
		cbTempGongxian[1]=0;
	}
	//2个六星级以上
	if(cbTempGongxian[2]>=2)
	{		
		ASSERT((2+cbTempGongxian[2]-1)<15);
		cbTempGongxian[2+cbTempGongxian[2]-1]++;
		cbTempGongxian[2]=0;
	}
	//2个7星级以上	
	if(cbTempGongxian[3]>=2)
	{
		ASSERT((3+cbTempGongxian[3]-1)<15);
		cbTempGongxian[3+cbTempGongxian[3]-1]++;
		cbTempGongxian[3]=0;
	}
	//2个8星级以上	
	if(cbTempGongxian[4]>=2)
	{
		ASSERT((4+cbTempGongxian[4]-1)<15);
		cbTempGongxian[4+cbTempGongxian[4]-1]++;
		cbTempGongxian[4]=0;
	}
	//2个9星级以上	
	if(cbTempGongxian[5]>=2)
	{
		ASSERT((5+cbTempGongxian[5]-1)<15);
		cbTempGongxian[5+cbTempGongxian[5]-1]++;
		cbTempGongxian[5]=0;
	}
	//2个10星级以上	
	if(cbTempGongxian[6]>=2)
	{
		ASSERT((6+cbTempGongxian[6]-1)<15);
		cbTempGongxian[6+cbTempGongxian[6]-1]++;
		cbTempGongxian[6]=0;
	}
	//2个11星级以上	
	if(cbTempGongxian[7]>=2)
	{
		ASSERT((7+cbTempGongxian[7]-1)<15);
		cbTempGongxian[7+cbTempGongxian[7]-1]++;
		cbTempGongxian[7]=0;
	}
	//2个12星级以上	
	if(cbTempGongxian[8]>=2)
	{
		ASSERT((8+cbTempGongxian[8]-1)<15);
		cbTempGongxian[8+cbTempGongxian[8]-1]++;
		cbTempGongxian[8]=0;
	}
	//2个13星级以上	
	if(cbTempGongxian[9]>=2)
	{
		ASSERT((9+cbTempGongxian[9]-1)<15);
		cbTempGongxian[9+cbTempGongxian[9]-1]++;
		cbTempGongxian[9]=0;
	}
	//2个14星级以上	
	if(cbTempGongxian[10]>=2)
	{
		ASSERT((10+cbTempGongxian[10]-1)<15);
		cbTempGongxian[10+cbTempGongxian[10]-1]++;
		cbTempGongxian[10]=0;
	}
	//2个15星级以上	
	if(cbTempGongxian[11]>=2)
	{
		ASSERT((11+cbTempGongxian[11]-1)<15);
		cbTempGongxian[11+cbTempGongxian[11]-1]++;
		cbTempGongxian[11]=0;
	}
	//2个16星级以上	
	if(cbTempGongxian[12]>=2)
	{
		ASSERT((12+cbTempGongxian[12]-1)<15);
		cbTempGongxian[12+cbTempGongxian[12]-1]++;
		cbTempGongxian[12]=0;
	}
	//2个17星级以上	
	if(cbTempGongxian[13]==2)
	{
		ASSERT((13+cbTempGongxian[13]-1)<15);
		cbTempGongxian[13+cbTempGongxian[13]-1]++;
		cbTempGongxian[13]=0;
	}
	//其实上面的完全可以用一个循环写  为了让人看得更清楚 退尔求其次才用if
	//算贡献的时候返回所有倍数 不累加
	//从6番开始算

	//从底往高
	lTotalTimes=0L;
	for(INT i=2;i<15;i++)
	{
		//过滤贡献为0
		if(cbTempGongxian[i]==0) continue;
		//累加
		LONG lTempTotalTimes = pow(2.0,i);
		if(lTempTotalTimes>lTotalTimes)
			lTotalTimes=lTempTotalTimes;
	}

	//算最大倍 从5番算起
	lMaxTimes=1L;
	for(INT i=14;i>=1;i--)
	{
		//过滤贡献为0
		if(cbTempGongxian[i]==0) continue;
		//累加
		 LONG lTempMaxTimes=pow(2.0,i);
		 if(lTempMaxTimes>lMaxTimes)
			 lMaxTimes=lTempMaxTimes;
		 if(lMaxTimes>=16)
			 lMaxTimes=16;
	}

	//现在要从高往低算 
	CopyMemory(cbTempGongxian,cbGongxian[wChairID],sizeof(cbTempGongxian));
	INT lMinGongxian=0;
	for(INT i=0;i<15;i++)
	{
		//过滤无贡献
		if(cbTempGongxian[i]==0) continue;
		lMinGongxian=i;
		break;
	}
	for (INT i=14;i>lMinGongxian;i--)
	{
		if(cbTempGongxian[i]==0) continue;
		ASSERT(cbTempGongxian[lMinGongxian]!=0);
		cbTempGongxian[lMinGongxian]+=cbTempGongxian[i];
	}

	//如果是4星
	if((lMinGongxian==0)&&(cbTempGongxian[lMinGongxian]>2))
	{
		LONG lTempTotalTimes=pow(2.0,cbTempGongxian[lMinGongxian]-2);
		if(lTempTotalTimes>lMaxTimes)
		{
			lMaxTimes=lTempTotalTimes;
			if(lMaxTimes>=16)
				lMaxTimes=16;
		}

	}
	if(lMinGongxian==1)
	{
		LONG lTempTotalTimes=pow(2.0,cbTempGongxian[lMinGongxian]-1);
		if(lTempTotalTimes>lMaxTimes)
		{
			lMaxTimes=lTempTotalTimes;
			if(lTempTotalTimes>=16)
				lMaxTimes=16;
		}

	}
	if(lMinGongxian>1)
	{
		LONG lTempTotalTimes=pow(2.0,cbTempGongxian[lMinGongxian]-1);
		if(lTempTotalTimes>lMaxTimes)
		{
			lMaxTimes=lTempTotalTimes;
			if(lMaxTimes>=16)
				lMaxTimes=16;
		}
		if(lTempTotalTimes>lTotalTimes)
		{
			lTotalTimes=lTempTotalTimes;
		}
	}
	
}
//////////////////////////////////////////////////////////////////////////
