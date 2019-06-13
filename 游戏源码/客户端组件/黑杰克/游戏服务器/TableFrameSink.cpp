#include "StdAfx.h"
#include "TableFrameSink.h"
#include ".\tableframesink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const enStartMode	CTableFrameSink::m_GameStartMode = enStartMode_AllReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{	
	//游戏变量							
	m_wCurrentUser   = INVALID_CHAIR;	
	m_wPlayerCount   = 0;		

	//游戏庄家
	m_nBankerStation = -1;
	m_lBankerScore   = 0L;
	m_bBankerWin     = false;
	m_bBankerGetCard = false;

	//标识加注
	m_bAddGoldEnd    = false;	
	memset(m_bAddGold,0,sizeof(m_bAddGold));
	memset(m_lUserAddGold,0,sizeof(m_lUserAddGold));

	//金币信息
	m_lMaxGold       = 0L;
	m_lTurnBasicGold = 1L;
	memset(m_lTableGold,0,sizeof(m_lTableGold));
	
	//分牌相关
	memset(m_cbCutCard,0,sizeof(m_cbCutCard));
	memset(m_cbTableCutCardCount,0,sizeof(m_cbTableCutCardCount));
	memset(m_cbTableCutCardArray,0,sizeof(m_cbTableCutCardArray));

	//初始扑克	
  	memset(m_cbTableCardCount,0,sizeof(m_cbTableCardCount));	
	memset(m_cbTableCardArray,0,sizeof(m_cbTableCardArray));  
    
	//发牌变量
	memset(m_cbRandCardArray,0,sizeof(m_cbRandCardArray)); 
	m_cbSendCardCount = 0;

	//玩家状态		
	memset(m_cbCardStatus,0,sizeof(m_cbCardStatus));
	memset(m_cbCutCardStatus,0,sizeof(m_cbCutCardStatus));	
	
	//组件变量
	m_pITableFrame     = NULL;
	pGameServiceOption = NULL;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void * __cdecl CTableFrameSink::QueryInterface(const IID & Guid,
											   DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
	return NULL;
}

//初始化
bool __cdecl CTableFrameSink::InitTableFrameSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx != NULL);

	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
	
	if (m_pITableFrame == NULL)
	{
		return false;
	}

	//获取参数
	pGameServiceOption = m_pITableFrame->GetGameServiceOption();

	ASSERT(pGameServiceOption != NULL);

	m_pITableFrame->SetGameStatus(GS_FREE);

	return true;
}

//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	//游戏变量							
	m_wCurrentUser   = INVALID_CHAIR;	
	m_wPlayerCount   = 0;		

	//游戏庄家
	m_lBankerScore   = 0L;
	m_bBankerGetCard = false;

	//标识加注
	m_bAddGoldEnd    = false;	
	memset(m_bAddGold,0,sizeof(m_bAddGold));
	memset(m_lUserAddGold,0,sizeof(m_lUserAddGold));	

	//金币信息
	m_lMaxGold       = 0L;
	m_lTurnBasicGold = 1L;
	memset(m_lTableGold,0,sizeof(m_lTableGold));

	//分牌相关
	memset(m_cbCutCard,0,sizeof(m_cbCutCard));
	memset(m_cbTableCutCardCount,0,sizeof(m_cbTableCutCardCount));
	memset(m_cbTableCutCardArray,0,sizeof(m_cbTableCutCardArray));

	//初始扑克	
  	memset(m_cbTableCardCount,0,sizeof(m_cbTableCardCount));	
	memset(m_cbTableCardArray,0,sizeof(m_cbTableCardArray));
	memset(m_cbRandCardArray,0,sizeof(m_cbRandCardArray));  
    
	//发牌变量
	memset(m_cbRandCardArray,0,sizeof(m_cbRandCardArray)); 
	m_cbSendCardCount = 0;

	//玩家状态		
	memset(m_cbCardStatus,0,sizeof(m_cbCardStatus));
	memset(m_cbCutCardStatus,0,sizeof(m_cbCutCardStatus));	

	//设置状态
	m_pITableFrame->SetGameStatus(GS_FREE);

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
	return  true;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, 
											IServerUserItem * pIServerUserItem, 
											BYTE bGameStatus, 
											bool bSendSecret)
{
	bool bWatchUser;

	const tagServerUserData *pUserData = pIServerUserItem->GetUserData();
	
	if (pUserData->cbUserStatus != US_LOOKON)
		bWatchUser = false; 
	
	switch (bGameStatus)
	{
	case GS_FREE:
		{			
			//构造数据
			CMD_S_StatusFree StatusFree;

			memset(&StatusFree, 0, sizeof(StatusFree));

			StatusFree.dwBasicGold = pGameServiceOption->lCellScore;

			//发送场景
			return 	m_pITableFrame->SendGameScene(pIServerUserItem,
			                                      &StatusFree,
												  sizeof(StatusFree));

	
		}
	case GS_ADDGOLDING:
		{
			//构造数据
			CMD_S_StatusAddGold StatusAddGold;
			memset(&StatusAddGold,0,sizeof(StatusAddGold));

			StatusAddGold.dwBasicGold    = pGameServiceOption->lCellScore;
			StatusAddGold.lMaxGold       = m_lMaxGold;		
			StatusAddGold.wBankerStation = m_nBankerStation;
			StatusAddGold.wCurrentUser   = wChiarID;
			CopyMemory(StatusAddGold.bAddGoldEnd,m_bAddGold,sizeof(m_bAddGold));
			CopyMemory(StatusAddGold.lTableGold,m_lTableGold,sizeof(m_lTableGold));
		
			//发送场景
			return 	m_pITableFrame->SendGameScene(pIServerUserItem,
			                                      &StatusAddGold,
												  sizeof(StatusAddGold));
		}

	case GS_PLAYING:
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			StatusPlay.wCurrentUser   = m_wCurrentUser;
			StatusPlay.wBankerStation = m_nBankerStation;
			StatusPlay.bBankerGetCard = m_bBankerGetCard;
			StatusPlay.lMaxGold       = m_lMaxGold;				
			StatusPlay.lBasicGold     = pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.cbPlayStatus,m_cbCardStatus,sizeof(m_cbCardStatus));
			CopyMemory(StatusPlay.lTableGold,m_lTableGold,sizeof(m_lTableGold));

			//设置扑克
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (m_cbCardStatus[i] != NO_CARD)
				{
					ASSERT(m_pITableFrame->GetServerUserItem(i) != NULL);
					StatusPlay.cbTableCardCount[i] = m_cbTableCardCount[i];
					if ((i == wChiarID) && (bSendSecret == true)) 
					{
						StatusPlay.cbTableCardArray[i][0] = m_cbTableCardArray[i][0];
					}				
				
					CopyMemory(&StatusPlay.cbTableCardArray[i][1],&m_cbTableCardArray[i][1],(m_cbTableCardCount[i]-1)*sizeof(BYTE));
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
	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, 
											const void * pDataBuffer, 
											WORD wDataSize, 
											IServerUserItem * pIServerUserItem)
{
	bool bWatchUser = false;

	const tagServerUserData *pUserData = pIServerUserItem->GetUserData();

	if(pUserData->cbUserStatus == US_LOOKON)
	{
		bWatchUser = true;
	}

	WORD wChairID = pIServerUserItem->GetChairID();

	switch (wSubCmdID)
	{
	case SUB_C_ADD_GOLD:	//用户加注
		{
			//校验数据
			tagServerUserData * pUserData = pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus != US_PLAY) return true;
			ASSERT(m_cbCardStatus[pUserData->wChairID] == INIT_CARD);
			if (m_cbCardStatus[pUserData->wChairID] != INIT_CARD) return true;

			//获取数据
			CMD_C_AddGold * pAddGold = (CMD_C_AddGold *)pDataBuffer;
		
			//设置信息
			CMD_S_AddGold  AddGoldResult;
			AddGoldResult.wCurrentUser = pUserData->wChairID;

			//校验数据
			ASSERT(pAddGold->lGold <= m_lMaxGold);
			if(pAddGold->lGold > m_lMaxGold) return false;

			//记录初始闲家下注
			if (!pAddGold->bDoubleGold)
			{
				m_lNotBankerScore[wChairID]=pAddGold->lGold;
			}

			//双倍处理
			if(pAddGold->bDoubleGold)
			{				
				AddGoldResult.lLastAddGold = 2 * m_lUserAddGold[pUserData->wChairID];
				AddGoldResult.bDoubleGold  = true;

				m_lUserAddGold[pUserData->wChairID] = 2 * m_lUserAddGold[pUserData->wChairID];	
			}
			else
			{
				AddGoldResult.lLastAddGold = pAddGold->lGold;
				AddGoldResult.bDoubleGold  = false;

				m_bAddGold[pUserData->wChairID]     = true;	
				m_lUserAddGold[pUserData->wChairID] = pAddGold->lGold;
			}	

			m_lTableGold[pUserData->wChairID * 2]     = pGameServiceOption->lCellScore;
			m_lTableGold[pUserData->wChairID * 2 + 1] = m_lUserAddGold[pUserData->wChairID] -pGameServiceOption->lCellScore;

			//标识加注
			m_bAddGold[pUserData->wChairID] = true;
			m_bAddGoldEnd = true;

			//发送消息
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{		
				if (m_cbCardStatus[i] != NO_CARD)
				{
					if( !m_bAddGold[i] && (i != m_nBankerStation) )  m_bAddGoldEnd = false; 
					m_pITableFrame->SendTableData(i,SUB_S_ADD_GOLD,&AddGoldResult,sizeof(AddGoldResult));
				}
				m_pITableFrame->SendLookonData(i,SUB_S_ADD_GOLD,&AddGoldResult,sizeof(AddGoldResult));
			}

			//加注结束
			//开始发牌
			if(m_bAddGoldEnd && !pAddGold->bDoubleGold)
			{
				//设置状态
				m_pITableFrame->SetGameStatus(GS_PLAYING);
			
				//初始发牌
				for (BYTE i = 0; i < GAME_PLAYER; i++) 
				{
					if (m_cbCardStatus[i] == INIT_CARD)
					{
						m_cbTableCardCount[i] = 2;
					}
				}

				//设置信息
				CMD_S_InitCard InitCard;
		
				for (WORD i = 0; i < GAME_PLAYER; i++) 
				{
					m_cbTableCardArray[i][1] = GetRandCard();
					InitCard.cbCardData[i]   = m_cbTableCardArray[i][1];						
				}

				for (WORD i = 0; i < GAME_PLAYER; i++)
				{		
					InitCard.cbFundusCard = 0;

					if (m_cbCardStatus[i] != NO_CARD)
					{					
						m_cbTableCardArray[i][0] = GetRandCard();

						InitCard.cbFundusCard  = m_cbTableCardArray[i][0];									
						InitCard.cbCardSum     = m_GameLogic.GetCardSum(m_cbTableCardArray[i],2);
						InitCard.bCanCutCard   = m_GameLogic.CanCutCard(m_cbTableCardArray[i][0],m_cbTableCardArray[i][1]);

						//发送消息
						m_pITableFrame->SendTableData(i,SUB_S_INIT_CARD,&InitCard,sizeof(InitCard));
					}
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_INIT_CARD,&InitCard,sizeof(InitCard));
				}
			}

			return true;
		}
	case SUB_C_STOP_CARD:	//用户停牌
		{	
			//校验数据
			tagServerUserData * pUserData = pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus != US_PLAY) return true;
			
			return OnSubUserStop(wChairID);
		}

	case SUB_C_GET_CARD:	//用户要牌
		{
			//校验数据			
			tagServerUserData * pUserData = pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus != US_PLAY) return true;	

			//获取数据
			CMD_C_GetCard * pGetCard = (CMD_C_GetCard *)pDataBuffer;

			//设置信息
			CMD_S_SendCard SendCard;
			BYTE cbCardCount,cbCutCardCount;	

			//判断要牌
			if(!pGetCard->bCutCard)
			{
				//左手牌
				cbCardCount = ++m_cbTableCardCount[wChairID];
				m_cbTableCardArray[wChairID][cbCardCount-1] =  GetRandCard();
				
				SendCard.cbUserCard   = m_cbTableCardArray[wChairID][cbCardCount-1];
				SendCard.wCurrentUser = m_wCurrentUser;
				SendCard.wGetCardUser = wChairID;
				SendCard.cbCardSum     = m_GameLogic.GetCardSum(m_cbTableCardArray[wChairID],cbCardCount);
				SendCard.bCutCard     = false;
			}
			else
			{
				//右手牌
				ASSERT(m_cbCutCardStatus[wChairID] != NO_CARD);
				
				cbCutCardCount = ++m_cbTableCutCardCount[wChairID];
				m_cbTableCutCardArray[wChairID][cbCutCardCount-1] =  GetRandCard();

				SendCard.cbUserCard   = m_cbTableCutCardArray[wChairID][cbCutCardCount-1];
				SendCard.wCurrentUser = m_wCurrentUser;
				SendCard.wGetCardUser = wChairID;
				SendCard.cbCardSum    = m_GameLogic.GetCardSum(m_cbTableCutCardArray[wChairID],cbCutCardCount);
				SendCard.bCutCard     = true;				
			}
			
			//发送扑克
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

			//玩家爆牌		
			if(m_cbCutCard[wChairID])
			{
				if(m_GameLogic.IsBurst(m_cbTableCardArray[wChairID],m_cbTableCardCount[wChairID]))
				{
					m_cbCardStatus[wChairID] = BRUST_CARD;
					//if(m_cbTableCardCount[wChairID]==5)
					//{
					//	m_pITableFrame->SendTableData(wChairID,SUB_S_CUT_CARD_MARK);
					//}
					if((m_cbCutCardStatus[wChairID] == CUT_CARD))
					{
						m_pITableFrame->SendTableData(wChairID,SUB_S_CUT_CARD_MARK);
						m_cbCutCardStatus[wChairID] = INIT_CARD;
					}
				}
					
				if(m_GameLogic.IsBurst(m_cbTableCutCardArray[wChairID],m_cbTableCutCardCount[wChairID]))
				{
					m_cbCutCardStatus[wChairID] = BRUST_CARD;
					//if(m_cbTableCutCardCount[wChairID]==5)
					//{
					//	OnSubUserStop(wChairID);
					//}
					if(m_cbCardStatus[wChairID] == BRUST_CARD)
					{				
						m_wPlayerCount--;
					}

					CMD_S_BrustCard BrustCard;
					BrustCard.wBrustUser = wChairID;

					//发送爆牌消息
					m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BRUST_CARD,&BrustCard,sizeof(BrustCard)); 
				}					
			}
			else if(m_GameLogic.IsBurst(m_cbTableCardArray[wChairID],m_cbTableCardCount[wChairID]))
			{
				m_cbCardStatus[wChairID] = BRUST_CARD;
				m_wPlayerCount--;

				CMD_S_BrustCard BrustCard;
				BrustCard.wBrustUser = wChairID;

				//发送爆牌消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BRUST_CARD,&BrustCard,sizeof(BrustCard));				
				//if(m_cbTableCardCount[wChairID]==5)
				//{
				//	OnSubUserStop(wChairID);
				//}

				//庄家爆牌
				if(wChairID == m_nBankerStation)
				{		
					//游戏结束		
					OnEventGameEnd(INVALID_CHAIR,NULL,GO_NORMAL_FINISH);
					return true;
				}			
			}

			//仅剩庄家
			if (m_wPlayerCount == 1)
			{
				m_cbCardStatus[m_nBankerStation] = STOP_CARD;
				OnEventGameEnd(INVALID_CHAIR,NULL,GO_NORMAL_FINISH);
				return true;
			}		

			WORD wPlayerCount = 0;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{					
				if(m_cbCutCard[i])
				{
					if(((m_cbCardStatus[i] == BRUST_CARD) || (m_cbCardStatus[i]    == STOP_CARD)) &&
					 ((m_cbCutCardStatus[i] == BRUST_CARD) || (m_cbCutCardStatus[i] == STOP_CARD)))
						wPlayerCount++;
				}
				else
				{
					if ((m_cbCardStatus[i] == BRUST_CARD) || (m_cbCardStatus[i] == NO_CARD) || (m_cbCardStatus[i] == STOP_CARD)) 
						wPlayerCount++;
				}
			}

			if ((GAME_PLAYER - wPlayerCount) == 1)
			{
				//庄家发牌
				m_bBankerGetCard = true;
				m_pITableFrame->SendTableData(m_nBankerStation,SUB_S_BANKER_SEND_CARD);		
			}			
			
			return true;
		}	
	case SUB_C_CUT_CARD:	//用户分牌
		{	
			//校验数据			
			tagServerUserData * pUserData = pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus != US_PLAY) return true;
			WORD wChairID = pUserData->wChairID;
			
			ASSERT(m_GameLogic.CanCutCard(m_cbTableCardArray[wChairID][0],m_cbTableCardArray[wChairID][1]));
			if(!m_GameLogic.CanCutCard(m_cbTableCardArray[wChairID][0],m_cbTableCardArray[wChairID][1])) return false;

			//设置信息
			m_cbTableCutCardArray[wChairID][0] = m_cbTableCardArray[wChairID][1];
			m_cbTableCardArray[wChairID][1]    = 0;

			m_cbTableCardCount[wChairID]    = 1;
			m_cbTableCutCardCount[wChairID] = 1;

			CMD_S_CutCard CutCard;
			CutCard.wCutCardUser    = wChairID;
			CutCard.cbFirstCard     = m_cbTableCardArray[wChairID][0];
			CutCard.cbSecondCard    = m_cbTableCutCardArray[wChairID][0];
			CutCard.cbFirstCardSum  = m_GameLogic.GetCardSum(m_cbTableCardArray[wChairID],1);
			CutCard.cbSecondCardSum = m_GameLogic.GetCardSum(m_cbTableCutCardArray[wChairID],1);

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CUT_CARD,&CutCard,sizeof(CutCard));  

			m_cbCutCardStatus[wChairID] = CUT_CARD;
			m_cbCutCard[wChairID]       = true;
		
			return true;
		}

	}
	return false;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, 
											 const void * pDataBuffer, 
											 WORD wDataSize, 
											 IServerUserItem * pIServerUserItem)
{
	return false;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_ADDGOLDING);

	//变量定义
	const tagUserScore * pUserScore=NULL;
	IServerUserItem * pIServerUserItem=NULL;

	

	//散列扑克
	m_GameLogic.RandCard(m_cbRandCardArray,52);

	//发牌状态	
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		pIServerUserItem = m_pITableFrame->GetServerUserItem(i);

		if (pIServerUserItem != NULL)
		{		
			m_wPlayerCount++;

			m_cbCardStatus[i] = INIT_CARD;
		}
	}
	
	//庄家位置
	if(!m_bBankerWin)
	{	
		for (BYTE i = 0; i < GAME_PLAYER; i++) 
		{
			m_nBankerStation = (m_nBankerStation + 1) % GAME_PLAYER ;

			if (m_cbCardStatus[m_nBankerStation] == INIT_CARD) break;		
		}
	}

	//计算限注
	bool bCalculateGold = false;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if(m_nBankerStation == i) continue;

		pIServerUserItem = m_pITableFrame->GetServerUserItem(i);
		if (pIServerUserItem != NULL)
		{
			if (bCalculateGold == false)
			{
				bCalculateGold = true;
				pUserScore = pIServerUserItem->GetUserScore();
				ASSERT(pUserScore->lScore > pGameServiceOption->lCellScore);
				m_lMaxGold = pUserScore->lScore / 8;
			}
			else 
			{
				pUserScore = pIServerUserItem->GetUserScore();
				ASSERT(pUserScore->lScore > 1L);
				m_lMaxGold = __min(m_lMaxGold, pUserScore->lScore / 8);
			}
		}
	}

	pIServerUserItem = m_pITableFrame->GetServerUserItem(m_nBankerStation);
	pUserScore = pIServerUserItem->GetUserScore();
	m_lMaxGold = __min(m_lMaxGold,pUserScore->lScore / m_wPlayerCount / 8);
	m_lTurnBasicGold=pGameServiceOption->lCellScore;
	//发送开始
	CMD_S_GameStart GameStart;
	memset(&GameStart,0,sizeof(GameStart));
	
	GameStart.wBankerStation  = m_nBankerStation;
	GameStart.wCurrentUser    = m_wCurrentUser;
	GameStart.lMaxGold        = m_lMaxGold;
	GameStart.lTurnBasicGold  = m_lTurnBasicGold;


	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_cbCardStatus[i] ==TRUE)
		{
			if(i!=m_nBankerStation)
			{
				m_lTableGold[2*i] = m_lTurnBasicGold;
			}
			else
			{
				m_lTableGold[2*i]=0;
			}
			
			m_pITableFrame->SendTableData(i,
				                          SUB_S_GAME_START,
										  &GameStart,
										  sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(i, 
			                           SUB_S_GAME_START, 
									   &GameStart, 
									   sizeof(GameStart));
	}
	return true;
}


//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, 
											 IServerUserItem * pIServerUserItem, 
											 BYTE cbReason)
{	
	tagScoreInfo	   ScoreInfo[GAME_PLAYER];
	memset(&ScoreInfo, 0, sizeof(ScoreInfo));

	//定义变量
	CMD_S_GameEnd GameEnd;
	memset(&GameEnd,0,sizeof(GameEnd));	

	LONG	lBankerScore = 0;
	LONG	lTempTax       = 0;

	switch(cbReason)
	{
	case GER_USER_LEFT:			//用户强退
		{	
			//强退处理
			if(wChairID == m_nBankerStation)
			{			
				 return OnUserLeft(m_bAddGoldEnd);			
			}
			else
			{	
				return OnUserLeft(wChairID,m_bAddGoldEnd);					
			}			
			return true;
		}				
	case GO_NORMAL_FINISH: 
		{	
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//底牌信息
				if ((m_cbCardStatus[i] == NO_CARD)) GameEnd.cbUserCard[i]=0;
				else GameEnd.cbUserCard[i] = m_cbTableCardArray[i][0];	
				if(!m_cbCutCard)GameEnd.cbCutCard[i] = 0;
				else GameEnd.cbCutCard[i] = m_cbTableCutCardArray[i][0];

				if(m_nBankerStation == i) continue;

				//统计成绩
				if(m_cbCardStatus[i] != NO_CARD)
				{
					IServerUserItem * pIUserItem = m_pITableFrame->GetServerUserItem(i);
					ASSERT(pIUserItem != NULL);	

					LONG lGameGold = 0L, lBankerScore = 0L;
					bool bCutCard  = false;

					BYTE cbCardStatus = m_cbCardStatus[i];
					CalcScore(cbCardStatus, i, lGameGold, lBankerScore, bCutCard);

					GameEnd.lGameGold[i]  = lGameGold;
					m_lBankerScore       += lBankerScore;

					if(m_cbCutCard[i])
					{						
						lGameGold    = 0L; 
						lBankerScore = 0L;
						bCutCard     = true;

						cbCardStatus = m_cbCutCardStatus[i];
						CalcScore(cbCardStatus, i, lGameGold, lBankerScore, bCutCard);

						GameEnd.lGameGold[i] += lGameGold;
						m_lBankerScore       += lBankerScore;	
					}									
				}	
			}	

			//分数类型
			LONG lTax=0;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardSum[i]    = m_GameLogic.GetCardSum(m_cbTableCardArray[i],m_cbTableCardCount[i]);
				GameEnd.cbCutCardSum[i] = m_GameLogic.GetCardSum(m_cbTableCutCardArray[i],m_cbTableCutCardCount[i]);

				if(m_nBankerStation == i) continue;

				if(m_cbCardStatus[i] != NO_CARD)
				{
					IServerUserItem * pIUserItem = m_pITableFrame->GetServerUserItem(i);
					ASSERT(pIUserItem != NULL);		

					if(GameEnd.lGameGold[i] > 0)
					{
						//赢家收税
						//GameEnd.lTax += (GameEnd.lGameGold[i] >= 100L) ? GameEnd.lGameGold[i] / 50L : 0L;
						lTax=(GameEnd.lGameGold[i] >= MIN_TAX_NUM) ? GameEnd.lGameGold[i] * pGameServiceOption->wRevenue/100L: 0L;
						GameEnd.lTax +=lTax; 
						ScoreInfo[i].lRevenue= lTax;
						GameEnd.lGameGold[i] -= lTax;
						ScoreInfo[i].ScoreKind = enScoreKind_Win;
					}
					else if(GameEnd.lGameGold[i] == 0)
					{
						ScoreInfo[i].ScoreKind = enScoreKind_Draw;
					}
					else
					{
						ScoreInfo[i].ScoreKind = enScoreKind_Lost;
					}	

					ScoreInfo[i].lScore = GameEnd.lGameGold[i];
				}
			}	

			//游戏税收
			lTax=(m_lBankerScore >= MIN_TAX_NUM) ? m_lBankerScore * pGameServiceOption->wRevenue/100L: 0L;
			GameEnd.lTax += lTax;
			ScoreInfo[m_nBankerStation].lRevenue =lTax;
			GameEnd.lGameGold[m_nBankerStation]   = m_lBankerScore - lTax;

			//庄家分数
			ScoreInfo[m_nBankerStation].lScore    = GameEnd.lGameGold[m_nBankerStation];				
			if(ScoreInfo[m_nBankerStation].lScore > 0)
			{
				ScoreInfo[m_nBankerStation].ScoreKind = enScoreKind_Win;
			}
			else if(ScoreInfo[m_nBankerStation].lScore == 0)
			{
				ScoreInfo[m_nBankerStation].ScoreKind = enScoreKind_Draw;
			}
			else
			{
				ScoreInfo[m_nBankerStation].ScoreKind = enScoreKind_Lost;
			}	

			//辅助变量
			m_bBankerWin = ScoreInfo[m_nBankerStation].lScore > 0 ? true : false;

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改分数
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore,ScoreInfo[i].lRevenue,ScoreInfo[i].ScoreKind);

			}
			//m_pITableFrame->WriteTableScore(ScoreInfo,GAME_PLAYER,GameEnd.lTax);

			//结束游戏
			m_pITableFrame->ConcludeGame();
			break;
		}	
	}
	return true;
}

//计算成绩
bool CTableFrameSink::CalcScore(BYTE cbCardStatus,WORD wChairID,
								LONG &lGameGold,  LONG &lBankerScore,
								bool bCutCard)
{
	BYTE bWin = 0;
	BYTE bTimes=1;
	//爆牌玩家
	//if ((cbCardStatus == BRUST_CARD)) 
	//{
	//	lGameGold    =  -m_lUserAddGold[wChairID];
	//	lBankerScore = m_lUserAddGold[wChairID]; 
	//	return true;
	//}

	ASSERT((cbCardStatus == STOP_CARD) || (cbCardStatus == DOUBLE_GOLD));

	////庄家爆牌
	//if(m_cbCardStatus[m_nBankerStation] == BRUST_CARD)
	//{						
	//	lGameGold     = m_lUserAddGold[wChairID];
	//	lBankerScore -= m_lUserAddGold[wChairID]; 
	//	return true;
	//}
	////庄家停牌
	//else
	{	
		//推断胜者
		if(bCutCard)
		{
			bWin = m_GameLogic.DeduceWiner(m_cbTableCardArray[m_nBankerStation],
				m_cbTableCardCount[m_nBankerStation],
				m_cbTableCutCardArray[wChairID],
				m_cbTableCutCardCount[wChairID],&bTimes);
		}
		else
		{
			bWin = m_GameLogic.DeduceWiner(m_cbTableCardArray[m_nBankerStation],
				m_cbTableCardCount[m_nBankerStation],
				m_cbTableCardArray[wChairID],
				m_cbTableCardCount[wChairID],&bTimes);
		}	
		//char *str=new char[100];
		//sprintf(str,"%d倍",bTimes);
		//MessageBox(0,str,0,0);
		//delete []str;
		switch (bWin)
		{
			//平牌
		case 0:
			{
				lGameGold     +=0;// -m_lUserAddGold[wChairID];
				lBankerScore  +=0;// m_lUserAddGold[wChairID];	

				break;
			}
			//庄家胜
		case 1:
			{
				//BYTE times;
				//if(m_GameLogic.IsSevensCard(m_cbTableCardArray[m_nBankerStation],))
				lGameGold     = -bTimes*m_lUserAddGold[wChairID];
				lBankerScore  += bTimes*m_lUserAddGold[wChairID];							

				break;
			}
			//闲家胜
		case 2:
			{
				lGameGold     = bTimes*m_lUserAddGold[wChairID];					
				lBankerScore  -= bTimes*m_lUserAddGold[wChairID];

				break;
			}
		}
	}	
	return true;
}

//闲家强退
bool CTableFrameSink::OnUserLeft(WORD wChairID,bool bAddGoldEnd)
{
	//定义变量
	CMD_S_GameEnd GameEnd;
	memset(&GameEnd,0,sizeof(GameEnd));	

	m_cbCardStatus[wChairID] = NO_CARD;
	m_wPlayerCount--;

	//判断结束
	WORD wPlayerCount = 0;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetServerUserItem(i);

		if (pIServerUserItem != NULL && m_cbCardStatus[i] == INIT_CARD)
		{		
			wPlayerCount++;
		}
	}

	//修改分数
	if(!m_bAddGoldEnd)	
	{
		//修改分数	
		tagScoreInfo ScoreInfo;	
		memset(&ScoreInfo, 0, sizeof(ScoreInfo));
		ScoreInfo.ScoreKind = enScoreKind_Flee;
		ScoreInfo.lScore    = -m_lNotBankerScore[wChairID]*3;	
		m_lBankerScore      += m_lTurnBasicGold;

		m_pITableFrame->WriteUserScore(wChairID,ScoreInfo.lScore,ScoreInfo.lRevenue,ScoreInfo.ScoreKind);		

	}
	else
	{
		//修改分数	
		tagScoreInfo ScoreInfo;	
		memset(&ScoreInfo, 0, sizeof(ScoreInfo));
		ScoreInfo.ScoreKind = enScoreKind_Flee;
		ScoreInfo.lScore    = -m_lNotBankerScore[wChairID]*3;	
		m_lBankerScore      += m_lUserAddGold[wChairID];

		m_pITableFrame->WriteUserScore(wChairID,ScoreInfo.lScore,ScoreInfo.lRevenue,ScoreInfo.ScoreKind);	
	}

	if(wPlayerCount >= 2)
	{
		return true;
	}
	//仅剩庄家
	else 
	{
		tagScoreInfo	   ScoreInfo[GAME_PLAYER];
		memset(&ScoreInfo, 0, sizeof(ScoreInfo));

		LONG lTax = 0L;

		//庄家分数		
		lTax = (m_lBankerScore >= 100L) ? m_lBankerScore * pGameServiceOption->wRevenue/100L : 0L;
		ScoreInfo[m_nBankerStation].lScore    = m_lBankerScore - lTax;	
		ScoreInfo[m_nBankerStation].ScoreKind = enScoreKind_Win;
		ScoreInfo[m_nBankerStation].lRevenue=lTax;
		//修改分数
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore,ScoreInfo[i].lRevenue,ScoreInfo[i].ScoreKind);

		}
		//m_pITableFrame->WriteTableScore(ScoreInfo,GAME_PLAYER,lTax);		

		for(int i = 0; i < GAME_PLAYER; i++)
		{
			if(!m_bAddGoldEnd) GameEnd.lGameGold[i] = -m_lTurnBasicGold;
			else GameEnd.lGameGold[i] = -m_lNotBankerScore[wChairID]*3;
		}

		GameEnd.lGameGold[m_nBankerStation] = ScoreInfo[m_nBankerStation].lScore;	
		GameEnd.lTax = lTax;

		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
		
		//结束游戏
		m_pITableFrame->ConcludeGame();
	}
	return true;
}

//庄家强退
bool CTableFrameSink::OnUserLeft(bool bAddGoldEnd)
{
	//定义变量
	CMD_S_GameEnd GameEnd;
	memset(&GameEnd,0,sizeof(GameEnd));	

	//发送消息
	//	CMD_S_UserLeft UserLeft;
	//	UserLeft.wUserChairID = m_nBankerStation;

	//	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_LEFT,&UserLeft,sizeof(UserLeft));
	//	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_LEFT,&UserLeft,sizeof(UserLeft));

	//修改分数
	if(!m_bAddGoldEnd)
	{	
		//庄家分数	
		tagScoreInfo ScoreInfo;	
		memset(&ScoreInfo, 0, sizeof(ScoreInfo));
		ScoreInfo.ScoreKind = enScoreKind_Flee;
		ScoreInfo.lScore    = -m_lMaxGold;

		m_pITableFrame->WriteUserScore(m_nBankerStation,ScoreInfo.lScore,ScoreInfo.lRevenue,ScoreInfo.ScoreKind);	

		GameEnd.lGameGold[m_nBankerStation] = -m_lMaxGold;	

		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
	}
	else
	{
		tagScoreInfo	   ScoreInfo[GAME_PLAYER];
		memset(&ScoreInfo, 0, sizeof(ScoreInfo));
		LONG lTax=0L;

		LONG lBankerScore = 0;

		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//底牌信息
			if ((m_cbCardStatus[i] == NO_CARD)) GameEnd.cbUserCard[i]=0;
			else GameEnd.cbUserCard[i] = m_cbTableCardArray[i][0];

			if(m_nBankerStation == i) continue;

			//统计成绩
			if(m_cbCardStatus[i] != NO_CARD)
			{
				IServerUserItem * pIUserItem=m_pITableFrame->GetServerUserItem(i);
				ASSERT(pIUserItem!=NULL);					

				GameEnd.lGameGold[i] = m_lUserAddGold[i];
				lTax=(GameEnd.lGameGold[i] >= 100L) ? GameEnd.lGameGold[i] *pGameServiceOption->wRevenue/100L : 0L;

				GameEnd.lTax += lTax;
				GameEnd.lGameGold[i] -= lTax;		

				lBankerScore += -m_lUserAddGold[i]*2;	

				ScoreInfo[i].lScore    = GameEnd.lGameGold[i];	
				ScoreInfo[i].ScoreKind = enScoreKind_Win;
				ScoreInfo[i].lRevenue=lTax;
			}					
		}	

		GameEnd.lGameGold[m_nBankerStation]   = lBankerScore;		
		ScoreInfo[m_nBankerStation].lScore    = lBankerScore;
		ScoreInfo[m_nBankerStation].ScoreKind = enScoreKind_Flee;

		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

		//修改分数
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore,ScoreInfo[i].lRevenue,ScoreInfo[i].ScoreKind);

		}
		//m_pITableFrame->WriteTableScore(ScoreInfo,GAME_PLAYER,GameEnd.lTax);
	}

	m_bBankerWin = false;

	//结束游戏
	m_pITableFrame->ConcludeGame();

	return true;
}

//获取扑克
BYTE CTableFrameSink::GetRandCard()
{
	m_cbSendCardCount++;
	return 	m_cbRandCardArray[m_cbSendCardCount];
}

//用户停牌
bool CTableFrameSink::OnSubUserStop(WORD wChairID)
{
	//设置信息
	CMD_S_StopCard StopCard;
	StopCard.wStopUser = wChairID;	

	//庄家停牌
	if(wChairID == m_nBankerStation)
	{			
		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard));

		//游戏结束
		OnEventGameEnd(INVALID_CHAIR,NULL,GO_NORMAL_FINISH);
	}
	if(m_cbCardStatus[wChairID] != BRUST_CARD)
	m_cbCardStatus[wChairID] = STOP_CARD;		

	if(!m_cbCutCard[wChairID])
	{		
		StopCard.bDoubleBrsut = m_GameLogic.IsBurst(m_cbTableCardArray[wChairID],m_cbTableCardCount[wChairID]);

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard));
	}
	else
	{
		if((m_cbCardStatus[wChairID] == STOP_CARD)/* ||m_cbTableCardCount[wChairID]==5*/|| (m_cbCardStatus[wChairID] == BRUST_CARD))
		{
			if(m_cbCutCardStatus[wChairID] == INIT_CARD)
			{
				m_cbCutCardStatus[wChairID] = STOP_CARD;
			}
			if(m_cbCutCardStatus[wChairID] != STOP_CARD)
			{
				m_cbCutCardStatus[wChairID] = INIT_CARD;
				m_pITableFrame->SendTableData(wChairID,SUB_S_CUT_CARD_MARK);
			}
		}

		if(m_cbCutCardStatus[wChairID] == STOP_CARD)
		{
			StopCard.bDoubleBrsut = m_GameLogic.IsBurst(m_cbTableCutCardArray[wChairID],m_cbTableCutCardCount[wChairID]);

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard));
		}	
	}			

	//判断是否轮庄家要牌
	WORD wPlayerCount = 0;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{					
		if(m_cbCutCard[wChairID])
		{
			if ((m_cbCardStatus[i] == BRUST_CARD/*m_cbTableCardCount[i]==5 */|| m_cbCardStatus[i] == NO_CARD    || m_cbCardStatus[i]    == STOP_CARD) &&
				(m_cbCutCardStatus[i] == BRUST_CARD/*m_cbTableCutCardCount[i]==5 */|| m_cbCutCardStatus[i] == NO_CARD || m_cbCutCardStatus[i] == STOP_CARD))
				wPlayerCount++;
		}
		else
		{
			if (m_cbCardStatus[i] == BRUST_CARD/*m_cbTableCardCount[i]==5*/ || m_cbCardStatus[i] == NO_CARD || m_cbCardStatus[i] == STOP_CARD) 
				wPlayerCount++;
		}
	}
	if ((GAME_PLAYER - wPlayerCount) == 1)
	{
		//庄家发牌
		m_bBankerGetCard = true;
		m_pITableFrame->SendTableData(m_nBankerStation,SUB_S_BANKER_SEND_CARD);		
	}			

	return true;
}
//////////////////////////////////////////////////////////////////////////

