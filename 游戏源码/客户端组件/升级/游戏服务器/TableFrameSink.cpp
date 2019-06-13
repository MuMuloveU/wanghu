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
	m_cbPackCount=PACK_COUNT;
	m_cbMainValue=0x02;
	m_cbMainColor=COLOR_ERROR;

	//请求状态
	ZeroMemory(m_cbResponses,sizeof(m_cbResponses));
	ZeroMemory(m_wRequestTimes,sizeof(m_wRequestTimes));

	//连局信息
	m_cbValueOrder[0]=2;
	m_cbValueOrder[1]=2;

	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_bUserStrustee,sizeof(m_bUserStrustee));

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
	ZeroMemory(m_cbLastTurnCard,sizeof(m_cbLastTurnCard));
	ZeroMemory(m_cbLastTurnCount,sizeof(m_cbLastTurnCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

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
	m_cbPackCount=2;
	m_cbMainColor=COLOR_ERROR;

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;

	//请求状态
	ZeroMemory(m_cbResponses,sizeof(m_cbResponses));
	ZeroMemory(m_wRequestTimes,sizeof(m_wRequestTimes));

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
	ZeroMemory(m_cbLastTurnCard,sizeof(m_cbLastTurnCard));
	ZeroMemory(m_cbLastTurnCount,sizeof(m_cbLastTurnCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

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
	m_pITableFrame->SetGameStatus(GS_UG_CALL);

	//设置变量
	m_cbMainColor=COLOR_ERROR;

	//设置属性
	m_GameLogic.SetMainColor(m_cbMainColor);
	m_GameLogic.SetMainValue(m_cbMainValue);
	m_GameLogic.SetPackCount(m_cbPackCount);

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.cbPackCount=m_cbPackCount;
	GameStart.cbMainValue=m_cbMainValue;
	GameStart.cbValueOrder[0]=m_cbValueOrder[0];
	GameStart.cbValueOrder[1]=m_cbValueOrder[1];

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

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

			//底牌信息
			GameEnd.cbConcealCount=m_cbConcealCount;
			CopyMemory(GameEnd.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);

			//游戏得分
			GameEnd.wConcealScore=m_GameLogic.GetCardScore(m_cbConcealCard,m_cbConcealCount);
			GameEnd.wGameScore=m_GameLogic.GetCardScore(m_cbScoreCardData,m_cbScoreCardCount);

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
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//底牌信息
			GameEnd.cbConcealCount=m_cbConcealCount;
			CopyMemory(GameEnd.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);

			//游戏得分
			GameEnd.wConcealScore=m_GameLogic.GetCardScore(m_cbConcealCard,m_cbConcealCount);
			GameEnd.wGameScore=m_GameLogic.GetCardScore(m_cbScoreCardData,m_cbScoreCardCount);

			//扣底倍数
			if ((wChairID!=m_wBankerUser)&&(((wChairID+2)%m_wPlayerCount)!=m_wBankerUser))
			{
				//变量定义
				BYTE cbMaxCount=1;
				GameEnd.wConcealTime=1;

				//分析牌型
				tagAnalyseResult AnalyseResult;
				m_GameLogic.AnalyseCardData(m_cbOutCardData[wChairID],m_cbOutCardCount[wChairID],AnalyseResult);

				//同牌分析
				for (BYTE i=0;i<CountArray(AnalyseResult.SameDataInfo);i++)
				{
					if (AnalyseResult.SameDataInfo[i].cbBlockCount>0) cbMaxCount=__max(i,cbMaxCount);
					
				}

				//拖牌分析
				for (BYTE i=0;i<CountArray(AnalyseResult.TractorDataInfo);i++)
				{
					//获取属性
					BYTE cbSameCount=i+2;
					BYTE cbMaxLength=AnalyseResult.TractorDataInfo[i].cbTractorMaxLength;

					//设置变量
					cbMaxCount=__max(cbSameCount*cbMaxLength,cbMaxCount);
				}

				//设置倍数
				cbMaxCount=__min(cbMaxCount,6);
				for (BYTE i=0;i<cbMaxCount;i++) GameEnd.wConcealTime*=2;
			}

			//统计变量
			LONG lBankerScore=0;
			WORD wWholeScore=GameEnd.wGameScore+GameEnd.wConcealTime*GameEnd.wConcealScore;

			//积分统计
			if (wWholeScore==0) 
			{
				lBankerScore=3L;
				m_cbValueOrder[m_wBankerUser%2]=(m_cbValueOrder[m_wBankerUser%2]+2)%13+1;
			}
			else if (wWholeScore<20*m_cbPackCount) 
			{
				lBankerScore=2L;
				m_cbValueOrder[m_wBankerUser%2]=(m_cbValueOrder[m_wBankerUser%2]+1)%13+1;
			}
			else if (wWholeScore<40*m_cbPackCount)
			{
				lBankerScore=1L;
				m_cbValueOrder[m_wBankerUser%2]=m_cbValueOrder[m_wBankerUser%2]%13+1;
			}
			else
			{
				lBankerScore=-(wWholeScore-40*m_cbPackCount)/(20*m_cbPackCount)-1;
				m_cbValueOrder[(m_wBankerUser+1)%2]=(BYTE)((m_cbValueOrder[(m_wBankerUser+1)%2]-lBankerScore-1)%13+1);
			}

			//玩家得分
			GameEnd.lScore[m_wBankerUser]=lBankerScore*m_pGameServiceOption->lCellScore;
			GameEnd.lScore[(m_wBankerUser+2)%m_wPlayerCount]=lBankerScore*m_pGameServiceOption->lCellScore;
			GameEnd.lScore[(m_wBankerUser+1)%m_wPlayerCount]=-(lBankerScore*m_pGameServiceOption->lCellScore);
			GameEnd.lScore[(m_wBankerUser+3)%m_wPlayerCount]=-(lBankerScore*m_pGameServiceOption->lCellScore);

			//计算税收
			LONG lGameTax[GAME_PLAYER];
			ZeroMemory(lGameTax,sizeof(lGameTax));
			if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			{
				LONG lRevence = m_pGameServiceOption->wRevenue;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (GameEnd.lScore[i]>=10L)
					{
						lGameTax[i]=GameEnd.lScore[i]*lRevence/100L;
						GameEnd.lScore[i]-=lGameTax[i];
					}
				}
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				enScoreKind ScoreKind;
				if( GameEnd.lScore[i] == 0 ) ScoreKind = enScoreKind_Draw;
				else if( GameEnd.lScore[i] > 0 ) ScoreKind = enScoreKind_Win;
				else ScoreKind = enScoreKind_Lost;
				m_pITableFrame->WriteUserScore(i,GameEnd.lScore[i],lGameTax[i],ScoreKind);
			}

			//庄家切换
			if (GameEnd.lScore[m_wBankerUser]>0L) m_wBankerUser=(m_wBankerUser+2)%m_wPlayerCount;
			else m_wBankerUser=(m_wBankerUser+1)%m_wPlayerCount;

			//主牌设置
			m_cbMainValue=m_cbValueOrder[m_wBankerUser%2];

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//底牌信息
			GameEnd.cbConcealCount=m_cbConcealCount;
			CopyMemory(GameEnd.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);

			//游戏得分
			GameEnd.wConcealScore=m_GameLogic.GetCardScore(m_cbConcealCard,m_cbConcealCount);
			GameEnd.wGameScore=m_GameLogic.GetCardScore(m_cbScoreCardData,m_cbScoreCardCount);

			//玩家得分
			GameEnd.lScore[wChairID]=-20*m_pGameServiceOption->lCellScore;

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			m_pITableFrame->WriteUserScore(wChairID,GameEnd.lScore[wChairID],0,enScoreKind_Flee);

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
			//托管变量
			CopyMemory(StatusFree.bStrustee,m_bUserStrustee,sizeof(m_bUserStrustee));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_UG_CALL:		//叫牌状态
		{
			//构造数据
			CMD_S_StatusCall StatusCall;
			ZeroMemory(&StatusCall,sizeof(StatusCall));

			//游戏变量
			StatusCall.wBankerUser=m_wBankerUser;
			StatusCall.cbPackCount=m_cbPackCount;
			StatusCall.cbMainValue=m_cbMainValue;
			StatusCall.cbValueOrder[0]=m_cbValueOrder[0];
			StatusCall.cbValueOrder[1]=m_cbValueOrder[1];

			//发牌变量
			StatusCall.cbCardCount=m_cbHandCardCount[wChiarID];
			CopyMemory(StatusCall.cbCardData,m_cbHandCardData[wChiarID],sizeof(BYTE)*m_cbHandCardCount[wChiarID]);

			//叫牌信息
			StatusCall.cbCallCard=m_cbCallCard;
			StatusCall.cbCallCount=m_cbCallCount;
			StatusCall.wCallCardUser=m_wCallCardUser;
			StatusCall.cbComplete=(m_bCallCard[wChiarID]==true)?TRUE:FALSE;

			//托管变量
			CopyMemory(StatusCall.bStrustee,m_bUserStrustee,sizeof(m_bUserStrustee));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusCall,sizeof(StatusCall));
		}
	case GS_UG_BACK:		//底牌状态
		{
			//构造数据
			CMD_S_StatusBack StatusBack;
			ZeroMemory(&StatusBack,sizeof(StatusBack));

			//游戏变量
			StatusBack.cbPackCount=m_cbPackCount;
			StatusBack.cbMainValue=m_cbMainValue;
			StatusBack.cbMainColor=m_cbMainColor;
			StatusBack.cbValueOrder[0]=m_cbValueOrder[0];
			StatusBack.cbValueOrder[1]=m_cbValueOrder[1];

			//叫牌变量
			StatusBack.cbCallCard=m_cbCallCard;
			StatusBack.cbCallCount=m_cbCallCount;
			StatusBack.wCallCardUser=m_wCallCardUser;

			//用户变量
			StatusBack.wBankerUser=m_wBankerUser;
			StatusBack.wCurrentUser=m_wCurrentUser;

			//扑克变量
			StatusBack.cbCardCount = m_cbHandCardCount[wChiarID];
			CopyMemory(StatusBack.cbCardData,m_cbHandCardData[wChiarID],sizeof(BYTE)*m_cbHandCardCount[wChiarID]);

			//底牌信息
			if (wChiarID==m_wBankerUser)
			{
				StatusBack.cbConcealCount=m_cbConcealCount;
				CopyMemory(StatusBack.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);
			}

			//托管变量
			CopyMemory(StatusBack.bStrustee,m_bUserStrustee,sizeof(m_bUserStrustee));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusBack,sizeof(StatusBack));
		}
	case GS_UG_PLAY:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.cbPackCount=m_cbPackCount;
			StatusPlay.cbMainValue=m_cbMainValue;
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
			StatusPlay.cbCardCount = m_cbHandCardCount[wChiarID];
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
			CopyMemory(StatusPlay.cbLastTurnCard,m_cbLastTurnCard,sizeof(m_cbLastTurnCard));
			CopyMemory(StatusPlay.cbLastTurnCount,m_cbLastTurnCount,sizeof(m_cbLastTurnCount));

			//得分变量
			StatusPlay.cbScoreCardCount=m_cbScoreCardCount;
			CopyMemory(StatusPlay.cbScoreCardData,m_cbScoreCardData,sizeof(StatusPlay.cbScoreCardData));

			//托管变量
			CopyMemory(StatusPlay.bStrustee,m_bUserStrustee,sizeof(m_bUserStrustee));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	case GS_UG_WAIT:	//等待状态
		{
			//构造数据
			CMD_S_StatusWait StatusWait;
			ZeroMemory(&StatusWait,sizeof(StatusWait));

			//用户变量
			StatusWait.wBankerUser=m_wBankerUser;

			//游戏变量
			StatusWait.cbPackCount=m_cbPackCount;
			StatusWait.cbMainValue=m_cbMainValue;
			StatusWait.cbValueOrder[0]=m_cbValueOrder[0];
			StatusWait.cbValueOrder[1]=m_cbValueOrder[1];

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusWait,sizeof(StatusWait));
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
	case SUB_C_CALL_CARD:		//用户叫牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CallCard));
			if (wDataSize!=sizeof(CMD_C_CallCard)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_CallCard * pCallCard=(CMD_C_CallCard *)pDataBuffer;
			return OnUserCallCard(pUserData->wChairID,pCallCard->cbCallCard,pCallCard->cbCallCount);
		}
	case SUB_C_CALL_FINISH:		//叫牌完成
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserCallFinish(pUserData->wChairID);
		}
	case SUB_C_CONCEAL_CARD:	//底牌扑克
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ConcealCard));
			if (wDataSize!=sizeof(CMD_C_ConcealCard)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_ConcealCard * pConcealCard=(CMD_C_ConcealCard *)pDataBuffer;
			return OnUserConcealCard(pUserData->wChairID,pConcealCard->cbConcealCard,pConcealCard->cbConcealCount);
		}
	case SUB_C_OUT_CARD:		//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//效验数据
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0]))) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserOutCard(pUserData->wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
	case SUB_C_STRUSTEE:
		{
			//变量定义
			CMD_C_Strustee *pStrustee = (CMD_C_Strustee *)pDataBuffer;

			//效验数据
			ASSERT( wDataSize == sizeof(CMD_C_Strustee) );
			if( wDataSize != sizeof(CMD_C_Strustee))  return false;

			//用户变量
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();

			//消息处理
			return OnUserStrustee(pUserData->wChairID,pStrustee->bStrustee);
		}
	case SUB_C_REQUEST_LEAVE:		//请求离开
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_RequestLeave));
			if (wDataSize!=sizeof(CMD_C_RequestLeave)) return false;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAY);
			if (pIServerUserItem->GetUserStatus()!=US_PLAY) return true;

			//消息处理
			CMD_C_RequestLeave * pRequestLeave=(CMD_C_RequestLeave *)pDataBuffer;
			return OnUserRequestLeave(pIServerUserItem->GetChairID(),(LPCTSTR)pRequestLeave->szLeaveReason);
		}
	case SUB_C_RESPONSES_LEAVE:		//请求回应
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ResponsesLeave));
			if (wDataSize!=sizeof(CMD_C_ResponsesLeave)) return false;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAY);
			if (pIServerUserItem->GetUserStatus()!=US_PLAY) return true;

			//消息处理
			CMD_C_ResponsesLeave * pResponsesLeave=(CMD_C_ResponsesLeave *)pDataBuffer;
			return OnUserResponsesLeave(pIServerUserItem->GetChairID(),pResponsesLeave->wChairID,pResponsesLeave->cbAgreeLeave);
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
	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//设置变量
	if (bLookonUser==false)
	{
		m_cbMainValue=0x02;
		m_cbValueOrder[0]=0x02;
		m_cbValueOrder[1]=0x02;
		m_wBankerUser=INVALID_CHAIR;
		if( m_bUserStrustee[wChairID] == true )
		{
			m_bUserStrustee[wChairID] = false;
			//托管标志
			CMD_S_Strustee Strustee;
			Strustee.bStrustee = m_bUserStrustee[wChairID];
			Strustee.wChairId = wChairID;
			IServerUserItem *pUserItem = NULL;
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				pUserItem = m_pITableFrame->GetServerUserItem(i);
				if( pUserItem == NULL || pUserItem == pIServerUserItem ) continue;
				m_pITableFrame->SendTableData(i,SUB_S_STRUSTEE,&Strustee,sizeof(Strustee));
			}
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STRUSTEE,&Strustee,sizeof(Strustee));
		}
	}

	return true;
}

//用户叫牌
bool CTableFrameSink::OnUserCallCard(WORD wChairID, BYTE cbCallCard, BYTE cbCallCount)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_CALL);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_CALL) return true;

	//用户效验
	ASSERT(m_bCallCard[wChairID]==false);
	if (m_bCallCard[wChairID]==true) return false;

	//数目统计
	BYTE cbHandCardCount=0;
	for (BYTE i=0;i<m_cbHandCardCount[wChairID];i++)
	{
		if (m_cbHandCardData[wChairID][i]==cbCallCard) cbHandCardCount++;
	}

	//效验数据
	ASSERT((cbHandCardCount>=cbCallCount)&&(cbCallCount>0));
	if ((cbHandCardCount<cbCallCount)||(cbCallCount==0)) return false;

	//叫牌判断
	if (m_wCallCardUser==wChairID)
	{
		//效验数据
		ASSERT((cbCallCard==m_cbCallCard)&&(cbCallCount>m_cbCallCount));
		if ((cbCallCard!=m_cbCallCard)||(cbCallCount<=m_cbCallCount)) return true;
	}
	else
	{
		//效验数据
		BYTE cbPackCount=m_GameLogic.GetPackCount();
		BYTE cbCardColor=m_GameLogic.GetCardColor(cbCallCard);
		BYTE cbCallColor=m_GameLogic.GetCardColor(m_cbCallCard);

		//效验数据
		ASSERT((cbCallCount>m_cbCallCount)||((cbCallCount==cbPackCount)&&(cbCardColor>cbCallColor||cbCallCard==0x4F)));
		if ((cbCallCount<=m_cbCallCount)&&((cbCallCount!=cbPackCount)||(cbCardColor<cbCallColor)||cbCallCard!=0x4F)) return true;
	}

	//设置变量
	m_cbCallCard=cbCallCard;
	m_wCallCardUser=wChairID;
	m_cbCallCount=cbCallCount;

	//构造数据
	CMD_S_CallCard CallCard;
	CallCard.cbCallCard=m_cbCallCard;
	CallCard.cbCallCount=m_cbCallCount;
	CallCard.wCallCardUser=m_wCallCardUser;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CALL_CARD,&CallCard,sizeof(CallCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_CARD,&CallCard,sizeof(CallCard));

	return true;
}

//叫牌完成
bool CTableFrameSink::OnUserCallFinish(WORD wChairID)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_CALL);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_CALL) return true;

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

	//重新发牌
	if (m_wCallCardUser==INVALID_CHAIR)
	{
		//设置状态
		ZeroMemory(m_bCallCard,sizeof(m_bCallCard));

		//发送扑克
		DispatchUserCard();

		return true;
	}

	//花色调整
	if (m_wCallCardUser==INVALID_CHAIR) m_cbMainColor=COLOR_NT;
	else m_cbMainColor=m_GameLogic.GetCardColor(m_cbCallCard);

	//庄家设置
	if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=m_wCallCardUser;

	//设置变量
	m_wCurrentUser=m_wBankerUser;
	m_wFirstOutUser=m_wBankerUser;
	m_GameLogic.SetMainColor(m_cbMainColor);
	m_pITableFrame->SetGameStatus(GS_UG_BACK);

	//设置底牌
	BYTE cbHandCardCount=m_cbHandCardCount[m_wCurrentUser];
	CopyMemory(&m_cbHandCardData[m_wCurrentUser][cbHandCardCount],m_cbConcealCard,m_cbConcealCount*sizeof(BYTE));
	m_cbHandCardCount[m_wCurrentUser]+=m_cbConcealCount;

	//构造数据
	CMD_S_SendConceal SendConceal;
	SendConceal.cbMainColor=m_cbMainColor;
	SendConceal.wBankerUser=m_wBankerUser;
	SendConceal.wCurrentUser=m_wCurrentUser;

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//构造底牌
		if (i==m_wCurrentUser)
		{
			SendConceal.cbConcealCount=m_cbConcealCount;
			CopyMemory(SendConceal.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);
		}
		else 
		{
			SendConceal.cbConcealCount=0;
			ZeroMemory(SendConceal.cbConcealCard,sizeof(SendConceal.cbConcealCard));
		}

		//发送消息
		m_pITableFrame->SendTableData(i,SUB_S_SEND_CONCEAL,&SendConceal,sizeof(SendConceal));
		m_pITableFrame->SendLookonData(i,SUB_S_SEND_CONCEAL,&SendConceal,sizeof(SendConceal));
	}

	return true;
}

//底牌扑克
bool CTableFrameSink::OnUserConcealCard(WORD wChairID, BYTE cbConcealCard[], BYTE cbConcealCount)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_BACK);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_BACK) return true;

	//效验数据
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbConcealCount==cbConcealCount));
	if ((wChairID!=m_wCurrentUser)||(m_cbConcealCount!=cbConcealCount)) return false;

	//删除底牌
	if (m_GameLogic.RemoveCard(cbConcealCard,cbConcealCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置扑克
	m_cbHandCardCount[wChairID]-=cbConcealCount;
	CopyMemory(m_cbConcealCard,cbConcealCard,cbConcealCount*sizeof(BYTE));

	//扑克排列
	for (WORD i=0;i<m_wPlayerCount;i++) 
	{
		ASSERT(m_cbHandCardCount[i]==m_GameLogic.GetDispatchCount());
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i]);
	}

	//设置状态
	m_pITableFrame->SetGameStatus(GS_UG_PLAY);

	//构造数据
	CMD_S_GamePlay GamePlay;
	GamePlay.wCurrentUser=m_wCurrentUser;

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//构造底牌
		if (i==m_wCurrentUser)
		{
			GamePlay.cbConcealCount=m_cbConcealCount;
			CopyMemory(GamePlay.cbConcealCard,m_cbConcealCard,sizeof(BYTE)*m_cbConcealCount);
		}
		else 
		{
			GamePlay.cbConcealCount=0;
			ZeroMemory(GamePlay.cbConcealCard,sizeof(GamePlay.cbConcealCard));
		}

		//发送消息
		m_pITableFrame->SendTableData(i,SUB_S_GAME_PLAY,&GamePlay,sizeof(GamePlay));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_PLAY,&GamePlay,sizeof(GamePlay));
	}

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_UG_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_UG_PLAY) return true;

	//效验用户
	ASSERT((wChairID==m_wCurrentUser)&&(cbCardCount<=MAX_COUNT));
	if ((wChairID!=m_wCurrentUser)||(cbCardCount>MAX_COUNT)) return false;

	//首出牌者
	if (m_wFirstOutUser==wChairID)
	{
		//类型判断
		BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount);
		if (cbCardType==CT_ERROR) return false;

		//甩牌处理
		if (cbCardType==CT_THROW_CARD)
		{
			//甩牌判断
			tagOutCardResult OutCardResult;
			ZeroMemory(&OutCardResult,sizeof(OutCardResult));
			bool bThrowSuccess=m_GameLogic.EfficacyThrowCard(cbCardData,cbCardCount,wChairID,m_cbHandCardData,m_cbHandCardCount[wChairID],OutCardResult);

			//结果处理
			if (bThrowSuccess==false)
			{
				//删除扑克
				if (m_GameLogic.RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false) return false;
				m_cbHandCardCount[wChairID]-=OutCardResult.cbCardCount;

				//出牌记录
				m_cbOutCardCount[wChairID]=OutCardResult.cbCardCount;
				CopyMemory(m_cbOutCardData[wChairID],OutCardResult.cbResultCard,OutCardResult.cbCardCount);

				//切换用户
				m_wCurrentUser=(wChairID+1)%m_wPlayerCount;

				//构造数据
				CMD_S_ThrowResult ThrowResult;
				ThrowResult.wOutCardUser=wChairID;
				ThrowResult.wCurrentUser=m_wCurrentUser;
				ThrowResult.cbThrowCardCount=cbCardCount;
				ThrowResult.cbResultCardCount=OutCardResult.cbCardCount;
				CopyMemory(&ThrowResult.cbCardDataArray[0],cbCardData,sizeof(BYTE)*cbCardCount);
				CopyMemory(&ThrowResult.cbCardDataArray[cbCardCount],OutCardResult.cbResultCard,sizeof(BYTE)*OutCardResult.cbCardCount);

				//发送计算
				BYTE cbCardDataCount=ThrowResult.cbThrowCardCount+ThrowResult.cbResultCardCount;
				WORD wSendSize=sizeof(ThrowResult)-sizeof(ThrowResult.cbCardDataArray)+cbCardDataCount*sizeof(BYTE);

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_THROW_RESULT,&ThrowResult,wSendSize);
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_THROW_RESULT,&ThrowResult,wSendSize);

				return true;
			}
		}
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
	if (m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false) 
	{
		ASSERT( FALSE );
		return false;
	}

	//最后一轮甩牌
	if( m_cbHandCardCount[wChairID] == cbCardCount )
	{
		//出牌记录
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_cbOutCardCount[i]=cbCardCount;
			if( i == wChairID )
				CopyMemory(m_cbOutCardData[i],cbCardData,cbCardCount*sizeof(BYTE));
			else 
				CopyMemory(m_cbOutCardData[i],m_cbHandCardData[i],cbCardCount*sizeof(BYTE));
		}

		//判断胜者
		m_wTurnWinner=m_GameLogic.CompareCardArray(m_cbOutCardData,m_cbOutCardCount[0],m_wFirstOutUser);

		//计算得分
		if ((m_wTurnWinner!=m_wBankerUser)&&((m_wTurnWinner+2)%m_wPlayerCount!=m_wBankerUser))
		{
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				BYTE * pcbScoreCard=&m_cbScoreCardData[m_cbScoreCardCount];
				BYTE cbMaxCount=CountArray(m_cbScoreCardData)-m_cbScoreCardCount;
				m_cbScoreCardCount+=m_GameLogic.GetScoreCard(m_cbOutCardData[i],m_cbOutCardCount[i],pcbScoreCard,cbMaxCount);
			}
		}

		//变量定义
		CMD_S_BatchCard BatchCard;
		ZeroMemory(&BatchCard,sizeof(BatchCard));

		//设置变量
		BatchCard.wOutCardUser=wChairID;
		BatchCard.cbCardCount=cbCardCount;
		BatchCard.wTurnWinner = m_wTurnWinner;

		//设置消息
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//拷贝扑克
			ASSERT(m_cbHandCardCount[i]==cbCardCount);
			CopyMemory(&BatchCard.cbCardData[i*cbCardCount],m_cbOutCardData[i],cbCardCount*sizeof(BYTE));
		}

		//设置变量
		m_wCurrentUser=INVALID_CHAIR;

		//发送数据
		WORD wHeadSize=sizeof(BatchCard)-sizeof(BatchCard.cbCardData);
		WORD wSendSize=wHeadSize+GAME_PLAYER*BatchCard.cbCardCount*sizeof(BYTE);
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BATCH_CARD,&BatchCard,wSendSize);
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_BATCH_CARD,&BatchCard,wSendSize);

		//结束游戏
		OnEventGameEnd(m_wTurnWinner,NULL,GER_NORMAL);
	}
	else
	{
		//设置变量
		m_cbHandCardCount[wChairID]-=cbCardCount;

		//出牌记录
		m_cbOutCardCount[wChairID]=cbCardCount;
		CopyMemory(m_cbOutCardData[wChairID],cbCardData,cbCardCount);

		//切换用户
		m_wCurrentUser=(wChairID+1)%m_wPlayerCount;
		if (m_wCurrentUser==m_wFirstOutUser) m_wCurrentUser=INVALID_CHAIR;

		//构造数据
		CMD_S_OutCard OutCard;
		OutCard.wOutCardUser=wChairID;
		OutCard.cbCardCount=cbCardCount;
		OutCard.wCurrentUser=m_wCurrentUser;
		CopyMemory(OutCard.cbCardData,cbCardData,cbCardCount*sizeof(BYTE));

		//发送数据
		WORD wSendSize=sizeof(OutCard)-sizeof(OutCard.cbCardData)+OutCard.cbCardCount*sizeof(BYTE);
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

		//一轮结算
		if (m_wCurrentUser==INVALID_CHAIR)
		{
			//判断胜者
			m_wTurnWinner=m_GameLogic.CompareCardArray(m_cbOutCardData,m_cbOutCardCount[0],m_wFirstOutUser);

			//计算得分
			if ((m_wTurnWinner!=m_wBankerUser)&&((m_wTurnWinner+2)%m_wPlayerCount!=m_wBankerUser))
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					BYTE * pcbScoreCard=&m_cbScoreCardData[m_cbScoreCardCount];
					BYTE cbMaxCount=CountArray(m_cbScoreCardData)-m_cbScoreCardCount;
					m_cbScoreCardCount+=m_GameLogic.GetScoreCard(m_cbOutCardData[i],m_cbOutCardCount[i],pcbScoreCard,cbMaxCount);
				}
			}

			//记录上次出牌
			CopyMemory(m_cbLastTurnCard,m_cbOutCardData,sizeof(m_cbLastTurnCard));
			CopyMemory(m_cbLastTurnCount,m_cbOutCardCount,sizeof(m_cbLastTurnCount));

			//设置变量
			if (m_cbHandCardCount[m_wTurnWinner]!=0)
			{
				m_wFirstOutUser=m_wTurnWinner;
				ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
				ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));
			}

			//用户切换
			m_wCurrentUser=(m_cbHandCardCount[m_wTurnWinner]==0)?INVALID_CHAIR:m_wTurnWinner;

			//构造数据
			CMD_TurnBalance TurnBalance;
			TurnBalance.wTurnWinner=m_wTurnWinner;
			TurnBalance.wCurrentUser=m_wCurrentUser;

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TURN_BALANCE,&TurnBalance,sizeof(TurnBalance));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TURN_BALANCE,&TurnBalance,sizeof(TurnBalance));
		}
	}

	return true;
}

//派发扑克
bool CTableFrameSink::DispatchUserCard()
{
	//混乱扑克
	BYTE cbRandCard[CELL_PACK*PACK_COUNT];
	m_GameLogic.RandCardList(cbRandCard,CELL_PACK*m_cbPackCount);

	//用户扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_cbHandCardCount[i]=m_GameLogic.GetDispatchCount();
		CopyMemory(&m_cbHandCardData[i],&cbRandCard[i*m_cbHandCardCount[i]],sizeof(BYTE)*m_cbHandCardCount[i]);
	}

	//底牌扑克
	m_cbConcealCount=CELL_PACK*m_cbPackCount-m_wPlayerCount*m_GameLogic.GetDispatchCount();
	CopyMemory(m_cbConcealCard,&cbRandCard[CELL_PACK*m_cbPackCount-m_cbConcealCount],sizeof(BYTE)*m_cbConcealCount);

	//发送开始
	CMD_S_SendCard SendCard;
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

//用户托管
bool CTableFrameSink::OnUserStrustee(WORD wChairID,bool bStrustee)
{
	//效验
	ASSERT( m_bUserStrustee[wChairID] != bStrustee );
	if( m_bUserStrustee[wChairID] == bStrustee ) return true;

	//设置变量
	m_bUserStrustee[wChairID] = bStrustee;

	//发送数据
	CMD_S_Strustee Strustee;
	Strustee.wChairId = wChairID;
	Strustee.bStrustee = bStrustee;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_STRUSTEE,&Strustee,sizeof(Strustee));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STRUSTEE,&Strustee,sizeof(Strustee));

	return true;
}

//请求离开
bool CTableFrameSink::OnUserRequestLeave(WORD wChairID, LPCTSTR pszLeaveReason)
{
	//次数判断
	if (m_wRequestTimes[wChairID]>=2)
	{
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
		m_pITableFrame->SendGameMessage(pIServerUserItem,TEXT("您请求离开的次数太多了，请稍后再次尝试！"),SMT_INFO);
	}

	//变量定义
	CMD_S_RequestLeave RequestLeave;
	ZeroMemory(&RequestLeave,sizeof(RequestLeave));

	//设置变量
	m_cbResponses[wChairID]=0;
	m_wRequestTimes[wChairID]++;

	//构造变量
	RequestLeave.wChairID=wChairID;
	lstrcpyn(RequestLeave.szLeaveReason,pszLeaveReason,CountArray(RequestLeave.szLeaveReason)-1);

	//发送请求
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);

		//发送请求
		if ((i!=wChairID)&&(pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()!=US_OFFLINE))
		{
			m_pITableFrame->SendTableData(i,SUB_S_REQUEST_LEAVE,&RequestLeave,sizeof(RequestLeave));
		}
		else
		{
			m_cbResponses[wChairID]|=(1<<i);
		}
	}

	return true;
}

//回应离开
bool CTableFrameSink::OnUserResponsesLeave(WORD wChairID, WORD wTargerChairID, BYTE cbAgreeLeave)
{
	//效验参数
	ASSERT((wTargerChairID!=wChairID)&&(wTargerChairID<GAME_PLAYER));
	if ((wTargerChairID==wChairID)||(wTargerChairID>=GAME_PLAYER)) return false;

	//消息处理
	if (m_wRequestTimes[wTargerChairID]>0)
	{
		//获取用户
		IServerUserItem * pIResponsesUserItem=m_pITableFrame->GetServerUserItem(wChairID);
		IServerUserItem * pIRequestUserItem=m_pITableFrame->GetServerUserItem(wTargerChairID);

		//构造消息
		TCHAR szMessage[128]=TEXT("");
		_snprintf(szMessage,CountArray(szMessage),TEXT("“%s”%s您离开！"),
			pIResponsesUserItem->GetAccounts(),(cbAgreeLeave==TRUE)?TEXT("同意"):TEXT("不同意"));

		//发送消息
		m_pITableFrame->SendGameMessage(pIRequestUserItem,szMessage,SMT_INFO);

		//消息处理
		if (cbAgreeLeave==TRUE)
		{
			//设置变量
			m_cbResponses[wTargerChairID]|=(1<<wChairID);

			//人数判断
			WORD wAgreeCount=0;
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				if ((m_cbResponses[wTargerChairID]&(1<<i))!=0)
				{
					wAgreeCount++;
				}
			}

			//结束判断
			if (wAgreeCount>=GAME_PLAYER)
			{
				OnEventGameEnd(INVALID_CHAIR,NULL,GER_DISMISS);
			}
		}
		//只要一人不同意,则不允许退出
		else
			m_cbResponses[wTargerChairID] = 0;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
