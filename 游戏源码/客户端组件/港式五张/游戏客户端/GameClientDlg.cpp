#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_START_GAME				200									//开始定时器
#define IDI_USER_ADD_SCORE			201									//加注定时器

//时间标识
#define TIME_START_GAME				30									//开始定时器
//test
#define TIME_USER_ADD_SCORE			30									//放弃定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_NO_ADD,OnFollow)
	ON_MESSAGE(IDM_FOLLOW,OnFollow)
	ON_MESSAGE(IDM_GIVE_UP,OnGiveUp)
	ON_MESSAGE(IDM_ADD_SCORE,OnAddScore)
	ON_MESSAGE(IDM_SHOW_HAND,OnShowHand)
	ON_MESSAGE(IDM_SEND_CARD_FINISH,OnSendCardFinish)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//配置变量
	m_dwCardHSpace=DEFAULT_PELS;

	//加注信息
	m_lMaxScore=0L;
	m_lCellScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//状态变量
	m_bAddScore=false;
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

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

	//读取配置
	m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//配置控件
	for (int i=0;i<GAME_PLAYER;i++) m_GameClientView.m_CardControl[i].SetCardSpace(m_dwCardHSpace);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//加注信息
	m_lMaxScore=0L;
	m_lCellScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//状态变量
	m_bAddScore=false;
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_dwCardHSpace=m_dwCardHSpace;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bAllowLookon=IsAllowUserLookon();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置扑克
		m_dwCardHSpace=GameOption.m_dwCardHSpace;
		for (int i=0;i<GAME_PLAYER;i++) m_GameClientView.m_CardControl[i].SetCardSpace(m_dwCardHSpace);

		//设置配置
		EnableSound(GameOption.m_bEnableSound);
		AllowUserLookon(0,GameOption.m_bAllowLookon);

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始定时器
		{
			//中止判断
			if (nElapse==0)
			{
				PostMessage(WM_CLOSE,0,0);
				return false;
			}

			//警告通知
			if (nElapse<=5) PlayGameSound(AfxGetInstanceHandle(),TEXT("START_WARN"));

			return true;
		}
	case IDI_USER_ADD_SCORE:	//加注定时器
		{
			//获取位置
			WORD wViewChairID=SwitchViewChairID(wChairID);

			//中止判断
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wViewChairID==2)) OnGiveUp(1,1);
				return false;
			}

			//警告通知
			if ((nElapse<=3)&&(wViewChairID==2)&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("TIME_END"));
				return true;
			}

			return true;
		}
	}

	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	return;
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//游戏开始
		{
			//结束动画
			m_GameClientView.FinishDispatchCard();

			//消息处理
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_ADD_SCORE:	//用户下注
		{
			//结束动画
			m_GameClientView.FinishDispatchCard();

			//消息处理
			return OnSubAddScore(pBuffer,wDataSize);
		}
	case SUB_S_GIVE_UP:		//用户放弃
		{
			//结束动画
			m_GameClientView.FinishDispatchCard();

			//消息处理
			return OnSubGiveUp(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:	//发牌消息
		{
			//结束动画
			m_GameClientView.FinishDispatchCard();

			//消息处理
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:	//游戏结束
		{
			//结束动画
			m_GameClientView.FinishDispatchCard();

			//消息处理
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

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
			}

			//设置时间
			if (IsLookonMode()==false) SetGameTimer(GetMeChairID(),IDI_START_GAME,30);

			return true;
		}
	case GS_PLAYING:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//下注信息
			m_lMaxScore=pStatusPlay->lMaxScore;
			m_lCellScore=pStatusPlay->lCellScore;
			m_lTurnMaxScore=pStatusPlay->lTurnMaxScore;
			m_lTurnLessScore=pStatusPlay->lTurnLessScore;
			CopyMemory(m_lTableScore,pStatusPlay->lTableScore,sizeof(m_lTableScore));

			//状态变量
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_bAddScore=(pStatusPlay->bAddScore==TRUE)?true:false;
			m_bShowHand=(pStatusPlay->bShowHand==TRUE)?true:false;
			CopyMemory(m_cbPlayStatus,pStatusPlay->cbPlayStatus,sizeof(m_cbPlayStatus));

			//帐号名字
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				const tagUserData * pUserData=GetUserData(i);
				if (pUserData!=NULL) lstrcpyn(m_szAccounts[i],pUserData->szName,CountArray(m_szAccounts[i]));
			}

			//设置界面
			LONG lTableScore=0L;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//设置位置
				WORD wViewChairID=SwitchViewChairID(i);

				//设置扑克
				if (m_cbPlayStatus[i]==TRUE) 
				{
					BYTE cbCardCount=pStatusPlay->cbCardCount[i];
					m_GameClientView.m_CardControl[wViewChairID].SetCardData(pStatusPlay->cbHandCardData[i],cbCardCount);
				}
				lTableScore += m_lTableScore[2*i+1];
				//筹码设置
				m_GameClientView.m_PlayerJeton[wViewChairID].SetScore(m_lTableScore[2*i]);
				//设置下注
				m_GameClientView.SetUserTableScore(wViewChairID,m_lTableScore[2*i]+m_lTableScore[2*i+1]);
			}
			m_GameClientView.m_PlayerJeton[GAME_PLAYER].SetScore(lTableScore);

			//
			m_GameClientView.SetCellScore(m_lCellScore);

			//玩家设置
			if (IsLookonMode()==false) 
			{
				//控制设置
				m_GameClientView.m_CardControl[2].SetPositively(true);
				if (m_wCurrentUser==GetMeChairID()) UpdateScoreControl();
			}

			//设置定时器
			SetGameTimer(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

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

	//设置状态
	SetGameStatus(GS_PLAYING);

	//下注变量
	m_lMaxScore=pGameStart->lMaxScore;
	m_lCellScore=pGameStart->lCellScore;
	m_lTurnMaxScore=pGameStart->lTurnMaxScore;
	m_lTurnLessScore=pGameStart->lTurnLessScore;

	//状态变量
	m_bAddScore=false;
	m_wCurrentUser=pGameStart->wCurrentUser;

	//设置变量
	LONG lTableScore=0L;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取用户
		const tagUserData * pUserData=GetUserData(i);
		if (pUserData==NULL) continue;

		//游戏信息
		m_cbPlayStatus[i]=TRUE;
		lTableScore+=m_lCellScore;
		m_lTableScore[2*i+1]=m_lCellScore;

		//用户名字
		lstrcpyn(m_szAccounts[i],pUserData->szName,CountArray(m_szAccounts[i]));
	}

	//设置界面
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L);
	m_GameClientView.SetCellScore(m_lCellScore);

	//设置界面
	lTableScore = 0L;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wViewChairID=SwitchViewChairID(i);

		//设置扑克
		m_GameClientView.m_CardControl[wViewChairID].SetCardData(NULL,0); 
		m_GameClientView.m_CardControl[wViewChairID].SetDisplayHead(false);
		lTableScore += m_lTableScore[2*i+1];
		//设置筹码
		m_GameClientView.m_PlayerJeton[wViewChairID].SetScore(m_lTableScore[2*i]);
		m_GameClientView.SetUserTableScore(wViewChairID,m_lTableScore[i*2]+m_lTableScore[i*2+1]);
	}
	m_GameClientView.m_PlayerJeton[GAME_PLAYER].SetScore(lTableScore);

	//派发扑克
	for (BYTE cbIndex=0;cbIndex<2;cbIndex++)
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (m_cbPlayStatus[i]==TRUE)
			{
				//变量定义
				WORD wViewChairID=SwitchViewChairID(i);
				BYTE cbCardData[2]={0,pGameStart->cbCardData[i]};

				//派发扑克
				cbCardData[0]=(GetMeChairID()==i)?pGameStart->cbObscureCard:0;
				m_GameClientView.DispatchUserCard(wViewChairID,cbCardData[cbIndex]);
			}
		}
	}

	//玩家设置
	if (IsLookonMode()==false) 
	{
		//控制设置
		m_GameClientView.m_CardControl[2].SetPositively(true);
	}

	//环境设置
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//激活框架
	if (IsLookonMode()==false) ActiveGameFrame();

//	//获取胜者
//#ifdef SUPPER_VER
//	CMD_C_GetWinner GetWinner;
//	GetWinner.dwUserID=GetUserData(GetMeChairID())->dwUserID;
//	SendData(SUB_C_GET_WINNER,&GetWinner,sizeof(GetWinner));
//#endif

	return true;
}

//用户加注
bool CGameClientDlg::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wAddScoreUser=pAddScore->wAddScoreUser;
	WORD wViewChairID=SwitchViewChairID(wAddScoreUser);

	//梭哈判断
	if (m_bShowHand==false)
	{
		//获取用户
		const tagUserData * pUserData=GetUserData(wAddScoreUser);
		if (pUserData!=NULL) m_bShowHand=(pUserData->lScore==(m_lTableScore[wAddScoreUser*2+1]+pAddScore->lAddScoreCount));
	}

	//加注处理
	if ((IsLookonMode()==true)||(pAddScore->wAddScoreUser!=wMeChairID))
	{
		//加注界面
		m_GameClientView.m_PlayerJeton[wViewChairID].SetScore(pAddScore->lAddScoreCount);

		//播放声音
		if (m_cbPlayStatus[wAddScoreUser]==TRUE)
		{
			//获取积分
			const tagUserData * pUserData=(tagUserData *)GetUserData(wAddScoreUser);
			LONG lTableScore=m_lTableScore[wAddScoreUser*2+1];
			LONG lTurnAddScore=m_lTableScore[wAddScoreUser*2];
			LONG lShowHandScore=pUserData->lScore-lTableScore;

			//播放声音
			if ((pAddScore->lAddScoreCount-lTurnAddScore)==0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("NO_ADD"));
			else if (pAddScore->lAddScoreCount==lShowHandScore) PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
			else if ((pAddScore->lAddScoreCount+lTableScore)==m_lTurnLessScore) PlayGameSound(AfxGetInstanceHandle(),TEXT("FOLLOW"));
			else PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
		}
	}

	//设置变量
	m_wCurrentUser=pAddScore->wCurrentUser;
	m_lTurnLessScore=pAddScore->lTurnLessScore;
	m_lTableScore[wAddScoreUser*2]=pAddScore->lAddScoreCount;

	//设置筹码
	m_GameClientView.SetUserTableScore(wViewChairID,m_lTableScore[wAddScoreUser*2+1]+m_lTableScore[wAddScoreUser*2]);
	m_GameClientView.UpdateGameView(NULL);

	//控制界面
	if ((IsLookonMode()==false)&&(pAddScore->wCurrentUser==wMeChairID))
	{
		ActiveGameFrame();
		UpdateScoreControl();
	}

	//设置时间
	if (m_wCurrentUser==INVALID_CHAIR) KillGameTimer(IDI_USER_ADD_SCORE);
	else SetGameTimer(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	return true;
}

//用户放弃
bool CGameClientDlg::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	//设置变量
	m_cbPlayStatus[pGiveUp->wGiveUpUser]=FALSE;

	//变量定义
	WORD wGiveUpUser=pGiveUp->wGiveUpUser;
	BYTE cbCardData[MAX_COUNT]={0,0,0,0,0};
	WORD wViewChairID=SwitchViewChairID(wGiveUpUser);
	BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[wViewChairID].GetCardCount();

	//设置扑克
	m_GameClientView.m_CardControl[wViewChairID].SetPositively(false);
	m_GameClientView.m_CardControl[wViewChairID].SetDisplayHead(false);
	m_GameClientView.m_CardControl[wViewChairID].SetCardData(cbCardData,cbCardCount);

	//状态设置
	if ((IsLookonMode()==false)&&(pGiveUp->wGiveUpUser==GetMeChairID())) 
	{
		HideScoreControl();
		SetGameStatus(GS_FREE);
	}

	//环境设置
	if (wGiveUpUser==GetTimeChairID()) KillGameTimer(IDI_USER_ADD_SCORE);
	if ((IsLookonMode()==true)||(wGiveUpUser!=GetMeChairID())) PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	return true;
}

//发牌消息
bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//设置变量
	m_bAddScore=false;
	m_lTurnLessScore=0L;
	m_wCurrentUser=pSendCard->wCurrentUser;
	m_lTurnMaxScore=pSendCard->lTurnMaxScore;
	m_lTurnLessScore=pSendCard->lTurnLessScore;

	//设置筹码
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_PlayerJeton[wViewChairID].SetScore(0L);
		m_GameClientView.m_PlayerJeton[GAME_PLAYER].AddScore(m_lTableScore[i*2]);
	}

	//累计金币
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_lTableScore[i*2+1]+=m_lTableScore[i*2];
		m_lTableScore[i*2]=0L;
	}

	//派发扑克,从上次最大玩家开始发起
	WORD wLastMostUser = pSendCard->wLastMostUser;
	ASSERT( wLastMostUser != INVALID_CHAIR );
	for (BYTE i=0;i<pSendCard->cbSendCardCount;i++)
	{
		for (WORD j=0;j<GAME_PLAYER;j++)
		{
			WORD wChairId = (wLastMostUser+j)%GAME_PLAYER;
			if (m_cbPlayStatus[wChairId]==TRUE)
			{
				WORD wViewChairID=SwitchViewChairID(wChairId);
				m_GameClientView.DispatchUserCard(wViewChairID,pSendCard->cbCardData[wChairId][i]);
			}
		}
	}

	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//状态设置
	HideScoreControl();
	SetGameStatus(GS_FREE);
	KillGameTimer(IDI_USER_ADD_SCORE);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L);

	//播放声音
	if (IsLookonMode()==false)
	{
		if (pGameEnd->lGameScore[GetMeChairID()]>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//正常结束
	m_GameClientView.m_ScoreView.ResetScore();
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置信息
		if (pGameEnd->lGameScore[i]!=0L)
		{
			if (m_szAccounts[i][0]==0) m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("已离开"),pGameEnd->lGameScore[i]);
			else m_GameClientView.m_ScoreView.SetGameScore(i,m_szAccounts[i],pGameEnd->lGameScore[i]);
			//扣税
			m_GameClientView.m_ScoreView.SetGameTax(i,pGameEnd->lGameTax[i]);
			if (pGameEnd->cbCardData[i]!=0)
			{
				//获取扑克
				BYTE cbCardData[5];
				WORD wViewChairID=SwitchViewChairID(i);
				BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[wViewChairID].GetCardData(cbCardData,CountArray(cbCardData));

				//设置扑克
				cbCardData[0]=pGameEnd->cbCardData[i];
				m_GameClientView.m_CardControl[wViewChairID].SetDisplayHead(true);
				m_GameClientView.m_CardControl[wViewChairID].SetCardData(cbCardData,cbCardCount);
			}
		}
	}
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置积分
	TCHAR szBuffer[128] = TEXT("");
	InsertGeneralString(TEXT("\n本局结束,成绩统计:"),RGB(255,0,255),true);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if( pGameEnd->lGameScore[i] != 0L && m_szAccounts[i][0]!=0 )
		{
			//在聊天框显示成绩
			_snprintf(szBuffer,CountArray(szBuffer),TEXT(" %s: %+ld分"),m_szAccounts[i],pGameEnd->lGameScore[i]);
			InsertGeneralString(szBuffer,RGB(255,0,255),true);
		}
	}

	//加注信息
	m_lMaxScore=0L;
	m_lCellScore=0L;
	m_lTurnMaxScore=0L;
	m_lTurnLessScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//状态变量
	m_bAddScore=false;
	m_bShowHand=false;
	m_wCurrentUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));


	//开始按钮
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}

	return true;
}

//隐藏控制
void CGameClientDlg::HideScoreControl()
{
	//控制按钮
	m_GameClientView.m_btAddScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoAdd.ShowWindow(SW_HIDE);
	m_GameClientView.m_btFollow.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUp.ShowWindow(SW_HIDE);
	m_GameClientView.m_btShowHand.ShowWindow(SW_HIDE);
	m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);
	return;
}

//更新控制
void CGameClientDlg::UpdateScoreControl()
{
	//变量定义
	WORD wMeChairID=GetMeChairID();
	BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[2].GetCardCount();
	LONG lDrawAddScore=m_lTableScore[wMeChairID*2]+m_lTableScore[wMeChairID*2+1];

	//梭哈金币
	const tagUserData * pUserData=GetUserData(wMeChairID);
	LONG lMaxLeaveScore=pUserData->lScore-lDrawAddScore;
	LONG lUserLessScore=__min(lMaxLeaveScore,__max(m_lTurnLessScore-lDrawAddScore,0L));

	//下注按钮
	LONG lLeaveScore=__max(m_lTurnMaxScore-lDrawAddScore,0L);
	m_GameClientView.m_btAddScore.ShowWindow(SW_SHOW);
	m_GameClientView.m_btAddScore.EnableWindow(((m_bShowHand==false)&&(m_bAddScore==false)&&(lLeaveScore>0))?TRUE:FALSE);

	//显示按钮
	if( lUserLessScore == 0L )
		m_GameClientView.m_btNoAdd.ShowWindow(SW_SHOW);
	else
		m_GameClientView.m_btFollow.ShowWindow(SW_SHOW);
	m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
	m_GameClientView.m_btShowHand.ShowWindow(SW_SHOW);

	//控制按钮
	m_GameClientView.m_btNoAdd.EnableWindow((lUserLessScore==0L)?TRUE:FALSE);
	m_GameClientView.m_btShowHand.EnableWindow((lMaxLeaveScore>0L)&&(cbCardCount>=3)?TRUE:FALSE);
	m_GameClientView.m_btFollow.EnableWindow(((m_bShowHand==false)&&(lUserLessScore>0L))?TRUE:FALSE);

	return;
}

//开始按钮
LRESULT	CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//删除定时器
	KillGameTimer(IDI_START_GAME);

	//设置控件
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_PlayerJeton[i].SetScore(0L);
	}
	m_GameClientView.m_PlayerJeton[GAME_PLAYER].SetScore(0L);

	//设置界面
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L);

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//放弃按钮
LRESULT CGameClientDlg::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	//放弃设置
	m_bAddScore=true;
	HideScoreControl();

	//界面设置
	KillGameTimer(IDI_USER_ADD_SCORE);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	//发送消息
	SendData(SUB_C_GIVE_UP);

	return 0;
}

//跟注按钮
LRESULT CGameClientDlg::OnFollow(WPARAM wParam, LPARAM lParam)
{
	//获取筹码
	WORD wMeChairID=GetMeChairID();
	LONG lCurrentScore=m_lTurnLessScore-m_lTableScore[wMeChairID*2+1];

	//加注设置
	m_bAddScore=true;
	HideScoreControl();
	m_GameClientView.m_PlayerJeton[2].SetScore(lCurrentScore);

	//界面设置
	KillGameTimer(IDI_USER_ADD_SCORE);
	PlayGameSound(AfxGetInstanceHandle(),0L==lCurrentScore?TEXT("NO_ADD"):TEXT("FOLLOW"));

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=lCurrentScore;
	SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//加注按钮 
LRESULT	CGameClientDlg::OnAddScore(WPARAM wParam, LPARAM lParam)
{
	//取消按钮
	if( lParam == -1 )
	{
		UpdateScoreControl();
		return 0;
	}
	else if( lParam == 0 )
	{
		//不加或跟注
		return OnFollow(0,0);
	}

	//获取筹码
	WORD wMeChairID=GetMeChairID();
	LONG lCurrentScore=m_lTurnLessScore-m_lTableScore[wMeChairID*2+1]+m_lCellScore*(lParam);

	//加注设置
	m_bAddScore=true;
	HideScoreControl();
	m_GameClientView.m_PlayerJeton[2].SetScore(lCurrentScore);

	//界面设置
	KillGameTimer(IDI_USER_ADD_SCORE);
	m_GameClientView.UpdateGameView(NULL);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=lCurrentScore;
	SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//梭哈按钮
LRESULT CGameClientDlg::OnShowHand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	WORD wMeChairID=GetMeChairID();
	const tagUserData * pUserData=NULL;

	//获取用户
	if (wMeChairID!=INVALID_CHAIR)
	{
		pUserData=GetUserData(GetMeChairID());
		if (pUserData==NULL) return 0;
	}

	//验证
	BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[2].GetCardCount();
	if( cbCardCount < 3 || m_wCurrentUser!=wMeChairID ) return 0;

	//金币统计
	LONG lTurnAddScore=m_lTableScore[wMeChairID*2];
	LONG lCurrentScore=pUserData->lScore-m_lTableScore[wMeChairID*2+1];
	LONG lDrawAddScore=m_lTableScore[wMeChairID*2]+m_lTableScore[wMeChairID*2+1];

	//加注设置
	m_bAddScore=true;
	HideScoreControl();
	m_GameClientView.m_PlayerJeton[2].SetScore(lCurrentScore);

	//界面设置
	KillGameTimer(TIME_USER_ADD_SCORE);
	m_GameClientView.UpdateGameView(NULL);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));

	//发送消息
	CMD_C_AddScore AddScore;
	AddScore.lScore=lCurrentScore;
	SendData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//发牌完成
LRESULT CGameClientDlg::OnSendCardFinish(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_wCurrentUser==INVALID_CHAIR) return 0;

	//加注状态
	if ((IsLookonMode()==false)&&(GetMeChairID()==m_wCurrentUser))
	{
		//界面设置
		ActiveGameFrame();
		UpdateScoreControl();
	}

	//设置时间
	SetGameTimer(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
