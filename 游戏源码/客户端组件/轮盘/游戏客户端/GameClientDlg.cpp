#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏定时器
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_START_ROLL					300								//开始定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_CHIP_IN,OnChipIn)
	ON_MESSAGE(IDM_WIN_NUMBER,OnWinNumber)
	ON_MESSAGE(IDM_GET_STOP,OnGetStop)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//开始下注
LRESULT	CGameClientDlg::OnChipIn(WPARAM wParam, LPARAM lParam)
{
	//获取信息
	CopyMemory(m_wChipsInfo,(void *)wParam,sizeof(m_wChipsInfo));

	//构造数据
	CMD_C_ChipIn ChipIn;
	CopyMemory(ChipIn.ChipsInfo,m_wChipsInfo,sizeof(m_wChipsInfo));

	//发送数据
	SendData(SUB_C_CHIP_IN,&ChipIn,sizeof(CMD_C_ChipIn));
	return 0;
}

//胜利数字
LRESULT	CGameClientDlg::OnWinNumber(WPARAM wParam, LPARAM lParam)
{
	//获取信息
	m_bWinNumber=(BYTE)wParam;

	//构造数据
	CMD_C_RollEnd RollEnd;
	RollEnd.WinNumber=m_bWinNumber;

	//发送数据
	SendData(SUB_C_ROLL_END,&RollEnd,sizeof(CMD_C_RollEnd));
	return 0;
}

//得到停止消息
LRESULT	CGameClientDlg::OnGetStop(WPARAM wParam, LPARAM lParam)
{
	//发送数据
	//SendData(SUB_C_GET_STOP);
	return 0;
}

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_bWinNumber=255;
	ZeroMemory(m_wChipsInfo,sizeof(m_wChipsInfo));
	//memset(m_bHistoryInfo,0xff,sizeof(m_bHistoryInfo));

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
	SetWindowText(TEXT("轮盘游戏"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);
	INT nCom = m_btControl.GetDlgCtrlID();
	PostMessage(WM_COMMAND,nCom,0);
	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{

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
		//设置控件
		EnableSound(GameOption.m_bEnableSound);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch(nTimerID)
	{
	case IDI_START_ROLL:
		{
			m_GameClientView.m_DxDialog.m_bTimeLeave=nElapse;

			//超时判断
			if (nElapse<=0)
			{
				KillGameTimer(IDI_START_ROLL);
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
	//旁观设置

	return;
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//用户叫分
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_USER_CHIP:
		{
			return OnSubUserChip(pBuffer,wDataSize);
		}
	case SUB_S_STOP_ROLL:
		{
			return OnSubStopRoll(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:
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

			//单元信息
			m_lCellScore = pStatusFree->lCellScore;

			//const tagUserData * pUserData=GetUserData(GetMeChairID());
			//long lMaxScore=(long)__max(pUserData->lScore,0);
			//lMaxScore=(long)__min(lMaxScore,m_lCellScore*700);
			long lMaxWinScore=m_lCellScore*720;
			m_GameClientView.SetCellScore(m_lCellScore);
			m_GameClientView.SetUserCome(pStatusFree->bHistoryInfo,m_lCellScore,lMaxWinScore,pStatusFree->lUserScore,bLookonOther);

			//设置控件
			if (IsLookonMode()==false)
			{
				//下注筹码
				SetGameTimer(GetMeChairID(),IDI_START_ROLL,pStatusFree->cbTimeLeave);
			}

			return true;
		}
	}

	return true;
}


//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	m_GameClientView.SetGameStart(pGameStart->bWinNumber);

	//设置时间

	return true;
}

//用户下注
bool CGameClientDlg::OnSubUserChip(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_UserChipIn));
	if (wDataSize!=sizeof(CMD_S_UserChipIn)) return false;

	//消息处理
	CMD_S_UserChipIn * pUserChip=(CMD_S_UserChipIn *)pBuffer;

	m_GameClientView.SetUserChip(pUserChip->lAllChips);

	return true;
}
//停止旋转
bool CGameClientDlg::OnSubStopRoll(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_StopRoll));
	if (wDataSize!=sizeof(CMD_S_StopRoll)) return false;

	//消息处理
	CMD_S_StopRoll * pStopRoll=(CMD_S_StopRoll *)pBuffer;

	m_GameClientView.SetRollStop(pStopRoll->bWinNumber);

	SendData(SUB_C_GET_STOP);
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
	m_GameClientView.SetGameEnd(pGameEnd->lGameScore[GetMeChairID()]);
	m_GameClientView.SetUserCome(pGameEnd->bHistoryInfo,m_lCellScore,pGameEnd->lMaxBetWinScore[GetMeChairID()],pGameEnd->lMaxUserScore[GetMeChairID()],IsLookonMode());

	SetGameTimer(GetMeChairID(),IDI_START_ROLL,30);
	return true;
}


//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}


//////////////////////////////////////////////////////////////////////////

