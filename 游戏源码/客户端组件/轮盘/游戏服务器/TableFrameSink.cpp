#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_TimeControl;	//开始模式

#define IDI_START_GAME 1
#define IDI_CHECK_PLAYER	2

#define TIME_CHECK_PLAYER   10000

#ifdef _DEBUG
#define TIME_START_GAME 30000
#else
#define TIME_START_GAME 30000
#endif
//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_bWinNumber=255;
	m_bTargetNumber=0;
	m_dwBetTime=0L;
	ZeroMemory(m_wAllUserChips,sizeof(m_wAllUserChips));
	ZeroMemory(m_wChipsInfo,sizeof(m_wChipsInfo));
	ZeroMemory(m_bUserStop,sizeof(m_bUserStop));
	memset(m_bHistoryInfo,0xff,sizeof(m_bHistoryInfo));

	//组件变量
	m_pITableFrame=NULL;
	m_pITableFrameControl=NULL;
	m_pGameServiceOption=NULL;

	//库存变量
	m_lStockScore=0;
	m_lDesPer = 0 ;
	m_lFreezeScore=0L;

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

	//查询接口
	m_pITableFrameControl=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrame,ITableFrameControl);
	ASSERT(m_pITableFrameControl!=NULL);
	if (m_pITableFrameControl==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//配置参数
	TCHAR   buffer[MAX_PATH];
	if (GetCurrentDirectory(sizeof(buffer), buffer))
	{
		CString strIniFileName ;

		CString szDir = buffer;
		if(szDir.Right(1) != "\\")	szDir += "\\";
		szDir += "Round.ini";
		strIniFileName= szDir ;

		if(!strIniFileName.IsEmpty())
		{
            //库存数目
			CString StockScore ;
			GetPrivateProfileString(TEXT("StockScore"), TEXT("StockScore"), NULL, StockScore.GetBuffer(64), 64, strIniFileName) ;
			StockScore.ReleaseBuffer() ;
			m_lStockScore = atol(LPCTSTR(StockScore)) ;

			//递减比率
			CString DesPer ;
			GetPrivateProfileString(TEXT("DesPer"), TEXT("DesPer"), NULL, DesPer.GetBuffer(64), 64, strIniFileName) ;
			DesPer.ReleaseBuffer() ;
			m_lDesPer = atol(LPCTSTR(DesPer)) ;
		}

		if(m_lDesPer<=0) m_lDesPer = 2 ;
		if(m_lStockScore<=0) m_lStockScore = 5000 ;

	}

	return true;
}

//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	m_pITableFrame->KillGameTimer(IDI_START_GAME);
	m_dwBetTime=0L;

	//游戏变量
	m_bWinNumber=255;
	ZeroMemory(m_wAllUserChips,sizeof(m_wAllUserChips));
	ZeroMemory(m_wChipsInfo,sizeof(m_wChipsInfo));
	ZeroMemory(m_bUserStop,sizeof(m_bUserStop));

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
	//m_pITableFrame->SetGameStatus(GS_WK_SCORE);

	m_bTargetNumber=rand()%37;
	//m_bTargetNumber=30;

	//库存判断
	LONGLONG lPayoffScore=GetPayScore(m_wAllUserChips,m_bTargetNumber);
	if ((lPayoffScore+m_lStockScore)<0L)
	{
		LONGLONG lTempScore=lPayoffScore;
		for(int i=0;i<37;i++)
		{
			//得到最小付出数字
			lPayoffScore=GetPayScore(m_wAllUserChips,numberArrayIndex[i]);
			if(lPayoffScore>lTempScore)
			{
				m_bTargetNumber=numberArrayIndex[i];
				if(rand()%3==0) break;
			}
		}
	}

	//构造数据
	CMD_S_GameStart GameStart;
	GameStart.bWinNumber=m_bTargetNumber;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	
	m_pITableFrame->SetGameTimer(IDI_CHECK_PLAYER,TIME_CHECK_PLAYER,1,NULL);
	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	m_bWinNumber=m_bTargetNumber;
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

			m_dwBetTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_START_GAME,TIME_START_GAME,1,NULL);
			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			//变量定义
			LONGLONG lIncrease=(LONGLONG)(m_lStockScore*((float)m_lDesPer/100L));

			//设置库存
			if (GetPayScore(m_wAllUserChips,m_bWinNumber)!=0)
			{
				//庄家输
				if(GetPayScore(m_wAllUserChips,m_bWinNumber)<0) m_lStockScore-=lIncrease;
				//玩家输
				if(GetPayScore(m_wAllUserChips,m_bWinNumber)>0) m_lStockScore+=lIncrease;
				m_lFreezeScore+=lIncrease;
			}

			//定义变量
			CMD_S_GameEnd GameEnd;

			//变量定义
			LONG lCellScore=m_pGameServiceOption->lCellScore;

			LONG lGameScore[GAME_PLAYER];
			ZeroMemory(lGameScore,sizeof(lGameScore));
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
				if (pIServerUserItem==NULL) continue;

				lGameScore[i]=m_GameLogic.ComputeScore(m_wChipsInfo[i],m_bWinNumber);
			}

			GameEnd.bWinNumber=m_bWinNumber;
			CopyMemory(GameEnd.lGameScore,lGameScore,sizeof(lGameScore));

			//扣除下注筹码
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
				if (pIServerUserItem==NULL) continue;

				for(int j=0;j<256;j++)
				{
					if(m_wChipsInfo[i][j]>0) GameEnd.lGameScore[i]-=m_wChipsInfo[i][j];
				}			
			}
			
			//写入积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
				if (pIServerUserItem==NULL) continue;

				LONG lScore=GameEnd.lGameScore[i];
				LONG lRevenue=0;
				enScoreKind	ScoreKind;
				if(lScore>0)
				{
					ScoreKind=enScoreKind_Win;
					if(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)	lRevenue=lScore*m_pGameServiceOption->wRevenue/100;
					lScore-=lRevenue;
				}
				else if(lScore==0) 	ScoreKind=enScoreKind_Draw;
				else				ScoreKind=enScoreKind_Lost;
				
				m_pITableFrame->WriteUserScore(pIServerUserItem,lScore,lRevenue,ScoreKind);
			}

			//变量定义
			IServerUserItem * pIServerUserItem=NULL;
			const tagUserScore * pUserScore=NULL;
			//为每个玩家设置最大下注
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
				if(pIServerUserItem==NULL) continue;

				//设置最大下注
				//pUserScore=pIServerUserItem->GetUserScore();
				//GameEnd.lMaxBetWinScore[i]=(long)__max(pUserScore->lScore,0);
				//GameEnd.lMaxBetWinScore[i]=(long)__min(GameEnd.lMaxBetWinScore[i],m_pGameServiceOption->lCellScore*700);
				GameEnd.lMaxBetWinScore[i]=m_pGameServiceOption->lCellScore*720;
				GameEnd.lMaxUserScore[i]=pIServerUserItem->GetUserScore()->lScore;
			}

			//已记满,数据前移
			if(m_bHistoryInfo[HISTORY_NUMBER-1]!=0xff)
			{
				for(BYTE j=0;j<HISTORY_NUMBER-1;j++)
				{
					m_bHistoryInfo[j]=m_bHistoryInfo[j+1];
				}
				m_bHistoryInfo[HISTORY_NUMBER-1]=0xff;
			}
			//记录历史
			for(BYTE i=0;i<HISTORY_NUMBER;i++)
			{
				//记录到最后位置
				if(m_bHistoryInfo[i]==0xff)
				{
					m_bHistoryInfo[i]=m_bWinNumber;
					break;
				}
			}

			CopyMemory(GameEnd.bHistoryInfo,m_bHistoryInfo,sizeof(m_bHistoryInfo));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));


			//结束游戏
			m_pITableFrame->ConcludeGame();

			m_dwBetTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_START_GAME,TIME_START_GAME,1,NULL);
			return true;
		}
	case GER_USER_LEFT:		//用户强退
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

			//写分
			LONG lScore=(WORD)(-GetAllBetChip(m_wChipsInfo[wChairID]));
			LONG lRevenue=0;
			enScoreKind	ScoreKind=enScoreKind_Lost;
			
			m_pITableFrame->WriteUserScore(pIServerUserItem,lScore,lRevenue,ScoreKind);

			//结束游戏
			m_pITableFrame->ConcludeGame();

			m_dwBetTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_START_GAME,TIME_START_GAME,1,NULL);
			return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch(cbGameStatus)
	{
	case GS_FREE:
		{
			CMD_S_StatusFree StatusFree;
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			StatusFree.lUserScore=pIServerUserItem->GetUserScore()->lScore;
			CopyMemory(StatusFree.bHistoryInfo,m_bHistoryInfo,sizeof(m_bHistoryInfo));	

			if(m_dwBetTime==0) 
			{
				m_dwBetTime=(DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_START_GAME,TIME_START_GAME,1,NULL);
			}
			DWORD dwCurrentTime=(DWORD)time(NULL);
			DWORD dwPassTime=dwCurrentTime-m_dwBetTime;

			StatusFree.cbTimeLeave=(BYTE)((TIME_START_GAME/1000-__min(dwPassTime,TIME_START_GAME/1000)));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));	
		}
	}
	return true;
}

//用户坐下
bool __cdecl CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//设置时间
	if ((bLookonUser==false)&&m_dwBetTime==0L)
	{
		m_dwBetTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_START_GAME,TIME_START_GAME,1,NULL);
	}
	
	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserReqStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(bLookonUser==false)
	{
		//库存统计
		m_lStockScore+=GetAllBetChip(m_wChipsInfo[wChairID]);
			
		//变量定义
		tagScoreInfo ScoreInfo;
		ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

		//统计成绩
		ScoreInfo.lScore=(WORD)(-GetAllBetChip(m_wChipsInfo[wChairID]));

		//写入积分
		if (ScoreInfo.lScore!=0L)
		{
			//写入积分
			ScoreInfo.ScoreKind=enScoreKind_Flee;

			m_pITableFrame->WriteUserScore(pIServerUserItem,ScoreInfo.lScore,0,ScoreInfo.ScoreKind);
		}
		//清除筹码
		ZeroMemory(m_wChipsInfo[wChairID],sizeof(m_wChipsInfo[wChairID]));
	}

	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//检测用户
	bool bHaveUser=false;
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
		if(pIServerUserItem!=NULL) bHaveUser=true;
	}

	//没有用户时重置桌子
	if(bHaveUser==false) RepositTableFrameSink();

	return true;
}
//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_START_GAME:		//下注时间
		{
			//开始游戏
			m_pITableFrameControl->StartGame();

			return true;
		}
	case IDI_CHECK_PLAYER:
		{
			//检测用户
			bool bHaveUser=false;
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
				if(pIServerUserItem!=NULL) bHaveUser=true;
			}

			//没有用户时重置桌子
			if(bHaveUser==false)
			{
				m_pITableFrame->KillGameTimer(IDI_CHECK_PLAYER);
				m_pITableFrame->ConcludeGame();
			}

			return true;
		}
	}
	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_GET_STOP:
		{
			if(m_bUserStop[pIServerUserItem->GetChairID()]==true) return true;
			m_bUserStop[pIServerUserItem->GetChairID()]=true;
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
				if (pIServerUserItem==NULL) continue;

				if(m_bUserStop[i]==false&&pIServerUserItem->GetUserStatus()!=US_OFFLINE) return true;
			}		
			return OnEventGameEnd(pIServerUserItem->GetChairID(),pIServerUserItem,GER_NORMAL);
		}
	case SUB_C_CHIP_IN:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ChipIn));
			if (wDataSize!=sizeof(CMD_C_ChipIn)) return false;

			//消息处理
			CMD_C_ChipIn * pChipIn=(CMD_C_ChipIn *)pDataBuffer;
			return OnUserChipIn(pIServerUserItem->GetChairID(),pChipIn->ChipsInfo);
		}
	case SUB_C_ROLL_END:
		{
			//效验数据
 			ASSERT(wDataSize==sizeof(CMD_C_RollEnd));
			if (wDataSize!=sizeof(CMD_C_RollEnd)) return false;

			//消息处理
			CMD_C_RollEnd * pRollEnd=(CMD_C_RollEnd *)pDataBuffer;
			return OnUserRollEnd(pIServerUserItem->GetChairID(),pRollEnd->WinNumber,pIServerUserItem);	
		}
	}

	return true;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户下注
bool __cdecl CTableFrameSink::OnUserChipIn(WORD wChairID,int ChipsInfo[])
{
	//效验数据
	LONGLONG sum=0;
	LONG lUserScore=m_pITableFrame->GetServerUserItem(wChairID)->GetUserScore()->lScore;
	for(int i=0;i<MAX_BET_POS;i++)	sum+=ChipsInfo[i];
	ASSERT(sum<=lUserScore);
	if(sum>lUserScore) return false;

	//保存下注信息
	CopyMemory(m_wChipsInfo[wChairID],ChipsInfo,sizeof(m_wChipsInfo[0]));

	ZeroMemory(m_wAllUserChips,sizeof(m_wAllUserChips));
	for(int i=0;i<GAME_PLAYER;i++)
	{
		for(int j=0;j<256;j++)
		{
			if(m_wChipsInfo[i][j]!=0)
				m_wAllUserChips[j]+=m_wChipsInfo[i][j];
		}
	}

	//构造数据
	CMD_S_UserChipIn UserChipIn;
	memset(&UserChipIn,0,sizeof(UserChipIn));	
	CopyMemory(UserChipIn.lAllChips,m_wAllUserChips,sizeof(m_wAllUserChips));
	
	//发送信息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_CHIP,&UserChipIn,sizeof(UserChipIn));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_CHIP,&UserChipIn,sizeof(UserChipIn));

	return true;
}

//旋转结束
bool __cdecl CTableFrameSink::OnUserRollEnd(WORD wChairID,BYTE WinNumber,IServerUserItem * pIServerUserItem)
{
	//if(m_bWinNumber==255)
	{
		m_bWinNumber=m_bTargetNumber;
		
		//构造数据
		CMD_S_StopRoll StopRoll;
		memset(&StopRoll,0,sizeof(StopRoll));	
		StopRoll.bWinNumber=m_bWinNumber;
		
		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_STOP_ROLL,&StopRoll,sizeof(StopRoll));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STOP_ROLL,&StopRoll,sizeof(StopRoll));
	}

	return true;
}

//计算总注
LONGLONG __cdecl CTableFrameSink::GetAllBetChip(long lAllUserChips[256])
{
	LONGLONG allChip=0;
	for(int i=0;i<256;i++)
	{
		if(lAllUserChips[i]>0)
			allChip+=lAllUserChips[i];
	}
	return allChip;
}

//计算盈利
LONGLONG __cdecl CTableFrameSink::GetPayScore(long lAllUserChips[256],BYTE bWinNumber)
{
	return (GetAllBetChip(lAllUserChips)-m_GameLogic.ComputeScore(lAllUserChips,bWinNumber));
}
//////////////////////////////////////////////////////////////////////////
