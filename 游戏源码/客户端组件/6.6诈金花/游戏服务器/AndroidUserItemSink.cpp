#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS					1									//最少时间

//游戏时间
#define IDI_START_GAME				(IDI_ANDROID_ITEM_SINK+0)			//开始定时器
#define IDI_USER_ADD_SCORE			(IDI_ANDROID_ITEM_SINK+1)			//加注定时器
#define IDI_USER_COMPARE_CARD		(IDI_ANDROID_ITEM_SINK+2)			//选比牌用户定时器

//时间标识
#define TIME_START_GAME				3									//开始定时器
#define TIME_USER_ADD_SCORE			3									//加注定时器
#define	TIME_USER_COMPARE_CARD		3									//比牌定时器

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
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
	m_pIAndroidUserItem=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool __cdecl CAndroidUserItemSink::RepositUserItemSink()
{
	return true;
}

//时间消息
bool __cdecl CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始定时器
		{
			//发送准备
			m_pIAndroidUserItem->SendUserReady(NULL,0);

			return true;
		}
	case IDI_USER_ADD_SCORE:	//加注定时器
		{
			//放弃
			//加注
			//比牌
			//开牌
			//跟注
			//看牌

			//一为跟注
			WORD wTemp=rand()%2;

			//获取筹码
			LONG lCurrentScore=(wTemp==0)?(加注):(m_lCellScore*m_lCurrentTimes);
			WORD wMeChairID=GetMeChairID();

			//明注加倍
			if(m_bMingZhu)lCurrentScore*=2;

			//发送消息
			CMD_C_AddScore AddScore;
			AddScore.wState=0;
			AddScore.lScore=lCurrentScore;
			SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

			//自动放弃
			SendData(SUB_C_GIVE_UP,NULL,0);

			////加注信息
			//m_lCellScore=pStatusPlay->lCellScore;
			//m_lCurrentTimes=pStatusPlay->lCurrentTimes;
			//m_lUserMaxScore=pStatusPlay->lUserMaxScore;

			////设置变量
			//m_wCurrentUser=pStatusPlay->wCurrentUser;
			//m_bMingZhu=pStatusPlay->bMingZhu[wMeChairID];
			//m_lTableScore=pStatusPlay->lTableScore[wMeChairID];

			return true;
		}
	case IDI_USER_COMPARE_CARD:	//选择定时器
		{
			//构造变量
			CMD_C_CompareCard CompareCard;
			ZeroMemory(&CompareCard,sizeof(CompareCard));

			//查找上家
			WORD wMeChairID=GetMeChairID();
			for (LONG i=wMeChairID-1;;i--)
			{
				if(i==-1)i=GAME_PLAYER-1;
				if(m_cbPlayStatus[i]==TRUE)
				{
					CompareCard.wCompareUser=(WORD)i;
					break;
				}
			}

			//发送消息
			m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CompareCard));

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
			//消息处理
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_ADD_SCORE:		//用户下注
		{
			//消息处理
			return OnSubAddScore(pData,wDataSize);
		}
	case SUB_S_LOOK_CARD:		//看牌消息
		{
			//消息处理
			return OnSubLookCard(pData,wDataSize);
		}
	case SUB_S_COMPARE_CARD:	//比牌消息
		{
			//消息处理
			return OnSubCompareCard(pData,wDataSize);
		}
	case SUB_S_OPEN_CARD:		//开牌消息
		{
			//消息处理
			return OnSubOpenCard(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:			//用户放弃
		{
			//消息处理
			return OnSubGiveUp(pData,wDataSize);
		}
	case SUB_S_PLAYER_EXIT:		//用户强退
		{
			//消息处理
			return OnSubPlayerExit(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			//消息处理
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_WAIT_COMPARE:	//等待比牌
		{
			//消息处理
			if (wDataSize!=sizeof(CMD_S_WaitCompare)) return false;
			CMD_S_WaitCompare * pWaitCompare=(CMD_S_WaitCompare *)pData;	
			ASSERT(pWaitCompare->wCompareUser==m_wCurrentUser);

			//重新定时
			if(GetMeChairID()==m_wCurrentUser)
			{
				//比牌时间
				UINT nElapse=rand()%(TIME_USER_COMPARE_CARD+3)+TIME_LESS*3;
				m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD,nElapse);
			}

			return true;
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
bool __cdecl CAndroidUserItemSink::OnEventGameScene(BYTE cbGameStatus, bool bLookonOther, void * pBuffer, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//开始时间
			UINT nElapse=rand()%(TIME_USER_START_GAME+3)+TIME_LESS*3;
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

			return true;
		}
	case GS_PLAYING:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
			WORD wMeChairID=GetMeChairID();

			//加注信息
			m_lCellScore=pStatusPlay->lCellScore;
			m_lCurrentTimes=pStatusPlay->lCurrentTimes;
			m_lUserMaxScore=pStatusPlay->lUserMaxScore;

			//设置变量
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_bMingZhu=pStatusPlay->bMingZhu[wMeChairID];
			CopyMemory(m_lTableScore,pStatusPlay->lTableScore,sizeof(pStatusPlay->lTableScore));
			CopyMemory(m_cbPlayStatus,pStatusPlay->cbPlayStatus,sizeof(pStatusPlay->cbPlayStatus));

			if((!IsLookonMode()) && wMeChairID==m_wCurrentUser)
			{
				//判断控件
				if(!(pStatusPlay->bCompareState))
				{
					//设置时间
					UINT nElapse=rand()%(TIME_USER_ADD_SCORE+3)+TIME_LESS*3;
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
				}
				else 
				{
					//等待比牌
					SendData(SUB_C_WAIT_COMPARE,NULL,0);

					//设置时间
					UINT nElapse=rand()%(TIME_USER_COMPARE_CARD+3)+TIME_LESS*3;
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD,nElapse);
				}
			}
			return true;
		}
	}

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
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//数据信息
	m_lCellScore=pGameStart->lCellScore;
	m_lCurrentTimes=pGameStart->lCurrentTimes;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_lUserMaxScore=pGameStart->lUserMaxScore;

	//设置变量
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		if (m_pIAndroidUserItem->GetTableUserItem(i)!=NULL) 
		{
			//参数信息
			m_lTableScore[i]=m_lCellScore;
			m_cbPlayStatus[i]=TRUE;
		}
		else 
		{
			m_lTableScore[i]=0;
			m_cbPlayStatus[i]=0;
		}
	}

	//设置时间
	if(m_wCurrentUser==GetMeChairID())
	{
		//下注时间
		UINT nElapse=rand()%(TIME_USER_ADD_SCORE+3)+TIME_LESS*8;
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
	}

	return true;
}

//用户放弃
bool CAndroidUserItemSink::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	//设置变量
	m_cbPlayStatus[pGiveUp->wGiveUpUser]=FALSE;

	return true;
}

//用户下注
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	WORD wMeChairID=GetMeChairID();
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;

	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_AddScore));
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;
	ASSERT(pAddScore->wCurrentUser < GAME_PLAYER);
	ASSERT(pAddScore->lCurrentTimes<=m_lMaxCellScore/m_lCellScore);

	//当前用户
	m_lCurrentTimes = pAddScore->lCurrentTimes;
	m_wCurrentUser=pAddScore->wCurrentUser;
	m_lTableScore[pAddScore->wAddScoreUser]+=pAddScore->lAddScoreCount;

	//设置时间
	if(pAddScore->wCompareState==FALSE && m_wCurrentUser==wMeChairID)
	{
		//下注时间
		UINT nElapse=rand()%(TIME_USER_ADD_SCORE+3)+TIME_LESS*3;
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
	}

	return true;
}

//用户看牌
bool CAndroidUserItemSink::OnSubLookCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_LookCard)) return false;

	//重新定时
	if(m_wCurrentUser==GetMeChairID())
	{
		//下注时间
		UINT nElapse=rand()%(TIME_USER_ADD_SCORE+3)+TIME_LESS*3;
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
	}

	return true;
}

//用户比牌
bool CAndroidUserItemSink::OnSubCompareCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_CompareCard)) return false;
	CMD_S_CompareCard * pCompareCard=(CMD_S_CompareCard *)pBuffer;

	//设置变量
	m_cbPlayStatus[pCompareCard->wLostUser]=FALSE;

	//当前用户
	m_wCurrentUser=pCompareCard->wCurrentUser;

	//玩家人数
	BYTE bCount=0;
	for (WORD i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]==TRUE)bCount++;

	//判断结束
	if(bCount>1)
	{
		//控件信息
		if(!IsLookonMode() && wMeChairID==m_wCurrentUser)
		{
			//下注时间
			UINT nElapse=rand()%(TIME_USER_ADD_SCORE+3)+TIME_LESS*3;
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
		}
	}

	return true;
}

//用户开牌
bool CAndroidUserItemSink::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_OpenCard)) return false;

	return true;
}

//用户强退
bool CAndroidUserItemSink::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_PlayerExit)) return false;
	CMD_S_PlayerExit * pPlayerExit=(CMD_S_PlayerExit *)pBuffer;

	//游戏信息
	ASSERT(m_cbPlayStatus[pPlayerExit->wPlayerID]==TRUE);
	m_cbPlayStatus[pPlayerExit->wPlayerID]=FALSE;

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//开始按钮
	if (IsLookonMode()==false)
	{
		//下注时间
		UINT nElapse=rand()%(TIME_START_GAME+3)+TIME_LESS*3;
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
