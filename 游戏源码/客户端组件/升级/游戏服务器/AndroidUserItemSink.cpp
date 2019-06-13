#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//test
//#define TEST						

#ifndef	TEST
//辅助时间
#define TIME_LESS_OUTCARD			1									//出牌最少时间
#define TIME_LESS_STARTGAME			1									//开始最少时间
#define TIME_LESS_CALLCARD			2									//叫牌最少时间
#define TIME_LESS_SENDCONCEAL		5									//设底牌最少时间
#define TIME_LESS_CALLFINISH		1									//完成叫牌最少时间
//游戏时间
#define TIME_START_GAME				8									//开始时间
#define TIME_CALL_CARD				10									//叫牌时间
#define TIME_CALL_FINISH			5									//完成叫牌时间
#define TIME_SEND_CONCEAL			12									//设底牌时间
#define TIME_OUT_CARD				5									//出牌时间
#define TIME_RESPONSE_LEAVE			4									//回应离开请求
#else
//辅助时间
#define TIME_LESS_OUTCARD			1									//出牌最少时间
#define TIME_LESS_STARTGAME			1									//开始最少时间
#define TIME_LESS_CALLCARD			2									//叫牌最少时间
#define TIME_LESS_SENDCONCEAL		5									//设底牌最少时间
#define TIME_LESS_CALLFINISH		2									//完成叫牌最少时间
//游戏时间
#define TIME_START_GAME				1									//开始时间
#define TIME_CALL_CARD				1									//叫牌时间
#define TIME_CALL_FINISH			1									//完成叫牌时间
#define TIME_SEND_CONCEAL			1									//设底牌时间
#define TIME_OUT_CARD				1									//出牌时间
#define TIME_RESPONSE_LEAVE			4									//回应离开请求
#endif

//游戏时间
#define IDI_START_GAME				(IDI_ANDROID_ITEM_SINK+1)			//开始时间
#define IDI_CALL_CARD				(IDI_ANDROID_ITEM_SINK+2)			//叫牌时间
#define IDI_SEND_CONCEAL			(IDI_ANDROID_ITEM_SINK+3)			//垫底牌时间
#define IDI_OUT_CARD				(IDI_ANDROID_ITEM_SINK+4)			//出牌时间
#define IDI_CALL_FINISH				(IDI_ANDROID_ITEM_SINK+5)			//叫牌时间
#define IDI_RESPONSE_LEAVE			(IDI_ANDROID_ITEM_SINK+6)			//回应离开请求

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//逻辑变量
	m_cbPackCount=0;
	m_cbMainColor=COLOR_ERROR;
	m_cbMainValue=VALUE_ERROR;

	//叫牌信息
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	
	//得分信息
	m_cbScoreCardCount=0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	//出牌变量
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

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
	//逻辑变量
	m_cbPackCount=0;
	m_cbMainColor=COLOR_ERROR;
	m_cbMainValue=VALUE_ERROR;

	//叫牌信息
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	
	//得分信息
	m_cbScoreCardCount=0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	//出牌变量
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//删除定时器
	m_pIAndroidUserItem->KillGameTimer(0);

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
	case IDI_CALL_CARD:		//叫牌
		{
			VERIFY( CallCard() );
			return true;
		}
	case IDI_CALL_FINISH:	//完成叫牌
		{
			m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_FINISH);
			return true;
		}
	case IDI_SEND_CONCEAL:	//底牌
		{
			VERIFY( SendConceal() );
			return true;
		}	
	case IDI_OUT_CARD:		//出牌
		{
			VERIFY( OutCard() );
			return true;
		}
	case IDI_RESPONSE_LEAVE:	//用户请求离开
		{
			for( INT_PTR i = 0; i < m_RequestUserAry.GetCount(); i++ )
			{
				//随机判断是否同意
				BOOL bAgree = (rand()%2==0)?TRUE:FALSE;

				//变量定义
				CMD_C_ResponsesLeave ResponsesLeave;
				ZeroMemory(&ResponsesLeave,sizeof(ResponsesLeave));
				//构造消息
				ResponsesLeave.wChairID=m_RequestUserAry[i];
				ResponsesLeave.cbAgreeLeave=bAgree;

				//发送消息
				m_pIAndroidUserItem->SendSocketData(SUB_C_RESPONSES_LEAVE,&ResponsesLeave,sizeof(ResponsesLeave));

				//删除用户
				m_RequestUserAry.RemoveAt(i);
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
	case SUB_S_SEND_CARD:		//发送扑克
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_CALL_CARD:		//用户叫牌
		{
			return OnSubCallCard(pData,wDataSize);
		}
	case SUB_S_SEND_CONCEAL:	//发送底牌
		{
			return OnSubSendConceal(pData,wDataSize);
		}
	case SUB_S_GAME_PLAY:		//游戏开始
		{
			return OnSubGamePlay(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_THROW_RESULT:	//甩牌结果
		{
			return OnSubThrowResult(pData,wDataSize);
		}
	case SUB_S_TURN_BALANCE:	//一轮结算
		{
			return OnSubTurnBalance(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_STRUSTEE:
		{
			return true;
		}
	case SUB_S_REQUEST_LEAVE:	//请求离开
		{
			return OnSubRequestLeave(pData,wDataSize);
		}
	case SUB_S_BATCH_CARD:		//批量出牌
		{
			return OnSubBatchCard(pData,wDataSize);
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
	case GS_UG_FREE:	//空闲状态
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
				UINT nElapse=rand()%TIME_START_GAME+TIME_LESS_STARTGAME;
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
			}

			return true;
		}
	case GS_UG_CALL:
	case GS_UG_BACK:
	case GS_UG_PLAY:		//游戏状态
		{
			ASSERT( FALSE );
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

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_UG_CALL);

	//逻辑变量
	m_cbMainColor=COLOR_ERROR;
	m_cbMainValue=pGameStart->cbMainValue;
	m_cbPackCount=pGameStart->cbPackCount;

	//叫牌变量
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;

	//底牌变量
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//状态变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=pGameStart->wBankerUser;

	//得分信息
	m_cbScoreCardCount=0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	//出牌变量
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//逻辑组件
	m_GameLogic.SetPackCount(m_cbPackCount);
	m_GameLogic.SetMainColor(m_cbMainColor);
	m_GameLogic.SetMainValue(m_cbMainValue);

	return true;
}

//发送扑克
bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;
	WORD wFirstSize=sizeof(CMD_S_SendCard)-sizeof(pSendCard->cbCardData);

	//效验数据
	ASSERT(wDataSize>=wFirstSize);
	if (wDataSize<wFirstSize) return false;
	ASSERT(wDataSize==(wFirstSize+pSendCard->cbCardCount*sizeof(BYTE)));
	if (wDataSize!=(wFirstSize+pSendCard->cbCardCount*sizeof(BYTE))) return false;

	//设置扑克
	m_cbHandCardCount=pSendCard->cbCardCount;
	CopyMemory(m_cbHandCardData,pSendCard->cbCardData,sizeof(BYTE)*m_cbHandCardCount);

	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);

	//叫牌定时
	UINT nElapse = TIME_LESS_CALLCARD+rand()%TIME_CALL_CARD;
	m_pIAndroidUserItem->SetGameTimer(IDI_CALL_CARD,nElapse);

	return true;
}

//用户叫牌
bool CAndroidUserItemSink::OnSubCallCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_CallCard));
	if (wDataSize!=sizeof(CMD_S_CallCard)) return false;

	//消息处理
	CMD_S_CallCard * pCallCard=(CMD_S_CallCard *)pBuffer;

	//设置变量
	m_cbCallCard=pCallCard->cbCallCard;
	m_cbCallCount=pCallCard->cbCallCount;
	m_wCallCardUser=pCallCard->wCallCardUser;

	return true;
}

//发送底牌
bool CAndroidUserItemSink::OnSubSendConceal(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendConceal));
	if (wDataSize!=sizeof(CMD_S_SendConceal)) return false;

	//消息处理
	CMD_S_SendConceal * pSendConceal=(CMD_S_SendConceal *)pBuffer;

	//设置变量
	m_cbMainColor=pSendConceal->cbMainColor;
	m_wBankerUser=pSendConceal->wBankerUser;
	m_wCurrentUser=pSendConceal->wCurrentUser;

	//状态设置
	m_pIAndroidUserItem->SetGameStatus(GS_UG_BACK);
	m_GameLogic.SetMainColor(m_cbMainColor);

	//扑克设置
	if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
	{
		//设置底牌
		m_cbConcealCount=pSendConceal->cbConcealCount;
		CopyMemory(m_cbConcealCard,pSendConceal->cbConcealCard,m_cbConcealCount*sizeof(BYTE));

		//设置扑克
		CopyMemory(&m_cbHandCardData[m_cbHandCardCount],m_cbConcealCard,m_cbConcealCount*sizeof(BYTE));
		m_cbHandCardCount+=m_cbConcealCount;

		//设置扑克
		m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);

		//设置定时
		UINT nElapse = TIME_LESS_SENDCONCEAL + rand()%TIME_SEND_CONCEAL;
		m_pIAndroidUserItem->SetGameTimer(IDI_SEND_CONCEAL,nElapse);
	}
	else
	{
		//设置扑克
		m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);
	}

	return true;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGamePlay(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GamePlay));
	if (wDataSize!=sizeof(CMD_S_GamePlay)) return false;

	//消息处理
	CMD_S_GamePlay * pGamePlay=(CMD_S_GamePlay *)pBuffer;

	//设置变量
	m_wCurrentUser=pGamePlay->wCurrentUser;
	m_wFirstOutUser=pGamePlay->wCurrentUser;
	m_cbConcealCount=pGamePlay->cbConcealCount;
	CopyMemory(m_cbConcealCard,pGamePlay->cbConcealCard,m_cbConcealCount*sizeof(BYTE));

	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_UG_PLAY);

	//当前玩家
	WORD wMeChairid = m_pIAndroidUserItem->GetChairID();
	if (m_wCurrentUser==wMeChairid)
	{
		UINT uElapse = TIME_LESS_OUTCARD + rand()%TIME_OUT_CARD;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,uElapse);
	}

	return true;
}

//用户出牌
bool CAndroidUserItemSink::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	WORD wFirstSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);

	//效验数据
	ASSERT(wDataSize>=wFirstSize);
	if (wDataSize<wFirstSize) return false;
	ASSERT(wDataSize==(wFirstSize+pOutCard->cbCardCount*sizeof(BYTE)));
	if (wDataSize!=(wFirstSize+pOutCard->cbCardCount*sizeof(BYTE))) return false;

	//变量定义
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	WORD wOutCardUser=pOutCard->wOutCardUser;

	//出牌记录
	m_cbOutCardCount[wOutCardUser]=pOutCard->cbCardCount;
	CopyMemory(m_cbOutCardData[wOutCardUser],pOutCard->cbCardData,pOutCard->cbCardCount*sizeof(BYTE));

	//设置变量
	m_wCurrentUser=pOutCard->wCurrentUser;

	//玩家设置
	if (m_wCurrentUser==wMeChairID)
	{
		UINT uElapse = TIME_LESS_OUTCARD + rand()%TIME_OUT_CARD;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,uElapse);
	}

	return true;
}

//甩牌结果
bool CAndroidUserItemSink::OnSubThrowResult(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_ThrowResult * pThrowResult=(CMD_S_ThrowResult *)pBuffer;
	WORD wCardCount=pThrowResult->cbThrowCardCount+pThrowResult->cbResultCardCount;
	WORD wFirstSize=sizeof(CMD_S_ThrowResult)-sizeof(pThrowResult->cbCardDataArray);

	//效验数据
	ASSERT(wDataSize>=wFirstSize);
	if (wDataSize<wFirstSize) return false;
	ASSERT(wDataSize==(wFirstSize+wCardCount*sizeof(BYTE)));
	if (wDataSize!=(wFirstSize+wCardCount*sizeof(BYTE))) return false;

	//变量定义
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	WORD wOutCardUser=pThrowResult->wOutCardUser;

	//扑克变量
	BYTE cbThrowCardCount=pThrowResult->cbThrowCardCount;
	BYTE cbResultCardCount=pThrowResult->cbResultCardCount;
	BYTE * pcbThrowCardData=&pThrowResult->cbCardDataArray[0];
	BYTE * pcbResultCardData=&pThrowResult->cbCardDataArray[pThrowResult->cbThrowCardCount];

	//出牌记录
	m_cbOutCardCount[wOutCardUser]=cbResultCardCount;
	CopyMemory(m_cbOutCardData[wOutCardUser],pcbResultCardData,cbResultCardCount*sizeof(BYTE));

	//玩家扑克
	if (wOutCardUser==wMeChairID)
	{
		//设置扑克
		BYTE cbSourceCount=m_cbHandCardCount;
		m_cbHandCardCount+=cbThrowCardCount;
		CopyMemory(&m_cbHandCardData[cbSourceCount],pcbThrowCardData,sizeof(BYTE)*cbThrowCardCount);

		//排列扑克
		m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);

		//删除扑克
		cbSourceCount=m_cbHandCardCount;
		m_cbHandCardCount-=cbResultCardCount;
		m_GameLogic.RemoveCard(pcbResultCardData,cbResultCardCount,m_cbHandCardData,cbSourceCount);
	}

	//设置变量
	m_wCurrentUser=pThrowResult->wCurrentUser;

	//设置时间
	if( m_wCurrentUser == wMeChairID )
	{
		UINT uElapse = TIME_LESS_OUTCARD + rand()%TIME_OUT_CARD;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,uElapse);
	}

	return true;
}

//一轮结算
bool CAndroidUserItemSink::OnSubTurnBalance(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_TurnBalance));
	if (wDataSize!=sizeof(CMD_TurnBalance)) return false;

	//消息处理
	CMD_TurnBalance * pTurnBalance=(CMD_TurnBalance *)pBuffer;

	//计算得分
	WORD wWinnerUser=pTurnBalance->wTurnWinner;
	if ((wWinnerUser!=m_wBankerUser)&&((wWinnerUser+2)%GAME_PLAYER!=m_wBankerUser))
	{
		//获取分牌
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			BYTE * pcbScoreCard=&m_cbScoreCardData[m_cbScoreCardCount];
			BYTE cbMaxCount=CountArray(m_cbScoreCardData)-m_cbScoreCardCount;
			m_cbScoreCardCount+=m_GameLogic.GetScoreCard(m_cbOutCardData[i],m_cbOutCardCount[i],pcbScoreCard,cbMaxCount);
		}
	}

	//设置用户
	m_wCurrentUser=pTurnBalance->wCurrentUser;
	m_wFirstOutUser=pTurnBalance->wCurrentUser;

	//玩家设置
	if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
	{
		UINT uElapse = TIME_LESS_OUTCARD + rand()%TIME_OUT_CARD;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,uElapse);
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//游戏定时器
	m_pIAndroidUserItem->KillGameTimer(0);

	UINT uElapse = TIME_LESS_STARTGAME + rand()%TIME_START_GAME;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,uElapse);

	return true;
}

//请求离开
bool CAndroidUserItemSink::OnSubRequestLeave(const void *pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RequestLeave));
	if (wDataSize!=sizeof(CMD_S_RequestLeave)) return false;

	//变量定义
	CMD_S_RequestLeave * pRequestLeave=(CMD_S_RequestLeave *)pBuffer;

	m_RequestUserAry.Add(pRequestLeave->wChairID);

	UINT nElapse = 1+rand()%TIME_RESPONSE_LEAVE;
	m_pIAndroidUserItem->SetGameTimer(IDI_RESPONSE_LEAVE,nElapse);

	return true;
}

//叫牌
bool CAndroidUserItemSink::CallCard()
{
	//随机叫牌
	bool bCallCard = rand()%2 == 0 ? true:false;
	if( bCallCard )
	{
		//辅助变量
		BYTE cbCallColor=m_GameLogic.GetCardColor(m_cbCallCard);
		BYTE cbCardCount[6]={0,0,0,0,0,0};
		BYTE cbCallColorArray[6]={COLOR_FANG_KUAI,COLOR_MEI_HUA,COLOR_HONG_TAO,COLOR_HEI_TAO,COLOR_NT,COLOR_NT};
		BYTE cbColorCount[6] = {0,0,0,0,0,0};
		//扑克统计
		for (BYTE i=0;i<m_cbHandCardCount;i++)
		{
			//获取属性
			BYTE cbCardColor=m_GameLogic.GetCardColor(m_cbHandCardData[i]);
			BYTE cbCardValue=m_GameLogic.GetCardValue(m_cbHandCardData[i]);

			//数据统计
			if (cbCardColor==COLOR_NT) 
			{
				cbCardCount[4+cbCardValue-0x0E]++;
				cbColorCount[4+cbCardValue-0x0E]++;
			}
			else if (cbCardValue==m_cbMainValue)
			{
				cbCardCount[cbCardColor>>4]++;
				cbColorCount[cbCardColor>>4]++;
			}
		}
		//主牌控制
		for (BYTE i=0;i<CountArray(cbCardCount);i++)
		{
			//规则判断
			if ( cbCardCount[i]==0 ) continue;

			//数目判断
			if ((i<4)&&(cbCardCount[i]>m_cbCallCount))
				continue;

			//无主判断
			if ((i>=4)&&(cbCardCount[i]>=2)&&(cbCardCount[i]>m_cbCallCount||
				(m_cbCallCard&LOGIC_MASK_VALUE)==m_cbMainValue||i==5))
				continue;

			////花色判断
			//BYTE cbPackCount=m_GameLogic.GetPackCount();
			//if ((cbCardCount[i]==cbPackCount)&&(cbCallColorArray[i]>cbCallColor))
			//	continue;

			cbCardCount[i] = 0;
		}
		//搜索最大主牌
		BYTE byIndex = 0;
		for( BYTE i = 1; i < CountArray(cbCardCount); i++ )
		{
			if( cbCardCount[i] == 0 ) continue;

			if( cbColorCount[i] > cbColorCount[byIndex] )
				byIndex = i;
		}
		if( cbCardCount[byIndex] > 0 )
		{
			BYTE cbCallCard;
			if( byIndex == 4 ) cbCallCard = 0x4E;
			else if( byIndex == 5 ) cbCallCard = 0x4F;
			else cbCallCard = m_cbMainValue|cbCallColorArray[byIndex];

			//构造数据
			CMD_C_CallCard CallCard;
			CallCard.cbCallCard=cbCallCard;
			CallCard.cbCallCount=cbCardCount[byIndex];

			//发送数据
			m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_CARD,&CallCard,sizeof(CallCard));
		}
	}
	UINT nElapse = TIME_LESS_CALLFINISH + rand()%TIME_CALL_FINISH;
	m_pIAndroidUserItem->SetGameTimer(IDI_CALL_FINISH,nElapse);
	return true;
}

//下底牌
bool CAndroidUserItemSink::SendConceal()
{
	//删除扑克
	m_cbHandCardCount-=m_cbConcealCount;
	VERIFY( m_GameLogic.RemoveCard(m_cbConcealCard,m_cbConcealCount,m_cbHandCardData,m_cbHandCardCount+m_cbConcealCount) );

	//构造数据
	CMD_C_ConcealCard ConcealCard;
	ConcealCard.cbConcealCount=m_cbConcealCount;
	CopyMemory(ConcealCard.cbConcealCard,m_cbConcealCard,m_cbConcealCount*sizeof(BYTE));

	//发送数据
	m_pIAndroidUserItem->SendSocketData(SUB_C_CONCEAL_CARD,&ConcealCard,sizeof(ConcealCard));
	return true;
}

//出牌
bool CAndroidUserItemSink::OutCard()
{
	//变量定义
	tagOutCardResult OutCardResult;
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//搜索出牌
	BYTE cbTurnCardCount=(m_wCurrentUser==m_wFirstOutUser)?0:m_cbOutCardCount[m_wFirstOutUser];
	m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount,m_cbOutCardData[m_wFirstOutUser],cbTurnCardCount,OutCardResult);
	ASSERT( m_wFirstOutUser==m_pIAndroidUserItem->GetChairID() ||
		m_GameLogic.EfficacyOutCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,m_cbOutCardData[m_wFirstOutUser],
		m_cbOutCardCount[m_wFirstOutUser],m_cbHandCardData,m_cbHandCardCount) );

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardCount=OutCardResult.cbCardCount;
	CopyMemory(OutCard.cbCardData,OutCardResult.cbResultCard,sizeof(BYTE)*OutCard.cbCardCount);
	m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)-sizeof(OutCard.cbCardData)+OutCard.cbCardCount*sizeof(BYTE));

	//预先删除
	BYTE cbSourceCount=m_cbHandCardCount;
	m_cbHandCardCount-=OutCard.cbCardCount;
	VERIFY( m_GameLogic.RemoveCard(OutCard.cbCardData,OutCard.cbCardCount,m_cbHandCardData,cbSourceCount) );

	return true;
}

//批量出牌
bool CAndroidUserItemSink::OnSubBatchCard(const void *pBuffer,WORD wDataSize)
{
	//变量定义
	CMD_S_BatchCard * pBatchCard=(CMD_S_BatchCard *)pBuffer;
	WORD wFirstSize=sizeof(CMD_S_BatchCard)-sizeof(pBatchCard->cbCardData);

	//效验数据
	ASSERT(wDataSize>=wFirstSize);
	if (wDataSize<wFirstSize) return false;
	ASSERT(wDataSize==(wFirstSize+pBatchCard->cbCardCount*GAME_PLAYER*sizeof(BYTE)));
	if (wDataSize!=(wFirstSize+pBatchCard->cbCardCount*GAME_PLAYER*sizeof(BYTE))) return false;

	//消息处理
	BYTE cbCardCount=pBatchCard->cbCardCount;
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		//历史扑克
		m_cbOutCardCount[i]=cbCardCount;
		CopyMemory(m_cbOutCardData[i],&pBatchCard->cbCardData[i*cbCardCount],cbCardCount*sizeof(BYTE));
	}

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;

	return true;
}

//////////////////////////////////////////////////////////////////////////
