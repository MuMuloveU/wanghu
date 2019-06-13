#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_AllReady;	//开始模式

//////////////////////////////////////////////////////////////////////////
//定时器
#define				IDI_BANK_TIME							1				//选庄定时器
/////////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	//历史积分
	memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
	memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));
	//运行信息
	m_bAnyFriendEnd=false;
	m_bTurnCardCount=0;
	m_wBankerUser=0;
	m_bWinKind=0;
	m_wPassCount=0;
	m_bResidualCardCount=54;
	memset(m_bCount1,0,sizeof(m_bCount1));
	memset(m_bCount2,0,sizeof(m_bCount2));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	ZeroMemory(m_bTempCardData,sizeof(m_bTempCardData));
	//分值组初始化
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_UserScoreCard[i].RemoveAll();
		m_UserScoreCard[i].FreeExtra();
	}
	m_bOneTurnScoreCard.RemoveAll();
	m_bOneTurnScoreCard.FreeExtra();
	m_wEndUsers.RemoveAll();
	m_wEndUsers.FreeExtra();
	//扑克信息
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

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
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_bAnyFriendEnd=false;
	m_bWinKind=0;
	m_bResidualCardCount=54;
	m_wPassCount=0;
	//运行信息
	m_bTurnCardCount=0;
	memset(m_bCount1,0,sizeof(m_bCount1));
	memset(m_bCount2,0,sizeof(m_bCount2));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	//分值组初始化
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_UserScoreCard[i].RemoveAll();
		m_UserScoreCard[i].FreeExtra();
	}
	m_bOneTurnScoreCard.RemoveAll();
	m_bOneTurnScoreCard.FreeExtra();
	m_wEndUsers.RemoveAll();
	m_wEndUsers.FreeExtra();
	//扑克信息
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	ZeroMemory(m_bTempCardData,sizeof(m_bTempCardData));
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
//设置服务器时间
void CTableFrameSink::SetGameTimer(WORD wTimerID,UINT nElapse)
{
	nElapse=nElapse*1000;
	/*SetTimer(NULL,wTimerID,nElapse,NULL);
	MSG msg;
	msg.message=NULL;
	PeekMessage(&msg,NULL,NULL,NULL,FALSE);
	while(GetMessage(&msg,NULL,NULL,NULL))
	{
		if(msg.message==WM_TIMER)
		{
					SendCard();	
					break;
		}
	}
	KillTimer(NULL,wTimerID);*/
	Sleep(nElapse);
	SendCard();	
}
void CTableFrameSink::KillGameTimer(WORD wTimerID)
{
	KillTimer(NULL,wTimerID);
	return;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	//设置分发扑克
	CopyMemory(m_bTempCardData,&m_GameLogic.m_bCardListData,sizeof(m_bTempCardData));

	//历史积分
	memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
	memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));
	//运行信息
	m_bAnyFriendEnd=false;
	m_bTurnCardCount=0;
	m_wBankerUser=0;
	m_bWinKind=0;
	m_wPassCount=0;
	m_bResidualCardCount=54;
	memset(m_bCount1,0,sizeof(m_bCount1));
	memset(m_bCount2,0,sizeof(m_bCount2));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	//设置状态
	m_pITableFrame->SetGameStatus(GS_RF_BANKER);

	//混乱扑克
	BYTE bRandBankCard[GAME_PLAYER];
	m_GameLogic.RandBankCardList(bRandBankCard,CountArray(bRandBankCard));

	//分发扑克
	BYTE bBankCard[GAME_PLAYER];
	ZeroMemory(bBankCard,sizeof(bBankCard));
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(!m_pITableFrame->GetServerUserItem(i))continue;
		CopyMemory(&bBankCard[i],&bRandBankCard[i],sizeof(BYTE));
		//m_GameLogic.SortCardList(m_bHandCardData[i],m_bCardCount[i]);
	}
	//检测最大牌
	BYTE wBanker;
	BYTE bData=m_GameLogic.GetCardValue(bBankCard[0]);
	for(BYTE j=0;j<GAME_PLAYER;j++)
	{
		if(!m_pITableFrame->GetServerUserItem(j))continue;
		BYTE bCardData=m_GameLogic.GetCardValue(bBankCard[j]);
		if(bCardData==0x01)
		{
			wBanker=j;
			break;
		}
		if(bData<bCardData)
		{
			bData=bCardData;
			wBanker=j;
		}

	}
	//设置庄家
	m_wBankerUser=wBanker;
	//设置当前玩家
	m_wCurrentUser=wBanker;
	////设定每一局庄家
 //  m_wCurrentUser=(BYTE)(rand()%m_wPlayerCount);
 //  while(!m_pITableFrame->GetServerUserItem(m_wCurrentUser)||!CanBeBanker(m_wCurrentUser))
 //  {
	//srand((unsigned)time(NULL));
	//m_wCurrentUser=(BYTE)(rand()%m_wPlayerCount);
 //  }
 //  m_wFirstPlayer=m_wCurrentUser;

	//错误判断
//	ASSERT(FALSE);
	//发送扑克
	CMD_S_Bank BankData;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(!m_pITableFrame->GetServerUserItem(i))continue;
		CopyMemory(&BankData.bCardData,bBankCard,sizeof(bBankCard));
		BankData.Moster=wBanker;
		m_pITableFrame->SendTableData(i,SUB_S_BANK,&BankData,sizeof(BankData));
		m_pITableFrame->SendLookonData(i,SUB_S_BANK,&BankData,sizeof(BankData));
	}
	//设置时间
	SetGameTimer(IDI_BANK_TIME,5);

	return true;
}
//发牌
void CTableFrameSink::SendCard()
{
	//设置游戏状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);
	//删出定时器
	KillGameTimer(IDI_BANK_TIME);

	//构造数据
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(!m_pITableFrame->GetServerUserItem(i))continue;
	
		//设置扑克数
		if(i==m_wBankerUser)
			m_bCardCount[i]=6;
		else
			m_bCardCount[i]=5;
		//构造数据
		GameStart.wBanker=m_wBankerUser;
		GameStart.wCurrentUser=m_wCurrentUser;
		//混乱扑克
		m_GameLogic.RandCardList(GameStart.bCardData,m_bCardCount[i],m_bTempCardData,m_bResidualCardCount);
		CopyMemory(m_bHandCardData[i],&GameStart.bCardData,m_bCardCount[i]);
		m_GameLogic.SortCardList(GameStart.bCardData,m_bCardCount[i]);
		m_GameLogic.SortCardList(m_bHandCardData[i],m_bCardCount[i]);
		//m_GameLogic.SortCardList(m_bTempCardData,m_bResidualCardCount+m_bCardCount[i]);

		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	return ;

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

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
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
			memset(&GameEnd,0,sizeof(GameEnd));

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//统计积分
			BYTE bPlayerCount=0;
			for(BYTE i=0;i<GAME_PLAYER;i++)
				if(m_pITableFrame->GetServerUserItem(i))
					bPlayerCount++;

			for(BYTE j=0;j<GAME_PLAYER;j++)
			{
				if(!m_pITableFrame->GetServerUserItem(j))continue;
				BYTE bFiveCount=0;
				for(BYTE k=0;k<m_UserScoreCard[j].GetSize();k++)
					if(m_GameLogic.GetCardValue(m_UserScoreCard[j].GetAt(k))==0x05)
						bFiveCount++;
				//计算分数
				GameEnd.lGameScore[j]=(bFiveCount*5+(m_UserScoreCard[j].GetSize()-bFiveCount)*10)*10-100/bPlayerCount*10;
			}


					
        	//统计牌分
		    LONG lCardScore[GAME_PLAYER];
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				LONG lScoreTimes=1;
				if (m_bCardCount[i]==10) lScoreTimes=4;
				else if (m_bCardCount[i]>=10) lScoreTimes=3;
				else if (m_bCardCount[i]>=8) lScoreTimes=2;
				lCardScore[i]=lScoreTimes*m_bCardCount[i];
			}

			//计算税收
			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (GameEnd.lGameScore[i]>1L)
					{
						GameEnd.lGameTax+=GameEnd.lGameScore[i]*5/100L;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]*(100-5)/100;
					}
				}
			}

			//积分统计
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				m_lAllTurnScore[i]+=GameEnd.lGameScore[i];
				m_lLastTurnScore[i]=GameEnd.lGameScore[i];
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfo[m_wPlayerCount];
			memset(&ScoreInfo,0,sizeof(ScoreInfo));
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				ScoreInfo[i].lScore=GameEnd.lGameScore[i];
				ScoreInfo[i].ScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
			}
		//	m_pITableFrame->WriteUserScore(ScoreInfo,m_wPlayerCount,GameEnd.lGameTax);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//剩余扑克
			BYTE bCardPos=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.bCardCount[i]=m_bCardCount[i];
				CopyMemory(&GameEnd.bCardData[bCardPos],m_bHandCardData[i],m_bCardCount[i]*sizeof(BYTE));
				bCardPos+=m_bCardCount[i];
			}

			//统计积分
			GameEnd.lGameScore[wChairID]=-200L*m_pGameServiceOption->lCellScore;

			//计算税收
			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (GameEnd.lGameScore[i]>=100L)
					{
						GameEnd.lGameTax+=GameEnd.lGameScore[i]/50L;
						GameEnd.lGameScore[i]=GameEnd.lGameScore[i]*49L/50L;
					}
				}
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfo;
			ScoreInfo.ScoreKind=enScoreKind_Draw;
			ScoreInfo.lScore=GameEnd.lGameScore[wChairID];
//			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_RF_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//设置变量
			StatusFree.lBaseScore=m_pGameServiceOption->lCellScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_RF_PLAYING:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//设置变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.bTurnCardCount=m_bTurnCardCount;
			StatusPlay.lBaseScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.bCardCount,m_bCardCount,sizeof(m_bCardCount));
			CopyMemory(StatusPlay.bTurnCardData,m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
			CopyMemory(StatusPlay.bCardData,m_bHandCardData[wChiarID],sizeof(BYTE)*m_bCardCount[wChiarID]);

			//历史积分
			CopyMemory(StatusPlay.lAllTurnScore,m_lAllTurnScore,sizeof(m_lAllTurnScore));
			CopyMemory(StatusPlay.lLastTurnScore,m_lLastTurnScore,sizeof(m_lLastTurnScore));

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
	case SUB_C_OUT_CART:		//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->bCardData);

			//效验数据
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserOutCard(pUserData->wChairID,pOutCard->bCardData,pOutCard->bCardCount);
		}
	case SUB_C_PASS_CARD:
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserPassCard(pUserData->wChairID);
		}

	case SUB_C_JIU://揪人
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			return true;

		}
	case SUB_C_DISPLAYTEN:
		{
			//变量定义
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			CMD_C_DisplayTen * pDisplayTen=(CMD_C_DisplayTen *)pDataBuffer;
			WORD wHeadSize=sizeof(CMD_C_DisplayTen)-sizeof(pDisplayTen->bDisplayStatus);

			//效验数据
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
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


//判断哪个组用户获胜
int CTableFrameSink::GetSpecialWinKind()
{
	BYTE bSWin=1;
	BYTE bDogFall=0;
	int bCWin=-1;

return 0;

}
//游戏是否结束
bool CTableFrameSink::IsGameOver()
{

	return false;
}
//补牌
void CTableFrameSink::RecruitCard(WORD wChairID)
{
	BYTE bUserOweCardCount;
	//补牌
	//构造数据
	CMD_S_RecruitCard RecruitCard;
	ZeroMemory(&RecruitCard,sizeof(RecruitCard));
	//设置一轮分值牌
	for(BYTE j=0;j<m_bOneTurnScoreCard.GetSize();j++)
		RecruitCard.bOneTurnScoreCard[j]=m_bOneTurnScoreCard.GetAt(j);
	RecruitCard.bOneTurnScoreCardCount=m_bOneTurnScoreCard.GetSize();
	//补牌
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		bUserOweCardCount=5-m_bCardCount[i];
		if(!m_pITableFrame->GetServerUserItem(i))continue;
		RecruitCard.wCurrentUser=wChairID;	
		BYTE * bUserOweCardData=new BYTE [bUserOweCardCount];
		ZeroMemory(bUserOweCardData,sizeof(bUserOweCardData));
		m_GameLogic.RandCardList(bUserOweCardData,bUserOweCardCount,m_bTempCardData,m_bResidualCardCount);
		//补
		m_GameLogic.CombinationArray(m_bHandCardData[i],m_bCardCount[i],bUserOweCardData,bUserOweCardCount);
		m_bCardCount[i]+=bUserOweCardCount;
		RecruitCard.bRecruitCount=bUserOweCardCount;
		CopyMemory(&RecruitCard.bRecruitCard,bUserOweCardData,sizeof(BYTE)*bUserOweCardCount);
		m_pITableFrame->SendTableData(i,SUB_S_RECRUIT_CARD,&RecruitCard,sizeof(CMD_S_RecruitCard));

	}
}


//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_RF_PLAYING) return true;
	if (wChairID!=m_wCurrentUser) return false;
	//类型判断
	BYTE bCardType=m_GameLogic.GetCardType(bCardData,bCardCount);
	if (bCardType==CT_ERROR) return false;

	//设置变量
	m_wPassCount=0;

	//更随出牌
	if (m_bTurnCardCount==0) m_bTurnCardCount=bCardCount;
	else if (m_GameLogic.CompareCard(bCardData,m_bTurnCardData,bCardCount,m_bTurnCardCount)==false) return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(bCardData,bCardCount,m_bHandCardData[wChairID],m_bCardCount[wChairID])==false) return false;
	m_bCardCount[wChairID]-=bCardCount;

	////大牌判断
	//bool bMostest=false;
	//if ((bCardCount<=2)&&(bCardData[0]==0x42)) bMostest=true;

	//出牌记录
	m_bTurnCardCount=bCardCount;
	CopyMemory(m_bTurnCardData,bCardData,sizeof(BYTE)*bCardCount);
	//分值记录
	for(WORD j=0;j<m_bTurnCardCount;j++)
	{
		BYTE m_bTemp1=m_GameLogic.GetCardValue(m_bTurnCardData[j]);
		if(m_bTemp1==0x05||m_bTemp1==0x0A||m_bTemp1==0x0D)
			m_bOneTurnScoreCard.Add(m_bTurnCardData[j]);
	}

	BYTE i=wChairID+1;
	while(!m_bCardCount[(i%m_wPlayerCount)])
	{
		i++;
	}

	//设置变量
	m_wCurrentUser=i%m_wPlayerCount;
	m_wPassCount=i-wChairID-1;

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.bCardCount=bCardCount;
	OutCard.wOutCardUser=wChairID;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.bCardData,m_bTurnCardData,m_bTurnCardCount*sizeof(BYTE));

	//发送数据
	WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//出牌最大
	/*if (bMostest==true) m_bTurnCardCount=0;*/

	


	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	if (m_pITableFrame->GetGameStatus()!=GS_RF_PLAYING) return false;
	//设置变量
	BYTE i=wChairID+1;
	while(!m_bCardCount[(i%m_wPlayerCount)])
	{
		i++;
	}

	//设置变量
	m_wCurrentUser=i%m_wPlayerCount;
	m_wPassCount+=i-wChairID;

	if(m_wPassCount>=m_wPlayerCount-1)
	{
		ZeroMemory(m_bTurnCardData,sizeof(BYTE)*m_bTurnCardCount);
		m_bTurnCardCount=0;
		if(m_bResidualCardCount)
			RecruitCard(m_wCurrentUser);
	}
	//新一轮设置分数牌
	if(!m_bTurnCardCount)
		for(BYTE i=0;i<m_bOneTurnScoreCard.GetSize();i++)
			m_UserScoreCard[m_wCurrentUser].Add(m_bOneTurnScoreCard.GetAt(i));

	//清理一轮分值牌数组
	m_bOneTurnScoreCard.RemoveAll();
	m_bOneTurnScoreCard.FreeExtra();

	
	//发送数据
	CMD_S_PassCard PassCard;
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bNewTurn=(m_bTurnCardCount==0)?TRUE:FALSE;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	//结束判断
	BYTE bScoreCardCount=0;
	for(BYTE j=0;j<GAME_PLAYER;j++)
	{
		bScoreCardCount+=m_UserScoreCard[j].GetSize();

	}
	if(bScoreCardCount==12)
		OnEventGameEnd(wChairID,NULL,GER_NORMAL);

	return true;
}

//////////////////////////////////////////////////////////////////////////
