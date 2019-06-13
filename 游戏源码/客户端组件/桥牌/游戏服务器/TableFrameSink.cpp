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
	//逻辑变量
	m_cbPackCount=1;
	m_cbMainColor=COLOR_ERROR;
	m_wFirstUser=0;
	m_bLandScore=250;
	m_cbJuKuang=0;
	m_cbMultiples=1;

	//连局信息
	m_cbValueOrder[0]=VALUE_ERROR;
	m_cbValueOrder[1]=VALUE_ERROR;

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//叫牌信息
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;

	//状态变量
	m_cbScoreCardCount=0;
	ZeroMemory(m_bCallCard,sizeof(m_bCallCard));
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	//出牌变量
	m_wTurnWinner=INVALID_CHAIR;
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//贴点扑克
	ZeroMemory(m_cbTieCardData,sizeof(m_cbTieCardData));
	ZeroMemory(m_cbTieCardCount,sizeof(m_cbTieCardCount));

	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));

	//明手控制
	ZeroMemory(bHandOutFlag,sizeof(bHandOutFlag));

	//叫分记录
	memset(m_bColorInfo,1,sizeof(m_bScoreInfo));
	memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));

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
	//QUERYINTERFACE(ITableFrameEvent,Guid,dwQueryVer);
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
	//逻辑变量
	m_cbPackCount=1;
	m_cbMainColor=COLOR_ERROR;

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	//m_wFirstUser=INVALID_CHAIR;

	//叫牌信息
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;
	m_bLandScore=250;
	m_cbJuKuang=0;
	m_cbMultiples=1;

	//状态变量
	m_cbScoreCardCount=0;
	ZeroMemory(m_bCallCard,sizeof(m_bCallCard));
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	//出牌变量
	m_wTurnWinner=INVALID_CHAIR;
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//贴点扑克
	ZeroMemory(m_cbTieCardData,sizeof(m_cbTieCardData));
	ZeroMemory(m_cbTieCardCount,sizeof(m_cbTieCardCount));

	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));

	//明手控制
	ZeroMemory(bHandOutFlag,sizeof(bHandOutFlag));

	//叫分记录
	memset(m_bColorInfo,1,sizeof(m_bScoreInfo));
	memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));

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
	//m_pITableFrame->SetGameStatus(GS_UG_CALL);
	m_pITableFrame->SetGameStatus(GS_UG_SCORE);

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	
	//设置变量
	m_cbMainColor=COLOR_ERROR;

	//明手控制
	ZeroMemory(bHandOutFlag,sizeof(bHandOutFlag));

	//叫分记录
	memset(m_bColorInfo,1,sizeof(m_bScoreInfo));
	memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));

	//设置属性
	m_GameLogic.SetMainColor(m_cbMainColor);
	m_GameLogic.SetPackCount(m_cbPackCount);
	SetJuKuang(m_wFirstUser);
	m_cbMultiples=1;

	//设置历史叫分信息
	switch (m_cbJuKuang)
	{
	case 2:
		{
			m_bScoreInfo[0][0]=240;
			m_bColorInfo[0][0]=240;
			break;
		}
	case 3:
		{
			m_bScoreInfo[0][0]=240;
			m_bColorInfo[0][0]=240;
			m_bScoreInfo[1][0]=240;
			m_bColorInfo[1][0]=240;
			break;
		}
	case 4:
		{
			m_bScoreInfo[0][0]=240;
			m_bColorInfo[0][0]=240;
			m_bScoreInfo[1][0]=240;
			m_bColorInfo[1][0]=240;
			m_bScoreInfo[2][0]=240;
			m_bColorInfo[2][0]=240;
			break;
		}
	}

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.cbPackCount=m_cbPackCount;
	GameStart.cbValueOrder[0]=0;
	GameStart.cbValueOrder[1]=0;
	GameStart.cbJuKuang=m_cbJuKuang;
	
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

	//贴点扑克
	ZeroMemory(m_cbTieCardData,sizeof(m_cbTieCardData));
	ZeroMemory(m_cbTieCardCount,sizeof(m_cbTieCardCount));

	//发送扑克
	DispatchUserCard();

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_DISMISS:		//游戏解散
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			m_cbValueOrder[0]=VALUE_ERROR;
			m_cbValueOrder[1]=VALUE_ERROR;

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			tagScoreInfo ScoreInfo[m_wPlayerCount];
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

			//统计变量
			LONG lBankerScore[2];
			BYTE bImp=0;

			//0为庄分 1为对家分
			lBankerScore[0]=0;
			lBankerScore[1]=0;

			//-------------------------------------------------------------------------------
			//判断是否完成定约？ 未完成定约
			if ((m_bLandScore+6)>m_cbValueOrder[0])
			{
				LONG lFirstDunValue=0;  //第一墩
				LONG lNextDunValue=0;   //1墩后每墩
				LONG lFouthDunValue=0;  //第4墩和以后
				BYTE bHaveDun=((m_bLandScore+6)-m_cbValueOrder[0]);  //计算差墩
				
				//效验变量
				ASSERT(m_cbJuKuang!=0);
				if (m_cbJuKuang==0) return false;
				if (m_cbJuKuang!=1) //双有，南北，东西
				{
					switch (m_cbMultiples)
					{
					case 1:
						{
							lFirstDunValue=100;
							lNextDunValue=100;
							lFouthDunValue=0;
							break;
						}
					case 2:
						{
							lFirstDunValue=200;
							lNextDunValue=300;
							lFouthDunValue=0;
							break;
						}
					case 6:
						{
							lFirstDunValue=400;
							lNextDunValue=600;
							lFouthDunValue=0;
							break;
						}
					}
				}
				else  //双无
				{
					switch (m_cbMultiples)
					{
					case 1:
						{
							lFirstDunValue=50;
							lNextDunValue=50;
							lFouthDunValue=0;
							break;
						}
					case 2:
						{
							lFirstDunValue=100;
							lNextDunValue=200;
							lFouthDunValue=100;
							break;
						}
					case 6:
						{
							lFirstDunValue=200;
							lNextDunValue=400;
							lFouthDunValue=200;
							break;
						}
					}
				}

				LONG lHaveValue=0; //相差墩计算后得分
				if (bHaveDun<4)   //相差墩少于4墩
				{
					lHaveValue=lFirstDunValue+(bHaveDun-1)*lNextDunValue;
				}
				else
				{
					BYTE bLostDun=bHaveDun-4+1;  
					lHaveValue=lFirstDunValue+2*lNextDunValue+bLostDun*lFouthDunValue;
				}

				//统计得分
				lBankerScore[0]=-lHaveValue;
				lBankerScore[1]=lHaveValue;
			}
			//-------------------------------------------------------------------------------
			else  //完成定约
			{
				LONG lFirstCardValue=0;
				LONG lNextCardValue=0;
				bool bIsFinishJu=false;  //成局标志  false=部分 true=成局
				
				//计算花色的分值
				switch (m_cbMainColor)
				{
				case COLOR_MEI_HUA:
				case COLOR_FANG_KUAI:
					{
						switch (m_cbMultiples)
						{
						case 1:
							{
								lFirstCardValue=20;
								lNextCardValue=20;
								break;
							}
						case 2:
							{
								lFirstCardValue=40;
								lNextCardValue=40;
								break;
							}
						case 6:
							{
								lFirstCardValue=80;
								lNextCardValue=80;
								break;
							}
						}
						break;
					}
				case COLOR_HONG_TAO:
				case COLOR_HEI_TAO:
					{
						switch (m_cbMultiples)
						{
						case 1:
							{
								lFirstCardValue=30;
								lNextCardValue=30;
								break;
							}
						case 2:
							{
								lFirstCardValue=40;
								lNextCardValue=40;
								break;
							}
						case 6:
							{
								lFirstCardValue=120;
								lNextCardValue=120;
								break;
							}
						}
						break;
					}
				case COLOR_NT:
					{
						switch (m_cbMultiples)
						{
						case 1:
							{
								lFirstCardValue=40;
								lNextCardValue=30;
								break;
							}
						case 2:
							{
								lFirstCardValue=80;
								lNextCardValue=60;
								break;
							}
						case 6:
							{
								lFirstCardValue=160;
								lNextCardValue=120;
								break;
							}
						}
						break;
					}
				}
				
				//判断是否成局
				LONG bDingYueValue=lFirstCardValue+((m_bLandScore-1)*lNextCardValue);
				if (bDingYueValue>=100) bIsFinishJu=true;

				//计算满贯
				if (m_bLandScore==6||m_bLandScore==7)
				{
					//效验变量
					ASSERT(m_cbJuKuang==0);
					if (m_cbJuKuang==0) return false;
					if (m_cbJuKuang!=1) //双有，南北，东西
					{
						lBankerScore[0]=(m_bLandScore==6)?750:1500;
					}
					else  //双无
					{
						lBankerScore[0]=(m_bLandScore==6)?500:1000;
					}

					//计算得分
					lBankerScore[0]+=bDingYueValue;
					lBankerScore[1]=-lBankerScore[0];
				}
				else
				{
					//计算超墩
					BYTE bChaoDun=m_cbValueOrder[0]-(m_bLandScore+6);
					LONG bChaoDunValue=0;
					if (bChaoDun!=0)
					{
						//效验变量
						//ASSERT(m_cbJuKuang==0);
						if (m_cbJuKuang==0) return false;
						if (m_cbJuKuang!=1) //双有，南北，东西
						{
							switch (m_cbMultiples)
							{
							case 1:
								{
									switch (m_cbMainColor)
									{
										case COLOR_MEI_HUA:
										case COLOR_FANG_KUAI:
										{
											bChaoDunValue=20;	
											break;
										}
										case COLOR_HONG_TAO:
										case COLOR_HEI_TAO:
										{
											bChaoDunValue=30;		
											break;
										}
										case COLOR_NT:
										{
											bChaoDunValue=30;		
											break;
										}
									}
								}
							case 2:
								{
									bChaoDunValue=200;
									break;
								}
							case 6:
								{
									bChaoDunValue=400;
									break;
								}

							}
						}
						else  //双无
						{
							switch (m_cbMultiples)
							{
							case 1:
								{
									switch (m_cbMainColor)
									{
										case COLOR_MEI_HUA:
										case COLOR_FANG_KUAI:
										{
											bChaoDunValue=20;	
											break;
										}
										case COLOR_HONG_TAO:
										case COLOR_HEI_TAO:
										{
											bChaoDunValue=30;		
											break;
										}
										case COLOR_NT:
										{
											bChaoDunValue=30;		
											break;
										}
									}
								}
							case 2:
								{
									bChaoDunValue=100;
									break;
								}
							case 6:
								{
									bChaoDunValue=200;
									break;
								}

							}
						}
					//------------------------------------------------
					//计算得分
					lBankerScore[0]+=(bDingYueValue+bChaoDun*bChaoDunValue);
					}
					
					//定约奖分
					LONG lDingYueScore=0;

					//效验变量
					//ASSERT(m_cbJuKuang==0);
					if (m_cbJuKuang==0) return false;
					
					//有局成局，无局成局，部分定约
					if (bIsFinishJu)
					{
						if (m_cbJuKuang!=1) //双有，南北，东西
						{
							lDingYueScore=500;
						}
						else  //双无
						{
							lDingYueScore=300;
						}
					}
					else
					{
						lDingYueScore=50;
					}
					
					//加倍，再加倍
					if (m_cbMultiples==2) lDingYueScore+=50;
					if (m_cbMultiples==6) lDingYueScore+=100;

					//计算总分
					lBankerScore[0]+=lDingYueScore;
					lBankerScore[1]=-lBankerScore[0];
				}
			}


			//-------------------------------------------------------------------------------
			
			//IMP换算成积分
			LONG lTempImp=0;
			BYTE bTiePoint=0;

			lTempImp=abs(lBankerScore[0]);
			if (lTempImp>=0&&lTempImp<50) bImp=1;
			if (lTempImp>=50&&lTempImp<90) bImp=2;
			if (lTempImp>=90&&lTempImp<130) bImp=3;
			if (lTempImp>=130&&lTempImp<170) bImp=4;
			if (lTempImp>=170&&lTempImp<220) bImp=5;
			if (lTempImp>=220&&lTempImp<270) bImp=6;
			if (lTempImp>=270&&lTempImp<320) bImp=7;
			if (lTempImp>=320&&lTempImp<370) bImp=8;
			if (lTempImp>=370&&lTempImp<430) bImp=9;
			if (lTempImp>=430&&lTempImp<500) bImp=10;
			if (lTempImp>=500&&lTempImp<600) bImp=11;
			if (lTempImp>=600&&lTempImp<750) bImp=12;
			if (lTempImp>=750&&lTempImp<900) bImp=13;
			if (lTempImp>=900&&lTempImp<1100) bImp=14;
			if (lTempImp>=1100&&lTempImp<1300) bImp=15;
			if (lTempImp>=1300&&lTempImp<1500) bImp=16;
			if (lTempImp>=1500&&lTempImp<1750) bImp=17;
			if (lTempImp>=1750&&lTempImp<2000) bImp=18;
			if (lTempImp>=2000&&lTempImp<2250) bImp=19;
			if (lTempImp>=2250&&lTempImp<2500) bImp=20;
			if (lTempImp>=2500&&lTempImp<3000) bImp=21;
			if (lTempImp>=3000&&lTempImp<3500) bImp=22;
			if (lTempImp>=3500&&lTempImp<4000) bImp=23;
			if (lTempImp>=4000) bImp=24;

			//计算得分方的大牌值
			if (lBankerScore[0]>0)
			{
				bTiePoint=GetBigCardPoint(m_wBankerUser,(m_wBankerUser+2)%m_wPlayerCount);
			}
			else
			{
				bTiePoint=GetBigCardPoint((m_wBankerUser+1)%m_wPlayerCount,(m_wBankerUser+3)%m_wPlayerCount);
			}

			//最后得分
			BYTE bTotalScore=bImp+20-bTiePoint;
			LONG bNSScore=0;  //南北得分
			LONG bEWScore=0;  //东西得分
			if (lBankerScore[0]>0)
			{
				//玩家得分
				GameEnd.lScore[m_wBankerUser]=bTotalScore*m_pGameServiceOption->lCellScore;
				GameEnd.lScore[(m_wBankerUser+2)%m_wPlayerCount]=bTotalScore*m_pGameServiceOption->lCellScore;
				GameEnd.lScore[(m_wBankerUser+1)%m_wPlayerCount]=-(bTotalScore*m_pGameServiceOption->lCellScore);
				GameEnd.lScore[(m_wBankerUser+3)%m_wPlayerCount]=-(bTotalScore*m_pGameServiceOption->lCellScore);
				bEWScore=-bTotalScore;
				bNSScore=bTotalScore;
			}
			else
			{
				//玩家得分
				GameEnd.lScore[m_wBankerUser]=-(bTotalScore*m_pGameServiceOption->lCellScore);
				GameEnd.lScore[(m_wBankerUser+2)%m_wPlayerCount]=-(bTotalScore*m_pGameServiceOption->lCellScore);
				GameEnd.lScore[(m_wBankerUser+1)%m_wPlayerCount]=bTotalScore*m_pGameServiceOption->lCellScore;
				GameEnd.lScore[(m_wBankerUser+3)%m_wPlayerCount]=bTotalScore*m_pGameServiceOption->lCellScore;
				bEWScore=bTotalScore;
				bNSScore=-bTotalScore;
			}

			//计算税收


			LONG lGameTax=0L;
			if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (GameEnd.lScore[i]>=100L)
					{
						lGameTax+=GameEnd.lScore[i] * m_pGameServiceOption->wRevenue /100L;
						ScoreInfo[i].lRevenue = GameEnd.lScore[i] * m_pGameServiceOption->wRevenue /100L ;
						GameEnd.lScore[i] -=ScoreInfo[i].lRevenue;
					}
				}
			}

			
			//-------------------------------------------------------------------------------
			char cFangWei[10];
			if (m_wBankerUser==0) strcpy(cFangWei,"北");
			if (m_wBankerUser==1) strcpy(cFangWei,"东");
			if (m_wBankerUser==2) strcpy(cFangWei,"南");
			if (m_wBankerUser==3) strcpy(cFangWei,"西");
			char cColor[10];
			if (m_cbMainColor==COLOR_NT) strcpy(cColor,"NT");
			if (m_cbMainColor==COLOR_HEI_TAO) strcpy(cColor,"黑桃");
			if (m_cbMainColor==COLOR_HONG_TAO) strcpy(cColor,"红桃");
			if (m_cbMainColor==COLOR_FANG_KUAI) strcpy(cColor,"方块");
			if (m_cbMainColor==COLOR_MEI_HUA) strcpy(cColor,"梅花");

			//通知消息
			TCHAR szMessage[256]=TEXT("");
			_snprintf(szMessage,CountArray(szMessage),TEXT("结果：[ %s%d%s ]IMP：[ %d ] 南北得分：[ %d ] 东西得分：[ %d ] 游戏税率：[ %d ]"),cFangWei,m_bLandScore,cColor,lTempImp,bNSScore,bEWScore,lGameTax);
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				IServerUserItem * pISendUserItem=m_pITableFrame->GetServerUserItem(i);
				if (pISendUserItem!=NULL) m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_INFO);
			}
			WORD wIndex=0;
			do
			{
				IServerUserItem * pISendUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
				if (pISendUserItem==NULL) break;
				m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_INFO);
			} while (true);

			//-------------------------------------------------------------------------------

			//下次发牌
			if (m_wFirstUser!=INVALID_CHAIR) m_wFirstUser=(m_wFirstUser+1)%GAME_PLAYER;
			
			//游戏倍数
			GameEnd.wConcealTime=m_cbMultiples;

			//双方得分
			GameEnd.bBankerScore=m_cbValueOrder[0];
			GameEnd.bOtherScore=m_cbValueOrder[1];

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				ScoreInfo[i].lScore=GameEnd.lScore[i];
				ScoreInfo[i].ScoreKind=(GameEnd.lScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
				m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore ,ScoreInfo[i].lRevenue ,ScoreInfo[i].ScoreKind);
			}
			//m_pITableFrame->WriteTableScore(ScoreInfo,m_wPlayerCount,lGameTax);

			m_cbValueOrder[0]=VALUE_ERROR;
			m_cbValueOrder[1]=VALUE_ERROR;

			//庄家切换
			if (GameEnd.lScore[m_wBankerUser]>0L) m_wBankerUser=(m_wBankerUser+2)%m_wPlayerCount;
			else m_wBankerUser=(m_wBankerUser+1)%m_wPlayerCount;

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
			//玩家得分
			GameEnd.lScore[wChairID]=-8*m_pGameServiceOption->lCellScore;
			GameEnd.bBankerScore=0;
			GameEnd.bOtherScore=0;
			GameEnd.wConcealTime=0;

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			
			ScoreInfo.ScoreKind=enScoreKind_Flee;
			ScoreInfo.lScore=GameEnd.lScore[wChairID];
			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo.lScore,ScoreInfo.lRevenue,ScoreInfo.ScoreKind);

			//通知消息
			TCHAR szMessage[128]=TEXT("");
			_snprintf(szMessage,CountArray(szMessage),TEXT("由于 [ %s ] 离开游戏，游戏结束"),pIServerUserItem->GetAccounts());
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				IServerUserItem * pISendUserItem=m_pITableFrame->GetServerUserItem(i);
				if (pISendUserItem!=NULL) m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_INFO);
			}
			WORD wIndex=0;
			do
			{
				IServerUserItem * pISendUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
				if (pISendUserItem==NULL) break;
				m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_INFO);
			} while (true);

			m_cbValueOrder[0]=VALUE_ERROR;
			m_cbValueOrder[1]=VALUE_ERROR;
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
	case GS_UG_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lBaseScore=m_pGameServiceOption->lCellScore;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_UG_SCORE:		//叫分状态
		{
			//构造数据
			CMD_S_StatusScore StatusScore;
			ZeroMemory(&StatusScore,sizeof(StatusScore));
			
        	//设置变量
			StatusScore.bBossScore=m_bLandScore;
			StatusScore.wCurrentUser=m_wCurrentUser;
			
			//StatusScore.lBaseScore=m_pGameServiceOption->lCellScore;
	
			//CopyMemory(StatusScore.bScoreInfo,m_bScoreInfo,sizeof(m_bScoreInfo));
			//CopyMemory(StatusScore.bCardData,m_bHandCardData[wChiarID],m_bCardCount[wChiarID]);

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	
	
	case GS_UG_PLAY:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.cbPackCount=m_cbPackCount;
			StatusPlay.cbMainColor=m_cbMainColor;
			StatusPlay.cbValueOrder[0]=m_cbValueOrder[0];
			StatusPlay.cbValueOrder[1]=m_cbValueOrder[1];

			//叫牌变量
			StatusPlay.cbCallCard=m_cbCallCard;
			StatusPlay.cbCallCount=m_cbCallCount;
			StatusPlay.wCallCardUser=m_wCallCardUser;

			//用户变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.wFirstOutUser=m_wFirstOutUser;

			//扑克变量
			StatusPlay.cbCardCount=m_cbHandCardCount[wChiarID];
			CopyMemory(StatusPlay.cbCardData,m_cbHandCardData[wChiarID],sizeof(BYTE)*m_cbHandCardCount[wChiarID]);

			//底牌信息
			if (wChiarID==m_wBankerUser)
			{
				StatusPlay.cbConcealCount=m_cbConcealCount;
				CopyMemory(StatusPlay.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);
			}

			//出牌变量
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				StatusPlay.cbOutCardCount[i]=m_cbOutCardCount[i];
				CopyMemory(StatusPlay.cbOutCardData[i],m_cbOutCardData[i],sizeof(BYTE)*m_cbOutCardCount[i]);
			}

			//得分变量
			StatusPlay.cbScoreCardCount=m_cbScoreCardCount;
			CopyMemory(StatusPlay.cbScoreCardData,m_cbScoreCardData,sizeof(StatusPlay.cbScoreCardData));

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
	
	case SUB_C_CALL_SCORE:		//用户叫分
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CallScore));
			if (wDataSize!=sizeof(CMD_C_CallScore)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			
			//消息处理
			CMD_C_CallScore * pLandScore=(CMD_C_CallScore *)pDataBuffer;
			return OnUserLandScore(pUserData->wChairID,pLandScore->bBossScore,pLandScore->bBossColor);
			
		}
	case SUB_C_MULTIPLES:      //用户加倍
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_OnMultiples));
			if (wDataSize!=sizeof(CMD_C_OnMultiples)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			
			//消息处理
			CMD_C_OnMultiples * pMultiples=(CMD_C_OnMultiples *)pDataBuffer;
			return OnUserMultiples(pUserData->wChairID,pMultiples->bMultiples);
			
		}

	case SUB_C_OUT_CARD:		//用户出牌
		{
			/*//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//效验数据
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0]))) return false;
			*/

			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
			if (wDataSize!=sizeof(CMD_C_OutCard)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;
			
			//消息处理
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			return OnUserOutCard(pUserData->wChairID,pOutCard->cbCardData,pOutCard->cbCardCount,pOutCard->bIsHandOut,pOutCard->bIsAutoOut);
		}
	}

	return false;
}


//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户起来
bool __cdecl CTableFrameSink::OnEventUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//设置变量
	if (bLookonUser==false)
	{
		//逻辑变量
		m_cbPackCount=1;
		m_cbMainColor=COLOR_ERROR;
		m_wFirstUser=INVALID_CHAIR;
		m_bLandScore=250;
		m_cbJuKuang=0;
		m_cbMultiples=1;

		//连局信息
		m_cbValueOrder[0]=VALUE_ERROR;
		m_cbValueOrder[1]=VALUE_ERROR;

		//游戏变量
		m_wBankerUser=INVALID_CHAIR;
		m_wCurrentUser=INVALID_CHAIR;

		//叫牌信息
		m_cbCallCard=0;
		m_cbCallCount=0;
		m_wCallCardUser=INVALID_CHAIR;

		//状态变量
		m_cbScoreCardCount=0;
		ZeroMemory(m_bCallCard,sizeof(m_bCallCard));
		ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

		//出牌变量
		m_wTurnWinner=INVALID_CHAIR;
		m_wFirstOutUser=INVALID_CHAIR;
		ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
		ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

		//底牌扑克
		m_cbConcealCount=0;
		ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

		//用户扑克
		ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
		ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

		//贴点扑克
		ZeroMemory(m_cbTieCardData,sizeof(m_cbTieCardData));
		ZeroMemory(m_cbTieCardCount,sizeof(m_cbTieCardCount));

		memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));

		//明手控制
		ZeroMemory(bHandOutFlag,sizeof(bHandOutFlag));

		//叫分记录
		memset(m_bColorInfo,1,sizeof(m_bScoreInfo));
		memset(m_bScoreInfo,0,sizeof(m_bScoreInfo));

	}

	return true;
}

//用户加倍
bool CTableFrameSink::OnUserMultiples(WORD wChairID,BYTE bMultiples)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_SCORE);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_SCORE) return true;
	if (wChairID!=m_wCurrentUser) return false;
	if (bMultiples<1||bMultiples>6) return false;

	//设置变量
	switch (m_cbMultiples)
	{
	case 1:
		{
			m_cbMultiples=2;
			break;
		}
	case 2:
		{
			m_cbMultiples=6;
			break;
		}
	}

	//叫分记录
	for (byte i=0;i<50;i++)
	{
		if (m_bScoreInfo[wChairID][i]==0)
		{
			m_bScoreInfo[wChairID][i]=(m_cbMultiples==2)?200:210;
			m_bColorInfo[wChairID][i]=(m_cbMultiples==2)?200:210;
			break;
		}
	}

	//设置用户
	ZeroMemory(m_bPlayStatus,sizeof(m_bPlayStatus));

	//顺时针
	m_wCurrentUser=(wChairID+1)%m_wPlayerCount;

	//发送消息
	CMD_S_CallScore LandScore;
	LandScore.bCallScoreUser=wChairID;
	LandScore.wCurrentUser=m_wCurrentUser;
	LandScore.bCurrentScore=m_bLandScore;
	LandScore.bCurrentColor=m_cbMainColor;
	LandScore.bMultiples=m_cbMultiples;

	//判断m_wCurrentUser是否能加倍或再加倍
	bool bDbl=false;
	bool bRedbl=false;
	//m_wCurrentUser能否再加倍
	if (m_cbMultiples!=6)  //如果已经是再加倍，那下家只能PASS或升定
	{
		byte bNum1=GetScoreNum(wChairID);
		byte bNum2=GetScoreNum((wChairID+2)%m_wPlayerCount);
		//如果wChairID没再加倍，m_wCurrentUser才能再加倍
		if (m_bScoreInfo[wChairID][bNum1-1]!=210&&m_bScoreInfo[wChairID][bNum1-1]!=240)
		{
			if (m_bScoreInfo[wChairID][bNum1-1]==200) 
			{
				bRedbl=true;
			}

			if (bNum2!=50)
			{
				if (m_bScoreInfo[(wChairID+2)%m_wPlayerCount][bNum2-1]==200) 
				{
					bRedbl=true;
				}
			}
		}
	}

	if (m_cbMultiples==2)
	{
		//加倍
		LandScore.bLastColor=200;
		LandScore.bLastScore=200;
	}
	else
	{
		//再加倍
		LandScore.bLastColor=210;
		LandScore.bLastScore=210;
	}
	
	LandScore.bDbl=bDbl;
	LandScore.bReDbl=bRedbl;

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CALL_SCORE,&LandScore,sizeof(LandScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_SCORE,&LandScore,sizeof(LandScore));
	return true;
}



//用户叫分
bool CTableFrameSink::OnUserLandScore(WORD wChairID, BYTE bBossScore,BYTE bBossColor)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_SCORE);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_SCORE) return true;
	if (wChairID!=m_wCurrentUser) return false;
	//效验参数
	if ((bBossScore>7)&&(bBossScore!=250)) return false;

	//设置变量 定约升级，游戏倍数重置
	if (bBossScore!=250)
	{
		m_bLandScore=bBossScore;
		m_wBankerUser=m_wCurrentUser;
		m_cbMainColor=bBossColor;
		m_cbMultiples=1;
	}

	//叫分记录
	for (byte i=0;i<50;i++)
	{
		if (m_bScoreInfo[wChairID][i]==0) 
		{
			m_bScoreInfo[wChairID][i]=bBossScore;
			m_bColorInfo[wChairID][i]=bBossColor;
			break;
		}
	}

	//开始判断
	//if ((m_bLandScore==5)||(m_wFirstUser==(wChairID+1)%m_wPlayerCount))
	if (bBossScore==250) 
	{
		m_bPlayStatus[wChairID]=1;
	}
	else
	{
		ZeroMemory(m_bPlayStatus,sizeof(m_bPlayStatus));
	}

	WORD PassCount=0;	  //判断放弃人数
	WORD PassCountUser=INVALID_CHAIR;  
	WORD wNextUser=INVALID_CHAIR;	  

	for(WORD i=0;i<m_wPlayerCount;i++)													  
	{
		if (m_bPlayStatus[i]==1) 
		{
			PassCount++;
		}
		else
		{
			PassCountUser=i;
		}
	}    
	
	if ((PassCount==3&&m_wBankerUser!=INVALID_CHAIR)||(m_bLandScore==7&&m_cbMainColor==COLOR_NT))
	{
		if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=0;
		//设置状态
		m_pITableFrame->SetGameStatus(GS_UG_PLAY);

		if (PassCount==3) m_wCurrentUser=PassCountUser;
		
		//判断庄家
		//------------------------------------------------------------------------------------
		//排除特殊情况(当首叫定约而其他人都PASS时，庄还是首叫者)
		if (GetScoreNum(0)!=1&&GetScoreNum(1)!=1&&GetScoreNum(2)!=1&&GetScoreNum(3)!=1) 
		{
			BYTE bNum1=0;
			BYTE bNum2=0;
			for (byte j=0;j<GetScoreNum(m_wCurrentUser);j++)
			{
				if (m_bColorInfo[m_wCurrentUser][j]!=m_cbMainColor)
				{
					bNum1++;
				}
				else
				{
					break;
				}
			}
			for (byte j=0;j<GetScoreNum((m_wCurrentUser+2)%m_wPlayerCount);j++)
			{
				if (m_bColorInfo[(m_wCurrentUser+2)%m_wPlayerCount][j]!=m_cbMainColor)
				{
					bNum2++;
				}
				else
				{
					break;
				}
			}
			if (bNum2<=bNum1) 
			{
				//设置变量
				m_wCurrentUser=(m_wCurrentUser+2)%m_wPlayerCount;
				m_wBankerUser=m_wCurrentUser;
			}
		}
		//------------------------------------------------------------------------------------

		//构造数据
		CMD_S_GamePlay GamePlay;
		GamePlay.wCurrentUser=m_wCurrentUser;
		GamePlay.m_cbLandColor=m_cbMainColor;
		GamePlay.m_cbLandScore=m_bLandScore;
		GamePlay.bMultiples=m_cbMultiples;
		
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
				GamePlay.bCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GamePlay.bCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
		}

		m_GameLogic.SetMainColor(m_cbMainColor);
		m_GameLogic.SetPackCount(1);

		//首出牌者
		m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
		m_wFirstOutUser=m_wCurrentUser;
		
		//发送数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//发送消息
			m_pITableFrame->SendTableData(i,SUB_S_GAME_PLAY,&GamePlay,sizeof(GamePlay));
			m_pITableFrame->SendLookonData(i,SUB_S_GAME_PLAY,&GamePlay,sizeof(GamePlay));
		}

		return true;
	}

	if (PassCount==4&&m_wBankerUser==INVALID_CHAIR)
	{
		//重新发牌
		memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
		DispatchUserCard();
		return true;
	}

	//设置用户
	//顺时针
	m_wCurrentUser=(wChairID+1)%m_wPlayerCount;

	//判断m_wCurrentUser是否能加倍或再加倍
	bool bDbl=false;
	bool bRedbl=false;
	
	switch (m_cbMultiples)
	{
		//m_wCurrentUser是否能加倍
	case 1:
		{
			byte bNum=GetScoreNum(wChairID);
			byte bNum1=GetScoreNum((wChairID+2)%m_wPlayerCount);
			if (bNum==50) return false;

			//如果还没叫定约
			if (m_bLandScore==250) break;

			//如果m_wCurrentUser的友家叫的定约是目前定约
			if (m_bScoreInfo[(m_wCurrentUser+2)%m_wPlayerCount][bNum-1]==m_bLandScore&&m_bColorInfo[(m_wCurrentUser+2)%m_wPlayerCount][bNum-1]==m_cbMainColor) break;

			if (bNum1!=50)
			{
				if ((m_bScoreInfo[wChairID][bNum-1]!=250||m_bScoreInfo[(wChairID+2)%m_wPlayerCount][bNum1-1]!=250)&&(m_bScoreInfo[wChairID][bNum-1]!=210&&m_bScoreInfo[(wChairID+2)%m_wPlayerCount][bNum1-1]!=210)&&(m_bScoreInfo[wChairID][bNum-1]!=240&&m_bScoreInfo[(wChairID+2)%m_wPlayerCount][bNum1-1]!=240)) 
				{
					bDbl=true;
					break;
				}
			}
			else
			{
				if (m_bScoreInfo[wChairID][bNum-1]!=250&&m_bScoreInfo[wChairID][bNum-1]!=210&&m_bScoreInfo[wChairID][bNum-1]!=240&&m_bScoreInfo[wChairID][bNum-1]!=240)
				{
					bDbl=true;
					break;
				}
			}
			
			if (m_bScoreInfo[wChairID][bNum-1]!=250&&m_bScoreInfo[wChairID][bNum-1]!=240)
			{
				bDbl=true;
			}
			else
			{
				bNum=GetScoreNum((wChairID+2)%m_wPlayerCount);
				if (bNum==50) break;
				if (m_bScoreInfo[(wChairID+2)%m_wPlayerCount][bNum-1]!=250&&m_bScoreInfo[(wChairID+2)%m_wPlayerCount][bNum-1]!=240) bDbl=true;
			}
			
			break;
		}
		//m_wCurrentUser是否能再加倍
	case 2:
		{
			byte bNum1=GetScoreNum(wChairID);
			byte bNum2=GetScoreNum((wChairID+2)%m_wPlayerCount);
			//不能针对自己同伴再加倍
			if (m_bScoreInfo[wChairID][bNum1-1]!=210&&m_bScoreInfo[wChairID][bNum1-1]!=240)
			{
				if (m_bScoreInfo[wChairID][bNum1-1]==200) 
				{
					bRedbl=true;
					break;
				}
				
				if (bNum2!=50)
				{
					if (m_bScoreInfo[(wChairID+2)%m_wPlayerCount][bNum2-1]==200) 
					{
						bRedbl=true;
						break;
					}
				}
			}
			break;
		}
	}
	
	//设置用户
	//逆时针
	/*
	for(WORD i=m_wPlayerCount-1;i>=0;i--)													  
	{
		wNextUser=(m_wCurrentUser+i-1)%m_wPlayerCount;	
		if (m_bPlayStatus[wNextUser]==0) break;         
	}
	m_wCurrentUser=wNextUser;
	*/

	//发送消息
	CMD_S_CallScore LandScore;
	LandScore.bCallScoreUser=wChairID;
	LandScore.wCurrentUser=m_wCurrentUser;
	LandScore.bCurrentScore=m_bLandScore;
	LandScore.bCurrentColor=m_cbMainColor;
	LandScore.bMultiples=m_cbMultiples;
	LandScore.bLastColor=bBossColor;
	LandScore.bLastScore=bBossScore;
	LandScore.bDbl=bDbl;
	LandScore.bReDbl=bRedbl;

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CALL_SCORE,&LandScore,sizeof(LandScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_SCORE,&LandScore,sizeof(LandScore));
	

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount, bool bIsHandOut[],bool bIsAutoOut)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_PLAY) return true;

	//效验用户
	if(bIsHandOut[wChairID]==false)
	{
		ASSERT((wChairID==m_wCurrentUser)&&(cbCardCount<=MAX_COUNT));
		if ((wChairID!=m_wCurrentUser)||(cbCardCount>MAX_COUNT)) return false;
	}
	else
	{
		ASSERT((cbCardCount<=MAX_COUNT));
		if (cbCardCount>MAX_COUNT) return false;
		if (!bIsAutoOut) wChairID=(m_wBankerUser+2)%m_wPlayerCount;
	}
	//首出牌者
	if (m_wFirstOutUser==wChairID)
	{
		//类型判断
		BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);
		if (cbCardType==CT_ERROR||cbCardType!=CT_SINGLE) return false;
	}
	else
	{
		//数目判断
		ASSERT(cbCardCount==m_cbOutCardCount[m_wFirstOutUser]);
		if (cbCardCount!=m_cbOutCardCount[m_wFirstOutUser]) return false;

		//出牌效验
		if (m_GameLogic.EfficacyOutCard(cbCardData,cbCardCount,m_cbOutCardData[m_wFirstOutUser],
			m_cbOutCardCount[m_wFirstOutUser],m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//删除扑克
	if (m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false) return false;
	m_cbHandCardCount[wChairID]-=cbCardCount;
	
	//出牌记录
	m_cbOutCardCount[wChairID]=cbCardCount;
	CopyMemory(m_cbOutCardData[wChairID],cbCardData,cbCardCount);

	//切换用户
	m_wCurrentUser=(wChairID+1)%m_wPlayerCount;
	if (m_wCurrentUser==m_wFirstOutUser) m_wCurrentUser=INVALID_CHAIR;

	

	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//轮到明手出牌
		if(m_wCurrentUser==(m_wBankerUser+2)%GAME_PLAYER)
		{
			bHandOutFlag[m_wBankerUser]=true;
		}
	}

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbCardCount=cbCardCount;
	OutCard.wCurrentUser=m_wCurrentUser;
	OutCard.bHandOutCardCount=m_cbHandCardCount[(m_wBankerUser+2)%m_wPlayerCount];
	//OutCard.bHandOutCardData=m_cbHandCardData[(m_wBankerUser+2)%m_wPlayerCount];
	CopyMemory(OutCard.bHandOutCardData,m_cbHandCardData[(m_wBankerUser+2)%m_wPlayerCount],sizeof(m_cbHandCardData[(m_wBankerUser+2)%m_wPlayerCount]));
	CopyMemory(OutCard.cbCardData,cbCardData,cbCardCount*sizeof(BYTE));
	CopyMemory(OutCard.bHandOutFlag,bHandOutFlag,sizeof(bHandOutFlag));

	//发送数据
	//WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.cbCardData)+OutCard.cbCardCount*sizeof(BYTE);
	//m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	//一轮结算
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//判断胜者
		m_wTurnWinner=m_GameLogic.CompareCardArray(m_cbOutCardData,m_cbOutCardCount[0],m_wFirstOutUser);

		//计算得分
		if ((m_wTurnWinner!=m_wBankerUser)&&((m_wTurnWinner+2)%m_wPlayerCount!=m_wBankerUser)) //庄家和对家
		{
			m_cbValueOrder[1]+=1;
		}
		else
		{
			m_cbValueOrder[0]+=1;
		}

		//设置变量
		if (m_cbHandCardCount[m_wTurnWinner]!=0)
		{
			m_wFirstOutUser=m_wTurnWinner;
			ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
			ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));
		}

		ZeroMemory(bHandOutFlag,sizeof(bHandOutFlag));
		//用户切换
		m_wCurrentUser=(m_cbHandCardCount[m_wTurnWinner]==0)?INVALID_CHAIR:m_wTurnWinner;

		//设置明手
		if (m_wCurrentUser!=INVALID_CHAIR)
		{
			//轮到明手出牌
			if(m_wCurrentUser==(m_wBankerUser+2)%GAME_PLAYER)
			{
				bHandOutFlag[m_wBankerUser]=true;
			}
		}

		//构造数据
		CMD_TurnBalance TurnBalance;
		TurnBalance.wTurnWinner=m_wTurnWinner;
		TurnBalance.wCurrentUser=m_wCurrentUser;
		TurnBalance.bBankerScore=m_cbValueOrder[0];
		TurnBalance.bOtherScore=m_cbValueOrder[1];
		CopyMemory(TurnBalance.bHandOutFlag,bHandOutFlag,sizeof(bHandOutFlag));


		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TURN_BALANCE,&TurnBalance,sizeof(TurnBalance));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TURN_BALANCE,&TurnBalance,sizeof(TurnBalance));
	}

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameEnd(m_wTurnWinner,NULL,GER_NORMAL);

	return true;
}

//派发扑克
bool CTableFrameSink::DispatchUserCard()
{
	//混乱扑克
	BYTE cbRandCard[CELL_PACK*2];
	m_GameLogic.RandCardList(cbRandCard,CELL_PACK*m_cbPackCount);

	//用户扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_cbHandCardCount[i]=m_GameLogic.GetDispatchCount();
		CopyMemory(&m_cbHandCardData[i],&cbRandCard[i*m_cbHandCardCount[i]],sizeof(BYTE)*m_cbHandCardCount[i]);
		
		//贴点扑克
		m_cbTieCardCount[i]=m_GameLogic.GetDispatchCount();
		CopyMemory(&m_cbTieCardData[i],&cbRandCard[i*m_cbTieCardCount[i]],sizeof(BYTE)*m_cbTieCardCount[i]);
	}
	
	//发送开始
	if(m_wFirstUser==INVALID_CHAIR) m_wFirstUser=0;
	m_wCurrentUser=m_wFirstUser;
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=m_wCurrentUser;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//效验状态
		ASSERT(m_cbHandCardCount[i]<=CountArray(SendCard.cbCardData));
		if (m_cbHandCardCount[i]>CountArray(SendCard.cbCardData)) return false;

		//拷贝扑克
		SendCard.cbCardCount=m_cbHandCardCount[i];
		CopyMemory(SendCard.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);

		//发送统计
		WORD wHeadSize=sizeof(SendCard)-sizeof(SendCard.cbCardData);
		WORD wSendSize=wHeadSize+sizeof(SendCard.cbCardData[0])*SendCard.cbCardCount;

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendCard,wSendSize);
		m_pITableFrame->SendLookonData(i,SUB_S_SEND_CARD,&SendCard,wSendSize);
	}

	return true;
}

//局况设置
bool CTableFrameSink::SetJuKuang(WORD wFirstUser)
{
	if(wFirstUser==INVALID_CHAIR) wFirstUser=0;
	m_cbJuKuang=(wFirstUser%GAME_PLAYER)+1;
	if (m_cbJuKuang<=0||m_cbJuKuang>4) m_cbJuKuang=1;
	/*随机局况
	m_cbJuKuang=1+(int)(4.0*rand()/(RAND_MAX+1.0)); 
	if(m_cbJuKuang<=0||m_cbJuKuang>=5) SetJuKuang();
	*/
	return true;
}

//大牌点数
BYTE CTableFrameSink::GetBigCardPoint(WORD wFirstUser,WORD wNextUser)
{
	BYTE bBigCardPoint=0;
	for (byte i=0;i<m_cbTieCardCount[wFirstUser];i++)
	{
		//1=A 13=K 12=Q 11=J
		if (m_GameLogic.GetCardValue(m_cbTieCardData[wFirstUser][i])==1) bBigCardPoint+=4;
		if (m_GameLogic.GetCardValue(m_cbTieCardData[wFirstUser][i])==13) bBigCardPoint+=3;
		if (m_GameLogic.GetCardValue(m_cbTieCardData[wFirstUser][i])==12) bBigCardPoint+=2;
		if (m_GameLogic.GetCardValue(m_cbTieCardData[wFirstUser][i])==11) bBigCardPoint+=1;

		if (m_GameLogic.GetCardValue(m_cbTieCardData[wNextUser][i])==1) bBigCardPoint+=4;
		if (m_GameLogic.GetCardValue(m_cbTieCardData[wNextUser][i])==13) bBigCardPoint+=3;
		if (m_GameLogic.GetCardValue(m_cbTieCardData[wNextUser][i])==12) bBigCardPoint+=2;
		if (m_GameLogic.GetCardValue(m_cbTieCardData[wNextUser][i])==11) bBigCardPoint+=1;
	}
	return bBigCardPoint;
}

//得到所叫次数
BYTE CTableFrameSink::GetScoreNum(WORD wChariID)
{
	byte bNum=0;
	for (byte i=0;i<50;i++)
	{
		if (m_bScoreInfo[wChariID][i]!=0) bNum++;
	}
	return bNum;
}
//////////////////////////////////////////////////////////////////////////
