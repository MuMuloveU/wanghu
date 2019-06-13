#include "Stdafx.h"
#include "BJ.h"
#include "BJGameDlg.h"
#include "BJGameView.h"
#include "..\消息定义\CMD_BlackJack.h"


//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_STRAT_GAME					200								//继续游戏定时器
#define IDI_GIVE_UP						201								//放弃定时器

#define IDI_ADD_GOLD					202								//加注定时器
#define IDI_GET_CARD					203								//要牌定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBJGameDlg, CGameFrameDlg)
	ON_MESSAGE(IDM_GAME_START,OnStart)
	ON_MESSAGE(IDM_ADD_GOLD,OnAddGold)
	ON_MESSAGE(IDM_GIVE_UP,OnGiveUp)
	ON_MESSAGE(IDM_GET_CARD,OnGetCard)
	ON_MESSAGE(IDM_STOP_CARD,OnStopCard)
	ON_MESSAGE(IDM_DOUBLE_CARD,OnDoubleCard)	
	ON_MESSAGE(IDM_CUT_CARD,OnCutCard)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////

//构造函数
CBJGameDlg::CBJGameDlg() : CGameFrameDlg(&m_GameView)
{	
	//辅助变量
	m_lGoldShow    = 0L;	
	m_lTurnMaxGold = 0L;
	m_bMeAddGold   = false;
	m_wCardSum     = 0;
	m_bCutCard     = false;
	memset(m_szName, 0, sizeof(m_szName));
	return;
}

//析构函数
CBJGameDlg::~CBJGameDlg()
{
}

//初始函数
bool CBJGameDlg::InitGameFrame()
{
	//设置标题
	SetWindowText(TEXT("21点-黑杰克游戏"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

#ifdef	DEBUG_GAME
	m_GameView.m_btAddGold.ShowWindow(SW_SHOW);
	m_GameView.m_GoldControl.ShowWindow(SW_SHOW);
	for(int i=0; i<4;i++)
	{
		m_GameView.SetUserGoldInfo(i,false,1000);
	}
#endif
	return true;
}

//重置框架
void CBJGameDlg::ResetGameFrame()
{	
	//辅助变量
	m_lGoldShow    = 0L;	
	m_lTurnMaxGold = 0L;
	m_bMeAddGold   = false;
	m_wCardSum     = 0;
	memset(m_szName, 0, sizeof(m_szName));
	memset(m_GameView.m_cbPlayStatus,0,sizeof(m_GameView.m_cbPlayStatus));
	return;
}

//游戏设置
void CBJGameDlg::OnGameOptionSet()
{
	return;
}

//时间消息
bool CBJGameDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_ADD_GOLD:		    //加注定时器
		{
			if (nElapse == 0)
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("TIME_OUT"));
				
				OnAddGold(0,0);			
				return true;
			}
			if (nElapse <= 10) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_GET_CARD:			//要牌定时器
		{	
			if (nElapse == 0)
			{			
				PlayGameSound(AfxGetInstanceHandle(),TEXT("TIME_OUT"));
				
				OnStopCard(0,0);
				return true;
			}
			if (nElapse <= 10) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_STRAT_GAME:       //继续游戏定时器
		{
			if (nElapse==0)
			{
				OnStart(0,0);
				return false;
			}
			if (nElapse<=10) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			return true;
		}
	}
	return false;
}

//旁观状态
void CBJGameDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{

}

//游戏场景
bool CBJGameDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_FREE:		//空闲状态
		{		
			//效验数据
			if (wDataSize != sizeof(CMD_S_StatusFree)) return false;

			CMD_S_StatusFree * pStatusFree = (CMD_S_StatusFree *)pBuffer;

			//辅助变量
			m_bMeAddGold = false;

			//设置界面
			m_lGoldShow      = 0L;
			m_lTurnBasicGold = 0L;
			m_GameView.SetGoldTitleInfo(0, pStatusFree->dwBasicGold);

			//设置控件
			if (IsLookonMode() == false)
			{
				m_GameView.m_btBegin.ShowWindow(SW_SHOW);
				m_GameView.m_btBegin.SetFocus();
			}
			return true;
		}
	case GS_ADDGOLDING:		//加注状态
		{		
			//效验数据
			if (wDataSize != sizeof(CMD_S_StatusAddGold)) return false;

			CMD_S_StatusAddGold * pStatusAddGold = (CMD_S_StatusAddGold *)pBuffer;	

			for(WORD i = 0; i < GAME_PLAYER; i++)
			{
				WORD wViewChairID = SwitchViewChairID(i);			

				//加注金币
				m_GameView.SetUserGoldInfo(wViewChairID,true,pStatusAddGold->lTableGold[2 * i]);
				m_GameView.SetUserGoldInfo(wViewChairID,false,pStatusAddGold->lTableGold[2 * i + 1]);
				
				const tagUserData * pUserData = GetUserData(i);
				if (pUserData != NULL)
				{
					lstrcpyn(m_szName[i],pUserData->szName,CountArray(m_szName[i]));
					if((IsLookonMode() == false) && (GetMeChairID() != pStatusAddGold->wBankerStation))
					{
						//加注界面
						if(!pStatusAddGold->bAddGoldEnd[GetMeChairID()])
						{
							m_GameView.m_GoldControl.SetMaxGold(pStatusAddGold->lMaxGold);
							m_GameView.m_GoldControl.ShowWindow(SW_SHOW);	
							m_GameView.m_btAddGold.ShowWindow(SW_SHOW);
							m_GameView.m_btGiveUp.ShowWindow(SW_HIDE);	
						}
						
						//设置定时器
						if(GetMeChairID() == pStatusAddGold->wCurrentUser)
						{
							SetGameTimer(GetMeChairID(), IDI_ADD_GOLD, 30);
						}						
					}
					//标识状态
					if(!pStatusAddGold->bAddGoldEnd[i] && i != pStatusAddGold->wBankerStation )
					{
						m_GameView.m_cbPlayStatus[wViewChairID] = 1;
					}		
					m_GameView.m_wBankerStation = SwitchViewChairID(pStatusAddGold->wBankerStation);
				}			
			}			
			return true;
		}

	case GS_PLAYING:	//游戏状态
		{
			//效验数据
			if (wDataSize != sizeof(CMD_S_StatusPlay)) return false;

			CMD_S_StatusPlay * pStatusPlay = (CMD_S_StatusPlay *)pBuffer;

			for(WORD i = 0; i < GAME_PLAYER; i++)
			{
				//变量定义
				WORD wViewChairID = SwitchViewChairID(i);
				const tagUserData * pUserData = GetUserData(i);

				if (pUserData != NULL)
				{
					lstrcpyn(m_szName[i],pUserData->szName,CountArray(m_szName[i]));			
					m_GameView.m_CardControl[wViewChairID].SetCardData(pStatusPlay->cbTableCardArray[i],pStatusPlay->cbTableCardCount[i]);

					if(i == pStatusPlay->wBankerStation) continue;
					
					//标识状态			
					if(pStatusPlay->cbPlayStatus[i] == STOP_CARD)
					{
						m_GameView.m_cbPlayStatus[wViewChairID] = 3;
					}	
					else if(pStatusPlay->cbPlayStatus[i] == BRUST_CARD)
					{
						m_GameView.m_cbPlayStatus[wViewChairID] = 4;
					}
					else					
					{
						m_GameView.m_cbPlayStatus[wViewChairID] = 2;
					}	

					//庄家标识
					m_GameView.m_wBankerStation = SwitchViewChairID(pStatusPlay->wBankerStation);
				}	
				else
				{
					m_GameView.m_CardControl[wViewChairID].SetCardData(NULL,0);
				}

				//加注金币
				m_GameView.SetUserGoldInfo(wViewChairID, true,  pStatusPlay->lTableGold[2 * i]);
				m_GameView.SetUserGoldInfo(wViewChairID, false, pStatusPlay->lTableGold[2 * i + 1]);							
			}			

			//设置界面
			if((IsLookonMode() == false) && (GetMeChairID() != pStatusPlay->wBankerStation ))
			{
				if(pStatusPlay->cbPlayStatus[GetMeChairID()] == STOP_CARD || 
				   pStatusPlay->cbPlayStatus[GetMeChairID()] == BRUST_CARD)
				{
					m_GameView.m_btGetCard.ShowWindow(SW_HIDE);
					m_GameView.m_btDouble.ShowWindow(SW_HIDE);
					m_GameView.m_btStop.ShowWindow(SW_HIDE);
					KillGameTimer(IDI_GET_CARD);
				}
				else
				{
					m_GameView.m_btGetCard.ShowWindow(SW_SHOW);
					m_GameView.m_btDouble.ShowWindow(SW_SHOW);
					m_GameView.m_btStop.ShowWindow(SW_SHOW);	
					SetGameTimer(GetMeChairID(), IDI_GET_CARD, 30);
				}			
		
			}	

			//设置界面
			if((IsLookonMode() == false) && (GetMeChairID() == pStatusPlay->wBankerStation))
			{
				if(pStatusPlay->bBankerGetCard)
				{					
					m_GameView.m_btGetCard.ShowWindow(SW_SHOW);		
					m_GameView.m_btStop.ShowWindow(SW_SHOW);

					//设置定时器
					SetGameTimer(GetMeChairID(), IDI_GET_CARD, 20);
				}
				else
				{
					m_GameView.m_btGetCard.ShowWindow(SW_HIDE);		
					m_GameView.m_btStop.ShowWindow(SW_HIDE);

					//设置定时器
					KillGameTimer(IDI_GET_CARD);
				}
			}

			//设置控件
			m_GameView.m_CardControl[SwitchViewChairID(GetMeChairID())].ShowFirstCard(true);
			
			if (IsLookonMode() == false) 
			{
				ActiveGameFrame();
				m_GameView.m_CardControl[2].AllowPositively(true);
			}	

			m_GameView.UpdateGameView(NULL);

			return true;
		}		
	}
	return false;
}

//网络消息
bool CBJGameDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch(wSubCmdID)
	{
	case SUB_S_GAME_START:           //游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_ADD_GOLD:			//加注结果
		{
			return OnSubAddGold(pBuffer,wDataSize);
		}
	case SUB_S_INIT_CARD:           //初始发牌
		{
			return OnSubInitCard(pBuffer,wDataSize);
		}
	case SUB_S_CUT_CARD:			//分牌结果
		{
			return OnSubCutCard(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:			//加注结果
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_STOP_CARD:			//用户停牌
		{
			return OnSubStopCard(pBuffer,wDataSize);
		}
	case SUB_S_BRUST_CARD:			//用户爆牌
		{
			return OnSubBrustCard(pBuffer,wDataSize);
		}
	case SUB_S_BANKER_SEND_CARD:    //庄家发牌
		{			
			return OnSubBankerSendCard(pBuffer,wDataSize);
		}	
	case SUB_S_USER_LEFT:		   //用户强退
		{						
			return OnSubUserLeft(pBuffer,wDataSize);
		}	
	case SUB_S_GAME_END:		   //游戏结束
		{			
			return OnSubGameEnd(pBuffer,wDataSize);
		}	
	case SUB_S_CUT_CARD_MARK:     //切换分牌
		{
			return OnCutCardMark(pBuffer,wDataSize);
		}
	}
	return false;
}


//游戏开始
bool CBJGameDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{	
	//效验数据
	if (wDataSize != sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart = (CMD_S_GameStart *)pBuffer;

	//游戏变量
	m_lTurnMaxGold   = pGameStart->lMaxGold;
	m_lTurnBasicGold = pGameStart->lTurnBasicGold;

	memset(m_szName, 0, sizeof(m_szName));

	//变量定义
	WORD wMeChairID  = GetMeChairID();
	bool bLookonMode = IsLookonMode();
	LONG lBaseGold   = m_lTurnBasicGold;

	//设置状态
	SetGameStatus(GS_ADDGOLDING);
	m_GameView.SetGoldTitleInfo(pGameStart->lMaxGold,lBaseGold);

	//设置界面
	m_GameView.m_wBankerStation = SwitchViewChairID(pGameStart->wBankerStation);	
	
	//设置控制
	if (bLookonMode == false && wMeChairID != pGameStart->wBankerStation)
	{
		//设置界面
		m_GameView.m_btGiveUp.ShowWindow(SW_HIDE);
		m_GameView.m_btAddGold.ShowWindow(SW_SHOW);
		
		//m_GameView.SetUserGoldInfo(2, false, m_lTurnBasicGold);
		m_GameView.SetUserGoldInfo(SwitchViewChairID(wMeChairID),false,m_lTurnBasicGold);
		m_GameView.m_GoldControl.SetMaxGold(pGameStart->lMaxGold - m_lTurnBasicGold);
		m_GameView.m_GoldControl.ShowWindow(SW_SHOW);

		//设置定时器	
		SetGameTimer(GetMeChairID(), IDI_ADD_GOLD, 30);		
	}

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//变量定义
		WORD wViewChairID = SwitchViewChairID(i);
		const tagUserData * pUserData = GetUserData(i);

		//设置界面
		if (pUserData != NULL)
		{

			//m_GameView.SetUserGoldInfo(wViewChairID,true,lBaseGold);
			lstrcpyn(m_szName[i],pUserData->szName,CountArray(m_szName[i]));			
			if(wViewChairID != m_GameView.m_wBankerStation)
				m_GameView.m_cbPlayStatus[wViewChairID] = 1;
		}
		else
		{	
			m_GameView.SetUserGoldInfo(wViewChairID,true,0L);
			m_GameView.SetUserGoldInfo(wViewChairID,false,0L);	
		}

	}
	if (bLookonMode == false) 
	{
		ActiveGameFrame();
		m_GameView.m_CardControl[2].AllowPositively(true);
	}	
	else
	{	
		//设置界面			
		m_GameView.m_ScoreView.ShowWindow(SW_HIDE);	
		for (int i=0;i<CountArray(m_GameView.m_CardControl);i++) m_GameView.m_CardControl[i].SetCardData(NULL,0);
		memset(m_GameView.m_cbTopCardSum,0,sizeof(m_GameView.m_cbTopCardSum));
		memset(m_GameView.m_cbBottomCardSum,0,sizeof(m_GameView.m_cbBottomCardSum));
	}
	m_GameView.UpdateGameView(NULL);
	
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_BEGIN"));

	return true;
}

//初始发牌
bool CBJGameDlg::OnSubInitCard(const void * pBuffer, WORD wDataSize)
{
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

	//效验数据
	if (wDataSize != sizeof(CMD_S_InitCard)) return false;
	CMD_S_InitCard * pInitCard = (CMD_S_InitCard *)pBuffer;

	BYTE cbCardData[2];	
	
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//变量定义
		WORD wViewChairID = SwitchViewChairID(i);
		const tagUserData * pUserData = GetUserData(i);

		//设置界面
		if (pUserData != NULL)
		{					
			cbCardData[1] = pInitCard->cbCardData[i];			
			cbCardData[0] = (wViewChairID == 2) ? pInitCard->cbFundusCard : 0;

			m_GameView.m_CardControl[wViewChairID].SetCardData(cbCardData,2);	

			//标识状态
			if(wViewChairID != m_GameView.m_wBankerStation)
				m_GameView.m_cbPlayStatus[wViewChairID] = 2;
		}
		else
		{
			m_GameView.m_CardControl[wViewChairID].SetCardData(NULL,0);
		}		

		//设置控件
		m_GameView.m_CardControl[wViewChairID].ShowFirstCard(true);
	}

	//界面设置
	if(SwitchViewChairID(GetMeChairID()) != m_GameView.m_wBankerStation && !IsLookonMode())
	{
		m_GameView.m_btGetCard.ShowWindow(SW_SHOW);
		m_GameView.m_btDouble.ShowWindow(SW_SHOW);
		m_GameView.m_btStop.ShowWindow(SW_SHOW);	
	
		//能否分牌
		if(pInitCard->bCanCutCard) m_GameView.m_btCutCard.ShowWindow(SW_SHOW);	

		//设置定时器
		SetGameTimer(GetMeChairID(), IDI_GET_CARD, 30);
	}	

	//显示扑克总值
	m_GameView.m_cbTopCardSum[2] = pInitCard->cbCardSum;
	m_GameView.UpdateGameView(NULL);

	return true;
}

//用户分牌
bool CBJGameDlg::OnSubCutCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_CutCard)) return false;
	CMD_S_CutCard * pCutCard = (CMD_S_CutCard *)pBuffer;

	BYTE cbFirstCard[1]; 
	BYTE cbSecondCard[1];
	cbFirstCard[0]     = pCutCard->cbFirstCard;
	cbSecondCard[0]    = pCutCard->cbSecondCard;
	WORD wViewChairID = SwitchViewChairID(pCutCard->wCutCardUser);

	m_GameView.m_CardControl[wViewChairID].SetCardData(cbFirstCard, 1);			
	m_GameView.m_CardControl[wViewChairID + 4].SetCardData(cbSecondCard,1);

	if(pCutCard->wCutCardUser == GetMeChairID())
	{
		m_GameView.m_CardControl[wViewChairID + 4].ShowFirstCard(true);
		m_GameView.m_cbCutCardMark  = 1;
		m_GameView.m_cbTopCardSum[wViewChairID]    = pCutCard->cbFirstCardSum;
		m_GameView.m_cbBottomCardSum[wViewChairID] = pCutCard->cbSecondCardSum;
	}
	else
	{
		m_GameView.m_CardControl[wViewChairID + 4].ShowFirstCard(false);
		m_GameView.m_CardControl[wViewChairID].ShowFirstCard(false);
	}
	m_GameView.UpdateGameView(NULL);

	//标识分牌
	m_bCutCard = false;

	return true;
}

//用户加注
bool CBJGameDlg::OnSubAddGold(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_AddGold)) return false;
	CMD_S_AddGold * pAddGold = (CMD_S_AddGold *)pBuffer;

	//设置状态
	SetGameStatus(GS_PLAYING);

	//变量定义
	WORD wMeChairID   = GetMeChairID();
	WORD wViewChairID = SwitchViewChairID(pAddGold->wCurrentUser);

	//处理数据
	m_GameView.SetUserGoldInfo(wViewChairID,false,pAddGold->lLastAddGold);

	if ((IsLookonMode() == false)&&(pAddGold->wCurrentUser == wMeChairID))
	{
		//设置数据
		m_bMeAddGold = true;
	
		//不是双倍
		if(!pAddGold->bDoubleGold)
		{
			//获取扑克
			WORD wCardCount = m_GameView.m_CardControl[2].GetCardData(NULL,0);
		}
		else
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
		}
		ActiveGameFrame();
	}

	//标识状态
	if(wViewChairID != m_GameView.m_wBankerStation)
		m_GameView.m_cbPlayStatus[wViewChairID] = 0;
	m_GameView.UpdateGameView(NULL);

	return true;
}

//切换分牌
bool CBJGameDlg::OnCutCardMark(const void * pBuffer, WORD wDataSize)
{	
	m_GameView.m_cbCutCardMark = 2;

	m_GameView.m_btGetCard.ShowWindow(SW_SHOW);
	m_GameView.m_btStop.ShowWindow(SW_SHOW);
	m_GameView.m_btDouble.ShowWindow(SW_SHOW);

	m_GameView.UpdateGameView(NULL);

	//标识分牌
	m_bCutCard = true;

	return true;
}
//发牌消息
bool CBJGameDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{		
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
	
	//效验数据
	if (wDataSize != sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * pSendCard = (CMD_S_SendCard *)pBuffer;

	//更新界面
	BYTE cbCardData[8];
	LONG lUserTableGold = 0L;

	WORD wCardCount;
	WORD wViewChairID = SwitchViewChairID(pSendCard->wGetCardUser);

	//左手牌
	if(!pSendCard->bCutCard)
	{
		wCardCount = m_GameView.m_CardControl[wViewChairID].GetCardData(cbCardData,CountArray(cbCardData));
		cbCardData[wCardCount++] = pSendCard->cbUserCard;
		m_GameView.m_CardControl[wViewChairID].SetCardData(cbCardData,wCardCount);

		//显示扑克总值
		if(wViewChairID == 2)
		{
			m_GameView.m_cbTopCardSum[2] = pSendCard->cbCardSum;	
		}	
	}
	else
	{
		//右手牌
		wCardCount = m_GameView.m_CardControl[wViewChairID + 4].GetCardData(cbCardData,CountArray(cbCardData));
		cbCardData[wCardCount++] = pSendCard->cbUserCard;
		m_GameView.m_CardControl[wViewChairID + 4].SetCardData(cbCardData,wCardCount);

		//显示扑克总值
		if(wViewChairID == 2)
		{
			m_GameView.m_cbBottomCardSum[2] = pSendCard->cbCardSum;			
		}	
	}

	m_GameView.Invalidate();
	m_GameView.UpdateGameView(NULL);

	return true;
}

//停牌消息
bool CBJGameDlg::OnSubStopCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize != sizeof(CMD_S_StopCard)) return false;
	CMD_S_StopCard * pStopCard = (CMD_S_StopCard *)pBuffer;

	WORD wViewChairID = SwitchViewChairID(pStopCard->wStopUser);

	//标识状态
	if((wViewChairID != m_GameView.m_wBankerStation) && (wViewChairID != 2)) 
	{
		if(!pStopCard->bDoubleBrsut)
			m_GameView.m_cbPlayStatus[wViewChairID] = 3;
		else
			m_GameView.m_cbPlayStatus[wViewChairID] = 4;
	}	

	if(wViewChairID == m_GameView.m_wBankerStation) m_GameView.m_cbPlayStatus[wViewChairID] = 0;

	m_GameView.UpdateGameView(NULL);

	return true;
}

//爆牌消息
bool CBJGameDlg::OnSubBrustCard(const void * pBuffer, WORD wDataSize)
{
	if (wDataSize != sizeof(CMD_S_BrustCard)) return false;
	CMD_S_BrustCard * pBrustCard = (CMD_S_BrustCard *)pBuffer;

	WORD wViewChairID = SwitchViewChairID(pBrustCard->wBrustUser);
	
	if(pBrustCard->wBrustUser == GetMeChairID())
	{
		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("BRUSH_CARD"));

		//设置定时器
		KillGameTimer(IDI_GET_CARD);
		
		//更新界面
		m_GameView.m_btGetCard.ShowWindow(SW_HIDE);
		m_GameView.m_btStop.ShowWindow(SW_HIDE);
		m_GameView.m_btDouble.ShowWindow(SW_HIDE);
		m_GameView.m_btCutCard.ShowWindow(SW_HIDE);		
	}

	//设置界面
	m_GameView.m_CardControl[wViewChairID].ShowFirstCard(true);	

	//标识状态
	if(wViewChairID != m_GameView.m_wBankerStation)
		m_GameView.m_cbPlayStatus[wViewChairID] = 4;
	m_GameView.UpdateGameView(NULL);

	return true;
}

//庄家发牌
bool CBJGameDlg::OnSubBankerSendCard(const void * pBuffer, WORD wDataSize)
{
	//设置界面
	m_GameView.m_btGetCard.ShowWindow(SW_SHOW);
	m_GameView.m_btStop.ShowWindow(SW_SHOW);

	//设置定时器
	SetGameTimer(GetMeChairID(), IDI_GET_CARD, 30);
	m_GameView.UpdateGameView(NULL);

	return true;
}

//用户强退
bool CBJGameDlg::OnSubUserLeft(const void * pBuffer, WORD wDataSize)
{
	return true;
}

//游戏结束
bool CBJGameDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{	
	//设置定时器
	KillGameTimer(IDI_GET_CARD);

	//效验参数
	if (wDataSize != sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd = (CMD_S_GameEnd *)pBuffer;
    
	//游戏正常结束
	BYTE cbCardData[13];
	m_GameView.m_ScoreView.ResetScore();

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//设置信息
		const tagUserData * pUserData = GetUserData(i);

		if (pUserData != NULL)
		{
			lstrcpyn(m_szName[i],pUserData->szName,CountArray(m_szName[i]));
		}
		
		if (m_szName[i][0] != 0) 
		{	
			m_GameView.m_ScoreView.SetGameScore(i,m_szName[i],pGameEnd->lGameGold[i]);

			if (pGameEnd->cbUserCard[i] != 0)
			{
				WORD wViewStation = SwitchViewChairID(i);
				WORD wCardCount = m_GameView.m_CardControl[wViewStation].GetCardData(cbCardData,CountArray(cbCardData));
				cbCardData[0] = pGameEnd->cbUserCard[i];
				m_GameView.m_CardControl[wViewStation].SetCardData(cbCardData,wCardCount);
				m_GameView.m_CardControl[wViewStation].SortCardData();
				m_GameView.m_CardControl[wViewStation].ShowFirstCard(true);	

				wCardCount = m_GameView.m_CardControl[wViewStation+4].GetCardData(cbCardData,CountArray(cbCardData));
				cbCardData[0] = pGameEnd->cbCutCard[i];
				m_GameView.m_CardControl[wViewStation+4].SetCardData(cbCardData,wCardCount);
				m_GameView.m_CardControl[wViewStation+4].SortCardData();
				m_GameView.m_CardControl[wViewStation+4].ShowFirstCard(true);		
				
				//显示总值
				m_GameView.m_cbTopCardSum[wViewStation]    = pGameEnd->cbCardSum[i];
				m_GameView.m_cbBottomCardSum[wViewStation] = pGameEnd->cbCutCardSum[i];
			}			
		}			
	}
	m_GameView.m_ScoreView.SetTax(pGameEnd->lTax);
	m_GameView.m_ScoreView.ShowWindow(SW_SHOW);	

	//游戏变量
	m_lTurnMaxGold   = 0L;
	m_lTurnBasicGold = 0L;

	//界面变量
	m_lGoldShow  = 0L;	
	m_GameView.m_wBankerStation = 255;
	m_GameView.m_cbCutCardMark  = 0;
	memset(m_szName,0,sizeof(m_szName));
	memset(m_GameView.m_cbPlayStatus,0,sizeof(m_GameView.m_cbPlayStatus));

	//标识分牌
	m_bCutCard = false;

	//设置界面
	if (IsLookonMode() == false)
	{
		m_GameView.m_btBegin.ShowWindow(SW_SHOW);
		m_GameView.m_btBegin.SetFocus();
		//SetGameTimer(GetMeChairID(),IDI_STRAT_GAME,30);
	}
	
	m_GameView.m_GoldControl.ShowWindow(SW_HIDE);
	m_GameView.m_btGiveUp.ShowWindow(SW_HIDE);
	m_GameView.m_btAddGold.ShowWindow(SW_HIDE);

	m_GameView.m_btDouble.ShowWindow(SW_HIDE);
	m_GameView.m_btGetCard.ShowWindow(SW_HIDE);
	m_GameView.m_btStop.ShowWindow(SW_HIDE);
	m_GameView.m_btCutCard.ShowWindow(SW_HIDE);	
	
	m_GameView.UpdateGameView(NULL);

	//设置状态
	SetGameStatus(GS_FREE);

	//播放声音
	if(pGameEnd->lGameGold[GetMeChairID()] > 0)
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));	

	return true;
}

//游戏开始
LRESULT	CBJGameDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("HIT_BUTTON"));
	
	//设置界面	
	m_GameView.ResetGameView();
	m_GameView.m_btBegin.ShowWindow(SW_HIDE);
	m_GameView.m_ScoreView.ShowWindow(SW_HIDE);	
	for (int i=0;i<CountArray(m_GameView.m_CardControl);i++) m_GameView.m_CardControl[i].SetCardData(NULL,0);
	memset(m_GameView.m_cbTopCardSum,0,sizeof(m_GameView.m_cbTopCardSum));
	memset(m_GameView.m_cbBottomCardSum,0,sizeof(m_GameView.m_cbBottomCardSum));
	memset(m_GameView.m_cbPlayStatus,0,sizeof(m_GameView.m_cbPlayStatus));

	KillGameTimer(IDI_STRAT_GAME);

	//发送消息
    SendUserReady(NULL,0);

	return true;
}

//用户下注
LRESULT	CBJGameDlg::OnAddGold(WPARAM wParam, LPARAM lParam)
{	
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
	
	LONG lGold =  m_GameView.m_GoldView[4].GetGold() + m_GameView.m_GoldControl.GetGold();

	lGold = lGold;//+m_lTurnBasicGold;/*__max(lGold,m_lTurnBasicGold);*/

	//设置信息
	CMD_C_AddGold AddPoint;
	AddPoint.bDoubleGold = false;
	AddPoint.lGold       = lGold;

	SendData(SUB_C_ADD_GOLD,&AddPoint,sizeof(AddPoint));

	//设置定时器
	KillGameTimer(IDI_ADD_GOLD);	

	//更新界面
	m_GameView.m_GoldControl.ShowWindow(SW_HIDE);
	m_GameView.m_btGiveUp.ShowWindow(SW_HIDE);
	m_GameView.m_btAddGold.ShowWindow(SW_HIDE);
	m_GameView.UpdateGameView(NULL);

	return 0;
}

//用户取消
LRESULT	CBJGameDlg::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("HIT_BUTTON"));

	CMD_C_AddGold AddPoint;
	AddPoint.lGold       = m_lTurnBasicGold;
	AddPoint.bDoubleGold = false;

	//设置定时器
	KillGameTimer(IDI_ADD_GOLD);
	
	//设置界面
	m_GameView.m_GoldControl.ShowWindow(SW_HIDE);
	m_GameView.m_btGiveUp.ShowWindow(SW_HIDE);
	m_GameView.m_btAddGold.ShowWindow(SW_HIDE);
    
	//发送数据
	SendData(SUB_C_ADD_GOLD,&AddPoint,sizeof(AddPoint));

	return true;
}

//用户要牌
LRESULT CBJGameDlg::OnGetCard(WPARAM wParam, LPARAM lParam)
{	
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("HIT_BUTTON"));
	
	//设置定时器
	KillGameTimer(IDI_GET_CARD);
	SetGameTimer(GetMeChairID(), IDI_GET_CARD, 30);	

	//发送消息
	CMD_C_GetCard GetCard;
	GetCard.bCutCard = m_bCutCard;

	SendData(SUB_C_GET_CARD,&GetCard,sizeof(GetCard));

	//设置界面
	m_GameView.m_btCutCard.ShowWindow(SW_HIDE);	

	return true;
}

//用户停牌
LRESULT CBJGameDlg::OnStopCard(WPARAM wParam, LPARAM lParam)
{
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("HIT_BUTTON"));
	
	//停止定时器
	KillGameTimer(IDI_GET_CARD);

	//设置界面
	m_GameView.m_btGetCard.ShowWindow(SW_HIDE);
	m_GameView.m_btDouble.ShowWindow(SW_HIDE);
	m_GameView.m_btStop.ShowWindow(SW_HIDE);
	m_GameView.m_btCutCard.ShowWindow(SW_HIDE);

	//发送消息
	CMD_C_StopCard StopCard;
	StopCard.bCutCard = m_bCutCard;
	SendData(SUB_C_STOP_CARD,&StopCard,sizeof(StopCard));

	return true;
}

//用户双倍
LRESULT CBJGameDlg::OnDoubleCard(WPARAM wParam, LPARAM lParam)
{
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
	
	//停止定时器
	KillGameTimer(IDI_GET_CARD);
	
	//发送消息
	CMD_C_AddGold AddPoint;
	AddPoint.bDoubleGold = true;
	AddPoint.lGold       = 0L;   
	SendData(SUB_C_ADD_GOLD,&AddPoint,sizeof(AddPoint));	
	
	CMD_C_GetCard GetCard;
	GetCard.bCutCard = m_bCutCard;
	SendData(SUB_C_GET_CARD,&GetCard,sizeof(GetCard));

	SendData(SUB_C_STOP_CARD);	

	//设置界面
	m_GameView.m_btGetCard.ShowWindow(SW_HIDE);
	m_GameView.m_btDouble.ShowWindow(SW_HIDE);
	m_GameView.m_btStop.ShowWindow(SW_HIDE);
	m_GameView.m_btCutCard.ShowWindow(SW_HIDE);

	return true;
}

//用户分牌
LRESULT CBJGameDlg::OnCutCard(WPARAM wParam, LPARAM lParam)
{
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("HIT_BUTTON"));	

	//发送消息
	SendData(SUB_C_CUT_CARD);

	//设置界面
	m_GameView.m_btCutCard.ShowWindow(SW_HIDE);
	
	return true;
}








