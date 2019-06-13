#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_START_GAME					200								//开始定时器
#define IDI_GIVE_UP						201								//放弃定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_FOLLOW,OnFollow)
	ON_MESSAGE(IDM_GIVE_UP,OnGiveUp)
	ON_MESSAGE(IDM_SHOWHAND,OnShowHand)
	ON_MESSAGE(IDM_ADD_GOLD,OnAddGold)
	ON_MESSAGE(IDM_SEND_CARD_FINISH,OnSendCardFinish)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_lTurnMaxGold=0L;
	m_lTurnBasicGold=0L;
	m_lBasicGold = 0L;
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));

	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;
	m_bAddGold = false;
	memset(m_szName,0,sizeof(m_szName));

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_lTurnMaxGold=0L;
	m_lTurnBasicGold=0L;
	m_lBasicGold = 0L;
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
	m_wCurrentUser = INVALID_CHAIR;


	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;
	m_bAddGold = false;
	memset(m_szName,0,sizeof(m_szName));

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bAllowLookon=IsAllowUserLookon();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
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
	case IDI_START_GAME:		//继续游戏定时器
		{
			if (nElapse==0)
			{
				PostMessage(WM_CLOSE,0,0);
				return false;
			}
			if (nElapse<=10) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_GIVE_UP:			//放弃定时器
		{
			WORD wViewChairID=SwitchViewChairID(wChairID);
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wViewChairID==2)) OnGiveUp(0,0);
				return false;
			}
			if ((nElapse<=10)&&(wViewChairID==2)&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	}
	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	//控件控制
	m_GameClientView.m_CardControl[2].AllowPositively(IsAllowLookon());

	return;
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_ADD_GOLD:	//用户下注
		{
			m_GameClientView.FinishDispatchCard();
			return OnSubAddGold(pBuffer,wDataSize);
		}
	case SUB_S_GIVE_UP:		//用户放弃
		{
			m_GameClientView.FinishDispatchCard();
			return OnSubGiveUp(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:	//发牌消息
		{
			m_GameClientView.FinishDispatchCard();
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:	//游戏结束
		{
			m_GameClientView.FinishDispatchCard();
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	}

	return false;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//辅助变量
			m_bShowHand=false;
			m_bMeAddGold=false;

			//设置界面
			m_lGoldShow=0L;
			m_lTurnBasicGold=0L;
			m_lBasicGold = pStatusFree->dwBasicGold;
			m_GameClientView.SetGoldTitleInfo(0,pStatusFree->dwBasicGold);

			//玩家设置
			if (IsLookonMode()==false)
			{
				//设置按钮
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();

				//设置时间
				SetGameTimer(GetMeChairID(),IDI_START_GAME,30);
			}

			return true;
		}
	case GS_PLAYING:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置变量
			m_lTurnMaxGold=pStatusPlay->lTurnMaxGold;
			m_lTurnBasicGold=pStatusPlay->lTurnBasicGold;
			m_lBasicGold = pStatusPlay->lBasicGold;
			CopyMemory(m_bPlayStatus,pStatusPlay->bPlayStatus,sizeof(m_bPlayStatus));
			m_wCurrentUser = pStatusPlay->wCurrentUser;
//			m_bShowHand = pStatusPlay->bShowHand;

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewStation=SwitchViewChairID(i);
				const tagUserData * pUserData=GetUserData(i);
				if (pUserData!=NULL)
				{
					lstrcpyn(m_szName[i],pUserData->szName,CountArray(m_szName[i]));
					if (m_bPlayStatus[i]==TRUE) m_GameClientView.m_CardControl[wViewStation].SetCardData(pStatusPlay->bTableCardArray[i],pStatusPlay->bTableCardCount[i]);
				}
				m_GameClientView.SetUserGoldInfo(wViewStation,false,pStatusPlay->lTableGold[2*i]);
				m_GameClientView.SetUserGoldInfo(wViewStation,true,pStatusPlay->lTableGold[2*i+1]);

				//判断是否梭哈
				if (i==GetMeChairID())
				{
					m_bShowHand=((pStatusPlay->lMaxGold-pStatusPlay->lTableGold[2*i+1]-pStatusPlay->lTableGold[2*i])<=0L);
				}
			}
			m_GameClientView.SetGoldTitleInfo(pStatusPlay->lMaxGold,pStatusPlay->lBasicGold);

			//判断是否观看
			if ((IsLookonMode()==false)||(IsAllowLookon()==true)) m_GameClientView.m_CardControl[2].AllowPositively(true);

			//判断是否自己下注
			if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				m_bMeAddGold=true;
				BYTE wCardCount=pStatusPlay->bTableCardCount[GetMeChairID()];

				LONG lLeaveScore=m_lTurnMaxGold-m_lTurnBasicGold-m_GameClientView.m_GoldView[5].GetGold();

				//显示按钮
				m_GameClientView.ShowScoreControl( true );

				//禁止按钮
				m_GameClientView.m_btShowHand.EnableWindow((m_bShowHand==true)||(wCardCount>=3&&lLeaveScore>0)?TRUE:FALSE);
				m_GameClientView.m_btFollow.EnableWindow(((m_bShowHand==false)||(lLeaveScore>=0L)||(m_lTurnBasicGold==0L))?TRUE:FALSE);

				//加注按钮
				lLeaveScore=m_lTurnMaxGold-m_lTurnBasicGold-m_GameClientView.m_GoldView[5].GetGold();
				m_GameClientView.m_btAddTimes1.EnableWindow( (lLeaveScore>=m_lBasicGold)?TRUE:FALSE );
				m_GameClientView.m_btAddTimes2.EnableWindow( (lLeaveScore>=2*m_lBasicGold)?TRUE:FALSE );
				m_GameClientView.m_btAddTimes3.EnableWindow( (lLeaveScore>=3*m_lBasicGold)?TRUE:FALSE );
			}
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_GIVE_UP,30);

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//游戏变量
	m_lTurnMaxGold=pGameStart->lTurnMaxGold;
	m_lTurnBasicGold=pGameStart->lTurnBasicGold;
	m_lBasicGold = pGameStart->lBasicGold;
	memset(m_szName,0,sizeof(m_szName));
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
	m_wCurrentUser = pGameStart->wCurrentUser;

	//用户名称
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		const tagUserData * pUserData=GetUserData(i);
		if( pUserData == NULL ) 
		{
			m_szAccounts[i][0] = 0;
			continue;
		}
		lstrcpy( m_szAccounts[i],pUserData->szName );
	}

	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	bool bLookonMode=IsLookonMode();
	LONG lBaseGold=m_lBasicGold;
	m_GameClientView.SetGoldTitleInfo( pGameStart->lMaxGold,m_lBasicGold );

	//设置状态
	SetGameStatus(GS_PLAYING);

	//设置界面
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);
		const tagUserData * pUserData=GetUserData(i);

		//设置界面
		if (pUserData!=NULL)
		{
			m_bPlayStatus[i]=TRUE;
			m_GameClientView.SetUserGoldInfo(wViewChairID,false,0L);
			m_GameClientView.SetUserGoldInfo(wViewChairID,true,lBaseGold);
			m_GameClientView.m_GoldView[i*2].SetGoldIndex(1);
			lstrcpyn(m_szName[i],pUserData->szName,CountArray(m_szName[i]));
		}
		else
		{
			m_bPlayStatus[i]=FALSE;
			m_GameClientView.SetUserGoldInfo(wViewChairID,true,0L);
			m_GameClientView.SetUserGoldInfo(wViewChairID,false,0L);
			
		}

		//设置控件
		m_GameClientView.m_CardControl[wViewChairID].SetCardData(NULL,0);
		m_GameClientView.m_CardControl[wViewChairID].ShowFirstCard(false);
	}
	if (bLookonMode==false) 
	{
		ActiveGameFrame();
		m_GameClientView.m_CardControl[2].AllowPositively(true);
	}

	//旁观控制
	if ((bLookonMode==true)&&(IsAllowLookon()==true))
	{
		m_GameClientView.m_CardControl[2].AllowPositively(true);
	}

	//设置控制
	if ((bLookonMode==false)&&(pGameStart->wCurrentUser==wMeChairID))
	{
		m_bMeAddGold=true;
	}

	//派发扑克
	for (BYTE cbIndex=0;cbIndex<2;cbIndex++)
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (m_bPlayStatus[i]==TRUE)
			{
				//变量定义
				WORD wViewChairID=SwitchViewChairID(i);
				BYTE cbCardData[2]={0,pGameStart->bCardData[i]};

				//派发扑克
				cbCardData[0]=(GetMeChairID()==i)?pGameStart->bFundusCard:0;
				m_GameClientView.DispatchUserCard(wViewChairID,cbCardData[cbIndex]);
			}
		}
	}

	//设置
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//用户加注
bool CGameClientDlg::OnSubAddGold(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_AddGold)) return false;
	CMD_S_AddGold * pAddGold=(CMD_S_AddGold *)pBuffer;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wViewChairID=SwitchViewChairID(pAddGold->wLastChairID);
	m_wCurrentUser = pAddGold->wCurrentUser;

	//处理数据
	m_lTurnBasicGold=__max(pAddGold->lLastAddGold,pAddGold->lCurrentLessGold);
	if( m_lTurnBasicGold >= m_lTurnMaxGold-m_GameClientView.m_GoldView[5].GetGold() )
		m_bShowHand = true;
	m_GameClientView.SetUserGoldInfo(wViewChairID,false,pAddGold->lLastAddGold);
	if ((IsLookonMode()==false)&&(pAddGold->wCurrentUser==wMeChairID))
	{
		//设置数据
		m_bMeAddGold=true;
		m_lGoldShow=m_GameClientView.m_GoldView[4].GetGold();

		//变量定义
		WORD wCardCount=m_GameClientView.m_CardControl[2].GetCardData(NULL,0);
		LONG lGold = m_GameClientView.m_GoldView[5].GetGold();
		LONG lLeaveScore=m_lTurnMaxGold-m_lTurnBasicGold-m_GameClientView.m_GoldView[5].GetGold();

		//显示按钮
		m_GameClientView.ShowScoreControl( true );

		//禁止按钮
		m_GameClientView.m_btShowHand.EnableWindow((m_bShowHand||(wCardCount>=3&&lLeaveScore>0))?TRUE:FALSE);
		m_GameClientView.m_btFollow.EnableWindow((m_bShowHand==false&&((lLeaveScore>=0L)||(m_lTurnBasicGold==0L)))?TRUE:FALSE);

		ActiveGameFrame();

		//加注按钮
		m_GameClientView.m_btAddTimes1.EnableWindow( (lLeaveScore>=m_lBasicGold&&m_bAddGold==false)?TRUE:FALSE );
		m_GameClientView.m_btAddTimes2.EnableWindow( (lLeaveScore>=2*m_lBasicGold&&m_bAddGold==false)?TRUE:FALSE );
		m_GameClientView.m_btAddTimes3.EnableWindow( (lLeaveScore>=3*m_lBasicGold&&m_bAddGold==false)?TRUE:FALSE );
	}

	//其他处理
	SetGameTimer(pAddGold->wCurrentUser,IDI_GIVE_UP,30);
	LONG lLeaveScore=m_lTurnMaxGold-m_lTurnBasicGold-m_GameClientView.m_GoldView[5].GetGold();
	WORD wCardCount=m_GameClientView.m_CardControl[2].GetCardData(NULL,0);
	if ( m_bShowHand && m_bPlayStatus[pAddGold->wLastChairID]) PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
	else if (m_bPlayStatus[pAddGold->wLastChairID]==TRUE) PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

	if( m_bShowHand && pAddGold->wLastChairID != wMeChairID && m_bPlayStatus[pAddGold->wLastChairID] )
		m_GameClientView.m_GoldView[wViewChairID*2].SetGoldIndex(2);

	return true;
}

//用户放弃
bool CGameClientDlg::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	//设置变量
	m_bPlayStatus[pGiveUp->wUserChairID]=false;
	if (pGiveUp->wUserChairID==GetMeChairID()) SetGameStatus(GS_FREE);

	//设置界面
	BYTE bCard[5]={0,0,0,0,0};
	WORD wViewStation=SwitchViewChairID(pGiveUp->wUserChairID);
	WORD bCount=m_GameClientView.m_CardControl[wViewStation].GetCardCount();
	m_GameClientView.m_CardControl[wViewStation].AllowPositively(false);
	m_GameClientView.m_CardControl[wViewStation].ShowFirstCard(false);
	m_GameClientView.m_CardControl[wViewStation].SetCardData(bCard,bCount);

	//其他处理
	if (pGiveUp->wUserChairID==GetTimeChairID()) KillGameTimer(IDI_GIVE_UP);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	return true;
}

//发牌消息
bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_R_SendCard)) return false;
	CMD_R_SendCard * pSendCard=(CMD_R_SendCard *)pBuffer;

	//设置变量
	m_lGoldShow=0L;
	m_lTurnBasicGold=0L;
	m_lTurnMaxGold=pSendCard->lMaxGold;
	m_wCurrentUser = pSendCard->wCurrentUser;
	m_bAddGold = false;

	//更新界面
	LONG lUserTableGold=0L;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取位置
		WORD wViewChairID=SwitchViewChairID(i);

		//设置金币
		lUserTableGold=m_GameClientView.m_GoldView[wViewChairID*2].GetGold();
		lUserTableGold+=m_GameClientView.m_GoldView[wViewChairID*2+1].GetGold();
		m_GameClientView.SetUserGoldInfo(wViewChairID,false,0L);
		m_GameClientView.SetUserGoldInfo(wViewChairID,true,lUserTableGold);
	}

	//下注判断
	if ((IsLookonMode()==false)&&(pSendCard->wCurrentUser==GetMeChairID()))
	{
		m_bMeAddGold=true;
	}

	//派发扑克,从上次最大玩家开始发起
	WORD wLastMostUser = pSendCard->wStartChairId;
	ASSERT( wLastMostUser != INVALID_CHAIR );
	for (BYTE i=0;i<pSendCard->cbSendCardCount;i++)
	{
		for (WORD j=0;j<GAME_PLAYER;j++)
		{
			WORD wChairId = (wLastMostUser+j)%GAME_PLAYER;
			if (m_bPlayStatus[wChairId]==TRUE&&pSendCard->bUserCard[wChairId][i]!=0)
			{
				WORD wViewChairID=SwitchViewChairID(wChairId);
				m_GameClientView.DispatchUserCard(wViewChairID,pSendCard->bUserCard[wChairId][i]);
			}
		}
	}

	//播放声音
// 	if( !m_bShowHand )
// 		PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
// 	else 
//		PlayGameSound(AfxGetInstanceHandle(), TEXT("GET_CARD"));

	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//播放声音
	if (IsLookonMode()==false)
	{
		if (pGameEnd->lGameGold[GetMeChairID()]>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//设置积分
	CString strTemp ,strEnd = "\n";
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if(pGameEnd->lGameGold[i]>0)
			strTemp.Format("%s: %+ld\n",m_szAccounts[i],pGameEnd->lGameGold[i]);
		else if( m_szAccounts[i][0] != 0 )
			strTemp.Format("%s: %ld\n",m_szAccounts[i],pGameEnd->lGameGold[i]);
		else strTemp.Empty();
		strEnd += strTemp;
	}
	//消息积分
	InsertSystemString((LPCTSTR)strEnd);

	//游戏正常结束
	BYTE bCardData[5];
	m_GameClientView.m_ScoreView.ResetScore();
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置信息
		if (pGameEnd->lGameGold[i]!=0L)
		{
			if (m_szName[i][0]==0) m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("已离开"),pGameEnd->lGameGold[i]);
			else m_GameClientView.m_ScoreView.SetGameScore(i,m_szName[i],pGameEnd->lGameGold[i]);
			if (pGameEnd->bUserCard[i]!=0)
			{
				WORD wViewStation=SwitchViewChairID(i);
				WORD wCardCount=m_GameClientView.m_CardControl[wViewStation].GetCardData(bCardData,CountArray(bCardData));
				bCardData[0]=pGameEnd->bUserCard[i];
				m_GameClientView.m_CardControl[wViewStation].SetCardData(bCardData,wCardCount);
				m_GameClientView.m_CardControl[wViewStation].ShowFirstCard(true);
			}
		}
	}
	m_GameClientView.m_ScoreView.SetTax(pGameEnd->lTax);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置控件
	KillGameTimer(IDI_GIVE_UP);
	//显示按钮
	m_GameClientView.ShowScoreControl( false );

	//设置状态
	SetGameStatus(GS_FREE);

	//游戏变量
	m_lTurnMaxGold=0L;
	m_lTurnBasicGold=0L;
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
	m_wCurrentUser = INVALID_CHAIR;

	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;
	m_bAddGold = false;
	memset(m_szName,0,sizeof(m_szName));

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		m_GameClientView.m_btStart.SetFocus();
		SetGameTimer(GetMeChairID(),IDI_START_GAME,30);
	}

	return true;
}

//开始按钮
LRESULT	CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//删除时间
	KillGameTimer(IDI_START_GAME);

	//设置控件
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_GoldView[i*2].SetGold(0L);
		m_GameClientView.m_GoldView[i*2+1].SetGold(0L);
		m_GameClientView.m_GoldView[i*2].SetGoldIndex(1);
		m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_CardControl[i].ShowFirstCard(false);
	}

	//设置控件
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	//显示按钮
	m_GameClientView.ShowScoreControl( false );


	//隐藏控件
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//放弃按钮
LRESULT CGameClientDlg::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	//放弃游戏
	m_bMeAddGold=false;
	KillGameTimer(IDI_GIVE_UP);
	//显示按钮
	m_GameClientView.ShowScoreControl( false );

	m_GameClientView.SetUserGoldInfo(2,false,m_lGoldShow);

	//发送消息
	SendData(SUB_C_GIVE_UP);

	return 0;
}

//跟注按钮
LRESULT CGameClientDlg::OnFollow(WPARAM wParam, LPARAM lParam)
{
	//获取金币
	LONG lGold;
	if( (BOOL)wParam )
	{
		const tagUserData * pUserData=GetUserData(GetMeChairID());
		ASSERT( pUserData != NULL );
		lGold = pUserData->lScore-m_GameClientView.m_GoldView[5].GetGold();
	}else
	{
		lGold = m_GameClientView.m_GoldView[4].GetGold();
		lGold = __max(lGold,m_lTurnBasicGold);
	}

	//设置变量
	m_bMeAddGold=false;
	m_bAddGold = true;

	//设置界面
	if( (BOOL)wParam )
	{
		m_GameClientView.m_GoldView[4].SetGoldIndex(2);
	}

	//显示按钮
	m_GameClientView.ShowScoreControl( false );

	m_GameClientView.SetUserGoldInfo(2,false,lGold);

	//删除定时器
	KillGameTimer(IDI_GIVE_UP);
	
	//发送数据
	CMD_C_AddGold AddPoint;
	AddPoint.lGold=lGold;
	SendData(SUB_C_ADD_GOLD,&AddPoint,sizeof(AddPoint));

	return 0;
}

//梭哈按钮
LRESULT CGameClientDlg::OnShowHand(WPARAM wParam, LPARAM lParam)
{
	LONG lGold=m_lTurnMaxGold-m_GameClientView.m_GoldView[5].GetGold();
	LONG dwMoney4=m_GameClientView.m_GoldView[4].GetGold();

	//
	m_bAddGold = true;

	if (m_bMeAddGold==true)
	{
		m_GameClientView.SetUserGoldInfo(2,false,lGold);
		OnFollow(TRUE,0);
	}

	m_GameClientView.m_GoldView[4].SetGoldIndex(2);

	return 0;
}

//加注消息
LRESULT CGameClientDlg::OnAddGold(WPARAM wParam, LPARAM lParam)
{
	//增加筹码
	if (m_lTurnBasicGold>m_GameClientView.m_GoldView[4].GetGold())
	{
		m_GameClientView.m_GoldView[4].SetGold(m_lTurnBasicGold);
	}

	//设置加注
	LONG lScore4=m_GameClientView.m_GoldView[4].GetGold();
	m_GameClientView.m_GoldView[4].SetGold(lScore4+m_lBasicGold*(LONG)wParam);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("CHOOSE_SCORE"));

	//
	OnFollow(0,0);

	return 0;
}

//发牌完成
LRESULT CGameClientDlg::OnSendCardFinish(WPARAM wParam, LPARAM lParam)
{
	//下注判断
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();

		LONG lLeaveScore=m_lTurnMaxGold-m_lTurnBasicGold-m_GameClientView.m_GoldView[5].GetGold();

		//显示按钮
		m_GameClientView.ShowScoreControl( true );

		//禁止按钮
		WORD wCardCount=m_GameClientView.m_CardControl[2].GetCardData(NULL,0);
		m_GameClientView.m_btShowHand.EnableWindow(((m_bShowHand==true)||(wCardCount>=3)&&(m_bShowHand==false))?TRUE:FALSE);
		m_GameClientView.m_btFollow.EnableWindow(((m_bShowHand==false)||(lLeaveScore>=0L)||(m_lTurnBasicGold==0L))?TRUE:FALSE);

		//加注按钮
		m_GameClientView.m_btAddTimes1.EnableWindow( (lLeaveScore>=m_lBasicGold)?TRUE:FALSE );
		m_GameClientView.m_btAddTimes2.EnableWindow( (lLeaveScore>=2*m_lBasicGold)?TRUE:FALSE );
		m_GameClientView.m_btAddTimes3.EnableWindow( (lLeaveScore>=3*m_lBasicGold)?TRUE:FALSE );
	}

	//设置定时器
	SetGameTimer(m_wCurrentUser,IDI_GIVE_UP,30);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
