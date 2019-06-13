#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "..\消息定义\CMD_QuickRun.h"
#include ".\gameclientdlg.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_MESSAGE(WM_OUTPISA,OnCmdOutPisa)
	ON_MESSAGE(WM_USERREADY,OnCmdUserReady)
	ON_MESSAGE(IDM_TIMER_OUT,OnCmdTimerOut)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
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
#ifdef _TWICE
	SetWindowText(TEXT("跑得快游戏（两副牌）"));
#else
	SetWindowText(TEXT("跑得快游戏"));
#endif

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置 配置控件
	m_GameClientView.SetUserOrder(AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),TRUE)?true:false);
	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	TRACE("CGameClientDlg::ResetGameFrame");
	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bDeasilOrder=m_GameClientView.IsDeasilOrder();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置控件
		EnableSound(GameOption.m_bEnableSound);
		m_GameClientView.SetUserOrder(GameOption.m_bDeasilOrder);

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),GameOption.m_bDeasilOrder?TRUE:FALSE);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	return true;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	TRACE("CGameClientDlg::OnLookonChanged");
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	TRACE("CGameClientDlg::OnGameMessage:SubCmdID:%d",wSubCmdID);
	switch(wSubCmdID)
	{
		case MSG_SET_INITPISA:
		{
			//初始化
			struct tagSetInitPisa *pInit=(struct tagSetInitPisa *)pBuffer;
			m_GameClientView.m_nOwnerIndex=(pInit->nOwnerIndex+2)%GAME_PLAYER;//在桌面上的相对偏移
			TRACE("收到初始化的索引：%d,计算后的索引：%d,我的椅子id：%d\n",pInit->nOwnerIndex,m_GameClientView.m_nOwnerIndex,GetMeChairID());
			//m_GameClientView.m_nFocusIndex=0;
			for(int i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_nPlayerLeftCount[i]=PISACOUNT;
				m_GameClientView.m_OutPisaArray[i].RemoveAll();
				m_GameClientView.m_bOutPass[i]=FALSE;
			}
			m_GameClientView.m_PisaArray.RemoveAll();
			for(i=0;i<PISACOUNT;i++)
			{
				m_GameClientView.m_PisaArray.Add(pInit->tPisa[i]);
				m_GameClientView.m_bOutSelect[i]=FALSE;
			}
			m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
			m_GameClientView.m_bStart=TRUE;
		}
			break;
		case MSG_SET_FOCUS:
		{
			//设置当前谁出牌
			struct tagSetFocus *pFocus=(struct tagSetFocus *)pBuffer;
			m_GameClientView.m_nFocusIndex=(pFocus->nFoucsId+GAME_PLAYER-m_GameClientView.m_nOwnerIndex)%GAME_PLAYER;
			//如果设置焦点不是自己，那么就说明不是自己先出牌
			if (m_GameClientView.m_nFocusIndex!=2)
				m_GameClientView.m_bStart=FALSE;
			TRACE("收到设置焦点的索引：%d,计算后的索引：%d\n",pFocus->nFoucsId,m_GameClientView.m_nFocusIndex);
			if (pFocus->bClean)
			{
				for(int i=0;i<GAME_PLAYER;i++)
				{
					m_GameClientView.m_OutPisaArray[i].RemoveAll();
					m_GameClientView.m_bOutPass[i]=FALSE;
				}
			}
			m_GameClientView.SetTimerUp(TRUE);
		}
			break;
		case MSG_SET_OTHERPISA:
		{
			//设置其它玩家出了什么牌
			struct tagSetOtherPisa *pOther=(struct tagSetOtherPisa *)pBuffer;
			int nLocalIndex=(pOther->nChairId+GAME_PLAYER-m_GameClientView.m_nOwnerIndex)%GAME_PLAYER;
			TRACE("收到设置玩家出牌的索引：%d,计算后的索引：%d\n",pOther->nChairId,nLocalIndex);
			m_GameClientView.m_nPlayerLeftCount[nLocalIndex]-=pOther->nCount;
			m_GameClientView.m_OutPisaArray[nLocalIndex].RemoveAll();
			if (pOther->nCount==0)
			{
				m_GameClientView.m_bOutPass[nLocalIndex]=TRUE;
			}
			else
			{
				m_GameClientView.m_bOutPass[nLocalIndex]=FALSE;
				for(int i=0;i<pOther->nCount;i++)
					m_GameClientView.m_OutPisaArray[nLocalIndex].Add(pOther->tOutPisa[i]);
			}
		}
			break;
		case MSG_OUTPISA_MSG:
		{
			//玩家出的牌经过服务器检查返回的结果
			struct tagOutPisaMsg *pOutPisa=(struct tagOutPisaMsg *)pBuffer;
			if (pOutPisa->nResult==0)
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
				//delete all select out pisa
				for(int i=PISACOUNT-1;i>=0;i--)
				{
					if (m_GameClientView.m_bOutSelect[i])
					{
						m_GameClientView.m_PisaArray.RemoveAt(i);
						m_GameClientView.m_bOutSelect[i]=FALSE;
					}
				}
			}
			else
			{
				m_GameClientView.SetTimerPause(false);
				MessageBox(pOutPisa->chErrMsg,"不能这样出牌");
			}
		}
			break;
		case MSG_GAME_END:
		{
			struct tagGameEnd *pGameEnd=(struct tagGameEnd *)pBuffer;
			//设置积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				const tagUserData * pUserData=GetUserData(i);
				m_GameClientView.m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lGameScore[i]);
			}
			m_GameClientView.m_nFocusIndex=-1;
			m_GameClientView.m_bReady=FALSE;
			m_GameClientView.m_ScoreView.SetGameTax(pGameEnd->lGameTax);
			m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

			//播放声音
			WORD wMeChairID=GetMeChairID();
			LONG lMeScore=pGameEnd->lGameScore[GetMeChairID()];
			if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
			else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
			else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

		}
			break;
	}
	m_GameClientView.UpdateGameView(NULL);
	return true;
	return false;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	TRACE("CGameClientDlg::OnGameMessage:GameStation:%d,Lookon:%d",cbGameStation,bLookonOther);
	return true;
	return false;
}

LRESULT CGameClientDlg::OnCmdOutPisa(WPARAM wParam,LPARAM lParam)
{
	struct tagSetOutPisa *pOut=(struct tagSetOutPisa *)wParam;
	TRACE("我的椅子id:%d，id:%d\n",this->GetMeChairID(),pOut->nChairId);
	m_GameClientView.SetTimerPause(true);
	SendData(MSG_SET_OUTPISA,pOut,sizeof(struct tagSetOutPisa));
	return 0;
}

LRESULT CGameClientDlg::OnCmdUserReady(WPARAM wParam,LPARAM lParam)
{
	SendUserReady(NULL,0);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("Ready"));
	return 0;
}

LRESULT CGameClientDlg::OnCmdTimerOut(WPARAM wParam,LPARAM lParam)
{
	struct tagSetOutPisa out;
	memset(&out,0,sizeof(tagSetOutPisa));
	if (m_GameClientView.m_bStart)
	{
		//如果是刚开始的出牌，需要把黑桃3发出去
		int index=m_GameClientView.SetHeiTao3Out();
		//获取黑桃3错误
		if (index==-1) return 0;
		out.tOutPisa[0].line=4;
		out.tOutPisa[0].row=3;
		out.nCount=1;
	}
	SendData(MSG_SET_OUTPISA,&out,sizeof(struct tagSetOutPisa));
	return 0;
}
