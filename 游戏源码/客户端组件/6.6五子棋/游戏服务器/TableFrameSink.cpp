#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define GAME_CLOCK					600										//游戏局时

//////////////////////////////////////////////////////////////////////////
//静态变量

const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//规则变量
	m_bRestrict=true;
	m_bTradeUser=true;
	m_bDoubleChess=true;

	//时间变量
	m_dwLastClock=0L;
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));

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
	//时间变量
	m_dwLastClock=0L;
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));

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

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//设置变量
	m_wCurrentUser=m_wBlackUser;
	m_dwLastClock=(DWORD)time(NULL);
	m_wLeftClock[0]=m_wLeftClock[1]=GAME_CLOCK;

	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	//设置棋盘
	m_GameLogic.InitGameLogic();

	//发送开始
	CMD_S_GameStart GameStart;
	GameStart.wGameClock=GAME_CLOCK;
	GameStart.wBlackUser=m_wBlackUser;
	GameStart.cbRestrict=m_bRestrict?TRUE:FALSE;
	GameStart.cbTradeUser=m_bTradeUser?TRUE:FALSE;
	GameStart.cbDoubleChess=m_bDoubleChess?TRUE:FALSE;
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

			//胜利玩家
			GameEnd.wWinUser=wChairID;

			//计算分差
			LONG lUserScore[2];
			IServerUserItem * pIServerUserItem1=m_pITableFrame->GetServerUserItem(0);
			IServerUserItem * pIServerUserItem2=m_pITableFrame->GetServerUserItem(1);
			lUserScore[0]=pIServerUserItem1->GetUserData()->UserScoreInfo.lScore;
			lUserScore[1]=pIServerUserItem2->GetUserData()->UserScoreInfo.lScore;
			LONG lScoreMargin=(lUserScore[0]>lUserScore[1])?(lUserScore[0]-lUserScore[1]):(lUserScore[1]-lUserScore[0]);

			//计算积分
			LONG lScore=0;
			enScoreKind ScoreKind;
			if (wChairID!=INVALID_CHAIR)
			{
				//赢棋算分
				WORD wWinUser=wChairID;
				WORD wLostUser=(wChairID+1)%GAME_PLAYER;
				if (lScoreMargin<100L)
				{
					if (lUserScore[wWinUser]>=lUserScore[wLostUser])
					{
						GameEnd.lUserScore[wWinUser]=10L-lScoreMargin/10L;
						GameEnd.lUserScore[wLostUser]=-GameEnd.lUserScore[wWinUser];
					}
					else
					{
						GameEnd.lUserScore[wWinUser]=10L+lScoreMargin/10L;
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

				//写入积分
				lScore=GameEnd.lUserScore[wWinUser];
				ScoreKind=enScoreKind_Win;
				m_pITableFrame->WriteUserScore(wWinUser,lScore,0,ScoreKind);

				lScore=GameEnd.lUserScore[wLostUser];
				ScoreKind=enScoreKind_Lost;
				m_pITableFrame->WriteUserScore(wLostUser,lScore,0,ScoreKind);
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
				lScore=GameEnd.lUserScore[0];
				ScoreKind=enScoreKind_Draw;
				m_pITableFrame->WriteUserScore((WORD)0,lScore,0,ScoreKind);

				lScore=GameEnd.lUserScore[1];
				ScoreKind=enScoreKind_Draw;
				m_pITableFrame->WriteUserScore((WORD)1,lScore,0,ScoreKind);
			}

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame();

			//切换用户
			m_wBlackUser=(m_wBlackUser+1)%GAME_PLAYER;

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//更改积分
			LONG lScore=0;
			enScoreKind ScoreKind;
			lScore=-30L;
			ScoreKind=enScoreKind_Flee;
			m_pITableFrame->WriteUserScore(wChairID,lScore,0,ScoreKind);

			//发送消息
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));
			GameEnd.wWinUser=INVALID_CHAIR;
			GameEnd.lUserScore[wChairID]=lScore;

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame();

			//切换用户
			m_wBlackUser=(m_wBlackUser+1)%GAME_PLAYER;

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
			StatusFree.wBlackUser=m_wBlackUser;

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

			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//设置变量
			StatusPlay.wGameClock=GAME_CLOCK;
			StatusPlay.wBlackUser=m_wBlackUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.cbRestrict=m_bRestrict?TRUE:FALSE;
			StatusPlay.cbTradeUser=m_bTradeUser?TRUE:FALSE;
			StatusPlay.cbDoubleChess=m_bDoubleChess?TRUE:FALSE;
			CopyMemory(StatusPlay.cbBegStatus,m_cbBegStatus,sizeof(m_cbBegStatus));

			//时间变量
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_wCurrentUser==i)
				{
					WORD wUserClock=(WORD)((DWORD)time(NULL)-m_dwLastClock);
					if (m_wLeftClock[i]>wUserClock) StatusPlay.wLeftClock[i]=m_wLeftClock[i]-wUserClock;
				}
				else StatusPlay.wLeftClock[i]=m_wLeftClock[i];
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
	case SUB_C_PLACE_CHESS:		//放置棋子
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_PlaceChess));
			if (wDataSize!=sizeof(CMD_C_PlaceChess)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_PlaceChess * pPlaceChess=(CMD_C_PlaceChess *)pDataBuffer;
			return OnUserPlaceChess(pUserData->wChairID,pPlaceChess->cbXPos,pPlaceChess->cbYPos);
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
	case SUB_C_TRADE_REQ:		//对换请求
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			return OnUserTradeReq(pUserData->wChairID);
		}
	}

	return false;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//放棋事件
bool CTableFrameSink::OnUserPlaceChess(WORD wChairID, BYTE cbXPos, BYTE cbYPos)
{
	//游戏状态
	if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return true;

	//效验用户
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return false;

	//放置棋子
	BYTE cbColor=(wChairID==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
	if (m_GameLogic.PlaceChess(cbXPos,cbYPos,cbColor)==0) return false;

	//计算时间
	DWORD dwNowClock=(DWORD)time(NULL);
	WORD wUseClock=(WORD)(dwNowClock-m_dwLastClock);
	m_dwLastClock=dwNowClock;
	m_wLeftClock[m_wCurrentUser]-=__min(wUseClock,m_wLeftClock[m_wCurrentUser]);

	//结束判断
	bool bGameEnd=false;
	WORD wWinUser=INVALID_CHAIR;
	if (m_GameLogic.WinEstimate(cbXPos,cbYPos)==true)
	{
		bGameEnd=true;
		wWinUser=wChairID;
	}
	else if ((m_GameLogic.GetStepCount(BLACK_CHESS)+m_GameLogic.GetStepCount(WHITE_CHESS))==225)
	{
		bGameEnd=true;
		wWinUser=INVALID_CHAIR;
	}
	else m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;

	//构造数据
	CMD_S_PlaceChess PlaceChess;
	PlaceChess.cbXPos=cbXPos;
	PlaceChess.cbYPos=cbYPos;
	PlaceChess.wPlaceUser=wChairID;
	PlaceChess.wCurrentUser=m_wCurrentUser;
	CopyMemory(PlaceChess.wLeftClock,m_wLeftClock,sizeof(PlaceChess.wLeftClock));

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_CHESS,&PlaceChess,sizeof(PlaceChess));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_CHESS,&PlaceChess,sizeof(PlaceChess));

	//结束游戏
	if (bGameEnd==true) OnEventGameEnd(wWinUser,NULL,GER_NORMAL);

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

	//变量定义
	CMD_S_RegretResult RegretResult;
	memset(&RegretResult,0,sizeof(RegretResult));

	//计算次数
	WORD wIndex=0;
	BYTE cbRegretColor=(wRegretUser==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
	INT_PTR nChessManualCount=m_GameLogic.m_ChessManualInfo.GetCount();
	while (wIndex<nChessManualCount)
	{
		//获取棋谱
		tagChessManual * pChessManual=&m_GameLogic.m_ChessManualInfo[nChessManualCount-wIndex-1];
		if (pChessManual->cbColor==cbRegretColor) 
		{
			RegretResult.wRegretCount++;
			break;
		}
		
		//设置变量
		wIndex++;
		RegretResult.wRegretCount++;
	}

	//悔棋操作
	m_GameLogic.RegretChess(RegretResult.wRegretCount);

	//计算时间
	DWORD dwNowClock=(DWORD)time(NULL);
	WORD wUseClock=(WORD)(dwNowClock-m_dwLastClock);
	m_wLeftClock[m_wCurrentUser]-=__min(wUseClock,m_wLeftClock[m_wCurrentUser]);

	//切换用户
	if (RegretResult.wRegretCount>0) m_wCurrentUser=wRegretUser;

	//构造数据
	RegretResult.wRegretUser=wRegretUser;
	RegretResult.wCurrentUser=m_wCurrentUser;
	CopyMemory(RegretResult.wLeftClock,m_wLeftClock,sizeof(RegretResult.wLeftClock));

	//构造数据
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

//对换请求
bool CTableFrameSink::OnUserTradeReq(WORD wChairID)
{
	//游戏状态
	if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return true;

	//玩家判断
	if (wChairID==m_wBlackUser) return false;
	if (m_wCurrentUser!=wChairID) return false;
	if (m_GameLogic.GetStepCount(BLACK_CHESS)!=2) return false;

	//计算时间
	DWORD dwNowClock=(DWORD)time(NULL);
	WORD wUseClock=(WORD)(dwNowClock-m_dwLastClock);
	m_dwLastClock=dwNowClock;
	m_wLeftClock[m_wCurrentUser]-=__min(wUseClock,m_wLeftClock[m_wCurrentUser]);

	//用户交换
	m_wCurrentUser=m_wBlackUser;
	m_wBlackUser=(m_wBlackUser+1)%GAME_PLAYER;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_BLACK_TRADE);

	return true;
}

//////////////////////////////////////////////////////////////////////////
