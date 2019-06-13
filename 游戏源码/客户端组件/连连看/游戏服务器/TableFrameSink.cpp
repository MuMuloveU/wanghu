#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	m_bIsGameEnd=false;
	InitVariable();

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

	//设置参数
	//m_pITableFrame->SetLessTimes(69L);
	return true;
}

void CTableFrameSink::InitVariable()
{
	for(int p = 0; p<GAME_PLAYER; p++)
	for(int i=0; i<CHESSCOUNT_H; ++i)
	for(int j=0; j<CHESSCOUNT_W; ++j)
	{
		m_bChess[p][i][j] = ERROR_ALL;
	}
	memset(m_bPlayStatus,GS_FREE,sizeof(m_bPlayStatus));
	memset(m_bTimerCnt,60,sizeof(m_bTimerCnt));

	memset(m_bSeriesLinkCnt,0,sizeof(m_bSeriesLinkCnt));

	memset(m_bPropCnt,0,sizeof(m_bPropCnt));
	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_bPropCnt[i][0]=3;
		m_bPropCnt[i][1]=3;
		m_bPropCnt[i][2]=0;
		//m_bPropCnt[i][3]=0;
		//m_bPropCnt[i][4]=0;
	}
	m_WinnerBonus = 0;
}

//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	InitVariable();
	return;
}

//开始模式
enStartMode __cdecl CTableFrameSink::GetGameStartMode()
{
	return enStartMode_AllReady;
}

//游戏状态
bool __cdecl CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<GAME_PLAYER);
	return m_bPlayStatus[wChairID]?true:false;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{

	m_bIsGameEnd=false;
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	srand((unsigned int)time(NULL));
	int nRandNum=rand()%19;
	CMD_S_GameStart  start;
	//ZeroMemory(&start,sizeof(start));
	//memset(&start,ERROR_ALL,sizeof(start));

	memset(start.bChess, ERROR_ALL, sizeof(start.bChess));

	//初始化棋盘
	int tempvalue,randnum;
	CArrayTemplate<CPoint> Map;
	for(int i=0;i<CHESSCOUNT_H;i++)
	{
		for(int j=0;j<CHESSCOUNT_W;j++)
		{
			if(LLK_MAP[nRandNum][i][j])
			{
				Map.Add(CPoint(i,j));
			}
		}
	}
	if(Map.GetCount()%2 == 1)
	{
		TRACE("地图棋子为奇数");
		return false;
	}
	//随地图
	for(; Map.GetCount()!=0;)
	{
		//随机一个棋子类型
		if(rand()%10)
		{
			tempvalue = RandNumber(0,MAX_CHESS_VALUE);
		}
		else
		{
			tempvalue =RandNumber(PROP_MIN_VALUE,PROP_MIN_VALUE+PROP_MAX_CNT);
		}
		//找出第一个位置
		randnum = RandNumber(0,(int)Map.GetCount());
		start.bChess[Map.ElementAt(randnum).x][Map.ElementAt(randnum).y] = tempvalue;
		Map.RemoveAt(randnum);
		//找出第二个位置
		randnum = RandNumber(0,(int)Map.GetCount());
		start.bChess[Map.ElementAt(randnum).x][Map.ElementAt(randnum).y] = tempvalue;
		Map.RemoveAt(randnum);
	}
	IServerUserItem * pIServerUserItem;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
		if (pIServerUserItem!=NULL)
		{
			m_bPlayStatus [i] = GS_PLAYING;
			memcpy(m_bChess[i],start.bChess,sizeof(m_bChess[i]));
		}
		else
			m_bPlayStatus [i] = GS_FREE;
	}
	memcpy(start.bPlayState, m_bPlayStatus, sizeof(m_bPlayStatus));
	//发送地图数据
	for(int i=0;i<GAME_PLAYER;i++)
	{
		memcpy(start.bPropCnt,m_bPropCnt[i],sizeof(m_bPropCnt[i]));
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&start,sizeof(start));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&start,sizeof(start));
	}
	return true;
}

int	CTableFrameSink::RandNumber(int nMin,int nMax)
{
	return rand()%(nMax-nMin) + nMin;
}


//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	if(m_bPlayStatus[wChairID] !=GS_FREE){
		return SubGameLost(wChairID, PLAYER_ESCAPE);
	}
	m_pITableFrame->SetGameStatus(GS_FREE);
	return true;
}

bool CTableFrameSink::SubGameLost(WORD wSeat, BYTE bEndReason)
{	
	CMD_S_Lost  LostSeat;
	LostSeat.wSeat =wSeat;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LOST,&LostSeat,sizeof(LostSeat));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LOST,&LostSeat,sizeof(LostSeat));

	BYTE bPlayCnt =0;
	WORD seat=0;

	if(bEndReason == PLAYER_ESCAPE)
	{
		m_bPlayStatus[wSeat] =GS_FREE;
		tagScoreInfo score;
		ZeroMemory(&score,sizeof(score));
		score.lScore =-AccountChessCount(m_bChess[wSeat])/2;
		//m_WinnerBonus -= score.lScore;
		score.ScoreKind =enScoreKind_Flee;
		//RetailOutput("\nSubGameLost score.lScore = %d", score.lScore);
		//m_pITableFrame->WriteUserScore(wSeat,score);
		m_pITableFrame->WriteUserScore(wSeat,score.lScore,score.lRevenue,score.ScoreKind);
	} else if(bEndReason == PLAYER_OVERTIME){
		m_bPlayStatus[wSeat] = GS_PLAYING_LOSE;
	}

	//只剩下一个玩家了
	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(m_bPlayStatus[i] ==GS_PLAYING && wSeat != i)
		{
			seat=i;
			bPlayCnt++;
		}
	}

	if(bPlayCnt ==1)
	{
		return GameEnd(seat,GER_NO_PLAYER);
	} 
	return true;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret)
{
	switch (bGameStatus)
	{
	case GS_FREE:		//空闲状态
		{
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,NULL,0);
		}
	case GS_PLAYING:	//游戏状态
		{
			CMD_S_GameState	GameStatus;
			for(int i=0;i< GAME_PLAYER; i++){
				GameStatus.bPlayState[i] = (m_bPlayStatus[i]==GS_PLAYING_LOSE?GS_FREE:m_bPlayStatus[i]);
				//memcpy(GameStatus.bPlayState,m_bPlayStatus,sizeof(m_bPlayStatus));;
			}
			GameStatus.nLeaveTime =m_bTimerCnt[wChairID];
			memcpy(GameStatus.map,m_bChess,sizeof(m_bChess));
			memcpy(GameStatus.bPropCnt,m_bPropCnt[wChairID],sizeof(m_bPropCnt[wChairID]));
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&GameStatus,sizeof(GameStatus));
		}
	default:
		{
			return false;
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
	
	if(m_bIsGameEnd)
	{
		return true;
	}
	switch (wSubCmdID)
	{
	case SUB_C_GUIDE:
		{
			WORD wSeat = pIServerUserItem->GetChairID();

			//指南针道具
			m_bPropCnt[wSeat][PROP_GUIDE]--;
			return true;
		}
	case SUB_C_BOMB:
		{
			WORD wSeat = pIServerUserItem->GetChairID();

			//指南针道具
			m_bPropCnt[wSeat][PROP_BOM]--;
			return true;
		}
	case SUB_C_STOPIT:
		{
			return true;
			/*if(wDataSize != sizeof(CMD_C_StopIt)) return false;
			CMD_C_StopIt *pCnt = (CMD_C_StopIt *)pDataBuffer;
			WORD wSeat = pIServerUserItem->GetChairID();
			CMD_S_StopIt Stop;
			Stop.nSrcUserId =wSeat;
			Stop.nDesUserId =pCnt->nDesUserId;
			//禁手道具
			m_bPropCnt[wSeat][PROP_STOPIT]--;
			Stop.nPropCount = m_bPropCnt[wSeat][PROP_STOPIT];

			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_STOPIT,&Stop,sizeof(Stop));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STOPIT,&Stop,sizeof(Stop));
			return true;*/
		}
	case SUB_C_CLOG:
		{
			return true;
			/*if(wDataSize != sizeof(CMD_C_Clog)) return false;
			CMD_C_Clog *pCnt = (CMD_C_Clog *)pDataBuffer;
			WORD wSeat = pIServerUserItem->GetChairID();
			CMD_S_Clog  Clog;
			memset(&Clog,ERROR_ALL,sizeof(Clog));
			Clog.nDesUserId = pCnt->nDesUserId;
			Clog.nSrcUserId = wSeat;
			CArrayTemplate<CPoint> Map;
			for(int i=0;i<CHESSCOUNT_H;i++)
			{
				for(int j=0;j<CHESSCOUNT_W;j++)
				{
					if(m_bChess[pCnt->nDesUserId][i][j] == ERROR_ALL &&i>0 &&i<CHESSCOUNT_H-1&&\
						j>0 && j<CHESSCOUNT_W-1)
					{
						Map.Add(CPoint(i,j));
					}
				}
			}
			if(Map.GetCount())
			{
				
				int randMaxNum=Map.GetCount() >=12?12:Map.GetCount();
				//取偶数
				randMaxNum = randMaxNum%2?randMaxNum-1:randMaxNum;
				srand((unsigned int)time(NULL));
				int pos=0;
				while(randMaxNum>0)
				{
					int chessValue =RandNumber(0,MAX_CHESS_VALUE);
					int randNum = rand()%Map.GetCount();
					Clog.nChessPosX[pos] =Map.GetAt(randNum).y;
					Clog.nChessPosY[pos] =Map.GetAt(randNum).x;
					Clog.nChessFace[pos]=chessValue;
					pos++;
					Map.RemoveAt(randNum);
					randMaxNum--;
					randNum = rand()%Map.GetCount();
					Clog.nChessPosX[pos] =Map.GetAt(randNum).y;
					Clog.nChessPosY[pos] =Map.GetAt(randNum).x;
					Clog.nChessFace[pos]=chessValue;
					pos++;
					Map.RemoveAt(randNum);
					randMaxNum--;
				}
			}

			//障碍道具
			m_bPropCnt[wSeat][PROP_CLOG]--;
			Clog.nPropCount = m_bPropCnt[wSeat][PROP_CLOG];

			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CLOG,&Clog,sizeof(Clog));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CLOG,&Clog,sizeof(Clog));

			return true;*/
		}
	case SUB_C_TIMER:
		{
			if(wDataSize != sizeof(CMD_C_Timer)) return false;
			CMD_C_Timer *pCnt = (CMD_C_Timer *)pDataBuffer;
			WORD wSeat = pIServerUserItem->GetChairID();
			m_bTimerCnt[wSeat]=pCnt->nTime;
			CString str;
			str.Format("--%d--\n",m_bTimerCnt[wSeat]);
			TRACE(str);
			if(m_bTimerCnt[wSeat] ==0)
			{
				return SubGameLost(wSeat, PLAYER_OVERTIME);
			}
			return true;
		}
	case SUB_C_LINK:
		{
			if(wDataSize != sizeof(CMD_C_Link)) return false;
			CMD_C_Link *pPos = (CMD_C_Link *)pDataBuffer;
			WORD wSeat = pIServerUserItem->GetChairID();
			CArrayTemplate<CPoint> path;
			CPoint p1,p2;
			if(!m_GameLogic.IsLink(m_bChess[wSeat],CPoint(pPos->nFirst_X,pPos->nFirst_Y),CPoint(pPos->nEnd_X,pPos->nEnd_Y),\
				&path,&p1,&p2))
			{
				return false;
			}
			else
			{
				if(pPos->bLinkKind ==LK_SERIES_LINK)
				{
					m_bSeriesLinkCnt[wSeat] ++;
				}
				BYTE bValue =m_bChess[wSeat][pPos->nFirst_Y][pPos->nFirst_X];
				m_bChess[wSeat][pPos->nFirst_Y][pPos->nFirst_X]=ERROR_ALL;
				m_bChess[wSeat][pPos->nEnd_Y][pPos->nEnd_X]=ERROR_ALL;

				CMD_S_Link LinkPos;
				ZeroMemory(&LinkPos,sizeof(LinkPos));
				LinkPos.nEnd_X =pPos->nEnd_X;
				LinkPos.nEnd_Y =pPos->nEnd_Y;
				LinkPos.nFirst_X =pPos->nFirst_X;
				LinkPos.nFirst_Y =pPos->nFirst_Y;
				LinkPos.nCurrentPlayer = wSeat;
				if(bValue >=PROP_MIN_VALUE && bValue < 53)
				{
					m_bPropCnt[wSeat][bValue-PROP_MIN_VALUE] ++;
					LinkPos.nPropCount =m_bPropCnt[wSeat][bValue-PROP_MIN_VALUE];
					LinkPos.nPropType  = bValue-PROP_MIN_VALUE;
				}
				else
					LinkPos.nPropType = ERROR_ALL;

				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LINK,&LinkPos,sizeof(LinkPos));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LINK,&LinkPos,sizeof(LinkPos));

				//检验游戏是否结束
				bool bIsGameEnd=true;
				for(int i=0;i<CHESSCOUNT_H;i++)
				{
					for(int j=0;j<CHESSCOUNT_W;j++)
					{
						if(m_bChess[wSeat][i][j] != ERROR_ALL)
						{
							i=CHESSCOUNT_H;
							bIsGameEnd=false;
							break;
						}
					}
				}
				if(bIsGameEnd)
				{
					GameEnd(wSeat,GER_NORMAL);
				}

				return true;
			}
		}
	case SUB_C_REALIGN:
		{
			if(wDataSize != sizeof(CMD_C_Realign)) return false;
			CMD_C_Realign *pcRealign = (CMD_C_Realign *)pDataBuffer;
			WORD wSeat = pIServerUserItem->GetChairID();

			memcpy(m_bChess[wSeat],pcRealign->nChess,sizeof(pcRealign->nChess));

			CMD_S_Realign sRealign;
			sRealign.nCurrentPlayer = wSeat;
			memcpy(sRealign.nChess,pcRealign->nChess,sizeof(pcRealign->nChess));

			//4为障碍道具
			m_bPropCnt[wSeat][PROP_RESET]--;
			sRealign.nPropCount = m_bPropCnt[wSeat][PROP_RESET];

			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_REALIGN,&sRealign,sizeof(sRealign));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_REALIGN,&sRealign,sizeof(sRealign));

			return true;
		}
	default:
		{
			return false;
		}
	}

	return false;
}

bool CTableFrameSink::GameEnd(WORD wSeat,BYTE bEndReason)
{
	m_bIsGameEnd=true;
	switch(bEndReason)
	{
	case GER_NORMAL:
	case GER_NO_PLAYER:
		{
			//构造数据
			CMD_S_GameOver  gameEnd;
			ZeroMemory(&gameEnd,sizeof(gameEnd));
			BYTE iOrder=0;

			////计算得分
			//for(int i=0;i<GAME_PLAYER;i++)
			//{
			//	if(m_bPlayStatus[i]==0) continue;
			//	if(m_bPlayStatus[i] == GS_PLAYING || m_bPlayStatus[i] == GS_PLAYING_LOSE)
			//	{
			//		if(i != wSeat)
			//		{
			//			gameEnd.nScores[i] = -AccountChessCount(m_bChess[i]) /2;				
			//		}
			//	}			
			//}

			////排名
			//for(int i = 0; i< GAME_PLAYER; i++)
			//{
			//	if(m_bPlayStatus[i] == GS_PLAYING || m_bPlayStatus[i] == GS_PLAYING_LOSE)
			//	{
			//		iOrder =2;
			//		for( int j= 0; j <GAME_PLAYER; j++)
			//		{
			//			if((m_bPlayStatus[i] == GS_PLAYING  || m_bPlayStatus[i] == GS_PLAYING_LOSE) && gameEnd.nScores[i] > gameEnd.nScores[j] && wSeat != i)
			//			{
			//				iOrder ++;
			//			}
			//		}
			//		gameEnd.bOrder[i] = iOrder;
			//		if(wSeat != i) gameEnd.nScores[wSeat] -= gameEnd.nScores[i];
			//	}
			//}
			//gameEnd.bOrder[wSeat] =1;
			//gameEnd.nScores[wSeat] += m_WinnerBonus;

///////////////////////////////////////////////////////////////////////////////////////////////////

			//计算得分
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bPlayStatus[i]==0) continue;
				if(i==wSeat) continue;
				gameEnd.nScores[i]=-AccountChessCount(m_bChess[i]) /2;
				gameEnd.nScores[wSeat]+=AccountChessCount(m_bChess[i]) /2;
			}

			//前面名次排序算法有问题，重新排列如下
			//先将分数设置为最大负分
			int Score1=-(CHESSCOUNT_W*CHESSCOUNT_H),Score2=-(CHESSCOUNT_W*CHESSCOUNT_H),Score3=-(CHESSCOUNT_W*CHESSCOUNT_H),Score4=-(CHESSCOUNT_W*CHESSCOUNT_H);
			//最高分
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bPlayStatus[i]==0) continue;
				if(gameEnd.nScores[i]>Score1) Score1=gameEnd.nScores[i];
			}
			//第2名
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bPlayStatus[i]==0) continue;
				if(gameEnd.nScores[i]>Score2&&gameEnd.nScores[i]<Score1) Score2=gameEnd.nScores[i];
			}
			//第3名
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bPlayStatus[i]==0) continue;
				if(gameEnd.nScores[i]>Score3&&gameEnd.nScores[i]<Score1&&gameEnd.nScores[i]<Score2) Score3=gameEnd.nScores[i];
			}
			//第4名
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bPlayStatus[i]==0) continue;
				if(gameEnd.nScores[i]>Score4&&gameEnd.nScores[i]<Score1&&gameEnd.nScores[i]<Score2&&gameEnd.nScores[i]<Score3) Score4=gameEnd.nScores[i];
			}
			//排名
			ZeroMemory(gameEnd.bOrder,sizeof(gameEnd.bOrder)); 
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(gameEnd.nScores[i]==Score1) gameEnd.bOrder[i]=1;
				if(gameEnd.nScores[i]==Score2) gameEnd.bOrder[i]=2;
				if(gameEnd.nScores[i]==Score3) gameEnd.bOrder[i]=3;
				if(gameEnd.nScores[i]==Score4) gameEnd.bOrder[i]=4;
			}
//////////////////////////////////////////////////////////////////////////////////////////////////
	
			//写分
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bPlayStatus[i]==0) continue;
				LONG lScore=gameEnd.nScores[i];
				LONG lRevenue=0;
				enScoreKind	ScoreKind;
				if(lScore>0)
				{
					ScoreKind=enScoreKind_Win;
					if(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
						lRevenue=lScore*m_pGameServiceOption->wRevenue/1000;
					lScore-=lRevenue;
				}
				else if(lScore==0) 
				{
					ScoreKind=enScoreKind_Draw;
				}
				else
				{
					ScoreKind=enScoreKind_Lost;
				}
				
				m_pITableFrame->WriteUserScore(i,lScore,lRevenue,ScoreKind);
			}

			m_pITableFrame->ConcludeGame();

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_OVER,&gameEnd,sizeof(gameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_OVER,&gameEnd,sizeof(gameEnd));
			
			BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
			return true;
		}
	default:
		{
			return false;
		}
	}
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

// 计算棋子数量
int CTableFrameSink::AccountChessCount(BYTE chess[CHESSCOUNT_H][CHESSCOUNT_W])
{
	int count=0;
	for(int j=0; j<CHESSCOUNT_W; ++j)
	for(int i=0; i<CHESSCOUNT_H; ++i)
	{
		if(chess[i][j] != ERROR_ALL)
		{
			++count;
		}
	}
	return count;

}


/*static void RetailOutput(TCHAR *szFormat, ...)
{
    TCHAR szBuffer[1024];  // Large buffer for long filenames or URLs
    const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
    const int LASTCHAR = NUMCHARS - 1;

    // Format the input string
    va_list pArgs;
    va_start(pArgs, szFormat);

    // Use a bounded buffer size to prevent buffer overruns.  Limit count to
    // character size minus one to allow for a NULL terminating character.
    _vsntprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
    va_end(pArgs);

    // Ensure that the formatted string is NULL-terminated
    szBuffer[LASTCHAR] = TEXT('\0');

    OutputDebugString(szBuffer);
}*/

//////////////////////////////////////////////////////////////////////////
