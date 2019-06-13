#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define IDI_TIME_GIVE_UP						1							//放弃定时器

//静态变量
const BYTE	CGameLogic::m_bCardArray[52]=									//扑克数组
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
};

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_AllReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//排列扑克
void CGameLogic::SortCard(BYTE bCardData[], BYTE bCardCount)
{
	//转换数值
	BYTE bLogicVolue[2];
	for (BYTE i=0;i<bCardCount;i++)	bLogicVolue[i]=GetCardLogicValue(bCardData[i]);

	//排序操作
	bool bSorted=true;
	BYTE bTempData,bLast=bCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((bLogicVolue[i]<bLogicVolue[i+1])||
				((bLogicVolue[i]==bLogicVolue[i+1])&&(bCardData[i]<bCardData[i+1])))
			{
				//交换位置
				bTempData=bCardData[i];
				bCardData[i]=bCardData[i+1];
				bCardData[i+1]=bTempData;
				bTempData=bLogicVolue[i];
				bLogicVolue[i]=bLogicVolue[i+1];
				bLogicVolue[i+1]=bTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);
	return;
}

//判断对子
BYTE CGameLogic::GetCardType(BYTE bCardList[], BYTE bCardCount)
{
	BYTE Value1=GetCardValue(bCardList[0]);
	BYTE Value2=GetCardValue(bCardList[1]);

	if(Value1==Value2) return CT_DOUBLE;
	if( Value1>10&&Value1!=21&&Value2>10&&Value2!=21 ) return CT_COLOR;
	return CT_SINGLE;
}
BYTE CGameLogic::GetMaxValue(BYTE bCardList[])
{
	BYTE Value1=GetCardValue(bCardList[0]);
	BYTE Value2=GetCardValue(bCardList[1]);
	return (Value1>Value2) ? Value1 : Value2;
}
BYTE CGameLogic::GetMinValue(BYTE bCardList[])
{
	BYTE Value1=GetCardValue(bCardList[0]);
	BYTE Value2=GetCardValue(bCardList[1]);
	return (Value1>Value2) ? Value2 : Value1;
}
BYTE CGameLogic::GetMaxColor(BYTE bCardList[])
{
	BYTE Value1=GetCardValue(bCardList[0]);
	BYTE Value2=GetCardValue(bCardList[1]);
	return (Value1>Value2) ? GetCardColor(bCardList[0]) : GetCardColor(bCardList[1]);
}
//对比扑克
bool CGameLogic::CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE bCardCount)
{
	//获取类型
	BYTE bNextType=GetCardType(bNextList,bCardCount);
	BYTE bFirstType=GetCardType(bFirstList,bCardCount);
	//两手牌的最大值
	BYTE FirstMaxValue=GetMaxValue(bFirstList);
	BYTE NextMaxValue=GetMaxValue(bNextList);
	//两手牌的最小值
	BYTE FirstMinValue=GetMinValue(bFirstList);
	BYTE NextMinValue=GetMinValue(bNextList);
	//最大牌的花色
	BYTE FirstColor=GetMaxColor(bFirstList);
	BYTE NextColor=GetMaxColor(bNextList);

	//开始比较
	if(bFirstType!=bNextType)
	{
		return (bFirstType>bNextType);
	}
	else
	{
		switch(bFirstType)
		{
			case CT_SINGLE:
				{
					BYTE FirstTotalValue=(GetCardLogicValue(bFirstList[0])+GetCardLogicValue(bFirstList[1]))%10;
					BYTE NextTotalValue=(GetCardLogicValue(bNextList[0])+GetCardLogicValue(bNextList[1]))%10;
					if(FirstTotalValue==NextTotalValue) 
					{
						if(FirstMaxValue!=NextMaxValue) return FirstMaxValue>NextMaxValue;
						if(FirstMinValue!=NextMinValue) return FirstMinValue>NextMinValue;
						else return FirstColor>NextColor;
					}
					else 
					{
						return FirstTotalValue>NextTotalValue;
					}
					break;
				}
			case CT_COLOR:
			case CT_DOUBLE:
				{
					if(FirstMaxValue==NextMaxValue) 
					{
						if(FirstMinValue!=NextMinValue) return FirstMinValue>NextMinValue;
						else return FirstColor>NextColor;
					}
					else 
					{
						return FirstMaxValue>NextMaxValue;
					}
					break;
				}
			default:
				return false;
		}
	}
}
//混乱扑克
void CGameLogic::RandCard(BYTE bCardBuffer[], BYTE bBufferCount)
{
	BYTE bSend=0,bStation=0,bCardList[CountArray(m_bCardArray)];
	CopyMemory(bCardList,m_bCardArray,sizeof(m_bCardArray));
	static long int dwRandCount=0L;
	srand((unsigned)time(NULL)+dwRandCount++);
	do
	{
		bStation=rand()%(CountArray(m_bCardArray)-bSend);
		bCardBuffer[bSend++]=bCardList[bStation];
		bCardList[bStation]=bCardList[CountArray(m_bCardArray)-bSend];
	} while (bSend<bBufferCount);

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wOperaCount=0;
	m_bSendCardCount=0;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_bTableCardCount,0,sizeof(m_bTableCardCount));
	memset(m_bTableCardArray,0,sizeof(m_bTableCardArray));

	//金币信息
	m_lMaxGold=0;
	m_lTurnBasicGold=0;
	memset(m_lTableGold,0,sizeof(m_lTableGold));

	//辅助变量
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));

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
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
	memset(m_bTableCardArray,0,sizeof(m_bTableCardArray));
	memset(m_bTableCardCount,0,sizeof(m_bTableCardCount));

	//金币变量
	m_lMaxGold=0L;
	m_lTurnBasicGold=0L;
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
	return m_bPlayStatus[wChairID]?true:false;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//状态变量
	m_wOperaCount=0;
	m_bSendCardCount=0;
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
	memset(m_bTableCardCount,0,sizeof(m_bTableCardCount));

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
			m_bPlayStatus[i]=true;
			m_lTableGold[i*2+1]=m_pGameServiceOption->lCellScore;
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

	//分发扑克
	m_bSendCardCount=4;
	m_GameLogic.RandCard(m_bTableCardArray[0],sizeof(m_bTableCardArray)/sizeof(m_bTableCardArray[0][0]));
	//////////////////////////////////////////////
	//m_bTableCardArray[0][0]=0x07;
	//m_bTableCardArray[0][1]=0x08;
	//m_bTableCardArray[1][0]=0x11;
	//m_bTableCardArray[1][1]=0x14;
	//m_bTableCardArray[2][0]=0x07;
	//m_bTableCardArray[2][1]=0x08;
	//m_bTableCardArray[3][0]=0x11;
	//m_bTableCardArray[3][1]=0x14;
	//////////////////////////////////////////////
	for (WORD i=0;i<m_wPlayerCount;i++) 
	{
		if (m_bPlayStatus[i]==TRUE)
		{
			m_bTableCardCount[i]=m_bSendCardCount;
		}
	}
	m_wCurrentUser=DeduceWiner(1,1);

	//发送开始
	CMD_S_GameStart GameStart;
	memset(&GameStart,0,sizeof(GameStart));
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.lMaxGold=m_lMaxGold;
	GameStart.lTurnMaxGold=m_lMaxGold/4L;
	GameStart.lTurnBasicGold=m_lTurnBasicGold;
	for (WORD i=0;i<m_wPlayerCount;i++) GameStart.bCardData[i]=m_bTableCardArray[i][1];
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		GameStart.bFundusCard=0;
		if (m_bPlayStatus[i]==TRUE)
		{
			GameStart.bFundusCard=m_bTableCardArray[i][0];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	//设置定时器
	//m_pITableFrame->SetGameTimer(IDI_TIME_GIVE_UP,35000L,1,m_wCurrentUser);

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
	case GER_NO_PLAYER:		//没有玩家
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));
			tagScoreInfo ScoreInfo[m_wPlayerCount];
			memset(&ScoreInfo,0,sizeof(ScoreInfo));
			LONG lTax=0L;

			//计算总注
			LONG lAllGold=0L;
			for (WORD i=0;i<CountArray(m_lTableGold);i++) lAllGold+=m_lTableGold[i];

			//统计信息
			WORD wWinerUser=DeduceWiner(0,4);
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//底牌信息
				if ((cbReason==GER_NO_PLAYER)||(m_bPlayStatus[i]==FALSE)) GameEnd.bUserCard[i]=0;
				else GameEnd.bUserCard[i]=m_bTableCardArray[i][0];

				//积分信息
				if (i==wWinerUser)
				{
					GameEnd.lGameGold[i]=lAllGold-m_lTableGold[i*2]-m_lTableGold[i*2+1];
					lTax=(GameEnd.lGameGold[i]>=100L)?GameEnd.lGameGold[i] *m_pGameServiceOption->wRevenue /100L:0L;
					GameEnd.lTax=lTax;
					GameEnd.lGameGold[i]-=lTax;
					ScoreInfo[i].lRevenue=lTax;
				}
				else GameEnd.lGameGold[i]=-m_lTableGold[i*2]-m_lTableGold[i*2+1];
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改用户分数

			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_bPlayStatus[i]==TRUE)
				{
					ScoreInfo[i].lScore=GameEnd.lGameGold[i];
					ScoreInfo[i].ScoreKind=(GameEnd.lGameGold[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
				}
				m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore ,ScoreInfo[i].lRevenue ,ScoreInfo[i].ScoreKind);
			}
			//m_pITableFrame->WriteTableScore(ScoreInfo,m_wPlayerCount,GameEnd.lTax);

			//删除定时器
			/*if (m_bTimeStation!=255)
			{
				KillTimer(m_bTimeStation);
				m_bTimeStation=255;
			}*/

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
			StatusFree.dwBasicGold=m_pGameServiceOption->lCellScore;

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
			StatusPlay.lBasicGold=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.bPlayStatus,m_bPlayStatus,sizeof(m_bPlayStatus));
			CopyMemory(StatusPlay.lTableGold,m_lTableGold,sizeof(m_lTableGold));

			//设置下注
			if (m_bSendCardCount<=2) StatusPlay.lTurnMaxGold=m_lMaxGold/4L;
			else if (m_bSendCardCount==3) StatusPlay.lTurnMaxGold=m_lMaxGold/2L;
			else StatusPlay.lTurnMaxGold=m_lMaxGold;

			//设置扑克
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_bPlayStatus[i]==TRUE)
				{
					ASSERT(m_pITableFrame->GetServerUserItem(i)!=NULL);
					StatusPlay.bTableCardCount[i]=m_bTableCardCount[i];
					if ((i==wChiarID)&&(bSendSecret==true)) StatusPlay.bTableCardArray[i][0]=m_bTableCardArray[i][0];
					CopyMemory(&StatusPlay.bTableCardArray[i][1],&m_bTableCardArray[i][1],(m_bTableCardCount[i]-1)*sizeof(BYTE));
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
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_ADD_GOLD:	//用户加注
		{
			//效验数据
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			if (m_bPlayStatus[pUserData->wChairID]==FALSE) return true;

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
			if (m_bPlayStatus[pUserData->wChairID]==FALSE) return true;

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
	ASSERT(m_bPlayStatus[wChairID]==TRUE);
	if (m_bPlayStatus[wChairID]==FALSE) return false;
	//变量定义
	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));


	//设置数据
	m_bPlayStatus[wChairID]=FALSE;
	m_bTableCardCount[wChairID]=0;

	//发送放弃消息
	CMD_S_GiveUp GiveUp;
	GiveUp.wUserChairID=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

	//写入数据库	
	ScoreInfo.ScoreKind=enScoreKind_Lost;
	ScoreInfo.lScore=-m_lTableGold[wChairID*2]-m_lTableGold[wChairID*2+1];
	m_pITableFrame->WriteUserScore(wChairID,ScoreInfo.lScore,ScoreInfo.lRevenue,enScoreKind_Lost);

	//判断结束
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_bPlayStatus[i]==TRUE) wPlayerCount++;
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
	//设置数据
	if (bCancel==false)
	{
		//效验数据
		ASSERT(m_wCurrentUser==wChairID);
		if (m_wCurrentUser!=wChairID) return false;

		//计算限注
		LONG lMaxGold=0L;
		if (m_bSendCardCount<=2) lMaxGold=m_lMaxGold/4L;
		else if (m_bSendCardCount==3) lMaxGold=m_lMaxGold/2L;
		else lMaxGold=m_lMaxGold;
		
		//计算剩下的金币
		LONG lLeaveGold=lMaxGold-m_lTableGold[wChairID*2+1];
		m_lTableGold[wChairID*2]=__min(lLeaveGold,__max(lGold,m_lTurnBasicGold));
	}

	//操作次数
	bool bFinishTurn=false;
	WORD wNextUser=INVALID_CHAIR;
	for (WORD i=0;i<m_wPlayerCount-1;i++)
	{
		m_wOperaCount++;
		wNextUser=(m_wCurrentUser+i+1)%m_wPlayerCount;
		if (m_bPlayStatus[wNextUser]==TRUE) break;
	}
	
	//判断下注
	if (m_wOperaCount>=m_wPlayerCount)
	{
		LONG lTableGold=0L;
		WORD i=0;
		for (i=0;i<m_wPlayerCount;i++)
		{
			if (m_bPlayStatus[i]==TRUE)
			{
				lTableGold=m_lTableGold[i*2];
				break;
			}
		}
		while (i<m_wPlayerCount)
		{
			if ((m_bPlayStatus[i]==TRUE)&&(m_lTableGold[i*2]!=lTableGold)) break;
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
		if (m_bSendCardCount<2) 
		{
			//设置变量
			m_wOperaCount=0;
			m_bSendCardCount++;
			m_lTurnBasicGold=0L;
			m_wCurrentUser=DeduceWiner(1,m_bSendCardCount-1);

			//发送扑克
			CMD_R_SendCard SendCard;
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				if (m_bPlayStatus[i]==TRUE) m_bTableCardCount[i]=m_bSendCardCount;
				SendCard.bUserCard[i]=m_bTableCardArray[i][m_bSendCardCount-1];
			}

			//删除旧定时器
			/*if (m_bTimeStation!=255)
			{
			KillTimer(m_bTimeStation);
			m_bTimeStation=255;
			}*/

			//设置定时器
			/*m_bTimeStation=TIME_PASS+m_wCurrentUser;
			SetTimer(m_bTimeStation,TIME_COUNT);*/

			//发送消息
			SendCard.wCurrentUser=m_wCurrentUser;
			SendCard.lMaxGold=(m_bSendCardCount!=3)?m_lMaxGold:m_lMaxGold/2L;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
		}
		else OnEventGameEnd(INVALID_CHAIR,NULL,GER_NORMAL);
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

		//删除旧定时器
		/*if (m_bTimeStation!=255)
		{
			KillTimer(m_bTimeStation);
			m_bTimeStation=255;
		}

		//设置定时器
		m_bTimeStation=TIME_PASS+m_wCurrentUser;
		SetTimer(m_bTimeStation,TIME_COUNT);*/
	}

	return true;
}

//推断胜者
WORD CTableFrameSink::DeduceWiner(BYTE bBeginPos, BYTE bEndPos)
{
	//保存扑克
	BYTE bTableCardArray[m_wPlayerCount][5];
	CopyMemory(bTableCardArray,m_bTableCardArray,sizeof(m_bTableCardArray));

	//寻找玩家
	BYTE bWiner;
	for (bWiner=0;bWiner<m_wPlayerCount;bWiner++)
	{
		if (m_bPlayStatus[bWiner]==TRUE) break;
	}

	//对比玩家
	for (BYTE i=(bWiner+1);i<m_wPlayerCount;i++)
	{
		if (m_bPlayStatus[i]==false) continue;
		if (m_GameLogic.CompareCard(bTableCardArray[i]+bBeginPos,bTableCardArray[bWiner]+bBeginPos,bEndPos-bBeginPos+1)==true) bWiner=i;
	}

	return bWiner;
}

//////////////////////////////////////////////////////////////////////////
