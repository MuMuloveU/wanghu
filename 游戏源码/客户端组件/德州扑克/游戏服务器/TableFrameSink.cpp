#include "StdAfx.h"
#include "TableFrameSink.h"
//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_AllReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

void TraceMessage(LPCTSTR pszMessage);

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	//玩家变量
	m_wDUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	//m_wWinner = INVALID_CHAIR;

	//玩家状态
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	//扑克变量
	m_cbSendCardCount = 0;
	ZeroMemory(m_cbCenterCardData,sizeof(m_cbCenterCardData));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//加注变量
	m_lCellScore = 0L;
	m_lTurnLessScore = 0L;
	m_lAddLessScore = 0L;
	m_lTurnMaxScore = 0L;
	m_wOperaCount = 0;
	m_cbBalanceCount = 0;
	m_lBalanceScore =0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));
	ZeroMemory(m_cbShowHand,sizeof(m_cbShowHand));

	//税收变量
	//ZeroMemory(m_bUserTax,sizeof(m_bUserTax));
	//ZeroMemory(m_bLastTax,sizeof(m_bLastTax));

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
	//玩家变量
	m_wCurrentUser = INVALID_CHAIR;

	//玩家状态
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	//扑克变量
	m_cbSendCardCount = 0;
	ZeroMemory(m_cbCenterCardData,sizeof(m_cbCenterCardData));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//加注变量
	m_lCellScore = 0L;
	m_lTurnLessScore = 0L;
	m_lTurnMaxScore = 0L;
	m_lAddLessScore = 0L;
	m_wOperaCount = 0;
	m_cbBalanceCount = 0;
	m_lBalanceScore = 0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));
	ZeroMemory(m_cbShowHand,sizeof(m_cbShowHand));

	//税收变量
	//ZeroMemory(m_bUserTax,sizeof(m_bUserTax));	
	//ZeroMemory(m_bLastTax,sizeof(m_bLastTax));	

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
	ASSERT(wChairID<m_wPlayerCount);
	return (m_cbPlayStatus[wChairID]==TRUE)?true:false;
}

//用户坐下 
bool __cdecl CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{	
	if(bLookonUser || m_wDUser==INVALID_CHAIR)return true;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(i==wChairID)continue;
		if (m_pITableFrame->GetServerUserItem(i)!=NULL)
		{
			break;
		}
	}

	//庄家设置
	if(i==m_wPlayerCount)m_wDUser=INVALID_CHAIR;

	return true;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	//游戏变量
	WORD wUserCount=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);

		//无效用户
		if (pIServerUserItem==NULL) continue;

		//获取积分
		const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();
		ASSERT(pUserScore->lScore>=m_pGameServiceOption->lCellScore);
		m_lUserMaxScore[i]=pUserScore->lScore;

		//设置状态
		m_cbPlayStatus[i]=TRUE;
		wUserCount++;
	}
	m_lCellScore=m_pGameServiceOption->lCellScore;

	//混乱扑克
	srand((unsigned int)time(NULL));
	BYTE cbRandCard[FULL_COUNT];
	ZeroMemory(cbRandCard,sizeof(cbRandCard));
	m_GameLogic.RandCardList(cbRandCard,wUserCount*MAX_COUNT+MAX_CENTERCOUNT);

	//用户扑克
	WORD wCardCount=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==TRUE)
		{
			CopyMemory(&m_cbHandCardData[i],&cbRandCard[(wCardCount++)*MAX_COUNT],sizeof(BYTE)*MAX_COUNT);
		}
	}

	//中心扑克
	CopyMemory(m_cbCenterCardData,&cbRandCard[wUserCount*MAX_COUNT],CountArray(m_cbCenterCardData));

	//扑克数目
	m_cbSendCardCount = 0;
	m_cbBalanceCount = 0;

	//首家判断
	if(m_wDUser == INVALID_CHAIR)m_wDUser = 0;
	else m_wDUser =(m_wDUser+1)%GAME_PLAYER;

	//盲注玩家
	WORD wPlayer[]={INVALID_CHAIR,INVALID_CHAIR,INVALID_CHAIR},wPlayerCount = 0;
	WORD wNextUser = m_wDUser;
	do
	{
		if (m_cbPlayStatus[wNextUser]==TRUE) 
		{
			wPlayer[wPlayerCount++] = wNextUser;	
		}
		wNextUser =(wNextUser+1)%GAME_PLAYER;
	}while(wPlayerCount < 3);

	m_wDUser = wPlayer[0];
	m_wCurrentUser = wPlayer[2];

	//当前下注
	m_lTableScore[m_wDUser] = m_lCellScore;
	m_lTableScore[wPlayer[1]] = 2*m_lCellScore;
	m_lTotalScore[m_wDUser] = m_lCellScore;
	m_lTotalScore[wPlayer[1]] = 2*m_lCellScore;

	//设置变量
	m_lBalanceScore = 2L*m_lCellScore;
	m_lTurnMaxScore = m_lUserMaxScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];
	m_lTurnLessScore = m_lBalanceScore - m_lTotalScore[m_wCurrentUser];
	m_lAddLessScore = 2L*m_lCellScore+m_lTurnLessScore;

	//构造变量
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	GameStart.wDUser = m_wDUser;
	GameStart.wMaxChipInUser = wPlayer[1];
	GameStart.wCurrentUser = m_wCurrentUser;
	GameStart.lCellScore = m_pGameServiceOption->lCellScore;
	GameStart.lAddLessScore = m_lAddLessScore;
	GameStart.lTurnLessScore = m_lTurnLessScore;
	GameStart.lTurnMaxScore = m_lTurnMaxScore;

	//作弊/漏洞数据
	//CopyMemory(GameStart.cbAllData,m_cbHandCardData,sizeof(m_cbHandCardData));

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			//发送数据
			CopyMemory(GameStart.cbCardData[i],m_cbHandCardData[i],MAX_COUNT);
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
			ZeroMemory(GameStart.cbCardData[i],MAX_COUNT);
		}	
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.cbTotalEnd = 1;

			//扑克数据
			BYTE cbEndCardData[GAME_PLAYER][MAX_CENTERCOUNT];
			ZeroMemory(cbEndCardData,sizeof(cbEndCardData));

			try{
				//获取扑克
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//用户过滤
					if (m_cbPlayStatus[i]==FALSE) continue;

					//最大牌型
					BYTE cbEndCardKind = m_GameLogic.FiveFromSeven(m_cbHandCardData[i],MAX_COUNT,m_cbCenterCardData,MAX_CENTERCOUNT,cbEndCardData[i],MAX_CENTERCOUNT);
					ASSERT(cbEndCardKind!=FALSE);			
					CopyMemory(GameEnd.cbLastCenterCardData[i],cbEndCardData[i],sizeof(BYTE)*CountArray(cbEndCardData));
				}
			}catch(...)
			{
				TraceMessage("用户过滤v最大牌型");
				ASSERT(FALSE);
			}

			//总下注备份
			LONG lTotalScore[GAME_PLAYER];
			ZeroMemory(lTotalScore,sizeof(lTotalScore));
			CopyMemory(lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));

			//胜利列表
			UserWinList WinnerList[GAME_PLAYER];
			ZeroMemory(WinnerList,sizeof(WinnerList));

			//临时数据
			BYTE bTempData[GAME_PLAYER][MAX_CENTERCOUNT];
			CopyMemory(bTempData,cbEndCardData,GAME_PLAYER*MAX_CENTERCOUNT);

			WORD wWinCount=0;
			try{
				//用户得分顺序
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//查找最大用户
					if(!m_GameLogic.SelectMaxUser(bTempData,WinnerList[i],lTotalScore))
					{
						wWinCount=i;
						break;
					}

					//删除胜利数据
					for (WORD j=0;j<WinnerList[i].bSameCount;j++)
					{
						WORD wRemoveId=WinnerList[i].wWinerList[j];
						ASSERT(bTempData[wRemoveId][0]!=0);
						ZeroMemory(bTempData[wRemoveId],sizeof(BYTE)*MAX_CENTERCOUNT);
					}
				}
			}catch(...)
			{
				TraceMessage("用户得分顺序");
				ASSERT(FALSE);
			}

			//强退用户
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE && lTotalScore[i]>0l)
				{					
					WinnerList[wWinCount].wWinerList[WinnerList[wWinCount].bSameCount++] = i;
				}
			}

			//得分变量
			LONG lUserScore[GAME_PLAYER];
			ZeroMemory(lUserScore,sizeof(lUserScore));
			//CopyMemory(lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));

			try
			{
				//得分情况
				for (int i=0;i<m_wPlayerCount-1;i++)
				{
					//胜利人数
					int iWinCount = (int)WinnerList[i].bSameCount;
					if(0 == iWinCount)break;

					//胜利用户得分情况
					for(int j=0;j<iWinCount;j++)
					{
						if(0 == lTotalScore[WinnerList[i].wWinerList[j]])continue;

						if(j>0 && lTotalScore[WinnerList[i].wWinerList[j]] - 
							lTotalScore[WinnerList[i].wWinerList[j-1]] == 0)continue;

						//失败用户失分情况
						for(int k=i+1;k<m_wPlayerCount;k++)
						{
							//失败人数
							if(0 == WinnerList[k].bSameCount)break;

							for(int l=0;l<WinnerList[k].bSameCount;l++)
							{
								//用户已赔空
								if(0 == lTotalScore[WinnerList[k].wWinerList[l]])continue;

								WORD wLostId=WinnerList[k].wWinerList[l];
								WORD wWinId=WinnerList[i].wWinerList[j];
								LONG lMinScore = 0;

								//上家得分数目
								LONG lLastScore = ((j>0)?lTotalScore[WinnerList[i].wWinerList[j-1]]:0);
								if(j>0)ASSERT(lLastScore>0L);							
								lMinScore = min(lTotalScore[wWinId]-lLastScore,lTotalScore[wLostId]);

								for(int m=j;m<iWinCount;m++)
								{
									//得分数目
									lUserScore[WinnerList[i].wWinerList[m]]+=lMinScore/(iWinCount-j);
								}

								//赔偿数目
								lUserScore[wLostId]-=lMinScore;
								lTotalScore[wLostId]-=lMinScore;
							}
						}
					}
				}
			}catch(...)
			{
				TraceMessage("得分数目/赔偿数目");
				ASSERT(FALSE);
			}

			//扣税变量
			WORD wRevenue=m_pGameServiceOption->wRevenue;

			//统计用户分数(税收)
			for(WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.lGameScore[i]=lUserScore[i];
				ASSERT(lUserScore[i]+m_lTotalScore[i]>=0L);
				if(GameEnd.lGameScore[i]>0L)
				{
					GameEnd.lGameTax[i]=GameEnd.lGameScore[i]*wRevenue/100L;
					GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
				}
			}

			////统计用户分数
			//for(WORD i=0;i<m_wPlayerCount;i++)
			//{
			//	GameEnd.lGameScore[i]=lUserScore[i];
			//	GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
			//}

			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//积分变量
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_cbPlayStatus[i]==TRUE)
				{
					enScoreKind nScoreKind;
					if(GameEnd.lGameScore[i]==0L)nScoreKind=enScoreKind_Draw;
					else nScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;

					//写入积分
					m_pITableFrame->WriteUserScore(i,GameEnd.lGameScore[i],GameEnd.lGameTax[i],nScoreKind);
				}
			}

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_NO_PLAYER:		//没有玩家
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.cbTotalEnd = 0;

			//效验结果
			WORD wUserCount=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]!=FALSE)wUserCount++;
			}
			if(wUserCount!=1)
			{
				ASSERT(FALSE);
				TraceMessage("没有玩家//效验结果出错");
			}

			//统计分数
			LONG lScore = 0,lRevenue = 0;
			enScoreKind nScoreKind ;
			WORD wWinner = INVALID_CHAIR;
			for (WORD i = 0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)
				{
					if(m_lTotalScore[i] > 0L)GameEnd.lGameScore[i]-=m_lTotalScore[i];
					continue;
				}

				wWinner = i;

				////处理税收
				//for (WORD t=0;t<m_wPlayerCount;t++)
				//{
				//	//赢家不用收税
				//	if(wWinner==t)continue;
				//	if(m_lTotalScore[t]>0L)
				//	{
				//		GameEnd.lGameScore[t]=-m_lTotalScore[t];
				//		m_lTotalScore[t]-=m_bUserTax[t];
				//		GameEnd.lGameTax[t] = m_bUserTax[t];
				//	}
				//}

				//总下注数目
				LONG lAllScore = 0L;
				for (WORD j = 0;j<GAME_PLAYER;j++)
				{
					if(wWinner==j)continue;
					lAllScore += m_lTotalScore[j];
				}
				ASSERT(lAllScore>=0);
				GameEnd.lGameScore[i] = lAllScore;

				//统计税收
				if(GameEnd.lGameScore[i]>0L)
				{
					//扣税变量
					WORD wRevenue=m_pGameServiceOption->wRevenue;
					GameEnd.lGameTax[i]=GameEnd.lGameScore[i]*wRevenue/100L;
					GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
				}

				//构造扑克
				CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

				lScore=GameEnd.lGameScore[i];
				lRevenue= GameEnd.lGameTax[i];
				nScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
			}

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//写入积分
			if(wWinner<GAME_PLAYER)
			{
				m_pITableFrame->WriteUserScore(wWinner,lScore,lRevenue,nScoreKind);
			}
			else TraceMessage("//写入积分ffff");

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//强退处理
			OnUserGiveUp(wChairID);
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
	case GS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//游戏变量
			StatusFree.lCellMinScore=m_pGameServiceOption->lCellScore;
			StatusFree.lCellMaxScore=m_pGameServiceOption->lRestrictScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));	
		}
	case GS_PLAYING:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//标志变量
			//CopyMemory(StatusPlay.bShowHand,m_cbShowHand,sizeof(m_cbShowHand));

			//总下注数目
			LONG lAllScore = 0L;
			for (WORD j = 0;j<GAME_PLAYER;j++)
			{
				lAllScore += m_lTotalScore[j];
				lAllScore -= m_lTableScore[j];
			}
			ASSERT(lAllScore>=0);

			//加注信息
			StatusPlay.lAddLessScore = m_lAddLessScore;
			StatusPlay.lCellScore	 = m_lCellScore;
			StatusPlay.lTurnMaxScore = m_lTurnMaxScore;
			StatusPlay.lTurnLessScore= m_lTurnLessScore;
			StatusPlay.lCenterScore	 = lAllScore;
			StatusPlay.lCellMaxScore = m_pGameServiceOption->lRestrictScore;
			StatusPlay.cbBalanceCount= m_cbBalanceCount;

			//扑克信息
			CopyMemory(StatusPlay.cbHandCardData,m_cbHandCardData[wChiarID],MAX_COUNT);
			if(m_cbBalanceCount>0)
			{
				CopyMemory(StatusPlay.cbCenterCardData,m_cbCenterCardData,sizeof(m_cbCenterCardData));
			}

			//设置变量
			StatusPlay.wDUser = m_wDUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;//当前玩家
			CopyMemory(StatusPlay.lTableScore,m_lTableScore,sizeof(m_lTableScore));//下注数目
			CopyMemory(StatusPlay.lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));//下注数目
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));//用户游戏状态

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}
	//效验结果
	ASSERT(FALSE);
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
	switch(wSubCmdID)
	{
	case SUB_C_GIVE_UP:		//用户放弃
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserGiveUp(pUserData->wChairID);
		}
	case SUB_C_ADD_SCORE:	//用户加注	
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pDataBuffer;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserAddScore(pUserData->wChairID,pAddScore->lScore,false);
		}
	case SUB_C_OPEN_CARD:	//用户坐下
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();

			//状态判断
			//ASSERT(m_cbPlayStatus[pUserData->wChairID]!=TRUE);
			//if (m_cbPlayStatus[pUserData->wChairID]!=FALSE) return false;

			//发送数据
			CMD_S_OpenCard OpenCard;
			OpenCard.wWinUser = pUserData->wChairID;			
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));	

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

//放弃事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID)
{
	//重置状态
	m_cbPlayStatus[wChairID]=FALSE;
	m_cbShowHand[wChairID]=FALSE;

	//发送消息
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser=wChairID;
	GiveUp.lLost = -m_lTotalScore[wChairID];
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

	//写入积分
	m_pITableFrame->WriteUserScore(wChairID,-m_lTotalScore[wChairID],0,enScoreKind_Lost);

	//清空下注
	m_lTableScore[wChairID] = 0L;

	//人数统计
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//判断结束
	if (wPlayerCount>=2)
	{
		if (m_wCurrentUser==wChairID) OnUserAddScore(wChairID,0L,true);
	}
	else OnEventGameEnd(INVALID_CHAIR,NULL,GER_NO_PLAYER);

	return true;
}

//加注事件 
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONG lScore, bool bGiveUp)
{
	//校验用户
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return false; 

	//校验金币
	ASSERT((lScore +m_lTotalScore[wChairID])<= m_lUserMaxScore[wChairID]);	
	if ((lScore+m_lTotalScore[wChairID])>m_lUserMaxScore[wChairID]) return false;
	ASSERT(lScore>=0L);
	if ((lScore<0)) return false;

	//累计金币
	m_lTableScore[wChairID] += lScore;
	m_lTotalScore[wChairID] += lScore;

	//平衡下注
	if(m_lTableScore[wChairID] > m_lBalanceScore )
	{
		m_lBalanceScore = m_lTableScore[wChairID];
	}

	//梭哈判断
	if(m_lTotalScore[wChairID]==m_lUserMaxScore[wChairID])
	{
		m_cbShowHand[wChairID] = TRUE;
	}

	//用户切换
	WORD wNextPlayer=INVALID_CHAIR;
	for (WORD i=1;i<m_wPlayerCount;i++)
	{
		//设置变量
		m_wOperaCount++;
		wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

		//继续判断
		if ((m_cbPlayStatus[wNextPlayer]==TRUE) &&(m_cbShowHand[wNextPlayer] == FALSE)) break;
	}
	ASSERT(wNextPlayer < m_wPlayerCount);

	//完成判断
	bool bFinishTurn=false;
	if (m_wOperaCount>=m_wPlayerCount)
	{
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//过滤未平衡 和未梭哈用户
			if ((m_cbPlayStatus[i]==TRUE)&&(m_lTableScore[i]<m_lBalanceScore)&&(m_cbShowHand[i]==FALSE)) 
				break;
		}
		if (i==m_wPlayerCount) 
			bFinishTurn=true;
	}

	//A家show190,B放弃,C还选择?
	if(!bFinishTurn)
	{
		WORD wPlayCount = 0,wShowCount = 0;
		for (BYTE i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==TRUE)
			{
				if(m_cbShowHand[i]==TRUE)
				{
					wShowCount++;
				}
				wPlayCount++;
			}
		}
		if(wPlayCount-1==wShowCount && m_lTableScore[wNextPlayer]>=m_lBalanceScore) bFinishTurn=true;
	}

	//继续加注
	if (!bFinishTurn)
	{
		//当前用户
		m_wCurrentUser=wNextPlayer;

		//最小值为平衡下注 -桌面下注  和 剩余金币中取小 可能梭哈
		m_lTurnLessScore = min(m_lBalanceScore - m_lTableScore[m_wCurrentUser],m_lUserMaxScore[m_wCurrentUser] - m_lTotalScore[m_wCurrentUser]);
		m_lTurnMaxScore = m_lUserMaxScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];
		if(m_lTotalScore[m_wCurrentUser]==m_lCellScore)
		{
			LONG bTemp = (m_lBalanceScore==m_lCellScore*2)?(m_lCellScore*2):((m_lBalanceScore-m_lCellScore*2)*2);
			m_lAddLessScore = m_lCellScore+bTemp;
		}
		else m_lAddLessScore = (m_lBalanceScore==0)?(2*m_lCellScore):(__max((m_lBalanceScore-m_lTableScore[m_wCurrentUser])*2,2L*m_lCellScore));

		//构造数据
		CMD_S_AddScore AddScore;
		ZeroMemory(&AddScore,sizeof(AddScore));
		AddScore.lAddScoreCount=lScore;
		AddScore.wAddScoreUser=wChairID;
		AddScore.wCurrentUser=m_wCurrentUser;
		AddScore.lTurnLessScore=m_lTurnLessScore;
		AddScore.lTurnMaxScore = m_lTurnMaxScore;
		AddScore.lAddLessScore = m_lAddLessScore;

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

		return true;
	}

	//平衡次数
	m_cbBalanceCount++;
	m_wOperaCount=0;

	//第1次下注平衡后就开始发给三张公牌
	//第2次下注平衡后就开始发第四张公牌
	//第3次下注平衡后就开始发第五张公牌
	//第4次下注平衡后就结束游戏 

	//D家下注
	WORD wDUser=m_wDUser;
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		wDUser=(m_wDUser+i)%GAME_PLAYER;		
		if(m_cbPlayStatus[wDUser]==TRUE && m_cbShowHand[wDUser]==FALSE) break;
	}

	//重值变量
	m_lBalanceScore = 0L;
	m_lTurnLessScore = 0L;
	m_lTurnMaxScore = m_lUserMaxScore[wDUser]-m_lTotalScore[wDUser];
	m_lAddLessScore = 2*m_lCellScore;

	//构造数据
	CMD_S_AddScore AddScore;
	ZeroMemory(&AddScore,sizeof(AddScore));
	AddScore.wAddScoreUser=wChairID;
	AddScore.wCurrentUser=INVALID_CHAIR;
	AddScore.lAddScoreCount=lScore;
	AddScore.lTurnLessScore=m_lTurnLessScore;
	AddScore.lTurnMaxScore = m_lTurnMaxScore;
	AddScore.lAddLessScore = m_lAddLessScore;

	//清理数据
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	m_lBalanceScore = 0L;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

	//结束判断
	if (m_cbBalanceCount == 4) 
	{
		OnEventGameEnd(INVALID_CHAIR,NULL,GER_NORMAL);
		return true;
	}

	//梭哈用户统计
	WORD wShowHandCount=0,wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbShowHand[i]==TRUE)	wShowHandCount++;
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//只剩一玩家没梭或者全梭
	if((wShowHandCount >= wPlayerCount -1) && m_cbBalanceCount < 4)
	{
		//构造数据
		CMD_S_SendCard SendCard;
		ZeroMemory(&SendCard,sizeof(SendCard));
		SendCard.cbPublic = m_cbBalanceCount;
		SendCard.wCurrentUser = INVALID_CHAIR;
		SendCard.cbSendCardCount = MAX_CENTERCOUNT;
		CopyMemory(SendCard.cbCenterCardData,m_cbCenterCardData,sizeof(BYTE)*MAX_CENTERCOUNT);

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

		//结束游戏
		OnEventGameEnd(INVALID_CHAIR,NULL,GER_NORMAL);

		return true;
	}

	//盲注玩家
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//临时变量
		BYTE cbNextUser =(m_wDUser+i)%GAME_PLAYER;

		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(cbNextUser);

		//无效用户 梭哈用户过滤
		if (pIServerUserItem==NULL||m_cbPlayStatus[cbNextUser] == FALSE||m_cbShowHand[cbNextUser] == 1) 
			continue;

		m_wCurrentUser = cbNextUser;
		break;
	}

	//构造数据
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard,sizeof(SendCard));
	SendCard.cbPublic = 0;
	SendCard.wCurrentUser = m_wCurrentUser;
	SendCard.cbSendCardCount = 3 +(m_cbBalanceCount-1);
	CopyMemory(SendCard.cbCenterCardData,m_cbCenterCardData,sizeof(BYTE)*(SendCard.cbSendCardCount));

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	return true;
}

//输出信息
void TraceMessage(LPCTSTR pszMessage)
{
	CFile File;
	if ((File.Open(TEXT("TraceData.txt"),CFile::modeWrite)==FALSE)&&
		(File.Open(TEXT("TraceData.txt"),CFile::modeWrite|CFile::modeCreate)==FALSE))
	{
		ASSERT(FALSE);
		return;
	}

	File.SeekToEnd();
	File.Write(pszMessage,lstrlen(pszMessage));
	File.Write(TEXT("\r\n"),lstrlen(TEXT("\r\n")));

	File.Flush();
	File.Close();

	return;
}
//////////////////////////////////////////////////////////////////////////
