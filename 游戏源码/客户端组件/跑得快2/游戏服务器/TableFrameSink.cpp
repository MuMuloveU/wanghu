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
	TRACE("CTableFrameSink::RepositTableFrameSink");
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
	TRACE("OnEventGameStart");
	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);
	int nIndex=m_GameLogic.StartANewGame();
	for(int i=0;i<m_wPlayerCount;i++)
	{
		struct tagSetInitPisa tInit;
		m_GameLogic.GetInitPisa(i,&tInit);
		//初始化每个玩家的牌
//		m_pITableFrame->SendUserData(pUser,MSG_SET_INITPISA,(void *)&tInit,sizeof(struct tagSetInitPisa));
		m_pITableFrame->SendTableData(i,MSG_SET_INITPISA,(void *)&tInit,sizeof(struct tagSetInitPisa));
		m_pITableFrame->SendLookonData(i,MSG_SET_INITPISA,(void *)&tInit,sizeof(struct tagSetInitPisa));
	}
	//设置有黑桃3的人先出牌
	struct tagSetFocus focus;
	focus.bClean=TRUE;
	focus.nFoucsId=nIndex;
	SendUserDataToAll(MSG_SET_FOCUS,(void *)&focus,sizeof(focus));
	//清零最后的出牌记录
	m_GameLogic.EmptyLastCardInfo();
	return true;
}

//游戏结束 wChairID为最先出完的人
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			struct tagGameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));

			//变量定义
			LONG lCellScore=m_pGameServiceOption->lCellScore;
			tagScoreInfo ScoreInfo[m_wPlayerCount];
			memset(&ScoreInfo,0,sizeof(ScoreInfo));

			//统计积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (i!=wChairID)
				{
					int LeftCard=m_GameLogic.GetPlayerLeftCard(i);
					GameEnd.lGameScore[i]-=LeftCard*lCellScore;
					ScoreInfo[i].lScore-=LeftCard*lCellScore;
					GameEnd.lGameScore[wChairID]+=LeftCard*lCellScore;
					ScoreInfo[i].lScore += LeftCard*lCellScore;
				}
			}

			//计算税收
			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{
				if (GameEnd.lGameScore[wChairID]>=100L)
				{
					LONG lTax=GameEnd.lGameScore[wChairID]*m_pGameServiceOption->wRevenue/100L;
					GameEnd.lGameTax+=lTax;
					ScoreInfo[wChairID].lRevenue=lTax;
					GameEnd.lGameScore[wChairID]=GameEnd.lGameScore[wChairID]-lTax;
				}
			}

			//发送信息
			SendUserDataToAll(MSG_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,MSG_GAME_END,&GameEnd,sizeof(GameEnd));

			for (WORD i=0;i<m_wPlayerCount;i++)
			{				
				ScoreInfo[i].ScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
				m_pITableFrame->WriteUserScore(i,ScoreInfo[i].lScore,ScoreInfo[i].lRevenue,ScoreInfo[i].ScoreKind);
			}
			//m_pITableFrame->WriteTableScore(ScoreInfo,m_wPlayerCount,GameEnd.lGameTax);

			//结束游戏
			m_pITableFrame->ConcludeGame();
			return true;
		}
	case GER_USER_LEFT:		//用户强退
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//构造数据
			struct tagGameEnd GameEnd;
			memset(&GameEnd,0,sizeof(GameEnd));
			GameEnd.lGameScore[wChairID]=-20*m_pGameServiceOption->lCellScore;//逃跑扣20倍积分

			//发送信息
			SendUserDataToAll(MSG_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,MSG_GAME_END,&GameEnd,sizeof(GameEnd));

			//修改积分
			tagScoreInfo ScoreInfo;
			memset(&ScoreInfo,0,sizeof(ScoreInfo));
			ScoreInfo.ScoreKind=enScoreKind_Draw;
			ScoreInfo.lScore=GameEnd.lGameScore[wChairID];
			m_pITableFrame->WriteUserScore(wChairID,ScoreInfo.lScore,0L,ScoreInfo.ScoreKind);

			//结束游戏
			m_pITableFrame->ConcludeGame();
			return true;
		}
	}
	ASSERT(FALSE);
	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret)
{
	//TRACE("CTableFrameSink::SendGameScene:ChairID:%d,GameStatus:%d",wChiarID,bGameStatus);
	return m_pITableFrame->SendGameScene(pIServerUserItem,NULL,0);
	//效验结果
	//ASSERT(FALSE);
	return true;
	//return false;
}

//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	TRACE("CTableFrameSink::OnGameMessage,SubCmdID:%d",wSubCmdID);
	//玩家提交的pass或者出牌操作
	if (wSubCmdID==MSG_SET_OUTPISA)
	{
		ASSERT(wDataSize==sizeof(struct tagSetOutPisa));
		if (wDataSize!=sizeof(struct tagSetOutPisa)) return false;
		struct tagSetOutPisa *pOut=(struct tagSetOutPisa *)pDataBuffer;
		ASSERT(pOut!=NULL);
		pOut->nChairId=pIServerUserItem->GetChairID();
		TRACE("收到用户：%d的出牌消息\n",pOut->nChairId);

		//如果玩家pass
		if (pOut->nCount==0)
		{
			//如果是玩家开始出的牌，玩家不允许pass
			if (m_GameLogic.IsUserBegin(pOut->nChairId))
			{
				//Pass出错，返回出错信息
				struct tagOutPisaMsg out;
				memset(&out,0,sizeof(out));
				out.nResult=1;
				strcpy(out.chErrMsg,"不能Pass自己出的牌！");
				m_pITableFrame->SendUserData(pIServerUserItem,MSG_OUTPISA_MSG,(void *)&out,sizeof(out));
			}
			else
			{
				//通知各个玩家该玩家pass
				struct tagSetOtherPisa pisa;
				pisa.nChairId=pOut->nChairId;
				pisa.nCount=0;
				SendUserDataToAll(MSG_SET_OTHERPISA,(void *)&pisa,sizeof(pisa));
				//设置下一个玩家为焦点
				int index=(pOut->nChairId+1)%m_wPlayerCount;
				TRACE("设置下一个%d为焦点\n",index);
				struct tagSetFocus focus;
				//如果已经Pass轮回到发牌者
				focus.bClean=m_GameLogic.IsUserBegin(index)?TRUE:FALSE;
				focus.nFoucsId=index;
				SendUserDataToAll(MSG_SET_FOCUS,(void *)&focus,sizeof(focus));
			}
		}
		else
		{
			if (m_GameLogic.IsUserBegin(pOut->nChairId))
				m_GameLogic.EmptyLastCardInfo();
			if (m_GameLogic.IsBiggerThanLastCard(pOut))
			{
				//通知玩家出牌通过
				struct tagOutPisaMsg out;
				memset(&out,0,sizeof(out));
				out.nResult=0;
				m_pITableFrame->SendUserData(pIServerUserItem,MSG_OUTPISA_MSG,(void *)&out,sizeof(out));
				//通知各个玩家出了什么牌
				struct tagSetOtherPisa pisa;
				pisa.nChairId=pOut->nChairId;
				pisa.nCount=pOut->nCount;
				for(int i=0;i<pOut->nCount;i++)
					pisa.tOutPisa[i]=pOut->tOutPisa[i];
				SendUserDataToAll(MSG_SET_OTHERPISA,(void *)&pisa,sizeof(pisa));
				//更新最后出牌信息
				if (!m_GameLogic.SaveLastCard(pOut))
				{
					//表示该玩家已经出完他的牌了,完成游戏
					OnEventGameEnd(pOut->nChairId,NULL,GER_NORMAL);
				}
				else
				{
					//设置下一个玩家为焦点
					int index=(pOut->nChairId+1)%m_wPlayerCount;
					TRACE("设置下一个%d为焦点\n",index);
					struct tagSetFocus focus;
					focus.bClean=FALSE;
					focus.nFoucsId=index;
					SendUserDataToAll(MSG_SET_FOCUS,(void *)&focus,sizeof(focus));
				}
			}
			else
			{
				//出牌出错，返回出错信息
				struct tagOutPisaMsg out;
				memset(&out,0,sizeof(out));
				out.nResult=1;
				strncpy(out.chErrMsg,m_GameLogic.GetCompareError(),sizeof(out.chErrMsg)-1);
				m_pITableFrame->SendUserData(pIServerUserItem,MSG_OUTPISA_MSG,(void *)&out,sizeof(out));
			}
		}
		return true;
	}
	else
	{
		TRACE("Warning:receive other gamemessage.");
	}
	return true;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//m_GameLogic.StartANewGame();
	//TRACE("CTableFrameSink::OnFrameMessage:SubCmdID:%d",wSubCmdID);
	//return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////

//向所有玩家发送的消息
void CTableFrameSink::SendUserDataToAll(WORD wSubCmdId,void *pBuffer,int nLen)
{
	m_pITableFrame->SendTableData(INVALID_CHAIR,wSubCmdId,pBuffer,nLen);
//	IServerUserItem *pUserItem;
/*	for(int i=0;i<m_wPlayerCount;i++)
	{
		//pUserItem=m_pITableFrame->GetServerUserItem(i);
		//ASSERT(pUserItem!=NULL);
		//m_pITableFrame->SendUserData(pUserItem,wSubCmdId,pBuffer,nLen);
		m_pITableFrame->SendTableData(i,wSubCmdId,pBuffer,nLen);
	}*/
}
