#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

#define IDI_GAME_END				1										//结束标识

//////////////////////////////////////////////////////////////////////////

//静态变量
__int64				CTableFrameSink::g_lMaxScore=100000000L;				//最大变化
__int64				CTableFrameSink::g_lMaxWinScore=100000000L;				//最大赢分
__int64				CTableFrameSink::g_lMaxLoseScore=-100000000L;			//最大输分

//用户索引
CBlacklistIDMap		CTableFrameSink::g_BlacklistIDMap;						//限制用户
CScoreHistoryMap	CTableFrameSink::g_ScoreHistoryMap;						//历史记录

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_AllReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wOperaCount=0;
	m_bSendCardCount=0;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_cbCardCount,0,sizeof(m_cbCardCount));
	memset(m_cbHandCardData,0,sizeof(m_cbHandCardData));
	m_cbGameEndReason = GER_NORMAL;

	//金币信息
	m_lMaxGold=0;
	m_lTurnBasicGold=0;
	m_lBasicGold = 0;
	memset(m_lTableGold,0,sizeof(m_lTableGold));

	//辅助变量
	memset(m_cbPlayStatus,0,sizeof(m_cbPlayStatus));

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
	//状态变量
	m_wOperaCount=0;
	m_bSendCardCount=0;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_cbPlayStatus,0,sizeof(m_cbPlayStatus));
	memset(m_cbHandCardData,0,sizeof(m_cbHandCardData));
	memset(m_cbCardCount,0,sizeof(m_cbCardCount));
	m_cbGameEndReason = GER_NORMAL;

	//金币变量
	m_lMaxGold=0L;
	m_lTurnBasicGold=0L;
	m_lBasicGold = 0;
	memset(m_lTableGold,0,sizeof(m_lTableGold));

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
	return m_cbPlayStatus[wChairID]?true:false;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//状态变量
	m_wOperaCount=0;
	m_bSendCardCount=0;
	memset(m_cbPlayStatus,0,sizeof(m_cbPlayStatus));
	memset(m_cbCardCount,0,sizeof(m_cbCardCount));

	//金币变量
	m_lMaxGold=0L;
	m_lTurnBasicGold=0L;
	memset(m_lTableGold,0,sizeof(m_lTableGold));

	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	//变量定义
	const tagUserScore * pUserScore=NULL;
	IServerUserItem * pIServerUserItem=NULL;

	//计算下注
	bool bCalculateGold=false;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
		if (pIServerUserItem!=NULL)
		{
			m_cbPlayStatus[i]=true;
			if (bCalculateGold==false)
			{
				bCalculateGold=true;
				pUserScore=pIServerUserItem->GetUserScore();
				ASSERT(pUserScore->lScore>m_pGameServiceOption->lCellScore);
				m_lMaxGold=pUserScore->lScore;
			}
			else 
			{
				pUserScore=pIServerUserItem->GetUserScore();
				ASSERT(pUserScore->lScore>1L);
				m_lMaxGold=__min(m_lMaxGold,pUserScore->lScore);
			}
		}
	}
	m_lBasicGold = __max(m_lMaxGold/100L,m_pGameServiceOption->lCellScore);

	for( WORD i = 0; i < m_wPlayerCount; i++ )
	{
		if( m_cbPlayStatus[i] )
		{
			m_lTableGold[i*2+1]=m_lBasicGold;
		}
	}

	//分发扑克
	m_bSendCardCount=2;
	m_GameLogic.RandCardList(m_cbHandCardData[0],sizeof(m_cbHandCardData)/sizeof(m_cbHandCardData[0][0]));
	for (WORD i=0;i<m_wPlayerCount;i++) 
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			m_cbCardCount[i]=m_bSendCardCount;
		}
	}

	//限制处理
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			//变量定义
			bool bChangeCard=false;

			//获取用户
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//积分判断
			tagScoreHistory * pScoreHistory=NULL;
			if ((g_ScoreHistoryMap.Lookup(pIServerUserItem->GetUserID(),pScoreHistory)==TRUE)&&(pScoreHistory!=NULL))
			{
				if ((g_lMaxScore!=0L)&&(pScoreHistory->lScore>g_lMaxScore)) bChangeCard=true;
				if ((g_lMaxWinScore!=0L)&&(pScoreHistory->lWinScore>g_lMaxWinScore)) bChangeCard=true;
				if ((g_lMaxLoseScore!=0L)&&(pScoreHistory->lLoseScore<g_lMaxLoseScore)) bChangeCard=true;

			}

			//限制用户
			DWORD dwUserID=0L;
			if ((g_BlacklistIDMap.Lookup(pIServerUserItem->GetUserID(),dwUserID)==TRUE)&&(dwUserID!=0L)) bChangeCard=true;

			//交换扑克
			if (bChangeCard==true)
			{
				//获取输牌
				WORD wLoseIndex=EstimateLoser(0,4);

				//交换扑克
				if (wLoseIndex!=i)
				{
					BYTE cbTempCardData[5];
					CopyMemory(cbTempCardData,m_cbHandCardData[i],5*sizeof(BYTE));
					CopyMemory(m_cbHandCardData[i],m_cbHandCardData[wLoseIndex],5*sizeof(BYTE));
					CopyMemory(m_cbHandCardData[wLoseIndex],cbTempCardData,5*sizeof(BYTE));
				}

				break;
			}
		}
	}

	//设置用户
	m_wCurrentUser=EstimateWinner(1,1);

	//发送开始
	CMD_S_GameStart GameStart;
	memset(&GameStart,0,sizeof(GameStart));
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.lMaxGold=m_lMaxGold;
	GameStart.lTurnMaxGold=m_lMaxGold/4L;
	GameStart.lTurnBasicGold=m_lTurnBasicGold;
	GameStart.lBasicGold = m_lBasicGold;
	for (WORD i=0;i<m_wPlayerCount;i++) GameStart.bCardData[i]=m_cbHandCardData[i][1];
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		GameStart.bFundusCard=0;
		if (m_cbPlayStatus[i]==TRUE)
		{
			GameStart.bFundusCard=m_cbHandCardData[i][0];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
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
	case GER_DISMISS:		//游戏解散
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//构造数据
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_NORMAL:		//常规结束
	case GER_NO_PLAYER:		//没有玩家
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//计算总注
			LONG lAllGold=0L;
			for (WORD i=0;i<CountArray(m_lTableGold);i++) lAllGold+=m_lTableGold[i];

			//统计信息
			WORD wWinerUser=EstimateWinner(0,4);
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				LONG lScore=0;
				LONG lRevenue=0;
				enScoreKind ScoreKind;

				//底牌信息
				if ((cbReason==GER_NO_PLAYER)||(m_cbPlayStatus[i]==FALSE)) GameEnd.bUserCard[i]=0;
				else GameEnd.bUserCard[i]=m_cbHandCardData[i][0];

				//积分信息
				if (i==wWinerUser)
				{
					lScore=lAllGold-m_lTableGold[i*2]-m_lTableGold[i*2+1];
					GameEnd.lGameGold[i]=lAllGold-m_lTableGold[i*2]-m_lTableGold[i*2+1];
				}
				else
				{
					lScore=-m_lTableGold[i*2]-m_lTableGold[i*2+1];
					GameEnd.lGameGold[i]=-m_lTableGold[i*2]-m_lTableGold[i*2+1];
				}

				//胜利类型
				ScoreKind=(GameEnd.lGameGold[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
				//计算税收
				if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
				{
					//扣税变量
					BYTE cbRevenue=(BYTE)m_pGameServiceOption->wRevenue;

					if (GameEnd.lGameGold[i]>=100L)
					{
						//计算税收
						lRevenue=GameEnd.lGameGold[i]*cbRevenue/100L;

						//成绩变量
						GameEnd.lTax+=lRevenue;
						GameEnd.lGameGold[i]=GameEnd.lGameGold[i]-lRevenue;

						//写分变量
						lRevenue=lRevenue;
						lScore=lScore-lRevenue;
					}
				}
				//写入积分
				if (m_cbPlayStatus[i]==TRUE)
				{
					m_pITableFrame->WriteUserScore(i,lScore,lRevenue,ScoreKind);
				}

				//成绩记录
				if (m_cbPlayStatus[i]==TRUE)
				{
					//获取用户
					IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
					if (pIServerUserItem==NULL) continue;
 
					//搜索用户
					tagScoreHistory * pScoreHistory=NULL;
					g_ScoreHistoryMap.Lookup(pIServerUserItem->GetUserID(),pScoreHistory);

					//获取时间
					SYSTEMTIME SystemTime;
					GetLocalTime(&SystemTime);

					//增加用户
					if (pScoreHistory==NULL)
					{
						//创建对象
						pScoreHistory=new tagScoreHistory;
						g_ScoreHistoryMap[pIServerUserItem->GetUserID()]=pScoreHistory;

						//设置变量
						pScoreHistory->lScore=0L;
						pScoreHistory->lWinScore=0L;
						pScoreHistory->lLoseScore=0L;
						pScoreHistory->RecordTime=SystemTime;
					}

					//还原判断
					if (pScoreHistory->RecordTime.wDay!=SystemTime.wDay)
					{
						pScoreHistory->lScore=0L;
						pScoreHistory->lWinScore=0L;
						pScoreHistory->lLoseScore=0L;
						pScoreHistory->RecordTime=SystemTime;
					}

					//记录积分
					pScoreHistory->lScore+=lScore;
					if (lScore>0L) pScoreHistory->lWinScore+=lScore;
					if (lScore<0L) pScoreHistory->lLoseScore+=lScore;
				}
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

			//强退处理
			OnUserGiveUp(wChairID);

			return true;
		}
	}

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret)
{
	switch (bGameStatus)
	{
	case GS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));
			StatusFree.dwBasicGold=0;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_PLAYING:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lMaxGold=m_lMaxGold;
			StatusPlay.lTurnBasicGold=m_lTurnBasicGold;
			StatusPlay.lBasicGold=m_lBasicGold;
			CopyMemory(StatusPlay.bPlayStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));
			CopyMemory(StatusPlay.lTableGold,m_lTableGold,sizeof(m_lTableGold));

			//设置下注
			if (m_bSendCardCount<=2) StatusPlay.lTurnMaxGold=m_lMaxGold/4L;
			else if (m_bSendCardCount==3) StatusPlay.lTurnMaxGold=m_lMaxGold/2L;
			else StatusPlay.lTurnMaxGold=m_lMaxGold;

			//设置扑克
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayStatus[i]==TRUE)
				{
					ASSERT(m_pITableFrame->GetServerUserItem(i)!=NULL);
					StatusPlay.bTableCardCount[i]=m_cbCardCount[i];
					if (i==wChiarID) StatusPlay.bTableCardArray[i][0]=m_cbHandCardData[i][0];
					CopyMemory(&StatusPlay.bTableCardArray[i][1],&m_cbHandCardData[i][1],(m_cbCardCount[i]-1)*sizeof(BYTE));
				}
			}

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
	switch (wTimerID)
	{
	case IDI_GAME_END:	//游戏结束
		{
			if (m_pITableFrame->GetGameStatus()==GS_PLAYING)
			{
				OnEventGameEnd(INVALID_CHAIR,NULL,m_cbGameEndReason);
				return true;
			}
		}
	}
	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_ADD_GOLD:	//用户加注
		{
			//效验数据
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return true;

			//消息处理
			CMD_C_AddGold * pAddGold=(CMD_C_AddGold *)pDataBuffer;
			OnUserAddGold(pUserData->wChairID,pAddGold->lGold,false);

			return true;
		}
	case SUB_C_GIVE_UP:		//用户放弃
		{
			//效验数据
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return true;

			//消息处理
			OnUserGiveUp(pUserData->wChairID);

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
	//状态判断
	ASSERT(m_cbPlayStatus[wChairID]==TRUE);
	if (m_cbPlayStatus[wChairID]==FALSE) return false;

	//设置数据
	m_cbPlayStatus[wChairID]=FALSE;
	m_cbCardCount[wChairID]=0;

	//发送放弃消息
	CMD_S_GiveUp GiveUp;
	GiveUp.wUserChairID=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

	//变量定义
	LONG lScore=0;
	LONG lRevenue=0;
	enScoreKind ScoreKind;

	//设置变量
	ScoreKind=enScoreKind_Lost;
	lScore=-m_lTableGold[wChairID*2]-m_lTableGold[wChairID*2+1];

	//用户写分
	m_pITableFrame->WriteUserScore(wChairID,lScore,lRevenue,ScoreKind);

	//判断结束
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}
	if (wPlayerCount>=2)
	{
		if (m_wCurrentUser==wChairID) OnUserAddGold(wChairID,0L,true);
	}
	else OnEventGameEnd(INVALID_CHAIR,NULL,GER_NO_PLAYER);

	return true;
}

//加注事件
bool CTableFrameSink::OnUserAddGold(WORD wChairID, LONG lGold, bool bCancel)
{
	bool bShowHand = false;
	//设置数据
	if (bCancel==false)
	{
		//效验数据
		ASSERT(m_wCurrentUser==wChairID);
		if (m_wCurrentUser!=wChairID) return false;

		//计算限注
		LONG lMaxGold=0L;
		if (m_bSendCardCount<=2) lMaxGold=m_lMaxGold/4L;
		else lMaxGold=m_lMaxGold;
		
		//计算剩下的金币
		LONG lLeaveGold=lMaxGold-m_lTableGold[wChairID*2+1];
		m_lTableGold[wChairID*2]=__min(lLeaveGold,__max(lGold,m_lTurnBasicGold));

		if( m_lTableGold[wChairID*2]+m_lTableGold[wChairID*2+1] >= m_lMaxGold ) bShowHand = true;
	}

	//操作次数
	bool bFinishTurn=false;
	WORD wNextUser=INVALID_CHAIR;
	for (WORD i=0;i<m_wPlayerCount-1;i++)
	{
		m_wOperaCount++;
		wNextUser=(m_wCurrentUser+i+1)%m_wPlayerCount;
		if (m_cbPlayStatus[wNextUser]==TRUE) break;
	}
	
	//判断下注
	if (m_wOperaCount>=m_wPlayerCount)
	{
		LONG lTableGold=0L;
		WORD i;
		for (i=0;i<m_wPlayerCount;i++)
		{
			if (m_cbPlayStatus[i]==TRUE)
			{
				lTableGold=m_lTableGold[i*2];
				break;
			}
		}
		while (i<m_wPlayerCount)
		{
			if ((m_cbPlayStatus[i]==TRUE)&&(m_lTableGold[i*2]!=lTableGold)) break;
			i++;
		}
		if (i==m_wPlayerCount) bFinishTurn=true;
	}

	//一轮判断
	if (bFinishTurn==true)
	{
		//发送数据
		CMD_S_AddGold GoldResult;
		GoldResult.lCurrentLessGold=0L;
		GoldResult.wLastChairID=wChairID;
		GoldResult.wCurrentUser=INVALID_CHAIR;
		GoldResult.lLastAddGold=m_lTableGold[wChairID*2];
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_GOLD,&GoldResult,sizeof(GoldResult));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_GOLD,&GoldResult,sizeof(GoldResult));

		//累计金币
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			m_lTableGold[i*2+1]+=m_lTableGold[i*2];
			m_lTableGold[i*2]=0L;
		}

		//开始一轮
		if (m_bSendCardCount<5) 
		{
			//设置变量
			m_wOperaCount=0;
			m_lTurnBasicGold=0L;
			WORD wLastMostUser = EstimateWinner(1,m_bSendCardCount-1);

			BYTE bySourceCount = m_bSendCardCount;
			if( bShowHand ) m_bSendCardCount = MAX_COUNT;
			else m_bSendCardCount++;

			//发送扑克
			CMD_R_SendCard SendCard;
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				if (m_cbPlayStatus[i]==FALSE) continue;

				m_cbCardCount[i]=m_bSendCardCount;

				//派发扑克
				for (BYTE j=0;j<(m_bSendCardCount-bySourceCount);j++)
				{
					ASSERT(j<CountArray(SendCard.bUserCard[i]));
					SendCard.bUserCard[i][j]=m_cbHandCardData[i][bySourceCount+j];
				}
			}

			m_wCurrentUser=EstimateWinner(1,m_bSendCardCount-1);

			//发送消息
			SendCard.wCurrentUser=(bShowHand?INVALID_CHAIR:m_wCurrentUser);
			SendCard.wStartChairId = wLastMostUser;
			SendCard.cbSendCardCount = m_bSendCardCount-bySourceCount;
			SendCard.lMaxGold=(m_bSendCardCount>=3)?m_lMaxGold:m_lMaxGold/2L;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

			//结束游戏
			if ( m_bSendCardCount == 5 && m_lMaxGold <= m_lTableGold[wChairID*2+1] ) 
			{
				m_cbGameEndReason = GER_NORMAL;
				m_pITableFrame->SetGameTimer(IDI_GAME_END,2000,1,0);
			}
		}
		else 
		{
			m_cbGameEndReason = GER_NORMAL;
			m_pITableFrame->SetGameTimer(IDI_GAME_END,2000,1,0);
		}
	}
	else
	{
		//设置变量
		m_wCurrentUser=wNextUser;
		m_lTurnBasicGold=__max(m_lTableGold[wChairID*2],m_lTurnBasicGold);

		//发送数据
		CMD_S_AddGold AddGold;
		AddGold.wLastChairID=wChairID;
		AddGold.wCurrentUser=m_wCurrentUser;
		AddGold.lCurrentLessGold=m_lTurnBasicGold;
		AddGold.lLastAddGold=m_lTableGold[wChairID*2];
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_GOLD,&AddGold,sizeof(AddGold));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_GOLD,&AddGold,sizeof(AddGold));
	}

	return true;
}

bool CTableFrameSink::OnUserGetWinner(IServerUserItem * pIServerUserItem)
{
	//获取胜者
	IServerUserItem * pIWinnerUserItem=m_pITableFrame->GetServerUserItem(EstimateWinner(0,4));
	if (pIWinnerUserItem==NULL) return true;

	//构造消息
	TCHAR szMessage[128];
	_snprintf(szMessage,CountArray(szMessage),TEXT("本局的胜利玩家是：%s"),pIWinnerUserItem->GetAccounts());

	//发送消息
	m_pITableFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);

	return true;
}

//推断输者
WORD CTableFrameSink::EstimateLoser(BYTE cbStartPos, BYTE cbConcludePos)
{
	//保存扑克
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

	//寻找玩家
	WORD wLoser;
	for (wLoser=0;wLoser<GAME_PLAYER;wLoser++)
	{
		if (m_cbPlayStatus[wLoser]==TRUE) 
		{
			m_GameLogic.SortCardList(cbUserCardData[wLoser]+cbStartPos,cbConcludePos-cbStartPos+1);
			break;
		}
	}

	//对比玩家
	for (WORD i=(wLoser+1);i<GAME_PLAYER;i++)
	{
		//用户过滤
		if (m_cbPlayStatus[i]==FALSE) continue;

		//排列扑克
		m_GameLogic.SortCardList(cbUserCardData[i]+cbStartPos,cbConcludePos-cbStartPos+1);

		//对比扑克
		if (m_GameLogic.CompareCard(cbUserCardData[i]+cbStartPos,cbUserCardData[wLoser]+cbStartPos,cbConcludePos-cbStartPos+1)==false) 
		{
			wLoser=i;
		}
	}

	return wLoser;
}

//推断胜者
WORD CTableFrameSink::EstimateWinner(BYTE cbStartPos, BYTE cbConcludePos)
{
	//保存扑克
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

	//寻找玩家
	WORD wWinner;
	for (wWinner=0;wWinner<GAME_PLAYER;wWinner++)
	{
		if (m_cbPlayStatus[wWinner]==TRUE) 
		{
			m_GameLogic.SortCardList(cbUserCardData[wWinner]+cbStartPos,cbConcludePos-cbStartPos+1);
			break;
		}
	}

	//对比玩家
	for (WORD i=(wWinner+1);i<GAME_PLAYER;i++)
	{
		//用户过滤
		if (m_cbPlayStatus[i]==FALSE) continue;

		//排列扑克
		m_GameLogic.SortCardList(cbUserCardData[i]+cbStartPos,cbConcludePos-cbStartPos+1);

		//对比扑克
		if (m_GameLogic.CompareCard(cbUserCardData[i]+cbStartPos,cbUserCardData[wWinner]+cbStartPos,cbConcludePos-cbStartPos+1)==true) 
		{
			wWinner=i;
		}
	}

	return wWinner;
}

//////////////////////////////////////////////////////////////////////////
