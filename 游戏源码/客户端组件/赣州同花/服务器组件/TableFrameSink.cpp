#include "StdAfx.h"
#include "TableFrameSink.h"


//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//辅助变量
	m_dwPlayTimeCount=0L;

	//请求状态
	ZeroMemory(m_cbResponses,sizeof(m_cbResponses));
	ZeroMemory(m_wRequestTimes,sizeof(m_wRequestTimes));

	//游戏变量
	m_wHeadUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbContinue,sizeof(m_cbContinue));
	ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));
	ZeroMemory(m_bAutoPlayer,sizeof(m_bAutoPlayer));

	//得分变量
	m_TurnScore=0;
	ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));

	//游戏等级
	m_wOrderIndex=0;
	m_cbMainValue=0x02;
	m_cbValueOrder[0]=0x02;
	m_cbValueOrder[1]=0x02;

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));

	//出牌信息
	m_cbTurnCardCount=0;
	m_cbTurnCardType=CT_ERROR;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

	//扑克信息
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
}

//开始模式
enStartMode __cdecl CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//接口查询
VOID * __cdecl CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//配置桌子
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
VOID __cdecl CTableFrameSink::RepositTableFrameSink()
{
	//辅助变量
	m_dwPlayTimeCount=0L;

	//请求状态
	ZeroMemory(m_cbResponses,sizeof(m_cbResponses));
	ZeroMemory(m_wRequestTimes,sizeof(m_wRequestTimes));

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbContinue,sizeof(m_cbContinue));
	ZeroMemory(m_bAutoPlayer,sizeof(m_bAutoPlayer));

	//得分变量
	m_TurnScore=0;

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));


	//出牌信息
	m_cbTurnCardCount=0;
	m_cbTurnCardType=CT_ERROR;
	m_wTurnWiner=INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

	//扑克信息
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	return;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//游戏时间
	m_dwPlayTimeCount=(DWORD)time(NULL);

	//设置状态
	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

	//混乱扑克
	BYTE cbRandCard[FULL_COUNT];
	m_GameLogic.RandCardList(cbRandCard,CountArray(cbRandCard));

	//设置用户
	m_wTurnWiner=m_wHeadUser;
	m_wCurrentUser=m_wHeadUser;

	//用户扑克
	m_cbHandCardCount[m_wCurrentUser]=MAX_COUNT;
	CopyMemory(&m_cbHandCardData[m_wCurrentUser],&cbRandCard[0],sizeof(BYTE)*m_cbHandCardCount[m_wCurrentUser]);
	
	m_cbHandCardCount[(m_wCurrentUser+1)%m_wPlayerCount]=MAX_COUNT;
	CopyMemory(&m_cbHandCardData[(m_wCurrentUser+1)%m_wPlayerCount],&cbRandCard[41],sizeof(BYTE)*m_cbHandCardCount[(m_wCurrentUser+1)%m_wPlayerCount]);
	
	m_cbHandCardCount[(m_wCurrentUser+2)%m_wPlayerCount]=MAX_COUNT-1;
	CopyMemory(&m_cbHandCardData[(m_wCurrentUser+2)%m_wPlayerCount],&cbRandCard[82],sizeof(BYTE)*m_cbHandCardCount[(m_wCurrentUser+2)%m_wPlayerCount]);
	
	m_cbHandCardCount[(m_wCurrentUser+3)%m_wPlayerCount]=MAX_COUNT-1;
	CopyMemory(&m_cbHandCardData[(m_wCurrentUser+3)%m_wPlayerCount],&cbRandCard[122],sizeof(BYTE)*m_cbHandCardCount[(m_wCurrentUser+3)%m_wPlayerCount]);

	//设置逻辑
	m_GameLogic.SetMainValue(m_cbMainValue);

	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//构造变量
	GameStart.cbMainValue=m_cbMainValue;
	GameStart.wCurrentUser=m_wCurrentUser;
	CopyMemory(GameStart.cbValueOrder,m_cbValueOrder,sizeof(m_cbValueOrder));
	CopyMemory(GameStart.wPersistInfo,m_wPersistInfo,sizeof(m_wPersistInfo));

	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//构造扑克
		ASSERT(CountArray(GameStart.cbCardData)>=m_cbHandCardCount[i]);
		ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
		CopyMemory(GameStart.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	//排列扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i],ST_ORDER);
	}

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	ZeroMemory(m_bAutoPlayer,sizeof(m_bAutoPlayer));
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			m_TurnScore=0;
			GameEnd.TurnScore=m_TurnScore;

			//胜利信息
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_cbHandCardCount[i]!=0)
				{
					m_wWinOrder[m_wWinCount++]=i;
				}
			}

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}

			//升级等级
			BYTE cbValueOrder=0;
			if ((m_wWinOrder[0]%2)==(m_wWinOrder[1]%2)) cbValueOrder=3;
			else if ((m_wWinOrder[0]%2)==((m_wWinOrder[3]+1)%2)) cbValueOrder=2;
			else cbValueOrder=1;

			//变量定义
			bool bStatScore=false;
			bool bFinishDraw=false;
			WORD wWinnerUser=m_wWinOrder[0];

			if( (m_wWinOrder[0]==0)&&(m_wWinOrder[1]==2) || (m_wWinOrder[0]==2)&&(m_wWinOrder[1]==0) )
			{
				m_PlayerScore[0]=300;
				m_PlayerScore[2]=300;
				//双扣胜利信息
				TCHAR szMessage[512]=TEXT("");
				_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]与[ %s ]双扣胜利，游戏结束。\n"),m_pITableFrame->GetServerUserItem(0)->GetAccounts(),m_pITableFrame->GetServerUserItem(2)->GetAccounts());
				//发送胜利信息
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
					m_pITableFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);
				}
			}
			else if( (m_wWinOrder[0]==1)&&(m_wWinOrder[1]==3) || (m_wWinOrder[0]==3)&&(m_wWinOrder[1]==1))
			{
				m_PlayerScore[1]=300;
				m_PlayerScore[3]=300;	
				//双扣胜利信息
				TCHAR szMessage[512]=TEXT("");
				_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]与[ %s ]双扣胜利，游戏结束。\n"),m_pITableFrame->GetServerUserItem(1)->GetAccounts(),m_pITableFrame->GetServerUserItem(3)->GetAccounts());
				//发送胜利信息
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
					m_pITableFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);
				}
			}
			else
			{
				//贡献信息
				TCHAR szMessage[512]=TEXT("");
				_sntprintf(szMessage,CountArray(szMessage),TEXT("玩家[ %s ]的 %d 分贡献给第一名[ %s ]。\n"),m_pITableFrame->GetServerUserItem(m_wWinOrder[3])->GetAccounts(),m_PlayerScore[m_wWinOrder[3]],m_pITableFrame->GetServerUserItem(wWinnerUser)->GetAccounts());
				//发送贡献信息
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
					m_pITableFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);
				}
				//下游分数给上游
				m_PlayerScore[wWinnerUser]+=m_PlayerScore[m_wWinOrder[3]];
				m_PlayerScore[m_wWinOrder[3]]=0;
			}

			//放入结束分数
			CopyMemory(GameEnd.PlayerScore,m_PlayerScore,sizeof(m_PlayerScore));

			//过关判断
			if ( (m_PlayerScore[0]+m_PlayerScore[2])>=600 || (m_PlayerScore[1]+m_PlayerScore[3])>=600 
				|| (m_wWinOrder[0]==0)&&(m_wWinOrder[1]==2) || (m_wWinOrder[0]==2)&&(m_wWinOrder[1]==0) 
				|| (m_wWinOrder[0]==1)&&(m_wWinOrder[1]==3) || (m_wWinOrder[0]==3)&&(m_wWinOrder[1]==1)
				)
			{
				//设置变量
				bStatScore=true;
				bFinishDraw=true;

				//设置变量
				m_wOrderIndex=0;
				m_cbMainValue=0x02;
				m_cbValueOrder[0]=0x02;
				m_cbValueOrder[1]=0x02;
			}



			////用户等级
			//if (bFinishDraw==false)
			//{
			//	//用户等级
			//	if (m_cbValueOrder[wWinnerUser%2]!=0x01)
			//	{
			//		for (BYTE i=0;i<cbValueOrder;i++)
			//		{
			//			//增加等级
			//			m_cbValueOrder[wWinnerUser%2]++;

			//			//强制打 A
			//			if (m_cbValueOrder[wWinnerUser%2]==14)
			//			{
			//				m_cbValueOrder[wWinnerUser%2]=0x01;
			//				break;
			//			}
			//		}
			//	}

			//	//等级数值
			//	m_wOrderIndex=wWinnerUser%2;
			//	m_cbMainValue=m_cbValueOrder[m_wOrderIndex];
			//}

			////名次累计
			//if (bFinishDraw==false)
			//{
			//	for (WORD i=0;i<GAME_PLAYER;i++)
			//	{
			//		//设置变量
			//		bool bHandle=false;

			//		//头游判断
			//		if (i==m_wWinOrder[0])
			//		{
			//			bHandle=true;
			//			m_wPersistInfo[i][0]++;
			//			m_wPersistInfo[i][1]=0;
			//		}

			//		//末游判断
			//		if ((i==m_wWinOrder[3])||((i==m_wWinOrder[2])&&((m_wWinOrder[2]+2)%GAME_PLAYER==m_wWinOrder[3])))
			//		{
			//			bHandle=true;
			//			m_wPersistInfo[i][1]++;
			//			m_wPersistInfo[i][0]=0;
			//		}

			//		//默认处理
			//		if (bHandle==false)
			//		{
			//			m_wPersistInfo[i][0]=0;
			//			m_wPersistInfo[i][1]=0;
			//		}
			//	}
			//}

			//积分统计
			if (bStatScore==true)
			{
				//变量定义
				LONG lRevenue=0L;
				LONG lUserScore[4];ZeroMemory(lUserScore,sizeof(lUserScore));
				LONG lCellScore=m_pGameServiceOption->lCellScore;

				if( (m_wWinOrder[0]==0)&&(m_wWinOrder[1]==2) || (m_wWinOrder[0]==2)&&(m_wWinOrder[1]==0) || ((m_PlayerScore[0]+m_PlayerScore[2])>=600 && (m_PlayerScore[0]+m_PlayerScore[2])>(m_PlayerScore[1]+m_PlayerScore[3])))
				{
					lUserScore[0]=1L*lCellScore;
					lUserScore[2]=1L*lCellScore;
					lUserScore[1]=-1L*lCellScore;
					lUserScore[3]=-1L*lCellScore;
				}
				if( (m_wWinOrder[0]==1)&&(m_wWinOrder[1]==3) || (m_wWinOrder[0]==3)&&(m_wWinOrder[1]==1) || ((m_PlayerScore[1]+m_PlayerScore[3])>=600 && (m_PlayerScore[1]+m_PlayerScore[3])>(m_PlayerScore[0]+m_PlayerScore[2])))
				{
					lUserScore[0]=-1L*lCellScore;
					lUserScore[2]=-1L*lCellScore;
					lUserScore[1]=1L*lCellScore;
					lUserScore[3]=1L*lCellScore;	
				}
				if(((m_PlayerScore[1]+m_PlayerScore[3])>=1000&& (m_PlayerScore[1]+m_PlayerScore[3])>(m_PlayerScore[0]+m_PlayerScore[2])))
				{
					lUserScore[0]=-2L*lCellScore;
					lUserScore[2]=-2L*lCellScore;
					lUserScore[1]=2L*lCellScore;
					lUserScore[3]=2L*lCellScore;
				}
				if(((m_PlayerScore[0]+m_PlayerScore[2])>=1000&& (m_PlayerScore[0]+m_PlayerScore[2])>(m_PlayerScore[1]+m_PlayerScore[3])))
				{
					lUserScore[0]=2L*lCellScore;
					lUserScore[2]=2L*lCellScore;
					lUserScore[1]=-2L*lCellScore;
					lUserScore[3]=-2L*lCellScore;
				}
				CopyMemory(GameEnd.lGameScore,lUserScore,sizeof(lUserScore));

				//统计积分
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					//设置积分
					if (m_pGameServiceOption->wServerType==GAME_GENRE_GOLD&&lUserScore[i]>0)
					{
						lRevenue=lUserScore[i]*m_pGameServiceOption->wRevenue/100;
						GameEnd.lGameScore[i]=lUserScore[i]-lRevenue;
					}
					else
					{
						GameEnd.lGameScore[i]=lUserScore[i];
					}

					//写入积分
					enScoreKind ScoreKind=(GameEnd.lGameScore[i]>=0L)?enScoreKind_Win:enScoreKind_Lost;
					m_pITableFrame->WriteUserScore(i,GameEnd.lGameScore[i],lRevenue,ScoreKind);
				}

				//清除游戏得分
				ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));
				CopyMemory(GameEnd.PlayerScore,m_PlayerScore,sizeof(m_PlayerScore));
			}

			//累计名次
			CopyMemory(GameEnd.wPersistInfo,m_wPersistInfo,sizeof(m_wPersistInfo));

			if(bFinishDraw==true)
			{
				//结束信息
				TCHAR szEndMessage[2048]=TEXT("");
				_sntprintf(szEndMessage,CountArray(szEndMessage),TEXT("本局结束，成绩统计：\n %s ： %d 分\n %s ： %d 分\n %s ： %d 分\n %s ： %d 分\n"),
					m_pITableFrame->GetServerUserItem(0)->GetAccounts(),GameEnd.lGameScore[0],m_pITableFrame->GetServerUserItem(1)->GetAccounts(),GameEnd.lGameScore[1],
					m_pITableFrame->GetServerUserItem(2)->GetAccounts(),GameEnd.lGameScore[2],m_pITableFrame->GetServerUserItem(3)->GetAccounts(),GameEnd.lGameScore[3]);

				//发送结束信息
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
					m_pITableFrame->SendGameMessage(pIServerUserItem,szEndMessage,SMT_INFO);
				}
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//设置变量
			m_wHeadUser=m_wWinOrder[0];

			//名次清理
			if (bFinishDraw==true) ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));

			//结束游戏
			if (bFinishDraw==true) m_pITableFrame->ConcludeGame();

			//设置状态
			if (bFinishDraw==false)
			{
				RepositTableFrameSink();
				m_pITableFrame->SetGameStatus(GAME_SCENE_WAIT);
			}

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//游戏等级
			m_wOrderIndex=0;
			m_cbMainValue=0x02;
			m_cbValueOrder[0]=0x02;
			m_cbValueOrder[1]=0x02;

			//名次信息
			ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//游戏等级
			m_wOrderIndex=0;
			m_cbMainValue=0x02;
			m_cbValueOrder[0]=0x02;
			m_cbValueOrder[1]=0x02;

			//名次信息
			ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
			}

			//玩家得分
			GameEnd.lGameScore[wChairID]=-5*m_pGameServiceOption->lCellScore;
					
			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			m_pITableFrame->WriteUserScore(wChairID,GameEnd.lGameScore[wChairID],0L,enScoreKind_Flee);

			//通知消息
			TCHAR szMessage[128]=TEXT("");
			_snprintf(szMessage,CountArray(szMessage),TEXT("由于 [ %s ] 离开游戏，游戏结束"),pIServerUserItem->GetAccounts());
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				IServerUserItem * pISendUserItem=m_pITableFrame->GetServerUserItem(i);
				if (pISendUserItem!=NULL) m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_INFO);
			}

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//游戏变量
			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusFree.bAutoStatus,m_bAutoPlayer,sizeof(StatusFree.bAutoStatus));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.wPersistInfo,m_wPersistInfo,sizeof(m_wPersistInfo));
			CopyMemory(StatusPlay.bAutoStatus,m_bAutoPlayer,sizeof(StatusPlay.bAutoStatus));

			//等级变量
			StatusPlay.cbMainValue=m_cbMainValue;
			CopyMemory(StatusPlay.cbValueOrder,m_cbValueOrder,sizeof(StatusPlay.cbValueOrder));

			//胜利信息
			StatusPlay.wWinCount=m_wWinCount;
			CopyMemory(StatusPlay.wWinOrder,m_wWinOrder,sizeof(StatusPlay.wWinOrder));

			//出牌信息
			StatusPlay.wTurnWiner=m_wTurnWiner;
			StatusPlay.cbTurnCardType=m_cbTurnCardType;
			StatusPlay.cbTurnCardCount=m_cbTurnCardCount;
			CopyMemory(StatusPlay.cbTurnCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));
			CopyMemory(StatusPlay.cbMagicCardData,m_cbMagicCardData,m_cbTurnCardCount*sizeof(BYTE));

			//扑克信息
			WORD wUserIndex=(m_cbHandCardCount[wChairID]==0)?(wChairID+2)%GAME_PLAYER:wChairID;
			CopyMemory(StatusPlay.cbHandCardCount,m_cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(StatusPlay.cbHandCardData,m_cbHandCardData[wUserIndex],sizeof(BYTE)*m_cbHandCardCount[wUserIndex]);

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	case GAME_SCENE_WAIT:	//等待状态
		{
			//构造数据
			CMD_S_StatusWait StatusWait;
			ZeroMemory(&StatusWait,sizeof(StatusWait));

			//设置变量
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				m_cbContinue[wChairID]=FALSE;
			}

			//游戏变量
			StatusWait.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusWait.cbContinue,m_cbContinue,sizeof(m_cbContinue));
			CopyMemory(StatusWait.wPersistInfo,m_wPersistInfo,sizeof(m_wPersistInfo));
			CopyMemory(StatusWait.bAutoStatus,m_bAutoPlayer,sizeof(StatusWait.bAutoStatus));

			//等级变量
			StatusWait.cbMainValue=m_cbMainValue;
			CopyMemory(StatusWait.cbValueOrder,m_cbValueOrder,sizeof(StatusWait.cbValueOrder));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusWait,sizeof(StatusWait));
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	return false;
}

//游戏消息
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_AUTOMATISM:
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_Automatism));
			if (wDataSize!=sizeof(CMD_C_Automatism)) return false;
			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();

			CMD_C_Automatism * pAutomatism =(CMD_C_Automatism *)pData;

			m_bAutoPlayer[pUserData->wChairID]=pAutomatism->bAutomatism;

			CMD_S_UserAutomatism UserAutomatismData;
			CopyMemory(UserAutomatismData.bAutoStatus,m_bAutoPlayer,sizeof(m_bAutoPlayer));
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_AUTOMATISM,&UserAutomatismData,sizeof(UserAutomatismData));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_AUTOMATISM,&UserAutomatismData,sizeof(UserAutomatismData));

			////////////////////////////////////////////////////////////////
			//插入托管消息
			TCHAR szMessage[256]=TEXT("");
			if(pAutomatism->bAutomatism==true) _sntprintf(szMessage,CountArray(szMessage),TEXT("玩家 [%s] 选择了托管功能。"),pUserData->szAccounts);
			else _sntprintf(szMessage,CountArray(szMessage),TEXT("玩家 [%s] 取消了托管功能。"),pUserData->szAccounts);

			for(int i=0; i<GAME_PLAYER; i++)
			{
				if(i==pUserData->wChairID) continue;
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
				if(pIServerUserItem!=NULL)
				{
					m_pITableFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);
				}
			}

			//托管玩家的信息
			if(pAutomatism->bAutomatism==true) _sntprintf(szMessage,CountArray(szMessage),TEXT("您选择了托管功能。"),pUserData->szAccounts);
			else _sntprintf(szMessage,CountArray(szMessage),TEXT("您取消了托管功能。"),pUserData->szAccounts);
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetServerUserItem(pUserData->wChairID);
			if(pIServerUserItem!=NULL)
			{
				m_pITableFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);
			}
			////////////////////////////////////////////////////////////////
			return true;
		}
	case SUB_C_OUT_CARD:			//用户出牌
		{
			//变量定义
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//效验数据
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE))));
			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE)))) return false;

			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAY);
			if (pIServerUserItem->GetUserStatus()!=US_PLAY) return true;

			//消息处理
			WORD wChairID=pIServerUserItem->GetChairID();
			return OnUserOutCard(wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
		}
	case SUB_C_PASS_CARD:			//用户放弃
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_PLAY);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLAY) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAY);
			if (pIServerUserItem->GetUserStatus()!=US_PLAY) return true;

			//消息处理
			return OnUserPassCard(pIServerUserItem->GetChairID());
		}
	case SUB_C_CONTINUE_GAME:		//继续游戏
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_WAIT);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_WAIT) return true;

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAY);
			if (pIServerUserItem->GetUserStatus()!=US_PLAY) return true;

			//消息处理
			return OnUserContinueGame(pIServerUserItem->GetChairID());
		}
	}

	return false;
}

//框架消息
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool __cdecl CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//设置庄家
	if (m_wHeadUser==INVALID_CHAIR) m_wHeadUser=pIServerUserItem->GetChairID();

	return true;
}

//用户起立
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//游戏信息
	if (bLookonUser==false)
	{
		//主牌数值
		m_wOrderIndex=0;
		m_cbMainValue=0x02;
		m_cbValueOrder[0]=0x02;
		m_cbValueOrder[1]=0x02;

		//游戏变量
		ZeroMemory(m_wPersistInfo,sizeof(m_wPersistInfo));
	}

	//庄家设置
	if ((bLookonUser==false)&&(m_wHeadUser==pIServerUserItem->GetChairID())) m_wHeadUser=INVALID_CHAIR;

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//效验状态
	ASSERT(wChairID==m_wCurrentUser);
	if (wChairID!=m_wCurrentUser) return false;

	//变幻扑克
	BYTE cbMagicCardData[MAX_COUNT];
	m_GameLogic.MagicCardData(cbCardData,cbCardCount,cbMagicCardData,m_cbTurnCardType);

	//获取类型
	BYTE cbCardType=m_GameLogic.GetCardType(cbMagicCardData,cbCardCount);

	//类型判断
	if (cbCardType==CT_ERROR) 
	{
		ASSERT(FALSE);
		return false;
	}

	//出牌判断
	if (m_cbTurnCardCount!=0)
	{
		//对比扑克
		if (m_GameLogic.CompareCard(m_cbMagicCardData,cbMagicCardData,m_cbTurnCardCount,cbCardCount)==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//删除扑克
	if (m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置变量
	m_cbTurnCardType=cbCardType;
	m_cbTurnCardCount=cbCardCount;
	m_cbHandCardCount[wChairID]-=cbCardCount;
	CopyMemory(m_cbTurnCardData,cbCardData,sizeof(BYTE)*cbCardCount);
	CopyMemory(m_cbMagicCardData,cbMagicCardData,sizeof(BYTE)*cbCardCount);

	//胜利判断
	if (m_cbHandCardCount[wChairID]==0) 
	{
		//设置用户
		m_wWinOrder[m_wWinCount++]=wChairID;

		//用户统计
		if (m_cbHandCardCount[(wChairID+2)%GAME_PLAYER]==0) m_wCurrentUser=INVALID_CHAIR;
	}

	//切换用户
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//设置用户
		m_wTurnWiner=wChairID;
		m_wCurrentUser=(wChairID+1)%GAME_PLAYER;

		//用户调整
		while (m_cbHandCardCount[m_wCurrentUser]==0) m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;
	}

	//本轮分数
	m_TurnScore+=m_GameLogic.GetCardScore(cbCardData,cbCardCount);

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbCardCount=cbCardCount;
	OutCard.TurnScore=m_TurnScore;
	OutCard.wCurrentUser=m_wCurrentUser;
	CopyMemory(OutCard.cbCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

	//发送数据
	WORD wHeadSize=sizeof(OutCard)-sizeof(OutCard.cbCardData);
	WORD wSendSize=wHeadSize+OutCard.cbCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,wSendSize);

	//搭档信息
	if ((m_cbHandCardCount[wChairID]==0)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		//构造变量
		CMD_S_CardInfo CardInfo;
		ZeroMemory(&CardInfo,sizeof(CardInfo));

		//设置扑克
		CardInfo.cbCardCount=m_cbHandCardCount[(wChairID+2)%GAME_PLAYER];
		CopyMemory(CardInfo.cbCardData,m_cbHandCardData[(wChairID+2)%GAME_PLAYER],sizeof(BYTE)*CardInfo.cbCardCount);

		//发送数据
		WORD wHeadSize=sizeof(CardInfo)-sizeof(CardInfo.cbCardData);
		m_pITableFrame->SendTableData(wChairID,SUB_S_CARD_INFO,&CardInfo,wHeadSize+CardInfo.cbCardCount*sizeof(BYTE));
		m_pITableFrame->SendLookonData(wChairID,SUB_S_CARD_INFO,&CardInfo,wHeadSize+CardInfo.cbCardCount*sizeof(BYTE));
	}

	//结束判断
	if (m_wCurrentUser==INVALID_CHAIR) OnEventGameEnd(wChairID,NULL,GER_NORMAL);

	return true;
}

//用户放弃
bool CTableFrameSink::OnUserPassCard(WORD wChairID)
{
	//效验状态
	ASSERT((wChairID==m_wCurrentUser)&&(m_cbTurnCardCount!=0));
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return false;

	//用户切换
	do
	{
		//设置用户
		m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;

		//完成判断
		if (m_wCurrentUser==m_wTurnWiner)
		{
			m_PlayerScore[m_wCurrentUser]+=m_TurnScore;
			m_TurnScore=0;

			//设置变量
			m_cbTurnCardCount=0;
			m_cbTurnCardType=CT_ERROR;

			//接风处理
			if (m_cbHandCardCount[m_wTurnWiner]==0)
			{
				m_wCurrentUser=(m_wTurnWiner+2)%GAME_PLAYER;
			}
		}

		//继续处理
		if (m_cbHandCardCount[m_wCurrentUser]!=0) break;

	} while(true);

	//构造消息
	CMD_S_PassCard PassCard;
	PassCard.TurnScore=m_TurnScore;
	CopyMemory(PassCard.PlayerScore,m_PlayerScore,sizeof(m_PlayerScore));
	PassCard.wPassCardUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.cbTurnOver=(m_cbTurnCardCount==0)?TRUE:FALSE;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	return true;
}

//继续游戏
bool CTableFrameSink::OnUserContinueGame(WORD wChairID)
{
	//效验状态
	ASSERT(m_cbContinue[wChairID]==FALSE);
	if (m_cbContinue[wChairID]==TRUE) return true;

	//设置变量
	m_cbContinue[wChairID]=TRUE;

	//用户统计
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_cbContinue[i]==FALSE) break;
	}

	//开始判断
	if (i<GAME_PLAYER)
	{
		//变量定义
		CMD_S_ContinueGame ContinueGame;
		ZeroMemory(&ContinueGame,sizeof(ContinueGame));

		//设置变量
		ContinueGame.wChairID=wChairID;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CONTINUE_GAME,&ContinueGame,sizeof(ContinueGame));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CONTINUE_GAME,&ContinueGame,sizeof(ContinueGame));
	}
	else
	{
		//开始游戏
		OnEventGameStart();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
