#include "StdAfx.h"
#include "TableFrameSink.h"
#include <math.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_FullReady;	//开始模式

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_wSiceCount=MAKEWORD(1,1);
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//出牌信息
	m_bOutCardCount=0;
	ZeroMemory(m_wOutCardUser,sizeof(m_wOutCardUser));
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));

	//发牌信息
	m_cbSendCardData=0;
	m_bSendCardCount=0;
	m_bLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	m_cbKingBrothterPos=0;
	
	//用户状态
	ZeroMemory(m_bReady, sizeof(m_bReady));
//	ZeroMemory(m_bChiHu,sizeof(m_bChiHu));
	for(int i=0; i<4; i++)
	{
		m_bChiHu[i] = true;
	}
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_bUserAction,sizeof(m_bUserAction));
	ZeroMemory(m_bOperateCard,sizeof(m_bOperateCard));
	ZeroMemory(m_bPerformAction,sizeof(m_bPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_bWeaveItemCount,sizeof(m_bWeaveItemCount));

	//杠牌信息
	m_LastGangInfo.Reset();

	//分数信息
	m_lCellScore = 0L;
	m_bCellScoreChange = false;

	//结束信息
	m_HuCourt.Reset();
	ZeroMemory(m_cbBankerContinuCount, sizeof(m_cbBankerContinuCount));
	ZeroMemory(m_lScoreLeft, sizeof(m_lScoreLeft));

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

#ifdef MJ_DESPERATE
	//获取单位积分
	m_lCellScore = m_pGameServiceOption->lCellScore;
	for(int i=0; i<4; i++)
	{
		m_lScoreLeft[i] = m_lCellScore*TIME_SCORE_STAKE_TIMES;
	}
	
#endif

	return true;
}

//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	//游戏变量
	m_wSiceCount=MAKEWORD(1,1);
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//出牌信息
	m_bOutCardCount=0;
	ZeroMemory(m_wOutCardUser,sizeof(m_wOutCardUser));
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));

	//发牌信息
	m_cbSendCardData=0;
	m_bSendCardCount=0;
	m_bLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	
	//用户状态
//	ZeroMemory(m_bChiHu,sizeof(m_bChiHu));
	for(int i=0; i<4; i++)
	{
		m_bChiHu[i] = true;
	}
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_bUserAction,sizeof(m_bUserAction));
	ZeroMemory(m_bOperateCard,sizeof(m_bOperateCard));
	ZeroMemory(m_bPerformAction,sizeof(m_bPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_bWeaveItemCount,sizeof(m_bWeaveItemCount));

	//杠牌信息
	m_LastGangInfo.Reset();

	//结束信息
    m_HuCourt.Reset();
	ZeroMemory(m_cbBankerContinuCount,sizeof(m_cbBankerContinuCount));
	for(i=0; i<4; i++)
	{
		m_lScoreLeft[i] = m_lCellScore*TIME_SCORE_STAKE_TIMES;
	}


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
	m_pITableFrame->SetGameStatus(GS_PLAYING);
	//确保关闭所有等待定时器
	for(int i=0; i<m_wPlayerCount; i++)
	{
		m_pITableFrame->KillGameTimer(i);
	}

	//设置随机
	srand( (unsigned)time( NULL ) );

	//混乱扑克
	m_GameLogic.SetKingCardData(0XFF);
	m_wSiceCount=MAKEWORD(rand()%6+1,rand()%6+1);
	m_bLeftCardCount=CountArray(m_cbRepertoryCard);
	m_GameLogic.RandCardData(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));

	//设置王牌
	m_cbKingBrothterPos = (HIBYTE(m_wSiceCount)+LOBYTE(m_wSiceCount))*4-2;
	BYTE cbKingCardData = m_GameLogic.GetKingFromBrother(m_cbRepertoryCard[m_cbKingBrothterPos]);
	m_GameLogic.SetKingCardData(cbKingCardData);
	//替换王牌
	BYTE cbSum = 0;
	if(cbKingCardData != 0XFF)
	{
		for(int i=0; i<136; i++)
		{
			if(m_cbRepertoryCard[i] == cbKingCardData)
			{
				m_cbRepertoryCard[i] = CARD_KING_DATA_MASK;
				cbSum ++;
			}
		}
	}

	
	//分发扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_bLeftCardCount-=(MAX_COUNT-1);
		m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_bLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);
	}

	//发送扑克
	BYTE cbSendCardData = _DealOneCard();
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(cbSendCardData)]++;

	//设置用户
	m_wCurrentUser=m_wBankerUser;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	////清空
	//for(int i=0; i<MAX_INDEX; i++)
	//{
	//	m_cbCardIndex[m_wBankerUser][i] = 0;
	//}
	//for(i=0; i<3; i++)
	//{
	//	m_cbCardIndex[m_wBankerUser][i] = 3;
	//}
	//m_cbCardIndex[m_wBankerUser][22] = 2; 
	//m_cbCardIndex[m_wBankerUser][30] = 2;
	//m_cbCardIndex[m_wBankerUser][23] = 1;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	//发送扑克
	CMD_S_GameStart GameStart;
	GameStart.wSiceCount=m_wSiceCount;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.cbKingCardData = m_GameLogic.GetKingCardData();
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//设置数据
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData,CountArray(GameStart.cbCardData));
	
		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
#ifndef MJ_DESPERATE
	for(int i=0; i<m_wPlayerCount; i++)
	{
		IServerUserItem * pUserItem = m_pITableFrame->GetServerUserItem(i);
		if(pUserItem)
		{
			m_pITableFrame->SetGameTimer(i, 120000, 1, pUserItem->GetUserID());
		}
	}
#endif
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			return _OnGameEndNormal(wChairID, pIServerUserItem);
		}
	case GER_USER_LEFT:		//用户强退
		{
			return _OnGameEndAbnormal(wChairID, pIServerUserItem);
		}
	case GER_DISMISS:		//解散桌子
		{
			return _OnGameEndDismiss();
		}
	}

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_FREE:		//空闲状态
	case GS_FREE_FAKE:	//伪空闲状态
		{
			//变量定义
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//构造数据
			StatusFree.wBankerUser = m_wBankerUser;
			StatusFree.lCellScore = m_lCellScore;	
			CopyMemory(StatusFree.lLeftScore, m_lScoreLeft, sizeof(m_lScoreLeft));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_PLAYING:
		{
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wSiceCount=m_wSiceCount;
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.lLeftScore, m_lScoreLeft, sizeof(m_lScoreLeft));

			//状态变量
			StatusPlay.cbLeftCardCount=m_bLeftCardCount;
			StatusPlay.cbChihu=(m_bChiHu[wChairID]==true)?TRUE:FALSE;
			StatusPlay.cbResponse=(m_bResponse[wChairID]==true)?TRUE:FALSE;

			//出牌信息
			StatusPlay.cbOutCardCount=m_bOutCardCount;
			CopyMemory(StatusPlay.wOutCardUser,m_wOutCardUser,sizeof(StatusPlay.wOutCardUser));
			CopyMemory(StatusPlay.cbOutCardData,m_cbOutCardData,sizeof(StatusPlay.cbOutCardData));

			//组合扑克
			CopyMemory(StatusPlay.bWeaveCount,m_bWeaveItemCount,sizeof(m_bWeaveItemCount));
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));

			//扑克数据
			StatusPlay.cbKingCardData = m_GameLogic.GetKingCardData();
			StatusPlay.cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID],StatusPlay.cbCardData,CountArray(StatusPlay.cbCardData));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	return false;
}

//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	//处理等待定时器
	if(wTimerID < m_wPlayerCount)
	{
		m_pITableFrame->KillGameTimer(wTimerID);

#ifndef MJ_DESPERATE
		IServerUserItem *pUserItem = m_pITableFrame->GetServerUserItem(wTimerID);
		if(pUserItem && pUserItem->GetUserID() == wBindParam
			&& pUserItem->GetUserStatus() < US_READY)
		{
			m_pITableFrame->SendGameMessage(pUserItem, "由于您2分钟内未点击开始，被请出游戏，欢迎下次光临！",  SMT_CLOSE_GAME|SMT_EJECT);
		}

		return true;
#endif
	}
	else
	{
		switch(wTimerID) 
		{
		case IDI_DECLARE_OVERTIME:
			{
				assert(0 && "有人30秒没有回应！");
				m_pITableFrame->KillGameTimer(IDI_DECLARE_OVERTIME);

				//检查谁没有抉择，模拟消息以保证游戏进行
				for(int i=0; i<4; i++)
				{
					if(m_bResponse[i] == false)
					{
						OnUserOperateCard(i, ACK_NULL, 0);
					}
				}

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
	case SUB_C_OUT_CARD:		//出牌消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
			if (wDataSize!=sizeof(CMD_C_OutCard)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			return OnUserOutCard(pUserData->wChairID,pOutCard->cbCardData);
		}
	case SUB_C_OPERATE_CARD:	//操作消息
		{
			//效验消息
			ASSERT(wDataSize==sizeof(CMD_C_OperateCard));
			if (wDataSize!=sizeof(CMD_C_OperateCard)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_PLAY) return true;

			//消息处理
			CMD_C_OperateCard * pOperateCard=(CMD_C_OperateCard *)pDataBuffer;
			return OnUserOperateCard(pUserData->wChairID,pOperateCard->cbOperateCode,pOperateCard->cbOperateCard);
		}
	case SUB_C_READY:			//READY信号
		{
			//用户效验
			tagServerUserData * pUserData = pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus != US_PLAY && pUserData->cbUserStatus != US_SIT) 
			{
				return true;
			}

			//消息处理
			return OnUserReady(pUserData->wChairID);
		}
	}

	return false;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户同意
bool __cdecl  CTableFrameSink::OnActionUserReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	return  true;
}

//用户坐下
bool __cdecl CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if ((bLookonUser==false)&&(m_wBankerUser==INVALID_CHAIR))
	{
		m_wBankerUser=pIServerUserItem->GetChairID();
	}


#ifndef MJ_DESPERATE
	//最低赌注设置 （慈溪麻将为 最少人的1/50 ）
	if(bLookonUser == false)
	{
		LONG lCellScore = m_lCellScore;
		if(lCellScore == 0)
		{
			lCellScore = pIServerUserItem->GetUserScore()->lScore/50;
		}
		else
		{
			lCellScore = __min(pIServerUserItem->GetUserScore()->lScore/50, lCellScore);
		}
		
		lCellScore = __max(1L, lCellScore);
		if(lCellScore != m_lCellScore)
		{
			m_lCellScore = lCellScore;
			//通知其他玩家
			CMD_S_Stake Msg;
			ZeroMemory(&Msg, sizeof(Msg));
			Msg.lCellScore = m_lCellScore;
			for(WORD i=0; i<m_wPlayerCount; i++)
			{
				if (i!=wChairID && (m_pITableFrame->GetServerUserItem(i)!=0))
				{
					m_pITableFrame->SendTableData(i,SUB_S_STAKE,&Msg,sizeof(Msg));
					m_pITableFrame->SendLookonData(i,SUB_S_STAKE,&Msg,sizeof(Msg));
				}
			}	
		}
		//开等待定时器
		if(pIServerUserItem)
		{
			m_pITableFrame->SetGameTimer(wChairID, 120000,1, pIServerUserItem->GetUserID());
		}

	}
#endif

	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{	
	//庄家设置
	if(bLookonUser == false)
	{
		//清空ready信号
		ZeroMemory(m_bReady, sizeof(m_bReady));
		//清空连庄数目
		ZeroMemory(m_cbBankerContinuCount, sizeof(m_cbBankerContinuCount));
		//重新设置庄家
		if(wChairID == m_wBankerUser)
		{
			m_wBankerUser=INVALID_CHAIR;

			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if ((i!=wChairID)&&(m_pITableFrame->GetServerUserItem(i)!=NULL))
				{
					m_wBankerUser=i;
					break;
				}
			}
		}
	}

#ifndef MJ_DESPERATE
	//最低赌注设置 （慈溪麻将为 最少人的1/50 ）
	if(bLookonUser == false)
	{
		LONG lCellScore = 0;
		bool bHaveUser = false;
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(i!=wChairID)
			{
				IServerUserItem* pIServerUserItem = m_pITableFrame->GetServerUserItem(i);
				if(pIServerUserItem != 0)
				{
					bHaveUser = true;
					if(lCellScore == 0)
					{
						lCellScore = pIServerUserItem->GetUserScore()->lScore/50;
					}
					else
					{
						lCellScore = __min(pIServerUserItem->GetUserScore()->lScore/50, lCellScore);
					}					
				}
			}
		}
		if(bHaveUser)
		{
			lCellScore = __max(1L, lCellScore);
			if(lCellScore != m_lCellScore)
			{
				m_lCellScore = lCellScore;
				//通知其他玩家
				CMD_S_Stake Msg;
				ZeroMemory(&Msg, sizeof(Msg));
				Msg.lCellScore = m_lCellScore;
				for(WORD i=0; i<m_wPlayerCount; i++)
				{
					if (i!=wChairID&&(m_pITableFrame->GetServerUserItem(i)!=0))
					{
						m_pITableFrame->SendTableData(i,SUB_S_STAKE,&Msg,sizeof(Msg));
						m_pITableFrame->SendLookonData(i,SUB_S_STAKE,&Msg,sizeof(Msg));
					}
				}	
			}
		}
		else
		{
			m_lCellScore = 0;
		}	
	}
#endif

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData)
{
	//效验信息
	if (wChairID != m_wCurrentUser)
	{
		return false;
	}
	if (m_GameLogic.IsValidCommonCard(cbCardData) == false)
	{
		return false;
	}

	//删除扑克
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置变量
	m_bOutCardCount++;
	m_bChiHu[wChairID]=true;
	m_wOutCardUser[m_bOutCardCount-1]=wChairID;
	m_cbOutCardData[m_bOutCardCount-1]=cbCardData;

	//响应判断
	bool bAroseAction=false;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (i!=wChairID)
		{
			m_bUserAction[i]=ACK_NULL;
#ifdef MJ_DESPERATE
			//冲击麻将限制吃碰一个玩家最多两次
			BYTE cbOperateSum = 0;
			for(WORD j=0; j<m_bWeaveItemCount[i]; j++)
			{
				if(m_WeaveItemArray[i][j].wProvideUser == wChairID)
				{
					cbOperateSum ++;
				}
			}
			if(cbOperateSum < 2)
			{
#endif
				//碰牌判断
				m_bUserAction[i] |= m_GameLogic.GetPengCardKind(m_cbCardIndex[i],cbCardData);

				//吃牌判断
				WORD wEatUser=(wChairID+m_wPlayerCount-1)%m_wPlayerCount;
				if (wEatUser == i) 
				{
					m_bUserAction[i]|=m_GameLogic.GetEatCardKind(m_cbCardIndex[i],cbCardData);
				}
				//杠判断
				if (m_bLeftCardCount > 1)
				{
					m_bUserAction[i]|=m_GameLogic.GetGangCardKind(m_cbCardIndex[i],cbCardData);
				}
#ifdef MJ_DESPERATE
			}
#endif
			//公胡判断
			if (m_bChiHu[i] == true)
			{
				tagHuInput HuInput;
				HuInput.bJudgeHuOnly		= true;
				HuInput.bJudgeKingWait		= false;
				HuInput.bInsertCurrentCard	= true;
				HuInput.cbCurrentCard		= cbCardData;
				HuInput.pWeaveItemSeq		= m_WeaveItemArray[i];
				HuInput.cbWeaveItemCount	= m_bWeaveItemCount[i];
				HuInput.pCardIndexSeq		= m_cbCardIndex[i];
				tagHuCourt HuCourt;
				if(m_GameLogic.GetHuInfo(&HuInput, HuCourt))
				{
					m_bUserAction[i] |= ACK_CHIHU;
					m_bChiHu[i] = false;
				}
			}

			//设置变量
			if (m_bUserAction[i]!=ACK_NULL)
			{
				bAroseAction=true;
				m_bResponse[i]=false;
				m_bPerformAction[i]=ACK_NULL;

				
			}
			else
			{
				m_bResponse[i]=true;
				m_bUserAction[i]=ACK_NULL;
				m_bPerformAction[i]=ACK_NULL;
			}

		}
		else
		{
			m_bResponse[i]=true;
			m_bUserAction[i]=ACK_NULL;
			m_bPerformAction[i]=ACK_NULL;
		}
	}

	//切换处理
	BYTE cbSendCard=0;
	if ((bAroseAction==false)&&(m_bLeftCardCount>0))
	{
		//发牌
		m_bSendCardCount++;
		m_wCurrentUser=(wChairID+m_wPlayerCount-1)%m_wPlayerCount;
		cbSendCard=m_cbSendCardData=_DealOneCard();
		m_cbCardIndex[m_wCurrentUser][m_GameLogic.SwitchToCardIndex(cbSendCard)]++;
	}
	else 	
	{
		m_wCurrentUser=INVALID_CHAIR;

		m_pITableFrame->SetGameTimer(IDI_DECLARE_OVERTIME, 30000, DWORD(-1), 0);
	}

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;
	OutCard.cbCurrentCard=cbSendCard;
	OutCard.wCurrentUser=m_wCurrentUser;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	//荒庄结束
	if ((bAroseAction==false)&&(m_wCurrentUser==INVALID_CHAIR))
	{
		m_HuCourt.Reset();
		m_bLeftCardCount=0;
		OnEventGameEnd(INVALID_CHAIR, NULL,GER_NORMAL);
	}

	return true;
}

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard)
{
	//当前用户
	if (m_wCurrentUser==wChairID)
	{
		//效验操作
		ASSERT(cbOperateCode!=ACK_NULL);
		if (cbOperateCode==ACK_NULL) 
		{
			return false;
		}

		//执行动作
		switch (cbOperateCode)
		{
		case ACK_GANG:		//暗杠操作
			{
				//杠牌效验
				ASSERT((m_bLeftCardCount>1)&&(m_GameLogic.IsValidCommonCard(cbOperateCard)==true));
				if ((m_bLeftCardCount==1)||(m_GameLogic.IsValidCommonCard(cbOperateCard)==false)) 
				{
					return false;
				}

				//杠牌分析
				BYTE cbCardIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard);
				BYTE cbWeaveIndex = 0XFF;
				if (m_cbCardIndex[m_wCurrentUser][cbCardIndex]!=4)
				{
					//效验数目
					ASSERT(m_cbCardIndex[m_wCurrentUser][cbCardIndex]==1);
					if (m_cbCardIndex[m_wCurrentUser][cbCardIndex]!=1) //要么是暗杠，要么是起明杠
					{
						return false;
					}

					//寻找组合
					for (BYTE i=0;i<m_bWeaveItemCount[m_wCurrentUser];i++)
					{
						BYTE cbWeaveKind=m_WeaveItemArray[m_wCurrentUser][i].cbWeaveKind;
						BYTE cbCenterCard=m_WeaveItemArray[m_wCurrentUser][i].cbCenterCard;
						if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==ACK_PENG))
						{
							cbWeaveIndex=i;
							break;
						}
					}
					
					//效验动作
					ASSERT(cbWeaveIndex!=0xFF);
					if (cbWeaveIndex==0xFF) 
					{
						return false;
					}

					//组合扑克
					//m_WeaveItemArray[m_wCurrentUser][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[m_wCurrentUser][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[m_wCurrentUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
				}
				else
				{
					//设置变量
					cbWeaveIndex=m_bWeaveItemCount[m_wCurrentUser]++;
					m_WeaveItemArray[m_wCurrentUser][cbWeaveIndex].cbWeaveKind=ACK_GANG;
					m_WeaveItemArray[m_wCurrentUser][cbWeaveIndex].wProvideUser=m_wCurrentUser;
				}

				//效验动作
				ASSERT(cbWeaveIndex!=0xFF);
				if (cbWeaveIndex==0xFF) 
				{
					return false;
				}

				//删除扑克
				m_cbCardIndex[m_wCurrentUser][cbCardIndex]=0;

				//补牌操作
				m_bSendCardCount++;
				m_cbSendCardData = _DealOneCard(false);
				m_cbCardIndex[m_wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

				//杠牌信息
				m_LastGangInfo.cbGangCardData		= cbOperateCard;
				m_LastGangInfo.wPerformUser			= m_wCurrentUser;
				m_LastGangInfo.wProvideUser			= m_WeaveItemArray[m_wCurrentUser][cbWeaveIndex].wProvideUser;
				m_LastGangInfo.cbSendData			= m_cbSendCardData;
				m_LastGangInfo.cbCurrentOutCount	= m_bOutCardCount;

				//构造消息
				CMD_S_OperateCard OperateCard;
				OperateCard.wProvideUser=wChairID;
				OperateCard.wOperateUser=m_wCurrentUser;
				OperateCard.wCurrentUser=m_wCurrentUser;
				OperateCard.cbOperateCard=cbOperateCard;
				OperateCard.cbOperateCode=cbOperateCode;
				OperateCard.cbCurrentCard=m_cbSendCardData;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_CARD,&OperateCard,sizeof(OperateCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_CARD,&OperateCard,sizeof(OperateCard));

				return true;
			}
		case ACK_CHIHU:		//胡牌操作
			{
				//普通胡牌
				tagHuInput HuInput;
				HuInput.bJudgeHuOnly		= false;
				HuInput.bJudgeKingWait		= true;
				HuInput.bInsertCurrentCard	= false;
				HuInput.cbCurrentCard		= m_cbSendCardData;
				HuInput.pWeaveItemSeq		= m_WeaveItemArray[wChairID];
				HuInput.cbWeaveItemCount	= m_bWeaveItemCount[wChairID];
				HuInput.pCardIndexSeq		= m_cbCardIndex[wChairID];
				tagHuCourt HuCourt;
				if(m_GameLogic.GetHuInfo(&HuInput, HuCourt))
				{
					//判断附加胡 自摸
					HuCourt.wHuAttachedType |= EN_HU_ATTACHED_ZIMO;
					//判断附加胡 天地胡
					if ((m_bOutCardCount == 0) && ( m_bSendCardCount == 0))
					{
						HuCourt.wHuAttachedType |= EN_HU_ATTACHED_TIANDI;
					}
					//判断杠上花
					if(m_wCurrentUser == m_LastGangInfo.wPerformUser
						&& m_cbSendCardData == m_LastGangInfo.cbSendData
						&& m_bOutCardCount == m_LastGangInfo.cbCurrentOutCount)
					{
						HuCourt.wHuAttachedType |= EN_HU_ATTACHED_GANG_FLOWER;
					}

				}
				//结果效验
				ASSERT(HuCourt.IsHu());
				if(HuCourt.IsHu() == false)
				{
					return false;
				}
				//结束信息
				HuCourt.wWinChair = m_wCurrentUser;
				HuCourt.wFuseChair = m_wCurrentUser;
				HuCourt.cbHuCardData = m_cbSendCardData;
				m_HuCourt = HuCourt;
	
				//结束游戏
				OnEventGameEnd(HuCourt.wFuseChair,NULL,GER_NORMAL);
				return true;
			}
		}

		//错误断言
		ASSERT(FALSE);

		return false;
	}

	//其他用户
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//效验状态
		//ASSERT((m_bResponse[wChairID]==false)&&((cbOperateCode==ACK_NULL)||(m_bUserAction[wChairID]&cbOperateCode)!=0));
		if ((m_bResponse[wChairID]==true)||((cbOperateCode!=ACK_NULL)&&(m_bUserAction[wChairID]&cbOperateCode)==0)) 
		{
			return true;
		}

		//设置变量
		m_bResponse[wChairID]=true;
		m_bOperateCard[wChairID]=cbOperateCard;
		m_bPerformAction[wChairID]=cbOperateCode;

		//执行判断
		WORD wTargetUser=wChairID;
		BYTE cbTargetAction=cbOperateCode;

		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//获取动作
			BYTE cbUserAction=ACK_NULL;
			if (m_bResponse[i]==false)
			{
				cbUserAction=m_bUserAction[i];
				if (m_bUserAction[i]&ACK_CHIHU)
				{
					cbUserAction&=ACK_CHIHU;
				}
				else if (m_bUserAction[i]&ACK_GANG) 
				{
					cbUserAction&=ACK_GANG;
				}
				else if (m_bUserAction[i]&ACK_PENG) 
				{
					cbUserAction&=ACK_PENG;
				}
				else if (m_bUserAction[i]&(ACK_RIGHT|ACK_CENTER|ACK_LEFT)) 
				{
					m_bUserAction[i]&=(ACK_RIGHT|ACK_CENTER|ACK_LEFT);
				}
			}
			else 
			{
				cbUserAction = m_bPerformAction[i];
				if (cbUserAction&ACK_GANG) 
				{
					cbUserAction=ACK_GANG;
				}
			}

			//动作判断
			if (cbUserAction>cbTargetAction)
			{
				wTargetUser=i;
				cbTargetAction=cbUserAction;
			}
		}
		if (m_bResponse[wTargetUser]==false) 
		{
			return true;
		}

		//吃胡延时(一炮多响）
		//慈溪麻将中无一炮多响，按顺序优先胡牌
		if (m_bPerformAction[wTargetUser]==ACK_CHIHU)
		{
			ASSERT(m_bOutCardCount > 0);
			WORD wOutCardChair = m_wOutCardUser[m_bOutCardCount-1];
			ASSERT(wOutCardChair != wTargetUser);
			
			WORD wTurnChair = ((wOutCardChair+m_wPlayerCount-1)%m_wPlayerCount);			
			while(wTurnChair != wTargetUser)
			{			
				if ((m_bResponse[wTurnChair]==false)&&(m_bUserAction[wTurnChair]&ACK_CHIHU))
				{
					return true;
				}
				wTurnChair = ((wTurnChair+m_wPlayerCount-1)%m_wPlayerCount);
			}

		}

		//放弃操作
		if (cbTargetAction==ACK_NULL)
		{

			//放弃首胡
			if ((m_bOutCardCount==0)&&(m_bSendCardCount==0))
			{
				//切换用户
				m_wCurrentUser=m_wBankerUser;

				//构造消息
				CMD_S_OperateCard OperateCard;
				OperateCard.cbCurrentCard=0;
				OperateCard.cbOperateCard=0;
				OperateCard.cbOperateCode=ACK_NULL;
				OperateCard.wOperateUser=INVALID_CHAIR;
				OperateCard.wProvideUser=INVALID_CHAIR;
				OperateCard.wCurrentUser=m_wCurrentUser;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_CARD,&OperateCard,sizeof(OperateCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_CARD,&OperateCard,sizeof(OperateCard));
			}
			else if (m_bLeftCardCount>1)
			{
				//设置变量
				m_bSendCardCount++;
				m_cbSendCardData= _DealOneCard();
				m_wCurrentUser=(m_wOutCardUser[m_bOutCardCount-1]+m_wPlayerCount-1)%m_wPlayerCount;
				m_cbCardIndex[m_wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

				//构造消息
				CMD_S_SendCard SendCard;
				SendCard.cbCardData=m_cbSendCardData;
				SendCard.wCurrentUser=m_wCurrentUser;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
			}
			else
			{
				//荒庄结束
				m_bLeftCardCount=0;
				m_HuCourt.Reset();				
				OnEventGameEnd(m_HuCourt.wFuseChair, NULL,GER_NORMAL);
			}

			//抉择结束，复位抉择变量
			for(i=0; i<m_wPlayerCount; i++)
			{
				m_bResponse[i]=true;
				m_bUserAction[i]=ACK_NULL;
				m_bPerformAction[i]=ACK_NULL;
			}

			m_pITableFrame->KillGameTimer(IDI_DECLARE_OVERTIME);

			return true;
		}

		//吃胡操作
		if (cbTargetAction==ACK_CHIHU)
		{
			//结束信息
			BYTE cbHuCardData =(m_bOutCardCount>0)?m_cbOutCardData[m_bOutCardCount-1]:0;
			ASSERT(cbHuCardData > 0);
			WORD wProvideUser=(m_bOutCardCount>0)?m_wOutCardUser[m_bOutCardCount-1]:INVALID_CHAIR;
			ASSERT(wProvideUser != INVALID_CHAIR);

			//胡牌判断 （优先胡)
			WORD wTurnChair = ((wProvideUser+m_wPlayerCount-1)%m_wPlayerCount);			
			while(wTurnChair != wProvideUser)
			{
				if ((m_bResponse[wTurnChair])&&(m_bUserAction[wTurnChair]&ACK_CHIHU))
				{
					break;
				}
				wTurnChair = ((wTurnChair+m_wPlayerCount-1)%m_wPlayerCount);
			}

			ASSERT(wTurnChair != wProvideUser);

			//普通胡牌
			tagHuInput HuInput;
			HuInput.bJudgeHuOnly		= false;
			HuInput.bJudgeKingWait		= false;
			HuInput.bInsertCurrentCard	= true;
			HuInput.cbCurrentCard		= cbHuCardData;
			HuInput.pWeaveItemSeq		= m_WeaveItemArray[wTurnChair];
			HuInput.cbWeaveItemCount	= m_bWeaveItemCount[wTurnChair];
			HuInput.pCardIndexSeq		= m_cbCardIndex[wTurnChair];
			tagHuCourt HuCourt;
			if(m_GameLogic.GetHuInfo(&HuInput, HuCourt))
			{
				//判断附加胡 天地胡
				if ((m_bOutCardCount == 0) && ( m_bSendCardCount == 0))
				{
					HuCourt.wHuAttachedType |= EN_HU_ATTACHED_TIANDI;
				}
			}
			//效验数据
			ASSERT(HuCourt.IsHu());
			if(HuCourt.IsHu() == false)	//诈胡
			{
				return  false;
			}

			//结束信息
			HuCourt.wWinChair = wTurnChair;
			HuCourt.wFuseChair = wProvideUser;
			HuCourt.cbHuCardData = cbHuCardData;
			m_HuCourt = HuCourt;

			//结束游戏
			OnEventGameEnd(HuCourt.wFuseChair,NULL,GER_NORMAL);

			//抉择结束，复位抉择变量
			for(i=0; i<m_wPlayerCount; i++)
			{
				m_bResponse[i]=true;
				m_bUserAction[i]=ACK_NULL;
				m_bPerformAction[i]=ACK_NULL;
			}


			m_pITableFrame->KillGameTimer(IDI_DECLARE_OVERTIME);
			return true;
		}

		//抉择结束，复位抉择变量
		for(i=0; i<m_wPlayerCount; i++)
		{
			m_bResponse[i]=true;
			m_bUserAction[i]=ACK_NULL;
			m_bPerformAction[i]=ACK_NULL;
		}

		m_pITableFrame->KillGameTimer(IDI_DECLARE_OVERTIME);

		//调整参数
		if (cbTargetAction&ACK_GANG) 
		{
			cbTargetAction=ACK_GANG;
		}
		cbOperateCard = (m_bOutCardCount==0)?m_bOperateCard[wTargetUser]:m_cbOutCardData[m_bOutCardCount-1];

		//组合扑克
		ASSERT(m_bWeaveItemCount[wTargetUser]<4);
		WORD wIndex=m_bWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind=cbTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser=(m_bOutCardCount>0)?m_wOutCardUser[m_bOutCardCount-1]:wTargetUser;

		//删除扑克
		switch (cbTargetAction)
		{
		case ACK_LEFT:		//上牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbOperateCard+1,cbOperateCard+2};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;
			}
		case ACK_RIGHT:		//上牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbOperateCard-2,cbOperateCard-1};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;
			}
		case ACK_CENTER:	//上牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbOperateCard-1,cbOperateCard+1};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;
			}
		case ACK_PENG:		//碰牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[]={cbOperateCard,cbOperateCard};
				m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));

				break;
			}
		case ACK_GANG:		//杠牌操作
			{
				//删除扑克
				if (m_bOutCardCount==0)
				{
					BYTE cbRemoveCard[]={cbOperateCard,cbOperateCard,cbOperateCard,cbOperateCard};
					m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));
				}
				else
				{
					BYTE cbRemoveCard[]={cbOperateCard,cbOperateCard,cbOperateCard};
					m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard));
				}

				//杠牌信息
				m_LastGangInfo.cbGangCardData		= cbOperateCard;
				m_LastGangInfo.wPerformUser			= wTargetUser;
				m_LastGangInfo.wProvideUser			= m_WeaveItemArray[wTargetUser][wIndex].wProvideUser;
				m_LastGangInfo.cbCurrentOutCount	= m_bOutCardCount;

				break;
			}
		}

		//补牌操作
		BYTE cbCurrentCard=0;
		if (cbTargetAction==ACK_GANG)
		{
			m_bSendCardCount++;
			cbCurrentCard = m_cbSendCardData = _DealOneCard(false);
			m_cbCardIndex[wTargetUser][m_GameLogic.SwitchToCardIndex(cbCurrentCard)]++;

			//杠牌信息
			m_LastGangInfo.cbSendData			= m_cbSendCardData;
		}

		//设置变量
		m_wCurrentUser=wTargetUser;
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_bUserAction,sizeof(m_bUserAction));
		ZeroMemory(m_bOperateCard,sizeof(m_bOperateCard));
		ZeroMemory(m_bPerformAction,sizeof(m_bPerformAction));

		//构造消息
		CMD_S_OperateCard OperateCard;
		OperateCard.wOperateUser=wTargetUser;
		OperateCard.wCurrentUser=m_wCurrentUser;
		OperateCard.cbCurrentCard=cbCurrentCard;
		OperateCard.cbOperateCard=cbOperateCard;
		OperateCard.cbOperateCode=cbTargetAction;
		OperateCard.wProvideUser=(m_bOutCardCount>0)?m_wOutCardUser[m_bOutCardCount-1]:wTargetUser;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_CARD,&OperateCard,sizeof(OperateCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_CARD,&OperateCard,sizeof(OperateCard));

		return true;
	}

	//错误断言 (碰了之后，吃的消息还可能发过来）
//	ASSERT(FALSE);

	return true;
}


bool CTableFrameSink::_OnGameEndNormal(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//清空ready信号
	ZeroMemory(m_bReady, sizeof(m_bReady));
	//变量定义
	CMD_S_GameEnd GameEnd;
	ZeroMemory(&GameEnd,sizeof(GameEnd));
	GameEnd.stHuCourt.Reset();

	//荒庄处理
	if (m_HuCourt.wFuseChair == INVALID_CHAIR)
	{
		//荒庄处理
		m_wBankerUser=(m_wBankerUser+1)%m_wPlayerCount;
		//拷贝扑克
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			const BYTE cbMaxCount=CountArray(GameEnd.cbCardData[i]);
			GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i],cbMaxCount);
		}

#ifdef MJ_DESPERATE
		//剩余财富
		CopyMemory(GameEnd.lLeftScore, m_lScoreLeft, sizeof(m_lScoreLeft));
		//ready类型
		GameEnd.cbSelfReady = 1;
		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

		//设置伪空闲状态
		m_pITableFrame->SetGameStatus(GS_FREE_FAKE);
		//复位变量
		_OnFakeOver();

		return true;
#else

		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
		//结束游戏
		m_pITableFrame->ConcludeGame();

		return true;
#endif
	}
	//统计积分
	else
	{
#ifdef MJ_DESPERATE	

		//处理全风向 剩下人全部赢光光
		if(m_HuCourt.wHuCardQueueType & EN_HU_CARD_QUEUE_WW)
		{
			//庄家设置 (赢家连庄)
			if(m_wBankerUser == m_HuCourt.wWinChair)
			{
				m_cbBankerContinuCount[m_wBankerUser] ++;
				m_HuCourt.cbBankerCount = m_cbBankerContinuCount[m_wBankerUser];
			}
			else
			{
				//闲家胡，算庄家连庄数目+1
				m_HuCourt.cbBankerCount = m_cbBankerContinuCount[m_wBankerUser] + 1;

				//换庄，清空连庄数目
				ZeroMemory(m_cbBankerContinuCount, sizeof(m_cbBankerContinuCount));
				m_wBankerUser = (m_wBankerUser+1)%m_wPlayerCount;
			}				
			
			//全部输完
			for(WORD i=0; i<GAME_PLAYER; i++)
			{
				if(i == m_HuCourt.wWinChair)
				{
					continue;
				}
				GameEnd.lGameScore[i] = -m_lScoreLeft[i];
			}
		}
		else

#endif
		
		{
			//计算分数
			LONG lGain = 0, lTimes = 0;
			m_GameLogic.CalScore(m_HuCourt, lGain, lTimes);

			//庄家设置 (赢家连庄)
			if(m_wBankerUser == m_HuCourt.wWinChair)
			{
				m_cbBankerContinuCount[m_wBankerUser] ++;
				m_HuCourt.cbBankerCount = m_cbBankerContinuCount[m_wBankerUser];
			}
			else
			{
				//闲家胡，算庄家连庄数目+1
				m_HuCourt.cbBankerCount = m_cbBankerContinuCount[m_wBankerUser] + 1;

				//换庄，清空连庄数目
				ZeroMemory(m_cbBankerContinuCount, sizeof(m_cbBankerContinuCount));
				m_wBankerUser = (m_wBankerUser+1)%m_wPlayerCount;
			}				

			if(m_HuCourt.cbBankerCount > 0)
			{
				lGain += (m_HuCourt.cbBankerCount-1) * 2;
			}			

			//包牌处理
			_CheckAfford(m_HuCourt);
			LONG lAffordWeight = 1;
			bool bHaveAfford = false;

			switch(m_HuCourt.AffordItem[0].cbAffordType) 
			{
			case EN_AFFORD_ACCEPT3:		//求三包牌
				bHaveAfford = true;
				lAffordWeight = 4;
				lGain += LONG_AFFORD_ADD_ACCEPT3;
				lTimes += LONG_AFFORD_TIME_ACCEPT3;
				break;
			case EN_AFFORD_GIVE3:		//供三包牌
				bHaveAfford = true;
				lAffordWeight = 4;
				lGain += LONG_AFFORD_ADD_GIVE3;
				lTimes += LONG_AFFORD_TIME_GIVE3;
				break;
			case EN_AFFORD_GANG_APPEND:	//杠补包牌
				bHaveAfford = true;
				lAffordWeight = 4;
				lGain += LONG_AFFORD_ADD_GANG_APPEND;
				lTimes += LONG_AFFORD_TIME_GANG_APPEND;
				break;
			case EN_AFFORD_HIDE_WIND:	//藏风包牌
				bHaveAfford = true;
				lAffordWeight = 4;
				lGain += LONG_AFFORD_ADD_HIDE_WIND;
				lTimes += LONG_AFFORD_TIME_HIDE_WIND;
				break;
			}
			
			LONG lWindScore = lGain * m_lCellScore * ((LONG)pow(2, lTimes)) * lAffordWeight;

			//加减分数
			if(bHaveAfford)
			{
				GameEnd.lGameScore[m_HuCourt.AffordItem[0].wChair] -= lWindScore;
			}
			else
			//普通胡
			{
				//自摸
				if(m_HuCourt.IsZimo())
				{
					for(WORD i=0; i<m_wPlayerCount; i++)
					{
						if(i == m_HuCourt.wWinChair)
						{
							continue;
						}
						GameEnd.lGameScore[i] -= lWindScore;
					}
				}
				//炮胡
				else
				{
					for(WORD i=0; i<m_wPlayerCount; i++)
					{
						if(i == m_HuCourt.wWinChair)
						{
							continue;
						}
						if(i == m_HuCourt.wFuseChair)
						{
							GameEnd.lGameScore[i] -= lWindScore*2;
						}
						else
						{
							GameEnd.lGameScore[i] -= lWindScore;
						}						
					}
				}
			}
		}
	}

	//胡牌详情
	GameEnd.stHuCourt = m_HuCourt;

	//拷贝扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		const BYTE cbMaxCount=CountArray(GameEnd.cbCardData[i]);
		GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i],cbMaxCount);
	}

	//变量定义
	IServerUserItem* pUserItem = 0;
	TCHAR szMsg[256];
	bool bHaveLostAll = false;
	bool bLostAll[4];
	ZeroMemory(bLostAll, sizeof(bLostAll));

#ifdef MJ_DESPERATE

	//调整分数 (加分处理)
	GameEnd.lGameScore[m_HuCourt.wWinChair] = 0;
	for(i=0; i<GAME_PLAYER; i++)
	{
		if(i == m_HuCourt.wWinChair)
		{
			continue;
		}
		LONG lScoreLeft = m_lScoreLeft[i];
		lScoreLeft += GameEnd.lGameScore[i];
		if(lScoreLeft <= 0)
		{			
			bHaveLostAll = true;
			bLostAll[i] = true;
			GameEnd.lGameScore[i] = -m_lScoreLeft[i];
			m_lScoreLeft[i] = 0;
		}
		else
		{
			m_lScoreLeft[i] = lScoreLeft;
		}
		GameEnd.lGameScore[m_HuCourt.wWinChair] -= GameEnd.lGameScore[i];  
	}
	m_lScoreLeft[m_HuCourt.wWinChair] += GameEnd.lGameScore[m_HuCourt.wWinChair];

	//其他信息
	CopyMemory(GameEnd.lLeftScore, m_lScoreLeft, sizeof(m_lScoreLeft));
	//特殊开始标记
	if(bHaveLostAll == false)
	{
		GameEnd.cbSelfReady = 1;
	}

	//发送信息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

	//写积分
	tagScoreInfo ScoreInfo[GAME_PLAYER];
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		ScoreInfo[i].lScore = GameEnd.lGameScore[i];
		if (GameEnd.lGameScore[i]==0L) 
		{
			ScoreInfo[i].ScoreKind=enScoreKind_Draw;
		}
		else 
		{
			ScoreInfo[i].ScoreKind=(GameEnd.lGameScore[i]>0L)?enScoreKind_Win:enScoreKind_Lost;
		}
	}
	m_pITableFrame->WriteTableScore(ScoreInfo,m_wPlayerCount,0L);

	//是否有人输光
	if(bHaveLostAll)
	{
		TCHAR szUserName[128];
		szUserName[0] = 0;
		//取输光玩家名字
		for(WORD i=0; i<GAME_PLAYER; i++)
		{	
			pUserItem = m_pITableFrame->GetServerUserItem(i);
			if(pUserItem == 0)
			{
				ASSERT(0 && "取玩家信息出错");
				continue;
			}

			if(bLostAll[i])	
			{	
				strcat(szUserName, _T(" ["));
				strcat(szUserName, pUserItem->GetAccounts());
				strcat(szUserName, _T("] "));
			}
			
		}
		TCHAR szTmp[64];
		sprintf(szTmp, "已经输光%d银子，欢迎下次光临！", m_pGameServiceOption->lLessScore);
		szMsg[0] = 0;
		strcat(szMsg, _T("游戏结束！玩家"));
		strcat(szMsg, szUserName);
		strcat(szMsg, szTmp);

		//通知玩家
		for(i=0; i<GAME_PLAYER; i++)
		{
			pUserItem = m_pITableFrame->GetServerUserItem(i);
			if(pUserItem == 0)
			{
				ASSERT(0 && "取玩家信息出错");
				continue;
			}

			m_pITableFrame->SendGameMessage(pUserItem, szMsg,  SMT_CLOSE_GAME|SMT_EJECT);
		}

		//通知旁观者
		WORD wIndex=0;
		do
		{
			pUserItem = m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pUserItem == 0) 
			{
				break;
			}
			m_pITableFrame->SendGameMessage(pUserItem,szMsg, SMT_CLOSE_GAME|SMT_EJECT);
		} while (true);

		//结束游戏
		m_pITableFrame->ConcludeGame();

	}
	else
	{
		//设置伪空闲状态
		m_pITableFrame->SetGameStatus(GS_FREE_FAKE);
		//复位变量
		_OnFakeOver();
	}

#else

	//调整分数 (加分处理)
	GameEnd.lGameScore[m_HuCourt.wWinChair] = 0;
	for(i=0; i<GAME_PLAYER; i++)
	{
		if(i == m_HuCourt.wWinChair)
		{
			continue;
		}
		pUserItem = m_pITableFrame->GetServerUserItem(i);
		if(pUserItem)
		{
			if(GameEnd.lGameScore[i] <= -pUserItem->GetUserScore()->lScore)
			{
				bHaveLostAll = true;
				bLostAll[i] = true;
				GameEnd.lGameScore[i] = -pUserItem->GetUserScore()->lScore;
			}
		}
		
		GameEnd.lGameScore[m_HuCourt.wWinChair] -= GameEnd.lGameScore[i];  
	}

	//发送信息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));


	//写分
	for(int i=0;i<GAME_PLAYER;i++)
	{
		LONG lScore=GameEnd.lGameScore[i];
		LONG lRevenue=0;
		enScoreKind	ScoreKind;
		if(lScore>0)
		{
			ScoreKind=enScoreKind_Win;
			if(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
				lRevenue=lScore*m_pGameServiceOption->wRevenue/100;
			lScore-=lRevenue;
		}
		else if(lScore==0) 	ScoreKind=enScoreKind_Draw;
		else				ScoreKind=enScoreKind_Lost;
		
		m_pITableFrame->WriteUserScore(i,lScore,lRevenue,ScoreKind);
	}

	//是否有人输光
	if(bHaveLostAll)
	{
		TCHAR szUserName[128];
		szUserName[0] = 0;
		//取输光玩家名字
		for(WORD i=0; i<GAME_PLAYER; i++)
		{	
			pUserItem = m_pITableFrame->GetServerUserItem(i);
			if(pUserItem == 0)
			{
				ASSERT(0 && "取玩家信息出错");
				continue;
			}

			if(bLostAll[i])	
			{	
				strcat(szUserName, _T(" ["));
				strcat(szUserName, pUserItem->GetAccounts());
				strcat(szUserName, _T("] "));
			}

		}
		
		//通知玩家
		for(i=0; i<GAME_PLAYER; i++)
		{
			pUserItem = m_pITableFrame->GetServerUserItem(i);
			if(pUserItem == 0)
			{
				ASSERT(0 && "取玩家信息出错");
				continue;
			}
			if(bLostAll[i])
			{
				TCHAR szTmp[64];
				sprintf(szTmp, "已经输光银子，欢迎下次光临！");
				szMsg[0] = 0;
				strcat(szMsg, _T("游戏结束！玩家"));
				strcat(szMsg, szUserName);
				strcat(szMsg, szTmp);
				m_pITableFrame->SendGameMessage(pUserItem, szMsg,  SMT_CLOSE_GAME|SMT_EJECT);
			}
			else
			{
				TCHAR szTmp[64];
				sprintf(szTmp, "已经输光银子，退出游戏！");
				szMsg[0] = 0;
				strcat(szMsg, _T("玩家"));
				strcat(szMsg, szUserName);
				strcat(szMsg, szTmp);
				m_pITableFrame->SendGameMessage(pUserItem, szMsg,  SMT_INFO);
			}			
		}

		TCHAR szTmp[64];
		sprintf(szTmp, "已经输光银子，退出游戏！");
		szMsg[0] = 0;
		strcat(szMsg, _T("玩家"));
		strcat(szMsg, szUserName);
		strcat(szMsg, szTmp);

		//通知旁观者
		WORD wIndex=0;
		do
		{
			pUserItem = m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pUserItem == 0) 
			{
				break;
			}
			m_pITableFrame->SendGameMessage(pUserItem,szMsg, SMT_INFO);
		} while (true);
	}

	//结束游戏
	m_pITableFrame->ConcludeGame();

#endif

	return true;
}

bool CTableFrameSink::_OnGameEndAbnormal(WORD wChairID, IServerUserItem * pIServerUserItem)
{	
	//清空ready信号
	ZeroMemory(m_bReady, sizeof(m_bReady));

	//通知消息
	TCHAR szMessage[128]=TEXT("");
	_snprintf(szMessage,CountArray(szMessage),TEXT("由于 [ %s ] 离开游戏，游戏结束！"),pIServerUserItem->GetAccounts());
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

	//更改积分
	tagScoreInfo ScoreInfo;
	ScoreInfo.lScore = -10*m_lCellScore;
	if(pIServerUserItem)
	{
		if(ScoreInfo.lScore <= -pIServerUserItem->GetUserScore()->lScore)
		{
			ScoreInfo.lScore = -pIServerUserItem->GetUserScore()->lScore;
		}
	}
	ScoreInfo.ScoreKind = enScoreKind_Flee;


	//变量定义
	CMD_S_GameEnd GameEnd;
	ZeroMemory(&GameEnd,sizeof(GameEnd));
	GameEnd.stHuCourt.Reset();
	GameEnd.lGameScore[wChairID] = ScoreInfo.lScore;

	m_pITableFrame->WriteUserScore(wChairID,GameEnd.lGameScore[wChairID],0L,enScoreKind_Flee);

	//拷贝扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		const BYTE cbMaxCount=CountArray(GameEnd.cbCardData[i]);
		GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i],cbMaxCount);
	}

	//发送信息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

	//结束游戏
	m_pITableFrame->ConcludeGame();

	return true;

}

bool CTableFrameSink::_OnGameEndDismiss()
{
	//清空ready信号
	ZeroMemory(m_bReady, sizeof(m_bReady));
	//变量定义
	CMD_S_GameEnd GameEnd;
	ZeroMemory(&GameEnd,sizeof(GameEnd));
	GameEnd.stHuCourt.Reset();

	//拷贝扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		const BYTE cbMaxCount=CountArray(GameEnd.cbCardData[i]);
		GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i],cbMaxCount);
	}

	//发送信息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
	//结束游戏
	m_pITableFrame->ConcludeGame();

	//通知消息
	TCHAR szMessage[128]=TEXT("");
	_snprintf(szMessage,CountArray(szMessage),TEXT("由于出现意外情况，管理员解散桌子，游戏结束！"));
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		IServerUserItem * pISendUserItem=m_pITableFrame->GetServerUserItem(i);
		if (pISendUserItem!=NULL) m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_CLOSE_GAME|SMT_EJECT);
	}
	WORD wIndex=0;
	do
	{
		IServerUserItem * pISendUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
		if (pISendUserItem==NULL) break;
		m_pITableFrame->SendGameMessage(pISendUserItem,szMessage,SMT_CLOSE_GAME|SMT_EJECT);
	} while (true);

	return true;

}

//检查包牌
BYTE CTableFrameSink::_CheckAfford(tagHuCourt& HuCourt)
{
	//效验输入
	if(HuCourt.IsHu() == false || m_bOutCardCount == 0)
	{
		return 0;
	}
	
	//包牌处理
	BYTE cbAffordCount = 0;

#ifdef MJ_DESPERATE

	//藏风包牌
	if(m_HuCourt.wHuCardQueueType == EN_HU_CARD_QUEUE_13A
		&& m_GameLogic.IsWindCard(m_HuCourt.cbHuCardData))
	{
		if(m_HuCourt.cbHuCardData != m_cbSendCardData)
		{
			bool bKeepWind = true;
			for(int i=0; i<m_bOutCardCount; i++)
			{
				if(m_wOutCardUser[i] == m_HuCourt.wFuseChair)
				{
					if(m_GameLogic.IsWindCard(m_cbOutCardData[i]) == false)
					{
						bKeepWind = false;
						break;
					}
				}
			}
			if(bKeepWind == false)
			{
				HuCourt.AffordItem[0].cbAffordType = EN_AFFORD_HIDE_WIND;
				HuCourt.AffordItem[0].wChair = HuCourt.wFuseChair;
			}
		}
	}

	if(HuCourt.AffordItem[0].wChair == INVALID_CHAIR)
	{
		//杠上花包牌
		if(m_HuCourt.wFuseChair == m_LastGangInfo.wPerformUser
			&& m_HuCourt.cbHuCardData == m_LastGangInfo.cbSendData
			&& (m_bOutCardCount-1) == m_LastGangInfo.cbCurrentOutCount)
		{
			HuCourt.AffordItem[0].cbAffordType = EN_AFFORD_GANG_APPEND;
			HuCourt.AffordItem[0].wChair = HuCourt.wFuseChair;
		}
	}	

#else

	//求三包牌
	BYTE cbCount[4] = {0,0,0,0};
	for(BYTE i=0; i<m_bWeaveItemCount[m_HuCourt.wWinChair]; i++)
	{
		cbCount[m_WeaveItemArray[m_HuCourt.wWinChair][i].wProvideUser] ++;
	}
	for(i=0; i<sizeof(cbCount); i++)
	{
		if(i == m_HuCourt.wWinChair)
		{
			continue;
		}
		if(cbCount[i] >= 3)
		{
			HuCourt.AffordItem[0].cbAffordType = EN_AFFORD_ACCEPT3;
			HuCourt.AffordItem[0].wChair = i;
			break;
		}
	}
	
	if(HuCourt.AffordItem[0].wChair == INVALID_CHAIR)
	{
		//供三包牌
		ZeroMemory(cbCount, sizeof(cbCount));
		for(i=0; i<m_wPlayerCount; i++)
		{
			if(i == m_HuCourt.wWinChair)
			{
				continue;
			}
			for(BYTE j=0; j<m_bWeaveItemCount[i]; j++)
			{
				if(m_WeaveItemArray[i][j].wProvideUser == m_HuCourt.wWinChair)
				{
					cbCount[i] ++;
				}
			}
		}
		for(i=0; i<sizeof(cbCount); i++)
		{
			if(i == m_HuCourt.wWinChair)
			{
				continue;
			}
			if(cbCount[i] >= 3)
			{
				HuCourt.AffordItem[0].cbAffordType = EN_AFFORD_GIVE3;
				HuCourt.AffordItem[0].wChair = i;
			}
		}
	}	

#endif

	return 1;
}

//ready信号
bool CTableFrameSink::OnUserReady(WORD wChairID)
{
	m_bReady[wChairID] = true;
	CMD_S_UserReady Msg;
	Msg.wChairID = wChairID;
	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_READY,&Msg,sizeof(Msg));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_READY,&Msg,sizeof(Msg));

	//检查开始信号
	for(BYTE i=0; i<4; i++)
	{
		if(m_bReady[i] == false)
		{
			return true;
		}
	}

	//开始游戏
	OnEventGameStart();

	return true;
}

//虚拟结束
void CTableFrameSink::_OnFakeOver()
{
	//游戏变量
	m_wSiceCount=MAKEWORD(1,1);
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	//出牌信息
	m_bOutCardCount=0;
	ZeroMemory(m_wOutCardUser,sizeof(m_wOutCardUser));
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));

	//发牌信息
	m_cbSendCardData=0;
	m_bSendCardCount=0;
	m_bLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

	//用户状态
	//	ZeroMemory(m_bChiHu,sizeof(m_bChiHu));
	for(int i=0; i<4; i++)
	{
		m_bChiHu[i] = true;
	}
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_bUserAction,sizeof(m_bUserAction));
	ZeroMemory(m_bOperateCard,sizeof(m_bOperateCard));
	ZeroMemory(m_bPerformAction,sizeof(m_bPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_bWeaveItemCount,sizeof(m_bWeaveItemCount));

	//杠牌信息
	m_LastGangInfo.Reset();

	//结束信息
	m_HuCourt.Reset();
}

//发一张牌
BYTE CTableFrameSink::_DealOneCard(bool bHead)
{
	m_bLeftCardCount --;
	//跳过财神弟弟的位置
	if(bHead)
	{		
		if(m_bLeftCardCount == m_cbKingBrothterPos)
		{
			m_bLeftCardCount --;
			BYTE cbKingBrother = m_GameLogic.GetBrotherFromKing(m_GameLogic.GetKingCardData());
			ASSERT(cbKingBrother == m_cbRepertoryCard[m_bLeftCardCount+1]);
		}

		return (m_cbRepertoryCard[m_bLeftCardCount]);
	}
	else
	{
		static bool bEven = true;
		BYTE cbCardData = 0XFF;
		if(bEven)
		{			
			if(m_cbKingBrothterPos == 1)
			{
				m_cbKingBrothterPos = 0XFF;
				m_bLeftCardCount --;
				cbCardData = m_cbRepertoryCard[0];
				for(int i=0; i<m_bLeftCardCount; i++)
				{
					m_cbRepertoryCard[i] = m_cbRepertoryCard[i+2];
				}				
			}
			else
			{
				m_cbKingBrothterPos --;
				cbCardData = m_cbRepertoryCard[1];
				for(int i=1; i<m_bLeftCardCount; i++)
				{
					m_cbRepertoryCard[i] = m_cbRepertoryCard[i+1];
				}				
			}			
		}
		else
		{
			cbCardData = m_cbRepertoryCard[0];
			for(int i=0; i<m_bLeftCardCount; i++)
			{
				m_cbRepertoryCard[i] = m_cbRepertoryCard[i+1];
			}	
		}

		bEven = !bEven;
		return cbCardData;
	}	
}

//////////////////////////////////////////////////////////////////////////
