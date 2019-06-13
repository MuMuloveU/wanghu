#include "StdAfx.h"
#include "TableFrameSink.h"

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
	m_wSiceCount2=MAKEWORD(1,1);
	m_wSiceCount3=MAKEWORD(1,1);
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_bCallCard,sizeof(m_bCallCard));
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_cbBatch,sizeof(m_cbBatch));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory(m_bBatchEnd,sizeof(m_bBatchEnd));
	
	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	m_cbGangCount=0;
	m_BankContinueCount = 1;
	m_cbMustLeft =MUST_LEFT_COUNT;
	
	//历史积分
	memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
	memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));
	ZeroMemory(m_lGameScoreEx,sizeof(m_lGameScoreEx));

	//精牌信息
	FillMemory(m_cbKingCardIndex,sizeof(m_cbKingCardIndex),0xFF);
	ZeroMemory(m_cbKingCount,sizeof(m_cbKingCount));
	ZeroMemory(m_cbViceKingCount,sizeof(m_cbViceKingCount));
	ZeroMemory(m_cbOutKingScore,sizeof(m_cbOutKingScore));

	//上下番
	ZeroMemory(m_lUpDownScore,sizeof(m_lUpDownScore));
	m_cbUpDownKingCardData=0;

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
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

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_wUserAction,sizeof(m_wUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_wPerformAction,sizeof(m_wPerformAction));

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
	if (m_pITableFrame==NULL)	return false;

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
	m_wSiceCount2=MAKEWORD(1,1);
	m_wSiceCount3=MAKEWORD(1,1);
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bCallCard,sizeof(m_bCallCard));
	ZeroMemory(m_cbBatch,sizeof(m_cbBatch));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory(m_bBatchEnd,sizeof(m_bBatchEnd));

	//精牌索引
	//精牌信息
	FillMemory(m_cbKingCardIndex,sizeof(m_cbKingCardIndex),0xFF);
	ZeroMemory(m_cbKingCount,sizeof(m_cbKingCount));
	ZeroMemory(m_cbViceKingCount,sizeof(m_cbViceKingCount));
	ZeroMemory(m_cbOutKingScore,sizeof(m_cbOutKingScore));

	//上下番
	ZeroMemory(m_lUpDownScore,sizeof(m_lUpDownScore));
	m_cbUpDownKingCardData=0;

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	ZeroMemory(m_lGameScoreEx,sizeof(m_lGameScoreEx));
	m_cbGangCount=0;
	m_cbMustLeft =MUST_LEFT_COUNT;

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
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

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_wUserAction,sizeof(m_wUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_wPerformAction,sizeof(m_wPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(&m_ChiHuResult,sizeof(m_ChiHuResult));

	//王牌索引
	m_GameLogic.SetKingCardIndex(m_cbKingCardIndex);

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
	m_pITableFrame->SetGameStatus(GS_MJ_BATCH);	
	
	//	发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BATCH);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BATCH);

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

			//结束信息
			GameEnd.wProvideUser=m_wProvideUser;
			GameEnd.cbChiHuCard=m_cbChiHuCard;

			//上下番
			CopyMemory(GameEnd.lUpDownScore,m_lUpDownScore,sizeof(m_lUpDownScore));

			BYTE cbOverlordKing[4]={1,1,1,1};//霸王倍数
			BYTE cbHasKingCount=0;

			//判断霸王精
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbKingCount[i]+m_cbViceKingCount[i]>0)
				{
					cbHasKingCount++;
				}
				
			}

			//判断结果
			if(cbHasKingCount==1)
			{
				//找霸王玩家
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					if(m_cbKingCount[i]+m_cbViceKingCount[i]>0)
					{
						cbOverlordKing[i]=2;
						GameEnd.cbBaWangKing[i]=1;
					}

				}
			}
			
			//德中德
			if(cbHasKingCount<=0)
			{
				if((cbHasKingCount==0)&&(m_cbKingCount[wChairID]+m_cbViceKingCount[wChairID]>0&&cbHasKingCount==1))
				{
					if((m_ChiHuResult[wChairID].wChiHuRight&CHR_GERMAN )|| (m_ChiHuResult[wChairID].wChiHuKind&CHK_GERMAN_SERVEN))
					{

						m_ChiHuResult[wChairID].wChiHuRight |=CHR_GERMAN_GERMAN;
					}
				}
			}

			//判断霸王 冲关
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbKingCount[i]+m_cbViceKingCount[i]>0)
				{
					m_cbOutKingScore[i]=(2*m_cbKingCount[i]+m_cbViceKingCount[i]);
					if(m_cbOutKingScore[i]>=5)
					{
						GameEnd.cbChongguan[i]=m_cbOutKingScore[i]-3;
						m_cbOutKingScore[i] =(m_cbOutKingScore[i])*(m_cbOutKingScore[i]-3)*cbOverlordKing[i];
					}
					else
						m_cbOutKingScore[i] *=cbOverlordKing[i];

					//找碰 杠冲关
					WORD wProviderUser=INVALID_CHAIR;
					bool bGangChonguan=false;
					if( 2*m_cbKingCount[i]+m_cbViceKingCount[i]>=5)
					{
						for (BYTE t=0;t<m_cbWeaveItemCount[i];t++)
						{
							if((m_WeaveItemArray[i][t].wWeaveKind&(WIK_PENG|WIK_GANG)))
							{
								if(m_WeaveItemArray[i][t].wWeaveKind&(WIK_GANG)) bGangChonguan=true;
								if(m_WeaveItemArray[i][t].cbCenterCard==m_GameLogic.SwitchToCardData(m_cbKingCardIndex[0]))
								{
									if(m_WeaveItemArray[i][t].wProvideUser!=i)
										wProviderUser=m_WeaveItemArray[i][t].wProvideUser;
									if(wProviderUser!=INVALID_CHAIR)//不是自己 是其他玩家
										break;
								}
								if(m_WeaveItemArray[i][t].cbCenterCard==m_GameLogic.SwitchToCardData(m_cbKingCardIndex[1]))
								{
									if(m_WeaveItemArray[i][t].wProvideUser!=i)
										wProviderUser=m_WeaveItemArray[i][t].wProvideUser;
									if(wProviderUser!=INVALID_CHAIR)//不是自己 是其他玩家
										break;
								}
							}
						}
					}
					//没有冲关
					if(wProviderUser==INVALID_CHAIR)
					{
						//查找
						for(WORD j=0;(j<GAME_PLAYER);j++)
						{
							if(i!=j)
							{
								m_lGameScoreEx[i] += m_cbOutKingScore[i]*m_pGameServiceOption->lCellScore*m_BankContinueCount;
								m_lGameScoreEx[j] -= m_cbOutKingScore[i]*m_pGameServiceOption->lCellScore*m_BankContinueCount;
							}
						}

					}
					else//有冲关
					{
                        
						m_lGameScoreEx[i] += 3*m_cbOutKingScore[i]*m_pGameServiceOption->lCellScore*m_BankContinueCount;
						m_lGameScoreEx[wProviderUser] -= 3*m_cbOutKingScore[i]*m_pGameServiceOption->lCellScore*m_BankContinueCount;

						//包杠精
						if(bGangChonguan)
						{
							for (WORD j=0;j<GAME_PLAYER;j++)
							{
								if(j==i) continue;
								if(j==wProviderUser) continue;
								m_lGameScoreEx[wProviderUser] -=10*m_pGameServiceOption->lLessScore*m_BankContinueCount;
								m_lGameScoreEx[j] +=10*m_pGameServiceOption->lLessScore*m_BankContinueCount;
							}
						}
					}

				}		

			}


			//统计积分
			if (wChairID!=INVALID_CHAIR)
			{
				if(m_ChiHuResult[wChairID].wChiHuRight&(CHR_TIAN|CHR_DI))
				{
					if(m_ChiHuResult[wChairID].wChiHuRight&(CHR_TIAN))
					{
						m_ChiHuResult[wChairID].wChiHuRight =m_ChiHuResult[wChairID].wChiHuRight&(CHR_KING_WAIT|CHR_TIAN);
						m_ChiHuResult[wChairID].wChiHuKind=CHK_JI_HU;
					}
					else
					{
						m_ChiHuResult[wChairID].wChiHuRight =m_ChiHuResult[wChairID].wChiHuRight&(CHR_KING_WAIT|CHR_DI);
						m_ChiHuResult[wChairID].wChiHuKind=CHK_JI_HU;

					}
					LONG lScore=0L;
					LONG lTimes=0L;
					//翻数计算
					WORD wChiHuOrder=m_GameLogic.CalScore(m_ChiHuResult[wChairID],lScore,lTimes);

					//循环累计
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						//赢家过滤
						if (m_ChiHuResult[i].wChiHuKind!=CHK_NULL)	continue;	
					
						int t=max(m_BankContinueCount,1);


						//自摸算分
						GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(wChiHuOrder);
						GameEnd.lGameScore[wChairID]+=t*m_pGameServiceOption->lCellScore*(wChiHuOrder);

					}

				}
				else
				{
					//自摸类型
					if ((m_ChiHuResult[wChairID].wChiHuKind!=CHK_NULL) &&(wChairID == m_wProvideUser))
					{


						LONG lScore=0L;
						LONG lTimes=0L;
						//翻数计算
						WORD wChiHuOrder=m_GameLogic.CalScore(m_ChiHuResult[wChairID],lScore,lTimes);

						//循环累计
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							//赢家过滤
							if (m_ChiHuResult[i].wChiHuKind!=CHK_NULL)	continue;	


							if(m_wBankerUser == m_wProvideUser)//庄家胡牌
							{
								int t=max(m_BankContinueCount,1);


								//闲家胡：
								//1，	自摸：
								//	其他闲家出=自摸*2  德国*2  七对*2  *2 +5=21
								//	庄家出=庄家*2  自摸*2  德国*2  七对*2  *2 +5=37

								//德国七对
								if(m_ChiHuResult[wChairID].wChiHuKind&CHK_GERMAN_SERVEN)
								{
									//自摸算分
									GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(2*wChiHuOrder*2+5);
									GameEnd.lGameScore[m_wProvideUser]+=t*m_pGameServiceOption->lCellScore*(2*wChiHuOrder*2+5);
								}

								else if(m_ChiHuResult[wChairID].wChiHuRight&CHR_GERMAN)
								{
									//自摸算分
									GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(2*wChiHuOrder+5);
									GameEnd.lGameScore[m_wProvideUser]+=t*m_pGameServiceOption->lCellScore*(2*wChiHuOrder+5);
								}							
								else
								{
									//自摸算分
									GameEnd.lGameScore[i]-=2*t*m_pGameServiceOption->lCellScore*(wChiHuOrder);
									GameEnd.lGameScore[m_wProvideUser]+=2*t*m_pGameServiceOption->lCellScore*(wChiHuOrder);

								}
							} 
							else   //下家胡牌
							{
								int t=max(m_BankContinueCount,1);
								//自摸算分
								if(i==m_wBankerUser) // 庄家输分
								{
									//			庄家胡：
									//1.	自摸
									//	庄家出=庄家*2  自摸*2  德国*2  七对*2  *2 +5=37


									//德国七对
									if(m_ChiHuResult[wChairID].wChiHuKind&CHK_GERMAN_SERVEN)
									{
										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(2*wChiHuOrder*2+5);
										GameEnd.lGameScore[m_wProvideUser]+=t*m_pGameServiceOption->lCellScore*(2*wChiHuOrder*2+5);
									}

									else if(m_ChiHuResult[wChairID].wChiHuRight&CHR_GERMAN)
									{
										//自摸算分
										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(2*wChiHuOrder+5);
										GameEnd.lGameScore[m_wProvideUser]+=t*m_pGameServiceOption->lCellScore*(2*wChiHuOrder+5);
									}		
									else
									{
										GameEnd.lGameScore[i]-=2*t*m_pGameServiceOption->lCellScore*wChiHuOrder;
										GameEnd.lGameScore[m_wProvideUser]+=2*t*m_pGameServiceOption->lCellScore*wChiHuOrder;

									}
								}
								else				//下家输分	
								{
									//德国七对
									if(m_ChiHuResult[wChairID].wChiHuKind&CHK_GERMAN_SERVEN)
									{
										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(wChiHuOrder+5);
										GameEnd.lGameScore[m_wProvideUser]+=t*m_pGameServiceOption->lCellScore*(wChiHuOrder+5);
									}
									else if(m_ChiHuResult[wChairID].wChiHuRight&CHR_GERMAN_GERMAN)
									{
										//自摸算分
										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(wChiHuOrder+5);
										GameEnd.lGameScore[m_wProvideUser]+=t*m_pGameServiceOption->lCellScore*(wChiHuOrder+5);
									}
									else if(m_ChiHuResult[wChairID].wChiHuRight&CHR_GERMAN)
									{
										//自摸算分
										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(wChiHuOrder+5);
										GameEnd.lGameScore[m_wProvideUser]+=t*m_pGameServiceOption->lCellScore*(wChiHuOrder+5);
									}		
									else
									{
										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*wChiHuOrder;
										GameEnd.lGameScore[m_wProvideUser]+=t*m_pGameServiceOption->lCellScore*wChiHuOrder;

									}
								}
							}

						}

						//判断是否连庄
						if(wChairID == m_wBankerUser)
						{
							m_BankContinueCount++;
						}
						else
						{				
							m_BankContinueCount=max(m_BankContinueCount-1,1);
						}
						//庄家设置
						m_wBankerUser=m_wProvideUser;

					}

					//捉炮类型
					if ((m_ChiHuResult[wChairID].wChiHuKind!=CHK_NULL) &&(wChairID != m_wProvideUser))
					{		

						LONG lScore=0L;
						LONG lTimes=0L;

						//翻数计算
						WORD wChiHuOrder=m_GameLogic.CalScore(m_ChiHuResult[wChairID],lScore,lTimes);

						//循环累计
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							//过滤胡家
							if(m_ChiHuResult[i].wChiHuKind!=CHK_NULL) continue;

							WORD wTempChihuOrder=wChiHuOrder;
							if(i==m_wProvideUser) wTempChihuOrder *= 2;

							if(wChairID==m_wBankerUser) //庄家胡牌
							{
								int t=max(m_BankContinueCount,1);


								//德国七对
								if(m_ChiHuResult[wChairID].wChiHuKind&CHK_GERMAN_SERVEN)
								{
									//输赢算分
									if(i==m_wProvideUser)
									{

										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(2*wTempChihuOrder*2+5);
										GameEnd.lGameScore[wChairID] +=t*m_pGameServiceOption->lCellScore*(2*wTempChihuOrder*2+5);
									}
									else
									{
										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*2*wTempChihuOrder*2;
										GameEnd.lGameScore[wChairID] +=t*m_pGameServiceOption->lCellScore*2*wTempChihuOrder*2;

									}

								}
								else if(m_ChiHuResult[wChairID].wChiHuRight&CHR_GERMAN)
								{
									if(i==m_wProvideUser)
									{
										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(2*wTempChihuOrder+5);
										GameEnd.lGameScore[wChairID]+=t*m_pGameServiceOption->lCellScore*(2*wTempChihuOrder+5);
									}
									else
									{
										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*2*wTempChihuOrder;
										GameEnd.lGameScore[wChairID]+=t*m_pGameServiceOption->lCellScore*2*wTempChihuOrder;

									}

								}		
								else
								{

									//输赢算分
									GameEnd.lGameScore[i]-=2*t*m_pGameServiceOption->lCellScore*wTempChihuOrder;
									GameEnd.lGameScore[wChairID] +=2*t*m_pGameServiceOption->lCellScore*wTempChihuOrder;
								}
							}
							else  //下家胡牌
							{
								if(i==m_wBankerUser) //庄家输分
								{
									int t=max(m_BankContinueCount,1);

									//德国七对
									if(m_ChiHuResult[wChairID].wChiHuKind&CHK_GERMAN_SERVEN)
									{
										//输赢算分
										if(i==m_wProvideUser)
										{
											GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(2*wTempChihuOrder*2+5);
											GameEnd.lGameScore[wChairID] +=t*m_pGameServiceOption->lCellScore*(2*wTempChihuOrder*2+5);
										}
										else
										{
											GameEnd.lGameScore[i]-=2*t*m_pGameServiceOption->lCellScore*wTempChihuOrder*2;
											GameEnd.lGameScore[wChairID] +=2*t*m_pGameServiceOption->lCellScore*wTempChihuOrder*2;

										}

									}

									else if(m_ChiHuResult[wChairID].wChiHuRight&CHR_GERMAN)
									{
										//自摸算分
										if(i==m_wProvideUser)
										{
											GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(2*wTempChihuOrder+5);
											GameEnd.lGameScore[wChairID]+=t*m_pGameServiceOption->lCellScore*(2*wTempChihuOrder+5);
										}
										else
										{
											GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(2*wTempChihuOrder);
											GameEnd.lGameScore[wChairID]+=t*m_pGameServiceOption->lCellScore*(2*wTempChihuOrder);

										}
									}		
									else
									{

										//输赢算分
										GameEnd.lGameScore[i]-=2*t*m_pGameServiceOption->lCellScore*wTempChihuOrder;
										GameEnd.lGameScore[wChairID] +=2*t*m_pGameServiceOption->lCellScore*wTempChihuOrder;
									}
								}
								else				//下家输分
								{
									//输赢算分
									//爬楼
									int t=max(m_BankContinueCount,1);

									//德国七对
									if(m_ChiHuResult[wChairID].wChiHuKind&CHK_GERMAN_SERVEN)
									{
										//输赢算分
										if(i==m_wProvideUser)
										{
											GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(wTempChihuOrder*2+5);
											GameEnd.lGameScore[wChairID] +=t*m_pGameServiceOption->lCellScore*(wTempChihuOrder*2+5);
										}
										else
										{
											GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(wTempChihuOrder);
											GameEnd.lGameScore[wChairID] +=t*m_pGameServiceOption->lCellScore*(wTempChihuOrder);

										}
									}

									//德国七对
									else if(m_ChiHuResult[wChairID].wChiHuRight&CHR_GERMAN)
									{
										//输赢算分
										if(i==m_wProvideUser)
										{
											GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(wTempChihuOrder+5);
											GameEnd.lGameScore[wChairID] +=t*m_pGameServiceOption->lCellScore*(wTempChihuOrder+5);
										}
										else
										{
											GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*(wTempChihuOrder);
											GameEnd.lGameScore[wChairID] +=t*m_pGameServiceOption->lCellScore*(wTempChihuOrder);

										}

									}
									else
									{

										//输赢算分
										GameEnd.lGameScore[i]-=t*m_pGameServiceOption->lCellScore*wTempChihuOrder;
										GameEnd.lGameScore[wChairID] +=t*m_pGameServiceOption->lCellScore*wTempChihuOrder;
									}
								}
							}
						}

						//抢杠全包
						if(m_ChiHuResult[wChairID].wChiHuRight&CHR_QIANG_GANG)
						{


							for (WORD j=0;j<GAME_PLAYER;j++)
							{
								if(j==m_wProvideUser) continue;
								if(j==wChairID) continue;

								//包输钱
								GameEnd.lGameScore[j]+=GameEnd.lGameScore[i];
								GameEnd.lGameScore[i]+=0;
							}
						}

						//判断是否连庄
						if(wChairID == m_wBankerUser)
						{
							m_BankContinueCount++;
						}
						else
						{
							m_BankContinueCount=max(m_BankContinueCount-1,1);
							//庄家设置
							m_wBankerUser=(m_wBankerUser+1)%GAME_PLAYER;

						}

					}	

				}
			}
			else
			{
				//荒庄处理
				GameEnd.cbChiHuCard=0;
				m_wBankerUser=(m_wBankerUser+1)%GAME_PLAYER;
			}


			//胡牌得分
			CopyMemory(GameEnd.lGameHuScore,GameEnd.lGameScore,sizeof(GameEnd.lGameHuScore));

			//奖励分
			CopyMemory(GameEnd.lGameScoreEx,m_lGameScoreEx,sizeof(m_lGameScoreEx));

			//结束信息
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.dwChiHuKind[i]=m_ChiHuResult[i].wChiHuKind;
				GameEnd.dwChiHuRight[i]=m_ChiHuResult[i].wChiHuRight;
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i],MAX_COUNT);
			}
			//最终成绩
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.lGameScore[i] += m_lGameScoreEx[i]+m_lUpDownScore[i];
			}
			

			//金币房间
			if(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{

				//积分过滤
				LONG lDifference = 0;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
					const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();
					if((pUserScore->lScore < abs(GameEnd.lGameScore[i]))&&(GameEnd.lGameScore[i]<0)&&(pUserScore->lScore >0))
					{
						lDifference += abs(GameEnd.lGameScore[i])-pUserScore->lScore;
						GameEnd.lGameScore[i] =-pUserScore->lScore;
					}
				}
				
				BYTE cbWinCount=0;
				//积分调整
				for (WORD i=0; i<m_wPlayerCount; i++)
				{
					if(GameEnd.lGameScore[i] > 0)
					{
						cbWinCount++;
					}
				}
				//积分调整
				for (WORD i=0; i<m_wPlayerCount; i++)
				{
					if(GameEnd.lGameScore[i] > 0&&cbWinCount>0)
					{
						GameEnd.lGameScore[i] -= lDifference/cbWinCount;
					}
				}
			}

			//历史积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				m_lAllTurnScore[i]+=GameEnd.lGameScore[i];
				m_lLastTurnScore[i]=GameEnd.lGameScore[i];
				GameEnd.lAllTurnScore[i]=m_lAllTurnScore[i];
				GameEnd.lLastTurnScore[i]=m_lLastTurnScore[i];
			}
			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
	
				//设置积分
				ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
				if (GameEnd.lGameScore[i]>0L)
				{
					GameEnd.lGameScore[i]=ScoreInfoArray[i].lScore-GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/100L;
					ScoreInfoArray[i].lRevenue=GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenue/100L;
				}

				//设置胜负
				if (ScoreInfoArray[i].lScore==0L) ScoreInfoArray[i].ScoreKind=enScoreKind_Draw;
				else ScoreInfoArray[i].ScoreKind=(ScoreInfoArray[i].lScore>0L)?enScoreKind_Win:enScoreKind_Lost;
			}
			//统计积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//写入积分
				m_pITableFrame->WriteUserScore(i,ScoreInfoArray[i].lScore,ScoreInfoArray[i].lRevenue,ScoreInfoArray[i].ScoreKind);
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


			//设置变量
			GameEnd.cbChiHuCard = 255;
			GameEnd.wProvideUser=INVALID_CHAIR;
			GameEnd.lGameScore[wChairID] -=6*m_BankContinueCount*m_pGameServiceOption->lCellScore;

			BYTE cbOverlordKing[4]={1,1,1,1};//霸王倍数
			BYTE cbHasKingCount=0;

			//判断霸王精
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbKingCount[i]+m_cbViceKingCount[i]>0)
				{
					cbHasKingCount++;
				}

			}

			//判断结果
			if(cbHasKingCount==1)
			{
				//找霸王玩家
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					if(m_cbKingCount[i]+m_cbViceKingCount[i]>0)
					{
						cbOverlordKing[i]=2;
						GameEnd.cbBaWangKing[i]=1;

					}

				}
			}

			//判断霸王 冲关
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbKingCount[i]+m_cbViceKingCount[i]>0)
				{
					m_cbOutKingScore[i]=(2*m_cbKingCount[i]+m_cbViceKingCount[i]);
					if(m_cbOutKingScore[i]>=5)
					{
						GameEnd.cbChongguan[i]=m_cbOutKingScore[i]-3;
						m_cbOutKingScore[i] =(m_cbOutKingScore[i])*(m_cbOutKingScore[i]-3)*cbOverlordKing[i];
					}
					else
						m_cbOutKingScore[i] *=cbOverlordKing[i];

					//找碰 杠冲关
					WORD wProviderUser=INVALID_CHAIR;
					for (BYTE t=0;t<m_cbWeaveItemCount[i];t++)
					{
						if(m_WeaveItemArray[i][t].wWeaveKind&(WIK_PENG|WIK_GANG))
						{
							if(m_WeaveItemArray[i][t].cbCenterCard==m_GameLogic.SwitchToCardData(m_cbKingCardIndex[0]))
							{
								if(wProviderUser!=i)
									wProviderUser=m_WeaveItemArray[i][t].wProvideUser;
								if(wProviderUser!=INVALID_CHAIR)//不是自己 是其他玩家
									break;
							}
							if(m_WeaveItemArray[i][t].cbCenterCard==m_GameLogic.SwitchToCardData(m_cbKingCardIndex[1]))
							{
								if(wProviderUser!=i)
									wProviderUser=m_WeaveItemArray[i][t].wProvideUser;
								if(wProviderUser!=INVALID_CHAIR)//不是自己 是其他玩家
									break;
							}
						}
					}
					//没有冲关
					if(wProviderUser==INVALID_CHAIR)
					{
						//查找
						for(WORD j=0;(j<GAME_PLAYER);j++)
						{
							if(i!=j)
							{
								m_lGameScoreEx[i] += m_cbOutKingScore[i]*m_pGameServiceOption->lCellScore*m_BankContinueCount;
								m_lGameScoreEx[j] -= m_cbOutKingScore[i]*m_pGameServiceOption->lCellScore*m_BankContinueCount;
							}
						}

					}
					else//有冲关
					{

						m_lGameScoreEx[i] += 3*m_cbOutKingScore[i]*m_pGameServiceOption->lCellScore*m_BankContinueCount;
						m_lGameScoreEx[wProviderUser] -= 3*m_cbOutKingScore[i]*m_pGameServiceOption->lCellScore*m_BankContinueCount;

						//包杠精
						for (WORD j=0;j<GAME_PLAYER;j++)
						{
							if(j==i) continue;
							if(j==wProviderUser) continue;
							m_lGameScoreEx[wProviderUser] -=10*m_pGameServiceOption->lLessScore*m_BankContinueCount;
							m_lGameScoreEx[j] +=10*m_pGameServiceOption->lLessScore*m_BankContinueCount;
						}
					}

				}		

			}




			//胡牌分
			CopyMemory(GameEnd.lGameHuScore,GameEnd.lGameScore,sizeof(GameEnd.lGameHuScore));

			//总分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.lGameScore[i] += m_lGameScoreEx[i]+m_lUpDownScore[i];
			}
			
			//金币房间
			if(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{

				//积分过滤
				LONG lDifference = 0;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
					const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();
					if((pUserScore->lScore < abs(GameEnd.lGameScore[i]))&&(GameEnd.lGameScore[i]<0)&&(pUserScore->lScore >0))
					{
						lDifference += abs(GameEnd.lGameScore[i])-pUserScore->lScore;
						GameEnd.lGameScore[i] =-pUserScore->lScore;
					}
				}

				BYTE cbWinCount=0;
				//积分调整
				for (WORD i=0; i<m_wPlayerCount; i++)
				{
					if(GameEnd.lGameScore[i] > 0)
					{
						cbWinCount++;
					}
				}
				//积分调整
				for (WORD i=0; i<m_wPlayerCount; i++)
				{
					if(GameEnd.lGameScore[i] > 0&&cbWinCount>0)
					{
						GameEnd.lGameScore[i] -= lDifference/cbWinCount;
					}
				}
			}
			//历史积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				m_lAllTurnScore[i]+=GameEnd.lGameScore[i];
				m_lLastTurnScore[i]=GameEnd.lGameScore[i];
				GameEnd.lAllTurnScore[i]=m_lAllTurnScore[i];
				GameEnd.lLastTurnScore[i]=m_lLastTurnScore[i];
			}
			CopyMemory(GameEnd.lUpDownScore,m_lUpDownScore,sizeof(m_lUpDownScore));
			CopyMemory(GameEnd.lGameScoreEx,m_lGameScoreEx,sizeof(m_lGameScoreEx));
	
			//结束信息
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i],MAX_COUNT);
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfo;
			ScoreInfo.ScoreKind=enScoreKind_Flee;
			ScoreInfo.lScore=GameEnd.lGameScore[wChairID];
			//写入积分
			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo.lScore,0L,ScoreInfo.ScoreKind);


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
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i],MAX_COUNT);
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
			memset(&StatusFree,0,sizeof(StatusFree));

			//构造数据
			StatusFree.wBankerUser=m_wBankerUser;
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore*m_BankContinueCount;
			CopyMemory(StatusFree.bTrustee,m_bTrustee,sizeof(m_bTrustee));


			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_MJ_BATCH:	//游戏顶底
		{
			//变量定义
			CMD_S_StatusBatch StatusChip;
			ZeroMemory(&StatusChip,sizeof(StatusChip));

			//构造数据
			StatusChip.bComplete=(m_bCallCard[wChiarID]==true)?TRUE:FALSE;
			StatusChip.lCellScore=m_pGameServiceOption->lCellScore*m_BankContinueCount;
			CopyMemory(StatusChip.bTrustee,m_bTrustee,sizeof(m_bTrustee));
			//历史积分
			CopyMemory(StatusChip.lAllTurnScore,m_lAllTurnScore,sizeof(m_lAllTurnScore));
			CopyMemory(StatusChip.lLastTurnScore,m_lLastTurnScore,sizeof(m_lLastTurnScore));


			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusChip,sizeof(StatusChip));


		}
	case GS_MJ_PLAY:	//游戏状态
		{
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;
			StatusPlay.wSiceCount=m_wSiceCount;
			StatusPlay.wSiceCount2=m_wSiceCount2;
			StatusPlay.wSiceCount3=m_wSiceCount3;
			CopyMemory(StatusPlay.cbChip,m_cbBatch,sizeof(m_cbBatch));
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//上下番
			CopyMemory(StatusPlay.lUpDownScore,m_lUpDownScore,sizeof(m_lUpDownScore));
			StatusPlay.cbUpDownKingCardData=m_cbUpDownKingCardData;
			StatusPlay.cbFloor=max(m_BankContinueCount,1);


			//状态变量
			StatusPlay.cbActionCard=m_cbProvideCard;
			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			StatusPlay.wActionMask=(m_bResponse[wChiarID]==false)?m_wUserAction[wChiarID]:WIK_NULL;

			//历史记录
			StatusPlay.wOutCardUser=m_wOutCardUser;
			StatusPlay.cbOutCardData=m_cbOutCardData;
			CopyMemory(StatusPlay.cbDiscardCard,m_cbDiscardCard,sizeof(StatusPlay.cbDiscardCard));
			CopyMemory(StatusPlay.cbDiscardCount,m_cbDiscardCount,sizeof(StatusPlay.cbDiscardCount));
			//历史积分
			CopyMemory(StatusPlay.lAllTurnScore,m_lAllTurnScore,sizeof(m_lAllTurnScore));
			CopyMemory(StatusPlay.lLastTurnScore,m_lLastTurnScore,sizeof(m_lLastTurnScore));
			CopyMemory(StatusPlay.lUpDownScore,m_lUpDownScore,sizeof(m_lUpDownScore));

			//组合扑克
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveCount,m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

			//扑克数据
			CopyMemory(StatusPlay.cbKingCardIndex,m_cbKingCardIndex,sizeof(m_cbKingCardIndex));
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				StatusPlay.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],StatusPlay.cbCardData[i],MAX_COUNT);
				StatusPlay.cbSendCardData=((m_cbSendCardData!=0)&&(m_wProvideUser==wChiarID))?m_cbSendCardData:0x00;
			}


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
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			return OnUserOutCard(pUserData->wChairID,pOutCard->cbCardData);
		}

	case SUB_C_OPERATE_CARD:	//操作消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OperateCard));
			if (wDataSize!=sizeof(CMD_C_OperateCard)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_OperateCard * pOperateCard=(CMD_C_OperateCard *)pDataBuffer;
			return OnUserOperateCard(pUserData->wChairID,pOperateCard->wOperateCode,pOperateCard->cbOperateCard);
		}
	case SUB_C_BATCH:			//洗澡操作
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_BATCH));
			if (wDataSize!=sizeof(CMD_C_BATCH)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_BATCH * pChip=(CMD_C_BATCH *)pDataBuffer;
			return OnUserCallFinish(pUserData->wChairID,pChip->cbChipTimes);
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
	case SUB_C_BATCH_END:			//洗澡结束
		{
			//效验消息
			ASSERT(wDataSize==0);
			if (wDataSize!=0) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserBatchEnd(pUserData->wChairID);
		}
	case SUB_C_PASS_KING:		//经过王牌
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY)
			{
				ASSERT(FALSE);
				return true;
			}

			m_cbLeftCardCount--;
			if(m_cbLeftCardCount<m_cbMustLeft-1)
			{
				m_cbChiHuCard=0;
				m_wProvideUser=INVALID_CHAIR;
				OnEventGameEnd(INVALID_CHAIR,NULL,GER_NORMAL);
				return true;
			}

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
	if(bLookonUser==false)
	{
		m_BankContinueCount=1;
	}
	ZeroMemory(m_lAllTurnScore,sizeof(m_lAllTurnScore));
	ZeroMemory(m_lLastTurnScore,sizeof(m_lLastTurnScore));
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
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//错误断言
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//效验参数
	if (wChairID!=m_wCurrentUser) return false;
	if (m_GameLogic.IsValidCard(cbCardData)==false) return false;

	//删除扑克
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置变量
	m_bSendStatus=true;	
	m_bEnjoinChiHu[wChairID]=true;
	for (WORD i =0;i<m_wPlayerCount;i++)
	{
		if (i==wChairID) continue;
		else
			m_bEnjoinChiHu[i] = false;
	}
	m_wUserAction[wChairID]=WIK_NULL;
	m_wPerformAction[wChairID]=WIK_NULL;
	m_bGangStatus=false;

	//出牌记录
	m_cbOutCardCount++;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData;



	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	//用户切换
	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;
	m_wCurrentUser=(wChairID+m_wPlayerCount-1)%m_wPlayerCount;

	//响应判断
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//刚流泪
	/*if (m_bGangStatus==true)
	{
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if ((m_wUserAction[i]&WIK_CHI_HU)!=0) break;
		}
		if (i==m_wPlayerCount) 
		{
			m_bGangStatus=false;
		}
	}*/

	//派发扑克
	if (bAroseAction==false) DispatchCardData(m_wCurrentUser);

	return true;
}

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, WORD wOperateCode, BYTE cbOperateCard)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return false;

	//效验用户
	ASSERT((wChairID==m_wCurrentUser)||(m_wCurrentUser==INVALID_CHAIR));
	if ((wChairID!=m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR)) return true;

	//被动动作
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//效验状态
		ASSERT(m_bResponse[wChairID]==false);
		ASSERT((wOperateCode==WIK_NULL)||((m_wUserAction[wChairID]&wOperateCode)!=0));

		//效验状态
		if (m_bResponse[wChairID]==true)  	return true;
		if ((wOperateCode!=WIK_NULL)&&((m_wUserAction[wChairID]&wOperateCode)==0))	return true;

		//变量定义
		WORD wTargetUser=wChairID;
		WORD wTargetAction=wOperateCode;

		//设置变量
		m_bResponse[wChairID]=true;
		m_wPerformAction[wChairID]=wOperateCode;
		m_cbOperateCard[wChairID]=(cbOperateCard==0)?m_cbProvideCard:cbOperateCard;

		//执行判断
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//获取动作
			WORD wUserAction=(m_bResponse[i]==false)?m_wUserAction[i]:m_wPerformAction[i];

			//优先级别
			BYTE cbUserActionRank=m_GameLogic.GetUserActionRank(wUserAction);
			BYTE cbTargetActionRank=m_GameLogic.GetUserActionRank(wTargetAction);

			//动作判断
			if (cbUserActionRank>cbTargetActionRank)
			{
				wTargetUser=i;
				wTargetAction=wUserAction;
			}
		}
		if (m_bResponse[wTargetUser]==false)	return true;

		//吃胡等待
		if (wTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if ((m_bResponse[i]==false)&&(m_wUserAction[i]&WIK_CHI_HU))
					return true;
			}
		}
		//吃胡执行判断
		if (wTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//吃牌判断
				WORD wFirstUser=(m_wProvideUser+m_wPlayerCount-1-i)%m_wPlayerCount;
				if(m_wPerformAction[wFirstUser]&WIK_CHI_HU)
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
		if (wTargetAction==WIK_NULL)
		{
			//用户状态
			ZeroMemory(m_bResponse,sizeof(m_bResponse));
			ZeroMemory(m_wUserAction,sizeof(m_wUserAction));
			ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
			ZeroMemory(m_wPerformAction,sizeof(m_wPerformAction));

			//发送扑克
			DispatchCardData(m_wResumeUser);

			return true;
		}



		//变量定义
		BYTE cbTargetCard=m_cbOperateCard[wTargetUser];

		//出精如果有人吃碰杠胡就算别人的 否则算自己的
		//减少正精
		if(m_cbKingCardIndex[0]==m_GameLogic.SwitchToCardIndex(cbTargetCard))
		{
			ASSERT(m_cbKingCount[m_wOutCardUser]>=1);
			m_cbKingCount[m_wOutCardUser] -= 1;
			m_cbKingCount[wTargetUser] += 1;
		}
		//减少副精
		if(m_cbKingCardIndex[1]==m_GameLogic.SwitchToCardIndex(cbTargetCard))
		{
			ASSERT(m_cbViceKingCount[m_wOutCardUser]>=1);
			m_cbViceKingCount[m_wOutCardUser] -= 1;
			m_cbViceKingCount[wTargetUser] += 1;
		}
		//出牌变量
		m_cbOutCardData=0;
		m_bSendStatus=true;
		m_wOutCardUser=INVALID_CHAIR;
		m_bEnjoinChiHu[wTargetUser]=false;


		//胡牌操作
		if (wTargetAction==WIK_CHI_HU)
		{
			//结束信息
			m_cbChiHuCard=cbTargetCard;
			m_wProvideUser=m_wProvideUser;

			//吃牌权位
			WORD wChiHuRight=0;
			if (m_bGangStatus==true) wChiHuRight|=CHR_QIANG_GANG;
			if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==1)) wChiHuRight|=CHR_DI;

			//胡牌判断
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//过虑判断
				if ((i==m_wProvideUser)||((m_wPerformAction[i]&WIK_CHI_HU)==0))
					continue;

				if(i!=wTargetUser) continue;

				//普通胡牌
				if (m_cbChiHuCard!=0)
				{
					//胡牌判断
					BYTE cbWeaveItemCount=m_cbWeaveItemCount[i];
					tagWeaveItem * pWeaveItem=m_WeaveItemArray[i];
					m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,wChiHuRight,m_ChiHuResult[i]);

					//插入扑克
					if (m_ChiHuResult[i].wChiHuKind!=CHK_NULL) 
						m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
				}
			}

			//结束游戏
			ASSERT(m_ChiHuResult[wTargetUser].wChiHuKind!=CHK_NULL);
			OnEventGameEnd(wTargetUser,NULL,GER_NORMAL);

			return true;
		}

		//用户状态
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_wUserAction,sizeof(m_wUserAction));
		ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
		ZeroMemory(m_wPerformAction,sizeof(m_wPerformAction));

		//组合扑克
		ASSERT(m_cbWeaveItemCount[wTargetUser]<MAX_WEAVE);
		WORD wIndex=m_cbWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard=cbTargetCard;
		m_WeaveItemArray[wTargetUser][wIndex].wWeaveKind=wTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//删除扑克
		switch (wTargetAction)
		{
		case WIK_DNBL:		//东南北左	
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard+1,cbTargetCard+3};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;

			}
		case WIK_DNBC:		//东南北中
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard-1,cbTargetCard+2};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;

			}
		case WIK_DNBR:		//东南北右
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard-3,cbTargetCard-2};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;

			}
		case WIK_DXBL:		//东西北左
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard+2,cbTargetCard+3};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;

			}
		case WIK_DXBC:		//东西北中
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard-2,cbTargetCard+1};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;

			}
		case WIK_DXBR:		//东西北右
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbTargetCard-3,cbTargetCard-1};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;

			}
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
					//杠牌积分
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if (i==wTargetUser) 
							m_lGameScoreEx[i]+=6*m_pGameServiceOption->lCellScore*m_BankContinueCount;
						else 
							m_lGameScoreEx[i]-=2*m_pGameServiceOption->lCellScore*m_BankContinueCount;
					}

					if(m_cbKingCardIndex[1]==m_GameLogic.SwitchToCardIndex(cbTargetCard))
					{
						//杠牌积分  本来是10分的 由于前面的的杠牌+2 所以这里说8分
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							if (i==wTargetUser) 
								m_lGameScoreEx[i]+=24*m_pGameServiceOption->lCellScore*m_BankContinueCount;
							else 
								m_lGameScoreEx[i]-=8*m_pGameServiceOption->lCellScore*m_BankContinueCount;
						}

					}

				}
				else
				{
					//删除扑克
					BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard};
					m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

					//杠牌积分
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if (i==wTargetUser) 
							m_lGameScoreEx[i]+=6*m_pGameServiceOption->lCellScore*m_BankContinueCount;
						else 
							m_lGameScoreEx[i]-=2*m_pGameServiceOption->lCellScore*m_BankContinueCount;
					}
					if(m_cbKingCardIndex[1]==m_GameLogic.SwitchToCardIndex(cbTargetCard))
					{
						//杠牌积分
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							if (i==wTargetUser) 
								m_lGameScoreEx[i]+=24*m_pGameServiceOption->lCellScore*m_BankContinueCount;
							else 
								m_lGameScoreEx[i]-=8*m_pGameServiceOption->lCellScore*m_BankContinueCount;
						}

					}

				}
				m_cbGangCount++;
		
				break;
			}
		}

		//构造结果
		CMD_S_OperateResult OperateResult;
		OperateResult.wOperateUser=wTargetUser;
		OperateResult.cbOperateCard=cbTargetCard;
		OperateResult.wOperateCode=wTargetAction;
		OperateResult.wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//设置状态
		if (wTargetAction==WIK_GANG) 	m_bGangStatus=true;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

		//设置用户
		m_wCurrentUser=wTargetUser;

		//杠牌处理
		if (wTargetAction==WIK_GANG)
		{
			//效验动作
			bool bAroseAction=EstimateUserRespond(wTargetUser,cbTargetCard,EstimatKind_GangCard);

			//发送扑克
			if (bAroseAction==false) 
			{
				DispatchCardData(wTargetUser,false);
			}

			return true;
		}

		//动作判断
		if (m_cbLeftCardCount>m_cbMustLeft-1)
		{
			//杠牌判断
			tagGangCardResult GangCardResult;
			m_wUserAction[m_wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[m_wCurrentUser],
				m_WeaveItemArray[m_wCurrentUser],m_cbWeaveItemCount[m_wCurrentUser],GangCardResult);

			//结果处理
			if (GangCardResult.cbCardCount>0)
			{
				//设置变量
				m_wUserAction[m_wCurrentUser]|=WIK_GANG;

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
		ASSERT((wOperateCode!=WIK_NULL)&&((m_wUserAction[wChairID]&wOperateCode)!=0));
		if ((wOperateCode==WIK_NULL)||((m_wUserAction[wChairID]&wOperateCode)==0))		return true;

		//扑克效验
		ASSERT((wOperateCode==WIK_NULL)||(wOperateCode==WIK_CHI_HU)||(m_GameLogic.IsValidCard(cbOperateCard)==true));
		if ((wOperateCode!=WIK_NULL)&&(wOperateCode!=WIK_CHI_HU)&&(m_GameLogic.IsValidCard(cbOperateCard)==false)) 	return false;

		//设置变量
		m_bSendStatus=true;
		m_bEnjoinChiHu[m_wCurrentUser]=false;
		m_wUserAction[m_wCurrentUser]=WIK_NULL;
		m_wPerformAction[m_wCurrentUser]=WIK_NULL;

		bool bPublic=false;

		//执行动作
		switch (wOperateCode)
		{
		case WIK_GANG:			//杠牌操作
			{
				//变量定义
				BYTE cbWeaveIndex=0xFF;
				BYTE cbCardIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard);

				//杠牌处理
				if (m_cbCardIndex[wChairID][cbCardIndex]==1)
				{
					//寻找组合
					for (BYTE i=0;i<m_cbWeaveItemCount[wChairID];i++)
					{
						WORD wWeaveKind=m_WeaveItemArray[wChairID][i].wWeaveKind;
						BYTE cbCenterCard=m_WeaveItemArray[wChairID][i].cbCenterCard;
						if ((cbCenterCard==cbOperateCard)&&(wWeaveKind==WIK_PENG))
						{
							bPublic=true;
							cbWeaveIndex=i;
							break;
						}
					}

					//效验动作
					ASSERT(cbWeaveIndex!=0xFF);
					if (cbWeaveIndex==0xFF) return false;


					//组合扑克
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=TRUE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wWeaveKind=wOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;

					//杠牌积分
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if (i==wChairID) 
							m_lGameScoreEx[i]+=6*m_pGameServiceOption->lCellScore*m_BankContinueCount;
						else 
							m_lGameScoreEx[i]-=2*m_pGameServiceOption->lCellScore*m_BankContinueCount;
					}
					if(m_cbKingCardIndex[1]==cbCardIndex)
					{
						//杠牌积分
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							if (i==wChairID) 
								m_lGameScoreEx[i]+=24*m_pGameServiceOption->lCellScore*m_BankContinueCount;
							else 
								m_lGameScoreEx[i]-=8*m_pGameServiceOption->lCellScore*m_BankContinueCount;
						}

					}
	
				}
				else
				{
					//扑克效验
					ASSERT(m_cbCardIndex[wChairID][cbCardIndex]==4);
					if (m_cbCardIndex[wChairID][cbCardIndex]!=4) 	return false;

					//设置变量
					bPublic=false;
					cbWeaveIndex=m_cbWeaveItemCount[wChairID]++;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=FALSE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wWeaveKind=wOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
					//杠牌积分
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if (i==wChairID) 
							m_lGameScoreEx[i]+=6*m_pGameServiceOption->lCellScore*m_BankContinueCount;
						else 
							m_lGameScoreEx[i]-=2*m_pGameServiceOption->lCellScore*m_BankContinueCount;
					}
					if(m_cbKingCardIndex[1]==cbCardIndex)
					{
						//杠牌积分
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							if (i==wChairID) 
								m_lGameScoreEx[i]+=24*m_pGameServiceOption->lCellScore*m_BankContinueCount;
							else 
								m_lGameScoreEx[i]-=8*m_pGameServiceOption->lCellScore*m_BankContinueCount;
						}

					}

				}

				//杠牌数目
				m_cbGangCount++;

				//删除扑克
				m_cbCardIndex[wChairID][cbCardIndex]=0;

				//设置状态
				if (wOperateCode==WIK_GANG) m_bGangStatus=true;

				//构造结果
				CMD_S_OperateResult OperateResult;
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				OperateResult.wOperateCode=wOperateCode;
				OperateResult.cbOperateCard=cbOperateCard;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

				//效验动作
				bool bAroseAction=false;
				if (bPublic==true) bAroseAction=EstimateUserRespond(wChairID,cbOperateCard,EstimatKind_GangCard);

				//发送扑克
				if (bAroseAction==false)
				{
					DispatchCardData(wChairID,false);
				}

				return true;
			}
		case WIK_CHI_HU:		//吃胡操作
			{
				//吃牌权位
				WORD wChiHuRight=0;
				if (m_bGangStatus==true) wChiHuRight|=CHR_QIANG_GANG;
				if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==1)) wChiHuRight|=CHR_DI;
				if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==0))
				{
					wChiHuRight|=CHR_TIAN;
					m_wProvideUser = m_wCurrentUser;
				}

				//结束信息
				m_cbChiHuCard=m_cbProvideCard;

				//普通胡牌
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[wChairID];
				BYTE cbTempCardIndex[MAX_INDEX];
				CopyMemory(cbTempCardIndex,m_cbCardIndex[wChairID],sizeof(cbTempCardIndex));
				if(m_GameLogic.RemoveCard(cbTempCardIndex,m_cbChiHuCard)==false)
				{
					ASSERT(FALSE);
				}

				m_GameLogic.AnalyseChiHuCard(cbTempCardIndex,pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,wChiHuRight,m_ChiHuResult[wChairID],true);



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
		if (m_wUserAction[i]!=WIK_NULL)
		{
			//构造数据
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.wResumeUser=m_wResumeUser;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.wActionMask=m_wUserAction[i];

			//发送数据
			m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		}
	}

	return true;
}

//派发扑克
bool CTableFrameSink::DispatchCardData(WORD wCurrentUser,bool bNotGang)
{
	//状态效验
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	if (wCurrentUser==INVALID_CHAIR)	return false;

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData;
	}	

	//设置变量
	m_cbOutCardData=0;
	m_wCurrentUser=wCurrentUser;
	m_wOutCardUser=INVALID_CHAIR;
	m_bEnjoinChiHu[wCurrentUser]=false;

	if (m_cbLeftCardCount<=m_cbMustLeft-1)
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


		//发送正精
		if(m_cbKingCardIndex[0]==m_GameLogic.SwitchToCardIndex(m_cbSendCardData))
		{
			m_cbKingCount[wCurrentUser] += 1;
		}
		//发送副精
		if(m_cbKingCardIndex[1]==m_GameLogic.SwitchToCardIndex(m_cbSendCardData))
		{
			m_cbViceKingCount[wCurrentUser] += 1;
		}

		//插入数据
		m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

		//设置变量
		m_wProvideUser=wCurrentUser;
		m_cbProvideCard=m_cbSendCardData;

		//杠牌判断
		if (m_cbLeftCardCount>m_cbMustLeft-1)
		{
			tagGangCardResult GangCardResult;
			m_wUserAction[wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[wCurrentUser],
				m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],GangCardResult);
		}

		//牌型权位
		WORD wChiHuRight=0;
		if (m_bGangStatus==true) wChiHuRight|=CHR_QIANG_GANG;

		//胡牌判断
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[m_wCurrentUser],sizeof(cbTempCardIndex));
		if(m_GameLogic.RemoveCard(cbTempCardIndex,m_cbSendCardData)==false)
		{
			ASSERT(FALSE);
		}
		tagChiHuResult ChiHuResult;
		m_wUserAction[wCurrentUser]|=m_GameLogic.AnalyseChiHuCard(cbTempCardIndex,
			m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],m_cbSendCardData,wChiHuRight,ChiHuResult,true);

	}

	//构造数据
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.wActionMask=m_wUserAction[wCurrentUser];
	SendCard.cbCardData=(m_bSendStatus==true)?m_cbSendCardData:0x00;
	SendCard.cbIsNotGang=bNotGang;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	//荒庄结束 因为精牌事先删除一个 所以要多减一个
	if ((m_cbLeftCardCount<=m_cbMustLeft-1)&&(!(m_wUserAction[wCurrentUser]&WIK_CHI_HU)))
	{
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameEnd(m_wProvideUser,NULL,GER_NORMAL);

		return true;
	}

	return true;
}

//响应判断
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//变量定义
	bool bAroseAction=false;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_wUserAction,sizeof(m_wUserAction));
	ZeroMemory(m_wPerformAction,sizeof(m_wPerformAction));

	//动作判断
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//用户过滤
		if (wCenterUser==i) continue;

		//出牌类型
		if (EstimatKind==EstimatKind_OutCard)
		{
			//吃碰判断

				//碰牌判断
				m_wUserAction[i]|=m_GameLogic.EstimatePengCard(m_cbCardIndex[i],cbCenterCard);

				//吃牌判断
				WORD wEatUser=(wCenterUser+m_wPlayerCount-1)%m_wPlayerCount;
				if (wEatUser==i)
					m_wUserAction[i]|=m_GameLogic.EstimateEatCard(m_cbCardIndex[i],cbCenterCard);


				//杠牌判断
				if (m_cbLeftCardCount>m_cbMustLeft-1) 
				{
					m_wUserAction[i]|=m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
				}
		}

		//胡牌判断
		if (m_bEnjoinChiHu[i]==false)
		{
			//牌型权位
			WORD wChiHuRight=0;
			if (m_bGangStatus==true) 
				wChiHuRight|=CHR_QIANG_GANG;
			if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==1))
				wChiHuRight|=CHR_DI;
			if ((m_cbSendCardCount==1)&&(m_cbOutCardCount==0))
				wChiHuRight|=CHR_TIAN;

			//吃胡判断
			tagChiHuResult ChiHuResult;
			BYTE cbWeaveCount=m_cbWeaveItemCount[i];
			m_wUserAction[i]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,wChiHuRight,ChiHuResult);

			if(m_GameLogic.IsKingWait(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,false)&&wChiHuRight!=CHR_DI)
			{
				m_wUserAction[i] &=~WIK_CHI_HU;
			}
			//吃胡限制
			if ((m_wUserAction[i]&WIK_CHI_HU)!=0) 
				m_bEnjoinChiHu[i]=true;
		}

		//结果判断
		if (m_wUserAction[i]!=WIK_NULL) 
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
//叫牌完成
bool CTableFrameSink::OnUserCallFinish(WORD wChairID,BYTE cbChipTiems)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_BATCH);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_BATCH) return true;

	//用户效验
	ASSERT(m_bCallCard[wChairID]==false);
	if (m_bCallCard[wChairID]==true) return false;

	ASSERT(cbChipTiems>=0&&cbChipTiems<=1);
	if (cbChipTiems>=2)
	{
		ASSERT(FALSE);
		return  false;
	}

	//变量设置
	m_bCallCard[wChairID]=true;
	m_cbBatch[wChairID]=cbChipTiems;

	//完成判断
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_bCallCard[i]==false) break;
	}
	if (i!=m_wPlayerCount) return true;


	//设置状态
	m_pITableFrame->SetGameStatus(GS_MJ_PLAY);

	//构造数据
	CMD_S_BatchResult BatchResult;
	CopyMemory(BatchResult.cbBatchResult,m_cbBatch,sizeof(m_cbBatch));

	//发送数据
	//	发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BATCHRESULT,&BatchResult,sizeof(BatchResult));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BATCHRESULT,&BatchResult,sizeof(BatchResult));

	//有无洗澡
	if(m_cbBatch[0]+m_cbBatch[1]+m_cbBatch[2]+m_cbBatch[3]==0)
	{
		//洗澡完毕
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_bBatchEnd[i]=true;
		}

		//游戏开始
		OnEventGameStart2();

	}
	return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart2()
{
	//预留牌数
	m_cbMustLeft =MUST_LEFT_COUNT;

	//混乱扑克
	m_GameLogic.RandCardData(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));

	m_wSiceCount=MAKEWORD(rand()%6+1,rand()%6+1);
	m_wSiceCount2=MAKEWORD(rand()%6+1,rand()%6+1);
	m_wSiceCount3=MAKEWORD(rand()%6+1,rand()%6+1);

	//设置精牌
	BYTE m_cbKingBrothterPos = (HIBYTE(m_wSiceCount3)+LOBYTE(m_wSiceCount3))*2-2;
	//正精
	BYTE cbKingCardData=m_cbRepertoryCard[m_cbKingBrothterPos];
	//副精
	BYTE cbViceKingCardData = m_GameLogic.GetKingFromBrother(m_cbRepertoryCard[m_cbKingBrothterPos]);

	//上下番精牌
	//正精
	BYTE cbUpDownKingData=m_cbRepertoryCard[m_cbKingBrothterPos+1];
	//副精
	BYTE cbUpDownViceKingData=m_GameLogic.GetKingFromBrother(cbUpDownKingData);

	//索引
	BYTE cbUpDownKingIndex=m_GameLogic.SwitchToCardIndex(cbUpDownKingData);
	BYTE cbUpDownViceKingIndex=m_GameLogic.SwitchToCardIndex(cbUpDownViceKingData);

	m_cbKingCardIndex[0]=m_GameLogic.SwitchToCardIndex(cbKingCardData);
	m_cbKingCardIndex[1]=m_GameLogic.SwitchToCardIndex(cbViceKingCardData);

	//初设王牌
	m_GameLogic.SetKingCardIndex(m_cbKingCardIndex);
	m_cbLeftCardCount=CountArray(m_cbRepertoryCard);

	m_GameLogic.SetKingCardIndex(m_cbKingCardIndex);


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
		//特殊胡牌
		//m_cbUserAction[i]|=m_GameLogic.EstimateChiHu(m_cbCardIndex[i]);

		//庄家判断
		if (i==m_wBankerUser)
		{
			//杠牌判断
			tagGangCardResult GangCardResult;
			m_wUserAction[i]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[i],NULL,0,GangCardResult);

			BYTE cbTempCardIndex[MAX_INDEX];
			CopyMemory(cbTempCardIndex,m_cbCardIndex[i],sizeof(cbTempCardIndex));
			if(m_GameLogic.RemoveCard(cbTempCardIndex,m_cbSendCardData)==false)
			{
				ASSERT(FALSE);
			}
			//胡牌判断
			tagChiHuResult ChiHuResult;
			m_wUserAction[i]|=m_GameLogic.AnalyseChiHuCard(cbTempCardIndex,NULL,0,m_cbSendCardData,0,ChiHuResult,true);
		}

		//状态设置
		if ((bAroseAction==false)&&(i!=m_wBankerUser)&&(m_wUserAction[i]!=WIK_NULL))
		{
			bAroseAction=true;
			m_wResumeUser=m_wCurrentUser;
			m_wCurrentUser=INVALID_CHAIR;
		}
	}

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.wSiceCount=m_wSiceCount;
	GameStart.wSiceCount2=m_wSiceCount2;
	GameStart.wSiceCount3=m_wSiceCount3;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser=m_wCurrentUser;
	CopyMemory(GameStart.cbKingCardIndex,m_cbKingCardIndex,sizeof(m_cbKingCardIndex));
	CopyMemory(GameStart.cbChip,m_cbBatch,sizeof(m_cbBatch));
	CopyMemory(GameStart.bTrustee,m_bTrustee,sizeof(m_bTrustee));
	
	BYTE cbUpDownKingCount[]={0,0,0,0};
	BYTE cbUpDownViceKingCount[]={0,0,0,0};

	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		for (BYTE j=0;j<MAX_INDEX;j++)
		{
			//计算上下番积分

			if(j==cbUpDownViceKingIndex && m_cbCardIndex[i][j]>0)
			{
				cbUpDownViceKingCount[i] +=m_cbCardIndex[i][j];	
			}
			if(j==cbUpDownKingIndex && m_cbCardIndex[i][j]>0)
			{
				cbUpDownKingCount[i] +=m_cbCardIndex[i][j];
			}
		}

	}
	//计算上下番 霸王精 冲关
	BYTE cbOverlordKing[4]={1,1,1,1};//霸王倍数
	BYTE cbHasKingCount=0;
	WORD wUpDownKingScore[]={0,0,0,0};

	//判断霸王精
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(cbUpDownKingCount[i]+cbUpDownViceKingCount[i]>0)
		{
			cbHasKingCount++;
		}

	}

	//判断结果
	if(cbHasKingCount==1)
	{
		//找霸王玩家
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if(cbUpDownKingCount[i]+cbUpDownViceKingCount[i]>0)
			{
				cbOverlordKing[i]=2;
			}

		}
	}

	//判断霸王 冲关
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(cbUpDownKingCount[i]+cbUpDownViceKingCount[i]>0)
		{
			wUpDownKingScore[i]=(2*cbUpDownKingCount[i]+cbUpDownViceKingCount[i]);
			if(wUpDownKingScore[i]>=5)
			{
				wUpDownKingScore[i] =(wUpDownKingScore[i])*(wUpDownKingScore[i]-3)*cbOverlordKing[i];
			}
			else
				wUpDownKingScore[i] *=cbOverlordKing[i];

			//查找
			for(WORD j=0;(j<GAME_PLAYER);j++)
			{
				if(i!=j)
				{
					m_lUpDownScore[i] += wUpDownKingScore[i]*m_pGameServiceOption->lCellScore*m_BankContinueCount;
					m_lUpDownScore[j] -= wUpDownKingScore[i]*m_pGameServiceOption->lCellScore*m_BankContinueCount;
				}
			}
		}
	}


	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		for (BYTE j=0;j<MAX_INDEX;j++)
		{
			if(j==m_cbKingCardIndex[0]&& m_cbCardIndex[i][j]>0)
			{
				m_cbKingCount[i]+=m_cbCardIndex[i][j];
			}
			if(j==m_cbKingCardIndex[1]&& m_cbCardIndex[i][j]>0)
			{
				m_cbViceKingCount[i]+=m_cbCardIndex[i][j];
			}
		}
	}
	m_cbUpDownKingCardData=cbUpDownKingData;

	GameStart.cbUpDownKingCardData=m_cbUpDownKingCardData;
	GameStart.cbFloor=max(m_BankContinueCount,1);
	CopyMemory(GameStart.lUpDownScore,m_lUpDownScore,sizeof(m_lUpDownScore));

	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//设置变量
		GameStart.wUserAction=m_wUserAction[i];
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData[i],MAX_COUNT);
	}
	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	return true;

}

bool CTableFrameSink::OnUserBatchEnd(WORD wChairID)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//用户效验
	ASSERT(m_bBatchEnd[wChairID]==false);
	if (m_bBatchEnd[wChairID]==true) return false;


	//变量设置
	m_bBatchEnd[wChairID]=true;

	//完成判断
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_bBatchEnd[i]==false) break;
	}
	if (i!=m_wPlayerCount) return true;

	//游戏开始
	OnEventGameStart2();

	return true;

}
//////////////////////////////////////////////////////////////////////////
