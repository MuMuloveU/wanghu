#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

#define RECTIFY_TIME				3										//调整时间
#define RULE_STEP_TIME				30										//规则步时
#define RULE_LIMIT_TIME				1800									//规则限时
#define RULE_TIME_OUT_COUNT			3										//规则超时

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_AllReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//规则变量
	m_bGameRule=false;
	m_GameRuleInfo.cbDirections=TRUE;
	m_GameRuleInfo.cbRegretFlag=TRUE;
	m_GameRuleInfo.cbGameMode=GM_MATCH;
	m_GameRuleInfo.wRuleStepTime=RULE_STEP_TIME;
	m_GameRuleInfo.wRuleLimitTime=RULE_LIMIT_TIME;
	m_GameRuleInfo.wRuleTimeOutCount=RULE_TIME_OUT_COUNT;

	//时间状态
	m_dwLastClock=0L;
	memset(m_wUseClock,0,sizeof(m_wUseClock));
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));
	memset(m_wTimeOutCount,0,sizeof(m_wTimeOutCount));

	//用户状态
	m_wBlackUser=0;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_cbBegStatus,0,sizeof(m_cbBegStatus));
	memset(m_cbPeaceCount,0,sizeof(m_cbPeaceCount));
	memset(m_cbRegretCount,0,sizeof(m_cbRegretCount));

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
	//规则变量
	m_bGameRule=false;

	//时间变量
	m_dwLastClock=0L;
	memset(m_wUseClock,0,sizeof(m_wUseClock));
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));
	memset(m_wTimeOutCount,0,sizeof(m_wTimeOutCount));

	//用户状态
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_cbBegStatus,0,sizeof(m_cbBegStatus));
	memset(m_cbPeaceCount,0,sizeof(m_cbPeaceCount));
	memset(m_cbRegretCount,0,sizeof(m_cbRegretCount));

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

//用户起来
bool __cdecl CTableFrameSink::OnEventUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//黑棋用户
	if ((bLookonUser==false)&&(wChairID==m_wBlackUser))
	{
		//设置变量
		m_wBlackUser=(wChairID+1)%GAME_PLAYER;

		//规则变量
		m_bGameRule=false;
		m_GameRuleInfo.cbDirections=TRUE;
		m_GameRuleInfo.cbRegretFlag=TRUE;
		m_GameRuleInfo.cbGameMode=GM_MATCH;
		m_GameRuleInfo.wRuleStepTime=RULE_STEP_TIME;
		m_GameRuleInfo.wRuleLimitTime=RULE_LIMIT_TIME;
		m_GameRuleInfo.wRuleTimeOutCount=RULE_TIME_OUT_COUNT;

		//请求设置
		if (m_pITableFrame->GetServerUserItem(m_wBlackUser)!=NULL)
		{
			CMD_S_Req_SetRult SetRult;
			SetRult.GameRuleInfo=m_GameRuleInfo;
			m_pITableFrame->SendTableData(m_wBlackUser,SUB_S_REQ_SET_RULE,&SetRult,sizeof(SetRult));
		}
	}

	return true;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//时间变量
	m_wCurrentUser=m_wBlackUser;
	m_dwLastClock=(DWORD)time(NULL);
	m_wLeftClock[0]=m_wLeftClock[1]=m_GameRuleInfo.wRuleLimitTime;
	m_wTimeOutCount[0]=m_wTimeOutCount[1]=m_GameRuleInfo.wRuleTimeOutCount;

	//重置棋盘
	m_GameLogic.ResetChessBorad();

	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	//发送开始
	CMD_S_GameStart GameStart;
	GameStart.wBlackUser=m_wBlackUser;
	GameStart.GameRuleInfo=m_GameRuleInfo;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

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
			memset(&GameEnd,0,sizeof(GameEnd));
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			memset(&ScoreInfo,0,sizeof(ScoreInfo));
			LONG lTax=0;

			//胜利玩家
			GameEnd.wWinUser=wChairID;

			//写分判断
			if (m_GameRuleInfo.cbGameMode==GM_MATCH)
			{
				//计算分差
				LONG lUserScore[2];
				IServerUserItem * pIServerUserItem1=m_pITableFrame->GetServerUserItem(0);
				IServerUserItem * pIServerUserItem2=m_pITableFrame->GetServerUserItem(1);
				lUserScore[0]=pIServerUserItem1->GetUserData()->UserScoreInfo.lScore;
				lUserScore[1]=pIServerUserItem2->GetUserData()->UserScoreInfo.lScore;
				LONG lScoreMargin=(lUserScore[0]>lUserScore[1])?(lUserScore[0]-lUserScore[1]):(lUserScore[1]-lUserScore[0]);

				//计算积分
				
				if (wChairID!=INVALID_CHAIR)
				{
					//赢棋算分
					WORD wWinUser=wChairID;
					WORD wLostUser=(wChairID+1)%GAME_PLAYER;
					if (lScoreMargin<100L)
					{
						if (lUserScore[wWinUser]>=lUserScore[wLostUser])
						{
							GameEnd.lUserScore[wWinUser]=(m_pGameServiceOption->lCellScore-lScoreMargin/10L);
							GameEnd.lUserScore[wLostUser]=-GameEnd.lUserScore[wWinUser];
						}
						else
						{
							GameEnd.lUserScore[wWinUser]=m_pGameServiceOption->lCellScore+lScoreMargin/10L;
							GameEnd.lUserScore[wLostUser]=-GameEnd.lUserScore[wWinUser];
						}
					}
					else if (lScoreMargin<200L)
					{
						GameEnd.lUserScore[wWinUser]=1L;
						GameEnd.lUserScore[wLostUser]=-1L;
					}
					else
					{
						GameEnd.lUserScore[0]=0L;
						GameEnd.lUserScore[1]=0L;
					}

					//计算税收

					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if (GameEnd.lUserScore[i]>=100L)
						{
							lTax=GameEnd.lUserScore[i]*m_pGameServiceOption->wRevenue/100L;
							GameEnd.lGameTax+=lTax;
							ScoreInfo[i].lRevenue =lTax;
							GameEnd.lUserScore[i] -=lTax;
						}
					}
					//写入积分
					ScoreInfo[0].lScore=GameEnd.lUserScore[0];
					ScoreInfo[1].lScore=GameEnd.lUserScore[1];
					ScoreInfo[wWinUser].ScoreKind=enScoreKind_Win;
					ScoreInfo[wLostUser].ScoreKind=enScoreKind_Lost;
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore,ScoreInfo[i].lRevenue,ScoreInfo[i].ScoreKind);
					}
					
				}
				else	
				{
					//和棋算分
					if (lScoreMargin<100L)
					{
						GameEnd.lUserScore[0]=(lUserScore[1]-lUserScore[0])/10L;
						GameEnd.lUserScore[1]=-GameEnd.lUserScore[0];
					}
					else if (lScoreMargin<200L)
					{
						GameEnd.lUserScore[0]=(lUserScore[1]-lUserScore[0])?1L:-1L;
						GameEnd.lUserScore[1]=-GameEnd.lUserScore[0];
					}
					else
					{
						GameEnd.lUserScore[0]=0L;
						GameEnd.lUserScore[1]=0L;
					}

					//写入积分
					ScoreInfo[0].ScoreKind=enScoreKind_Draw;
					ScoreInfo[1].ScoreKind=enScoreKind_Draw;
					ScoreInfo[0].lScore=GameEnd.lUserScore[0];
					ScoreInfo[1].lScore=GameEnd.lUserScore[1];
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore,ScoreInfo[i].lRevenue,ScoreInfo[i].ScoreKind);
					}
				}
			}

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame();

			//切换用户
			m_wBlackUser=(m_wBlackUser+1)%GAME_PLAYER;

			//请求设置
			CMD_S_Req_SetRult SetRult;
			SetRult.GameRuleInfo=m_GameRuleInfo;
			m_pITableFrame->SendTableData(m_wBlackUser,SUB_S_REQ_SET_RULE,&SetRult,sizeof(SetRult));

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//写分判断
			if (m_GameRuleInfo.cbGameMode==GM_MATCH)
			{
				//更改积分
				tagScoreInfo ScoreInfo;
				GameEnd.lUserScore[wChairID]=(-10L*m_pGameServiceOption->lCellScore);//10倍积分
				ScoreInfo.ScoreKind=enScoreKind_Flee;
				ScoreInfo.lScore=GameEnd.lUserScore[wChairID];
				m_pITableFrame->WriteUserScore(wChairID,ScoreInfo.lScore,0,ScoreInfo.ScoreKind);
			}

			//发送消息
			GameEnd.wWinUser=INVALID_CHAIR;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame();

			//切换用户
			m_wBlackUser=(wChairID+1)%GAME_PLAYER;

			//请求设置
			CMD_S_Req_SetRult SetRult;
			SetRult.GameRuleInfo=m_GameRuleInfo;
			m_pITableFrame->SendTableData(m_wBlackUser,SUB_S_REQ_SET_RULE,&SetRult,sizeof(SetRult));

			return true;
		}
	}

	//效验结果
	ASSERT(FALSE);

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStation, bool bSendSecret)
{
	switch (cbGameStation)
	{
	case GS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//用户切换
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_LOOKON)
			{
				WORD wUserCount=0;
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					if (m_pITableFrame->GetServerUserItem(i)!=NULL) wUserCount++;
				}
				if (wUserCount==1)
				{
					m_bGameRule=false;
					m_wBlackUser=wChiarID;
				}
			}
			
			//设置变量
			StatusFree.wBlackUser=m_wBlackUser;
			StatusFree.GameRuleInfo=m_GameRuleInfo;
			StatusFree.cbGameRule=(m_bGameRule==true)?TRUE:FALSE;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_PLAYING:	//游戏状态
		{
			//发送棋谱
			WORD wBufferSize=0;
			BYTE cbBuffer[SOCKET_PACKET];
			for (INT_PTR i=0;i<m_GameLogic.m_ChessManualInfo.GetCount();i++)
			{
				if ((wBufferSize+sizeof(tagChessManual))>sizeof(cbBuffer))
				{
					m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_CHESS_MANUAL,cbBuffer,wBufferSize);
					wBufferSize=0;
				}
				CopyMemory(cbBuffer+wBufferSize,&m_GameLogic.m_ChessManualInfo[i],sizeof(tagChessManual));
				wBufferSize+=sizeof(tagChessManual);
			}
			if (wBufferSize>0) m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_CHESS_MANUAL,cbBuffer,wBufferSize);

			//构造场景
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//设置场景
			StatusPlay.wBlackUser=m_wBlackUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.GameRuleInfo=m_GameRuleInfo;
			StatusPlay.wTimeOutCount=m_wTimeOutCount[wChiarID];
			CopyMemory(StatusPlay.cbBegStatus,m_cbBegStatus,sizeof(m_cbBegStatus));
			CopyMemory(StatusPlay.ChessItemArray,m_GameLogic.m_ChessItem,sizeof(StatusPlay.ChessItemArray));
			CopyMemory(StatusPlay.wChessStepCount,m_GameLogic.m_wStepCount,sizeof(StatusPlay.wChessStepCount));

			//时间变量
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_wCurrentUser==i)
				{
					WORD wUserClock=(WORD)((DWORD)time(NULL)-m_dwLastClock);
					StatusPlay.wUseClock[i]=m_wUseClock[i]+wUserClock;
					if (m_wLeftClock[i]>wUserClock) StatusPlay.wLeftClock[i]=m_wLeftClock[i]-wUserClock;
				}
				else 
				{
					StatusPlay.wUseClock[i]=m_wUseClock[i];
					StatusPlay.wLeftClock[i]=m_wLeftClock[i];
				}
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
	case SUB_C_MOVE_CHESS:		//移动棋子
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_MoveChess));
			if (wDataSize!=sizeof(CMD_C_MoveChess)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_MoveChess * pMoveChess=(CMD_C_MoveChess *)pDataBuffer;
			return OnUserMoveChess(pUserData->wChairID,pMoveChess->cbXSourcePos,pMoveChess->cbYSourcePos,pMoveChess->cbXTargetPos,pMoveChess->cbYTargetPos);
		}
	case SUB_C_TIME_OUT:		//游戏超时
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserTimeOut(pUserData->wChairID);
		}
	case SUB_C_REGRET_REQ:		//悔棋请求
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserRegretReq(pUserData->wChairID);
		}
	case SUB_C_REGRET_ANSWER:	//悔棋应答
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_RegretAnswer));
			if (wDataSize!=sizeof(CMD_C_RegretAnswer)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_RegretAnswer * pRegretAnswer=(CMD_C_RegretAnswer *)pDataBuffer;
			return OnUserRegretAnswer(pUserData->wChairID,pRegretAnswer->cbApprove);
		}
	case SUB_C_PEACE_REQ:		//求和请求
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserPeaceReq(pUserData->wChairID);
		}
	case SUB_C_PEACE_ANSWER:	//求和应答
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_PeaceAnswer));
			if (wDataSize!=sizeof(CMD_C_PeaceAnswer)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_PeaceAnswer * pPeaceAnswer=(CMD_C_PeaceAnswer *)pDataBuffer;
			return OnUserPeaceAnswer(pUserData->wChairID,pPeaceAnswer->cbApprove);
		}
	case SUB_C_GIVEUP_REQ:		//认输请求
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserGiveUp(pUserData->wChairID);
		}
	}

	return false;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_GF_USER_READY:		//用户同意
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus==US_LOOKON) return false;

			//黑棋用户
			if (pUserData->wChairID==m_wBlackUser)
			{
				//效验大小
				ASSERT(wDataSize==sizeof(CMD_C_CustomizeRult));
				if (wDataSize!=sizeof(CMD_C_CustomizeRult)) throw 0;

				//效验状态
				ASSERT(m_bGameRule==false);
				if (m_bGameRule!=false) throw 0;

				//消息处理
				CMD_C_CustomizeRult * pCustomizeRult=(CMD_C_CustomizeRult *)pDataBuffer;

				//规则效验
				pCustomizeRult->GameRuleInfo.wRuleStepTime=__min(pCustomizeRult->GameRuleInfo.wRuleStepTime,5999);
				pCustomizeRult->GameRuleInfo.wRuleLimitTime=__min(pCustomizeRult->GameRuleInfo.wRuleLimitTime,5999);
				pCustomizeRult->GameRuleInfo.wRuleTimeOutCount=__min(pCustomizeRult->GameRuleInfo.wRuleTimeOutCount,99);

				//设置消息
				m_bGameRule=true;
				m_GameRuleInfo=pCustomizeRult->GameRuleInfo;
			
				//请求同意
				WORD wAgreeChairID=(m_wBlackUser+1)%GAME_PLAYER;
				if (m_pITableFrame->GetServerUserItem(wAgreeChairID)!=NULL)
				{
					CMD_S_Req_AgreeRult AgreeRult;
					AgreeRult.GameRuleInfo=m_GameRuleInfo;
					m_pITableFrame->SendTableData(wAgreeChairID,SUB_S_REQ_AGREE_RULE,&AgreeRult,sizeof(AgreeRult));
				}
			}

			//白棋用户
			if ((pUserData->wChairID!=m_wBlackUser)&&(m_bGameRule==false)) return true;

			return false;
		}
	case SUB_GF_USER_CHAT:	//用户聊天
		{
			//游戏状态
			if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return false;

			//禁止指点
			if (m_GameRuleInfo.cbDirections==FALSE)
			{
				//权限判断
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				if (pUserData->dwMasterRight!=0) return false;

				//发送消息
				LPCTSTR pszMessage=TEXT("根据玩家规则设定，游戏正在进行中，禁止交谈活动");
				m_pITableFrame->SendGameMessage(pIServerUserItem,pszMessage,SMT_INFO);

				return true;
			}

			return false;
		}
	}

	return false;
}

//移动棋子
bool CTableFrameSink::OnUserMoveChess(WORD wChairID, BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//游戏状态
	if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return true;

	//效验参数
	ASSERT((cbXSourcePos<9)&&(cbYSourcePos<10));
	ASSERT((cbXTargetPos<9)&&(cbYTargetPos<10));
	if ((cbXSourcePos>=9)||(cbYSourcePos>=10)) return false;
	if ((cbXTargetPos>=9)||(cbYTargetPos>=10)) return false;

	//效验用户
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return false;

	//点棋判断
	BYTE cbUserColor=(wChairID==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
	const tagChessItem * pSourceChessItem=m_GameLogic.GetChessItem(cbXSourcePos,cbYSourcePos);
	ASSERT((pSourceChessItem!=NULL)&&(pSourceChessItem->cbColor==cbUserColor));
	if ((pSourceChessItem==NULL)||(pSourceChessItem->cbColor!=cbUserColor)) return false;

	//移动棋子
	if (m_GameLogic.IsWalkLegality(pSourceChessItem,cbXTargetPos,cbYTargetPos)==false) return false;
	if (m_GameLogic.MoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==false) return false;

	//结束判断
	bool bGameEnd=m_GameLogic.IsGameFinish((m_wCurrentUser==m_wBlackUser)?WHITE_CHESS:BLACK_CHESS);

	//计算时间
	DWORD dwNowClock=(DWORD)time(NULL);
	WORD wUseClock=(WORD)(dwNowClock-m_dwLastClock);
	m_wUseClock[m_wCurrentUser]+=(WORD)(dwNowClock-m_dwLastClock);
	if (m_wLeftClock[m_wCurrentUser]>=RECTIFY_TIME)
	{
		m_wLeftClock[m_wCurrentUser]-=__min(wUseClock,m_wLeftClock[m_wCurrentUser]-RECTIFY_TIME);
	}

	//设置变量
	m_dwLastClock=dwNowClock;
	m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;

	//构造数据
	CMD_S_MoveChess MoveChess;
	MoveChess.cbXSourcePos=cbXSourcePos;
	MoveChess.cbYSourcePos=cbYSourcePos;
	MoveChess.cbXTargetPos=cbXTargetPos;
	MoveChess.cbYTargetPos=cbYTargetPos;
	MoveChess.wCurrentUser=m_wCurrentUser;
	CopyMemory(MoveChess.wUseClock,m_wUseClock,sizeof(MoveChess.wUseClock));
	CopyMemory(MoveChess.wLeftClock,m_wLeftClock,sizeof(MoveChess.wLeftClock));

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_MOVE_CHESS,&MoveChess,sizeof(MoveChess));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_MOVE_CHESS,&MoveChess,sizeof(MoveChess));

	//结束游戏
	if (bGameEnd==true) OnEventGameEnd(wChairID,NULL,GER_NORMAL);

	return true;
}

//超时事件
bool CTableFrameSink::OnUserTimeOut(WORD wChairID)
{
	//游戏状态
	if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return true;

	//效验状态
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return false;

	//设置变量
	if (m_wTimeOutCount[wChairID]>0) m_wTimeOutCount[wChairID]--;
	else OnEventGameEnd((wChairID+1)%GAME_PLAYER,NULL,GER_NORMAL);

	return true;
}

//悔棋请求
bool CTableFrameSink::OnUserRegretReq(WORD wChairID)
{
	//游戏状态
	if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return true;

	//状态判断
	if ((m_cbBegStatus[wChairID]&GU_WAIT_REGRET)!=0) return false;
	if (m_GameLogic.GetStepCount((wChairID==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS)==0) return false;

	//规则限制
	if (m_GameRuleInfo.cbRegretFlag==FALSE)
	{
		//发送消息
		CMD_S_RegretFaile RegretFaile;
		RegretFaile.cbFaileReason=FR_RULE_LIMIT;
		m_pITableFrame->SendTableData(wChairID,SUB_S_REGRET_FAILE,&RegretFaile,sizeof(RegretFaile));

		//系统消息
		LPCTSTR pszMessage=TEXT("根据规则设置，此盘游戏禁止悔棋。");
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
		m_pITableFrame->SendGameMessage(pIServerUserItem,pszMessage,SMT_INFO);

		return true;
	}

	//判断次数
	if (m_cbRegretCount[wChairID]==5)
	{
		//发送消息
		CMD_S_RegretFaile RegretFaile;
		RegretFaile.cbFaileReason=FR_COUNT_LIMIT;
		m_pITableFrame->SendTableData(wChairID,SUB_S_REGRET_FAILE,&RegretFaile,sizeof(RegretFaile));

		//系统消息
		LPCTSTR pszMessage=TEXT("你已经请求 [ 悔棋 ] 超过 5 次了，不能再次悔棋。");
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
		m_pITableFrame->SendGameMessage(pIServerUserItem,pszMessage,SMT_INFO);

		return true;
	}

	//设置变量
	m_cbRegretCount[wChairID]++;
	m_cbBegStatus[wChairID]|=GU_WAIT_REGRET;

	//发送消息
	m_pITableFrame->SendTableData((wChairID+1)%GAME_PLAYER,SUB_S_REGRET_REQ);

	return true;
}

//悔棋应答
bool CTableFrameSink::OnUserRegretAnswer(WORD wChairID, BYTE cbApprove)
{
	//游戏状态
	if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return true;

	//状态判断
	WORD wRegretUser=(wChairID+1)%GAME_PLAYER;
	if ((m_cbBegStatus[wRegretUser]&GU_WAIT_REGRET)==0) return false;

	//设置变量
	m_cbBegStatus[wRegretUser]&=~GU_WAIT_REGRET;

	//消息处理
	if (cbApprove==FALSE)
	{
		CMD_S_RegretFaile RegretFaile;
		RegretFaile.cbFaileReason=FR_PLAYER_OPPOSE;
		m_pITableFrame->SendTableData(wRegretUser,SUB_S_REGRET_FAILE,&RegretFaile,sizeof(RegretFaile));
		return true;
	}

	//构造数据
	CMD_S_RegretResult RegretResult;
	memset(&RegretResult,0,sizeof(RegretResult));

	//计算次数
	WORD wStepCount=m_GameLogic.GetStepCount((wRegretUser==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);
	if (wStepCount>0) RegretResult.cbRegretCount++;
	wStepCount=m_GameLogic.GetStepCount((wChairID==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);
	if ((wRegretUser==m_wCurrentUser)&&(wStepCount>0)) RegretResult.cbRegretCount++;

	//悔棋操作
	m_GameLogic.RegretChess(RegretResult.cbRegretCount);

	//计算时间
	DWORD dwNowClock=(DWORD)time(NULL);
	WORD wUseClock=(WORD)(dwNowClock-m_dwLastClock);
	m_wUseClock[m_wCurrentUser]+=(WORD)(dwNowClock-m_dwLastClock);
	if (m_wLeftClock[m_wCurrentUser]>=RECTIFY_TIME)
	{
		m_wLeftClock[m_wCurrentUser]-=__min(wUseClock,m_wLeftClock[m_wCurrentUser]-RECTIFY_TIME);
	}

	//切换用户
	m_dwLastClock=dwNowClock;
	if (RegretResult.cbRegretCount>0) m_wCurrentUser=wRegretUser;

	//构造数据
	RegretResult.wRegretUser=wRegretUser;
	RegretResult.wCurrentUser=m_wCurrentUser;
	CopyMemory(RegretResult.wUseClock,m_wUseClock,sizeof(RegretResult.wUseClock));
	CopyMemory(RegretResult.wLeftClock,m_wLeftClock,sizeof(RegretResult.wLeftClock));

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_REGRET_RESULT,&RegretResult,sizeof(RegretResult));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_REGRET_RESULT,&RegretResult,sizeof(RegretResult));

	return true;
}

//求和请求
bool CTableFrameSink::OnUserPeaceReq(WORD wChairID)
{
	//游戏状态
	if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return true;

	//用户状态
	ASSERT((m_cbBegStatus[wChairID]&GU_WAIT_PEACE)==0);
	if ((m_cbBegStatus[wChairID]&GU_WAIT_PEACE)!=0) return false;

	//求和次数
	if (m_cbPeaceCount[wChairID]>=3)
	{
		LPCTSTR pszMessage=TEXT("你的 [ 求和 ] 次数已经超过 3 次了，请求不能处理。");
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
		m_pITableFrame->SendGameMessage(pIServerUserItem,pszMessage,SMT_INFO);
		return true;
	}

	//设置变量
	m_cbPeaceCount[wChairID]++;
	m_cbBegStatus[wChairID]|=GU_WAIT_PEACE;

	//发送消息
	m_pITableFrame->SendTableData((wChairID+1)%GAME_PLAYER,SUB_S_PEACE_REQ);

	return true;
}

//求和应答
bool CTableFrameSink::OnUserPeaceAnswer(WORD wChairID, BYTE cbApprove)
{
	//游戏状态
	if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return true;

	//用户状态
	WORD wPeaceUser=(wChairID+1)%GAME_PLAYER;
	if ((m_cbBegStatus[wPeaceUser]&GU_WAIT_PEACE)==0) return true;

	//设置变量
	m_cbBegStatus[wPeaceUser]&=~GU_WAIT_PEACE;

	//消息处理
	if (cbApprove==FALSE) m_pITableFrame->SendTableData(wPeaceUser,SUB_S_PEACE_ANSWER);
	else OnEventGameEnd(INVALID_CHAIR,NULL,GER_NORMAL);

	return true;
}

//认输事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID)
{
	//游戏状态
	if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return true;

	//结束游戏
	OnEventGameEnd((wChairID+1)%GAME_PLAYER,NULL,GER_NORMAL);

	return true;
}

//////////////////////////////////////////////////////////////////////////
