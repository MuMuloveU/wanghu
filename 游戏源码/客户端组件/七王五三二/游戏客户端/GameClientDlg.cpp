#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏定时器
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_MOST_CARD					201								//最大定时器
#define IDI_START_GAME					202								//开始定时器
#define IDI_JIU                         203								//揪人定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_OUT_PROMPT,OnOutPrompt)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_bHandCardCount=0;
	m_wBankerUser=INVALID_CHAIR;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	ZeroMemory(m_bScoreCardCount,sizeof(m_bScoreCardCount));
	ZeroMemory(m_bScoreCard[GAME_PLAYER],sizeof(m_bScoreCard[GAME_PLAYER]));

	//配置变量
	m_bDeasilOrder=false;
	m_dwCardHSpace=DEFAULT_PELS;

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
	//设置标题
	SetWindowText(TEXT("天水七王五二三游戏"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置
	m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//配置控件
	m_GameClientView.SetUserOrder(m_bDeasilOrder);
	m_GameClientView.m_HandCardControl.SetCardSpace(m_dwCardHSpace,0,20);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_bHandCardCount=0;
	m_wBankerUser=INVALID_CHAIR;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	ZeroMemory(m_bScoreCardCount,sizeof(m_bScoreCardCount));
	ZeroMemory(m_bScoreCard[GAME_PLAYER],sizeof(m_bScoreCard[GAME_PLAYER]));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;

	//删除定时
	KillGameTimer(0);
	KillTimer(IDI_MOST_CARD);

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_dwCardHSpace=m_dwCardHSpace;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bDeasilOrder=m_GameClientView.IsDeasilOrder();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//获取参数
		m_bDeasilOrder=GameOption.m_bDeasilOrder;
		m_dwCardHSpace=GameOption.m_dwCardHSpace;

		//设置控件
		EnableSound(GameOption.m_bEnableSound);
		m_GameClientView.SetUserOrder(GameOption.m_bDeasilOrder);
		m_GameClientView.m_HandCardControl.SetCardSpace(m_dwCardHSpace,0,20);

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_OUT_CARD:			//用户出牌
		{
			//超时判断
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) AutomatismOutCard();
				return false;
			}

			//播放声音
			if (m_bHandCardCount<m_bTurnCardCount) return true;
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnStart(0,0);
				return false;
			}
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

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
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pBuffer,wDataSize);
		}
	case SUB_S_PASS_CARD:		//放弃出牌
		{
			return OnSubPassCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}

	case SUB_S_BANK:
		{
			return OnSubBank(pBuffer,wDataSize);
		}
	case SUB_S_RECRUIT_CARD:
		{
			return OnSubRecruitCard(pBuffer,wDataSize);
		}
	}

	return true;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_RF_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(TRUE);
				m_GameClientView.m_btStart.SetFocus();
			}
			//设置扑克
			if (IsLookonMode()==false) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

			return true;
		}
	case GS_RF_PLAYING:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_bTurnCardCount=pStatusPlay->bTurnCardCount;
			m_bHandCardCount=pStatusPlay->bCardCount[GetMeChairID()];
			m_bTurnOutType=m_GameLogic.GetCardType(pStatusPlay->bTurnCardData,m_bTurnCardCount);
			CopyMemory(m_bHandCardData,pStatusPlay->bCardData,m_bHandCardCount);
			CopyMemory(m_bTurnCardData,pStatusPlay->bTurnCardData,pStatusPlay->bTurnCardCount);

			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_bCardCount[i]=pStatusPlay->bCardCount[i];
				m_GameClientView.SetCardCount(wViewChairID,pStatusPlay->bCardCount[i]);
				m_GameClientView.m_lAllTurnScore[wViewChairID]=pStatusPlay->lAllTurnScore[i];
				m_GameClientView.m_lLastTurnScore[wViewChairID]=pStatusPlay->lLastTurnScore[i];
			}
			m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

			//玩家设置
			if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{	
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount!=0)?TRUE:FALSE);	
				//m_GameClientView.//m_btOutPrompt.ShowWindow(SW_SHOW);
				//m_GameClientView.//m_btOutPrompt.EnableWindow(TRUE);
			}

			//桌面设置
			if (m_bTurnCardCount!=0)
			{
				WORD wViewChairID=SwitchViewChairID(pStatusPlay->wLastOutUser);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_bTurnCardData,m_bTurnCardCount);
			}

			//设置定时器
			WORD wTimeCount=30;
			if (m_GameClientView.m_btPassCard.IsWindowEnabled()) wTimeCount=30;
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_OUT_CARD,wTimeCount);

			//设置扑克
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
			}

			return true;
		}
	}

	return true;
}
//选庄
bool CGameClientDlg::OnSubBank(const void * pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Bank));
	if (wDataSize!=sizeof(CMD_S_Bank)) return false;
	//设置状态
	SetGameStatus(GS_RF_BANKER);
	//设置选庄数据
	CMD_S_Bank * pBank=(CMD_S_Bank *)pBuffer;
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData= GetUserData(i);
		if(!pUserData) continue;
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(&pBank->bCardData[i],1);
		m_GameClientView.m_UserCardControl[wViewChairID].SetDisplayFlag(true);
		
	}
	m_GameClientView.SetMostFlag(SwitchViewChairID(pBank->Moster),true);
	//设置手中扑克数量
	if(pBank->Moster==GetMeChairID())
		m_bHandCardCount=6;
	else
		m_bHandCardCount=5;
	return true;
}
//补牌
bool CGameClientDlg::OnSubRecruitCard(const void * pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RecruitCard));
	if (wDataSize!=sizeof(CMD_S_RecruitCard)) return false;
	//构造数据
	CMD_S_RecruitCard * pRecruitCard = (CMD_S_RecruitCard *)pBuffer;
	//补牌
	m_GameLogic.CombinationArray(m_bHandCardData,m_bHandCardCount,pRecruitCard->bRecruitCard,pRecruitCard->bRecruitCount);
	m_bHandCardCount=m_bHandCardCount+pRecruitCard->bRecruitCount;
	m_GameLogic.SortCardList(m_bHandCardData,m_bHandCardCount);
	//设置自己的扑克
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	ASSERT(m_bHandCardCount==5);
	//设置玩家分值扑克
	WORD wViewChairID=SwitchViewChairID(pRecruitCard->wCurrentUser);
	WORD wCurrentUser=pRecruitCard->wCurrentUser;

	m_GameLogic.CombinationArray(m_bScoreCard[wCurrentUser],m_bScoreCardCount[wCurrentUser],pRecruitCard->bOneTurnScoreCard,pRecruitCard->bOneTurnScoreCardCount);
	m_bScoreCardCount[wCurrentUser]+=pRecruitCard->bOneTurnScoreCardCount;
	m_GameClientView.m_ScoreCardControl[wViewChairID].SetCardData(m_bScoreCard[wCurrentUser],m_bScoreCardCount[wCurrentUser]);
	//设置当前玩家
	if(pRecruitCard->wCurrentUser==GetMeChairID())
	{
		m_GameClientView.m_btOutCard.ShowWindow(SW_NORMAL);
		m_GameClientView.m_btPassCard.ShowWindow(SW_NORMAL);
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	}
	//补他家牌
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		if(pUserData)
			m_bCardCount[i]=5;
		m_GameClientView.SetCardCount(SwitchViewChairID(i),m_bCardCount[i]);
	}

	return true;


}
//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	//设置游戏状态
	SetGameStatus(GS_PLAYING);
	//清理桌面
	for(BYTE i=0;i<GAME_PLAYER;i++)
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
	//设置庄家
	m_wBankerUser=pGameStart->wBanker;
	m_GameClientView.SetBankerFlag(SwitchViewChairID(m_wBankerUser),true);

	//设置变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));



	//设置界面
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetPassFlag(i,false);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	}
	if (IsLookonMode()==true)
	{
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
		for(WORD i=0;i<GAME_PLAYER-1;i++)m_GameClientView.m_LeaveCardControl[i].SetCardData(NULL,0);	
	}

	//设置扑克
	CopyMemory(m_bHandCardData,pGameStart->bCardData,sizeof(BYTE)*m_bHandCardCount);
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	//扑克数目
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(i==m_wBankerUser)
			m_bCardCount[i]=6;
		else
			m_bCardCount[i]=5;
		m_GameClientView.SetCardCount(SwitchViewChairID(i),m_bCardCount[i]);

	}

	//控制设置
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_HandCardControl.SetPositively(bPlayerMode);
	m_GameClientView.m_HandCardControl.SetDisplayFlag(bPlayerMode);

	//当前玩家
	if ((IsLookonMode()==false)&&(pGameStart->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);

		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);

		//m_GameClientView.//m_btOutPrompt.ShowWindow(SW_SHOW);
		//m_GameClientView.//m_btOutPrompt.EnableWindow(FALSE);
	}
	//设置定时器
	WORD wTimeCount=30;
	if (pGameStart->wCurrentUser!=INVALID_CHAIR) SetGameTimer(pGameStart->wCurrentUser,IDI_OUT_CARD,wTimeCount);

	//环境设置
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->bCardData);

	//效验数据
	ASSERT(wDataSize>=wHeadSize);
	if (wDataSize<wHeadSize) return false;
	ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
	if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameTimer(IDI_OUT_CARD);

	//界面设置
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);
	m_bCardCount[pOutCard->wOutCardUser]-=pOutCard->bCardCount;
	m_GameClientView.SetCardCount(wOutViewChairID,m_bCardCount[pOutCard->wOutCardUser]);

	//出牌设置
	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser))
	{
		m_GameClientView.m_UserCardControl[wOutViewChairID].SetCardData(pOutCard->bCardData,pOutCard->bCardCount);
	}

	//清理桌面
	if (m_bTurnCardCount==0)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			if (i==pOutCard->wOutCardUser) continue;
			m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
		}
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	}

	//记录出牌
	m_bTurnCardCount=pOutCard->bCardCount;
	m_bTurnOutType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);
	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);

	//自己扑克
	if ((IsLookonMode()==true)&&(pOutCard->wOutCardUser==GetMeChairID()))
	{
		//删除扑克 
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=pOutCard->bCardCount;
		m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData,bSourceCount);

		//设置界面
		m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	}

	//最大判断
	if (pOutCard->wCurrentUser==pOutCard->wOutCardUser)
	{
		//设置变量
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_ERROR;
		m_wMostUser=pOutCard->wCurrentUser;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

		//设置界面
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i!=pOutCard->wOutCardUser)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetPassFlag(wViewChairID,true);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			}
		}

		//环境设置
		SetTimer(IDI_MOST_CARD,3000,NULL);
		PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));

		return true;
	}

	//播放声音
	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser)) PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

	//玩家设置
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		WORD wViewChairID=SwitchViewChairID(pOutCard->wCurrentUser);
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pOutCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();	
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow(TRUE);
		//m_GameClientView.//m_btOutPrompt.ShowWindow(SW_SHOW);
		//m_GameClientView.//m_btOutPrompt.EnableWindow(TRUE);
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
	}

	//设置定时器
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		WORD wTimeCount=30;
		if (m_GameClientView.m_btPassCard.IsWindowEnabled()) wTimeCount=30;
		SetGameTimer(pOutCard->wCurrentUser,IDI_OUT_CARD,wTimeCount);
	}

	return true;
}

//放弃出牌
bool CGameClientDlg::OnSubPassCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pBuffer;

	//删除定时器
	KillGameTimer(IDI_OUT_CARD);

	//玩家设置
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=GetMeChairID()))
	{
		WORD wViewChairID=SwitchViewChairID(pPassCard->wPassUser);
		m_GameClientView.SetPassFlag(wViewChairID,true);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//一轮判断
	if (pPassCard->bNewTurn==TRUE)
	{
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_ERROR;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	}

	//设置界面
	WORD wViewChairID=SwitchViewChairID(pPassCard->wCurrentUser);
	m_GameClientView.SetPassFlag(wViewChairID,false);
	m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);

	//玩家设置
	if ((IsLookonMode()==false)&&(pPassCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		//m_GameClientView.//m_btOutPrompt.ShowWindow(SW_SHOW);

		m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount>0)?TRUE:FALSE);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		//m_GameClientView.//m_btOutPrompt.EnableWindow(m_bTurnCardCount>0?TRUE:FALSE);
		
	}



	//播放声音
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=GetMeChairID()))	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

	//设置时间
	if (m_bTurnCardCount!=0)
	{
		WORD wTimeCount=30;
		if (m_GameClientView.m_btPassCard.IsWindowEnabled()) wTimeCount=30;
		SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,wTimeCount);
	}
	else SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,30);

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

	//设置状态
	SetGameStatus(GS_RF_FREE);

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameTimer(IDI_OUT_CARD);

	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	//m_GameClientView.//m_btOutPrompt.ShowWindow(SW_HIDE);
	
	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	//m_GameClientView.//m_btOutPrompt.EnableWindow(FALSE);

	//设置积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		m_GameClientView.m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lGameScore[i]);
		m_GameClientView.m_lAllTurnScore[SwitchViewChairID(i)]+=pGameEnd->lGameScore[i];
		m_GameClientView.m_lLastTurnScore[SwitchViewChairID(i)]=pGameEnd->lGameScore[i];
	}
	m_GameClientView.m_ScoreView.SetTax(pGameEnd->lGameTax);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置扑克
	BYTE bCardPos=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置扑克
		WORD wViewChairID=SwitchViewChairID(i);
		if (wViewChairID!=2)
		{
			if (wViewChairID>2) m_GameClientView.m_LeaveCardControl[wViewChairID-1].SetCardData(&pGameEnd->bCardData[bCardPos],pGameEnd->bCardCount[i]);
			else m_GameClientView.m_LeaveCardControl[wViewChairID].SetCardData(&pGameEnd->bCardData[bCardPos],pGameEnd->bCardCount[i]);
		}

		//界面设置
		bCardPos+=pGameEnd->bCardCount[i];
		if (pGameEnd->bCardCount[i]!=0)
		{
			m_GameClientView.SetPassFlag(wViewChairID,false);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		}
	}

	//显示扑克
	if (IsLookonMode()==true) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

	//播放声音
	WORD wMeChairID=GetMeChairID();
	LONG lMeScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		//SetGameTimer(GetMeChairID(),IDI_START_GAME,90);
	}
	//设置按钮
	m_GameClientView.m_btDisplayTen.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAbnegate.ShowWindow(SW_HIDE);


	return true;
}


//出牌判断
bool CGameClientDlg::VerdictOutCard()
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE bCardData[13];
	BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(bCardData,CountArray(bCardData));

	//出牌判断
	if (bShootCount>0L)
	{
		//排列扑克
		m_GameLogic.SortCardList(bCardData,bShootCount);

		//分析类型
		BYTE bCardType=m_GameLogic.GetCardType(bCardData,bShootCount);

		//类型判断
		if (bCardType==CT_ERROR) return false;

		////首出判断
		//if ((m_wBankerUser==GetMeChairID())&&(m_bCardCount[m_wBankerUser]==10))
		//{
		//	//方四判断
		//	for (BYTE i=0;i<bShootCount;i++)
		//	{
		//		if (bCardData[i]==0x24) break;
		//	}
		//	if (i==bShootCount) return false;
		//}

		//跟牌判断
		if (m_bTurnCardCount==0) return true;
		return m_GameLogic.CompareCard(bCardData,m_bTurnCardData,bShootCount,m_bTurnCardCount);
	}

	return false;
}
//自动出牌
bool CGameClientDlg::AutomatismOutCard()
{
	//先出牌者
	if (m_bTurnCardCount==0)
	{
		//控制界面
		KillGameTimer(IDI_OUT_CARD);
		m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		//m_GameClientView.//m_btOutPrompt.ShowWindow(SW_HIDE);
		//m_GameClientView.//m_btOutPrompt.EnableWindow(FALSE);

		//首出判断
		BYTE cbOutCard=m_bHandCardData[m_bHandCardCount-1];
		if ((m_wBankerUser==GetMeChairID())&&(m_bCardCount[m_wBankerUser]==10)) cbOutCard=0x24;

		//发送数据
		CMD_C_OutCard OutCard;
		OutCard.bCardCount=1;
		OutCard.bCardData[0]=cbOutCard;
		SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

		//预先处理
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
		m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.bCardData,OutCard.bCardCount);

		//预先删除
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=OutCard.bCardCount;
		m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
		m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	}
	else OnPassCard(0,0);

	return true;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	if ((nIDEvent==IDI_MOST_CARD)&&(m_wMostUser!=INVALID_CHAIR))
	{
		//变量定义
		WORD wCurrentUser=m_wMostUser;
		m_wMostUser=INVALID_CHAIR;

		//删除定时器
		KillTimer(IDI_MOST_CARD);

		//设置界面
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
		for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);

		//玩家设置
		if ((IsLookonMode()==false)&&(wCurrentUser==GetMeChairID()))
		{
			ActiveGameFrame();
			m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btPassCard.EnableWindow(FALSE);	
			m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);	
			//m_GameClientView.//m_btOutPrompt.ShowWindow(SW_SHOW);
			//m_GameClientView.//m_btOutPrompt.EnableWindow(FALSE);
		}

		//设置时间
		WORD wTimeCount=30;
		if (m_GameClientView.m_btPassCard.IsWindowEnabled()) wTimeCount=30;
		SetGameTimer(wCurrentUser,IDI_OUT_CARD,wTimeCount);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_wTimeOutCount=0;
	m_bHandCardCount=0;
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	m_wMostUser=INVALID_CHAIR;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	ZeroMemory(m_bScoreCardCount,sizeof(m_bScoreCardCount));
	ZeroMemory(m_bScoreCard[GAME_PLAYER],sizeof(m_bScoreCard[GAME_PLAYER]));

	//设置界面
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	m_GameClientView.ClearOneTime();

	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(FALSE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//设置扑克
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl.SetPositively(false);
	for(WORD i=0;i<GAME_PLAYER-1;i++)m_GameClientView.m_LeaveCardControl[i].SetCardData(NULL,0);
	for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//出牌消息
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if ((m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE)||(m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE)) return 0;

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	//m_GameClientView.//m_btOutPrompt.ShowWindow(SW_HIDE);
	//m_GameClientView.//m_btOutPrompt.EnableWindow(FALSE);
	
	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	m_GameLogic.SortCardList(OutCard.bCardData,OutCard.bCardCount);
	SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

	//预先显示
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.bCardData,OutCard.bCardCount);

	//预先删除
	BYTE bSourceCount=m_bHandCardCount;
	m_bHandCardCount-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//放弃出牌
LRESULT CGameClientDlg::OnPassCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btPassCard.IsWindowEnabled()==FALSE) return 0;

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	//m_GameClientView.//m_btOutPrompt.ShowWindow(SW_HIDE);
	//m_GameClientView.//m_btOutPrompt.EnableWindow(FALSE);

	//发送数据
	SendData(SUB_C_PASS_CARD);

	//预先显示
	m_GameClientView.m_UserCardControl[2].SetCardData(NULL,0);
	m_GameClientView.SetPassFlag(2,true);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//出牌提示
LRESULT CGameClientDlg::OnOutPrompt(WPARAM wParam, LPARAM lParam)
{
	//AutoOutCard(0);
	return 0;
}

//右键扑克
LRESULT CGameClientDlg::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	//设置控件
	bool bOutCard=VerdictOutCard();
	m_GameClientView.m_btOutCard.EnableWindow(bOutCard?TRUE:FALSE);

	return 0;
}

//左键扑克
LRESULT CGameClientDlg::OnRightHitCard(WPARAM wParam, LPARAM lParam)
{
	//用户出牌
	OnOutCard(0,0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//void CGameClientDlg::AutoOutCard(BYTE m_WhichOnsKindCard)
//{
//	int i=0;
//	BYTE m_bWhichKindSel=0;
//	BYTE							m_bTempSCardCount=0;				//扑克数目
//	BYTE							m_bTempSCardData[13];				//手上扑克
//	BYTE							m_bTempDCardCount=0;				//扑克数目
//	BYTE							m_bTempDCardData[13];				//手上扑克
//	BYTE							m_bTempTCardCount=0;				//扑克数目
//	BYTE							m_bTempTCardData[13];				//手上扑克
//	BYTE							m_bTempFCardCount=0;				//扑克数目
//	BYTE							m_bTempFCardData[13];				//手上扑克
//	BYTE							m_bTempGetCardCount=0;				//扑克数目
//	BYTE							m_bTempGetCardData[13];				//手上扑克
//	BYTE							m_bTempGetCardIndex[13];			//手上扑克
//	BYTE m_TempCard=0;
//	//如果没有人出牌，不提示
//	if(m_bTurnCardCount==0)
//		return;
//	m_GameClientView.m_HandCardControl.ShootAllCard(false);
//	//获取单牌列表
//	for(i=0;i<m_bHandCardCount;i++)
//	{	
//		BYTE m_GetCard=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
//		if(m_TempCard!=m_GetCard)
//		{
//			m_bTempSCardData[m_bTempSCardCount++]=m_bHandCardData[i];
//			m_TempCard=m_GetCard;
//		}
//	}
//	//获取对牌列表
//	m_TempCard=0;
//	for(i=0;i<m_bHandCardCount-1;i++)
//	{	
//		BYTE m_GetCard1=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
//		BYTE m_GetCard2=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+1]);
//		if(m_TempCard!=m_GetCard1&&m_GetCard1==m_GetCard2&&m_GetCard1<16)
//		{
//			m_bTempDCardData[m_bTempDCardCount++]=m_bHandCardData[i];
//			m_bTempDCardData[m_bTempDCardCount++]=m_bHandCardData[i+1];
//			m_TempCard=m_GetCard1;
//		}
//	}
//	//获取三张牌列表
//	m_TempCard=0;
//	for(i=0;i<m_bHandCardCount-2;i++)
//	{	
//		BYTE m_GetCard1=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
//		BYTE m_GetCard2=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+1]);
//		BYTE m_GetCard3=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+2]);
//		if(m_TempCard!=m_GetCard1&&m_GetCard1==m_GetCard2&&m_GetCard1==m_GetCard3)
//		{
//			m_bTempTCardData[m_bTempTCardCount++]=m_bHandCardData[i];
//			m_bTempTCardData[m_bTempTCardCount++]=m_bHandCardData[i+1];
//			m_bTempTCardData[m_bTempTCardCount++]=m_bHandCardData[i+2];
//			m_TempCard=m_GetCard1;
//		}
//	}
//	//获取四张牌列表
//	m_TempCard=0;
//	for(i=0;i<m_bHandCardCount-3;i++)
//	{	
//		BYTE m_GetCard1=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
//		BYTE m_GetCard2=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+1]);
//		BYTE m_GetCard3=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+2]);
//		BYTE m_GetCard4=m_GameLogic.GetCardLogicValue(m_bHandCardData[i+3]);
//		if(m_TempCard!=m_GetCard1&&m_GetCard1==m_GetCard2&&m_GetCard1==m_GetCard3&&m_GetCard1==m_GetCard4)
//		{
//			m_bTempFCardData[m_bTempFCardCount++]=m_bHandCardData[i];
//			m_bTempFCardData[m_bTempFCardCount++]=m_bHandCardData[i+1];
//			m_bTempFCardData[m_bTempFCardCount++]=m_bHandCardData[i+2];
//			m_bTempFCardData[m_bTempFCardCount++]=m_bHandCardData[i+3];
//			m_TempCard=m_GetCard1;
//		}
//	}
//	//根据所出牌类型判断
//	i=0;
//	switch(m_bTurnOutType)
//	{
//	case CT_SINGLE:
//		if(m_WhichOnsKindCard==1)   //判断是不是具有唯一性
//		{
//			for(i=m_bTempSCardCount;i>0;i--)
//			{
//				if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempSCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
//				{
//					if((m_bWhichKindSel++)>1)
//						i=0;
//				}
//			}
//		}
//		for(i=m_bTempSCardCount;i>0;i--)
//		{
//			if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempSCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
//			{
//				//判断是不是最合理的
//				bool m_bIsHaveCard=false;
//				for(int j=0;j<m_bTempDCardCount;j++)
//				{
//					for(int n=0;n<m_bTurnCardCount;n++)
//					{
//						if(m_GameLogic.GetCardLogicValue(m_bTempSCardData[i-m_bTurnCardCount+n])==m_GameLogic.GetCardLogicValue(m_bTempDCardData[j]))
//							m_bIsHaveCard=true;
//					}
//				}
//				//把最合理的情况保存起来
//				if(m_bTempGetCardCount==0||!m_bIsHaveCard)
//				{
//					CopyMemory(m_bTempGetCardData,&m_bTempSCardData[i-m_bTurnCardCount],m_bTurnCardCount);
//					m_bTempGetCardCount=m_bTurnCardCount;
//				}
//				if(!m_bIsHaveCard)
//					break;
//			}
//		}
//		break;
//	case CT_DOUBLE:
//		if(m_WhichOnsKindCard==1)     //判断是不是具有唯一性
//		{
//			for(i=m_bTempDCardCount;i>0;i--)
//			{
//				if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempDCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
//				{
//					if((m_bWhichKindSel++)>1)
//						i=0;
//				}
//			}
//		}
//		for(i=m_bTempDCardCount;i>0;i--)
//		{
//			if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempDCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
//			{
//				//判断是不是最合理的
//				bool m_bIsHaveCard=false;
//				for(int j=0;j<m_bTempTCardCount;j++)
//				{
//					for(int n=0;n<m_bTurnCardCount;n++)
//					{
//						if(m_GameLogic.GetCardLogicValue(m_bTempDCardData[i-m_bTurnCardCount+n])==m_GameLogic.GetCardLogicValue(m_bTempTCardData[j]))
//							m_bIsHaveCard=true;
//					}
//				}
//				if(m_bTempGetCardCount==0||!m_bIsHaveCard)
//				{
//					CopyMemory(m_bTempGetCardData,&m_bTempDCardData[i-m_bTurnCardCount],m_bTurnCardCount);
//					m_bTempGetCardCount=m_bTurnCardCount;
//				}
//				if(!m_bIsHaveCard)
//					break;
//			}
//		}
//		break;
//	case CT_THREE_TIAO:
//		if(m_WhichOnsKindCard==1)           //判断是不是具有唯一性
//		{
//			for(i=m_bTempTCardCount;i>0;i--)
//			{
//				if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempTCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
//				{
//					if((m_bWhichKindSel++)>1)
//						i=0;
//				}
//			}
//		}
//		for(i=m_bTempTCardCount;i>0;i--)
//		{
//			if(i-m_bTurnCardCount>=0&&m_GameLogic.CompareCard(&m_bTempTCardData[i-m_bTurnCardCount],m_bTurnCardData,m_bTurnCardCount,m_bTurnCardCount))
//			{
//				//判断是不是最合理的
//				bool m_bIsHaveCard=false;
//				for(int j=0;j<m_bTempFCardCount;j++)
//				{
//					for(int n=0;n<m_bTurnCardCount;n++)
//					{
//						if(m_GameLogic.GetCardLogicValue(m_bTempTCardData[i-m_bTurnCardCount+n])==m_GameLogic.GetCardLogicValue(m_bTempFCardData[j]))
//							m_bIsHaveCard=true;
//					}
//				}
//				if(m_bTempGetCardCount==0||!m_bIsHaveCard)
//				{
//					CopyMemory(m_bTempGetCardData,&m_bTempTCardData[i-m_bTurnCardCount],m_bTurnCardCount);
//					m_bTempGetCardCount=m_bTurnCardCount;
//				}
//				if(!m_bIsHaveCard&&m_bTempGetCardCount!=0)
//					break;
//			}
//		}
//		break;
//	case CT_DRAGON:
//	case CT_HU_LU:
//		{
//			//分析扑克
//			tagAnalyseResult AnalyseResult;
//			m_GameLogic.AnalysebCardData(m_bTurnCardData,m_bTurnCardCount,AnalyseResult);      
//			if(m_WhichOnsKindCard==1)               //判断是不是具有唯一性
//			{
//				for(i=m_bTempTCardCount;i>0;i--)
//				{
//					if(i-AnalyseResult.bThreeCount*3>=0&&m_GameLogic.CompareCard(&m_bTempTCardData[i-AnalyseResult.bThreeCount*3],m_bTurnCardData,AnalyseResult.bThreeCount*3,AnalyseResult.bThreeCount*3))
//					{
//						if((m_bWhichKindSel++)>1)
//							i=0;
//					}
//				}
//			}
//			for(i=m_bTempTCardCount;i>0;i--)
//			{
//				if(i-AnalyseResult.bThreeCount*3>=0&&m_GameLogic.CompareCard(&m_bTempTCardData[i-AnalyseResult.bThreeCount*3],AnalyseResult.m_bTCardData,AnalyseResult.bThreeCount*3,AnalyseResult.bThreeCount*3))
//				{
//					//判断是不是最合理的
//					bool m_bIsHaveCard=false;
//					for(int j=0;j<m_bTempFCardCount;j++)
//					{
//						for(int n=0;n<AnalyseResult.bThreeCount*3;n++)
//						{
//							if(m_GameLogic.GetCardLogicValue(m_bTempTCardData[i-AnalyseResult.bThreeCount*3+n])==m_GameLogic.GetCardLogicValue(m_bTempFCardData[j]))
//								m_bIsHaveCard=true;
//						}
//					}
//					if(m_bTempGetCardCount==0||!m_bIsHaveCard)
//					{
//						CopyMemory(m_bTempGetCardData,&m_bTempTCardData[i-AnalyseResult.bThreeCount*3],AnalyseResult.bThreeCount*3);
//						m_bTempGetCardCount=AnalyseResult.bThreeCount*3;
//					}
//					if(!m_bIsHaveCard&&m_bTempGetCardCount!=0)
//						i=0;
//				}
//			}
//			if(m_bTempGetCardCount>0)
//			{
//				bool m_bIsHaveSame;
//				for(int m=0;m<AnalyseResult.bDoubleCount;m++)
//				{
//					for(int j=0;j<m_bTempDCardCount/2;j++)
//					{
//						//判断是不是最合理的
//						m_bIsHaveSame=false;
//						for(int n=0;n<m_bTempGetCardCount;n++)
//						{
//							if(m_GameLogic.GetCardLogicValue(m_bTempDCardData[m_bTempDCardCount-j*2-1])==m_GameLogic.GetCardLogicValue(m_bTempGetCardData[n]))
//							{
//								m_bIsHaveSame=true;
//							}
//						}
//						if(!m_bIsHaveSame)
//						{
//							bool m_bIsHaveCard=false;
//							for(int s=0;s<m_bTempTCardCount;s++)
//							{
//								for(int n=0;n<m_bTempGetCardCount;n++)
//								{
//									if(m_GameLogic.GetCardLogicValue(m_bTempDCardData[m_bTempDCardCount-j*2-1])==m_GameLogic.GetCardLogicValue(m_bTempTCardData[s]))
//										m_bIsHaveCard=true;
//								}
//							}
//							if(m_bTempGetCardCount==AnalyseResult.bThreeCount*3||!m_bIsHaveCard)
//							{
//								m_bTempGetCardData[AnalyseResult.bThreeCount*3+m*2]=m_bTempDCardData[m_bTempDCardCount-j*2-1];
//								m_bTempGetCardData[AnalyseResult.bThreeCount*3+m*2+1]=m_bTempDCardData[m_bTempDCardCount-j*2-2];
//								m_bTempGetCardCount=AnalyseResult.bThreeCount*3+(m+1)*2;
//							}
//							if(!m_bIsHaveCard)
//							{
//								n=m_bTempGetCardCount;
//								j=m_bTempDCardCount/2;
//							}
//						}
//					}
//				}
//				for(int m=0;m<AnalyseResult.bSignedCount;m++)
//				{
//					for(int j=0;j<m_bTempSCardCount;j++)
//					{
//						//判断是不是最合理的
//						m_bIsHaveSame=false;
//						for(int n=0;n<m_bTempGetCardCount;n++)
//						{
//							if(m_GameLogic.GetCardLogicValue(m_bTempSCardData[m_bTempSCardCount-j-1])==m_GameLogic.GetCardLogicValue(m_bTempGetCardData[n]))
//							{
//								m_bIsHaveSame=true;
//							}
//						}
//						if(!m_bIsHaveSame)
//						{
//							bool m_bIsHaveCard=false;
//							for(int s=0;s<m_bTempDCardCount;s++)
//							{
//								for(n=0;n<m_bTempGetCardCount;n++)
//								{
//									if(m_GameLogic.GetCardLogicValue(m_bTempSCardData[m_bTempSCardCount-j-1])==m_GameLogic.GetCardLogicValue(m_bTempDCardData[s]))
//										m_bIsHaveCard=true;
//								}
//							}
//							if(m_bTempGetCardCount==AnalyseResult.bThreeCount*3||!m_bIsHaveCard)
//							{
//								m_bTempGetCardData[AnalyseResult.bThreeCount*3+m]=m_bTempSCardData[m_bTempSCardCount-j-1];
//								m_bTempGetCardCount=AnalyseResult.bThreeCount*3+m+1;
//							}
//							if(!m_bIsHaveCard)
//							{
//								n=m_bTempGetCardCount;
//								j=m_bTempSCardCount;
//							}
//						}
//					}
//				}
//			}
//		}
//		break;
//	case CT_TIE_ZHI:
//	case CT_TONG_HUA:
//		{
//			//分析扑克
//			tagAnalyseResult AnalyseResult;
//			m_GameLogic.AnalysebCardData(m_bTurnCardData,m_bTurnCardCount,AnalyseResult);
//			if(m_WhichOnsKindCard==1)       //判断是不是具有唯一性
//			{
//				for(i=m_bTempFCardCount;i>0;i--)
//				{
//					if(i-AnalyseResult.bFourCount*4>=0&&m_GameLogic.CompareCard(&m_bTempFCardData[i-AnalyseResult.bFourCount*4],m_bTurnCardData,AnalyseResult.bFourCount*4,AnalyseResult.bFourCount*4))
//					{
//						if((m_bWhichKindSel++)>1)
//							i=0;
//					}
//				}
//			}
//			for(i=m_bTempFCardCount;i>0;i--)
//			{
//				if(i-AnalyseResult.bFourCount*4>=0&&m_GameLogic.CompareCard(&m_bTempFCardData[i-AnalyseResult.bFourCount*4],m_bTurnCardData,AnalyseResult.bFourCount*4,AnalyseResult.bFourCount*4))
//				{
//					CopyMemory(m_bTempGetCardData,&m_bTempFCardData[i-AnalyseResult.bFourCount*4],AnalyseResult.bFourCount*4);
//					m_bTempGetCardCount=AnalyseResult.bFourCount*4;
//					i=0;
//				}
//			}
//			if(m_bTempGetCardCount>0)
//			{
//				bool m_bIsHaveSame;
//				for(int m=0;m<AnalyseResult.bDoubleCount;m++)
//				{
//					for(int j=0;j<m_bTempDCardCount/2;j++)
//					{
//						//判断是不是最合理的
//						m_bIsHaveSame=false;
//						for(int n=0;n<m_bTempGetCardCount;n++)
//						{
//							if(m_GameLogic.GetCardLogicValue(m_bTempDCardData[m_bTempDCardCount-j*2-1])==m_GameLogic.GetCardLogicValue(m_bTempGetCardData[n]))
//							{
//								m_bIsHaveSame=true;
//							}
//						}
//						if(!m_bIsHaveSame)
//						{
//							bool m_bIsHaveCard=false;
//							for(int s=0;s<m_bTempTCardCount;s++)
//							{
//								for(int n=0;n<m_bTempGetCardCount;n++)
//								{
//									if(m_GameLogic.GetCardLogicValue(m_bTempDCardData[m_bTempDCardCount-j*2-1])==m_GameLogic.GetCardLogicValue(m_bTempTCardData[s]))
//										m_bIsHaveCard=true;
//								}
//							}
//							if(m_bTempGetCardCount==AnalyseResult.bFourCount*4||!m_bIsHaveCard)
//							{
//								m_bTempGetCardData[AnalyseResult.bFourCount*4+m*2]=m_bTempDCardData[m_bTempDCardCount-j*2-1];
//								m_bTempGetCardData[AnalyseResult.bFourCount*4+m*2+1]=m_bTempDCardData[m_bTempDCardCount-j*2-2];
//								m_bTempGetCardCount=AnalyseResult.bFourCount*4+(m+1)*2;
//							}
//							if(!m_bIsHaveCard)
//							{
//								n=m_bTempGetCardCount;
//								j=m_bTempDCardCount/2;
//							}
//						}
//					}
//				}
//				for(int m=0;m<AnalyseResult.bSignedCount;m++)
//				{
//					for(int j=0;j<m_bTempSCardCount;j++)
//					{
//						//判断是不是最合理的
//						m_bIsHaveSame=false;
//						for(int n=0;n<m_bTempGetCardCount;n++)
//						{
//							if(m_GameLogic.GetCardLogicValue(m_bTempSCardData[m_bTempSCardCount-j-1])==m_GameLogic.GetCardLogicValue(m_bTempGetCardData[n]))
//							{
//								m_bIsHaveSame=true;
//							}
//						}
//						if(!m_bIsHaveSame)
//						{
//							bool m_bIsHaveCard=false;
//							for(int s=0;s<m_bTempDCardCount;s++)
//							{
//								for(int n=0;n<m_bTempGetCardCount;n++)
//								{
//									if(m_GameLogic.GetCardLogicValue(m_bTempSCardData[m_bTempSCardCount-j-1])==m_GameLogic.GetCardLogicValue(m_bTempDCardData[j]))
//										m_bIsHaveCard=true;
//								}
//							}
//							if(m_bTempGetCardCount==AnalyseResult.bFourCount*4||!m_bIsHaveCard)
//							{
//								m_bTempGetCardData[AnalyseResult.bFourCount*4+m]=m_bTempSCardData[m_bTempSCardCount-j-1];
//								m_bTempGetCardCount=AnalyseResult.bFourCount*4+m+1;
//							}
//							if(!m_bIsHaveCard)
//							{
//								n=m_bTempGetCardCount;
//								j=m_bTempSCardCount;
//							}
//						}
//					}
//				}
//			}
//		}
//		break;
//	}
//	if(m_bTempGetCardCount==0)
//	{
//		m_bWhichKindSel=0;
//		//判断炸弹的可能性
//		if(m_bTempFCardCount>3)
//		{
//			for(i=m_bTempFCardCount-4;i>=0;i--)
//			{
//				if(m_GameLogic.CompareCard(&m_bTempFCardData[i],m_bTurnCardData,4,m_bTurnCardCount))
//				{
//					if((m_bWhichKindSel++)==0)
//					{
//						CopyMemory(m_bTempGetCardData,&m_bTempFCardData[i],4);
//						m_bTempGetCardCount=4;
//					}
//				}
//			}
//		}
//		if(m_bTempGetCardCount==0)
//		{
//			if(m_bHandCardCount>1)
//			{
//				if(m_GameLogic.GetCardLogicValue(m_bHandCardData[0])>15&&m_GameLogic.GetCardLogicValue(m_bHandCardData[1])>15)
//				{
//					CopyMemory(m_bTempGetCardData,m_bHandCardData,2);
//					m_bTempGetCardCount=2;
//					if(m_WhichOnsKindCard==1)
//						m_bWhichKindSel=1;
//				}
//			}
//		}
//	}
//	BYTE m_GetIndex=0;
//	if(m_bTempGetCardCount==0)
//	{
//		if(m_WhichOnsKindCard!=1)
//			OnPassCard(0,0);
//	}
//	else
//	{
//		for(int j=0;j<m_bTempGetCardCount;j++)
//		{
//			for(i=0;i<m_bHandCardCount;i++)
//			{
//				if(m_bHandCardData[i]==m_bTempGetCardData[j])
//				{
//					m_bTempGetCardIndex[m_GetIndex++]=i;
//				}
//			}
//		}
//
//	}
//	if(m_GameLogic.CompareCard(m_bTempGetCardData,m_bTurnCardData,m_bTempGetCardCount,m_bTurnCardCount))
//	{
//		if(m_WhichOnsKindCard==1&&m_bWhichKindSel==1||m_WhichOnsKindCard!=1)
//		{
//			m_GameClientView.m_HandCardControl.SetShootCard(m_bTempGetCardIndex,m_GetIndex);
//			m_GameClientView.m_btOutCard.EnableWindow(TRUE);
//		}
//	}
//	else
//	{
//		if(m_WhichOnsKindCard!=1)
//			OnPassCard(0,0);
//	}
//}
