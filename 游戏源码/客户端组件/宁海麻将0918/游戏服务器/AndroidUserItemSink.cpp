#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//test
//#define TEST

//辅助时间
#define TIME_LESS					1									//最少时间

//游戏时间
#ifndef TEST
#define TIME_START_GAME				5									//开始时间
#define TIME_OPERATE_CARD			3									//操作牌时间
#define TIME_HEAR_STATUS			2									//听牌时间
#else
#define TIME_START_GAME				3									//开始时间
#define TIME_OPERATE_CARD			3									//操作牌时间
#define TIME_HEAR_STATUS			3									//听牌时间
#endif

//游戏时间
#define IDI_START_GAME				(IDI_ANDROID_ITEM_SINK+1)			//开始时间
#define IDI_OPERATE_CARD			(IDI_ANDROID_ITEM_SINK+11)			//操作牌时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//状态变量
	m_bHearStatus=false;
	m_bActionMask = WIK_NULL;
	m_bActionCard = 0;

	//出牌信息
	m_cbEnjoinCardCount = 0;
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbEnjoinCardData,sizeof(m_cbEnjoinCardData));	
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

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
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//状态变量
	m_bHearStatus=false;
	m_bActionMask = WIK_NULL;
	m_bActionCard = 0;

	//出牌信息
	m_cbEnjoinCardCount = 0;
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbEnjoinCardData,sizeof(m_cbEnjoinCardData));	
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

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
	case IDI_OPERATE_CARD:		//用户出牌
		{
			tagOutCardResult OutCardResult;

			ASSERT( m_wCurrentUser == INVALID_CHAIR || m_pIAndroidUserItem->GetChairID() == m_wCurrentUser );
			if( m_wCurrentUser != INVALID_CHAIR && m_pIAndroidUserItem->GetChairID() != m_wCurrentUser )
				return true;
			if( m_GameLogic.SearchOutCard(m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,
				m_cbCardIndex,m_WeaveItemArray,m_cbWeaveCount,m_cbDiscardCard,m_cbDiscardCount,m_bActionMask,m_bActionCard,
				m_cbEnjoinCardData,m_cbEnjoinCardCount,OutCardResult) )
			{
				if( OutCardResult.cbOperateCode != WIK_NULL )
				{
					OnOperateCard(OutCardResult.cbOperateCode,OutCardResult.cbOperateCard);
				}else
				{				
					ASSERT( m_cbCardIndex[m_pIAndroidUserItem->GetChairID()][m_GameLogic.SwitchToCardIndex(OutCardResult.cbOperateCard)] > 0 );
					OnOutCard(OutCardResult.cbOperateCard);
				}
			}else
			{
				OnOperateCard(WIK_NULL,0);
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
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:		//发牌消息
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_OPERATE_NOTIFY:	//操作提示
		{
			return OnSubOperateNotify(pData,wDataSize);
		}
	case SUB_S_OPERATE_RESULT:	//操作结果
		{
			return OnSubOperateResult(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_FORCE_OUT_CARD:	//强迫出牌
		{
			return OnSubForceOutCard(pData,wDataSize);
		}
	case SUB_S_FORCE_SEND_CARD:
		{
			return OnSubForceSendCard(pData,wDataSize);
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
	case GS_MJ_FREE:	//空闲状态
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
	case GS_MJ_PLAY:		//游戏状态
		{
			return true;
		}
	}

	ASSERT(false);
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

//庄家信息
bool CAndroidUserItemSink::OnSubGameStart(void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart))	return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_PLAY);

	//设置变量
	m_wBankerUser=pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wCurrentUser;
	
	//首发处理
	if(pGameStart->bFirst)
	{
		m_cbLeftCardCount=MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1;
		ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
		ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
		//出牌信息
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
		m_cbEnjoinCardCount = 0;
		ZeroMemory(m_cbEnjoinCardData,sizeof(m_cbEnjoinCardData));
		//组合扑克
		ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
		ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
		ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
		//状态信息
		m_bHearStatus = false;
		m_bActionCard = 0;
		m_bActionMask = WIK_NULL;
	}

	//设置扑克
	if( pGameStart->bFirst )
	{
		WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
		BYTE cbCardCount = (wMeChairId==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
		m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex[wMeChairId]);
		BYTE bIndex = 1;
		for( WORD i = 0; i < GAME_PLAYER; i++ )
		{
			if( i == wMeChairId ) continue;
			cbCardCount=(i==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
			m_GameLogic.SwitchToCardIndex(&pGameStart->cbCardData[MAX_COUNT*bIndex++],cbCardCount,m_cbCardIndex[i]);
		}
		//库存扑克
		CopyMemory(m_cbRepertoryCard,pGameStart->cbRepertoryCard,sizeof(m_cbRepertoryCard));
	}
	

	//动作处理
	if ( m_wCurrentUser==m_pIAndroidUserItem->GetChairID() )
	{
		m_bActionMask = pGameStart->cbUserAction;
		m_bActionCard = 0;
		UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_FREE);

	//删除定时器
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//设置
	UINT nElapse = rand()%TIME_START_GAME+TIME_LESS;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	return true;
}

//用户出牌
bool CAndroidUserItemSink::OnSubOutCard( const void *pBuffer,WORD wDataSize )
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;


	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	//变量定义
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wOutCardUser=pOutCard->wOutCardUser;
	ASSERT( pOutCard->cbOutCardData != 0 );
	m_cbOutCardData=pOutCard->cbOutCardData;

	//当前用户
	if(pOutCard->wOutCardUser == wMeChairID)
	{
		m_cbEnjoinCardCount=0;
		ZeroMemory(m_cbEnjoinCardData,sizeof(m_cbEnjoinCardData));
	}

	//删除扑克
	if( wMeChairID != pOutCard->wOutCardUser )
		m_GameLogic.RemoveCard(m_cbCardIndex[pOutCard->wOutCardUser],pOutCard->cbOutCardData);

	return true;
}


//用户发牌
bool CAndroidUserItemSink::OnSubSendCard( const void *pBuffer,WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//设置变量
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	m_wCurrentUser=pSendCard->wCurrentUser;

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		//丢弃扑克
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]++] = m_cbOutCardData;
		//设置变量
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//发牌处理
	if (pSendCard->cbCardData!=0)
	{
		m_cbCardIndex[m_wCurrentUser][m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;	
		//扣除扑克
		m_cbLeftCardCount--;
	}

	//设置时间
	if( m_pIAndroidUserItem->GetChairID() == m_wCurrentUser )
	{
		m_bActionMask = pSendCard->cbActionMask;
		m_bActionCard = pSendCard->cbCardData;
		//计算时间
		UINT nElapse=rand()%TIME_OPERATE_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
	}

	return true;
}


//操作通知
bool CAndroidUserItemSink::OnSubOperateNotify( const void *pBuffer,WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_OperateNotify));
	if (wDataSize!=sizeof(CMD_S_OperateNotify)) return false;

	//变量定义
	CMD_S_OperateNotify * pOperateNotify=(CMD_S_OperateNotify *)pBuffer;

	//用户界面
	if ((pOperateNotify->cbActionMask!=WIK_NULL))
	{
		ASSERT( m_wCurrentUser == INVALID_CHAIR );
		//获取变量
		WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
		m_bActionMask = pOperateNotify->cbActionMask;
		m_bActionCard = pOperateNotify->cbActionCard;

		//设置时间
		UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
	}

	return true;
}


//操作结果通知
bool CAndroidUserItemSink::OnSubOperateResult( const void *pBuffer,WORD wDataSize )
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) return false;

	//消息处理
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;

	//变量定义
	BYTE cbPublicCard=TRUE;
	WORD wOperateUser=pOperateResult->wOperateUser;
	BYTE cbOperateCard=pOperateResult->cbOperateCard;

	//删除定时器
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//出牌变量
	if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//设置变量
	m_bActionMask = WIK_NULL;
	m_bActionCard = 0;

	//
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();

	//设置组合
	if ((pOperateResult->cbOperateCode&WIK_GANG)!=0)
	{
		//设置变量
		m_wCurrentUser=INVALID_CHAIR;

		//组合扑克
		BYTE cbWeaveIndex=0xFF;
		for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)
		{
			BYTE cbWeaveKind=m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
			BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
			{
				cbWeaveIndex=i;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
				break;
			}
		}

		//组合扑克
		if (cbWeaveIndex==0xFF)
		{
			//暗杠判断
			cbPublicCard=(pOperateResult->wProvideUser==wOperateUser)?FALSE:TRUE;

			//设置扑克
			cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=cbPublicCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
		}

		//扑克设置
		m_cbCardIndex[wOperateUser][m_GameLogic.SwitchToCardIndex(pOperateResult->cbOperateCard)]=0;

		if ((wMeChairID == wOperateUser)&&(m_cbWeaveCount[wMeChairID]>2))
		{
			m_cbEnjoinCardCount=0;
			ZeroMemory(m_cbEnjoinCardData,sizeof(m_cbEnjoinCardData));
		}

	}
	else if (pOperateResult->cbOperateCode!=WIK_NULL&&(pOperateResult->cbOperateCode&WIK_CHI_HU)==0)
	{
		//设置变量
		m_wCurrentUser=pOperateResult->wOperateUser;

		//设置组合
		BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);

		//删除扑克
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(m_cbCardIndex[wOperateUser],cbWeaveCard,cbWeaveCardCount-1);

		//脱牌处理
		if ((m_cbWeaveCount[wMeChairID]>2)&&(wMeChairID == wOperateUser))
		{
			m_cbEnjoinCardCount=0;
			ZeroMemory(m_cbEnjoinCardData,sizeof(m_cbEnjoinCardData));
		}
		else if((m_cbWeaveCount[wMeChairID]<3)&&(wMeChairID == wOperateUser))
		{
			//禁止脱牌
			if(cbWeaveKind == WIK_LEFT)  //左吃
			{
				//禁止吃牌数据
				m_cbEnjoinCardData[m_cbEnjoinCardCount] =cbOperateCard;
				m_cbEnjoinCardCount++;
				if((cbOperateCard&MASK_VALUE)<7)
				{
					m_cbEnjoinCardData[m_cbEnjoinCardCount] =cbOperateCard+3;
					m_cbEnjoinCardCount++;
				}
			}
			else if (cbWeaveKind == WIK_CENTER) //中吃
			{
				m_cbEnjoinCardData[m_cbEnjoinCardCount] =cbOperateCard;
				m_cbEnjoinCardCount++;
			}
			else  if(cbWeaveKind == WIK_RIGHT)//右吃
			{
				m_cbEnjoinCardData[m_cbEnjoinCardCount] =cbOperateCard;
				m_cbEnjoinCardCount++;
				if((cbOperateCard&MASK_VALUE)>3)
				{
					m_cbEnjoinCardData[m_cbEnjoinCardCount] =cbOperateCard-3;
					m_cbEnjoinCardCount++;
				}
			}
		}
	}

	//设置时间
	if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
	{
		//计算时间
		UINT nElapse=rand()%TIME_OPERATE_CARD+TIME_LESS;
		//设置时间
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
	}

	return true;
}

//强迫出牌
bool CAndroidUserItemSink::OnSubForceOutCard(const void *pBuffer,WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard))		
		return false;

	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wOutCardUser=pOutCard->wOutCardUser;
	m_cbOutCardData=pOutCard->cbOutCardData;

	VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[m_wOutCardUser],m_cbOutCardData) );

	return true;
}

//强制发牌
bool CAndroidUserItemSink::OnSubForceSendCard(const void *pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard))		return false;

	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//设置变量
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	m_wCurrentUser=pSendCard->wCurrentUser;

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		//丢弃扑克
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]++] = m_cbOutCardData;
		//设置变量
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}
	//发牌处理
	if (pSendCard->cbCardData!=0)
	{
		m_cbCardIndex[m_wCurrentUser][m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;	
		//扣除扑克
		m_cbLeftCardCount--;
	}

	return true;
}


//出牌
void CAndroidUserItemSink::OnOutCard( BYTE cbOutCard )
{
	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	m_GameLogic.RemoveCard(m_cbCardIndex[m_pIAndroidUserItem->GetChairID()],cbOutCard);

	//设置界面
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCard;
	if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)) )
	{
		ASSERT( FALSE );
		return ;
	}

	//设置变量
	m_bActionMask = WIK_NULL;
	m_bActionCard = 0;
	
	return ;
}



//操作牌
void CAndroidUserItemSink::OnOperateCard( BYTE cbOperateCode,BYTE cbOperateCard )
{
	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	OperateCard.cbOperateCard=cbOperateCard;
	if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard)) )
	{
		ASSERT( FALSE );
		return ;
	}

	//设置变量
	m_bActionMask = WIK_NULL;
	m_bActionCard = 0;

	return ;
}


//////////////////////////////////////////////////////////////////////////
