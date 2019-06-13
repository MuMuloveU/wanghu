#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//游戏定时器
#define IDI_START_GAME				200									//开始定时器
#define IDI_OUT_CARD				201									//出牌定时器
#define IDI_OPERATE_CARD			202									//操作定时器

//#define GENDER_GIRL					0									//性别女

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_CONSIGN_CHANGED,OnConsignChanged)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_CARD_OPERATE,OnCardOperate)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg()
:	CGameFrameDlg(&m_GameClientView),
	m_GameClientView(this)
{
	//游戏变量
	m_wHeapHand=0;
	m_wHeapTail=0;
	m_bChiHu=true;
	m_bLeftCardCount=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_lScoreLeft, sizeof(m_lScoreLeft));
	m_lCellScore = 0;

	//出牌信息
	m_bOutCardCount=0;
	ZeroMemory(m_wOutCardUser,sizeof(m_wOutCardUser));
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));

	//组合扑克
	ZeroMemory(m_bWeaveCount,sizeof(m_bWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bHeapCardInfo,sizeof(m_bHeapCardInfo));

	//属性变量
	m_bSelfReady = false;
	ZeroMemory(m_bReady, sizeof(m_bReady));
	m_cbOverTimeCount	= 0;
	m_bConsign			= false;

	m_wSiceCount = 0;

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
#ifdef MJ_DESPERATE
	//设置标题
	SetWindowText(TEXT("冲击麻将  --  Ver：6. 0. 9. 0"));
#else
	SetWindowText(TEXT("丹阳麻将  --  Ver：6. 5. 1. 0"));
#endif
	

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//加载资源
	g_CardResource.LoadResource();

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//环境设置
	KillGameTimer(IDI_START_GAME);

	//游戏变量
	m_wHeapHand=0;
	m_wHeapTail=0;
	m_bChiHu=true;
	m_bLeftCardCount=0;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_lScoreLeft, sizeof(m_lScoreLeft));
	m_lCellScore = 0;

	//出牌信息
	m_bOutCardCount=0;
	ZeroMemory(m_wOutCardUser,sizeof(m_wOutCardUser));
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));

	//组合扑克
	ZeroMemory(m_bWeaveCount,sizeof(m_bWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bHeapCardInfo,sizeof(m_bHeapCardInfo));

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound = IsEnableSound();
	GameOption.m_bAllowLookon = IsAllowLookOn();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置配置
		EnableSound(GameOption.m_bEnableSound);
		AllowUserLookon(0,GameOption.m_bAllowLookon);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				OnStart(0,0);
				return false;
			}
			if (nElapse<=5) 
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_OUT_CARD:			//用户出牌
		{
			//超时判断
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
				{
					BYTE cbCardData = m_GameClientView.m_HandCardControl.GetCurrentCard();
#ifdef _AUTOPALY

					 cbCardData = GetIsolatedCard();
					if(cbCardData == 0xFF)
					{
						cbCardData = m_GameClientView.m_HandCardControl.GetCurrentCard();
					}
#endif
					if(cbCardData == CARD_KING_DATA_MASK)
					{
						return false;
					}
					OnOutCard(cbCardData,cbCardData);

#ifndef _AUTOPALY
					//设置托管
					if(IsConsign() == false)
					{
						m_cbOverTimeCount ++;
						if(m_cbOverTimeCount >= 3)
						{
							SetConsign(true);							
						}
						else
						{
							CString strTmp;
							strTmp.Format("您第%d次出牌超时，超时大于两次后系统将自动托管！", m_cbOverTimeCount);
							InsertSystemString(strTmp);
						}
					}
#endif
				}
				return false;
			}

			//播放声音
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) 
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_OPERATE_CARD:		//操作定时器
		{
			//超时判断
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
				{
					OnCardOperate(ACK_NULL,0);
				}
				return false;
			}

			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) 
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	}

	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:			//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_OUT_CARD:			//用户出牌
		{
			return OnSubOutCard(pBuffer,wDataSize);
		}
	case SUB_S_OPERATE_CARD:		//操作扑克
		{
			return OnSubOperateCard(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:			//发送扑克
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:			//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_SYS_MSG:				//系统消息
		{
			return OnSubSysMsg(pBuffer, wDataSize);
		}
	case SUB_S_USER_READY:			//准备信号
		{
			return OnSubUserReady(pBuffer, wDataSize);
		}
	case SUB_S_STAKE:				//注额改变
		{
			return OnSubStake(pBuffer, wDataSize);
		}
	}

	return true;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_FREE:		//空闲状态
	case GS_FREE_FAKE:
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//fake一个金币设置消息
			CMD_S_Stake Msg;
			Msg.lCellScore = pStatusFree->lCellScore;
			CopyMemory(Msg.lLeftScore, pStatusFree->lLeftScore, sizeof(Msg.lLeftScore));
			OnSubStake(&Msg, sizeof(CMD_S_Stake));

			//设置数据
			m_wBankerUser=pStatusFree->wBankerUser;
			m_GameClientView.m_HandCardControl.SetDisplayItem(true);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_bHeapCardInfo[i][0]=0;
				m_bHeapCardInfo[i][1]=0;
				m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_bHeapCardInfo[i][0],m_bHeapCardInfo[i][1],HEAP_FULL_COUNT);
			}

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				m_GameClientView.m_btConsignEnter.ShowWindow(SW_SHOW);
				m_GameClientView.m_btConsignCancel.ShowWindow(SW_SHOW);

				SetGameTimer(GetMeChairID(),IDI_START_GAME,30);
			}

			//更新界面
			m_GameClientView.UpdateGameView(NULL);

			return true;
		}
	case GS_PLAYING:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//fake一个金币设置消息
			CMD_S_Stake Msg;
			Msg.lCellScore = pStatusPlay->lCellScore;
			CopyMemory(Msg.lLeftScore, pStatusPlay->lLeftScore, sizeof(Msg.lLeftScore));
			OnSubStake(&Msg, sizeof(CMD_S_Stake));

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_bOutCardCount=pStatusPlay->cbOutCardCount;
			m_bLeftCardCount=pStatusPlay->cbLeftCardCount;
			m_bChiHu=(pStatusPlay->cbChihu>0)?true:false;
			CopyMemory(m_wOutCardUser,pStatusPlay->wOutCardUser,sizeof(m_wOutCardUser));
			CopyMemory(m_cbOutCardData,pStatusPlay->cbOutCardData,sizeof(m_cbOutCardData));
			CopyMemory(m_bWeaveCount,pStatusPlay->bWeaveCount,sizeof(m_bWeaveCount));
			CopyMemory(m_WeaveItemArray,pStatusPlay->WeaveItemArray,sizeof(m_WeaveItemArray));
			m_GameLogic.SetKingCardData(pStatusPlay->cbKingCardData);
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,m_cbCardIndex);

			//辅助变量
			WORD wViewChairID[GAME_PLAYER]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++) wViewChairID[i]=SwitchViewChairID(i);

			//界面设置
			m_GameClientView.SetBankerUser(wViewChairID[m_wBankerUser]);

			//组合扑克
			BYTE cbGangCount=0;
			BYTE cbWeaveCard[4]={0,0,0,0};
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wOperateViewID = SwitchViewChairID(i);
				for (BYTE j=0;j<m_bWeaveCount[i];j++)
				{
					BYTE cbWeaveKind=m_WeaveItemArray[i][j].cbWeaveKind;
					BYTE cbCenterCard=m_WeaveItemArray[i][j].cbCenterCard;
					BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCardData(cbWeaveKind,cbCenterCard,cbWeaveCard);
					m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetCardData(cbWeaveCard,cbWeaveCardCount);
					if (cbWeaveKind==ACK_GANG)
					{
						cbGangCount++;
						if (m_WeaveItemArray[i][j].wProvideUser==i) m_GameClientView.m_WeaveCard[wViewChairID[i]][j].SetDisplayItem(false);
					}
					WORD wProviderViewID = SwitchViewChairID(m_WeaveItemArray[i][j].wProvideUser);
					m_GameClientView.m_WeaveCard[wOperateViewID][j].SetDirectionCardPos(3-(wOperateViewID-wProviderViewID+4)%4);
				}
			}

			//用户扑克
			if (m_wCurrentUser==GetMeChairID())
			{
				BYTE cbCardCount=pStatusPlay->cbCardCount;
				//m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,cbCardCount-1,pStatusPlay->cbCardData[cbCardCount-cbCardCount]);
				SetHandCardData(pStatusPlay->cbCardData,cbCardCount-1,pStatusPlay->cbCardData[cbCardCount-1]);
			}
			else 
			{
				//m_GameClientView.m_HandCardControl.SetCardData(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,0); 
				SetHandCardData(pStatusPlay->cbCardData,pStatusPlay->cbCardCount,0); 
			}

			//用户扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (i!=GetMeChairID())
				{
					BYTE cbCardCount=13-m_bWeaveCount[i]*3;
					WORD wUserCardIndex=(wViewChairID[i]<2)?wViewChairID[i]:2;
					m_GameClientView.m_UserCard[wUserCardIndex].SetCardData(cbCardCount,(m_wCurrentUser==i));
				}
			}

			//桌面扑克
			for (WORD i=0;i<m_bOutCardCount;i++)
			{
				WORD wOutUser=m_wOutCardUser[i];
				BYTE cbCardData=m_cbOutCardData[i];
				m_GameClientView.m_DiscardCard[wViewChairID[wOutUser]].AddCardItem(cbCardData);
			}

			//控制设置
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayItem(true);
			}

			//堆立扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_bHeapCardInfo[i][0]=0;
				m_bHeapCardInfo[i][1]=0;
			}

			//分发扑克
			m_wSiceCount = pStatusPlay->wSiceCount;
			BYTE bTakeCount=136-m_bLeftCardCount-cbGangCount;
			//BYTE bSiceFirst=HIBYTE(pGameStart->wSiceCount);
			//BYTE bSiceSecond=LOBYTE(pGameStart->wSiceCount);
			BYTE bSiceCount = HIBYTE(pStatusPlay->wSiceCount) + LOBYTE(pStatusPlay->wSiceCount);
			//WORD wTakeChairID=(m_wBankerUser+7-bSiceFirst)%GAME_PLAYER;
			WORD wTakeChairID = (m_wBankerUser + 3*bSiceCount - 3 - (bSiceCount-1)/GAME_PLAYER*GAME_PLAYER*3)%GAME_PLAYER;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//计算数目
				//BYTE bValidCount=HEAP_FULL_COUNT-m_bHeapCardInfo[wTakeChairID][1]-((i==0)?(bSiceSecond-1)*2:0);
				BYTE bValidCount=HEAP_FULL_COUNT-m_bHeapCardInfo[wTakeChairID][1]-((i==0)?(bSiceCount)*2:0);
				BYTE bRemoveCount=__min(bValidCount,bTakeCount);

				//提取扑克
				bTakeCount-=bRemoveCount;
				m_bHeapCardInfo[wTakeChairID][(i==0)?1:0]+=bRemoveCount;

				//完成判断
				if (bTakeCount==0)
				{
					m_wHeapHand=wTakeChairID;
					//m_wHeapTail=(m_wBankerUser+7-bSiceFirst)%GAME_PLAYER;
					m_wHeapTail = (m_wBankerUser + 3*bSiceCount - 3 - (bSiceCount-1)/GAME_PLAYER*GAME_PLAYER*3)%GAME_PLAYER;
					break;
				}

				//切换索引
				wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
			}

			//杠牌补牌
			for (BYTE i=0;i<cbGangCount;i++) DeductionTableCard(false);

			//设置财神的弟弟
			m_wKingBrotherHeapPos = (m_wHeapTail+3)%4;
			m_wKingBrotherCount = bSiceCount;
			WORD wHeapViewID=SwitchViewChairID(m_wKingBrotherHeapPos);
			for(i=0; i<GAME_PLAYER; i++)
			{
				if(i == wHeapViewID)
				{
					m_GameClientView.m_HeapCard[i].SetKingBrotherCount(m_wKingBrotherCount);
				}
				else
				{
					m_GameClientView.m_HeapCard[i].SetKingBrotherCount(0);
				}
			}
	
			//堆立界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_HeapCard[wViewChairID[i]].SetCardData(m_bHeapCardInfo[i][0],m_bHeapCardInfo[i][1],HEAP_FULL_COUNT);
			}
			
			//时间设置
			if ((m_wCurrentUser==INVALID_CHAIR)&&(IsLookonMode()==false)&&(m_bLeftCardCount>1))
			{
				//其他时候
				ASSERT(m_bOutCardCount > 0);
				if ((m_bOutCardCount>0)&&(pStatusPlay->cbResponse==FALSE))
				{
					//变量定义
					WORD wMeChairID=GetMeChairID();
					BYTE cbOutCardData = m_cbOutCardData[m_bOutCardCount-1];
					WORD wOutCardUser = m_wOutCardUser[m_bOutCardCount-1];

					//状态判断
					BYTE cbActionMask=m_GameLogic.GetPengCardKind(m_cbCardIndex,cbOutCardData);
					if (m_bLeftCardCount>1)
					{
						cbActionMask|=m_GameLogic.GetGangCardKind(m_cbCardIndex,cbOutCardData);
					}

					//吃牌判断
					WORD wEatUser=(wOutCardUser+GAME_PLAYER-1)%GAME_PLAYER;
					if (wEatUser == wMeChairID) 
					{
						cbActionMask|=m_GameLogic.GetEatCardKind(m_cbCardIndex,cbOutCardData);
					}

					//公胡判断
					if (m_bChiHu==true)
					{
						tagHuInput HuInput;
						HuInput.bJudgeHuOnly		= true;
						HuInput.bJudgeKingWait		= false;
						HuInput.bInsertCurrentCard	= true;
						HuInput.cbCurrentCard		= cbOutCardData;
						HuInput.pWeaveItemSeq		= m_WeaveItemArray[wMeChairID];
						HuInput.cbWeaveItemCount	= m_bWeaveCount[wMeChairID];
						HuInput.pCardIndexSeq		= m_cbCardIndex;
						tagHuCourt HuCourt;
						if(m_GameLogic.GetHuInfo(&HuInput, HuCourt))
						{
							cbActionMask |= ACK_CHIHU;
							m_bChiHu = false;
						}
						
					}

					//控制界面
					if (cbActionMask!=ACK_NULL)
					{

						SetGameTimer(wMeChairID,IDI_OPERATE_CARD,10);
						m_GameClientView.m_ControlWnd.SetControlInfo(cbOutCardData,cbActionMask,NULL);
					}
					else
					{
						InsertSystemString("OnGameSceneMessage : 竟然没有判断到");
					}
				}
				else
				{
					CString str;
					str.Format("OnGameSceneMessage : m_bOutCardCount == %d, pStatusPlay->cbResponse == %d", m_bOutCardCount, pStatusPlay->cbResponse);
					InsertSystemString(str);
				}
			}
			if (m_wCurrentUser!=INVALID_CHAIR) 
			{
				SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,20);
			}

			//出牌设置
			if (m_bOutCardCount>0)
			{
				WORD wOutUser=m_wOutCardUser[m_bOutCardCount-1];
				BYTE cbOutCardData=m_cbOutCardData[m_bOutCardCount-1];
				m_GameClientView.SetOutCardInfo(wViewChairID[wOutUser],cbOutCardData);
			}

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btConsignEnter.ShowWindow(SW_SHOW);
				m_GameClientView.m_btConsignCancel.ShowWindow(SW_SHOW);
			}

			//更新界面
			m_GameClientView.UpdateGameView(NULL);

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//复位变量
	ZeroMemory(m_bReady, sizeof(m_bReady));

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置数据
	m_bChiHu=true;
	m_bOutCardCount=0;
	m_bLeftCardCount=55;
	m_wBankerUser=pGameStart->wBankerUser;
	m_wCurrentUser=pGameStart->wCurrentUser;

	//设置数据
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bWeaveCount,sizeof(m_bWeaveCount));

	ZeroMemory(m_wOutCardUser,sizeof(m_wOutCardUser));
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_bHeapCardInfo,sizeof(m_bHeapCardInfo));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//设置扑克
	m_GameLogic.SetKingCardData(pGameStart->cbKingCardData);
	BYTE cbCardCount=(GetMeChairID()==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,cbCardCount,m_cbCardIndex);

	//设置界面
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(bPlayerMode);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	//旁观界面
	if (bPlayerMode==false)
	{
		m_GameClientView.SetStatusFlag(false,false);
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);
		m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	}

	//堆立扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_bHeapCardInfo[i][0]=0;
		m_bHeapCardInfo[i][1]=0;
	}

	//分发扑克
	m_wSiceCount = pGameStart->wSiceCount;
	BYTE bTakeCount=53;
	//BYTE bSiceFirst=HIBYTE(pGameStart->wSiceCount);
	//BYTE bSiceSecond=LOBYTE(pGameStart->wSiceCount);
	BYTE bSiceCount = HIBYTE(pGameStart->wSiceCount) + LOBYTE(pGameStart->wSiceCount);
	//WORD wTakeChairID=(m_wBankerUser+7-bSiceFirst)%GAME_PLAYER;
	WORD wTakeChairID = (m_wBankerUser + 3*bSiceCount - 3 - (bSiceCount-1)/GAME_PLAYER*GAME_PLAYER*3)%GAME_PLAYER;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//计算数目
		//BYTE bValidCount=HEAP_FULL_COUNT-m_bHeapCardInfo[wTakeChairID][1]-((i==0)?(bSiceSecond-1)*2:0);
		BYTE bValidCount=HEAP_FULL_COUNT-m_bHeapCardInfo[wTakeChairID][1]-((i==0)?(bSiceCount)*2:0);
		BYTE bRemoveCount=__min(bValidCount,bTakeCount);

		//提取扑克
		bTakeCount-=bRemoveCount;
		m_bHeapCardInfo[wTakeChairID][(i==0)?1:0]+=bRemoveCount;

		//完成判断
		if (bTakeCount==0)
		{
			m_wHeapHand=wTakeChairID;
			//m_wHeapTail=(m_wBankerUser+7-bSiceFirst)%GAME_PLAYER;
			m_wHeapTail = (m_wBankerUser + 3*bSiceCount - 3 - (bSiceCount-1)/GAME_PLAYER*GAME_PLAYER*3)%GAME_PLAYER;
			break;
		}

		//切换索引
		wTakeChairID=(wTakeChairID+1)%GAME_PLAYER;
	}

	//设置财神的弟弟
	m_wKingBrotherHeapPos = (m_wHeapTail+3)%4;
	m_wKingBrotherCount = bSiceCount;
	WORD wHeapViewID=SwitchViewChairID(m_wKingBrotherHeapPos);
	for(i=0; i<GAME_PLAYER; i++)
	{
		if(i == wHeapViewID)
		{
			m_GameClientView.m_HeapCard[i].SetKingBrotherCount(m_wKingBrotherCount);
		}
		else
		{
			m_GameClientView.m_HeapCard[i].SetKingBrotherCount(0);
		}
	}
	

	//CString str;
	//str.Format("Pos == %d, Count == %d , Banker == %d, bSiceFirst == %d, biSiceSecond == %d", 
	//	m_wKingBrotherHeapPos, m_wKingBrotherCount, m_wBankerUser, bSiceFirst, bSiceSecond);
	//AfxMessageBox(str);

	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);

		//堆立界面
		m_GameClientView.m_WeaveCard[i][0].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][1].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][2].SetDisplayItem(true);
		m_GameClientView.m_WeaveCard[i][3].SetDisplayItem(true);
		m_GameClientView.m_HeapCard[wViewChairID].SetCardData(m_bHeapCardInfo[i][0],m_bHeapCardInfo[i][1],HEAP_FULL_COUNT);

		//用户扑克
		if (wViewChairID!=2)
		{
			WORD wIndex=(wViewChairID>=3)?2:wViewChairID;
			m_GameClientView.m_UserCard[wIndex].SetCardData(CountArray(pGameStart->cbCardData),(i==m_wBankerUser));
		}
		else
		{
			BYTE cbBankerCard=(i==m_wBankerUser)?pGameStart->cbCardData[MAX_COUNT-1]:0;
			//m_GameClientView.m_HandCardControl.SetCardData(pGameStart->cbCardData,MAX_COUNT-1,cbBankerCard);
			SetHandCardData(pGameStart->cbCardData,MAX_COUNT-1,cbBankerCard);
		}

		//旁观界面
		if (bPlayerMode==false)
		{
			m_GameClientView.m_TableCard[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_DiscardCard[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][0].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][1].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][2].SetCardData(NULL,0);
			m_GameClientView.m_WeaveCard[wViewChairID][3].SetCardData(NULL,0);
		}
	}

	//玩家设置
	if ((bPlayerMode==true)&&(GetMeChairID()==m_wCurrentUser))
	{
		//控制界面
		BYTE cbGangCard[4]={0,0,0,0};
		m_GameClientView.SetStatusFlag(true,false);

		tagHuInput HuInput;
		HuInput.bJudgeHuOnly		= true;
		HuInput.bJudgeKingWait		= false;
		HuInput.bInsertCurrentCard	= false;
		HuInput.cbCurrentCard		= 0;
		HuInput.pWeaveItemSeq		= m_WeaveItemArray[m_wCurrentUser];
		HuInput.cbWeaveItemCount	= m_bWeaveCount[m_wCurrentUser];
		HuInput.pCardIndexSeq		= m_cbCardIndex;
		tagHuCourt HuCourt;
		bool bHuCard = m_GameLogic.GetHuInfo(&HuInput, HuCourt);

		m_GameLogic.GetGangCardInfo(m_cbCardIndex,NULL,0,cbGangCard);

		//控制界面
		if ((cbGangCard[0]!=0)||(bHuCard==true)) 
		{
			BYTE cbActionMask=(bHuCard==true)?ACK_CHIHU:ACK_NULL;
			m_GameClientView.m_ControlWnd.SetControlInfo(0,cbActionMask,cbGangCard);
		}
	}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//激活框架
	if (bPlayerMode==true)
	{
		ActiveGameFrame();
	}

	//环境处理
	SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,20);
	
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) 
	{
		return false;
	}

	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	//设置变量
	WORD wMeChairID=GetMeChairID();
	m_wCurrentUser=pOutCard->wCurrentUser;
	m_wOutCardUser[m_bOutCardCount]=pOutCard->wOutCardUser;
	m_cbOutCardData[m_bOutCardCount++]=pOutCard->cbOutCardData;
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);

	//公胡设置
	if (pOutCard->wOutCardUser==wMeChairID) 
	{
		m_bChiHu=true;
	}

	//其他用户
	if ((pOutCard->wOutCardUser!=wMeChairID)||(IsLookonMode()==true))
	{
		//环境设置
		KillGameTimer(IDI_OUT_CARD); 
//		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

		//出牌界面
		m_GameClientView.SetUserAction(INVALID_CHAIR,0);
		m_GameClientView.SetOutCardInfo(wOutViewChairID,pOutCard->cbOutCardData);

		//设置扑克
		if (wOutViewChairID==2)
		{
			BYTE cbCardData[MAX_COUNT];
			m_GameLogic.RemoveCard(m_cbCardIndex,pOutCard->cbOutCardData);
			BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData,CountArray(cbCardData));
			//m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
			SetHandCardData(cbCardData,cbCardCount,0);
		}
		else
		{
			WORD wUserIndex = (wOutViewChairID > 2) ? 2:wOutViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(false);
		}
	}

	//取牌处理
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//用户取牌
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		if (wViewChairID!=2)
		{
			WORD wUserIndex=(wViewChairID>2)?2:wViewChairID;
			m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(true);
		}
		else
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pOutCard->cbCurrentCard)]++;
			//m_GameClientView.m_HandCardControl.SetCurrentCard(pOutCard->cbCurrentCard);
			SetHandCurrentCard(pOutCard->cbCurrentCard);
		}

		//扣除扑克
		DeductionTableCard(true);

		//丢弃扑克
		m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(pOutCard->cbOutCardData);

		//当前用户
		if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
		{
			//变量定义
			BYTE cbGangCard[4]={0,0,0,0};
			BYTE cbItemCount=m_bWeaveCount[m_wCurrentUser];
			tagWeaveItem * pWeaveItem=m_WeaveItemArray[m_wCurrentUser];

			//动作判断
			if (m_bLeftCardCount>1) 
			{
				m_GameLogic.GetGangCardInfo(m_cbCardIndex,pWeaveItem,cbItemCount,cbGangCard);
			}

			tagHuInput HuInput;
			HuInput.bJudgeHuOnly		= true;
			HuInput.bJudgeKingWait		= false;
			HuInput.bInsertCurrentCard	= false;
			HuInput.cbCurrentCard		= 0;
			HuInput.pWeaveItemSeq		= m_WeaveItemArray[m_wCurrentUser];
			HuInput.cbWeaveItemCount	= m_bWeaveCount[m_wCurrentUser];
			HuInput.pCardIndexSeq		= m_cbCardIndex;
			tagHuCourt HuCourt;
			BYTE cbActionMask = ACK_NULL;
			if(m_GameLogic.GetHuInfo(&HuInput, HuCourt))
			{
				cbActionMask |= ACK_CHIHU;
			}

			if ((cbGangCard[0]!=0)||(cbActionMask == ACK_CHIHU) )
			{
				m_GameClientView.m_ControlWnd.SetControlInfo(0,cbActionMask,cbGangCard);
			}

			//激活框架
			ActiveGameFrame();
		}

		//出牌提示
		m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID),false);

		//时间设置
		SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,20);
	}
	else
	{
		//控制设置
		if ((IsLookonMode()==false)&&(pOutCard->wOutCardUser!=wMeChairID))
		{
			BYTE cbActionMask = ACK_NULL;

#ifdef MJ_DESPERATE	

			//冲击麻将限制吃碰一个玩家最多两次
			BYTE cbOperateSum = 0;
			for(WORD j=0; j<m_bWeaveCount[wMeChairID]; j++)
			{
				if(m_WeaveItemArray[wMeChairID][j].wProvideUser == pOutCard->wOutCardUser)
				{
					cbOperateSum ++;
				}
			}
			if(cbOperateSum < 2)
			{
				//碰牌判断
				cbActionMask = m_GameLogic.GetPengCardKind(m_cbCardIndex,pOutCard->cbOutCardData);
				//吃牌判断
				WORD wEatUser = (pOutCard->wOutCardUser+GAME_PLAYER-1)%GAME_PLAYER;
				if (wEatUser == wMeChairID)
				{
					cbActionMask|=m_GameLogic.GetEatCardKind(m_cbCardIndex,pOutCard->cbOutCardData);
				}
				//杠牌判断
				if (m_bLeftCardCount > 1)
				{
					cbActionMask|=m_GameLogic.GetGangCardKind(m_cbCardIndex,pOutCard->cbOutCardData);
				}
			}
			else
			{
				CString str("weaveCount == %d ", m_bWeaveCount[wMeChairID]);
				InsertSystemString(str);
			}
#else

			//碰牌判断
			cbActionMask = m_GameLogic.GetPengCardKind(m_cbCardIndex,pOutCard->cbOutCardData);
			//吃牌判断
			WORD wEatUser = (pOutCard->wOutCardUser+GAME_PLAYER-1)%GAME_PLAYER;
			if (wEatUser == wMeChairID)
			{
				cbActionMask|=m_GameLogic.GetEatCardKind(m_cbCardIndex,pOutCard->cbOutCardData);
			}
			//杠牌判断
			if (m_bLeftCardCount > 1)
			{
				cbActionMask|=m_GameLogic.GetGangCardKind(m_cbCardIndex,pOutCard->cbOutCardData);
			}

#endif
			//公胡判断
			if (m_bChiHu==true)
			{
				tagHuInput HuInput;
				HuInput.bJudgeHuOnly		= true;
				HuInput.bJudgeKingWait		= false;
				HuInput.bInsertCurrentCard	= true;
				HuInput.cbCurrentCard		= pOutCard->cbOutCardData;
				HuInput.pWeaveItemSeq		= m_WeaveItemArray[wMeChairID];
				HuInput.cbWeaveItemCount	= m_bWeaveCount[wMeChairID];
				HuInput.pCardIndexSeq		= m_cbCardIndex;
				tagHuCourt HuCourt;
				if(m_GameLogic.GetHuInfo(&HuInput, HuCourt))
				{
					cbActionMask |= ACK_CHIHU;
					m_bChiHu = false;
				}

			}
			else
			{
				BYTE cbWeaveCount=m_bWeaveCount[wMeChairID];
				tagHuInput HuInput;
				HuInput.bJudgeHuOnly		= true;
				HuInput.bJudgeKingWait		 = false;
				HuInput.bInsertCurrentCard	= true;
				HuInput.cbCurrentCard		= pOutCard->cbOutCardData;
				HuInput.pWeaveItemSeq		= m_WeaveItemArray[wMeChairID];
				HuInput.cbWeaveItemCount	= m_bWeaveCount[wMeChairID];
				HuInput.pCardIndexSeq		= m_cbCardIndex;
				tagHuCourt HuCourt;
				if(m_GameLogic.GetHuInfo(&HuInput, HuCourt))
				{
					InsertSystemString(TEXT("因为“公胡”规则，你无法胡此牌！"));
				}
			}

			//控制界面
			if (cbActionMask!=ACK_NULL)
			{
				ActiveGameFrame();
				SetGameTimer(wMeChairID,IDI_OPERATE_CARD,10);
				m_GameClientView.m_ControlWnd.SetControlInfo(pOutCard->cbOutCardData,cbActionMask,NULL);
			}
		}

		//出牌提示
		m_GameClientView.SetStatusFlag(false,false);
	}

	//播放声音
	PlayOutCardSound(pOutCard->wOutCardUser, pOutCard->cbOutCardData);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);


	//
	//_DebugAssert();

	return true;
}

//操作扑克
bool CGameClientDlg::OnSubOperateCard(const void * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OperateCard));
	if (wDataSize!=sizeof(CMD_S_OperateCard)) 
	{
		return false;
	}

	//消息处理
	CMD_S_OperateCard * pOperateCard=(CMD_S_OperateCard *)pBuffer;

	//变量定义
	bool bPublicGang=false;
	WORD wOperateUser	= pOperateCard->wOperateUser;
	BYTE cbOperateCard	= pOperateCard->cbOperateCard;
	WORD wOperateViewID = SwitchViewChairID(wOperateUser);
	WORD wProviderViewID = SwitchViewChairID(pOperateCard->wProvideUser);

	//设置变量
	m_wCurrentUser=pOperateCard->wCurrentUser;

	//设置组合
	if (pOperateCard->cbOperateCode==ACK_GANG)
	{
		//组合扑克
		BYTE cbWeaveIndex=0xFF;
		for (BYTE i=0;i<m_bWeaveCount[wOperateUser];i++)
		{
			BYTE cbWeaveKind=m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
			BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==ACK_PENG))
			{
				cbWeaveIndex=i;
				bPublicGang=true;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=ACK_GANG;
				break;
			}
		}

		//组合扑克
		if (cbWeaveIndex==0xFF)
		{
			cbWeaveIndex=m_bWeaveCount[wOperateUser]++;
			bPublicGang=(pOperateCard->wProvideUser!=wOperateUser);
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=ACK_GANG;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateCard->wProvideUser;
		}

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateCard->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCardData(cbWeaveKind,cbOperateCard,cbWeaveCard);
		bool bShowWeaveItem=bPublicGang;//(bPublicGang==true)||((IsLookonMode()==false)&&(GetMeChairID()==wOperateUser));
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetDisplayItem(bShowWeaveItem);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);

		//扑克设置
		if (GetMeChairID()==wOperateUser)
		{
			m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pOperateCard->cbOperateCard)]=0;
			//m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pOperateCard->cbCurrentCard)]++;
		}

		//扣除扑克
		DeductionTableCard(false);
	}
	else if (pOperateCard->cbOperateCode!=ACK_NULL)
	{
		//设置组合
		BYTE cbWeaveIndex=m_bWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateCard->cbOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateCard->wProvideUser;

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateCard->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCardData(cbWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetCardData(cbWeaveCard,cbWeaveCardCount);
		m_GameClientView.m_WeaveCard[wOperateViewID][cbWeaveIndex].SetDirectionCardPos(3-(wOperateViewID-wProviderViewID+4)%4);

		//删除扑克
		if (GetMeChairID()==wOperateUser)
		{
			m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
			m_GameLogic.RemoveCard(m_cbCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		}
	}

	//用户扑克
	if (pOperateCard->cbOperateCode!=ACK_NULL)
	{
		if (GetMeChairID()==wOperateUser)
		{
			BYTE cbCardData[MAX_COUNT];
			if (pOperateCard->cbCurrentCard!=0)
			{
				BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData,CountArray(cbCardData));
				m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pOperateCard->cbCurrentCard)]++;
				//m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,pOperateCard->cbCurrentCard);
				SetHandCardData(cbCardData,cbCardCount,pOperateCard->cbCurrentCard);
				
			}
			else
			{
				BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData,CountArray(cbCardData));
				//m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount-1,cbCardData[cbCardCount-1]);
				SetHandCardData(cbCardData,cbCardCount-1,cbCardData[cbCardCount-1]);
			}
		}
		else
		{
			WORD wUserIndex=(wOperateViewID>=3)?2:wOperateViewID;
			BYTE cbCardCount=MAX_COUNT-m_bWeaveCount[wOperateUser]*3;
			m_GameClientView.m_UserCard[wUserIndex].SetCardData(cbCardCount-1,true);
		}
	}

	//控制界面
	if ((IsLookonMode()==false)&&(GetMeChairID()==m_wCurrentUser))
	{
		//变量定义
		BYTE cbGangCard[4]={0,0,0,0};
		BYTE cbItemCount=m_bWeaveCount[m_wCurrentUser];
		tagWeaveItem * pWeaveItem=m_WeaveItemArray[m_wCurrentUser];

		//动作判断
		bool bHuCard=false;
		if (pOperateCard->cbCurrentCard!=0) 
		{
			tagHuInput HuInput;
			HuInput.bJudgeHuOnly		= true;
			HuInput.bJudgeKingWait		= false;
			HuInput.bInsertCurrentCard	= false;
			HuInput.cbCurrentCard		= 0;
			HuInput.pWeaveItemSeq		= m_WeaveItemArray[m_wCurrentUser];
			HuInput.cbWeaveItemCount	= m_bWeaveCount[m_wCurrentUser];
			HuInput.pCardIndexSeq		= m_cbCardIndex;
			tagHuCourt HuCourt;
			bHuCard = m_GameLogic.GetHuInfo(&HuInput, HuCourt);

		}
		if (m_bLeftCardCount>1) 
		{
			m_GameLogic.GetGangCardInfo(m_cbCardIndex,pWeaveItem,cbItemCount,cbGangCard);
		}
		
		//控制界面
		if ((cbGangCard[0]!=0)||(bHuCard==true))
		{
			BYTE cbActionMask=(bHuCard==true)?ACK_CHIHU:ACK_NULL;
			m_GameClientView.m_ControlWnd.SetControlInfo(0,cbActionMask,cbGangCard);
		}
		else 
		{
			m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
		}
	}
	else 
	{
		m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	}

	//设置界面
	m_GameClientView.SetUserAction(wOperateViewID,pOperateCard->cbOperateCode);
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()),false);

	//播放声音
	switch(pOperateCard->cbOperateCode) 
	{
		case ACK_GANG:
			PlayChairSound(pOperateCard->wOperateUser, _T("COMMAND_KONG"));
			break;
		case ACK_PENG:
			PlayChairSound(pOperateCard->wOperateUser, _T("COMMAND_PONG"));
			break;
		case ACK_LEFT:
		case ACK_CENTER:
		case ACK_RIGHT:
			PlayChairSound(pOperateCard->wOperateUser, _T("COMMAND_CHOW"));
			break;
	}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//环境设置
//	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,20);

	//
	//_DebugAssert();

	return true;
}

//发送扑克
bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//消息处理
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//设置变量
	WORD wMeChairID=GetMeChairID();
	m_wCurrentUser=pSendCard->wCurrentUser;

	//取牌界面
	WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
	if (wViewChairID!=2)
	{
		WORD wUserIndex=(wViewChairID>2)?2:wViewChairID;
		m_GameClientView.m_UserCard[wUserIndex].SetCurrentCard(true);
	}
	else
	{
		m_cbCardIndex[m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;
		//m_GameClientView.m_HandCardControl.SetCurrentCard(pSendCard->cbCardData);
		SetHandCurrentCard(pSendCard->cbCardData);
	}

	//扣除扑克
	DeductionTableCard(true);

	//丢弃扑克
	ASSERT(m_bOutCardCount>0);
	WORD wOutViewChairID=SwitchViewChairID(m_wOutCardUser[m_bOutCardCount-1]);
	m_GameClientView.m_DiscardCard[wOutViewChairID].AddCardItem(m_cbOutCardData[m_bOutCardCount-1]);

	//当前用户
	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		//变量定义
		BYTE cbGangCard[4]={0,0,0,0};
		BYTE cbItemCount=m_bWeaveCount[m_wCurrentUser];
		tagWeaveItem * pWeaveItem=m_WeaveItemArray[m_wCurrentUser];

		//动作判断
		tagHuInput HuInput;
		HuInput.bJudgeHuOnly		= true;
		HuInput.bJudgeKingWait		= false;
		HuInput.bInsertCurrentCard	= false;
		HuInput.cbCurrentCard		= 0;
		HuInput.pWeaveItemSeq		= m_WeaveItemArray[m_wCurrentUser];
		HuInput.cbWeaveItemCount	= m_bWeaveCount[m_wCurrentUser];
		HuInput.pCardIndexSeq		= m_cbCardIndex;
		tagHuCourt HuCourt;
		bool bHuCard = m_GameLogic.GetHuInfo(&HuInput, HuCourt);

		if (m_bLeftCardCount>1) m_GameLogic.GetGangCardInfo(m_cbCardIndex,pWeaveItem,cbItemCount,cbGangCard);

		//控制界面
		if ((cbGangCard[0]!=0)||(bHuCard==true)) 
		{
			BYTE cbActionMask=(bHuCard==true)?ACK_CHIHU:ACK_NULL;
			m_GameClientView.m_ControlWnd.SetControlInfo(0,cbActionMask,cbGangCard);
		}

		//激活框架
		ActiveGameFrame();

	}

	//出牌提示
	m_GameClientView.SetStatusFlag((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()),false);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//时间设置
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,20);

	//
	//_DebugAssert();
	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//删除定时器
	KillGameTimer(IDI_OUT_CARD);
	KillGameTimer(IDI_OPERATE_CARD);

	//设置控件
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl.SetPositively(false);

	//ready设置
	m_bSelfReady = (pGameEnd->cbSelfReady == 0 ? false : true);

	//计算剩余财富
#ifdef MJ_DESPERATE
	//m_lScoreLeft = pGameEnd->lLeftScore[GetMeChairID()];
	CopyMemory(m_lScoreLeft, pGameEnd->lLeftScore, sizeof(m_lScoreLeft));
#endif
    
	//结束设置
	tagGameScoreInfo GameScoreInfo;
	GameScoreInfo.wBankerUser=m_wBankerUser;
	GameScoreInfo.stHuCourt = pGameEnd->stHuCourt;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData = GetUserData(i);
		GameScoreInfo.lScore[i]=pGameEnd->lGameScore[i];
		lstrcpyn(GameScoreInfo.szUserName[i],pUserData->szName,CountArray(GameScoreInfo.szUserName[i]));
	}
	m_GameClientView.m_GameScoreWnd.SetScoreInfo(GameScoreInfo);

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		if(i == pGameEnd->stHuCourt.wWinChair)
		{
			m_GameClientView.SetUserAction(wViewChairID,ACK_CHIHU);
		}
		m_GameClientView.m_TableCard[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);
	}

	//设置扑克
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	//m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);
	SetHandCardData(NULL,0,0);

	//播放声音
	//LONG lScore=pGameEnd->lGameScore[GetMeChairID()];
	/*if (lScore>0L) 
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	}
	else if (lScore<0L) 
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else 
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));
	}*/

	if(pGameEnd->stHuCourt.IsHu())
	{
		if(pGameEnd->stHuCourt.IsZimo())
		{
			PlayChairSound(pGameEnd->stHuCourt.wWinChair,TEXT("GAME_HU_ZIMO"));
		}
		else if(pGameEnd->stHuCourt.wHuAttachedType & EN_HU_ATTACHED_GANG_FLOWER)
		{
			PlayChairSound(pGameEnd->stHuCourt.wWinChair,TEXT("GAME_KONGF"));
		}
		else
		{
			PlayChairSound(pGameEnd->stHuCourt.wWinChair,TEXT("GAME_HU_CHI"));
		}
	}
	else
	{
		PlayChairSound(GetMeChairID(),TEXT("GAME_END"));
	}

	//设置界面
	m_GameClientView.SetStatusFlag(false, false);
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameTimer(GetMeChairID(),IDI_START_GAME,30);

	}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//系统消息
bool CGameClientDlg::OnSubSysMsg(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SysMsg));
	if (wDataSize!=sizeof(CMD_S_SysMsg))
	{
		return false;
	}

	//处理消息
	CMD_S_SysMsg* pMsg = (CMD_S_SysMsg*)pBuffer;
	pMsg->cbBuf[127] = 0;
	CString strMsg(pMsg->cbBuf);

	return InsertSystemString(strMsg);
}

//玩家准备
bool CGameClientDlg::OnSubUserReady(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_UserReady));
	if (wDataSize!=sizeof(CMD_S_UserReady))
	{
		return false;
	}

	//处理消息
	CMD_S_UserReady* pMsg = (CMD_S_UserReady*)pBuffer;
	m_bReady[pMsg->wChairID] = true;

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//注额改变
bool CGameClientDlg::OnSubStake(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_Stake))
	{
		ASSERT(0 && "注额消息长度出错！");
		return false;
	}

	//处理消息
	CMD_S_Stake* pMsg = (CMD_S_Stake*)pBuffer;
	
	CopyMemory(m_lScoreLeft, pMsg->lLeftScore, sizeof(m_lScoreLeft));
	
	if(m_lCellScore != pMsg->lCellScore)
	{
		m_lCellScore = pMsg->lCellScore;
		CString strMsg;
		strMsg.Format(_T("当前底注为%d游戏币。"), m_lCellScore);
		InsertSystemString(strMsg);
	}

	return true;
}

//扣除扑克
void CGameClientDlg::DeductionTableCard(bool bHeadCard)
{
	if (bHeadCard==true)
	{
		//切换索引
		BYTE cbHeapCount=m_bHeapCardInfo[m_wHeapHand][0]+m_bHeapCardInfo[m_wHeapHand][1];
		if (cbHeapCount==HEAP_FULL_COUNT) m_wHeapHand=(m_wHeapHand+1)%4;//CountArray(m_bHeapCardInfo);

		//减少扑克
		m_bLeftCardCount--;
		m_bHeapCardInfo[m_wHeapHand][0]++;

		if(m_wHeapHand == m_wKingBrotherHeapPos &&
			m_bHeapCardInfo[m_wHeapHand][0] == (HEAP_FULL_COUNT/2-m_wKingBrotherCount+1)*2-1)
		{
			m_bLeftCardCount--;
			m_bHeapCardInfo[m_wHeapHand][0]++;
		}

		//堆立扑克
		WORD wHeapViewID=SwitchViewChairID(m_wHeapHand);
		WORD wMinusHeadCount=m_bHeapCardInfo[m_wHeapHand][0];
		WORD wMinusLastCount=m_bHeapCardInfo[m_wHeapHand][1];
		m_GameClientView.m_HeapCard[wHeapViewID].SetCardData(wMinusHeadCount,wMinusLastCount,HEAP_FULL_COUNT);
	}
	else
	{
		//切换索引
		BYTE cbHeapCount=m_bHeapCardInfo[m_wHeapTail][0]+m_bHeapCardInfo[m_wHeapTail][1];
		if (cbHeapCount==HEAP_FULL_COUNT) m_wHeapTail=(m_wHeapTail+3)%4;

		//减少扑克
		m_bLeftCardCount--;
		m_bHeapCardInfo[m_wHeapTail][1]++;

		if(m_wHeapTail == m_wKingBrotherHeapPos &&
			m_bHeapCardInfo[m_wHeapTail][1] == (m_wKingBrotherCount*2-1))
		{
			m_bLeftCardCount--;
			m_bHeapCardInfo[m_wHeapTail][1]++;
		}

		//堆立扑克
		WORD wHeapViewID=SwitchViewChairID(m_wHeapTail);
		WORD wMinusHeadCount=m_bHeapCardInfo[m_wHeapTail][0];
		WORD wMinusLastCount=m_bHeapCardInfo[m_wHeapTail][1];
		m_GameClientView.m_HeapCard[wHeapViewID].SetCardData(wMinusHeadCount,wMinusLastCount,HEAP_FULL_COUNT);
	}

	return;
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//环境设置
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);

	//设置界面
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.SetOutCardInfo(INVALID_CHAIR,0);

	//扑克设置
	m_GameClientView.m_UserCard[0].SetCardData(0,false);
	m_GameClientView.m_UserCard[1].SetCardData(0,false);
	m_GameClientView.m_UserCard[2].SetCardData(0,false);
	//m_GameClientView.m_HandCardControl.SetCardData(NULL,0,0);
	SetHandCardData(NULL,0,0);

	//扑克设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_TableCard[i].SetCardData(NULL,0);
		m_GameClientView.m_DiscardCard[i].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][0].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][1].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][2].SetCardData(NULL,0);
		m_GameClientView.m_WeaveCard[i][3].SetCardData(NULL,0);
	}

	//堆立扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_bHeapCardInfo[i][0]=0;
		m_bHeapCardInfo[i][1]=0;
		m_GameClientView.m_HeapCard[SwitchViewChairID(i)].SetCardData(m_bHeapCardInfo[i][0],m_bHeapCardInfo[i][1],HEAP_FULL_COUNT);
	}

	//游戏变量
	m_wHeapHand=0;
	m_wHeapTail=0;
	m_bLeftCardCount=0;
	m_wCurrentUser=INVALID_CHAIR;

	//出牌信息
	m_bOutCardCount=0;
	ZeroMemory(m_wOutCardUser,sizeof(m_wOutCardUser));
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));

	//组合扑克
	ZeroMemory(m_bWeaveCount,sizeof(m_bWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bHeapCardInfo,sizeof(m_bHeapCardInfo));

	//发送消息
	if(m_bSelfReady)
	{
		SendData(SUB_C_READY, 0, 0);
	}
	//使用自定义的ready信号
	else
	{
		SendUserReady(NULL,0);
	}
	
	return 0;
}

//出牌操作
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//出牌判断
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID())) return 0;

	//设置变量
	BYTE cbOutCard=(BYTE)wParam;
	m_wCurrentUser=INVALID_CHAIR;
	m_GameLogic.RemoveCard(m_cbCardIndex,cbOutCard);

	//设置扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex,cbCardData,CountArray(cbCardData));
	//m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount,0);
	SetHandCardData(cbCardData,cbCardCount,0);

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.UpdateGameView(NULL);
	m_GameClientView.SetStatusFlag(false,false);
	m_GameClientView.SetOutCardInfo(2,cbOutCard);
	m_GameClientView.SetUserAction(INVALID_CHAIR,0);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);

	//播放声音
	//PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCard;
	SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard));

	return 0;
}

//扑克操作
LRESULT CGameClientDlg::OnCardOperate(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	BYTE cbOperateCode=(BYTE)(wParam);
	BYTE cbOperateCard=(BYTE)(LOWORD(lParam));

	//状态判断
	if ((m_wCurrentUser==GetMeChairID())&&(cbOperateCode==ACK_NULL))
	{
		m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
		return 0;
	};

	//环境设置
	m_GameClientView.SetStatusFlag(false,true);
	m_GameClientView.m_ControlWnd.ShowWindow(SW_HIDE);
	KillGameTimer((GetMeChairID()==m_wCurrentUser)?IDI_OUT_CARD:IDI_OPERATE_CARD);

	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	OperateCard.cbOperateCard=cbOperateCard;
	SendData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard));

	return 0;
}

//托管操作
LRESULT CGameClientDlg::OnConsignChanged(WPARAM wParam, LPARAM lParam)
{
	SetConsign((wParam == 1 ? true : false));
	return 0;
}

//手牌信息
void CGameClientDlg::SetHandCardData(BYTE cbCardData[], WORD wCardCount, BYTE cbCurrentCard)
{
	//处理王牌
	if(cbCurrentCard == CARD_KING_DATA_MASK)
	{
//		AfxMessageBox("SetHandCardData");
		cbCurrentCard = cbCardData[wCardCount-1];
		BYTE cbCardIndex[MAX_INDEX];
		CopyMemory(cbCardIndex, m_cbCardIndex, sizeof(cbCardIndex));
		m_GameLogic.RemoveCard(cbCardIndex, cbCurrentCard);
		m_GameLogic.SwitchToCardData(cbCardIndex, cbCardData, (BYTE)wCardCount);	
		ASSERT(m_GameLogic.IsValidCommonCard(cbCurrentCard));
	}
	ASSERT(m_GameLogic.IsValidCommonCard(cbCurrentCard)||cbCurrentCard==0);
	m_GameClientView.m_HandCardControl.SetCardData(cbCardData,wCardCount, cbCurrentCard);

}

//手牌之当前牌
void CGameClientDlg::SetHandCurrentCard(BYTE cbCurrentCard)
{
	//处理王牌
	if(cbCurrentCard == CARD_KING_DATA_MASK)
	{
//		AfxMessageBox("SetHandCurrentCard");
		BYTE cbCardIndex[MAX_INDEX];
		CopyMemory(cbCardIndex, m_cbCardIndex, sizeof(cbCardIndex));
		for(int i=CARD_KING_INDEX-1; i>=0; i--)
		{
			if(cbCardIndex[i] > 0)
			{
				cbCardIndex[i]--;
				cbCurrentCard = m_GameLogic.SwitchToCardData(i);
				break;
			}
		}
		if(i < 0)	//只剩下王牌
		{
			m_GameClientView.m_HandCardControl.SetCurrentCard(cbCurrentCard);
			return;
		}
		
		BYTE cbCardData[MAX_COUNT];
		BYTE cbCardCount = m_GameLogic.SwitchToCardData(cbCardIndex,cbCardData,CountArray(cbCardData));
		ASSERT(m_GameLogic.IsValidCommonCard(cbCurrentCard));
		m_GameClientView.m_HandCardControl.SetCardData(cbCardData,cbCardCount, cbCurrentCard);
		return;
	}
	ASSERT(m_GameLogic.IsValidCommonCard(cbCurrentCard)||cbCurrentCard==0);
	m_GameClientView.m_HandCardControl.SetCurrentCard(cbCurrentCard);

}

//是否准备
bool CGameClientDlg::IsUserReady(WORD wChairID)
{
	//获取用户
	const tagUserData * pUserData = GetUserData(wChairID);
	if(pUserData->cbUserStatus == US_READY || m_bReady[wChairID])
	{
		return true;
	}
	return false;
}

//智能打牌
BYTE CGameClientDlg::GetIsolatedCard()
{
	//有风打风
	for(int i=27; i<27+7; i++)
	{
		if(m_cbCardIndex[i] == 1)
		{
			return m_GameLogic.SwitchToCardData(i);
		}
	}
	//有孤牌打孤牌
	//打19孤牌
	for(i=0; i<3; i++)
	{
		if(m_cbCardIndex[i*9] == 1)
		{
			if(m_cbCardIndex[i*9+1] == 0
				&& m_cbCardIndex[i*9+2] == 0)
			{
				return m_GameLogic.SwitchToCardData(i*9);
			}
		}
		if(m_cbCardIndex[i*9+8] == 1)
		{
			if(m_cbCardIndex[i*9+7] == 0
				&&  m_cbCardIndex[i*9+6] == 0)
			{
				return m_GameLogic.SwitchToCardData(i*9+8);
			}
		}
	}
	//打普通孤牌
	for( i=0; i<27; i++)
	{
		if(m_cbCardIndex[i] == 1)
		{
			BYTE cbTmp = i%9;
			switch(cbTmp)
			{
			case 0:
				if(m_cbCardIndex[i+1] == 0
					&& m_cbCardIndex[i+2] == 0)
				{
					return m_GameLogic.SwitchToCardData(i);
				}
				break;
			case 1:
				if(m_cbCardIndex[i+1] == 0
					&& m_cbCardIndex[i+2] == 0
					&& m_cbCardIndex[i-1] == 0)
				{
					return m_GameLogic.SwitchToCardData(i);
				}
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				if(m_cbCardIndex[i+1] == 0
					&& m_cbCardIndex[i+2] == 0
					&& m_cbCardIndex[i-1] == 0
					&& m_cbCardIndex[i-2] == 0)
				{
					return m_GameLogic.SwitchToCardData(i);
				}
				break;
			case 7:
				if(m_cbCardIndex[i-1] == 0
					&& m_cbCardIndex[i-2] == 0
					&& m_cbCardIndex[i+1] == 0)
				{
					return m_GameLogic.SwitchToCardData(i);
				}
				break;
			case 8:
				if(m_cbCardIndex[i-1] == 0
					&& m_cbCardIndex[i-2] == 0)
				{
					return m_GameLogic.SwitchToCardData(i);
				}
				break;
			}
			if(cbTmp < 3)
			{
				i += 5;
			}
			else
			{
				i = (i/9+1)*9 - 1;
			}
		}	
	}

	//打卡张
	//打19卡张
	for(i=0; i<3; i++)
	{
		if(m_cbCardIndex[i*9] == 1)
		{
			if(m_cbCardIndex[i*9+1] == 0
				|| m_cbCardIndex[i*9+2] == 0)
			{
				return m_GameLogic.SwitchToCardData(i*9);
			}
		}
		if(m_cbCardIndex[i*9+8] == 1)
		{
			if(m_cbCardIndex[i*9+7] == 0
				|| m_cbCardIndex[i*9+6] == 0)
			{
				return m_GameLogic.SwitchToCardData(i*9+8);
			}
		}
	}

	//打普通卡张
	for(i=0; i<27; i++)
	{
		if(m_cbCardIndex[i] == 1)
		{
			if((i%9) == 0)
			{
				i += 3;
				continue;
			}
			if((i+1)%9 == 0)
			{
				continue;
			}
			if(m_cbCardIndex[i+1] == 0
				&& m_cbCardIndex[i-1] == 0)
			{
				return m_GameLogic.SwitchToCardData(i);
			}
			i += 1;
		}	
	}

	return  0XFF;
}

//设置托管
void CGameClientDlg::SetConsign(bool bEnter)
{
	if(bEnter == m_bConsign)
	{
		return;
	}
	//超时次数重新计算
	m_cbOverTimeCount	= 0;	
	m_bConsign = bEnter;
	m_GameClientView.SetConsign(m_bConsign);
	if(m_bConsign)
	{
		InsertSystemString(_T("系统托管中，点击右下角按钮可取消托管！"));
	}
	else
	{
		InsertSystemString(_T("您已经取消托管！"));
	}
	
}

//设置定时器
bool CGameClientDlg::SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse)
{
	if(nTimerID == IDI_OUT_CARD || nTimerID == IDI_START_GAME)
	{
#ifdef _AUTOPALY
		return CGameFrameDlg::SetGameTimer(wChairID,nTimerID,1);
#else
		if(m_bConsign)
		{
			return CGameFrameDlg::SetGameTimer(wChairID,nTimerID,1);
		}
#endif
	}

	return CGameFrameDlg::SetGameTimer(wChairID, nTimerID, nElapse);
	
}

//播放出牌声音
void CGameClientDlg::PlayOutCardSound(WORD wChairID, BYTE cbCardData)
{
	if(m_GameLogic.IsValidCommonCard(cbCardData) == false)
	{
		return;
	}
	if(wChairID < 0 || wChairID > 3)
	{
		return;
	}

	//判断性别
	const tagUserData* pUserData = GetUserData(wChairID);
	if(pUserData == 0)
	{
		assert(0 && "得不到玩家信息");
		return;
	}
	bool bBoy = (pUserData->cbGender == GENDER_GIRL ? false : true);

	//BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(cbCardData);
	BYTE cbType= (cbCardData & MASK_COLOR);
	BYTE cbValue= (cbCardData & MASK_VALUE);
	CString strSoundName;
	switch(cbType)
	{
	case 0X30:	//风
		{
			switch(cbValue) 
			{
			case 1:
				{
					strSoundName = _T("WIND_EAST");
					break;
				}
			case 2:
				{
					strSoundName = _T("WIND_SOUTH");
					break;
				}
			case 3:
				{
					strSoundName = _T("WIND_WEST");
					break;
				}
			case 4:
				{
					strSoundName = _T("WIND_NORTH");
					break;
				}
			case 5:
				{
					strSoundName = _T("DRAGON_RED");
					break;
				}
			case 6:
				{
					strSoundName = _T("DRAGON_GREEN");
					break;
				}
			case 7:
				{
					strSoundName = _T("DRAGON_WHITE");
					break;
				}
			}
			break;
		}		
	case 0X20:	//筒
		{
			strSoundName.Format(_T("CIRCLE_%d"), cbValue);
			break;
		}
		
	case 0X10:	//索
		{
			strSoundName.Format(_T("BAMBOO_%d"), cbValue);
			break;
		}
	case 0X00:	//万
		{
			strSoundName.Format(_T("CHARACTER_%d"), cbValue);
			break;
		}
	}	
	
	if(bBoy)
	{
		strSoundName = _T("BOY1_") + strSoundName;
	}
	else
	{
		strSoundName = _T("GIRL1_") + strSoundName;
	}
	//strSoundName.Format("Gire")
	PlayGameSound(AfxGetInstanceHandle(), strSoundName);
}


//播放其他声音
void CGameClientDlg::PlayChairSound(WORD wChairID, LPCSTR lpstrName)
{
	if(lpstrName == 0)
	{
		return;
	}
	//判断性别
	const tagUserData* pUserData = GetUserData(wChairID);
	if(pUserData == 0)
	{
		assert(0 && "得不到玩家信息");
		return;
	}
	bool bBoy = (pUserData->cbGender == GENDER_GIRL ? false : true);

	CString strSoundName = lpstrName;
	if(bBoy)
	{
		strSoundName = _T("BOY1_") + strSoundName;
	}
	else
	{
		strSoundName = _T("GIRL1_") + strSoundName;
	}
	//strSoundName.Format("Gire")
	PlayGameSound(AfxGetInstanceHandle(), strSoundName);
}

void CGameClientDlg::_DebugAssert()
{
	int iSum = 0;
	for(int i=0; i<MAX_INDEX; i++)
	{
		if( m_cbCardIndex[i] > 0)
		{
			iSum = m_cbCardIndex[i];
			BYTE cbCardData = m_GameLogic.SwitchToCardData(i);
			for(int j=0; j<108; j++)
			{
				if(m_cbOutCardData[j] == cbCardData)
				{
					iSum ++;
				}
			}
			if(iSum > 4)
			{
				assert(0 && "竟然多牌了！");
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
