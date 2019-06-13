#include "StdAfx.h"
#include "TableFrameSink.h"
#include  "math.h"
//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wSiceCount=MAKEWORD(1,1);
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));


	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;

	m_wOutCardUser=INVALID_CHAIR;
	m_cbLeftCardCount=0;
	m_cbRemainCardCount = GAME_REMAIN;
	m_cbGangCount = 0;
	ZeroMemory(m_cbEnjoinHuCard,sizeof(m_cbEnjoinHuCard));
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	ZeroMemory(m_bCallCard,sizeof(m_bCallCard));

	//花牌数目
	ZeroMemory(m_cbWindCount,sizeof(m_cbWindCount));
	ZeroMemory(m_cbTempWinCount,sizeof(m_cbTempWinCount));
	ZeroMemory(m_cbWindData,sizeof(m_cbWindData));
	ZeroMemory(m_cbUserWindCount,sizeof(m_cbUserWindCount));
	ZeroMemory(m_cbUserWindData,sizeof(m_cbUserWindData));
	m_cbAllWindCount = 0;
	m_cbSumWindCount = 0;

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

	//运行变量
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus=false;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(&m_ChiHuResult,sizeof(m_ChiHuResult));

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
	if (m_pITableFrame==NULL) 
		return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}

//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	//游戏变量
	m_wSiceCount=MAKEWORD(1,1);
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	m_cbLeftCardCount=0;
	m_cbRemainCardCount = GAME_REMAIN;
	m_cbGangCount = 0;
	ZeroMemory(m_cbEnjoinHuCard,sizeof(m_cbEnjoinHuCard));
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	ZeroMemory(m_bCallCard,sizeof(m_bCallCard));

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

	//运行变量
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;

	ZeroMemory(m_cbWindCount,sizeof(m_cbWindCount));
	ZeroMemory(m_cbTempWinCount,sizeof(m_cbTempWinCount));
	ZeroMemory(m_cbWindData,sizeof(m_cbWindData));
	ZeroMemory(m_cbUserWindCount,sizeof(m_cbUserWindCount));
	ZeroMemory(m_cbUserWindData,sizeof(m_cbUserWindData));
	m_cbAllWindCount = 0;
	m_cbSumWindCount = 0;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus=false;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(&m_ChiHuResult,sizeof(m_ChiHuResult));

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

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_MJ_PLAY);

	//混乱扑克
	m_wSiceCount=MAKEWORD(rand()%6+1,rand()%6+1);
	m_cbLeftCardCount=CountArray(m_cbRepertoryCard);
	m_GameLogic.RandCardData(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));
	


	//分发扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_cbLeftCardCount-=(MAX_COUNT-1);
		m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);
	}

	//发送扑克
	m_cbSendCardCount++;
	m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	//设置变量
	m_cbProvideCard=m_cbSendCardData;
	m_wProvideUser=INVALID_CHAIR;
	m_wCurrentUser=m_wBankerUser;

	//动作分析
	bool bAroseAction=false;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//庄家判断
		if (i==m_wBankerUser)
		{
			//杠牌判断
			tagGangCardResult GangCardResult;
			m_cbUserAction[i]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[i],NULL,0,GangCardResult);

			//胡牌判断
			tagChiHuResult ChiHuResult;
			BYTE cbHandTai=0;
			BYTE cbHandFeng =0;
			m_cbUserAction[i]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],NULL,0,0,0,ChiHuResult,i,true,true);

		}

		//状态设置
		if ((bAroseAction==false)&&(i!=m_wBankerUser)&&(m_cbUserAction[i]!=WIK_NULL))
		{
			bAroseAction=true;
			m_wResumeUser=m_wCurrentUser;
			m_wCurrentUser=INVALID_CHAIR;
		}
	}

	//构造数据
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	GameStart.wSiceCount=m_wSiceCount;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser=INVALID_CHAIR;
	GameStart.cbLeftCardCount = m_cbLeftCardCount;
	GameStart.bFirst=true;
	CopyMemory(GameStart.bTrustee,m_bTrustee,sizeof(m_bTrustee));

	//花牌数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		for (BYTE j=MAX_INDEX-8;j<MAX_INDEX;j++)
		{
			m_cbWindCount[i] += m_cbCardIndex[i][j];
		}
		m_cbSumWindCount += m_cbWindCount[i];
		m_cbAllWindCount += m_cbWindCount[i];
	}

	//发送数据
	IServerUserItem *pUserItem = NULL;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//设置变量
		GameStart.cbUserAction=m_cbUserAction[i];
		pUserItem = m_pITableFrame->GetServerUserItem(i);
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData,CountArray(GameStart.cbCardData));
		if(i == m_wBankerUser)
			CopyMemory(m_cbWindData[i],&GameStart.cbCardData[MAX_COUNT-m_cbWindCount[i]],sizeof(BYTE)*m_cbWindCount[i]);
		else
			CopyMemory(m_cbWindData[i],&GameStart.cbCardData[MAX_COUNT-m_cbWindCount[i]-1],sizeof(BYTE)*m_cbWindCount[i]);
		if( pUserItem->IsAndroidUser() )
		{
			BYTE bIndex = 1;
			for( WORD j=0; j<GAME_PLAYER; j++ )
			{
				if( j == i ) continue;
				m_GameLogic.SwitchToCardData(m_cbCardIndex[j],&GameStart.cbCardData[MAX_COUNT*bIndex++]);
			}
			CopyMemory(GameStart.cbRepertoryCard,m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
		}

		GameStart.cbSunWindCount = m_cbSumWindCount;

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
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
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			LONG lCellScore=m_pGameServiceOption->lCellScore;

			//结束信息
			GameEnd.wProvideUser=wChairID;
			GameEnd.cbProvideCard=m_cbChiHuCard;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//胡牌类型
				GameEnd.wChiHuKind[i]	=m_ChiHuResult[i].wChiHuKind;
				GameEnd.wChiHuRight[i]	=m_ChiHuResult[i].wChiHuRight;
			}

			//统计积分
			if (m_wProvideUser!=INVALID_CHAIR)
			{
				WORD  wWinUser =INVALID_CHAIR;

				//自摸类型
				if (m_ChiHuResult[m_wProvideUser].wChiHuKind!=CHK_NULL)
				{

					wWinUser=m_wProvideUser;
					BYTE cbHua[4]={0,0,0,0};
					BYTE cbHu[4]={0,0,0,0};
					if(m_wProvideUser == m_wBankerUser)//东风位
					{
						if(m_cbUserWindCount[m_wProvideUser] == 8)
						{
							cbHua[m_wProvideUser] += 4;
							cbHu[m_wProvideUser] += 32;
						}
						else
						{
							cbHu[m_wProvideUser] += 4*m_cbUserWindCount[m_wProvideUser];
							m_GameLogic.SortCardList(m_cbUserWindData[m_wProvideUser],m_cbUserWindCount[m_wProvideUser]);
							
							for (INT bTemp = 0;bTemp<m_cbUserWindCount[m_wProvideUser];bTemp++)
							{
								//梅 春 正花
								if((m_cbUserWindData[m_wProvideUser][bTemp]==0x38)||(m_cbUserWindData[m_wProvideUser][bTemp]==0x3C))
								{
									cbHua[m_wProvideUser] +=1;
									if(bTemp+3<m_cbUserWindCount[m_wProvideUser])
									{
										if(m_cbUserWindData[m_wProvideUser][bTemp]==0x38 &&m_cbUserWindData[m_wProvideUser][bTemp+3]==0x3B)
											cbHua[m_wProvideUser]++;
										if(m_cbUserWindData[m_wProvideUser][bTemp]==0x3C &&m_cbUserWindData[m_wProvideUser][bTemp+3]==0x3F)
											cbHua[m_wProvideUser]++;

									}
								}
							}
						}
					}

					if(m_wProvideUser == (m_wBankerUser+m_wPlayerCount-1)%m_wPlayerCount)//南风位
					{
						if(m_cbUserWindCount[m_wProvideUser] == 8)
						{
							cbHua[m_wProvideUser] += 4;
							cbHu[m_wProvideUser] += 32;
						}
						else
						{
							cbHu[m_wProvideUser] += 4*m_cbUserWindCount[m_wProvideUser];
							m_GameLogic.SortCardList(m_cbUserWindData[m_wProvideUser],m_cbUserWindCount[m_wProvideUser]);
							for (INT bTemp = 0;bTemp<m_cbUserWindCount[m_wProvideUser];bTemp++)
							{
								//兰 夏 正花
								if((m_cbUserWindData[m_wProvideUser][bTemp]==0x39)||(m_cbUserWindData[m_wProvideUser][bTemp]==0x3D))
									cbHua[m_wProvideUser]++;
							
								if(bTemp+2<m_cbUserWindCount[m_wProvideUser]&&bTemp>=1)
								{
									if(m_cbUserWindData[m_wProvideUser][bTemp]==0x39 &&m_cbUserWindData[m_wProvideUser][bTemp+2]==0x3B&&m_cbUserWindData[m_wProvideUser][bTemp-1]==0x38)
										cbHua[m_wProvideUser]++;
									if(m_cbUserWindData[m_wProvideUser][bTemp]==0x3D &&m_cbUserWindData[m_wProvideUser][bTemp+2]==0x3F&&m_cbUserWindData[m_wProvideUser][bTemp-1]==0x3C)
										cbHua[m_wProvideUser]++;

								}
							}


						}
					}
					if(m_wProvideUser == (m_wBankerUser+m_wPlayerCount-2)%m_wPlayerCount)//西风位
					{
	
						if(m_cbUserWindCount[m_wProvideUser] == 8)
						{
							cbHua[m_wProvideUser] += 4;
							cbHu[m_wProvideUser] += 32;
						}
						else
						{
							cbHu[m_wProvideUser] += 4*m_cbUserWindCount[m_wProvideUser];
							m_GameLogic.SortCardList(m_cbUserWindData[m_wProvideUser],m_cbUserWindCount[m_wProvideUser]);
							
							for (INT bTemp = 0;bTemp<m_cbUserWindCount[m_wProvideUser];bTemp++)
							{

								//秋菊
								if((m_cbUserWindData[m_wProvideUser][bTemp]==0x3A)||(m_cbUserWindData[m_wProvideUser][bTemp]==0x3E))
									cbHua[m_wProvideUser] +=1;
								if(bTemp+1<m_cbUserWindCount[m_wProvideUser]&&bTemp>=2)
								{
									if(m_cbUserWindData[m_wProvideUser][bTemp]==0x3A &&m_cbUserWindData[m_wProvideUser][bTemp+1]==0x3B &&m_cbUserWindData[m_wProvideUser][bTemp-2]==0x38)
										cbHua[m_wProvideUser]++;
									if(m_cbUserWindData[m_wProvideUser][bTemp]==0x3E &&m_cbUserWindData[m_wProvideUser][bTemp+1]==0x3F&&m_cbUserWindData[m_wProvideUser][bTemp-2]==0x3C)
										cbHua[m_wProvideUser]++;

								}
							}
						}
					}
					if(m_wProvideUser == (m_wBankerUser+m_wPlayerCount-3)%m_wPlayerCount)//北风位
					{

						if(m_cbUserWindCount[m_wProvideUser] == 8)
						{
							cbHua[m_wProvideUser] += 4;
							cbHu[m_wProvideUser] += 32;
						}
						else
						{
							cbHu[m_wProvideUser] += 4*m_cbUserWindCount[m_wProvideUser];
							m_GameLogic.SortCardList(m_cbUserWindData[m_wProvideUser],m_cbUserWindCount[m_wProvideUser]);
							
							for (INT bTemp = 0;bTemp<m_cbUserWindCount[m_wProvideUser];bTemp++)
							{

								//冬梅
								if((m_cbUserWindData[m_wProvideUser][bTemp]==0x3B)||(m_cbUserWindData[m_wProvideUser][bTemp]==0x3F))
									cbHua[m_wProvideUser] +=1;
								if(bTemp-3<m_cbUserWindCount[m_wProvideUser])
								{
									if(m_cbUserWindData[m_wProvideUser][bTemp]==0x38 &&m_cbUserWindData[m_wProvideUser][bTemp-3]==0x3B)
										cbHua[m_wProvideUser]++;
									if(m_cbUserWindData[m_wProvideUser][bTemp]==0x3C &&m_cbUserWindData[m_wProvideUser][bTemp-3]==0x3F)
										cbHua[m_wProvideUser]++;

								}
							}


						}
					}
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wProvideUser) continue;
						//牌型分析
						for (BYTE j=0;j<m_cbWeaveItemCount[i];j++)
						{
							BYTE cbWeaveKind=m_WeaveItemArray[i][j].cbWeaveKind;
							if(cbWeaveKind==WIK_PENG)
							{
								if((m_WeaveItemArray[i][j].cbCenterCard&MASK_COLOR)==0x30)
								{
									cbHu[i] +=4;
								}
								else
								{
									if((m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x01 ||(m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x09)
									{
										cbHu[i] +=4;
									}
									else
										cbHu[i] +=2;
								}
							}
							if(cbWeaveKind==WIK_GANG)
							{
								if(m_WeaveItemArray[i][j].cbPublicCard==FALSE)
								{
									if((m_WeaveItemArray[i][j].cbCenterCard&MASK_COLOR)==0x30)
									{
										cbHu[i] +=32;
									}
									else
									{
										if((m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x01 ||(m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x09)
										{
											cbHu[i] +=32;
										}
										else
											cbHu[i] +=16;
									}
								}
								if(m_WeaveItemArray[i][j].cbPublicCard==TRUE)
								{
									if((m_WeaveItemArray[i][j].cbCenterCard&MASK_COLOR)==0x30)
									{
										cbHu[i] +=16;
									}
									else
									{
										if((m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x01 ||(m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x09)
										{
											cbHu[i] +=16;
										}
										else
											cbHu[i] +=8;
									}
								}
							}

							if(m_WeaveItemArray[i][j].cbCenterCard>=0x35 &&m_WeaveItemArray[i][j].cbCenterCard<=0x37)
							{
								cbHua[i]+=1;
							}
							if(i == m_wBankerUser && m_WeaveItemArray[i][j].cbCenterCard==0x31)
							{
								cbHua[i] += 1;
							}
							if((i == (m_wBankerUser+m_wPlayerCount-1)%m_wPlayerCount) && m_WeaveItemArray[i][j].cbCenterCard==0x32)
							{
								cbHua[i] += 1;
							}
							if((i == (m_wBankerUser+m_wPlayerCount-2)%m_wPlayerCount) && m_WeaveItemArray[i][j].cbCenterCard==0x33)
							{
								cbHua[i] += 1;
							}
							if((i == (m_wBankerUser+m_wPlayerCount-3)%m_wPlayerCount) && m_WeaveItemArray[i][j].cbCenterCard==0x34)
							{
								cbHua[i] += 1;
							}
						}

						//花牌
						if(i == m_wBankerUser)//东风位
						{
							if(m_cbUserWindCount[i] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								//正花 野花
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{
									//梅 春 正花
									if((m_cbUserWindData[i][bTemp]==0x38)||(m_cbUserWindData[i][bTemp]==0x3C))
									{
										cbHua[i] +=1;
										if(bTemp+3<m_cbUserWindCount[i])
										{
											if(m_cbUserWindData[i][bTemp]==0x38 &&m_cbUserWindData[i][bTemp+3]==0x3B)
												cbHua[i]++;
											if(m_cbUserWindData[i][bTemp]==0x3C &&m_cbUserWindData[i][bTemp+3]==0x3F)
												cbHua[i]++;

										}
									}
								}
							}
						}

						if(i == (m_wBankerUser+m_wPlayerCount-1)%m_wPlayerCount)//南风位
						{
							if(m_cbUserWindCount[i] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{
									//兰 夏 正花
									if((m_cbUserWindData[i][bTemp]==0x39)||(m_cbUserWindData[i][bTemp]==0x3D))
										cbHua[i]++;

									if(bTemp+2<m_cbUserWindCount[i]&&bTemp>=1)
									{
										if(m_cbUserWindData[i][bTemp]==0x39 &&m_cbUserWindData[i][bTemp+2]==0x3B&&m_cbUserWindData[i][bTemp-1]==0x38)
											cbHua[i]++;
										if(m_cbUserWindData[i][bTemp]==0x3D &&m_cbUserWindData[i][bTemp+2]==0x3F&&m_cbUserWindData[i][bTemp-1]==0x3C)
											cbHua[i]++;

									}
								}


							}
						}
						if(i == (m_wBankerUser+m_wPlayerCount-2)%m_wPlayerCount)//西风位
						{

							if(m_cbUserWindCount[i] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								//正花 野花
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{

									//梅 春 正花
									if((m_cbUserWindData[i][bTemp]==0x3A)||(m_cbUserWindData[i][bTemp]==0x3E))
										cbHua[i] +=1;
									if(bTemp+1<m_cbUserWindCount[i]&&bTemp>=2)
									{
										if(m_cbUserWindData[i][bTemp]==0x3A &&m_cbUserWindData[i][bTemp+1]==0x3B &&m_cbUserWindData[i][bTemp-2]==0x38)
											cbHua[i]++;
										if(m_cbUserWindData[i][bTemp]==0x3E &&m_cbUserWindData[i][bTemp+1]==0x3F&&m_cbUserWindData[i][bTemp-2]==0x3C)
											cbHua[i]++;

									}
								}
							}

						}
						if(i == (m_wBankerUser+m_wPlayerCount-3)%m_wPlayerCount)//北风位
						{

							if(m_cbUserWindCount[i] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								//正花 野花
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{

									//梅 春 正花
									if((m_cbUserWindData[i][bTemp]==0x3B)||(m_cbUserWindData[i][bTemp]==0x3F))
										cbHua[i] +=1;
									if(bTemp-3<m_cbUserWindCount[i])
									{
										if(m_cbUserWindData[i][bTemp]==0x38 &&m_cbUserWindData[i][bTemp-3]==0x3B)
											cbHua[i]++;
										if(m_cbUserWindData[i][bTemp]==0x3C &&m_cbUserWindData[i][bTemp-3]==0x3F)
											cbHua[i]++;

									}
								}


							}
						}

						for(BYTE j=0;j<MAX_INDEX-8;j++)
						{
							if(m_cbCardIndex[i][j]>=3)
							{
								if((j%9==8 || j%9==0)&&j<MAX_INDEX-15)
								{
									cbHu[i] += 8;
								}
								else if((j%9!=8 || j%9!=0)&&j<MAX_INDEX-15)
								{
									cbHu[i] += 4;
								}
								else
								{
									cbHu[i] += 8;
								}
							}
						}

						for(BYTE j=MAX_INDEX-3-8;j<MAX_INDEX-8;j++)
						{
							if(m_cbCardIndex[i][j]==2)
							{
								cbHu[i] += 2;

							}
							if(m_cbCardIndex[i][j]>=3)
							{
								cbHua[i] += 1;

							}
						}
						//花牌
						if(i == m_wBankerUser)//东风位
						{
							if(m_cbCardIndex[i][27]==2)
								cbHu[i] += 2;

							if(m_cbCardIndex[i][27]>=3)
								cbHua[i] += 1;


						}

						if(i == (m_wBankerUser+m_wPlayerCount-1)%m_wPlayerCount)//南风位
						{
							if(m_cbCardIndex[i][28]==2)
								cbHu[i] += 2;
							if(m_cbCardIndex[i][28]>=3)
								cbHua[i] += 1;

	
						}
						if(i == (m_wBankerUser+m_wPlayerCount-2)%m_wPlayerCount)//西风位
						{
							if(m_cbCardIndex[i][29]==2)
								cbHu[i] += 2;
							if(m_cbCardIndex[i][29]>=3)
								cbHua[i] += 1;


						}
						if(i == (m_wBankerUser+m_wPlayerCount-3)%m_wPlayerCount)//北风位
						{
							if(m_cbCardIndex[i][30]==2)
								cbHu[i] += 2;	
							if(m_cbCardIndex[i][30]>=3)
								cbHua[i] += 1;

						}

					}

					LONG lTimes=0;
					m_GameLogic.CalScore(m_ChiHuResult[m_wProvideUser],lTimes);
					WORD  cbAllTai=(cbHu[m_wProvideUser]+m_ChiHuResult[m_wProvideUser].cbHuCount+60)*pow(2,cbHua[m_wProvideUser]+m_ChiHuResult[m_wProvideUser].cbHuTimes+lTimes);
					GameEnd.cbHua[m_wProvideUser]=cbHua[m_wProvideUser]+m_ChiHuResult[m_wProvideUser].cbHuTimes+lTimes;
					if(cbAllTai%10)
					{
						cbAllTai -= cbAllTai%10;
						cbAllTai += 10;
					}

					if(cbAllTai>500)
						cbAllTai=500;

					GameEnd.wHuCount[m_wProvideUser]=cbAllTai;

					//循环累计
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						//赢家过滤
						if (m_ChiHuResult[i].wChiHuKind!=CHK_NULL)	continue;	

						//自摸算分
						GameEnd.lGameScore[i]-=2*m_pGameServiceOption->lCellScore*cbAllTai;					
						GameEnd.lGameScore[m_wProvideUser] +=2*m_pGameServiceOption->lCellScore*cbAllTai;
					}
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						//赢家过滤
						if (m_ChiHuResult[i].wChiHuKind!=CHK_NULL)	continue;
						WORD cbAllTai=cbHu[i]*pow(2,cbHua[i]);
						if(cbAllTai%10)
						{
							cbAllTai -= cbAllTai%10;
							cbAllTai += 10;
						}
						if(cbAllTai>500)
							cbAllTai=500;
						GameEnd.cbHua[i]=cbHua[i]+m_ChiHuResult[i].cbHuTimes;
						GameEnd.wHuCount[i]=cbAllTai;

						if(i==m_wBankerUser)
						{
							for (WORD j=0;j<m_wPlayerCount;j++)
							{
								//赢家过滤
								if (m_ChiHuResult[j].wChiHuKind!=CHK_NULL)	continue;
								if(i==j) continue;

								GameEnd.lGameScore[i] += 2*m_pGameServiceOption->lCellScore*cbAllTai;
								GameEnd.lGameScore[j] -= 2*m_pGameServiceOption->lCellScore*cbAllTai;

							}

						}
						else
						{
							for (WORD j=0;j<m_wPlayerCount;j++)
							{
								//赢家过滤
								if (m_ChiHuResult[j].wChiHuKind!=CHK_NULL)	continue;

								if(i==j) continue;

								if(j==m_wBankerUser)
								{
									GameEnd.lGameScore[i] += 2*m_pGameServiceOption->lCellScore*cbAllTai;
									GameEnd.lGameScore[j] -= 2*m_pGameServiceOption->lCellScore*cbAllTai;
								}
								else
								{
									GameEnd.lGameScore[i] += m_pGameServiceOption->lCellScore*cbAllTai;
									GameEnd.lGameScore[j] -= m_pGameServiceOption->lCellScore*cbAllTai;
								}

							}

						}

						
					}
					//轮庄方式
					if(m_wProvideUser==m_wBankerUser)
					{
						m_wBankerUser=m_wBankerUser;
					}
					else
					{
						m_wBankerUser=(m_wBankerUser-1+m_wPlayerCount)%m_wPlayerCount;
					}

				}

				//捉炮类型
				if (m_ChiHuResult[m_wProvideUser].wChiHuKind==CHK_NULL)
				{
					//循环累计
					BYTE cbHua[4]={0,0,0,0};
					BYTE cbHu[4]={0,0,0,0};

					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						//输家过滤
						if (m_ChiHuResult[i].wChiHuKind==CHK_NULL) 		continue;

						wWinUser=i;
						if( i== m_wBankerUser) //东风位
						{
							if(m_cbUserWindCount[m_wBankerUser] == 8)
							{
								cbHua[m_wBankerUser] += 4;
								cbHu[m_wBankerUser] += 32;
							}
							else
							{
								cbHu[m_wBankerUser] += 4*m_cbUserWindCount[m_wBankerUser];
								m_GameLogic.SortCardList(m_cbUserWindData[m_wBankerUser],m_cbUserWindCount[m_wBankerUser]);
								
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[m_wBankerUser];bTemp++)
								{
									//梅 春 
									if((m_cbUserWindData[m_wBankerUser][bTemp]==0x38)||(m_cbUserWindData[m_wBankerUser][bTemp]==0x3C))
									{
										cbHua[m_wBankerUser] +=1;
										if(bTemp+3<m_cbUserWindCount[m_wBankerUser])
										{
											if(m_cbUserWindData[m_wBankerUser][bTemp]==0x38 &&m_cbUserWindData[m_wBankerUser][bTemp+3]==0x3B)
												cbHua[m_wBankerUser]++;
											if(m_cbUserWindData[m_wBankerUser][bTemp]==0x3C &&m_cbUserWindData[m_wBankerUser][bTemp+3]==0x3F)
												cbHua[m_wBankerUser]++;

										}
									}
								}
							}
						}

						if(i == (m_wBankerUser+m_wPlayerCount-1)%m_wPlayerCount)//南风位
						{
							if(m_cbUserWindCount[i] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{
									//兰 夏 
									if((m_cbUserWindData[i][bTemp]==0x39)||(m_cbUserWindData[i][bTemp]==0x3D))
										cbHua[i]++;

									if(bTemp+2<m_cbUserWindCount[i]&&bTemp>=1)
									{
										if(m_cbUserWindData[i][bTemp]==0x39 &&m_cbUserWindData[i][bTemp+2]==0x3B&&m_cbUserWindData[i][bTemp-1]==0x38)
											cbHua[i]++;
										if(m_cbUserWindData[i][bTemp]==0x3D &&m_cbUserWindData[i][bTemp+2]==0x3F&&m_cbUserWindData[i][bTemp-1]==0x3C)
											cbHua[i]++;

									}
								}


							}
						}
						if(i == (m_wBankerUser+m_wPlayerCount-2)%m_wPlayerCount)//西风位
						{
							if(m_cbUserWindCount[i] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{

									//秋菊
									if((m_cbUserWindData[i][bTemp]==0x3A)||(m_cbUserWindData[i][bTemp]==0x3E))
										cbHua[i] +=1;
									if(bTemp+1<m_cbUserWindCount[i]&&bTemp>=2)
									{
										if(m_cbUserWindData[i][bTemp]==0x3A &&m_cbUserWindData[i][bTemp+1]==0x3B &&m_cbUserWindData[i][bTemp-2]==0x38)
											cbHua[i]++;
										if(m_cbUserWindData[i][bTemp]==0x3E &&m_cbUserWindData[i][bTemp+1]==0x3F&&m_cbUserWindData[i][bTemp-2]==0x3C)
											cbHua[i]++;

									}
								}
							}

						}
						if(i == (m_wBankerUser+m_wPlayerCount-3)%m_wPlayerCount)//北风位
						{
							if(m_cbUserWindCount[i] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{

									//冬竹
									if((m_cbUserWindData[i][bTemp]==0x3B)||(m_cbUserWindData[i][bTemp]==0x3F))
										cbHua[i] +=1;
									if(bTemp-3<m_cbUserWindCount[i])
									{
										if(m_cbUserWindData[i][bTemp]==0x38 &&m_cbUserWindData[i][bTemp-3]==0x3B)
											cbHua[i]++;
										if(m_cbUserWindData[i][bTemp]==0x3C &&m_cbUserWindData[i][bTemp-3]==0x3F)
											cbHua[i]++;

									}
								}


							}
						}
						LONG lTimes=0;
						m_GameLogic.CalScore(m_ChiHuResult[i],lTimes);

						WORD  cbAllTai=(cbHu[i]+60+m_ChiHuResult[i].cbHuCount)*pow(2,cbHua[i]+m_ChiHuResult[i].cbHuTimes+lTimes);
						if(cbAllTai%10)
						{
							cbAllTai -= cbAllTai%10;
							cbAllTai += 10;
						}

						if(cbAllTai>500)
							cbAllTai=500;
						GameEnd.cbHua[i]=cbHua[i]+m_ChiHuResult[i].cbHuTimes+lTimes;
						GameEnd.wHuCount[i]=cbAllTai;

						for (WORD j=0;j<m_wPlayerCount;j++)
						{
							//赢家过滤
							if (m_ChiHuResult[j].wChiHuKind!=CHK_NULL) 		continue;
							//放炮算分
							if(j==m_wProvideUser)
							{
								GameEnd.lGameScore[j]-=2*m_pGameServiceOption->lCellScore*cbAllTai;
								GameEnd.lGameScore[i]+=2*m_pGameServiceOption->lCellScore*cbAllTai;
							}
							else
							{
								GameEnd.lGameScore[j]-=1*m_pGameServiceOption->lCellScore*cbAllTai;
								GameEnd.lGameScore[i]+=1*m_pGameServiceOption->lCellScore*cbAllTai;

							}
						}
						WORD wChiHuRight=0;
						if((!m_GameLogic.AnalyseTingCard(m_cbCardIndex[m_wProvideUser],m_WeaveItemArray[m_wProvideUser],m_cbWeaveItemCount[m_wProvideUser],wChiHuRight))&&(IsAfford(m_wProvideUser,wWinUser)>1))
						{
							for (WORD j=0;j<m_wPlayerCount;j++)
							{
								//输家过滤
								if (m_ChiHuResult[j].wChiHuKind!=CHK_NULL) 		continue;
								if(j==m_wProvideUser) continue;
								//放炮算分
								GameEnd.lGameScore[m_wProvideUser]+=GameEnd.lGameScore[j];
								GameEnd.lGameScore[j]=0;

							}

						}
					}
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						//赢家过滤
						if (m_ChiHuResult[i].wChiHuKind!=CHK_NULL) 		continue;	//牌型分析
						for (BYTE j=0;j<m_cbWeaveItemCount[i];j++)
						{
							BYTE cbWeaveKind=m_WeaveItemArray[i][j].cbWeaveKind;
							if(cbWeaveKind==WIK_PENG)
							{
								if((m_WeaveItemArray[i][j].cbCenterCard&MASK_COLOR)==0x30)
								{
									cbHu[i] +=4;
								}
								else
								{
									if((m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x01 ||(m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x09)
									{
										cbHu[i] +=4;
									}
									else
										cbHu[i] +=2;
								}
							}
							if(cbWeaveKind==WIK_GANG)
							{
								if(m_WeaveItemArray[i][j].cbPublicCard==FALSE)
								{
									if((m_WeaveItemArray[i][j].cbCenterCard&MASK_COLOR)==0x30)
									{
										cbHu[i] +=32;
									}
									else
									{
										if((m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x01 ||(m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x09)
										{
											cbHu[i] +=32;
										}
										else
											cbHu[i] +=16;
									}
								}
								if(m_WeaveItemArray[i][j].cbPublicCard==TRUE)
								{
									if((m_WeaveItemArray[i][j].cbCenterCard&MASK_COLOR)==0x30)
									{
										cbHu[i] +=16;
									}
									else
									{
										if((m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x01 ||(m_WeaveItemArray[i][j].cbCenterCard&MASK_VALUE)==0x09)
										{
											cbHu[i] +=16;
										}
										else
											cbHu[i] +=8;
									}
								}
							}



							if(m_WeaveItemArray[i][j].cbCenterCard>=0x35 &&m_WeaveItemArray[i][j].cbCenterCard<=0x37)
							{
								cbHua[i]+=1;
							}
							if(i == m_wBankerUser && m_WeaveItemArray[i][j].cbCenterCard==0x31)
							{
								cbHua[i] += 1;
							}
							if((i == (m_wBankerUser+m_wPlayerCount-1)%m_wPlayerCount) && m_WeaveItemArray[i][j].cbCenterCard==0x32)
							{
								cbHua[i] += 1;
							}
							if((i == (m_wBankerUser+m_wPlayerCount-2)%m_wPlayerCount) && m_WeaveItemArray[i][j].cbCenterCard==0x33)
							{
								cbHua[i] += 1;
							}
							if((i == (m_wBankerUser+m_wPlayerCount-3)%m_wPlayerCount) && m_WeaveItemArray[i][j].cbCenterCard==0x34)
							{
								cbHua[i] += 1;
							}
						}

						//花牌
						if(i == m_wBankerUser)//东风位
						{
							if(m_cbUserWindCount[m_wBankerUser] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								//正花 野花
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{
									//梅 春 正花
									if((m_cbUserWindData[i][bTemp]==0x38)||(m_cbUserWindData[i][bTemp]==0x3C))
									{
										cbHua[i] +=1;
										if(bTemp+3<m_cbUserWindCount[i])
										{
											if(m_cbUserWindData[i][bTemp]==0x38 &&m_cbUserWindData[i][bTemp+3]==0x3B)
												cbHua[i]++;
											if(m_cbUserWindData[i][bTemp]==0x3C &&m_cbUserWindData[i][bTemp+3]==0x3F)
												cbHua[i]++;

										}
									}
								}
							}
						}

						if(i == (m_wBankerUser+m_wPlayerCount-1)%m_wPlayerCount)//南风位
						{
							if(m_cbUserWindCount[i] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{
									//兰 夏 正花
									if((m_cbUserWindData[i][bTemp]==0x39)||(m_cbUserWindData[i][bTemp]==0x3D))
										cbHua[i]++;

									if(bTemp+2<m_cbUserWindCount[i]&&bTemp>=1)
									{
										if(m_cbUserWindData[i][bTemp]==0x39 &&m_cbUserWindData[i][bTemp+2]==0x3B&&m_cbUserWindData[i][bTemp-1]==0x38)
											cbHua[i]++;
										if(m_cbUserWindData[i][bTemp]==0x3D &&m_cbUserWindData[i][bTemp+2]==0x3F&&m_cbUserWindData[i][bTemp-1]==0x3C)
											cbHua[i]++;

									}
								}


							}
						}
						if(i == (m_wBankerUser+m_wPlayerCount-2)%m_wPlayerCount)//西风位
						{

							if(m_cbUserWindCount[i] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								//正花 野花
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{

									//梅 春 正花
									if((m_cbUserWindData[i][bTemp]==0x3A)||(m_cbUserWindData[i][bTemp]==0x3E))
										cbHua[i] +=1;
									if(bTemp+1<m_cbUserWindCount[i]&&bTemp>=2)
									{
										if(m_cbUserWindData[i][bTemp]==0x3A &&m_cbUserWindData[i][bTemp+1]==0x3B &&m_cbUserWindData[i][bTemp-2]==0x38)
											cbHua[i]++;
										if(m_cbUserWindData[i][bTemp]==0x3E &&m_cbUserWindData[i][bTemp+1]==0x3F&&m_cbUserWindData[i][bTemp-2]==0x3C)
											cbHua[i]++;

									}
								}


							}

						}
						if(i == (m_wBankerUser+m_wPlayerCount-3)%m_wPlayerCount)//北风位
						{

							if(m_cbUserWindCount[i] == 8)
							{
								cbHua[i] += 4;
								cbHu[i] += 32;
							}
							else
							{
								cbHu[i] += 4*m_cbUserWindCount[i];
								m_GameLogic.SortCardList(m_cbUserWindData[i],m_cbUserWindCount[i]);
								//正花 野花
								for (INT bTemp = 0;bTemp<m_cbUserWindCount[i];bTemp++)
								{

									//梅 春 正花
									if((m_cbUserWindData[i][bTemp]==0x3B)||(m_cbUserWindData[i][bTemp]==0x3F))
										cbHua[i] +=1;
									if(bTemp-3<m_cbUserWindCount[i])
									{
										if(m_cbUserWindData[i][bTemp]==0x38 &&m_cbUserWindData[i][bTemp-3]==0x3B)
											cbHua[i]++;
										if(m_cbUserWindData[i][bTemp]==0x3C &&m_cbUserWindData[i][bTemp-3]==0x3F)
											cbHua[i]++;

									}
								}


							}
						}

						for(BYTE j=0;j<MAX_INDEX-8;j++)
						{
							if(m_cbCardIndex[i][j]>=3)
							{
								if((j%9==8 || j%9==0)&&j<MAX_INDEX-15)
								{
									cbHu[i] += 8;
								}
								else if((j%9!=8 || j%9!=0)&&j<MAX_INDEX-15)
								{
									cbHu[i] += 4;
								}
								else
								{
									cbHu[i] += 8;
								}
							}
						}

						for(BYTE j=MAX_INDEX-3-8;j<MAX_INDEX-8;j++)
						{
							if(m_cbCardIndex[i][j]==2)
							{
								cbHu[i] += 2;

							}
							if(m_cbCardIndex[i][j]>=3)
							{
								cbHua[i] += 1;

							}
						}
						//字牌
						if(i == m_wBankerUser)//东风位
						{
							if(m_cbCardIndex[i][27]==2)
								cbHu[i] += 2;
							if(m_cbCardIndex[i][27]>=3)
								cbHua[i] += 1;


						}

						if(i == (m_wBankerUser+m_wPlayerCount-1)%m_wPlayerCount)//南风位
						{
							if(m_cbCardIndex[i][28]==2)
								cbHu[i] += 2;
							if(m_cbCardIndex[i][28]>=3)
								cbHua[i] += 1;


						}
						if(i == (m_wBankerUser+m_wPlayerCount-2)%m_wPlayerCount)//西风位
						{
							if(m_cbCardIndex[i][29]==2)
								cbHu[i] += 2;
							if(m_cbCardIndex[i][29]>=3)
								cbHua[i] += 1;


						}
						if(i == (m_wBankerUser+m_wPlayerCount-3)%m_wPlayerCount)//北风位
						{
							if(m_cbCardIndex[i][30]==2)
								cbHu[i] += 2;
							if(m_cbCardIndex[i][30]>=3)
								cbHua[i] += 1;

						}
						WORD  cbAllTai=cbHu[i]*pow(2,cbHua[i]);
						if(cbAllTai%10)
						{
							cbAllTai -= cbAllTai%10;
							cbAllTai += 10;
						}

						if(cbAllTai>500)
							cbAllTai=500;

						GameEnd.cbHua[i]=cbHua[i]+m_ChiHuResult[i].cbHuTimes;
						GameEnd.wHuCount[i]=cbAllTai;

						if(i==m_wBankerUser)
						{
							for (WORD j=0;j<m_wPlayerCount;j++)
							{
								//输家过滤
								if (m_ChiHuResult[j].wChiHuKind!=CHK_NULL) 		continue;
								if(j==i) continue;
								GameEnd.lGameScore[j]-=2*m_pGameServiceOption->lCellScore*cbAllTai;
								GameEnd.lGameScore[i]+=2*m_pGameServiceOption->lCellScore*cbAllTai;

							}
						}
						else
						{
							for (WORD j=0;j<m_wPlayerCount;j++)
							{
								//输家过滤
								if (m_ChiHuResult[j].wChiHuKind!=CHK_NULL) 		continue;
								if(j==i) continue;
								if(j==m_wBankerUser)
								{
									GameEnd.lGameScore[j]-=2*m_pGameServiceOption->lCellScore*cbAllTai;
									GameEnd.lGameScore[i]+=2*m_pGameServiceOption->lCellScore*cbAllTai;

								}
								else
								{
									GameEnd.lGameScore[j]-=1*m_pGameServiceOption->lCellScore*cbAllTai;
									GameEnd.lGameScore[i]+=1*m_pGameServiceOption->lCellScore*cbAllTai;

								}
							}
						}
					}


					
					//轮庄方式
					if(wWinUser==m_wBankerUser)
					{
						m_wBankerUser=m_wBankerUser;
					}
					else
					{
						m_wBankerUser=(m_wBankerUser-1+m_wPlayerCount)%m_wPlayerCount;
					}
				}
			}
			else
			{
				//荒庄处理
				GameEnd.cbProvideCard=0;
				GameEnd.cbHaiDiCard = m_cbProvideCard;	
				m_wBankerUser = (m_wBankerUser+m_wPlayerCount-1)%m_wPlayerCount;
			}
			//拷贝扑克
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}

			//计算税收
			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (GameEnd.lGameScore[i]>=1000L)
					{
						LONG lTempTax = GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/1000;
						GameEnd.lGameTax+=lTempTax;
						GameEnd.lGameScore[i] -= lTempTax;
					}
				}
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				ScoreInfo[i].lScore=GameEnd.lGameScore[i];
				if (GameEnd.lGameScore[i]==0L) 
					ScoreInfo[i].ScoreKind=enScoreKind_Draw;
				else
					ScoreInfo[i].ScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
			}
			//统计积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//写入积分
				m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore,ScoreInfo[i].lRevenue,ScoreInfo[i].ScoreKind);
			}
			//结束游戏
			m_pITableFrame->ConcludeGame();
			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//设置变量
			GameEnd.cbProvideCard = 255;
			GameEnd.wProvideUser=INVALID_CHAIR;
			GameEnd.lGameScore[wChairID]=-24*m_pGameServiceOption->lCellScore;
			
			//拷贝扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}
			//积分过滤
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
			const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();


			//通知消息
			TCHAR szMessage[128]=TEXT("");
			_snprintf(szMessage,CountArray(szMessage),TEXT("由于 [ %s ] 离开游戏，游戏结束"),pIServerUserItem->GetAccounts());
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				IServerUserItem * pISendUserItem=m_pITableFrame->GetServerUserItem(i);
				if (pISendUserItem!=NULL) 
					m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_INFO);
			}

			WORD wIndex=0;
			do
			{
				IServerUserItem * pISendUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
				if (pISendUserItem==NULL)
					break;
				m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_INFO);
			} while (true);

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfo;
			ScoreInfo.ScoreKind=enScoreKind_Flee;
			ScoreInfo.lScore=GameEnd.lGameScore[wChairID];
			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo.lScore,0,ScoreInfo.ScoreKind);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//设置变量
			GameEnd.wProvideUser=INVALID_CHAIR;

			//拷贝扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame();

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
	case GS_MJ_FREE:	//空闲状态
		{
			//变量定义
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//构造数据
			StatusFree.wBankerUser=m_wBankerUser;
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusFree.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_MJ_PLAY:	//游戏状态

		{
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wSiceCount=m_wSiceCount;
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;

			//状态变量
			StatusPlay.cbActionCard=m_cbProvideCard;
			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			StatusPlay.cbActionMask=(m_bResponse[wChiarID]==false)?m_cbUserAction[wChiarID]:WIK_NULL;
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));
			
			//历史记录
			StatusPlay.wOutCardUser=m_wOutCardUser;
			StatusPlay.cbOutCardData=m_cbOutCardData;
			CopyMemory(StatusPlay.cbDiscardCard,m_cbDiscardCard,sizeof(StatusPlay.cbDiscardCard));
			CopyMemory(StatusPlay.cbDiscardCount,m_cbDiscardCount,sizeof(StatusPlay.cbDiscardCount));
			CopyMemory(StatusPlay.cbUserWindCount,m_cbUserWindCount,sizeof(m_cbUserWindCount));
			CopyMemory(StatusPlay.cbUserWindCardData,m_cbUserWindData,sizeof(m_cbUserWindData));

			//组合扑克
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveCount,m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

			//扑克数据
			StatusPlay.cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex[wChiarID],StatusPlay.cbCardData);

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
	case SUB_C_OUT_CARD:		//出牌消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
			if (wDataSize!=sizeof(CMD_C_OutCard)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) 	return true;

			//消息处理
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			return OnUserOutCard(pUserData->wChairID,pOutCard->cbCardData);
		}
	case SUB_C_OPERATE_CARD:	//操作消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OperateCard));
			if (wDataSize!=sizeof(CMD_C_OperateCard))	return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) 	return true;

			//消息处理
			CMD_C_OperateCard * pOperateCard=(CMD_C_OperateCard *)pDataBuffer;
			return OnUserOperateCard(pUserData->wChairID,pOperateCard->cbOperateCode,pOperateCard->cbOperateCard);
		}
	case SUB_C_FINISH:
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) 	return true;

			return OnUserCallFinish(pUserData->wChairID);

		}
	case SUB_C_TRUSTEE:
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pDataBuffer;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();

			m_bTrustee[pUserData->wChairID]=pTrustee->bTrustee;
			CMD_S_Trustee Trustee;
			Trustee.bTrustee=pTrustee->bTrustee;
			Trustee.wChairID = pUserData->wChairID;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));

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

//用户坐下
bool __cdecl CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if ((bLookonUser==false)&&(m_wBankerUser==INVALID_CHAIR))
	{
		m_wBankerUser=pIServerUserItem->GetChairID();
	}

	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{

	//庄家设置
	if ((bLookonUser==false)&&(wChairID==m_wBankerUser))
	{
		m_wBankerUser=INVALID_CHAIR;

		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if ((i!=wChairID)&&(m_pITableFrame->GetServerUserItem(i)!=NULL))
			{
				m_wBankerUser=i;
				break;
			}
		}
	}

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY)
		return true;

	//错误断言
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//效验参数
	if (wChairID!=m_wCurrentUser) 	return false;
	if (m_GameLogic.IsValidCard(cbCardData)==false)	return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置变量
	m_bSendStatus=true;
	m_bEnjoinChiHu[wChairID]=true;
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;


	//吃胡清空
	for (WORD i =0;i<m_wPlayerCount;i++)
	{
		if (i==wChairID) continue;
		else
			m_bEnjoinChiHu[i] = false;
	}


	//允许胡碰
	m_cbEnjoinHuCard[wChairID]=0;
	
	//出牌记录
	m_cbOutCardCount++;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData;

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;

	//设置变量
	m_bSendStatus=true;
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	//用户切换
	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;
	m_wCurrentUser=(wChairID+m_wPlayerCount-1)%m_wPlayerCount;


	//响应判断
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//派发扑克
	if (bAroseAction==false)
		DispatchCardData(m_wCurrentUser,false);

	return true;
}

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY)	return true;

	//效验用户
	//ASSERT((wChairID==m_wCurrentUser)||(m_wCurrentUser==INVALID_CHAIR));
	if ((wChairID!=m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR)) return true;
	

	//被动动作
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//效验状态
		ASSERT(m_bResponse[wChairID]==false);
		ASSERT((cbOperateCode==WIK_NULL)||((m_cbUserAction[wChairID]&cbOperateCode)!=0));

		//效验状态
		if (m_bResponse[wChairID]==true) return false;
		if ((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)==0)) return false;

		//变量定义
		WORD wTargetUser=wChairID;
		BYTE cbTargetAction=cbOperateCode;

		//设置变量
		m_bResponse[wChairID]=true;
		m_cbPerformAction[wChairID]=cbOperateCode;
		m_cbOperateCard[wChairID]=(cbOperateCard==0)?m_cbProvideCard:cbOperateCard;

		//执行判断
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//获取动作
			BYTE cbUserAction=(m_bResponse[i]==false)?m_cbUserAction[i]:m_cbPerformAction[i];

			//优先级别
			BYTE cbUserActionRank=m_GameLogic.GetUserActionRank(cbUserAction);
			BYTE cbTargetActionRank=m_GameLogic.GetUserActionRank(cbTargetAction);

			//动作判断
			if (cbUserActionRank>cbTargetActionRank)
			{
				wTargetUser=i;
				cbTargetAction=cbUserAction;
			}
		}
		if (m_bResponse[wTargetUser]==false)	return true;

		//吃胡等待
		if (cbTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if ((m_bResponse[i]==false)&&(m_cbUserAction[i]&WIK_CHI_HU)) 
					return true;
			}
		}

		//吃胡执行判断
		if (cbTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//吃牌判断
				WORD wFirstUser=(m_wProvideUser+m_wPlayerCount-1-i)%m_wPlayerCount;
				if(m_cbPerformAction[wFirstUser]&WIK_CHI_HU)
				{
					//数据校验
					ASSERT(m_bResponse[wFirstUser] == true);
					if((m_bResponse[wFirstUser] == false)) return false;
					//找到首家用户 就退出循环
					wTargetUser = wFirstUser;
					break;
				}
			}
		}
		//放弃操作
		if (cbTargetAction==WIK_NULL)
		{
			//用户状态
			ZeroMemory(m_bResponse,sizeof(m_bResponse));
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
			ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

			//发送扑克
			DispatchCardData(m_wResumeUser,false);

			return true;
		}

		//变量定义
		BYTE cbTargetCard=m_cbOperateCard[wTargetUser];

		//出牌变量
		m_bSendStatus=true;

		//胡牌操作
		if (cbTargetAction==WIK_CHI_HU)
		{
			//结束信息
			m_cbChiHuCard=cbTargetCard;

			//吃牌权位
			WORD wChiHuRight=0;
			//胡牌判断
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//过虑判断
				if ((i==m_wProvideUser)||((m_cbPerformAction[i]&WIK_CHI_HU)==0))		continue;
				
				if(i!=wTargetUser) continue;

				//普通胡牌
				if (m_cbChiHuCard!=0)
				{
					//胡牌判断
					BYTE cbWeaveItemCount=m_cbWeaveItemCount[i];
					tagWeaveItem * pWeaveItem=m_WeaveItemArray[i];
					m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,wChiHuRight,m_ChiHuResult[i],(m_wBankerUser-i+GAME_PLAYER)%GAME_PLAYER,false,false);
	

					//插入扑克
					if (m_ChiHuResult[i].wChiHuKind!=CHK_NULL)
						m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
				}
			}
			//构造结果
			CMD_S_OperateResult OperateResult;
			OperateResult.wOperateUser=wTargetUser;
			OperateResult.wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;
			OperateResult.cbOperateCode=WIK_CHI_HU;
			OperateResult.cbOperateCard=cbOperateCard;

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));


			//结束游戏
			ASSERT(m_ChiHuResult[wTargetUser].wChiHuKind!=CHK_NULL);
			OnEventGameEnd(m_wProvideUser,NULL,GER_NORMAL);

			return true;
		}

		//用户状态
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
		ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
		ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

		//组合扑克
		ASSERT(m_cbWeaveItemCount[wTargetUser]<4);
		WORD wIndex=m_cbWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard=cbTargetCard;
		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind=cbTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//删除扑克
		switch (cbTargetAction)
		{
		case WIK_LEFT:		//上牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard+1,cbTargetCard+2};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));
				
				break;
			}
		case WIK_RIGHT:		//上牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard-2,cbTargetCard-1};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));


				break;
			}
		case WIK_CENTER:	//上牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard-1,cbTargetCard+1};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));


				

				break;
			}
		case WIK_PENG:		//碰牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));
				break;
			}
		case WIK_GANG:		//杠牌操作
			{
				//杠牌设置
				if ((m_cbSendCardCount==1)&&(m_cbOutCardData==0))
				{
					//删除扑克
					BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard};
					m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));
				}
				else
				{
					//删除扑克
					BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard};
					m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));
				}

				//杠牌次数
				m_cbGangCount++;
				m_cbRemainCardCount += 2;


				break;
			}
		}



		//构造结果
		CMD_S_OperateResult OperateResult;
		OperateResult.wOperateUser=wTargetUser;
		OperateResult.cbOperateCard=cbTargetCard;
		OperateResult.cbOperateCode=cbTargetAction;
		OperateResult.wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//设置状态
		if (cbTargetAction==WIK_GANG)
		{
			m_bGangStatus=true;
			//m_bEnjoinChiPeng[wTargetUser]=true;
		}

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

		//设置用户
		m_wCurrentUser=wTargetUser;

		//杠牌处理
		if (cbTargetAction==WIK_GANG)
		{
			//效验动作
			bool bAroseAction=EstimateUserRespond(wTargetUser,cbTargetCard,EstimatKind_GangCard);

			//发送扑克
			if (bAroseAction==false) 
				DispatchCardData(wTargetUser,true);

			return true;
		}

		//动作判断
		if (m_cbLeftCardCount>=m_cbRemainCardCount+1)
		{
			//杠牌判断
			tagGangCardResult GangCardResult;
			m_cbUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[m_wCurrentUser],
				m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],GangCardResult);

			//结果处理
			if (GangCardResult.cbCardCount>0)
			{
				//设置变量
				m_cbUserAction[m_wCurrentUser]|=WIK_GANG;

				//发送动作
				SendOperateNotify();
			}
		}
		return true;
	}

	//主动动作
	if (m_wCurrentUser==wChairID)
	{
		//效验操作
		ASSERT((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)!=0));
		if ((cbOperateCode==WIK_NULL)||((m_cbUserAction[wChairID]&cbOperateCode)==0)) return false;

		//扑克效验
		ASSERT((cbOperateCode==WIK_NULL)||(cbOperateCode==WIK_CHI_HU)||(m_GameLogic.IsValidCard(cbOperateCard)==true));
		if ((cbOperateCode!=WIK_NULL)&&(cbOperateCode!=WIK_CHI_HU)&&(m_GameLogic.IsValidCard(cbOperateCard)==false)) return false;

		//设置变量
		m_bSendStatus=true;
		m_cbUserAction[wChairID]=WIK_NULL;
		m_cbPerformAction[wChairID]=WIK_NULL;

		//执行动作
		switch (cbOperateCode)
		{
		case WIK_GANG:			//杠牌操作
			{
				//变量定义
				BYTE cbWeaveIndex=0xFF;
				BYTE cbCardIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard);
				bool bpublic=true;

				//杠牌处理
				if (m_cbCardIndex[wChairID][cbCardIndex]==1)
				{
					//寻找组合
					for (BYTE i=0;i<m_cbWeaveItemCount[wChairID];i++)
					{
						BYTE cbWeaveKind=m_WeaveItemArray[wChairID][i].cbWeaveKind;
						BYTE cbCenterCard=m_WeaveItemArray[wChairID][i].cbCenterCard;
						if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
						{
							cbWeaveIndex=i;
							break;
						}
					}

					//效验动作
					ASSERT(cbWeaveIndex!=0xFF);
					if (cbWeaveIndex==0xFF) return false;

					//组合扑克
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=TRUE;
					//m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
					bpublic=true;
				}
				else
				{
					//扑克效验
					ASSERT(m_cbCardIndex[wChairID][cbCardIndex]==4);
					if (m_cbCardIndex[wChairID][cbCardIndex]!=4) return false;

					//设置变量
					cbWeaveIndex=m_cbWeaveItemCount[wChairID]++;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=FALSE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
					bpublic=false;
				}


				//杠牌次数
				m_cbGangCount++;
				m_cbRemainCardCount += 2;
	

				//删除扑克
				m_cbCardIndex[wChairID][cbCardIndex]=0;

				//设置状态
				if (cbOperateCode==WIK_GANG)
				{
					m_bGangStatus=true;
					//m_bEnjoinChiPeng[wChairID]=true;
				}

				//构造结果
				CMD_S_OperateResult OperateResult;
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.cbOperateCode=cbOperateCode;
				OperateResult.cbOperateCard=cbOperateCard;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

				bool bAroseAction=false;
				//效验动作
				if(bpublic)
					 bAroseAction=EstimateUserRespond(wChairID,cbOperateCard,EstimatKind_GangCard);

				//发送扑克
				if (bAroseAction==false)
					DispatchCardData(wChairID,true);

				return true;
			}
		case WIK_CHI_HU:		//吃胡操作
			{
				//吃牌权位
				WORD wChiHuRight=0;

				//普通胡牌
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[wChairID];
				
				//为了判断自摸
				//先删除
				m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],m_cbSendCardData);
				m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID],pWeaveItem,cbWeaveItemCount,m_cbSendCardData,wChiHuRight,m_ChiHuResult[wChairID],(m_wBankerUser-wChairID+GAME_PLAYER)%GAME_PLAYER,false,true);
				m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

				//结束信息
				m_cbChiHuCard=m_cbProvideCard;
			
				//构造结果
				CMD_S_OperateResult OperateResult;
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.cbOperateCode=WIK_CHI_HU;
				OperateResult.cbOperateCard=cbOperateCard;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));


				//结束游戏
				OnEventGameEnd(m_wProvideUser,NULL,GER_NORMAL);

				return true;
			}
		}

		return true;
	}

	return false;
}

//发送操作
bool CTableFrameSink::SendOperateNotify()
{
	//发送提示
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbUserAction[i]!=WIK_NULL)
		{
			//构造数据
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.wResumeUser=m_wResumeUser;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.cbActionMask=m_cbUserAction[i];

			//发送数据
			m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		}
	}

	return true;
}

//派发扑克
bool CTableFrameSink::DispatchCardData(WORD wCurrentUser,bool bGang)
{
	//状态效验
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	if (wCurrentUser==INVALID_CHAIR) return false;	
	
	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData;
	}
	//海底判断
	if ((m_cbLeftCardCount<=m_cbRemainCardCount+1) && bGang)
	{
		//发送扑克
		m_cbSendCardCount++;
		m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];

		//判断花牌
		if(m_cbLeftCardCount>0)  
		{

			//判断花牌
			while ((m_cbSendCardData>=0x38)&&(m_cbSendCardData<=0x3F))
			{
				ASSERT(m_cbSendCardData>=0x38&&m_cbSendCardData<=0x3F);

				//强制出牌
				if (m_GameLogic.IsValidCard(m_cbSendCardData)==false) return false;

				//插入数据
				m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

				//构造数据
				CMD_S_SendCard SendCard;
				SendCard.wCurrentUser=wCurrentUser;
				SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
				SendCard.cbCardData=m_cbSendCardData;
				SendCard.bGang =true;

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FORCE_SEND_CARD,&SendCard,sizeof(SendCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FORCE_SEND_CARD,&SendCard,sizeof(SendCard));

				//删除扑克
				if (m_GameLogic.RemoveCard(m_cbCardIndex[wCurrentUser],m_cbSendCardData)==false)
				{
					ASSERT(FALSE);
					return false;
				}

				//出牌记录
				m_cbOutCardCount++;
				m_wOutCardUser=wCurrentUser;
				m_cbOutCardData=m_cbSendCardData;
				m_cbAllWindCount +=1;			
				//m_cbWindData[wCurrentUser][m_cbTempWinCount[wCurrentUser]++] = m_cbSendCardData;
				m_cbUserWindData[wCurrentUser][m_cbUserWindCount[wCurrentUser]++] = m_cbSendCardData;


				//构造数据
				CMD_S_OutCard OutCard;
				OutCard.wOutCardUser=wCurrentUser;
				OutCard.cbOutCardData=m_cbSendCardData;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FORCE_OUT_CARD,&OutCard,sizeof(OutCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FORCE_OUT_CARD,&OutCard,sizeof(OutCard));	

				//重新发牌
				m_cbSendCardCount++;
				m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
			}
		}
	}
	
	if((m_cbLeftCardCount<=m_cbRemainCardCount+1) &&bGang==false)
	{
		//设置变量
		m_wResumeUser=wCurrentUser;
		m_wCurrentUser=wCurrentUser;
		m_wProvideUser=INVALID_CHAIR;
		m_cbProvideCard=m_cbSendCardData;
		OnEventGameEnd(m_wProvideUser,NULL,GER_NORMAL);
		return true;
	}

	//荒庄结束
	if (m_cbLeftCardCount==0)
	{
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameEnd(m_wProvideUser,NULL,GER_NORMAL);

		return true;
	}
	
	//发牌处理
	if (m_bSendStatus==true)
	{
		//发送扑克
		m_cbSendCardCount++;
		m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
		
		//判断花牌
		if(m_cbLeftCardCount>0)  ////此时可能还有杠
		{

			//判断花牌
			while ((m_cbSendCardData>=0x38)&&(m_cbSendCardData<=0x3F))
			{
				ASSERT(m_cbSendCardData>=0x38&&m_cbSendCardData<=0x3F);

				//强制出牌
				if (m_GameLogic.IsValidCard(m_cbSendCardData)==false) return false;

				//插入数据
				m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

				//构造数据
				CMD_S_SendCard SendCard;
				SendCard.wCurrentUser=wCurrentUser;
				SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
				SendCard.cbCardData=m_cbSendCardData;
				SendCard.bGang=true;

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FORCE_SEND_CARD,&SendCard,sizeof(SendCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FORCE_SEND_CARD,&SendCard,sizeof(SendCard));

				//删除扑克
				if (m_GameLogic.RemoveCard(m_cbCardIndex[wCurrentUser],m_cbSendCardData)==false)
				{
					ASSERT(FALSE);
					return false;
				}

				//出牌记录
				m_cbOutCardCount++;
				m_wOutCardUser=wCurrentUser;
				m_cbOutCardData=m_cbSendCardData;
				m_cbAllWindCount +=1;
				m_cbUserWindData[wCurrentUser][m_cbUserWindCount[wCurrentUser]++] = m_cbSendCardData;
	
				//状态判断
				if (m_pITableFrame->GetGameStatus()== GS_MJ_PLAY)
				{
					//规则判断
					if(m_cbAllWindCount<8)
						m_cbRemainCardCount = 14 + (m_cbGangCount+m_cbAllWindCount)%2+m_cbGangCount*2;
					else
						m_cbRemainCardCount = 16 + (m_cbGangCount+m_cbAllWindCount)%2+m_cbGangCount*2;
				}

				//构造数据
				CMD_S_OutCard OutCard;
				OutCard.wOutCardUser=wCurrentUser;
				OutCard.cbOutCardData=m_cbSendCardData;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FORCE_OUT_CARD,&OutCard,sizeof(OutCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FORCE_OUT_CARD,&OutCard,sizeof(OutCard));

				//重新发牌
				m_cbSendCardCount++;
				m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
			}

		}

		m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

		//设置变量
		m_wProvideUser=wCurrentUser;
		m_cbProvideCard=m_cbSendCardData;
		
		tagGangCardResult GangCardResult;
		m_cbUserAction[wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[wCurrentUser],
				m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],GangCardResult);

		//牌型权位
		WORD wChiHuRight=0;

		//胡牌判断
		tagChiHuResult ChiHuResult;
		m_cbUserAction[wCurrentUser]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],0,wChiHuRight,ChiHuResult,(m_wBankerUser-wCurrentUser+GAME_PLAYER)%GAME_PLAYER,true,true);
	}

	//设置变量
	m_wCurrentUser=wCurrentUser;

	//构造数据
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
	SendCard.cbCardData=(m_bSendStatus==true)?m_cbSendCardData:0x00;

	SendCard.bGang = bGang;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	return true;
}

//响应判断
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//变量定义
	bool bAroseAction=false;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//动作判断
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//用户过滤
		if (wCenterUser==i) 		continue;

		//出牌类型
		if (EstimatKind==EstimatKind_OutCard)
		{
			//吃碰判断
			if (m_bEnjoinChiPeng[i]==false&&m_cbLeftCardCount>=m_cbRemainCardCount+1)
			{
				//碰牌判断
				m_cbUserAction[i]|=m_GameLogic.EstimatePengCard(m_cbCardIndex[i],cbCenterCard);

	

				//吃牌判断
				WORD wEatUser=(wCenterUser+m_wPlayerCount-1)%m_wPlayerCount;
				if (wEatUser==i)
					m_cbUserAction[i]|=m_GameLogic.EstimateEatCard(m_cbCardIndex[i],cbCenterCard);
			}
			m_cbUserAction[i]|=m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
		}

		//胡牌判断
		if (m_bEnjoinChiHu[i]==false)
		{
			//牌型权位
			WORD wChiHuRight=0;

			//吃胡判断
			tagChiHuResult ChiHuResult;
			BYTE cbWeaveCount=m_cbWeaveItemCount[i];
			if(m_cbUserWindCount[i] == 8)
			{
				m_cbUserAction[i]|=WIK_CHI_HU;
			}
			BYTE cbHandTai=0;
			BYTE cbHandFeng = 0;
			if(m_cbEnjoinHuCard[i] != cbCenterCard)
			{
				m_cbUserAction[i]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,wChiHuRight,ChiHuResult,(m_wBankerUser-i+GAME_PLAYER)%GAME_PLAYER,true,false);
			}

           
			//禁止胡牌
			if(m_cbUserAction[i]&WIK_CHI_HU)
			{
				m_cbEnjoinHuCard[i]=cbCenterCard;
			}
            
		}

		//结果判断
		if (m_cbUserAction[i]!=WIK_NULL) 
			bAroseAction=true;
	}

	//结果处理
	if (bAroseAction==true) 
	{
		//设置变量
		m_wProvideUser=wCenterUser;
		m_cbProvideCard=cbCenterCard;
		m_wResumeUser=m_wCurrentUser;
		m_wCurrentUser=INVALID_CHAIR;

		//发送提示
		SendOperateNotify();

		return true;
	}

	return false;
}
//派发扑克
bool CTableFrameSink::ForceDispatchCardData(WORD wCurrentUser)
{
	//状态效验
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	if (wCurrentUser==INVALID_CHAIR) return false;
	
	//发送扑克
	m_cbSendCardCount++;
	m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];

	//判断花牌
	if (m_cbSendCardData>=0x38&&m_cbSendCardData<=0x3F)
	{
		ASSERT(m_cbSendCardData>=0x38&&m_cbSendCardData<=0x3F);
		m_cbSumWindCount +=1;
		m_cbWindData[wCurrentUser][m_cbTempWinCount[wCurrentUser]++] = m_cbSendCardData;
		m_cbAllWindCount += 1;


	}
	m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	//设置变量
	m_wProvideUser=wCurrentUser;
	m_cbProvideCard=m_cbSendCardData;

	//设置变量
	m_wCurrentUser=wCurrentUser;

	//构造数据
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
	SendCard.cbCardData=m_cbSendCardData;
	SendCard.bGang = true;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FORCE_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FORCE_SEND_CARD,&SendCard,sizeof(SendCard));

	return true;
}
//强迫出牌
bool CTableFrameSink::ForceOutCardData(WORD wCurrentUser,BYTE cbCardData)
{
	ASSERT(m_wCurrentUser != wCurrentUser);

	if(m_wCurrentUser != wCurrentUser) return false;
	return true;

}

//检查3包 互包
BYTE CTableFrameSink::IsAfford(WORD wUserProvider,WORD wUserAccept)
{
	//校验数据
	ASSERT(wUserProvider<4);
	ASSERT(wUserProvider>=0);
	ASSERT(wUserAccept<4);
	ASSERT(wUserAccept>=0);

	if(m_cbWeaveItemCount[wUserAccept]<3) return 1;

	BYTE cbAfford=0;//1：不是包关系 2：包关系
	BYTE cbCount[4] = {0,0,0,0};
	//供三包
	for (BYTE cbTemp =0;cbTemp<m_cbWeaveItemCount[wUserAccept];cbTemp++)
	{
			cbCount[(m_WeaveItemArray[wUserAccept][cbTemp].cbCenterCard&MASK_COLOR)>>4]++;
	}
	if((cbCount[0]+cbCount[3]>=3)||(cbCount[1]+cbCount[3]>=3) ||(cbCount[2]+cbCount[3]>=3))
		cbAfford+=2;

	return cbAfford>1?cbAfford:1;
}

//叫牌完成
bool CTableFrameSink::OnUserCallFinish(WORD wChairID)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//用户效验
	ASSERT(m_bCallCard[wChairID]==false);
	if (m_bCallCard[wChairID]==true) return false;

	//变量设置
	m_bCallCard[wChairID]=true;

	//完成判断
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_bCallCard[i]==false) break;
	}
	if (i!=m_wPlayerCount) return true;
	//花牌替换
	do 
	{
		for (WORD i=0;i<m_wPlayerCount;i++)
		{	
			//用户索引
			WORD wCurrentUser = (m_wBankerUser-i+m_wPlayerCount)%m_wPlayerCount;

			//花牌i出牌
			for(BYTE j=0;j<m_cbWindCount[wCurrentUser];j++)
			{
				BYTE cbCardData = m_cbWindData[wCurrentUser][j];

				if (m_GameLogic.IsValidCard(cbCardData)==false)	return false;

				ASSERT(cbCardData>=0x38&&cbCardData<=0x3F);

				//删除扑克
				if (m_GameLogic.RemoveCard(m_cbCardIndex[wCurrentUser],cbCardData)==false)
				{
					ASSERT(FALSE);
					return false;
				}

				//出牌记录
				m_cbOutCardCount++;
				m_wOutCardUser=wCurrentUser;
				m_cbOutCardData=cbCardData;

				//构造数据
				CMD_S_OutCard OutCard;
				OutCard.wOutCardUser=wCurrentUser;
				OutCard.cbOutCardData=cbCardData;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FORCE_OUT_CARD,&OutCard,sizeof(OutCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FORCE_OUT_CARD,&OutCard,sizeof(OutCard));
				m_cbUserWindData[wCurrentUser][m_cbUserWindCount[wCurrentUser]++] = cbCardData;

				//替换花番
				if(ForceDispatchCardData(wCurrentUser) ==false)
				{
					ASSERT(FALSE);
					return false;
				}
			}
			m_cbSumWindCount -= m_cbWindCount[wCurrentUser];
			m_cbWindCount[wCurrentUser] = m_cbTempWinCount[wCurrentUser];
			m_cbTempWinCount[wCurrentUser]=0;

		}
	} while(m_cbSumWindCount>0);

	//动作分析
	bool bAroseAction=false;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//庄家判断
		if (i==m_wBankerUser)
		{
			//杠牌判断
			tagGangCardResult GangCardResult;
			m_cbUserAction[i]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[i],NULL,0,GangCardResult);

			//胡牌判断
			tagChiHuResult ChiHuResult;
			m_cbUserAction[i]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],NULL,0,0,0,ChiHuResult,0,true,true);
		}

		//状态设置
		if ((bAroseAction==false)&&(i!=m_wBankerUser)&&(m_cbUserAction[i]!=WIK_NULL))
		{
			bAroseAction=true;
			m_wResumeUser=m_wCurrentUser;
			m_wCurrentUser=INVALID_CHAIR;
		}
	}


	//当前用户
	m_wCurrentUser=m_wBankerUser;

	CMD_S_GameStart GameStart;

	//构造数据
	ZeroMemory(&GameStart,sizeof(GameStart));
	GameStart.wSiceCount=m_wSiceCount;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.cbSunWindCount=m_cbSumWindCount;
	GameStart.cbLeftCardCount = m_cbLeftCardCount;
	GameStart.bFirst=false;
	CopyMemory(GameStart.bTrustee,m_bTrustee,sizeof(m_bTrustee));

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//设置变量
		GameStart.cbUserAction=m_cbUserAction[i];
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData);
		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
