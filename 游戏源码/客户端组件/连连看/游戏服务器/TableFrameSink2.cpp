#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	m_bIsGameEnd=false;
	InitVariable();
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
	m_pITableFrame=GET_OBJECTPTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//获取参数
	const tagGameServiceOption  *pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(pGameServiceOption!=NULL);

	//设置参数
	m_pITableFrame->SetLessTimes(252L);

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
	memset(m_ScoreInfo,0,sizeof(m_ScoreInfo));
	
	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_bPropCnt[i][0]=3;
		m_bPropCnt[i][1]=3;
		m_bPropCnt[i][2]=0;
		m_bPropCnt[i][3]=0;
		m_bPropCnt[i][4]=0;
	}
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

	srand((UINT)time(NULL));
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
	if(m_bPlayStatus[wChairID] !=GS_FREE)
	{
		BYTE bPlayCnt =0, OtherPlayers[GAME_PLAYER]; 
		int maxSeriesLinkCnt = -1;
		memset(OtherPlayers, 255, sizeof(OtherPlayers));
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(i!=wChairID && m_bPlayStatus[i] ==GS_PLAYING)
			{
				bPlayCnt++;
				OtherPlayers[i] = i;
				if(m_bSeriesLinkCnt[i] > maxSeriesLinkCnt){
					maxSeriesLinkCnt = m_bSeriesLinkCnt[i];
				}
				//RetailOutput("\nm_bPlayStatus[%d] = %d;",i, m_bPlayStatus[i]);
			}
		}
		int nBaseScore= 2 * (maxSeriesLinkCnt /10+1) * 3;
		m_ScoreInfo[wChairID].ScoreKind =enScoreKind_Flee;
		m_ScoreInfo[wChairID].lScore += -bPlayCnt*nBaseScore;
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(OtherPlayers[i]!=255){
				m_ScoreInfo[i].lScore += nBaseScore;
			}
			//RetailOutput("\nOnEventGameEnd m_ScoreInfo[%d].lScore = %d;",i, m_ScoreInfo[i].lScore);
		}
		//RetailOutput("\nOnEventGameEnd bPlayCnt = %d;",bPlayCnt);
		//RetailOutput("\nOnEventGameEnd maxSeriesLinkCnt = %d;",maxSeriesLinkCnt);
		//RetailOutput("\nOnEventGameEnd nBaseScore = %d;",nBaseScore);


		if(bPlayCnt>1)m_pITableFrame->WriteUserScore(wChairID,m_ScoreInfo[wChairID]);
		return SubGameLost(wChairID);
	}
	return true;
}

bool CTableFrameSink::SubGameLost(WORD wSeat)
{
	CMD_S_Lost  LostSeat;
	LostSeat.wSeat =wSeat;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_LOST,&LostSeat,sizeof(LostSeat));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LOST,&LostSeat,sizeof(LostSeat));

	BYTE bPlayCnt =0;
	WORD seat=0;
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
	} else m_bPlayStatus[wSeat] =GS_FREE;
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
			memcpy(GameStatus.bPlayState,m_bPlayStatus,sizeof(m_bPlayStatus));;
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
			if(wDataSize != sizeof(CMD_C_StopIt)) return false;
			CMD_C_StopIt *pCnt = (CMD_C_StopIt *)pDataBuffer;
			WORD wSeat = pIServerUserItem->GetChairID();
			CMD_S_StopIt Stop;
			Stop.nSrcUserId =(BYTE)wSeat;
			Stop.nDesUserId =pCnt->nDesUserId;
			//禁手道具
			m_bPropCnt[wSeat][PROP_STOPIT]--;
			Stop.nPropCount = m_bPropCnt[wSeat][PROP_STOPIT];

			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_STOPIT,&Stop,sizeof(Stop));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STOPIT,&Stop,sizeof(Stop));
			return true;
		}
	case SUB_C_CLOG:
		{
			if(wDataSize != sizeof(CMD_C_Clog)) return false;
			CMD_C_Clog *pCnt = (CMD_C_Clog *)pDataBuffer;
			WORD wSeat = pIServerUserItem->GetChairID();
			CMD_S_Clog  Clog;
			memset(&Clog,ERROR_ALL,sizeof(Clog));
			Clog.nDesUserId = pCnt->nDesUserId;
			Clog.nSrcUserId = (BYTE)wSeat;
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
				
				int randMaxNum=(int)Map.GetCount() >=12?12:(int)Map.GetCount();
				//取偶数
				randMaxNum = randMaxNum%2?randMaxNum-1:randMaxNum;
				srand((unsigned int)time(NULL));
				int pos=0;
				while(randMaxNum>0)
				{
					BYTE chessValue =RandNumber(0,MAX_CHESS_VALUE);
					int randNum = rand()%(int)Map.GetCount();
					Clog.nChessPosX[pos] =(BYTE)Map.GetAt(randNum).y;
					Clog.nChessPosY[pos] =(BYTE)Map.GetAt(randNum).x;
					Clog.nChessFace[pos]=chessValue;
					pos++;
					Map.RemoveAt(randNum);
					randMaxNum--;
					randNum = rand()%(int)Map.GetCount();
					Clog.nChessPosX[pos] =(BYTE)Map.GetAt(randNum).y;
					Clog.nChessPosY[pos] =(BYTE)Map.GetAt(randNum).x;
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

			return true;
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
				return SubGameLost(wSeat);
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
				LinkPos.nCurrentPlayer = (BYTE)wSeat;
				if(bValue >=PROP_MIN_VALUE)
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
			sRealign.nCurrentPlayer = (BYTE)wSeat;
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
			//int nScore[GAME_PLAYER]={0};
			int nBaseScore= 2 * (m_bSeriesLinkCnt[wSeat] /10+1);


			BYTE Losers[GAME_PLAYER], iOrder, iRepeatTimes = 1, iRepeatOrder = 0;
			memset(Losers, 255, sizeof(Losers));
			//WORD wMaxSeat=INVALID_CHAIR,wMaxSeat2=INVALID_CHAIR;
			CMD_S_GameOver  GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			int iPlayerCnt=0;
			//求最大的2个
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(i !=wSeat && m_pITableFrame->GetServerUserItem(i))
				{
					if(m_ScoreInfo[i].ScoreKind !=enScoreKind_Flee)iPlayerCnt++;
					Losers[i] = i;
				}
				//RetailOutput("\n1  Losers[%d] = %d;",i, Losers[i]);
			}
			iPlayerCnt++;
			//RetailOutput("\niPlayerCnt = %d;",iPlayerCnt);

			for(int i = 0; i< GAME_PLAYER; i++){
				if(Losers[i] != 255)
				{
					iOrder =2;
					for( int j= 0; j <GAME_PLAYER; j++){
						if(m_bSeriesLinkCnt[i] < m_bSeriesLinkCnt[j] && Losers[j] != 255){
							iOrder ++;
						}
					}
					GameEnd.bOrder[i] = iOrder;

				}

			}
			if (iPlayerCnt > 1){
				for(int i = 0; i< GAME_PLAYER; i++){
					for(int j= i+1; j< GAME_PLAYER; j++){
						if( Losers[j] != 255 && GameEnd.bOrder[i] == GameEnd.bOrder[j]){
							iRepeatTimes++;
							iRepeatOrder = GameEnd.bOrder[j];
						}
					}
				}
				if(iRepeatTimes >2) iRepeatTimes -=1;
				
				//RetailOutput("\nnBaseScore = %d;",nBaseScore);
				//RetailOutput("\niRepeatTimes = %d;",iRepeatTimes);
				//RetailOutput("\niRepeatOrder = %d;",iRepeatOrder);
				for(int i=0;i<GAME_PLAYER;i++)
				{

					if(i==wSeat)
					{
						if(iPlayerCnt!=1)
						{
							GameEnd.nScores[i] =nBaseScore * (iPlayerCnt == 2?1:(iPlayerCnt == 3?3:6));
						}

						GameEnd.bOrder[i]=1;
					}
					else if(Losers[i] != 255){
						if(GameEnd.bOrder[i] ==2)
						{
							if(iRepeatOrder != 2)
								GameEnd.nScores[i] =-nBaseScore;
							else GameEnd.nScores[i] =-nBaseScore * (iRepeatTimes == 3?2:1.5);
						}
						else if(GameEnd.bOrder[i] ==3)
						{
							if(iRepeatOrder != 3)
								GameEnd.nScores[i] =-nBaseScore * 2;
							else GameEnd.nScores[i] =-nBaseScore * 2.5;

						}
						else if(GameEnd.bOrder[i] ==4)
						{
							GameEnd.nScores[i] =-nBaseScore * 3;

						}
					}
					m_ScoreInfo[i].lScore +=  GameEnd.nScores[i];
					GameEnd.nScores[i]=m_ScoreInfo[i].lScore;
					//RetailOutput("\nGameEnd.nScores[%d] = %d;",i, GameEnd.nScores[i]);

				}
			} else {
				for(int i = 0; i< GAME_PLAYER; i++){
					GameEnd.nScores[i]=m_ScoreInfo[i].lScore;
				}
				GameEnd.bOrder[wSeat]=1;
			}
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.nScores[i]=m_pITableFrame->AccountGameGold(GameEnd.nScores[i]);
			}

		//	memcpy(GameEnd.nScores,nScore,sizeof(nScore));
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_OVER,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_OVER,&GameEnd,sizeof(GameEnd));

			//写分
			for(int i=0;i<GAME_PLAYER;i++)
			{
				//m_ScoreInfo[i].lScore+=nScore[i];	
				if(m_ScoreInfo[i].lScore>0)
				{
					m_ScoreInfo[i].ScoreKind=enScoreKind_Win;
				}
				else if(m_ScoreInfo[i].lScore==0)
				{
					m_ScoreInfo[i].ScoreKind=enScoreKind_Draw;
				}
				else
				{
					m_ScoreInfo[i].ScoreKind=enScoreKind_Lost;
				}
			}
			m_pITableFrame->WriteTableScore(m_ScoreInfo, 				GAME_PLAYER,				0);

			m_pITableFrame->ConcludeGame();
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
};*/

//////////////////////////////////////////////////////////////////////////
