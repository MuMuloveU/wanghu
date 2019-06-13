#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_Symmetry;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_Chess , sizeof(m_Chess)) ;
	ZeroMemory(m_bFinishPlacement , sizeof(m_bFinishPlacement)) ;
	ZeroMemory(m_bActiveUser , sizeof(m_bActiveUser)) ;

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_lCurrentStepCount=0;

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
//	QUERYINTERFACE(ITableFrameEvent,Guid,dwQueryVer);
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
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_Chess , sizeof(m_Chess)) ;
	ZeroMemory(m_bFinishPlacement , sizeof(m_bFinishPlacement)) ;
	ZeroMemory(m_bActiveUser , sizeof(m_bActiveUser)) ;

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
	//保存布局
	BYTE bServerX = 255 , 
		bServerY = 255 ;

	for(BYTE x=0 ; x<5 ; ++x)
		for(BYTE y=0 ; y<6 ; ++y)
		{
			//实际坐标
			BYTE bRealClientX = x + 6 ,
				bRealClientY = y + 11 ;
			ClientToServer(wChairID+1 , bRealClientX , bRealClientY , bServerX , bServerY) ;
			//数据校验
			ASSERT(255!=bServerX && 255!=bServerY) ;
			if(255==bServerX || 255==bServerY) return false ;

			m_Chess[bServerX][bServerY].bChessID = CHESS_NULL ;
			m_Chess[bServerX][bServerY].bColor   = CHESS_COLOR_NO ;
			m_Chess[bServerX][bServerY].bChessMode = CHESS_NO ;
		}

		//设置变量
		m_bFinishPlacement[wChairID] = false ;
		if(wChairID==m_wCurrentUser)
		{
			m_wCurrentUser = INVALID_CHAIR ;
			for(WORD wID = 0 ; wID<GAME_PLAYER ; ++wID)
			{
				if(true==m_bFinishPlacement[wID])
					m_wCurrentUser = wID ;
			}
		}
	CMD_S_UserUp UserUp ;
	memset(&UserUp , 0 , sizeof(UserUp)) ;
	UserUp.wUser = wChairID ;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_UP,&UserUp,sizeof(UserUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_UP,&UserUp,sizeof(UserUp));
	return true ;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//初始记录
	m_HistoryRecord.Init() ;

	for(WORD wChairID=0 ; wChairID<GAME_PLAYER ; ++wChairID)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);
		if(NULL!=pIServerUserItem)
		{
			m_bActiveUser[wChairID] = true ;
			if(INVALID_CHAIR==m_wCurrentUser)
				m_wCurrentUser = wChairID ;
		}
		else 
		{
			m_bActiveUser[wChairID] = false ;
			
			//布局后退出
			if(true==m_bFinishPlacement[wChairID])
			{
				BYTE bServerX , bServerY ; 
				m_bFinishPlacement[wChairID] = false ;
				for(BYTE x=6 ; x<=10 ; ++x)
					for(BYTE y=11 ; y<=16 ; ++y)
					{
						ClientToServer(wChairID+1 , x , y , bServerX , bServerY) ;
						m_Chess[bServerX][bServerY].bChessID  = CHESS_NULL ;
						m_Chess[bServerX][bServerY].bColor	  = CHESS_COLOR_NO ;
						m_Chess[bServerX][bServerY].bChessMode=CHESS_NO ;
					}
				if(wChairID == m_wCurrentUser)
				{
					m_wCurrentUser = INVALID_CHAIR ;

					//重新判断
					wChairID = 0 ;
				}
			}
		}
	}

	CMD_S_GameStart GameStart ;
	ZeroMemory(&GameStart , sizeof(GameStart)) ;
	GameStart.wCurrentUser = m_wCurrentUser ;

	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

	//保存布局
	CopyMemory(m_HistoryRecord.m_Chess , m_Chess , sizeof(m_Chess)) ;

	m_lCurrentStepCount=0;

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	m_wCurrentUser=INVALID_CHAIR ;
	ZeroMemory(m_bFinishPlacement , sizeof(m_bFinishPlacement)) ;
	ZeroMemory(m_bActiveUser , sizeof(m_bActiveUser)) ;

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

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame();
			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));
			tagScoreInfo ScoreInfo[m_wPlayerCount];
			memset(&ScoreInfo,0,sizeof(ScoreInfo));

			BYTE bPlayerCount=0 ;

			for(WORD wID=0 ; wID<GAME_PLAYER ; ++wID)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(wID);
				if(NULL!=pIServerUserItem) ++bPlayerCount ;
			}

			//不是和棋
			if(INVALID_CHAIR!=wChairID)
			{
				if(2==bPlayerCount)
				{
					GameEnd.lUserScore[wChairID]	= -1 ;
					GameEnd.lUserScore[(wChairID+2)%GAME_PLAYER]  = 1 ; 
				}
				else if(4==bPlayerCount)
				{
					GameEnd.lUserScore[wChairID]	= -1 ;
					GameEnd.lUserScore[(wChairID+2)%GAME_PLAYER]  = -1 ;
					GameEnd.lUserScore[(wChairID+1)%GAME_PLAYER]  = 1 ;
					GameEnd.lUserScore[(wChairID+3)%GAME_PLAYER]  = 1 ;
				}
			}

			//修改积分
			
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				ScoreInfo[i].lScore=GameEnd.lUserScore[i];
				ScoreInfo[i].ScoreKind=(GameEnd.lUserScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
				m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore,ScoreInfo[i].lRevenue,ScoreInfo[i].ScoreKind);
			}
			//m_pITableFrame->WriteTableScore(ScoreInfo,m_wPlayerCount,0);

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame();
			
			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			GameEnd.lUserScore[wChairID] = -1 ;

			//修改积分
			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
			ScoreInfo.ScoreKind=enScoreKind_Draw;
			ScoreInfo.lScore=GameEnd.lUserScore[wChairID];
			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo.lScore,ScoreInfo.lRevenue,ScoreInfo.ScoreKind);

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));


			//结束游戏
			m_pITableFrame->ConcludeGame();

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
			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem , NULL , 0) ;

	case GS_PLAYING:		//游戏状态
		{
			CMD_S_StatusPlay StatusPlay ;
			memset(&StatusPlay , 0 , sizeof(StatusPlay)) ;
			CopyMemory(StatusPlay.Chess , m_HistoryRecord.m_Chess , sizeof(m_HistoryRecord.m_Chess)) ;
			StatusPlay.wCurrentUser = m_wCurrentUser ;
			//发送布局
			m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_PLACEMENT,&StatusPlay,sizeof(CMD_S_StatusPlay));

				//发送棋谱
			WORD wBufferSize=0;
			BYTE cbBuffer[SOCKET_PACKET];
			SaveChessStruct SaveChess ;
			if(true==m_HistoryRecord.GetFirstFromList(SaveChess))
			{
				for (INT_PTR i=0 ; i<m_HistoryRecord.GetStepCount() ; i++)
				{
					CopyMemory(cbBuffer+wBufferSize , &SaveChess , sizeof(SaveChessStruct));
					wBufferSize+=sizeof(SaveChessStruct);

					if ((wBufferSize+sizeof(SaveChessStruct))>sizeof(cbBuffer))
					{
						m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_CHESS_MANUAL,cbBuffer,wBufferSize);
						wBufferSize=0;
					}
					if(false==m_HistoryRecord.GetNextFromList(SaveChess))
						break;
				}
				if (wBufferSize>0) m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_CHESS_MANUAL,cbBuffer,wBufferSize);
			}
			//发送场景
			memset(&StatusPlay , 0 , sizeof(StatusPlay)) ;
            
			StatusPlay.lStepCount = m_lCurrentStepCount ;
			StatusPlay.wCurrentUser = m_wCurrentUser ;
			CopyMemory(StatusPlay.Chess , m_Chess , sizeof(m_Chess)) ;

			
			return m_pITableFrame->SendGameScene(pIServerUserItem , &StatusPlay , sizeof(CMD_S_StatusPlay)) ;
			
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
	case SUB_C_FINISH_PLACEMENT :	//完成布局
		{
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			return FinishiPlacement(pUserData->wChairID , pDataBuffer, wDataSize) ;
		}

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
			return OnUserMoveChess(pUserData->wChairID,pMoveChess->bXSourcePos,pMoveChess->bYSourcePos,pMoveChess->bXTargetPos,pMoveChess->bYTargetPos);
		}
	case SUB_C_GAME_TIME_OUT:	//玩家超时
		{
			CMD_S_GameTimeOut GameTimeOut ;
			memset(&GameTimeOut , 0 , sizeof(GameTimeOut)) ;

			//更改变量
			for(BYTE i=0 ; i<GAME_PLAYER ; ++i)
			{
				m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER ;

				if(false==m_bActiveUser[m_wCurrentUser])
					continue ;
				else
					break ; 
			}

			GameTimeOut.wCurrentUser = m_wCurrentUser ;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_TIME_OUT,&GameTimeOut,sizeof(GameTimeOut));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_TIME_OUT,&GameTimeOut,sizeof(GameTimeOut));
			return true ;
		}
	case SUB_C_USER_SURRENDER:	//玩家认输
		{
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();

			CMD_S_OneLose OneLose ;
			memset(&OneLose , 0 , sizeof(OneLose)) ;

			OneLose.wLoseUser = pUserData->wChairID ;
			m_bActiveUser[OneLose.wLoseUser] = false ;

			//更改变量
			for(BYTE i=0 ; i<GAME_PLAYER ; ++i)
			{
				m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER ;

				if(false==m_bActiveUser[m_wCurrentUser])
					continue ;
				else
					break ; 
			}


			BYTE bChessColor = OneLose.wLoseUser+1 ;
			for(BYTE x=0 ; x<17 ; ++x)
				for(BYTE y=0 ; y<17 ; ++y)
					if(bChessColor==m_Chess[x][y].bColor)
					{
						m_Chess[x][y].bChessID	 = CHESS_NULL ;
						m_Chess[x][y].bChessMode = CHESS_NO ;
						m_Chess[x][y].bColor	 = CHESS_COLOR_NO ;
					}
					OneLose.wCurrentUser = m_wCurrentUser ;
					m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ONE_LOSE,&OneLose,sizeof(OneLose));
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ONE_LOSE,&OneLose,sizeof(OneLose));

					//判断结束
					BYTE bActiveCount=0 , 
						bPalyerCount=0 ; 
					for(BYTE count=0 ; count<GAME_PLAYER ; ++count)
					{
						if(true==m_bActiveUser[count]) ++bActiveCount ;
						IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(count);
						if(NULL!=pIServerUserItem)	++bPalyerCount ;
					}
					if(1==bActiveCount)
					{
						OnEventGameEnd(OneLose.wLoseUser , NULL , GER_NORMAL) ;
					}
					else if(2==bActiveCount && 4==bPalyerCount)
					{
						BYTE bActiveUser1=255 , bActiveUser2=255 ;
						for(BYTE count=0 ; count<GAME_PLAYER ; ++count)
						{
							if(true==m_bActiveUser[count] && 255==bActiveUser1)
								bActiveUser1 = count ;
							else if(true==m_bActiveUser[count] && 255==bActiveUser2 && 255!=bActiveUser1)
								bActiveUser2 = count ;
						}
						if(1!=abs(bActiveUser1-bActiveUser2)) OnEventGameEnd(OneLose.wLoseUser , NULL , GER_NORMAL) ;
					}

					return true ;
		}
	case SUB_C_REQUEST_PEACE:	//请求和棋
		{
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			
			CMD_S_RuquestPeace RuquestPeace  ;
			memset(&RuquestPeace , 0 , sizeof(RuquestPeace )) ;
			RuquestPeace.wRequestUser = pUserData->wChairID ;
			m_wRequestPeaceUser = pUserData->wChairID ;
			
			for(WORD wID=0 ; wID<GAME_PLAYER ; ++wID)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(wID);
				if(NULL!=pIServerUserItem && true==m_bActiveUser[wID] && pUserData->wChairID!=wID)
				{
                    m_pITableFrame->SendTableData(wID , SUB_S_REQUEST_PEACE , &RuquestPeace , sizeof(RuquestPeace));
				}
			}

			return true ;
		}
	case SUB_C_PEACE_ANSWER:	//和棋应答
		{
			CMD_C_PeaceAnswer *pPeaceAnswer = (CMD_C_PeaceAnswer *)pDataBuffer ;
			
			//同意次数
			static BYTE bAgreeCount = 0 ;

			//应答次数
			static BYTE bAnswerCount=0 ;
			++bAnswerCount ;
			if(true==pPeaceAnswer->bAgree)	bAgreeCount++ ;

			BYTE bActiveCount=0 ;
			for(BYTE count=0 ; count<GAME_PLAYER ; ++count)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(count);
				if(NULL!=pIServerUserItem && true==m_bActiveUser[count])	 ++bActiveCount ;
			}
			if(bAgreeCount==(bActiveCount-1)) OnEventGameEnd(INVALID_CHAIR , NULL , GER_NORMAL) ;

			//恢复变量 
			if(bAnswerCount==(bActiveCount-1)) 
			{
				bAnswerCount = 0 ;
				bAgreeCount  = 0 ;

				m_pITableFrame->SendTableData(m_wRequestPeaceUser , SUB_S_PEACE_FAIL , NULL , 0) ;
			}

			return true ;
		}
	
	}

	return false;
}

//消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_GF_USER_READY:		//用户同意
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus==US_LOOKON) return false;

		
			return false;
		}
	case SUB_GF_USER_CHAT:	//用户聊天
		{
				return false;
		}

			return false;
		
	}

	return false;
}

//移动棋子
bool CTableFrameSink::OnUserMoveChess(WORD wChairID, BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//游戏状态
	if (m_pITableFrame->GetGameStatus()!=GS_PLAYING) return true;

	//坐标转换
    BYTE bServerSXPos = 255 ,
		 bServerSYPos = 255 ,
		 bServerTXPos = 255 , 
		 bServerTYPos = 255 ;

	ClientToServer(wChairID+1 , cbXSourcePos , cbYSourcePos , bServerSXPos , bServerSYPos) ;
	ClientToServer(wChairID+1 , cbXTargetPos , cbYTargetPos , bServerTXPos , bServerTYPos) ;

	CMD_S_OneLose OneLose ;
	memset(&OneLose , 0 , sizeof(OneLose)) ;

	//比较棋子
	enCompareResult CompareResult = m_GameLogic.CompareChess(m_Chess , bServerSXPos , bServerSYPos , bServerTXPos , bServerTYPos) ;
	
	//目标玩家
	WORD wTargetUser = INVALID_CHAIR ;
	if(CHESS_NULL!=m_Chess[bServerTXPos][bServerTYPos].bChessID)
        wTargetUser = m_Chess[bServerTXPos][bServerTYPos].bColor-1 ;
	
	if(enError!=CompareResult && CHESS_JUN_QI==m_Chess[bServerTXPos][bServerTYPos].bChessID && 
		true==m_GameLogic.CanCarry(m_Chess[bServerSXPos][bServerSYPos].bChessID , m_Chess[bServerSXPos][bServerSYPos].bColor , m_Chess))
	{
		OneLose.wLoseUser = wTargetUser ;
		m_bActiveUser[wTargetUser] = false ;
	}

	switch(CompareResult)
	{
	case enSmall:
		m_Chess[bServerSXPos][bServerSYPos].bChessID   = CHESS_NULL ;
		m_Chess[bServerSXPos][bServerSYPos].bColor	   = CHESS_COLOR_NO ;
		m_Chess[bServerSXPos][bServerSYPos].bChessMode = CHESS_NO ;
		break;

	case enEqual:
		m_Chess[bServerSXPos][bServerSYPos].bChessID   = CHESS_NULL ;
		m_Chess[bServerSXPos][bServerSYPos].bColor     = CHESS_COLOR_NO ;
		m_Chess[bServerSXPos][bServerSYPos].bChessMode = CHESS_NO ;

		m_Chess[bServerTXPos][bServerTYPos].bChessID   = CHESS_NULL ;
		m_Chess[bServerTXPos][bServerTYPos].bColor     = CHESS_COLOR_NO ;
		m_Chess[bServerTXPos][bServerTYPos].bChessMode = CHESS_NO ;
		break;

	case enLarge:
		m_Chess[bServerTXPos][bServerTYPos].bChessID   = m_Chess[bServerSXPos][bServerSYPos].bChessID ; 
		m_Chess[bServerTXPos][bServerTYPos].bColor     = m_Chess[bServerSXPos][bServerSYPos].bColor ;   
		m_Chess[bServerTXPos][bServerTYPos].bChessMode = m_Chess[bServerSXPos][bServerSYPos].bChessMode;

		m_Chess[bServerSXPos][bServerSYPos].bChessID   = CHESS_NULL ;
		m_Chess[bServerSXPos][bServerSYPos].bColor	   = CHESS_COLOR_NO ;
		m_Chess[bServerSXPos][bServerSYPos].bChessMode = CHESS_NO ;
		break;

	case enError:
#ifdef _DEBUG
		AfxMessageBox("服务器：OnUserMoveChess:错误棋子！") ;
#endif		
		return false ;

	default:
		return false ;
	}

	if(true==m_GameLogic.NoChess(wTargetUser+1 , m_Chess))
	{
		OneLose.wLoseUser = wTargetUser ;
		m_bActiveUser[wTargetUser] = false ;
	}
	else if(true==m_GameLogic.NoChess(wChairID+1 , m_Chess))
	{
		OneLose.wLoseUser = wChairID ;
		m_bActiveUser[wChairID] = false ;
	}

	//更改变量
	for(BYTE i=0 ; i<GAME_PLAYER ; ++i)
	{
		m_wCurrentUser = (m_wCurrentUser+1)%GAME_PLAYER ;

		if(false==m_bActiveUser[m_wCurrentUser])
			continue ;
		else
			break ; 
	}

	//发送数据
    CMD_S_MoveChess	SendMoveChess ;
	memset(&SendMoveChess , 0 , sizeof(SendMoveChess)) ;
	SendMoveChess.bXSourcePos  = bServerSXPos ;
	SendMoveChess.bYSourcePos  = bServerSYPos ;
	SendMoveChess.bXTargetPos  = bServerTXPos ;
	SendMoveChess.bYTargetPos  = bServerTYPos ;
	SendMoveChess.bSourceColor = m_Chess[bServerSXPos][bServerSYPos].bColor ;
	SendMoveChess.bTargetColor = m_Chess[bServerTXPos][bServerTYPos].bColor ;
	SendMoveChess.wCurrentUser = m_wCurrentUser ;
	SendMoveChess.wMoveUser	   = wChairID ;

	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_MOVE_CHESS,&SendMoveChess,sizeof(SendMoveChess));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_MOVE_CHESS,&SendMoveChess,sizeof(SendMoveChess));

	//记录历史

	SaveChessStruct ChessStruct ;
	ChessStruct.bSourceX = bServerSXPos ;
	ChessStruct.bSourceY = bServerSYPos ;
	ChessStruct.bTargetX = bServerTXPos ;
	ChessStruct.bTargetY = bServerTYPos ;
	m_HistoryRecord.AddToList(ChessStruct) ;

	//玩家没棋
	if(INVALID_CHAIR!=OneLose.wLoseUser && false==m_bActiveUser[OneLose.wLoseUser])
	{
		BYTE bChessColor = OneLose.wLoseUser+1 ;
		for(BYTE x=0 ; x<17 ; ++x)
			for(BYTE y=0 ; y<17 ; ++y)
				if(bChessColor==m_Chess[x][y].bColor)
				{
					m_Chess[x][y].bChessID	 = CHESS_NULL ;
					m_Chess[x][y].bChessMode = CHESS_NO ;
					m_Chess[x][y].bColor	 = CHESS_COLOR_NO ;
				}
        OneLose.wCurrentUser = m_wCurrentUser ;
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ONE_LOSE,&OneLose,sizeof(OneLose));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ONE_LOSE,&OneLose,sizeof(OneLose));

		//判断结束
		BYTE bActiveCount=0 , 
			 bPalyerCount=0 ; 
		for(BYTE count=0 ; count<GAME_PLAYER ; ++count)
		{
			if(true==m_bActiveUser[count]) ++bActiveCount ;
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(count);
			if(NULL!=pIServerUserItem)	++bPalyerCount ;
		}
		if(1==bActiveCount)
		{
            OnEventGameEnd(OneLose.wLoseUser , NULL , GER_NORMAL) ;
		}
		else if(2==bActiveCount && 4==bPalyerCount)
		{
			BYTE bActiveUser1=255 , bActiveUser2=255 ;
            for(BYTE count=0 ; count<GAME_PLAYER ; ++count)
			{
				if(true==m_bActiveUser[count] && 255==bActiveUser1)
					bActiveUser1 = count ;
				else if(true==m_bActiveUser[count] && 255==bActiveUser2 && 255!=bActiveUser1)
					bActiveUser2 = count ;
			}
			if(1!=abs(bActiveUser1-bActiveUser2)) OnEventGameEnd(OneLose.wLoseUser , NULL , GER_NORMAL) ;
		}
	}

	//递增步数
	++m_lCurrentStepCount ;


	return true;
}



//完成布局
bool CTableFrameSink::FinishiPlacement(WORD wChairID , const void * pDataBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_C_FinishPlacement));
	if (wDataSize!=sizeof(CMD_C_FinishPlacement)) return false;

	CMD_C_FinishPlacement *pFinishPlacement=(CMD_C_FinishPlacement*)pDataBuffer ;

	//保存布局
	BYTE bServerX = 255 , 
		 bServerY = 255 ;

	for(BYTE x=0 ; x<5 ; ++x)
		for(BYTE y=0 ; y<6 ; ++y)
		{
			//实际坐标
			BYTE bRealClientX = x + 6 ,
				bRealClientY = y + 11 ;
			ClientToServer(pFinishPlacement->bColor , bRealClientX , bRealClientY , bServerX , bServerY) ;
			//数据校验
			ASSERT(255!=bServerX && 255!=bServerY) ;
			if(255==bServerX || 255==bServerY) return false ;

			m_Chess[bServerX][bServerY].bChessID = pFinishPlacement->bChessID[x][y] ;
			m_Chess[bServerX][bServerY].bColor = pFinishPlacement->bColor ;
		}

	//设置变量
	m_bFinishPlacement[wChairID] = true ;
	if(INVALID_CHAIR==m_wCurrentUser) m_wCurrentUser = wChairID ;

	//发送数据
	CMD_S_FinishPlacement FinishPlacement;
	ZeroMemory(&FinishPlacement , sizeof(FinishPlacement)) ;

	for(WORD wID=0 ; wID<GAME_PLAYER ; ++wID)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(wID);
		if(pIServerUserItem && true==m_bFinishPlacement[wID])
		{
			FinishPlacement.wCurrentUser = wID ;
			FinishPlacement.bColor = wID+1 ;
			GetUserChess(FinishPlacement.bChessID , wID) ;

			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_FINISH_PLACEMENT,&FinishPlacement,sizeof(FinishPlacement));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_FINISH_PLACEMENT,&FinishPlacement,sizeof(FinishPlacement));

		}
	}
	return true ;
}

//坐标转换
void CTableFrameSink::ClientToServer(const BYTE bChessColor , const BYTE bRealClientX , const BYTE bRealClientY , BYTE &bServerX , BYTE &bServerY) 
{
	switch(bChessColor) 
	{
	case CHESS_COLOR_GREEN:	//绿色棋子
		bServerX = 16-bRealClientX ;
		bServerY = 16-bRealClientY ;
		break;

	case CHESS_COLOR_BLACK:	//黑色棋子
		bServerX = bRealClientY ;
		bServerY = 16-bRealClientX ;
		break;

	case CHESS_COLOR_BLUE:	//蓝色棋子
		bServerX = bRealClientX ;
		bServerY = bRealClientY ;
		break;

	case CHESS_COLOR_RED:	//红色棋子
		bServerX = 16-bRealClientY ;
		bServerY = bRealClientX ;
		break;

    default:
#ifdef	_DEBUG
		AfxMessageBox("ClientToServer:错误棋子") ;
#endif		
		bServerX = 255 ;
		bServerY = 255 ;
	}
}

//获取棋子
void CTableFrameSink::GetUserChess(BYTE bChess[5][6] , WORD wUser)
{
	switch(wUser)
	{
	case 0:		//绿色棋子
		for(BYTE x=0 ; x<5 ; ++x)
			for(BYTE y=0 ; y<6 ; ++y)
			{
				bChess[x][y] = m_Chess[10-x][5-y].bChessID ;
				TRACE("0: %d\n" , bChess[x][y]) ;
			}
		break;

	case 1:		//黑色棋子
		for(BYTE x=0 ; x<5 ; ++x)
			for(BYTE y=0 ; y<6 ; ++y)
			{
				bChess[x][y] = m_Chess[11+y][10-x].bChessID ;
				TRACE("1: %d\n" , bChess[x][y]) ;
			}
		break;

	case 2:		//蓝色棋子	
		for(BYTE x=0 ; x<5 ; ++x)
			for(BYTE y=0 ; y<6 ; ++y)
			{
				bChess[x][y] = m_Chess[6+x][11+y].bChessID ;
				TRACE("2: %d\n" , bChess[x][y]) ;
			}
		break; 

	case 3:		//红色棋子
		for(BYTE x=0 ; x<5 ; ++x)
			for(BYTE y=0 ; y<6 ; ++y)
			{
				bChess[x][y] = m_Chess[5-y][6+x].bChessID ;
				TRACE("3: %d\n" , bChess[x][y]) ;
			}
		break;

	default:
		ZeroMemory(bChess , sizeof(BYTE)*5*6) ;
#ifdef _DEBUG
		AfxMessageBox("GetUserChess:错误棋子") ;
#endif		
	}
}
//////////////////////////////////////////////////////////////////////////
