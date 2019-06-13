#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS					2									//最少时间
#define TIME_DISPATCH				5									//发牌时间

//游戏时间
#define TIME_OUT_CARD				8									//出牌时间
#define TIME_START_GAME				8									//开始时间
#define TIME_CALL_SCORE				5									//叫分时间

//游戏时间
#define IDI_OUT_CARD				(IDI_ANDROID_ITEM_SINK+0)			//出牌时间
#define IDI_START_GAME				(IDI_ANDROID_ITEM_SINK+1)			//开始时间
#define IDI_CALL_SCORE				(IDI_ANDROID_ITEM_SINK+2)			//叫分时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//扑克变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	m_wOutCardUser = INVALID_CHAIR ;
	m_wBankerUser = INVALID_CHAIR ;

	//手上扑克
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	m_cbCurrentLandScore = 255 ;

	//接口变量
	m_pIAndroidUserItem=NULL;

	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void * __cdecl CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool __cdecl CAndroidUserItemSink::InitUserItemSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool __cdecl CAndroidUserItemSink::RepositUserItemSink()
{
	//扑克变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//手上扑克
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	return true;
}

//时间消息
bool __cdecl CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//开始游戏
		{
			//开始判断
			m_pIAndroidUserItem->SendUserReady(NULL,0);

			return true;
		}
	case IDI_CALL_SCORE:	//用户叫分
		{
			//构造变量
			CMD_C_LandScore CallScore;
			ZeroMemory(&CallScore,sizeof(CallScore));

			//设置变量
			CallScore.bLandScore=m_GameLogic.LandScore(m_pIAndroidUserItem->GetChairID(), m_cbCurrentLandScore);

			//发送数据
			m_pIAndroidUserItem->SendSocketData(SUB_C_LAND_SCORE,&CallScore,sizeof(CallScore));

			return true;
		}
	case IDI_OUT_CARD:		//用户出牌
		{
			//扑克分析
			tagOutCardResult OutCardResult;
			try
			{				
				m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount,m_cbTurnCardData,m_cbTurnCardCount, m_wOutCardUser, m_pIAndroidUserItem->GetChairID(), OutCardResult);
			}
			catch(...)
			{
				//这里的设置，使得进入下面的if处理
				ZeroMemory(OutCardResult.cbResultCard, sizeof(OutCardResult.cbResultCard)) ;
				OutCardResult.cbCardCount = 10 ;
			}
			//牌型合法判断
			if(OutCardResult.cbCardCount>0 && CT_ERROR==m_GameLogic.GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount))
			{
				ASSERT(false) ;
#ifdef _DEBUG
				CString str ;
				for(BYTE i=0; i<OutCardResult.cbCardCount; ++i)
				{
					CString strTmp ;
					strTmp.Format("%d,", m_GameLogic.GetCardValue(OutCardResult.cbResultCard[i])) ;
					str += strTmp ;
				}
				str += "\n\n" ;
				OutputDebugString(str) ;
			
#endif
				ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;
			}

			//先出牌不能为空
			if(m_cbTurnCardCount==0)
			{
				ASSERT(OutCardResult.cbCardCount>0) ;
				if(OutCardResult.cbCardCount==0)
				{
					//最小一张
					OutCardResult.cbCardCount = 1 ;
					OutCardResult.cbResultCard[0]=m_cbHandCardData[m_cbHandCardCount-1] ;
				}
			}

			//结果判断
			if (OutCardResult.cbCardCount>0)
			{
				//合法判断
				if(m_cbTurnCardCount>0 && !m_GameLogic.CompareCard(m_cbTurnCardData, OutCardResult.cbResultCard, m_cbTurnCardCount, OutCardResult.cbCardCount))
				{
					ASSERT(false) ;

					//放弃出牌
					m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
					return true ;
				}

				//删除扑克
				m_cbHandCardCount-=OutCardResult.cbCardCount;
				m_GameLogic.RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,m_cbHandCardData,m_cbHandCardCount+OutCardResult.cbCardCount);

				//构造变量
				CMD_C_OutCard OutCard;
				ZeroMemory(&OutCard,sizeof(OutCard));

				//设置变量
				OutCard.bCardCount=OutCardResult.cbCardCount;
				CopyMemory(OutCard.bCardData,OutCardResult.cbResultCard,OutCardResult.cbCardCount*sizeof(BYTE));

				//发送数据
				WORD wHeadSize=sizeof(OutCard)-sizeof(OutCard.bCardData);
				m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CART,&OutCard,wHeadSize+OutCard.bCardCount*sizeof(BYTE));
			}
			else
			{
				//放弃出牌
				m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
			}

			return true;
		}
	}

	return false;
}

//游戏消息
bool __cdecl CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_SEND_CARD:	//游戏开始
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_LAND_SCORE:	//用户叫分
		{
			return OnSubCallScore(pData,wDataSize);
		}
	case SUB_S_GAME_START:	//庄家信息
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:	//用户出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_PASS_CARD:	//用户放弃
		{
			return OnSubPassCard(pData,wDataSize);
		}
	case SUB_S_GAME_END:	//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏消息
bool __cdecl CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool __cdecl CAndroidUserItemSink::OnEventGameScene(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_WK_FREE:			//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

			//玩家设置
			if (pIServerUserItem->GetUserStatus()!=US_READY)
			{
				UINT nElapse=rand()%TIME_START_GAME+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
			}

			return true;
		}
	case GS_WK_SCORE:		//叫分状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusScore)) return false;
			CMD_S_StatusScore * pStatusCall=(CMD_S_StatusScore *)pData;

			//扑克数据
			m_cbHandCardCount=NORMAL_COUNT;
			CopyMemory(m_cbHandCardData,pStatusCall->bCardData,sizeof(BYTE)*m_cbHandCardCount);

			//叫分设置
			if (m_pIAndroidUserItem->GetChairID()==pStatusCall->wCurrentUser)
			{
				UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_CALL_SCORE,nElapse+TIME_DISPATCH);
			}

			return true;
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//出牌变量
			m_cbTurnCardCount=pStatusPlay->bTurnCardCount;
			CopyMemory(m_cbTurnCardData,pStatusPlay->bTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

			//扑克数据
			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
			m_cbHandCardCount=pStatusPlay->bCardCount[wMeChairID];
			CopyMemory(m_cbHandCardData,pStatusPlay->bCardData,sizeof(BYTE)*m_cbHandCardCount);

			//玩家设置
			if (pStatusPlay->wCurrentUser==m_pIAndroidUserItem->GetChairID())
			{
				UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
			}

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//用户进入
void __cdecl CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
void __cdecl CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
void __cdecl CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
void __cdecl CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
void __cdecl CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//游戏开始
bool CAndroidUserItemSink::OnSubSendCard(void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_SendAllCard));
	if (wDataSize!=sizeof(CMD_S_SendAllCard)) return false;

	//变量定义
	CMD_S_SendAllCard * pSendCard=(CMD_S_SendAllCard *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_WK_SCORE);

	//扑克变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	m_cbCurrentLandScore = 255 ;

	//所有扑克
	for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
		m_GameLogic.SetUserCard(wChairID, pSendCard->bCardData[wChairID], NORMAL_COUNT) ;

	//叫牌扑克
	BYTE cbLandScoreCardData[MAX_COUNT] ;
	CopyMemory(cbLandScoreCardData, pSendCard->bCardData[m_pIAndroidUserItem->GetChairID()], NORMAL_COUNT) ;
	CopyMemory(cbLandScoreCardData+NORMAL_COUNT, pSendCard->bBackCardData, BACK_COUNT) ;
	m_GameLogic.SetLandScoreCardData(cbLandScoreCardData, sizeof(cbLandScoreCardData)) ;

	//手上扑克
	m_cbHandCardCount=NORMAL_COUNT;	
	CopyMemory(m_cbHandCardData, pSendCard->bCardData[m_pIAndroidUserItem->GetChairID()], NORMAL_COUNT) ; 

	//排列扑克
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount,ST_ORDER);

	//玩家处理
	if (m_pIAndroidUserItem->GetChairID()==pSendCard->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_CALL_SCORE,nElapse+TIME_DISPATCH);
	}

	return true;
}

//用户叫分
bool CAndroidUserItemSink::OnSubCallScore(void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_LandScore));
	if (wDataSize!=sizeof(CMD_S_LandScore)) return false;

	//变量定义
	CMD_S_LandScore * pCallScore=(CMD_S_LandScore *)pData;
	m_cbCurrentLandScore = pCallScore->bCurrentScore ;

	//设置庄家
	m_wBankerUser = pCallScore->bLandUser ;
	m_GameLogic.SetBanker(pCallScore->bLandUser) ;

	//用户处理
	if (m_pIAndroidUserItem->GetChairID()==pCallScore->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_CALL_SCORE,nElapse);
	}

	return true;
}

//庄家信息
bool CAndroidUserItemSink::OnSubGameStart(void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_WK_PLAYING);

	//设置庄家
	m_wBankerUser = pGameStart->wLandUser ;
	m_GameLogic.SetBanker(pGameStart->wLandUser) ;

	//设置底牌
	m_GameLogic.SetBackCard(pGameStart->wLandUser, pGameStart->bBackCard, BACK_COUNT) ;
	
	//设置扑克
	if (pGameStart->wLandUser==m_pIAndroidUserItem->GetChairID())
	{
		//设置扑克
		m_cbHandCardCount+=CountArray(pGameStart->bBackCard);
		CopyMemory(&m_cbHandCardData[NORMAL_COUNT],pGameStart->bBackCard,sizeof(pGameStart->bBackCard));

		//排列扑克
		m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount,ST_ORDER);
	}

	//玩家设置
	if (pGameStart->wCurrentUser==m_pIAndroidUserItem->GetChairID())
	{
		UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
	}

	return true;
}

//用户出牌
bool CAndroidUserItemSink::OnSubOutCard(void * pData, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->bCardData);

	//效验数据
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(BYTE))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(BYTE)))) return false;

	//出牌变量
	if (pOutCard->wCurrentUser==pOutCard->wOutCardUser)
	{
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	}
	else
	{
		m_cbTurnCardCount=pOutCard->bCardCount;
		m_wOutCardUser = pOutCard->wOutCardUser ;
		CopyMemory(m_cbTurnCardData,pOutCard->bCardData,pOutCard->bCardCount*sizeof(BYTE));
	}

	//玩家设置
	if (m_pIAndroidUserItem->GetChairID()==pOutCard->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
	}

	//设置变量
	m_GameLogic.RemoveUserCardData(pOutCard->wOutCardUser, pOutCard->bCardData, pOutCard->bCardCount) ;

	return true;
}

//用户放弃
bool CAndroidUserItemSink::OnSubPassCard(void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PassCard));
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;

	//变量定义
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pData;

	//一轮判断
	if (pPassCard->bNewTurn==TRUE)
	{
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	}

	//玩家设置
	if (m_pIAndroidUserItem->GetChairID()==pPassCard->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//变量定义
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_WK_FREE);

	//设置变量
	m_cbTurnCardCount=0;
	m_cbHandCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OUT_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_CALL_SCORE);

	//开始设置
	UINT nElapse=rand()%TIME_START_GAME+TIME_LESS;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	return true;
}

//////////////////////////////////////////////////////////////////////////
