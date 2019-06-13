#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//超时次数
#define MAX_TIME_OUT				3									//超时次数

//游戏时间
#define IDI_OUT_CARD				200									//出牌定时器
#define IDI_START_GAME				203									//开始定时器

//游戏时间
#ifdef _DEBUG
#define TIME_OUT_CARD				300									//出牌定时器
#else
#define TIME_OUT_CARD				30									//出牌定时器
#endif
#define TIME_START_GAME				30									//开始定时器

//游戏时间
#define IDI_MOST_CARD				300									//最大定时器
#define IDI_HIDE_CARD				301									//隐藏定时器
#define IDI_LAST_TURN				302									//上轮定时器
#define IDI_DISPATCH_CARD			303									//发牌定时器


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnMessageStart)
	ON_MESSAGE(IDM_OUT_CARD,OnMessageOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnMessagePassCard)
	ON_MESSAGE(IDM_OUT_PROMPT,OnMessageOutPrompt)
	ON_MESSAGE(IDM_LAST_TURN_CARD,OnMessageLastTurn)
	ON_MESSAGE(IDM_SORT_HAND_CARD,OnMessageSortCard)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnMessageStustee)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnMessageLeftHitCard)
	ON_MESSAGE(WM_ENABLE_HISTORY,OnMessageEnableHistory)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg(): CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_cbMainValue=0;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbValueOrder,sizeof(m_cbValueOrder));
	ZeroMemory(m_bAutoStatus,sizeof(m_bAutoStatus));
		
	m_bTongHuaCount=0;
	m_bTongHuaIndex=0;
	ZeroMemory(m_bTongHuaCard,sizeof(m_bTongHuaCard));

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));

		//得分变量
	m_TurnScore=0;
	ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//出牌变量
	m_cbTurnCardCount=0;
	m_cbTurnCardType=CT_ERROR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

	//辅助变量
	m_bStustee=false;
	m_bLastTurn=false;
	m_wTimeOutCount=0;
	m_cbSortType=ST_ORDER;
	m_wMostUser=INVALID_CHAIR;
	
	//发牌变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	m_dwCardHSpace=DEFAULT_PELS;
	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool  CGameClientDlg::InitGameFrame()
{
	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	return true;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_dwCardHSpace=m_dwCardHSpace;
	GameOption.m_bEnableSound=IsEnableSound();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//获取参数
		m_dwCardHSpace=GameOption.m_dwCardHSpace;

		//设置控件
		EnableSound(GameOption.m_bEnableSound);
		m_GameClientView.m_HandCardControl[2].SetCardDistance(m_dwCardHSpace,0,20);

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
	}

	//配置数据

	return;
}
//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	//扑克控制
	m_GameClientView.m_HandCardControl[2].SetDisplayItem((IsAllowLookon()==true));

	return;
}

bool CGameClientDlg::OnSubUserAutomatism(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_UserAutomatism));
	if (wDataSize!=sizeof(CMD_S_UserAutomatism)) return false;

	//消息处理
	CMD_S_UserAutomatism * pUserAutomatism =(CMD_S_UserAutomatism *)pBuffer;

	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		m_bAutoStatus[SwitchViewChairID(i)]=pUserAutomatism->bAutoStatus[i];
	}

	m_GameClientView.SetAutoUser(m_bAutoStatus);

	return true;
}
//重置引擎
void  CGameClientDlg::ResetGameFrame()
{
	//内核重置
	//得分变量
	m_TurnScore=0;
	ZeroMemory(m_PlayerScore,sizeof(m_PlayerScore));

	//删除时间
	KillTimer(IDI_MOST_CARD);
	KillTimer(IDI_LAST_TURN);
	KillTimer(IDI_DISPATCH_CARD);

	//游戏变量
	m_cbMainValue=0;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_cbValueOrder,sizeof(m_cbValueOrder));
	ZeroMemory(m_bAutoStatus,sizeof(m_bAutoStatus));
		
	m_bTongHuaCount=0;
	m_bTongHuaIndex=0;
	ZeroMemory(m_bTongHuaCard,sizeof(m_bTongHuaCard));

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//出牌变量
	m_cbTurnCardCount=0;
	m_cbTurnCardType=CT_ERROR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

	//辅助变量
	m_bStustee=false;
	m_bLastTurn=false;
	m_wTimeOutCount=0;
	m_cbSortType=ST_ORDER;
	m_wMostUser=INVALID_CHAIR;
	
	//发牌变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	//历史数据
	m_HistoryCard.ResetData();

	return ;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_OUT_CARD:		//用户出牌
		{
			//自动处理
			if ((((nElapse==0)||((m_bStustee==true)&&(nElapse<TIME_OUT_CARD))))&&(wChairID==GetMeChairID()))
			{
				//超时判断
				if ((IsLookonMode()==false)&&(m_bStustee==false))
				{
					//超时判断
					BYTE cbCardCount=m_cbHandCardCount[wChairID];
					if ((cbCardCount>3)||(cbCardCount>=m_cbTurnCardCount)) m_wTimeOutCount++;

					//状态切换
					if (m_wTimeOutCount>=MAX_TIME_OUT)
					{
						m_wTimeOutCount=0;
						OnMessageStustee(0,0);

						InsertSystemString(TEXT("由于您多次超时，切换为“系统托管”模式"));
					}
				}

				//自动出牌
				AutomatismOutCard();

				return true;
			}

			//超时警告
			if ((nElapse<=5)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) 
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	case IDI_START_GAME:	//游戏开始
		{
			//关闭游戏
			if ((IsLookonMode()==false)&&(nElapse==0))
			{
				OnMessageStart(0,0);
				return true;
			}

			//开始判断
			if ((IsLookonMode()==false)&&(nElapse<TIME_START_GAME)&&(m_bStustee==true))
			{
				OnMessageStart(0,0);
				return true;
			}

			//超时警告
			if ((IsLookonMode()==false)&&(nElapse<=5))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}

			return true;
		}
	}

	return false;
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_AUTOMATISM:
		{
			return OnSubUserAutomatism(pData, wDataSize);
		}
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_PASS_CARD:		//用户放弃
		{
			return OnSubPassCard(pData,wDataSize);
		}
	case SUB_S_CARD_INFO:		//扑克信息
		{
			return OnSubCardInfo(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_CONTINUE_GAME:	//继续游戏
		{
			return OnSubContinueGame(pData,wDataSize);
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				m_bAutoStatus[i]=pStatusFree->bAutoStatus[i];
			}

			m_GameClientView.SetAutoUser(m_bAutoStatus);

			//玩家设置
			if (IsLookonMode()==false)
			{
				//控制按钮
				m_GameClientView.m_btStustee.EnableWindow(TRUE);

				//开始设置
				if (GetUserData(GetMeChairID())->cbUserStatus!=US_READY)
				{
					PerformAutoStart();
					m_GameClientView.m_btStart.ShowWindow(TRUE);
					m_GameClientView.m_btStart.SetFocus();
				}
			}

			//设置界面
			m_GameClientView.SetCellScore(pStatusFree->lCellScore);

			return true;
		}
	case GAME_SCENE_PLAY:	//游戏状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//变量定义
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				m_bAutoStatus[i]=pStatusPlay->bAutoStatus[i];
			}

			m_GameClientView.SetAutoUser(m_bAutoStatus);

			//变量定义
			WORD wMeChairID=GetMeChairID();

			//设置变量
			m_cbMainValue=pStatusPlay->cbMainValue;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			CopyMemory(m_cbValueOrder,pStatusPlay->cbValueOrder,sizeof(m_cbValueOrder));
			CopyMemory(m_cbHandCardCount,pStatusPlay->cbHandCardCount,sizeof(m_cbHandCardCount));

			//胜利信息
			m_wWinCount=pStatusPlay->wWinCount;
			CopyMemory(m_wWinOrder,pStatusPlay->wWinOrder,sizeof(m_wWinOrder));

			//出牌变量
			m_cbTurnCardType=pStatusPlay->cbTurnCardType;
			m_cbTurnCardCount=pStatusPlay->cbTurnCardCount;
			CopyMemory(m_cbTurnCardData,pStatusPlay->cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));
			CopyMemory(m_cbMagicCardData,pStatusPlay->cbMagicCardData,m_cbTurnCardCount*sizeof(BYTE));

			//扑克数据
			WORD wUserIndex=(pStatusPlay->cbHandCardCount[wMeChairID]==0)?(wMeChairID+2)%GAME_PLAYER:wMeChairID;
			CopyMemory(m_cbHandCardData,pStatusPlay->cbHandCardData,sizeof(BYTE)*m_cbHandCardCount[wUserIndex]);

			//设置界面
			m_GameClientView.SetCellScore(pStatusPlay->lCellScore);
			m_GameClientView.SetMainValue(pStatusPlay->cbMainValue);
			m_GameClientView.SetUserValueOrder(m_cbValueOrder[wMeChairID%2],m_cbValueOrder[(wMeChairID+1)%2]);

			//设置逻辑
			m_GameLogic.SetMainValue(m_cbMainValue);

			//设置名次
			for (WORD i=0;i<m_wWinCount;i++)
			{
				m_GameClientView.SetUserWinOrder(SwitchViewChairID(m_wWinOrder[i]),i+1);
			}

			//设置视图
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//获取位置
				WORD wViewChairID=SwitchViewChairID(i);

				//自己扑克
				if ((i==wMeChairID)&&(m_cbHandCardCount[wMeChairID]!=0))
				{
					m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[i],m_cbSortType);
					m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[i]);
				}

				//同伴扑克
				if (i==(wMeChairID+2)%GAME_PLAYER)
				{
					//设置扑克
					if (m_cbHandCardCount[wMeChairID]==0)
					{
						//显示设置
						bool bDisplayItem=((IsLookonMode()==false)||(IsAllowLookon()==true));
						m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayItem(bDisplayItem);

						//设置扑克
						m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[i],m_cbSortType);
						m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[i]);
					}
					else m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardCount[i]);
				}

				//玩家扑克
				if ((i==(wMeChairID+1)%GAME_PLAYER)||(i==(wMeChairID+3)%GAME_PLAYER))
				{
					m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardCount[i]);
				}

				//设置名次
				m_GameClientView.SetUserPersist(wViewChairID,pStatusPlay->wPersistInfo[i]);
			}

			//出牌界面
			if (pStatusPlay->wTurnWiner!=INVALID_CHAIR)
			{
				WORD wViewChairID=SwitchViewChairID(pStatusPlay->wTurnWiner);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbTurnCardData,m_cbTurnCardCount);
			}

			//玩家控制
			if (IsLookonMode()==false)	
			{
				//启用按钮
				m_GameClientView.m_btStustee.EnableWindow(TRUE);
				m_GameClientView.m_btSort.EnableWindow(TRUE);
				m_GameClientView.m_btSortCardColor.EnableWindow(TRUE);		
				m_GameClientView.m_btSortCardCount.EnableWindow(TRUE);
				m_GameClientView.m_btSortCardOrder.EnableWindow(TRUE);
			}

			//控制设置
			bool bPositively=(IsLookonMode()==false);
			bool bDisplayItem=((IsLookonMode()==false)||(IsAllowLookon()==true));
			m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetPositively(bPositively);
			m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetDisplayItem(bDisplayItem);

			//当前玩家
			if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
			{
				//显示按钮
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				//m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);

				//启用按钮
				m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
				m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
			}

			//扑克控制
			if (IsLookonMode()==true) m_GameClientView.m_HandCardControl[2].SetDisplayItem((IsAllowLookon()==true));

			//更新界面
			m_GameClientView.UpdateGameView(NULL);

			//设置时间
			if (m_wCurrentUser!=INVALID_CHAIR) SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

			return true;
		}
	case GAME_SCENE_WAIT:		//等待状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusWait));
			if (wDataSize!=sizeof(CMD_S_StatusWait)) return false;

			//变量定义
			CMD_S_StatusWait * pStatusWait=(CMD_S_StatusWait *)pData;

			//变量定义
			WORD wMeChairID=GetMeChairID();

			//设置变量
			m_cbMainValue=pStatusWait->cbMainValue;
			CopyMemory(m_cbValueOrder,pStatusWait->cbValueOrder,sizeof(m_cbValueOrder));

			//设置界面
			m_GameClientView.SetCellScore(pStatusWait->lCellScore);
			m_GameClientView.SetMainValue(pStatusWait->cbMainValue);
			m_GameClientView.SetUserValueOrder(m_cbValueOrder[wMeChairID%2],m_cbValueOrder[(wMeChairID+1)%2]);

			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				m_bAutoStatus[i]=pStatusWait->bAutoStatus[i];
			}

			m_GameClientView.SetAutoUser(m_bAutoStatus);

			//玩家设置
			if (IsLookonMode()==false)
			{
				//启用按钮
				m_GameClientView.m_btStart.ShowWindow(TRUE);
				m_GameClientView.m_btStustee.EnableWindow(TRUE);

				//设置时间
				SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
			}

			//设置开始
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				if (pStatusWait->cbContinue[i]==TRUE)
				{
					m_GameClientView.SetUserContinue(SwitchViewChairID(i),true);
				}
			}

			//设置界面
			m_GameClientView.SetCellScore(pStatusWait->lCellScore);

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	//设置状态
	SetGameStatus(GAME_SCENE_PLAY);

	//游戏变量
	m_wTimeOutCount=0;
	m_wCurrentUser=pGameStart->wCurrentUser;

	//等级变量
	m_cbMainValue=pGameStart->cbMainValue;
	m_cbValueOrder[0]=pGameStart->cbValueOrder[0];
	m_cbValueOrder[1]=pGameStart->cbValueOrder[1];

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));

	//出牌变量
	m_cbTurnCardCount=0;
	m_cbSortType=ST_ORDER;
	m_cbTurnCardType=CT_ERROR;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

	//界面设置
	m_GameClientView.m_ScoreControl.RestorationData();
	m_GameClientView.m_btSortCardColor.EnableWindow(TRUE);
	m_GameClientView.m_btSort.EnableWindow(TRUE);
	m_GameClientView.m_btSortCardCount.EnableWindow(TRUE);
	m_GameClientView.m_btSortCardOrder.EnableWindow(TRUE);

	//设置界面
	WORD wMeChairID=GetMeChairID();
	m_GameClientView.SetUserWinOrder(INVALID_CHAIR,0);
	m_GameClientView.SetUserContinue(INVALID_CHAIR,false);
	m_GameClientView.SetMainValue(pGameStart->cbMainValue);
	m_GameClientView.SetUserValueOrder(m_cbValueOrder[wMeChairID%2],m_cbValueOrder[(wMeChairID+1)%2]);

	//历史记录
	m_HistoryCard.ResetData();

	//设置逻辑
	m_GameLogic.SetMainValue(m_cbMainValue);

	//玩家扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取位置
		WORD wViewChairID=SwitchViewChairID(i);

		//设置扑克
		if (wViewChairID==MYSELF_VIEW_ID)
		{
			bool bShowCard=((IsLookonMode()==false)||(IsAllowLookon()==true));
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetPositively(false);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayItem(bShowCard);
		}
		else
		{
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayItem(false);
		}

		//设置名次
		m_GameClientView.SetUserPersist(wViewChairID,pGameStart->wPersistInfo[i]);
	}

	//设置扑克
	//for (WORD i=0;i<GAME_PLAYER;i++) m_cbHandCardCount[i]=CountArray(pGameStart->cbCardData);
	m_cbHandCardCount[pGameStart->wCurrentUser]=m_cbHandCardCount[(pGameStart->wCurrentUser+1)%GAME_PLAYER]=41;
	m_cbHandCardCount[(pGameStart->wCurrentUser+2)%GAME_PLAYER]=m_cbHandCardCount[(pGameStart->wCurrentUser+3)%GAME_PLAYER]=40;


	CopyMemory(m_cbHandCardData,pGameStart->cbCardData,sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);

	//排列扑克
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],m_cbSortType);

	//派发扑克
	//////////////////////////////////////////////////////////////////////////
	m_GameClientView.m_HandCardControl[SwitchViewChairID((m_wCurrentUser+0)%GAME_PLAYER)].SetCardData(41);
	m_GameClientView.m_HandCardControl[SwitchViewChairID((m_wCurrentUser+1)%GAME_PLAYER)].SetCardData(41);
	m_GameClientView.m_HandCardControl[SwitchViewChairID((m_wCurrentUser+2)%GAME_PLAYER)].SetCardData(40);
	m_GameClientView.m_HandCardControl[SwitchViewChairID((m_wCurrentUser+3)%GAME_PLAYER)].SetCardData(40);

	if(GetMeChairID()==((m_wCurrentUser+0)%GAME_PLAYER)||GetMeChairID()==((m_wCurrentUser+1)%GAME_PLAYER))
		m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetCardData(m_cbHandCardData,41);
	if(GetMeChairID()==((m_wCurrentUser+2)%GAME_PLAYER)||GetMeChairID()==((m_wCurrentUser+3)%GAME_PLAYER))
		m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetCardData(m_cbHandCardData,40);

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		//m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
	}

	//时间设置
	SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD+20);

	//环境设置
	if ((IsLookonMode()==false)&&(m_bStustee==false)) ActiveGameFrame();

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//玩家设置
	if (IsLookonMode()==false) m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetPositively(true);

	//////////////////////////////////////////////////////////////////////////

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//环境设置
	if ((IsLookonMode()==false)&&(m_bStustee==false)) ActiveGameFrame();

	return true;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pData, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);

	//效验数据
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE)))) return false;

	//环境切换
	SwitchToCurrentCard();
	DoncludeDispatchCard();

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillTimer(IDI_HIDE_CARD);
	KillGameTimer(IDI_OUT_CARD);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);
	bool bPlayGameSound=((IsLookonMode()==false)&&(wMeChairID==pOutCard->wOutCardUser));

	m_GameLogic.SetFirst2(false);
	//历史清理
	if (m_cbTurnCardCount==0)
	{
		//用户扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//保存数据
			m_HistoryCard.OnEventSaveData(i);

			//桌面清理
			if (i!=pOutCard->wOutCardUser)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(0);
			}
		}

		//放弃标志
		m_GameClientView.SetUserPass(INVALID_CHAIR,false);
		/////////////////////////////
		//2的先后出大小
		if(m_cbTurnCardCount==0)
		{
			m_GameLogic.SetFirst2(true);
		} 
		/////////////////////////////
	}

	//出牌变量
	m_wCurrentUser=pOutCard->wCurrentUser;
	m_cbTurnCardCount=pOutCard->cbCardCount;
	CopyMemory(m_cbTurnCardData,pOutCard->cbCardData,pOutCard->cbCardCount*sizeof(BYTE));

	//变幻扑克
	m_GameLogic.MagicCardData(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbMagicCardData,m_cbTurnCardType);

	//扑克类型
	m_cbTurnCardType=m_GameLogic.GetCardType(m_cbMagicCardData,pOutCard->cbCardCount);

	//if(m_GameLogic.GetCardValue(pOutCard->cbCardData[0])!=2)
	//{
	//	m_GameLogic.SetFirst2(false);
	//}


	//炸弹判断
	if (m_cbTurnCardType>=CT_TONG_HUA_SHUN)
	{
		//播放动画
		m_GameClientView.SetBombEffect(true);

		//播放声音
		if (bPlayGameSound==false)
		{
			bPlayGameSound=true;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB"));
		}
	}

	//出牌设置
	if ((IsLookonMode()==true)||(wMeChairID!=pOutCard->wOutCardUser))
	{
		//出牌界面
		m_HistoryCard.OnEventUserOutCard(pOutCard->wOutCardUser,pOutCard->cbCardData,pOutCard->cbCardCount);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(pOutCard->cbCardData,pOutCard->cbCardCount);

		//删除扑克
		if (wViewChairID==MYSELF_VIEW_ID)
		{
			//获取扑克
			BYTE cbHandCardData[MAX_COUNT];
			BYTE cbHandCardCount=(BYTE)m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].GetCardData(cbHandCardData,CountArray(cbHandCardData));

			//删除扑克
			m_cbHandCardCount[wMeChairID]-=pOutCard->cbCardCount;
			m_GameLogic.RemoveCard(pOutCard->cbCardData,pOutCard->cbCardCount,cbHandCardData,cbHandCardCount);
			m_GameLogic.RemoveCard(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbHandCardData,cbHandCardCount);

			//设置扑克
			m_GameLogic.SortCardList(cbHandCardData,m_cbHandCardCount[wMeChairID],m_cbSortType);
			m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetCardData(cbHandCardData,m_cbHandCardCount[wMeChairID]);
		}
		else
		{
			//好友玩家
			if ((m_cbHandCardCount[wMeChairID]==0)&&((pOutCard->wOutCardUser+2)%GAME_PLAYER==wMeChairID))
			{
				//删除扑克
				WORD wPardnerUser=(wMeChairID+2)%GAME_PLAYER;
				BYTE cbSourceCount=m_cbHandCardCount[wPardnerUser];
				m_cbHandCardCount[wPardnerUser]-=pOutCard->cbCardCount;
				m_GameLogic.RemoveCard(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbHandCardData,cbSourceCount);

				//设置扑克
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[wPardnerUser]);
			}
			else
			{
				//普通玩家
				m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardCount[pOutCard->wOutCardUser]);
			}
		}
	}

	//设置名次
	if (m_cbHandCardCount[pOutCard->wOutCardUser]==0)
	{
		m_wWinOrder[m_wWinCount++]=pOutCard->wOutCardUser;
		m_GameClientView.SetUserWinOrder(SwitchViewChairID(pOutCard->wOutCardUser),m_wWinCount);
	}

	//本轮分数
	m_TurnScore=pOutCard->TurnScore;
	int tempScore[4];
	for(WORD i=0;i<GAME_PLAYER;i++)	tempScore[SwitchViewChairID(i)]=m_PlayerScore[i];
	m_GameClientView.SetScoreInfo(m_TurnScore,tempScore);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//最大判断
	if (m_wCurrentUser==pOutCard->wOutCardUser)
	{
		//设置变量
		m_cbTurnCardCount=0;
		m_wCurrentUser=INVALID_CHAIR;
		m_wMostUser=pOutCard->wCurrentUser;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
		ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));

		//放弃动作
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//用户过虑
			if (pOutCard->wOutCardUser==i) continue;

			//放弃界面
			WORD wViewChairID=SwitchViewChairID(i);
			m_GameClientView.SetUserPass(wViewChairID,true);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);

			//放弃记录
			m_HistoryCard.OnEventSaveData(i);
			m_HistoryCard.OnEventUserPass(i);
		}

		//播放声音
		if (bPlayGameSound==false)
		{
			bPlayGameSound=true;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));
		}

		//设置定时器
		SetTimer(IDI_MOST_CARD,3000,NULL);

		return true;
	}

	//玩家设置
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//保存数据
		m_HistoryCard.OnEventSaveData(m_wCurrentUser);

		//变量定义
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);

		//门前清理
		m_GameClientView.SetUserPass(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		//激活框架
		if (m_bStustee==false) ActiveGameFrame();

		//显示按钮
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		//m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
	}

	//播放声音
	if ((bPlayGameSound==false)&&((IsLookonMode()==true)||(wMeChairID!=pOutCard->wOutCardUser)))
	{
		bPlayGameSound=true;
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	}

	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		if (m_cbTurnCardCount!=0)
		{
			BYTE cbCardCount=m_cbHandCardCount[m_wCurrentUser];
			SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);
		}
		else SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);
	}

	return true;
}

//用户放弃
bool CGameClientDlg::OnSubPassCard(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PassCard));
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;

	//变量定义
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pData;

	//本轮分数
	m_TurnScore=pPassCard->TurnScore;
	CopyMemory(m_PlayerScore,pPassCard->PlayerScore,sizeof(m_PlayerScore));
	int tempScore[4];
	for(WORD i=0;i<GAME_PLAYER;i++)	tempScore[SwitchViewChairID(i)]=m_PlayerScore[i];
	m_GameClientView.SetScoreInfo(m_TurnScore,tempScore);

	//当前扑克
	SwitchToCurrentCard();

	//删除时间
	KillGameTimer(IDI_OUT_CARD);

	//设置变量
	m_wCurrentUser=pPassCard->wCurrentUser;
	
	//放弃设置
	if ((IsLookonMode()==true)||(pPassCard->wPassCardUser!=GetMeChairID()))
	{
		WORD wPassCardUser=pPassCard->wPassCardUser;
		m_HistoryCard.OnEventUserPass(wPassCardUser);
		m_GameClientView.SetUserPass(SwitchViewChairID(wPassCardUser),true);
	}

	//设置界面
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		//保存数据
		m_HistoryCard.OnEventSaveData(m_wCurrentUser);

		//视图位置
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);

		//设置界面
		m_GameClientView.SetUserPass(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(0);
	}

	//一轮判断
	if (pPassCard->cbTurnOver==TRUE)
	{
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
		ZeroMemory(m_cbMagicCardData,sizeof(m_cbMagicCardData));
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		//激活框架
		if (m_bStustee==false) ActiveGameFrame();

		//设置按钮
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		//m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
	}

	//播放声音
	if ((IsLookonMode()==true)||(pPassCard->wPassCardUser!=GetMeChairID()))
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));
	}

	//设置时间
	if (m_cbTurnCardCount!=0)
	{
		BYTE cbCardCount=m_cbHandCardCount[m_wCurrentUser];
		SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);
	}
	else SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

	return true;
}

//扑克信息
bool CGameClientDlg::OnSubCardInfo(const void * pData, WORD wDataSize)
{
	//变量定义
	CMD_S_CardInfo * pCardInfo=(CMD_S_CardInfo *)pData;
	WORD wHeadSize=sizeof(CMD_S_CardInfo)-sizeof(pCardInfo->cbCardData);

	//效验数据
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pCardInfo->cbCardCount*sizeof(BYTE))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pCardInfo->cbCardCount*sizeof(BYTE)))) return false;

	//设置变量
	WORD wPardnerUser=(GetMeChairID()+2)%GAME_PLAYER;
	m_cbHandCardCount[wPardnerUser]=pCardInfo->cbCardCount;
	CopyMemory(m_cbHandCardData,pCardInfo->cbCardData,sizeof(BYTE)*pCardInfo->cbCardCount);

	//设置扑克
	WORD wPardnerViewID=SwitchViewChairID(wPardnerUser);
	m_GameClientView.m_HandCardControl[wPardnerViewID].SetCardData(m_cbHandCardData,m_cbHandCardCount[wPardnerUser]);

	//显示设置
	if ((IsLookonMode()==false)||(IsAllowLookon()==true))
	{
		m_GameClientView.m_HandCardControl[wPardnerViewID].SetDisplayItem(true);
	}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//变量定义
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

	//本轮分数
	m_TurnScore=pGameEnd->TurnScore;
	CopyMemory(m_PlayerScore,pGameEnd->PlayerScore,sizeof(m_PlayerScore));
	int tempScore[4];
	for(WORD i=0;i<GAME_PLAYER;i++)	tempScore[SwitchViewChairID(i)]=m_PlayerScore[i];
	m_GameClientView.SetScoreInfo(m_TurnScore,tempScore);

	//设置状态
	SetGameStatus(GAME_SCENE_FREE);

	//删除时间
	KillTimer(IDI_MOST_CARD);
	KillTimer(IDI_HIDE_CARD);
	KillTimer(IDI_DISPATCH_CARD);

	//游戏时间
	KillGameTimer(IDI_OUT_CARD);

	//控制按钮
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);

	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btLastTurn.EnableWindow(FALSE);
	m_GameClientView.m_btSort.EnableWindow(FALSE);
	m_GameClientView.m_btSortCardColor.EnableWindow(FALSE);
	m_GameClientView.m_btSortCardCount.EnableWindow(FALSE);
	m_GameClientView.m_btSortCardOrder.EnableWindow(FALSE);

	//设置扑克
	m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetPositively(false);

	//变量定义
	tagScoreInfoC ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

	//成绩用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		const tagUserData * pUserData=GetUserData(i);
		if (pUserData==NULL) continue;

		//设置成绩
		ScoreInfo.lGameScore[i]=pGameEnd->lGameScore[i];

		//设置名字
		lstrcpyn(ScoreInfo.szUserName[i],pUserData->szName,CountArray(ScoreInfo.szUserName[i]));
	}

	//设置界面
	m_GameClientView.SetUserPass(INVALID_CHAIR,false);
	m_GameClientView.SetUserContinue(INVALID_CHAIR,false);
	m_GameClientView.m_ScoreControl.SetScoreInfo(ScoreInfo);

	//设置界面
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//视图位置
		WORD wViewChairID=SwitchViewChairID(i);

		//设置扑克
		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayItem(true);
		m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);

		//设置名次
		m_GameClientView.SetUserPersist(wViewChairID,pGameEnd->wPersistInfo[i]);
	}

	//播放声音
	LONGLONG lMeScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOSE"));
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));

	//开始设置
	if (IsLookonMode()==false)
	{
		if(m_bStustee==true) OnMessageStustee(0,0);
		//显示按钮
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		m_GameClientView.m_btStart.SetFocus();

		//设置时间
		SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return true;
}


//继续游戏
bool CGameClientDlg::OnSubContinueGame(const void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ContinueGame));
	if (wDataSize!=sizeof(CMD_S_ContinueGame)) return false;

	//变量定义
	CMD_S_ContinueGame * pContinueGame=(CMD_S_ContinueGame *)pData;

	//设置界面
	WORD wChairID=pContinueGame->wChairID;
	m_GameClientView.SetUserContinue(SwitchViewChairID(wChairID),true);

	return true;
}

//出牌判断
bool CGameClientDlg::VerdictOutCard()
{
	//状态判断
	if (GetGameStatus()!=GAME_SCENE_PLAY) return false;
	if (m_wCurrentUser!=GetMeChairID()) return false;

	//获取扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbCardData,CountArray(cbCardData));

	//变幻扑克
	BYTE cbMagicCardData[MAX_COUNT];
	m_GameLogic.MagicCardData(cbCardData,cbShootCount,cbMagicCardData,m_cbTurnCardType);

	//出牌判断
	if (cbShootCount>0L)
	{
		//类型判断
		m_GameLogic.SortCardList(cbMagicCardData,cbShootCount,ST_ORDER);
		if (m_GameLogic.GetCardType(cbMagicCardData,cbShootCount)==CT_ERROR) return false;

		//跟牌判断
		if (m_cbTurnCardCount==0) return true;
		return m_GameLogic.CompareCard(m_cbMagicCardData,cbMagicCardData,m_cbTurnCardCount,cbShootCount);
	}

	return false;
}

//放弃判断
bool CGameClientDlg::VerdictPassCard()
{
	//放弃判断
	if (m_cbTurnCardCount==0) return false;

	return true;
}

//自动开始
bool CGameClientDlg::PerformAutoStart()
{
	//开始设置
	WORD wMeChairID=GetMeChairID();
	SetGameTimer(wMeChairID,IDI_START_GAME,TIME_START_GAME);

	return true;
	//变量定义
	WORD wTableUserCount=0,wReadyUserCount=0;
	//IClientUserItem * pIMySelfUserItem=GetMeUserItem();
	const tagUserData * pMeUserData=GetUserData(GetMeChairID());

	//状态判断
	if ((IsLookonMode()==true)/*||(m_pIClientKernel->GetTimeID()==IDI_START_GAME)*/) return false;
	if ((pMeUserData==NULL)||(pMeUserData->cbUserStatus==US_READY)) return false;

	//统计用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		const tagUserData * pUserData=GetUserData(i);

		//数目统计
		if (pUserData!=NULL) 
		{
			wTableUserCount++;
			if (pUserData->cbUserStatus==US_READY) wReadyUserCount++;
		}
	}

	//开始判断
	if ((wTableUserCount==GAME_PLAYER)&&((wReadyUserCount+1)==wTableUserCount))
	{
		//开始设置
		WORD wMeChairID=GetMeChairID();
		SetGameTimer(wMeChairID,IDI_START_GAME,TIME_START_GAME);

		return true;
	}

	return false;
}

//自动出牌
bool CGameClientDlg::AutomatismOutCard()
{
	//状态判断
	if (GetGameStatus()!=GAME_SCENE_PLAY) return false;
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID())) return false;

	//当前弹起
	CSkinButton & btOutCard=m_GameClientView.m_btOutCard;
	if ((btOutCard.IsWindowEnabled())&&(btOutCard.IsWindowVisible()))
	{
		OnMessageOutCard(0,0);
		return true;
	}

	//出牌处理
	if ((m_cbTurnCardCount==0)/*||(m_bStustee==true)*/)
	{
		//分析结果
		tagOutCardResult OutCardResult;
		m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],m_cbMagicCardData,m_cbTurnCardCount,OutCardResult);

		//设置界面
		if (OutCardResult.cbCardCount>0)
		{
			//设置界面
			m_GameClientView.m_btOutCard.EnableWindow(TRUE);
			m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

			//出牌动作
			OnMessageOutCard(0,0);

			return true;
		}
	}

	//放弃出牌
	if (m_cbTurnCardCount>0) OnMessagePassCard(0,0);

	return true;
}

//当前扑克
bool CGameClientDlg::SwitchToCurrentCard()
{
	//处理判断
	if (m_bLastTurn==false) return false;

	//环境设置
	m_bLastTurn=false;
	KillTimer(IDI_LAST_TURN);
	m_GameClientView.SetLastTurnCard(false);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);
		BYTE cbEventFlag=m_HistoryCard.GetCurrentEvent(i);

		//获取扑克
		BYTE cbCardData[MAX_COUNT];
		BYTE cbCardCount=m_HistoryCard.GetCurrentCard(i,cbCardData,CountArray(cbCardData));

		//设置界面
		m_GameClientView.SetUserPass(wViewChairID,cbEventFlag==AF_PASS);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(cbCardData,cbCardCount);
	}

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//停止发牌
bool CGameClientDlg::DoncludeDispatchCard()
{
	//停止发牌
	if (m_cbRemnantCardCount>0)
	{
		//删除时间
		KillTimer(IDI_DISPATCH_CARD);

		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//获取位置
			WORD wViewChairID=SwitchViewChairID(i);

			//设置扑克
			if (wViewChairID==MYSELF_VIEW_ID)
			{
				m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[i],m_cbSortType);
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[i]);
			}
			else m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardCount[i]);
		}

		//玩家设置
		if (IsLookonMode()==false) m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetPositively(true);

		//设置变量
		m_cbRemnantCardCount=0;
		m_cbDispatchCardCount=0;
		ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

		//更新界面
		m_GameClientView.UpdateGameView(NULL);
	}

	return true;
}

//派发扑克
bool CGameClientDlg::DispatchUserCard(BYTE cbCardData[], BYTE cbCardCount)
{
	//设置变量
	m_cbDispatchCardCount=0;
	m_cbRemnantCardCount=cbCardCount;
	CopyMemory(m_cbDispatchCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//设置定时器
	SetTimer(IDI_DISPATCH_CARD,100,NULL);

	return true;
}

//定时器消息
VOID CGameClientDlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case IDI_MOST_CARD:			//最大扑克
		{
			//用户效验
			ASSERT(m_wMostUser!=INVALID_CHAIR);
			if (m_wMostUser==INVALID_CHAIR) return;

			//设置变量
			m_wCurrentUser=m_wMostUser;
			m_wMostUser=INVALID_CHAIR;

			//删除时间
			KillTimer(IDI_MOST_CARD);

			//设置界面
			m_GameClientView.SetUserPass(INVALID_CHAIR,false);
			for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(0);

			//上轮设置
			for (WORD i=0;i<GAME_PLAYER;i++) m_HistoryCard.OnEventSaveData(i);

			//玩家设置
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				//激活框架
				if (m_bStustee==false) ActiveGameFrame();

				//设置按钮
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				//m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
				m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
			}

			//设置时间
			SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD+20);

			return;
		}
	case IDI_HIDE_CARD:			//隐藏扑克
		{
			//删除时间
			KillTimer(IDI_HIDE_CARD);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_UserCardControl[i].SetCardData(0);
			}

			//更新界面
			m_GameClientView.UpdateGameView(NULL);

			return;
		}
	case IDI_LAST_TURN:			//上轮扑克
		{
			//还原界面
			SwitchToCurrentCard();

			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//开始消息
LRESULT CGameClientDlg::OnMessageStart(WPARAM wParam, LPARAM lParam)
{
	//删除时间
	KillGameTimer(IDI_START_GAME);

	//用户扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetDisplayItem(false);
	}

	//用户扑克
	m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetPositively(false);

	//设置界面
	m_GameClientView.SetUserWinOrder(INVALID_CHAIR,0);
	m_GameClientView.SetUserPass(INVALID_CHAIR,false);
	m_GameClientView.SetUserContinue(INVALID_CHAIR,false);

	//界面设置
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreControl.RestorationData();

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//用户信息
	WORD wMeChairID=GetMeChairID();
	const tagUserData * pUserData=GetUserData(GetMeChairID());

	//发送消息
	if (pUserData->cbUserStatus!=US_PLAY)
	{
		SendUserReady(NULL,0);
	}
	else
	{
		SendData(SUB_C_CONTINUE_GAME);
	}

	return 0;
}

//拖管控制
LRESULT CGameClientDlg::OnMessageStustee(WPARAM wParam, LPARAM lParam)
{
	//资源变量
	HINSTANCE hResInstance=AfxGetInstanceHandle();

	//设置状态
	if (m_bStustee==true)
	{
		m_bStustee=false;
		m_GameClientView.m_btStustee.SetButtonImage(IDB_BT_START_TRUSTEE,hResInstance,false);
	}
	else
	{
		m_bStustee=true;
		m_GameClientView.m_btStustee.SetButtonImage(IDB_BT_STOP_TRUSTEE,hResInstance,false);
	}

	CMD_C_Automatism AutomatismData;
	AutomatismData.bAutomatism = m_bStustee;

	SendData(SUB_C_AUTOMATISM, &AutomatismData, sizeof(AutomatismData));

	return 0;
}

//排列扑克
LRESULT CGameClientDlg::OnMessageSortCard(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_cbSortType=(BYTE)wParam;

	//理牌消息
	if(m_cbSortType==255)
	{
		//获取扑克
		BYTE cbShootCard[MAX_COUNT];
		BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbShootCard,CountArray(cbShootCard));

		//无弹起返回
		if(cbShootCount==0) return 0;

		//保存扑克
		BYTE cbTempCard[MAX_COUNT];
		BYTE cbTempCount=MAX_COUNT;

		//移除弹起
		m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].RemoveShootItem();
		cbTempCount=(BYTE)m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].GetCardData(cbTempCard,cbTempCount);
		
		//重新排序
		CopyMemory(m_cbHandCardData,cbShootCard,sizeof(BYTE)*cbShootCount);
		CopyMemory(&m_cbHandCardData[cbShootCount],cbTempCard,sizeof(BYTE)*m_cbHandCardCount[GetMeChairID()]-cbShootCount);
				
		//设置数据
		m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetCardData(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()]);

		m_GameClientView.m_btOutCard.EnableWindow(false);
		//更新界面
		m_GameClientView.UpdateGameView(NULL);
		return 0;
	}

	//排列扑克
	if (m_cbSortType<ST_CUSTOM)
	{
		//排列扑克
		WORD wMeChairID=GetMeChairID();
		m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],m_cbSortType);

		//获取扑克
		BYTE cbShootCard[MAX_COUNT];
		BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbShootCard,CountArray(cbShootCard));

		//设置扑克
		if (m_cbRemnantCardCount>0)
		{
			m_GameLogic.SortCardList(m_cbDispatchCardData,m_cbDispatchCardCount,m_cbSortType);
			m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetCardData(m_cbDispatchCardData,m_cbDispatchCardCount);
		}
		else m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetCardData(m_cbHandCardData,m_cbHandCardCount[wMeChairID]); 

		//提起扑克
		m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(cbShootCard,cbShootCount);

		//更新界面
		m_GameClientView.UpdateGameView(NULL);
	}

	//排列扑克
	if (m_cbSortType==ST_QINGDUN)
	{
		//保存数据
		BYTE tempCardData[MAX_COUNT];ZeroMemory(tempCardData,sizeof(tempCardData));
		CopyMemory(tempCardData,m_cbHandCardData,sizeof(BYTE)*m_cbHandCardCount[GetMeChairID()]);

		//先按大小排序
		m_GameLogic.SortCardList(tempCardData,m_cbHandCardCount[GetMeChairID()],ST_ORDER);

		//设置弹起
		BYTE bShootCount=0;
		BYTE bShootCardData[MAX_COUNT];ZeroMemory(bShootCardData,sizeof(bShootCardData));

		//搜索清墩
		for(int i=0;i<m_cbHandCardCount[GetMeChairID()]-2;i++)
		{
			if(tempCardData[i]==tempCardData[i+2])
			{
				bShootCardData[bShootCount++]=tempCardData[i];
				bShootCardData[bShootCount++]=tempCardData[i+1];
				bShootCardData[bShootCount++]=tempCardData[i+2];
				i=i+2;
			}
		}
		m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(bShootCardData,bShootCount);

		//当前弹起可以出牌
		if(m_wCurrentUser==GetMeChairID()&&m_GameClientView.m_btOutCard.IsWindowVisible()==TRUE)
		{
			if(VerdictOutCard()==true)
			{
				m_GameClientView.m_btOutCard.EnableWindow(true);
			}
		}

		//更新界面
		m_GameClientView.UpdateGameView(NULL);
		return 0;
	}

	//排列扑克
	if (m_cbSortType==ST_TONGHUA)
	{
		BYTE tempCardData[MAX_COUNT];ZeroMemory(tempCardData,sizeof(tempCardData));
		CopyMemory(tempCardData,m_cbHandCardData,sizeof(BYTE)*m_cbHandCardCount[GetMeChairID()]);

		m_GameLogic.SortCardList(tempCardData,m_cbHandCardCount[GetMeChairID()],ST_ORDER);

		BYTE bShootCount=0;
		BYTE bShootCardData[MAX_COUNT];ZeroMemory(bShootCardData,sizeof(bShootCardData));

		if(m_bTongHuaCount>0)
		{
			bShootCount=5;
			CopyMemory(bShootCardData,m_bTongHuaCard[m_bTongHuaIndex],sizeof(BYTE)*5);
			m_bTongHuaIndex++;
			if(m_bTongHuaIndex>=m_bTongHuaCount) m_bTongHuaIndex=0;

			//设置弹起
			m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(bShootCardData,bShootCount);

			//当前弹起可以出牌
			if(m_wCurrentUser==GetMeChairID()&&m_GameClientView.m_btOutCard.IsWindowVisible()==TRUE)
			{
				if(VerdictOutCard()==true)
				{
					m_GameClientView.m_btOutCard.EnableWindow(true);
				}
			}

			//更新界面
			m_GameClientView.UpdateGameView(NULL);
			return 0;
		}

		//分析分布
		tagDistributing Distribute;
		m_GameLogic.AnalysebDistributing(tempCardData,m_cbHandCardCount[GetMeChairID()],Distribute);
		//拷贝A的数据
		CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(Distribute.cbDistributing[0]));


		for(BYTE i=0;i<10;i++)
		{
			for(BYTE j=0;j<4;j++)
			{
				//此处没有牌
				if( Distribute.cbDistributing[i][j]==0) continue;

				if(Distribute.cbDistributing[i+1][j]>=1)
				if(Distribute.cbDistributing[i+2][j]>=1)
				if(Distribute.cbDistributing[i+3][j]>=1)
				if(Distribute.cbDistributing[i+4][j]>=1)
				{
					BYTE bTongHuaCard[5];
					bTongHuaCard[0]=m_GameLogic.MakeCardData(i,j);
					bTongHuaCard[1]=m_GameLogic.MakeCardData(i+1,j);
					bTongHuaCard[2]=m_GameLogic.MakeCardData(i+2,j);
					bTongHuaCard[3]=m_GameLogic.MakeCardData(i+3,j);
					bTongHuaCard[4]=m_GameLogic.MakeCardData(i+4,j);
					if((i+4)==13) bTongHuaCard[4]=m_GameLogic.MakeCardData(0,j);				
					CopyMemory(m_bTongHuaCard[m_bTongHuaCount++],bTongHuaCard,sizeof(BYTE)*5);
				}
			}
		}

		if(m_bTongHuaCount>0)
		{
			bShootCount=5;
			CopyMemory(bShootCardData,m_bTongHuaCard[m_bTongHuaIndex],sizeof(BYTE)*5);
			m_bTongHuaIndex++;
		}
		//设置弹起
		m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(bShootCardData,bShootCount);

		//当前弹起可以出牌
		if(m_wCurrentUser==GetMeChairID()&&m_GameClientView.m_btOutCard.IsWindowVisible()==TRUE)
		{
			if(VerdictOutCard()==true)
			{
				m_GameClientView.m_btOutCard.EnableWindow(true);
			}
		}

		//更新界面
		m_GameClientView.UpdateGameView(NULL);
	}
	return 0;
}

//查看上轮
LRESULT CGameClientDlg::OnMessageLastTurn(WPARAM wParam, LPARAM lParam)
{
	if (m_bLastTurn==false)
	{
		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//变量定义
			WORD wViewChairID=SwitchViewChairID(i);
			BYTE cbEventFlag=m_HistoryCard.GetHistoryEvent(i);

			//获取扑克
			BYTE cbCardData[MAX_COUNT];
			BYTE cbCardCount=m_HistoryCard.GetHistoryCard(i,cbCardData,CountArray(cbCardData));

			//设置界面
			m_GameClientView.SetUserPass(wViewChairID,cbEventFlag==AF_PASS);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(cbCardData,cbCardCount);
		}

		//环境设置
		m_bLastTurn=true;
		SetTimer(IDI_LAST_TURN,3000,NULL);
		m_GameClientView.SetLastTurnCard(true);

		//更新界面
		m_GameClientView.UpdateGameView(NULL);
	}
	else SwitchToCurrentCard();

	return 0;
}

//出牌消息
LRESULT CGameClientDlg::OnMessageOutCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE) return 0;
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return 0;

	//删除时间
	KillTimer(IDI_HIDE_CARD);
	KillGameTimer(IDI_OUT_CARD);

	//设置界面
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	//m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);

	//清除同花提示
	m_bTongHuaCount=0;
	m_bTongHuaIndex=0;
	ZeroMemory(m_bTongHuaCard,sizeof(m_bTongHuaCard));

	//获取扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=(BYTE)m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbCardData,CountArray(cbCardData));

	//排列扑克
	m_GameLogic.SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	BYTE cbHandCardCount=m_cbHandCardCount[wMeChairID];

	//删除扑克
	m_cbHandCardCount[wMeChairID]-=cbCardCount;
	m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData,cbHandCardCount);

	//手上扑克
	if (m_cbSortType<ST_CUSTOM)
	{
		m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],m_cbSortType);
		m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetCardData(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);
	}
	else
	{
		m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].RemoveShootItem();
	}

	//出牌界面
	m_GameClientView.m_UserCardControl[MYSELF_VIEW_ID].SetCardData(cbCardData,cbCardCount);

	//历史扑克
	m_HistoryCard.OnEventUserOutCard(GetMeChairID(),cbCardData,cbCardCount);

	//变幻扑克
	BYTE cbMagicCardData[MAX_COUNT];
	m_GameLogic.MagicCardData(cbCardData,cbCardCount,cbMagicCardData,m_cbTurnCardType);

	//播放声音
	BYTE cbCardType=m_GameLogic.GetCardType(cbMagicCardData,cbCardCount);
	PlayGameSound(AfxGetInstanceHandle(),(cbCardType>=CT_TONG_HUA_SHUN)?TEXT("BOMB"):TEXT("OUT_CARD"));

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardCount=cbCardCount;
	CopyMemory(OutCard.cbCardData,cbCardData,cbCardCount);
	SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)-sizeof(OutCard.cbCardData)+OutCard.cbCardCount*sizeof(BYTE));

	return 0;
}

//PASS消息
LRESULT CGameClientDlg::OnMessagePassCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btPassCard.IsWindowEnabled()==FALSE) return 0;
	if (m_GameClientView.m_btPassCard.IsWindowVisible()==FALSE) return 0;

	//删除时间
	KillGameTimer(IDI_OUT_CARD);

	//设置界面
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	//m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));

	//放弃处理
	WORD wMeChairID=GetMeChairID();
	m_HistoryCard.OnEventUserPass(wMeChairID);
	m_GameClientView.SetUserPass(MYSELF_VIEW_ID,true);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//发送数据
	SendData(SUB_C_PASS_CARD);

	return 0;
}

//提示消息
LRESULT CGameClientDlg::OnMessageOutPrompt(WPARAM wParam, LPARAM lParam)
{
	//分析结果
	tagOutCardResult OutCardResult;
	bool bValid = m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],m_cbMagicCardData,m_cbTurnCardCount,OutCardResult);

	//设置界面
	if (bValid)
	{
		//设置界面
		m_GameClientView.m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

		//设置控件
		bool bOutCard=VerdictOutCard();
		m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);

		//更新界面
		m_GameClientView.UpdateGameView(NULL);
	}
	else 
	{
		//放弃出牌
		OnMessagePassCard(0,0);
	}

	return 0;
}

//左键扑克
LRESULT CGameClientDlg::OnMessageLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	//出牌状态
	if (m_GameClientView.m_btOutCard.IsWindowVisible())
	{
		//出牌判断
		bool bOutCard=VerdictOutCard();
		m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);

		return 0;
	}

	return 0;
}

//启用上轮
LRESULT CGameClientDlg::OnMessageEnableHistory(WPARAM wParam, LPARAM lParam)
{
	//启用上轮
	m_GameClientView.m_btLastTurn.EnableWindow(TRUE);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
