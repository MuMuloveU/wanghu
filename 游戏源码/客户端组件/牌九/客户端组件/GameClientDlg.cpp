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
	ON_MESSAGE(IDM_ADD_GOLD,OnAddGold)
	ON_MESSAGE(IDM_SHOWHAND,OnShowHand)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_lTurnMaxGold=0L;
	m_lTurnBasicGold=0L;
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));

	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;
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
	//设置标题
	SetWindowText(TEXT("牌九游戏"));

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
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));

	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;
	memset(m_szName,0,sizeof(m_szName));

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=IsEnableSound();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		EnableSound(GameOption.m_bEnableSound);
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
				OnStart(0,0);
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
				if ((IsLookonMode()==false)&&(wViewChairID==2)) OnGiveUp(1,1);
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
			return OnSubAddGold(pBuffer,wDataSize);
		}
	case SUB_S_GIVE_UP:		//用户放弃
		{
			return OnSubGiveUp(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:	//发牌消息
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:	//游戏结束
		{
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
			m_GameClientView.SetGoldTitleInfo(0,pStatusFree->dwBasicGold);

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
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
			CopyMemory(m_bPlayStatus,pStatusPlay->bPlayStatus,sizeof(m_bPlayStatus));

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
				m_GameClientView.m_btFollow.ShowWindow(SW_SHOW);
				m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAddGold.ShowWindow((m_bShowHand==false)?SW_SHOW:SW_HIDE);
				m_GameClientView.m_btShowHand.ShowWindow(((wCardCount>=4)&&(m_bShowHand==false))?SW_SHOW:SW_HIDE);
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
	memset(m_szName,0,sizeof(m_szName));
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));

	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	bool bLookonMode=IsLookonMode();
	LONG lBaseGold=m_GameClientView.m_lBasicGold;

	//设置状态
	SetGameStatus(GS_PLAYING);
	m_GameClientView.SetGoldTitleInfo(pGameStart->lMaxGold,lBaseGold);

	//设置界面
	BYTE bCardData[4];
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
			//bCardData[1]=pGameStart->bCardData[i];
			//bCardData[0]=(wViewChairID==2)?pGameStart->bFundusCard:0;
			CopyMemory(bCardData,pGameStart->bCardData,sizeof(BYTE)*4);
			m_GameClientView.m_CardControl[wViewChairID].SetCardData(bCardData,4);
			m_GameClientView.SetUserGoldInfo(wViewChairID,false,0L);
			m_GameClientView.SetUserGoldInfo(wViewChairID,true,lBaseGold);
			lstrcpyn(m_szName[i],pUserData->szName,CountArray(m_szName[i]));
		}
		else
		{
			m_bPlayStatus[i]=FALSE;
			m_GameClientView.SetUserGoldInfo(wViewChairID,true,0L);
			m_GameClientView.SetUserGoldInfo(wViewChairID,false,0L);
			m_GameClientView.m_CardControl[wViewChairID].SetCardData(NULL,0);
		}

		//设置控件
		m_GameClientView.m_CardControl[wViewChairID].ShowFirstCard(false);
	}
	if (bLookonMode==false) 
	{
		ActiveGameFrame();
		m_GameClientView.m_CardControl[2].AllowPositively(true);
	}

	//设置控制
	if ((bLookonMode==false)&&(pGameStart->wCurrentUser==wMeChairID))
	{
		m_bMeAddGold=true;
		m_GameClientView.m_btFollow.ShowWindow(SW_SHOW);
		m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAddGold.ShowWindow(SW_SHOW);
	}

	//设置定时器
	SetGameTimer(pGameStart->wCurrentUser,IDI_GIVE_UP,30);
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

	//处理数据
	m_lTurnBasicGold=pAddGold->lLastAddGold;
	m_GameClientView.SetUserGoldInfo(wViewChairID,false,pAddGold->lLastAddGold);
	if ((IsLookonMode()==false)&&(pAddGold->wCurrentUser==wMeChairID))
	{
		//设置数据
		m_bMeAddGold=true;
		m_lGoldShow=m_GameClientView.m_GoldView[4].GetGold();

		//获取扑克
		WORD wCardCount=m_GameClientView.m_CardControl[2].GetCardData(NULL,0);

		//自己下注
		ActiveGameFrame();
		m_GameClientView.m_btFollow.ShowWindow(SW_SHOW);
		m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAddGold.ShowWindow((m_bShowHand==false)?SW_SHOW:SW_HIDE);
		m_GameClientView.m_btShowHand.ShowWindow((m_bShowHand==false)&&(wCardCount>=8)?SW_SHOW:SW_HIDE);
	}

	//其他处理
	SetGameTimer(pAddGold->wCurrentUser,IDI_GIVE_UP,30);
	if (m_bPlayStatus[pAddGold->wCurrentUser]==TRUE) PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

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

	//更新界面
	BYTE bCardData[5];
	LONG lUserTableGold=0L;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取位置
		WORD wViewChairID=SwitchViewChairID(i);

		//设置扑克
		if (m_bPlayStatus[i]==TRUE)
		{
			WORD wCardCount=m_GameClientView.m_CardControl[wViewChairID].GetCardData(bCardData,CountArray(bCardData));
			bCardData[wCardCount++]=pSendCard->bUserCard[i];
			m_GameClientView.m_CardControl[wViewChairID].SetCardData(bCardData,wCardCount);
		}

		//设置金币
		lUserTableGold=m_GameClientView.m_GoldView[wViewChairID*2].GetGold();
		lUserTableGold+=m_GameClientView.m_GoldView[wViewChairID*2+1].GetGold();
		m_GameClientView.SetUserGoldInfo(wViewChairID,false,0L);
		m_GameClientView.SetUserGoldInfo(wViewChairID,true,lUserTableGold);
	}

	//下注判断
	if ((IsLookonMode()==false)&&(pSendCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_bMeAddGold=true;
		m_GameClientView.m_btFollow.ShowWindow(SW_SHOW);
		m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAddGold.ShowWindow((m_bShowHand==false)?SW_SHOW:SW_HIDE);
		m_GameClientView.m_btShowHand.ShowWindow(((m_GameClientView.m_CardControl[2].GetCardData(NULL,0)>=4)&&(m_bShowHand==false))?SW_SHOW:SW_HIDE);
	}

	//设置定时器
	SetGameTimer(pSendCard->wCurrentUser,IDI_GIVE_UP,30);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

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
				m_GameClientView.m_CardControl[wViewStation].SortCardData();
				m_GameClientView.m_CardControl[wViewStation].ShowFirstCard(true);
			}
		}
	}
	m_GameClientView.m_ScoreView.SetTax(pGameEnd->lTax);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置控件
	KillGameTimer(IDI_GIVE_UP);
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);
	m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddGold.ShowWindow(SW_HIDE);
	m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);
	m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);

	//设置状态
	SetGameStatus(GS_FREE);

	//游戏变量
	m_lTurnMaxGold=0L;
	m_lTurnBasicGold=0L;
	memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));

	//辅助变量
	m_lGoldShow=0L;
	m_bShowHand=false;
	m_bMeAddGold=false;
	memset(m_szName,0,sizeof(m_szName));

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		m_GameClientView.m_btStart.SetFocus();
		//SetGameTimer(GetMeChairID(),IDI_START_GAME,30);
	}

	return true;
}

//开始按钮
LRESULT	CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//加注按钮 
LRESULT	CGameClientDlg::OnAddGold(WPARAM wParam, LPARAM lParam)
{
	if ((m_bMeAddGold==true)&&(m_GameClientView.m_GoldControl.IsWindowVisible()==FALSE))
	{
		LONG lGold=m_lTurnMaxGold;
		m_GameClientView.SetUserGoldInfo(2,false,m_lTurnBasicGold);
		lGold-=m_GameClientView.m_GoldView[4].GetGold();
		lGold-=m_GameClientView.m_GoldView[5].GetGold();
		m_GameClientView.m_GoldControl.SetMaxGold(lGold);
		m_GameClientView.m_GoldControl.ShowWindow(SW_SHOW);
	}

	return 0;
}

//放弃按钮
LRESULT CGameClientDlg::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	if ((wParam==0)&&(m_GameClientView.m_GoldControl.IsWindowVisible()==TRUE))
	{
		//放弃跟注
		m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);
		m_GameClientView.m_GoldControl.SetMaxGold(0L);
		m_GameClientView.SetUserGoldInfo(2,false,m_lGoldShow);
	}
	else
	{
		//放弃游戏
		m_bMeAddGold=false;
		KillGameTimer(IDI_GIVE_UP);
		m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);
		m_GameClientView.m_btAddGold.ShowWindow(SW_HIDE);
		m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
		m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);
		m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);
		m_GameClientView.SetUserGoldInfo(2,false,m_lGoldShow);

		//发送消息
		SendData(SUB_C_GIVE_UP);
	}
	return 0;
}

//跟注按钮
LRESULT CGameClientDlg::OnFollow(WPARAM wParam, LPARAM lParam)
{
	//获取金币
	LONG lGold=m_GameClientView.m_GoldView[4].GetGold()+m_GameClientView.m_GoldControl.GetGold();
	lGold=__max(lGold,m_lTurnBasicGold);

	//设置变量
	m_bMeAddGold=false;
	if ((m_GameClientView.m_lMaxGold-m_GameClientView.m_GoldView[5].GetGold()-lGold)==0) m_bShowHand=true;

	//设置界面
	m_GameClientView.m_GoldControl.SetMaxGold(0L);
	m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);
	m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddGold.ShowWindow(SW_HIDE);
	m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);
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
	if ((lGold-dwMoney4)>0L)
	{
		CString strMessage;
		strMessage.Format(TEXT("梭哈的下注是 %ld 筹码，确实要梭哈吗？"),lGold-dwMoney4);
		if (IDYES!=AfxMessageBox(strMessage,MB_YESNO|MB_DEFBUTTON2|MB_ICONINFORMATION)) return 0;
	}
	if (m_bMeAddGold==true)
	{
		m_GameClientView.SetUserGoldInfo(2,false,lGold);
		OnFollow(0,0);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
