#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS					3									//最少时间

//游戏时间
#define TIME_OUT_CARD				3									//出牌时间
#define TIME_START_GAME				3									//开始时间
#define TIME_OPERATE_CARD			3									//操作时间
#define TIME_HEAR_STATUS			3									//听牌时间按

//游戏时间
#define IDI_OUT_CARD				(IDI_ANDROID_ITEM_SINK+0)			//出牌时间
#define IDI_START_GAME				(IDI_ANDROID_ITEM_SINK+1)			//开始时间
#define IDI_OPERATE_CARD			(IDI_ANDROID_ITEM_SINK+2)			//操作时加

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//状态变量
	m_bHearStatus=false;
	m_bWillHearStatus=false;
	m_bActionMask = WIK_NULL;
	m_cbActionCard=0;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	m_cbSendCardData = 0;

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
	m_bWillHearStatus=false;
	m_bActionMask = WIK_NULL;
	m_cbActionCard=0;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	m_cbSendCardData = 0;

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
	case IDI_START_GAME:		//开始游戏
		{
			//开始判断
			m_pIAndroidUserItem->SendUserReady(NULL,0);
			return true;
		}
	case IDI_OPERATE_CARD:		//操作定时器
		{

			WORD wMeChair=m_pIAndroidUserItem->GetChairID();
			if((m_bActionMask!=WIK_NULL)/*&&(m_wCurrentUser!=m_pIAndroidUserItem->GetChairID())*/)
			{
				if(m_bActionMask&WIK_CHI_HU)
				{
					OnOperateCard(WIK_CHI_HU,0);
					return true;
				}
				if(m_bActionMask&WIK_GANG)
				{
					OnOperateCard(WIK_GANG,0);
					return true;
				}
	
				m_bActionMask &=~WIK_LISTEN;
				if((m_bActionMask!=WIK_NULL)&&(m_bHearStatus==false))
				{
					BYTE cbOperateCode=GetTheBestOperate(m_bActionMask,m_cbActionCard);
					ASSERT( m_cbActionCard!=0xFF);
					ASSERT(m_cbActionCard!=0);
					if(cbOperateCode==WIK_NULL)
						m_cbActionCard=0;
					//OnOperateCard(m_bActionMask,m_cbActionCard);
					OnOperateCard(cbOperateCode,m_cbActionCard);
				}

			}
			if(m_wCurrentUser==wMeChair)
			{
				if((m_bHearStatus==true)&&(m_cbSendCardData!=0))
				{
					BYTE cbCardData=m_cbSendCardData;
					//出牌效验
					if (VerdictOutCard(cbCardData)==false)
					{
						for (BYTE i=0;i<MAX_INDEX;i++)
						{
							//出牌效验
							if (m_cbCardIndex[wMeChair][i]==0) continue;
							if (VerdictOutCard(m_GameLogic.SwitchToCardData(i))==false)continue;

							//设置变量
							cbCardData=m_GameLogic.SwitchToCardData(i);
						}
					}
					//出牌动作
					OnOutCard(cbCardData);
					return true;

				}

				//智能出牌
				BYTE cbCardData=GetIsolatedCard();
				if((cbCardData!=0xFF)&&(m_bHearStatus==false))
				{
					OnOutCard(cbCardData);
					return true;
				}
				
				//异常处理
				for (INT i=MAX_INDEX-1;i>0;i--)
				{
					if((m_cbCardIndex[wMeChair][i]>0))
					{
						BYTE cbIndex=i;
						cbCardData=m_GameLogic.SwitchToCardData(i);
						if(m_bHearStatus==false)
						{
							OnOutCard(cbCardData);
							return true;
						}
					}

				}
				//出牌效验
				if (VerdictOutCard(cbCardData)==false)
				{
					for (BYTE i=0;i<MAX_INDEX;i++)
					{
						//出牌效验
						if (m_cbCardIndex[wMeChair][i]==0) continue;
						if (VerdictOutCard(m_GameLogic.SwitchToCardData(i))==false)continue;

						//设置变量
						cbCardData=m_GameLogic.SwitchToCardData(i);
					}
				}
				//出牌动作
				OnOutCard(cbCardData);
				return true;

				ASSERT(FALSE);

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
	}

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
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

			//设置数据
			m_wBankerUser=pStatusFree->wBankerUser;

			if( pIServerUserItem->GetUserStatus() != US_READY )
			{
				UINT nElapse = rand()%TIME_START_GAME+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
			}
			return true;
		}
	case GS_MJ_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbLeftCardCount=pStatusPlay->cbLeftCardCount;

			//听牌状态
			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
			m_bHearStatus=(pStatusPlay->cbHearStatus[wMeChairID]==TRUE)?true:false;

			//历史变量
			m_wOutCardUser=pStatusPlay->wOutCardUser;
			m_cbOutCardData=pStatusPlay->cbOutCardData;
			CopyMemory(m_cbDiscardCard,pStatusPlay->cbDiscardCard,sizeof(m_cbDiscardCard));
			CopyMemory(m_cbDiscardCount,pStatusPlay->cbDiscardCount,sizeof(m_cbDiscardCount));

			//扑克变量
			CopyMemory(m_cbWeaveCount,pStatusPlay->cbWeaveCount,sizeof(m_cbWeaveCount));
			CopyMemory(m_WeaveItemArray,pStatusPlay->WeaveItemArray,sizeof(m_WeaveItemArray));
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,m_cbCardIndex[m_pIAndroidUserItem->GetChairID()]);

			//出牌设置
			if ((pStatusPlay->cbActionMask!=WIK_NULL))
			{
				UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
			}
			if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID()) 
			{
				{
					UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
					m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
				}

			}

			return true;
		}
	}

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
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_PLAY);

	//设置变量
	m_bHearStatus=false;
	m_bWillHearStatus=false;
	m_wBankerUser=pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_cbLeftCardCount=MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1;
	m_bActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	//m_cbSendCardData = 0;

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//设置扑克
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	BYTE cbCardCount = (wMeChairId==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex[wMeChairId]);

	
	//动作处理
	if ((pGameStart->cbUserAction!=WIK_NULL) || pGameStart->wCurrentUser==m_pIAndroidUserItem->GetChairID() )
	{
		m_bActionMask = pGameStart->cbUserAction;
		UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,3+nElapse);
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_FREE);

	//删除定时器
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//设置
	UINT nElapse = rand()%TIME_START_GAME+TIME_LESS;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	return true;
}



//操作提示
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
		//获取变量
		WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
		m_bActionMask = pOperateNotify->cbActionMask;
		m_cbActionCard=pOperateNotify->cbActionCard;

		//设置时间
		UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
	}

	return true;
}

//
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

	//出牌变量
	if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//设置数据
	m_bActionMask = WIK_NULL;
	m_cbActionCard =0;
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

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
		if(wOperateUser==m_pIAndroidUserItem->GetChairID())
			m_cbCardIndex[wOperateUser][m_GameLogic.SwitchToCardIndex(pOperateResult->cbOperateCard)]=0;

	}
	else if (pOperateResult->cbOperateCode!=WIK_NULL&&pOperateResult->cbOperateCode!=WIK_CHI_HU)
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
		if(wOperateUser==m_pIAndroidUserItem->GetChairID())
		{
			m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
			m_GameLogic.RemoveCard(m_cbCardIndex[wOperateUser],cbWeaveCard,cbWeaveCardCount-1);
		}
	}

	//设置时间
	if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
	{
		BYTE cbChiHuRight=0;
		BYTE cbActionMask=m_GameLogic.AnalyseTingCard(m_cbCardIndex[m_pIAndroidUserItem->GetChairID()],m_WeaveItemArray[m_pIAndroidUserItem->GetChairID()],m_cbWeaveCount[m_pIAndroidUserItem->GetChairID()],cbChiHuRight);
		if((cbActionMask==WIK_LISTEN)&&(m_bHearStatus==false))
		{
			if( !m_pIAndroidUserItem->SendSocketData(SUB_C_LISTEN_CARD))
			{
				ASSERT( FALSE );
				return false;
			}

			m_bHearStatus=true;

		}
		//计算时间
		UINT nElapse=rand()%TIME_OPERATE_CARD+TIME_LESS;
		if ((m_bHearStatus==true)) 
			nElapse=rand()%TIME_HEAR_STATUS+TIME_LESS;
		//ASSERT(FALSE);

		//设置时间
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
	}

	return true;
}

//
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


	return true;
}

//
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
	if ((pSendCard->cbCardData!=0)&&((m_wCurrentUser==wMeChairID)))
	{
		m_cbCardIndex[m_wCurrentUser][m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;	
		//扣除扑克
		m_cbLeftCardCount--;
	}

	//当前用户
	if ((m_wCurrentUser==wMeChairID))
	{
		m_cbSendCardData = pSendCard->cbCardData;
		//听牌判断
		if (m_bHearStatus==false)
		{
			BYTE cbChiHuRight=0;
			BYTE cbWeaveCount=m_cbWeaveCount[wMeChairID];
			if( WIK_LISTEN==m_GameLogic.AnalyseTingCard(m_cbCardIndex[wMeChairID],m_WeaveItemArray[wMeChairID],cbWeaveCount,cbChiHuRight) )
			{
				m_bWillHearStatus = true;
			}
			else 
				m_bWillHearStatus = false;
		}
	}

	//设置时间
	if( wMeChairID == m_wCurrentUser )
	{
		m_bActionMask = pSendCard->cbActionMask;
		if( (m_bWillHearStatus == true)&&(m_bHearStatus==false) )
			m_bActionMask |= WIK_LISTEN;

		if((m_bActionMask&WIK_LISTEN)&&(m_bHearStatus==false))
		{
			if( !m_pIAndroidUserItem->SendSocketData(SUB_C_LISTEN_CARD))
			{
				ASSERT( FALSE );
				return false;
			}

			m_bHearStatus=true;

		}
		//计算时间
		UINT nElapse=rand()%TIME_OPERATE_CARD+TIME_LESS;
		if ((m_bHearStatus==true)&&(m_wCurrentUser==wMeChairID)) 
			nElapse=rand()%TIME_HEAR_STATUS+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
	}

	return true;
}
//用户听牌
bool CAndroidUserItemSink::OnSubListenCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ListenCard));
	if (wDataSize!=sizeof(CMD_S_ListenCard)) return false;

	//变量定义
	CMD_S_ListenCard * pListenCard=(CMD_S_ListenCard *)pBuffer;
	if(pListenCard->wListenUser==m_pIAndroidUserItem->GetChairID())
	{
		m_bHearStatus=true;
	}
	return true;

}

//用户出牌
void CAndroidUserItemSink::OnOutCard( BYTE cbOutCard )
{
	//听牌设置
	if (m_bWillHearStatus==true)
	{
		m_bHearStatus=true;
		m_bWillHearStatus=false;
	}

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	m_GameLogic.RemoveCard(m_cbCardIndex[wMeChairId],cbOutCard);

	//设置界面
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);
	m_cbSendCardData=0;

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCard;
	if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)) )
	{
		ASSERT( FALSE );
		return ;
	}

	//重置变量
	m_bActionMask = WIK_NULL;
	
	return ;
}

//
void CAndroidUserItemSink::OnOperateCard( BYTE cbOperateCode,BYTE cbOperateCard )
{
	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//变量定义
	tagGangCardResult GangCardResult;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));


	//提取杆牌
	if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
	{
		WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
		BYTE cbWeaveCount=m_cbWeaveCount[wMeChairID];
		m_GameLogic.AnalyseGangCard(m_cbCardIndex[wMeChairID],m_WeaveItemArray[wMeChairID],cbWeaveCount,GangCardResult);
	}
	else
	{
		GangCardResult.cbCardCount=1;
		GangCardResult.cbCardData[0]=m_cbActionCard;

	}
	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	OperateCard.cbOperateCard=(((cbOperateCard==0)&&(cbOperateCode&WIK_GANG)))?GangCardResult.cbCardData[0]:cbOperateCard;

	if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard)) )
	{
		ASSERT( FALSE );
		return ;
	}

	//重置数据
	m_bActionMask = WIK_NULL;

	return ;
}
//智能打牌
BYTE CAndroidUserItemSink::GetIsolatedCard()
{

	//定义变量
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	int i=0;
	
	//有风打风
	for( i=27; i<27+7; i++)
	{
		if(m_cbCardIndex[wMeChairID][i]==1)
		{
			return m_GameLogic.SwitchToCardData(i);
		}
	}
	//有孤牌打孤牌
	//打19孤牌
	for(i=0; i<3; i++)
	{
		if(m_cbCardIndex[wMeChairID][i*9]==1)
		{
			if((m_cbCardIndex[wMeChairID][i*9+1]==0)
				&&(m_cbCardIndex[wMeChairID][i*9+2]==0))
			{
				return m_GameLogic.SwitchToCardData(i*9);
			}
		}
		if(m_cbCardIndex[wMeChairID][i*9+8]==1)
		{
			if((m_cbCardIndex[wMeChairID][i*9+7]==0)
				&&(m_cbCardIndex[wMeChairID][i*9+6]==0))
			{
				return m_GameLogic.SwitchToCardData(i*9+8);
			}
		}
	}
	//打普通孤牌
	for( i=0; i<27; i++)
	{
		if(m_cbCardIndex[wMeChairID][i]==1)
		{
			BYTE cbTmp = i%9;
			switch(cbTmp)
			{
			case 0:
				if((m_cbCardIndex[wMeChairID][i+1]==0)
					&& (m_cbCardIndex[wMeChairID][i+2]==0))
				{
					return m_GameLogic.SwitchToCardData(i);
				}
				break;
			case 1:
				if((m_cbCardIndex[wMeChairID][i+1]==0)
					&&(m_cbCardIndex[wMeChairID][i+2]==0)
					&&(m_cbCardIndex[wMeChairID][i-1]==0))
				{
					return m_GameLogic.SwitchToCardData(i);
				}
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				if((m_cbCardIndex[wMeChairID][i+1]==0)
					&&(m_cbCardIndex[wMeChairID][i+2]==0)
					&&(m_cbCardIndex[wMeChairID][i-1]==0)
					&& (m_cbCardIndex[wMeChairID][i-2]==0))
				{
					return m_GameLogic.SwitchToCardData(i);
				}
				break;
			case 7:
				if((m_cbCardIndex[wMeChairID][i-1]==0)
					&&(m_cbCardIndex[m_pIAndroidUserItem->GetChairID()][i-2]==0)
					&&(m_cbCardIndex[m_pIAndroidUserItem->GetChairID()][i+1]==0))
				{
					return m_GameLogic.SwitchToCardData(i);
				}
				break;
			case 8:
				if((m_cbCardIndex[wMeChairID][i-1]==0)
					&&(m_cbCardIndex[m_pIAndroidUserItem->GetChairID()][i-2]==0))
				{
					return m_GameLogic.SwitchToCardData(i);
				}
				break;
			}
			if(cbTmp < 3)
			{
				i += 5;
			}
			else
			{
				i = (i/9+1)*9 - 1;
			}
		}	
	}

	//打卡张
	//打19卡张
	for(i=0; i<3; i++)
	{
		if(m_cbCardIndex[wMeChairID][i*9] == 1)
		{
			if(m_cbCardIndex[wMeChairID][i*9+1] == 0
				|| m_cbCardIndex[wMeChairID][i*9+2] == 0)
			{
				return m_GameLogic.SwitchToCardData(i*9);
			}
		}
		if(m_cbCardIndex[wMeChairID][i*9+8] == 1)
		{
			if(m_cbCardIndex[wMeChairID][i*9+7] == 0
				|| m_cbCardIndex[wMeChairID][i*9+6] == 0)
			{
				return m_GameLogic.SwitchToCardData(i*9+8);
			}
		}
	}

	//打普通卡张
	for(i=0; i<27; i++)
	{
		if(m_cbCardIndex[wMeChairID][i] == 1)
		{
			if((i%9) == 0)
			{
				i += 3;
				continue;
			}
			if((i+1)%9 == 0)
			{
				continue;
			}
			if(m_cbCardIndex[wMeChairID][i+1] == 0
				&& m_cbCardIndex[wMeChairID][i-1] == 0)
			{
				return m_GameLogic.SwitchToCardData(i);
			}
			i += 1;
		}	
	}


	return  0XFF;
}
//孤牌数目
BYTE CAndroidUserItemSink::GetIsolatedCardCount(BYTE cbCardIndex[MAX_INDEX])
{
	//临时数据
	BYTE cbTempCardIndex[MAX_INDEX];
	CopyMemory(cbTempCardIndex,cbCardIndex,sizeof(cbTempCardIndex));

	//定义变量
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	int i=0;
	int iIsolateCardCount=0;

	//有风打风
	for( i=27; i<27+7; i++)
	{
		if(cbTempCardIndex[i]==1)
		{
			iIsolateCardCount++;
		}
	}
	//有孤牌打孤牌
	//打19孤牌
	for(i=0; i<3; i++)
	{
		if(cbTempCardIndex[i*9]==1)
		{
			if((cbTempCardIndex[i*9+1]==0)
				&&(cbTempCardIndex[i*9+2]==0))
			{
				iIsolateCardCount++;
			}
		}
		if(cbTempCardIndex[i*9+8]==1)
		{
			if((cbTempCardIndex[i*9+7]==0)
				&&(cbTempCardIndex[i*9+6]==0))
			{
				iIsolateCardCount++;
			}
		}
	}
	//打普通孤牌
	for( i=0; i<27; i++)
	{
		if(cbTempCardIndex[i]==1)
		{
			BYTE cbTmp = i%9;
			switch(cbTmp)
			{
			case 0:
				if((cbTempCardIndex[i+1]==0)
					&& (cbTempCardIndex[i+2]==0))
				{
					iIsolateCardCount++;
				}
				break;
			case 1:
				if((cbTempCardIndex[i+1]==0)
					&&(cbTempCardIndex[i+2]==0)
					&&(cbTempCardIndex[i-1]==0))
				{
					iIsolateCardCount++;
				}
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				if((cbTempCardIndex[i+1]==0)
					&&(cbTempCardIndex[i+2]==0)
					&&(cbTempCardIndex[i-1]==0)
					&& (cbTempCardIndex[i-2]==0))
				{
					iIsolateCardCount++;
				}
				break;
			case 7:
				if((cbTempCardIndex[i-1]==0)
					&&(cbTempCardIndex[i-2]==0)
					&&(cbTempCardIndex[i+1]==0))
				{
					iIsolateCardCount++;
				}
				break;
			case 8:
				if((cbTempCardIndex[i-1]==0)
					&&(cbTempCardIndex[i-2]==0))
				{
					iIsolateCardCount++;
				}
				break;
			}
			if(cbTmp < 3)
			{
				i += 5;
			}
			else
			{
				i = (i/9+1)*9 - 1;
			}
		}	
	}

	//打卡张
	//打19卡张
	for(i=0; i<3; i++)
	{
		if(cbTempCardIndex[i*9] == 1)
		{
			if(cbTempCardIndex[i*9+1] == 0
				|| cbTempCardIndex[i*9+2] == 0)
			{
				iIsolateCardCount++;
				
			}
		}
		if(cbTempCardIndex[i*9+8] == 1)
		{
			if(cbTempCardIndex[i*9+7] == 0
				|| cbTempCardIndex[i*9+6] == 0)
			{
				iIsolateCardCount++;
			}
		}
	}

	//打普通卡张
	for(i=0; i<27; i++)
	{
		if(cbTempCardIndex[i] == 1)
		{
			if((i%9) == 0)
			{
				i += 3;
				continue;
			}
			if((i+1)%9 == 0)
			{
				continue;
			}
			if(cbTempCardIndex[i+1] == 0
				&& cbTempCardIndex[i-1] == 0)
			{
				iIsolateCardCount++;
			}
			i += 1;
		}	
	}

	return  iIsolateCardCount;

}
BYTE CAndroidUserItemSink::GetTheBestOperate(BYTE cbOperateCode,BYTE cbOperateCard)
{
	//定义变量
	BYTE cbPreIsolateCardCount=GetIsolatedCardCount(m_cbCardIndex[m_pIAndroidUserItem->GetChairID()]);
	BYTE cbPengIsolateCardCount=0XFF;
	BYTE cbLeftIsolateCardCount=0XFF;
	BYTE cbCenterIsolateCradCount=0XFF;
	BYTE cbRightIsolateCardCount=0XFF;

	//碰牌操作
	if(cbOperateCode&WIK_PENG)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[m_pIAndroidUserItem->GetChairID()],sizeof(cbTempCardIndex));

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=WIK_PENG;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbPengIsolateCardCount=GetIsolatedCardCount(cbTempCardIndex);
	}
	//左吃操作
	if(cbOperateCode&WIK_LEFT)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[m_pIAndroidUserItem->GetChairID()],sizeof(cbTempCardIndex));

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=WIK_LEFT;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbLeftIsolateCardCount=GetIsolatedCardCount(cbTempCardIndex);
	}
	//中吃操作
	if(cbOperateCode&WIK_CENTER)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[m_pIAndroidUserItem->GetChairID()],sizeof(cbTempCardIndex));

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=WIK_CENTER;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbCenterIsolateCradCount=GetIsolatedCardCount(cbTempCardIndex);
	}
	//右吃操作
	if(cbOperateCode&WIK_RIGHT)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[m_pIAndroidUserItem->GetChairID()],sizeof(cbTempCardIndex));

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=WIK_RIGHT;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbRightIsolateCardCount=GetIsolatedCardCount(cbTempCardIndex);
	}
	//有碰就碰
	if(cbPengIsolateCardCount==0)
		return WIK_PENG;
	if(cbLeftIsolateCardCount==0)
		return WIK_LEFT;
	if(cbCenterIsolateCradCount==0)
		return WIK_CENTER;
	if(cbRightIsolateCardCount==0)
		return WIK_RIGHT;
	
	//调整权位
	BYTE cbMin=0xFF;
	if(cbMin>cbPengIsolateCardCount)
		cbMin=cbPengIsolateCardCount;
	if(cbMin>cbLeftIsolateCardCount)
		cbMin=cbLeftIsolateCardCount;
	if(cbMin>cbCenterIsolateCradCount)
		cbMin=cbCenterIsolateCradCount;
	if(cbMin>cbRightIsolateCardCount)
		cbMin=cbRightIsolateCardCount;
	ASSERT(cbMin!=0xFF);
	if(cbMin>cbPreIsolateCardCount)
		return WIK_NULL;
	if(cbMin==cbPengIsolateCardCount)
		return WIK_PENG;
	if(cbMin==cbLeftIsolateCardCount)
		return WIK_LEFT;
	if(cbMin==cbCenterIsolateCradCount)
		return WIK_CENTER;
	if(cbMin==cbRightIsolateCardCount)
		return WIK_RIGHT;

	//最优权位
	if((cbOperateCode&WIK_PENG)&&(cbPreIsolateCardCount-1==cbPengIsolateCardCount))
		return WIK_PENG;
	if((cbOperateCode&WIK_LEFT)&&(cbPreIsolateCardCount-1==cbLeftIsolateCardCount))
		return WIK_LEFT;
	if((cbOperateCode&WIK_CENTER)&&(cbPreIsolateCardCount-1==cbCenterIsolateCradCount))
		return WIK_CENTER;
	if((cbOperateCode&WIK_RIGHT)&&(cbPreIsolateCardCount-1==cbRightIsolateCardCount))
		return WIK_RIGHT;
	return WIK_NULL;
}
//出牌判断
bool CAndroidUserItemSink::VerdictOutCard(BYTE cbCardData)
{
	//听牌判断
	if ((m_bHearStatus==true))
	{
		//变量定义
		tagChiHuResult ChiHuResult;
		WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
		BYTE cbWeaveCount=m_cbWeaveCount[wMeChairID];

		//构造扑克
		BYTE cbCardIndexTemp[MAX_INDEX];
		CopyMemory(cbCardIndexTemp,m_cbCardIndex[wMeChairID],sizeof(cbCardIndexTemp));

		//删除扑克
		m_GameLogic.RemoveCard(cbCardIndexTemp,cbCardData);

		//听牌判断
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			//胡牌分析
			BYTE wChiHuRight=0;
			BYTE cbCurrentCard=m_GameLogic.SwitchToCardData(i);
			BYTE cbHuCardKind=m_GameLogic.AnalyseChiHuCard(cbCardIndexTemp,m_WeaveItemArray[wMeChairID],cbWeaveCount,cbCurrentCard,wChiHuRight,ChiHuResult);

			//结果判断
			if (cbHuCardKind!=CHK_NULL) 
				break;
		}

		//听牌判断
		return (i!=MAX_INDEX);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
