#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

#define IDI_GAME_END				1										//结束标识
#define IDI_PASS_CARD				2										//放弃标识

//////////////////////////////////////////////////////////////////////////

//静态变量
__int64				CTableFrameSink::g_lMaxScore=100000000L;				//最大变化
__int64				CTableFrameSink::g_lMaxWinScore=100000000L;				//最大赢分
__int64				CTableFrameSink::g_lMaxLoseScore=-100000000L;			//最大输分
CBlacklistIDMap		CTableFrameSink::g_BlacklistIDMap;						//限制用户
CScoreHistoryMap	CTableFrameSink::g_ScoreHistoryMap;						//历史记录

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_AllReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wOperaCount=0;
	m_bShowHand=false;
	m_bChaosCard=true;
	m_wCurrentUser=INVALID_CHAIR;
	m_wLastMostUser = INVALID_CHAIR;

	//用户状态
	m_lServiceScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbOperaScore,sizeof(m_cbOperaScore));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));

	//扑克变量
	m_cbSendCardCount=0;
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	m_lMaxScore=0L;
	m_lCellScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;

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
	//游戏变量
	m_wOperaCount=0;
	m_bShowHand=false;
	m_bChaosCard=true;
	m_wCurrentUser=INVALID_CHAIR;
	m_wLastMostUser = INVALID_CHAIR;

	//用户状态
	m_lServiceScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbOperaScore,sizeof(m_cbOperaScore));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));

	//扑克变量
	m_cbSendCardCount=0;
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	m_lMaxScore=0L;
	m_lCellScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;

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
	ASSERT(wChairID<m_wPlayerCount);
	return (m_cbPlayStatus[wChairID]==TRUE)?true:false;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);

	//服务费用
	LONG lBaseScore=0L;

	//服务费用
	//for (WORD i=0;i<m_wPlayerCount;i++)
	//{
	//	//获取用户
	//	IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
	//	if (pIServerUserItem==NULL) continue;

	//	const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();
	//	if (lBaseScore==0L)
	//	{
	//		lBaseScore=pUserScore->lScore;
	//	}
	//	else lBaseScore=__min(pUserScore->lScore,lBaseScore);
	//}

	////扣除费用
	//if ((lBaseScore>=20L)&&(lBaseScore<10000L))
	//{
	//	m_lServiceScore=10L;
	//}
	//else if ((lBaseScore>=10000L)&&(lBaseScore<100000L))
	//{
	//	m_lServiceScore=30L;
	//}
	//else if ((lBaseScore>=100000L)&&(lBaseScore<300000L))
	//{
	//	m_lServiceScore=50L;
	//}
	//else if ((lBaseScore>=300000L)&&(lBaseScore<600000L))
	//{
	//	m_lServiceScore=80L;
	//}
	//else if ((lBaseScore>=600000L)&&(lBaseScore<1000000L))
	//{
	//	m_lServiceScore=120L;
	//}
	//else if (lBaseScore>=1000000L)
	//{
	//	m_lServiceScore=500L;
	//}

	////服务费用
	//if (m_lServiceScore>0L)
	//{
	//	for (WORD i=0;i<m_wPlayerCount;i++)
	//	{
	//		//获取用户
	//		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
	//		if (pIServerUserItem==NULL) continue;

	//		//构造信息
	//		tagScoreInfo ScoreInfo;
	//		ScoreInfo.lScore=-m_lServiceScore;
	//		ScoreInfo.ScoreKind=enScoreKind_Service;

	//		//写入积分
	//		m_pITableFrame->WriteUserScore(i,ScoreInfo);

	//		//发送消息
	//		TCHAR szMess[512]=TEXT("");
	//		_sntprintf(szMess,sizeof(szMess),TEXT("系统扣除 %ld 的游戏服务费"),m_lServiceScore);
	//		m_pITableFrame->SendGameMessage(pIServerUserItem,szMess,SMT_INFO);
	//	}
	//}

	//最大下注
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//获取积分
		const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();
		ASSERT(pUserScore->lScore>=m_pGameServiceOption->lCellScore);

		//设置变量
		m_cbPlayStatus[i]=TRUE;
		m_lUserMaxScore[i]=pUserScore->lScore;
		if ((m_lMaxScore==0L)||(pUserScore->lScore<m_lMaxScore)) m_lMaxScore=pUserScore->lScore;
	}

	//下注变量
	m_lTurnMaxScore=m_lMaxScore/2L;
	m_lCellScore=__max(m_lMaxScore/100L,m_pGameServiceOption->lCellScore);
	m_lTurnLessScore=__max(m_lMaxScore/100L,m_pGameServiceOption->lCellScore);

	//分发扑克
	m_cbSendCardCount=2;
	m_GameLogic.RandCardList(m_cbHandCardData[0],sizeof(m_cbHandCardData)/sizeof(m_cbHandCardData[0][0]));

	//用户设置
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			m_lTableScore[2*i+1]=m_lCellScore;
			m_cbCardCount[i]=m_cbSendCardCount;
		}
	}

	//设置用户
	m_wOperaCount=0;
	m_wCurrentUser=EstimateWinner(1,1);
	m_wLastMostUser = m_wCurrentUser;

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.lMaxScore=m_lMaxScore;
	GameStart.lCellScore=m_lCellScore;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.lTurnMaxScore=m_lTurnMaxScore;
	GameStart.lTurnLessScore=m_lTurnLessScore;

	//设置扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==FALSE) GameStart.cbCardData[i]=0;
		else GameStart.cbCardData[i]=m_cbHandCardData[i][1];
	}

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		GameStart.cbObscureCard=0;
		if (m_cbPlayStatus[i]==TRUE)
		{
			GameStart.cbObscureCard=m_cbHandCardData[i][0];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		GameStart.cbObscureCard=0;
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	//设置时间
	m_pITableFrame->SetGameTimer(IDI_PASS_CARD,40000L,1,m_wCurrentUser);

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
	case GER_NO_PLAYER:		//没有玩家
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//计算总注
			LONG lAllScore=0L;
			for (WORD i=0;i<CountArray(m_lTableScore);i++) lAllScore+=m_lTableScore[i];

			//变量定义
			WORD wWinerUser=EstimateWinner(0,4);
			LONG lMaxLostScore=m_lTableScore[wWinerUser*2]+m_lTableScore[wWinerUser*2+1];

			//统计信息
			LONG lWinnerScore=0L;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//底牌信息
				if ((cbReason==GER_NO_PLAYER)||(m_cbPlayStatus[i]==FALSE)) GameEnd.cbCardData[i]=0;
				else GameEnd.cbCardData[i]=m_cbHandCardData[i][0];

				//积分信息
				if (i!=wWinerUser)
				{
					LONG lLostScore=m_lTableScore[i*2]+m_lTableScore[i*2+1];
					if (m_cbPlayStatus[i]==TRUE)
					{
						lWinnerScore+=__min(lLostScore,lMaxLostScore);
						GameEnd.lGameScore[i]=-__min(lLostScore,lMaxLostScore);
					}
					else
					{
						lWinnerScore+=__min(lLostScore,m_lTableScore[wWinerUser*2]+m_lTableScore[wWinerUser*2+1]);
						GameEnd.lGameScore[i]=-lLostScore;
					}
				}
			}

			//胜者得分
			GameEnd.lGameScore[wWinerUser]=lWinnerScore;
			//扣税
			LONG lRevenue = 0L;
			if( m_pGameServiceOption->wServerType == GAME_GENRE_GOLD )
			{
				if( GameEnd.lGameScore[wWinerUser] >= 100L )
				{
					lRevenue = GameEnd.lGameScore[wWinerUser]*m_pGameServiceOption->wRevenue/100L;
					GameEnd.lGameScore[wWinerUser] -= lRevenue;
					GameEnd.lGameTax[wWinerUser] = lRevenue;
				}
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if( m_cbPlayStatus[i] == TRUE )
				{
					enScoreKind ScoreKind;
					if( GameEnd.lGameScore[i] == 0 ) ScoreKind = enScoreKind_Draw;
					else if( GameEnd.lGameScore[i] > 0 ) ScoreKind = enScoreKind_Win;
					else ScoreKind = enScoreKind_Lost;
					m_pITableFrame->WriteUserScore(i,GameEnd.lGameScore[i],GameEnd.lGameTax[i],ScoreKind);
				}
			}
			
			//成绩记录
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayStatus[i]==TRUE)
				{
					//获取用户
					IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
					if (pIServerUserItem==NULL) continue;
 
					//搜索用户
					tagScoreHistory * pScoreHistory=NULL;
					g_ScoreHistoryMap.Lookup(pIServerUserItem->GetUserID(),pScoreHistory);

					//获取时间
					SYSTEMTIME SystemTime;
					GetLocalTime(&SystemTime);

					//增加用户
					if (pScoreHistory==NULL)
					{
						//创建对象
						pScoreHistory=new tagScoreHistory;
						g_ScoreHistoryMap[pIServerUserItem->GetUserID()]=pScoreHistory;

						//设置变量
						pScoreHistory->lScore=0L;
						pScoreHistory->lWinScore=0L;
						pScoreHistory->lLoseScore=0L;
						pScoreHistory->RecordTime=SystemTime;
					}

					//还原判断
					if (pScoreHistory->RecordTime.wDay!=SystemTime.wDay)
					{
						pScoreHistory->lScore=0L;
						pScoreHistory->lWinScore=0L;
						pScoreHistory->lLoseScore=0L;
						pScoreHistory->RecordTime=SystemTime;
					}

					//记录积分
					pScoreHistory->lScore+=GameEnd.lGameScore[i];
					if (GameEnd.lGameScore[i]>0L) pScoreHistory->lWinScore+=GameEnd.lGameScore[i];
					if (GameEnd.lGameScore[i]<0L) pScoreHistory->lLoseScore+=GameEnd.lGameScore[i];
				}
			}

			//删除时间
			m_pITableFrame->KillGameTimer(IDI_GAME_END);
			m_pITableFrame->KillGameTimer(IDI_PASS_CARD);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//设置数据
			m_cbCardCount[wChairID]=0;
			m_cbPlayStatus[wChairID]=FALSE;

			//发送消息
			CMD_S_GiveUp GiveUp;
			GiveUp.wGiveUpUser=wChairID;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

			//构造信息
			LONG lScore=-m_lTableScore[wChairID*2]-m_lTableScore[wChairID*2+1];

			//写入积分
			m_pITableFrame->WriteUserScore(wChairID,lScore,0,enScoreKind_Flee);

			//积分记录
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
			if (pIServerUserItem!=NULL)
			{
				//搜索用户
				tagScoreHistory * pScoreHistory=NULL;
				g_ScoreHistoryMap.Lookup(pIServerUserItem->GetUserID(),pScoreHistory);

				//获取时间
				SYSTEMTIME SystemTime;
				GetLocalTime(&SystemTime);

				//增加用户
				if (pScoreHistory==NULL)
				{
					//创建对象
					pScoreHistory=new tagScoreHistory;
					g_ScoreHistoryMap[pIServerUserItem->GetUserID()]=pScoreHistory;

					//设置变量
					pScoreHistory->lScore=0L;
					pScoreHistory->lWinScore=0L;
					pScoreHistory->lLoseScore=0L;
					pScoreHistory->RecordTime=SystemTime;
				}

				//还原判断
				if (pScoreHistory->RecordTime.wDay!=SystemTime.wDay)
				{
					pScoreHistory->lScore=0L;
					pScoreHistory->lWinScore=0L;
					pScoreHistory->lLoseScore=0L;
					pScoreHistory->RecordTime=SystemTime;
				}

				//记录积分
				pScoreHistory->lScore+=lScore;
				pScoreHistory->lLoseScore+=lScore;
			}

			//人数统计
			WORD wPlayerCount=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
			}

			//判断结束
			if (wPlayerCount>=2)
			{
				if (m_wCurrentUser==wChairID) OnUserAddScore(wChairID,0L,true);
			}
			else
			{
				OnEventGameEnd(INVALID_CHAIR,NULL,GER_NO_PLAYER);
			}

			return true;
		}
	case GER_DISMISS:
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//删除时间
			m_pITableFrame->KillGameTimer(IDI_GAME_END);
			m_pITableFrame->KillGameTimer(IDI_PASS_CARD);

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
	case GS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=0L;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_PLAYING:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//标志变量
			StatusPlay.bAddScore=m_cbOperaScore[wChiarID];
			StatusPlay.bShowHand=(m_bShowHand==true)?TRUE:FALSE;

			//加注信息
			StatusPlay.lMaxScore=m_lMaxScore;
			StatusPlay.lCellScore=m_lCellScore;
			StatusPlay.lTurnMaxScore=m_lTurnMaxScore;
			StatusPlay.lTurnLessScore=m_lTurnLessScore;

			//设置变量
			StatusPlay.wCurrentUser=m_wCurrentUser;
			CopyMemory(StatusPlay.lTableScore,m_lTableScore,sizeof(m_lTableScore));
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));

			//设置扑克
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayStatus[i]==TRUE)
				{
					StatusPlay.cbCardCount[i]=m_cbCardCount[i];
					if ((i==wChiarID)&&(pIServerUserItem->GetUserStatus()!=US_LOOKON)) StatusPlay.cbHandCardData[i][0]=m_cbHandCardData[i][0];
					else StatusPlay.cbHandCardData[i][0]=0;
					CopyMemory(&StatusPlay.cbHandCardData[i][1],&m_cbHandCardData[i][1],(m_cbCardCount[i]-1)*sizeof(BYTE));
				}
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	//效验错误
	ASSERT(FALSE);

	return false;
}

//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_GAME_END:	//游戏结束
		{
			if (m_pITableFrame->GetGameStatus()==GS_PLAYING)
			{
				OnEventGameEnd(INVALID_CHAIR,NULL,GER_NORMAL);
				return true;
			}
		}
	case IDI_PASS_CARD:	//放弃加注
		{
			if ((wBindParam==m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR))
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(m_wCurrentUser);
				
				//用户判断
				if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()!=US_OFFLINE))
				{
					OnUserGiveUp(m_wCurrentUser);
				}
				else m_pITableFrame->SetGameTimer(IDI_PASS_CARD,40000L,1,m_wCurrentUser);

				//test
				//m_pITableFrame->SetGameTimer(IDI_PASS_CARD,40000L,1,m_wCurrentUser);
				return true;
			}
		}
	}

	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_ADD_SCORE:			//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pDataBuffer;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserAddScore(pUserData->wChairID,pAddScore->lScore,false);
		}
	case SUB_C_GIVE_UP:				//用户放弃
		{
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserGiveUp(pUserData->wChairID);
		}
	case SUB_C_GET_WINNER:			//获取胜者
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_GetWinner));
			if (wDataSize!=sizeof(CMD_C_GetWinner)) return false;

			//变量定义
			CMD_C_GetWinner * pGetWiner=(CMD_C_GetWinner *)pDataBuffer;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			return OnUserGetWinner(pIServerUserItem);
		}
	case SUB_C_GET_OPTION:			//获取信息
		{
			//构造变量
			CMD_S_ScoreOption ScoreOption;
			ScoreOption.lMaxScore=g_lMaxScore;
			ScoreOption.lMaxWinScore=g_lMaxWinScore;
			ScoreOption.lMaxLoseScore=g_lMaxLoseScore;
			ScoreOption.lUserCount=(LONG)g_ScoreHistoryMap.GetCount();
			ScoreOption.lBlackCount=(LONG)g_BlacklistIDMap.GetCount();

			//发送数据
			m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_SCORE_OPTION,&ScoreOption,sizeof(ScoreOption));

			return true;
		}
	case SUB_C_SET_MAX_SCORE:		//设置积分
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ScoreInfo));
			if (wDataSize!=sizeof(CMD_C_ScoreInfo)) return false;

			//设置变量
			g_lMaxScore=((CMD_C_ScoreInfo *)pDataBuffer)->lScore;
			g_lMaxScore=__max(g_lMaxScore,0L);

			return true;
		}
	case SUB_C_SET_MAX_WIN_SCORE:	//设置积分
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ScoreInfo));
			if (wDataSize!=sizeof(CMD_C_ScoreInfo)) return false;

			//设置变量
			g_lMaxWinScore=((CMD_C_ScoreInfo *)pDataBuffer)->lScore;
			g_lMaxWinScore=__max(g_lMaxWinScore,0L);

			return true;
		}
	case SUB_C_SET_MAX_LOSE_SCORE:	//设置积分
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ScoreInfo));
			if (wDataSize!=sizeof(CMD_C_ScoreInfo)) return false;

			//设置变量
			g_lMaxLoseScore=((CMD_C_ScoreInfo *)pDataBuffer)->lScore;
			g_lMaxLoseScore=__min(g_lMaxLoseScore,0L);

			return true;
		}
	case SUB_C_SET_BLACKLIST:		//设置用户
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_UserIDInfo));
			if (wDataSize!=sizeof(CMD_C_UserIDInfo)) return false;

			//设置变量
			DWORD dwUserID=((CMD_C_UserIDInfo *)pDataBuffer)->dwUserID;
			g_BlacklistIDMap[dwUserID]=dwUserID;

			return true;
		}
	case SUB_C_REMOVE_BLACKLIST:	//设置用户
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_UserIDInfo));
			if (wDataSize!=sizeof(CMD_C_UserIDInfo)) return false;

			//设置变量
			g_BlacklistIDMap.RemoveKey(((CMD_C_UserIDInfo *)pDataBuffer)->dwUserID);

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

//用户断线
bool __cdecl CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if (wChairID==m_wCurrentUser)
	{
		m_pITableFrame->KillGameTimer(IDI_PASS_CARD);
	}

	return true;
}

//用户重入
bool __cdecl CTableFrameSink::OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if (wChairID==m_wCurrentUser)
	{
		m_pITableFrame->SetGameTimer(IDI_PASS_CARD,40000L,1,m_wCurrentUser);
	}

	return true;
}

//放弃事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID)
{
	//设置数据
	m_cbCardCount[wChairID]=0;
	m_cbPlayStatus[wChairID]=FALSE;

	//发送消息
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

	//构造信息
	LONG lScore = -m_lTableScore[wChairID*2]-m_lTableScore[wChairID*2+1];

	//写入积分
	m_pITableFrame->WriteUserScore(wChairID,lScore,0,enScoreKind_Lost);

	//积分记录
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
	if (pIServerUserItem!=NULL)
	{
		//搜索用户
		tagScoreHistory * pScoreHistory=NULL;
		g_ScoreHistoryMap.Lookup(pIServerUserItem->GetUserID(),pScoreHistory);

		//获取时间
		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);

		//增加用户
		if (pScoreHistory==NULL)
		{
			//创建对象
			pScoreHistory=new tagScoreHistory;
			g_ScoreHistoryMap[pIServerUserItem->GetUserID()]=pScoreHistory;

			//设置变量
			pScoreHistory->lScore=0L;
			pScoreHistory->lWinScore=0L;
			pScoreHistory->lLoseScore=0L;
			pScoreHistory->RecordTime=SystemTime;
		}

		//还原判断
		if (pScoreHistory->RecordTime.wDay!=SystemTime.wDay)
		{
			pScoreHistory->lScore=0L;
			pScoreHistory->lWinScore=0L;
			pScoreHistory->lLoseScore=0L;
			pScoreHistory->RecordTime=SystemTime;
		}

		//记录积分
		pScoreHistory->lScore+=lScore;
		pScoreHistory->lLoseScore+=lScore;
	}

	//人数统计
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//判断结束
	if (wPlayerCount>=2)
	{
		if (m_wCurrentUser==wChairID) OnUserAddScore(wChairID,0L,true);
	}
	else
	{
		OnEventGameEnd(INVALID_CHAIR,NULL,GER_NO_PLAYER);
	}

	return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONG lScore, bool bGiveUp)
{
	//设置数据
	if (bGiveUp==false)
	{
		//状态效验
		ASSERT(m_wCurrentUser==wChairID);
		if (m_wCurrentUser!=wChairID) return false;

		//金币效验
		ASSERT((lScore>=0)&&(lScore>=m_lTableScore[wChairID*2]));
		ASSERT((lScore+m_lTableScore[wChairID*2+1])<=m_lUserMaxScore[wChairID]);
		if ((lScore<0)||(lScore<m_lTableScore[wChairID*2])) return false;
		if ((lScore+m_lTableScore[wChairID*2+1])>m_lUserMaxScore[wChairID]) return false;

		//下注变量
		LONG lUserMaxScore=0L;
		LONG lUserLessScore=0L;
		if (m_bShowHand==false)
		{
			ASSERT(m_lTurnMaxScore>=m_lTableScore[wChairID*2+1]);
			ASSERT(m_lTurnLessScore>=m_lTableScore[wChairID*2+1]);
			lUserMaxScore=m_lTurnMaxScore-m_lTableScore[wChairID*2+1];
			lUserLessScore=m_lTurnLessScore-m_lTableScore[wChairID*2+1];
		}
		else
		{
			ASSERT(m_lUserMaxScore[wChairID]>=m_lTableScore[wChairID*2+1]);
			lUserMaxScore=m_lUserMaxScore[wChairID]-m_lTableScore[wChairID*2+1];
			lUserLessScore=m_lUserMaxScore[wChairID]-m_lTableScore[wChairID*2+1];
		}

		//梭哈判断
		bool bUserShowHand=((lScore+m_lTableScore[wChairID*2+1])==m_lUserMaxScore[wChairID]);

		//下注效验
		ASSERT((lScore==lUserLessScore)||(lScore>=lUserMaxScore)||(m_cbOperaScore[wChairID]==FALSE));
		if ((lScore>lUserLessScore)&&(lScore<lUserMaxScore)&&(m_cbOperaScore[wChairID]==TRUE)) return false;

		//梭哈判断
		if ((m_bShowHand==true)||((bUserShowHand==true)&&(m_lMaxScore==m_lTurnMaxScore)))
		{
			//设置金币
			ASSERT((lScore+m_lTableScore[wChairID*2+1])==m_lUserMaxScore[wChairID]);
			m_lTableScore[wChairID*2]=m_lUserMaxScore[wChairID]-m_lTableScore[wChairID*2+1];
		}
		else
		{
			//设置金币
			ASSERT((lScore>=lUserLessScore)&&(lScore<=lUserMaxScore));
			m_lTableScore[wChairID*2]=__min(lUserMaxScore,__max(lScore,lUserLessScore));
		}

		//设置变量
		m_cbOperaScore[wChairID]=TRUE;
		m_bShowHand=((m_bShowHand==true)||(bUserShowHand==true));
		m_lTurnLessScore=__max(m_lTableScore[wChairID*2]+m_lTableScore[wChairID*2+1],m_lTurnLessScore);
	}

	//用户切换
	WORD wNextPlayer=INVALID_CHAIR;
	for (WORD i=0;i<m_wPlayerCount-1;i++)
	{
		//设置变量
		m_wOperaCount++;
		wNextPlayer=(m_wCurrentUser+i+1)%m_wPlayerCount;

		//继续判断
		if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
	}
	
	//完成判断
	bool bFinishTurn=false;
	if (m_wOperaCount>=m_wPlayerCount)
	{
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			LONG lUserScore=m_lTableScore[i*2]+m_lTableScore[i*2+1];
			if ((m_cbPlayStatus[i]==TRUE)&&(lUserScore<m_lTurnLessScore)&&(lUserScore<m_lUserMaxScore[i])) break;
		}
		if (i==m_wPlayerCount) bFinishTurn=true;
	}

	//继续加注
	if (bFinishTurn==false)
	{
		//设置用户
		m_wCurrentUser=wNextPlayer;

		//构造数据
		CMD_S_AddScore AddScore;
		AddScore.wAddScoreUser=wChairID;
		AddScore.wCurrentUser=m_wCurrentUser;
		AddScore.lTurnLessScore=m_lTurnLessScore;
		AddScore.lAddScoreCount=m_lTableScore[wChairID*2];

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

		//设置时间
		m_pITableFrame->SetGameTimer(IDI_PASS_CARD,40000L,1,m_wCurrentUser);

		return true;
	}
	else
	{
		//构造数据
		CMD_S_AddScore AddScore;
		AddScore.wAddScoreUser=wChairID;
		AddScore.wCurrentUser=INVALID_CHAIR;
		AddScore.lTurnLessScore=m_lTurnLessScore;
		AddScore.lAddScoreCount=m_lTableScore[wChairID*2];

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	}

	//结束判断
	if (m_cbSendCardCount==MAX_COUNT) 
	{
		OnEventGameEnd(INVALID_CHAIR,NULL,GER_NORMAL);
		return true;
	}

	//累计金币
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_lTableScore[i*2+1]+=m_lTableScore[i*2];
		m_lTableScore[i*2]=0L;
	}

	//设置变量
	m_wOperaCount=0;
	ZeroMemory(m_cbOperaScore,sizeof(m_cbOperaScore));

	//混乱扑克
	if (m_bChaosCard==true)
	{
		//交换扑克
		BYTE cbSwitchCard=m_cbHandCardData[0][m_cbSendCardCount];

		//交换扑克
		for (WORD i=0;i<m_wPlayerCount-1;i++)
		{
			m_cbHandCardData[i][m_cbSendCardCount]=m_cbHandCardData[i+1][m_cbSendCardCount];
		}

		//交换扑克
		m_cbHandCardData[m_wPlayerCount-1][m_cbSendCardCount]=cbSwitchCard;
	}

	//派发扑克
	BYTE cbSourceCount=m_cbSendCardCount;
	if (m_bShowHand==false) m_cbSendCardCount++;
	else m_cbSendCardCount=MAX_COUNT;

	//上次最大用户
	WORD wLastMostUser = m_wLastMostUser;
	//当前用户
	if (m_bShowHand==false)
	{
		//用户设置
		if( m_cbSendCardCount == MAX_COUNT )
			m_wCurrentUser=EstimateWinner(1,m_cbSendCardCount-1);
		else
			m_wCurrentUser = EstimateWinner(m_cbSendCardCount-1,m_cbSendCardCount-1);
		m_wLastMostUser = m_wCurrentUser;
		
		//下注设置
		m_lTurnMaxScore=(m_cbSendCardCount>=3)?m_lMaxScore:m_lMaxScore/2L;
		m_lTurnLessScore=m_lTableScore[m_wCurrentUser*2]+m_lTableScore[m_wCurrentUser*2+1];
	}
	else m_wCurrentUser=INVALID_CHAIR; 

	//构造数据
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=m_wCurrentUser;
	SendCard.wLastMostUser = wLastMostUser;
	SendCard.lTurnMaxScore=m_lTurnMaxScore;
	SendCard.lTurnLessScore=m_lTurnLessScore;
	SendCard.cbSendCardCount=m_cbSendCardCount-cbSourceCount;

	//发送扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//扑克数目
		if (m_cbPlayStatus[i]==TRUE) m_cbCardCount[i]=m_cbSendCardCount;

		//派发扑克
		for (BYTE j=0;j<(m_cbSendCardCount-cbSourceCount);j++)
		{
			ASSERT(j<CountArray(SendCard.cbCardData[i]));
			SendCard.cbCardData[i][j]=m_cbHandCardData[i][cbSourceCount+j];
		}
	}

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	//结束处理
	if (m_wCurrentUser==INVALID_CHAIR) 
	{
		m_pITableFrame->KillGameTimer(IDI_PASS_CARD);
		m_pITableFrame->SetGameTimer(IDI_GAME_END,2000,1,0);
	}
	else m_pITableFrame->SetGameTimer(IDI_PASS_CARD,40000L,1,m_wCurrentUser);

	return true;
}

//获取胜者
bool CTableFrameSink::OnUserGetWinner(IServerUserItem * pIServerUserItem)
{
	////获取胜者
	//IServerUserItem * pIWinnerUserItem=m_pITableFrame->GetServerUserItem(EstimateWinner(0,4));
	//if (pIWinnerUserItem==NULL) return true;

	////构造消息
	//TCHAR szMessage[128];
	//_snprintf(szMessage,CountArray(szMessage),TEXT("本局的胜利玩家是：%s"),pIWinnerUserItem->GetAccounts());

	////设置变量
	//m_bChaosCard=false;

	////发送消息
	//m_pITableFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);

	return true;
}

//推断输者
WORD CTableFrameSink::EstimateLoser(BYTE cbStartPos, BYTE cbConcludePos)
{
	//保存扑克
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

	//寻找玩家
	for (WORD wLoser=0;wLoser<m_wPlayerCount;wLoser++)
	{
		if (m_cbPlayStatus[wLoser]==TRUE) 
		{
			m_GameLogic.SortCardList(cbUserCardData[wLoser]+cbStartPos,cbConcludePos-cbStartPos+1);
			break;
		}
	}

	//对比玩家
	for (WORD i=(wLoser+1);i<m_wPlayerCount;i++)
	{
		//用户过滤
		if (m_cbPlayStatus[i]==FALSE) continue;

		//排列扑克
		m_GameLogic.SortCardList(cbUserCardData[i]+cbStartPos,cbConcludePos-cbStartPos+1);

		//对比扑克
		if (m_GameLogic.CompareCard(cbUserCardData[i]+cbStartPos,cbUserCardData[wLoser]+cbStartPos,cbConcludePos-cbStartPos+1)==false)
		{
			wLoser=i;
		}
	}

	return wLoser;
}

//推断胜者
WORD CTableFrameSink::EstimateWinner(BYTE cbStartPos, BYTE cbConcludePos)
{
	//保存扑克
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

	//寻找玩家
	for (WORD wWinner=0;wWinner<m_wPlayerCount;wWinner++)
	{
		if (m_cbPlayStatus[wWinner]==TRUE) 
		{
			//排列扑克
			m_GameLogic.SortCardList(cbUserCardData[wWinner]+cbStartPos,cbConcludePos-cbStartPos+1);
			break;
		}
	}

	//对比玩家
	for (WORD i=(wWinner+1);i<m_wPlayerCount;i++)
	{
		//用户过滤
		if (m_cbPlayStatus[i]==FALSE) continue;

		//排列扑克
		m_GameLogic.SortCardList(cbUserCardData[i]+cbStartPos,cbConcludePos-cbStartPos+1);

		//对比扑克
		if (m_GameLogic.CompareCard(cbUserCardData[i]+cbStartPos,cbUserCardData[wWinner]+cbStartPos,cbConcludePos-cbStartPos+1)==true) 
		{
			wWinner=i;
		}
	}

	return wWinner;
}

//////////////////////////////////////////////////////////////////////////
